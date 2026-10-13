import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconFan = orcon_ns.class_("OrconFan", fan.Fan, cg.Component)

CONFIG_SCHEMA = fan.fan_schema(OrconFan).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await fan.new_fan(config)
    await cg.register_component(var, config)
    cg.add_build_flag("-DRADIO_CC1101_ASYNC=1")
    cg.add_build_flag("-DBOARD_GENERIC_ESPHOME=1")
    cg.add_library("jgromes/RadioLib", "7.1.0")
