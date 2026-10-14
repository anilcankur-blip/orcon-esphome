import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_MAX_VALUE, CONF_MIN_VALUE, CONF_STEP

# Explicitly load ESPHome core number support for this external number platform.
AUTO_LOAD = ["number"]

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconTimerNumber = orcon_ns.class_("OrconTimerNumber", number.Number, cg.Component)

CONFIG_SCHEMA = number.number_schema(OrconTimerNumber).extend(
    {
        cv.Required(CONF_MIN_VALUE): cv.float_,
        cv.Required(CONF_MAX_VALUE): cv.float_,
        cv.Optional(CONF_STEP, default=1): cv.float_,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await number.new_number(config, min_value=config[CONF_MIN_VALUE],
                                  max_value=config[CONF_MAX_VALUE], step=config[CONF_STEP])
    await cg.register_component(var, config)
