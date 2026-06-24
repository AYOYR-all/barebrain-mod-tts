# Changelog

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
