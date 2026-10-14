import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_NAME
from esphome.components import button

orcon_ns = cg.esphome_ns.namespace("orcon_esphome")
OrconPairButton = orcon_ns.class_("OrconPairButton", button.Button, cg.Component)

CONFIG_SCHEMA = button.button_schema(OrconPairButton).extend(cv.COMPONENT_SCHEMA)

# Button names select the callback during code generation. No YAML mode/action
# parameter is required, and C++ does not inspect entity names at runtime.
COMMANDS = {
    "Orcon Away": "modeAway",
    "Orcon 1": "mode1",
    "Orcon 2": "mode2",
    "Orcon 3": "mode3",
    "Orcon Auto": "modeAuto",
    "Orcon Boost": "modeBoost",
    "Orcon Eşleştirmeyi Başlat": "startPairing",
    "Orcon Pairing": "startPairing",
}

async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)

    name = config[CONF_NAME]
    callback = COMMANDS.get(name)
    if callback is None:
        raise cv.Invalid(
            f"Unsupported Orcon button name '{name}'. "
            f"Supported names: {', '.join(COMMANDS)}"
        )

    cg.add(var.set_action(cg.RawExpression(f"&esphome::orcon_esphome::{callback}")))
    cg.add_build_flag("-DRADIO_CC1101_ASYNC=1")
    cg.add_build_flag("-DBOARD_GENERIC_ESPHOME=1")
