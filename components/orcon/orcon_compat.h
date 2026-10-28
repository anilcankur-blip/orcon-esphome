#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <esphome/core/log.h>
#include <driver/uart.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef SERIAL_8N1
#define SERIAL_8N1 0
#endif

inline uint32_t millis() { return (uint32_t)(esp_timer_get_time() / 1000ULL); }
inline void delay(uint32_t ms) { if (ms) vTaskDelay(pdMS_TO_TICKS(ms)); }
inline void delayMicroseconds(uint32_t us) { esp_rom_delay_us(us); }

class OrconLogSerial {
 public:
  template <typename... Args>
  int printf(const char *format, Args... args) {
    char chunk[768];
    int written = snprintf(chunk, sizeof(chunk), format, args...);
    if (written <= 0) return written;
    const size_t count = (size_t)written < sizeof(chunk) ? (size_t)written : sizeof(chunk) - 1;
    for (size_t i = 0; i < count; ++i) {
      if (chunk[i] == '\n') {
        flushLine();
      } else if (length_ < sizeof(line_) - 1) {
        line_[length_++] = chunk[i];
        line_[length_] = '\0';
      }
    }
    return written;
  }

  void println() { flushLine(); }

 private:
  void flushLine() {
    if (length_ > 0) {
      line_[length_] = '\0';
      ESP_LOGI("orcon_radio", "%s", line_);
      length_ = 0;
      line_[0] = '\0';
    }
  }

  char line_[768] = {};
  size_t length_ = 0;
};
static OrconLogSerial OrconSerial;

class OrconUart {
 public:
  explicit OrconUart(int port) : port_((uart_port_t)port) {}
  size_t setRxBufferSize(size_t size) { rx_buffer_size_ = size; return size; }

  void begin(unsigned long baud, int, int rx_pin, int tx_pin) {
    uart_config_t cfg = {};
    cfg.baud_rate = (int)baud;
    cfg.data_bits = UART_DATA_8_BITS;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    cfg.rx_flow_ctrl_thresh = 0;
    esp_err_t e = uart_driver_install(port_, (int)(rx_buffer_size_ ? rx_buffer_size_ : 2048), 0, 0, nullptr, 0);
    if (e != ESP_OK && e != ESP_ERR_INVALID_STATE) { ::printf("[orcon] uart_driver_install failed: %d\n", (int)e); return; }
    e = uart_param_config(port_, &cfg);
    if (e != ESP_OK) { ::printf("[orcon] uart_param_config failed: %d\n", (int)e); return; }
    e = uart_set_pin(port_, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (e != ESP_OK) { ::printf("[orcon] uart_set_pin failed: %d\n", (int)e); return; }
    (void)uart_flush_input(port_);
    ready_ = true;
  }

  int available() {
    size_t count = 0;
    if (!ready_ || uart_get_buffered_data_len(port_, &count) != ESP_OK) return 0;
    return (int)count;
  }
  int read() {
    uint8_t byte = 0;
    if (!ready_ || uart_read_bytes(port_, &byte, 1, 0) != 1) return -1;
    return (int)byte;
  }
  size_t write(uint8_t byte) {
    if (!ready_) return 0;
    return uart_write_bytes(port_, (const char *)&byte, 1) == 1 ? 1 : 0;
  }
  size_t write(const uint8_t *data, size_t size) {
    if (!ready_ || !data || !size) return 0;
    int n = uart_write_bytes(port_, (const char *)data, size);
    return n > 0 ? (size_t)n : 0;
  }
  void flush() { if (ready_) (void)uart_wait_tx_done(port_, pdMS_TO_TICKS(1000)); }

 private:
  uart_port_t port_;
  size_t rx_buffer_size_ = 2048;
  bool ready_ = false;
};
