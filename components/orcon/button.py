import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconPairButton = orcon_ns.class_("OrconPairButton", button.Button, cg.Component)

CONFIG_SCHEMA = button.button_schema(OrconPairButton).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)
    cg.add_build_flag("-DRADIO_CC1101_ASYNC=1")
    cg.add_build_flag("-DBOARD_GENERIC_ESPHOME=1")
