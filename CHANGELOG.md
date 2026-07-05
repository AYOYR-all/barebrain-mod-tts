# Changelog

## 0.1.6

- Add WonderEcho voice command mappings for Xi'an weather, new movie recommendations, and book recommendations.
- Publish the new recognition IDs in the plugin manifest voice command map.

## 0.1.5

- Route WonderEcho voice-command agent messages to the BareBrainAPP default chat id `barebrain_app`.

## 0.1.4

- Add a WonderEcho voice command listener that maps recognition IDs 0x01, 0x02, 0x08, and 0x09 into BareBrain agent messages.
- Route voice commands through the normal chat agent so weather, music, cron, and memory requests can use the existing tool chain.
- Debounce repeated recognition bytes until the module reports 0 again.

## 0.1.3

- Notify the optional OLED face feedback hook when `tts_speak` starts playback.

## 0.1.2

- Keep `tool-tts` registered even when WonderEcho is not connected at boot.
- Add `tts_status` for runtime I2C probing at address `0x34`.
- Add `tts_read_recognition` for reading register `0x64`.
- Keep `tts_speak` on register `0x6E` and accept announcement/general aliases.

## 0.1.1

- Add a real WonderEcho I2C driver at address `0x34`.
- Trigger command or general phrase playback through register `0x6E`.
- Read SDA and SCL assignments from the generated firmware Profile.
- Correct the tool contract: WonderEcho plays preconfigured phrase IDs.

## 0.1.0

- 创建 BareBrain 外部 Mod 结构。
- 注册 `tts_speak` 工具。
- 添加 JSON 输入校验。
- 为后续 TTS、音频设备和 GPIO 配置预留实现位置。
