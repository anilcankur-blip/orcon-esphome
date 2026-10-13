import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconPairButton = orcon_ns.class_("OrconPairButton", button.Button, cg.Component)

CONF_MODE = "mode"
ORCON_MODES = ["Away", "1", "2", "3", "Auto", "Boost"]

CONFIG_SCHEMA = button.button_schema(OrconPairButton).extend(cv.COMPONENT_SCHEMA).extend({
    cv.Optional(CONF_MODE): cv.one_of(*ORCON_MODES, lower=False),
})

async def to_code(config):
    var = await button.new_button(config)
    if CONF_MODE in config:
        cg.add(var.set_mode(config[CONF_MODE]))
    await cg.register_component(var, config)
