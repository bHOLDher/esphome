from esphome.components import sensor, remote_base

DEPENDENCIES = ["remote_receiver"]

CONFIG_SCHEMA = remote_base.validate_sensor


async def to_code(config):
    var = await remote_base.build_sensor(config)
    await sensor.register_sensor(var, config)
