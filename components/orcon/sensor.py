import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ACCURACY_DECIMALS, CONF_UNIT_OF_MEASUREMENT

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconHumiditySensor = orcon_ns.class_(
    "OrconHumiditySensor", sensor.Sensor, cg.Component
)

CONF_SENSOR_TYPE = "type"
SENSOR_TYPES = {
    "humidity": (0, "%", 0),
    "exhaust_temperature": (1, "°C", 2),
    "supply_temperature": (2, "°C", 2),
    "indoor_temperature": (3, "°C", 2),
    "outdoor_temperature": (4, "°C", 2),
    "exhaust_fan_speed": (5, "%", 0),
    "supply_fan_speed": (6, "%", 0),
}


def validate_sensor_config(config):
    _, unit, accuracy = SENSOR_TYPES[config[CONF_SENSOR_TYPE]]
    config[CONF_UNIT_OF_MEASUREMENT] = unit
    config[CONF_ACCURACY_DECIMALS] = accuracy
    return config


CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(OrconHumiditySensor)
    .extend({
        cv.Optional(CONF_SENSOR_TYPE, default="humidity"): cv.enum(
            {key: key for key in SENSOR_TYPES}, lower=True
        ),
    })
    .extend(cv.COMPONENT_SCHEMA),
    validate_sensor_config,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    sensor_type, _, _ = SENSOR_TYPES[config[CONF_SENSOR_TYPE]]
    cg.add(var.set_sensor_type(sensor_type))
