import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ACCURACY_DECIMALS, CONF_UNIT_OF_MEASUREMENT

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconHumiditySensor = orcon_ns.class_(
    "OrconHumiditySensor", sensor.Sensor, cg.Component
)


def validate_sensor_config(config):
    config[CONF_UNIT_OF_MEASUREMENT] = "%"
    config[CONF_ACCURACY_DECIMALS] = 0
    return config


CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(OrconHumiditySensor).extend(cv.COMPONENT_SCHEMA),
    validate_sensor_config,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
