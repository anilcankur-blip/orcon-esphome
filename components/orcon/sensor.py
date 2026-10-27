import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconHumiditySensor = orcon_ns.class_(
    "OrconHumiditySensor", sensor.Sensor, cg.Component
)

CONFIG_SCHEMA = sensor.sensor_schema(
    OrconHumiditySensor,
    unit_of_measurement="%",
    accuracy_decimals=0,
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
