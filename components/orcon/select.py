import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_OPTIONS

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconModeSelect = orcon_ns.class_("OrconModeSelect", select.Select, cg.Component)

ORCON_MODES = ["Away", "1", "2", "3", "Auto", "Boost"]

CONFIG_SCHEMA = select.select_schema(
    OrconModeSelect,
    entity_category=None,
).extend(cv.COMPONENT_SCHEMA).extend({
    cv.Required(CONF_OPTIONS): cv.All(
        cv.ensure_list(cv.one_of(*ORCON_MODES, lower=False)), cv.Length(min=1)
    ),
})

async def to_code(config):
    var = await select.new_select(config, options=config[CONF_OPTIONS])
    await cg.register_component(var, config)
    cg.add_build_flag("-DRADIO_CC1101_ASYNC=1")
    cg.add_build_flag("-DBOARD_GENERIC_ESPHOME=1")
