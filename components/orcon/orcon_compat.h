#pragma once
#include <stdint.h>
#include <stdio.h>
#include <driver/uart.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
inline uint32_t millis() { return (uint32_t)(esp_timer_get_time() / 1000ULL); }
inline void delay(uint32_t ms) { if (ms) vTaskDelay(pdMS_TO_TICKS(ms)); }
inline void delayMicroseconds(uint32_t us) { esp_rom_delay_us(us); }
