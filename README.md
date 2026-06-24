# BareBrain Text To Speech Tool

BareBrain 的外部文本转语音插件骨架。插件注册 `tts_speak` 工具，将文本交给后续配置的 TTS 服务和音频输出设备。

## 当前状态

- 已完成 BareBrain Mod manifest。
- 已完成工具注册和输入校验。
- 尚未确定 TTS 提供方、音频功放和 GPIO。
- 在硬件配置完成前，工具会返回明确的未配置错误。

## 硬件接口

照片中的 Hiwonder 语音模块接口为：

```text
5V GND SDA SCL
```

- `5V` 接 ESP32-S3 开发板的 `5V`
- `GND` 接 `GND`
- `SDA`、`SCL` 在 Manager 引脚配置页自行选择

该模块使用 I2C 控制，不使用之前示例中的 I2S 三线接口。插件不提供默认 GPIO。

## 接入 BareBrain

云端构建下载 Release 后，将插件解压到：

```text
BareBrain/main/external_mods/tool-tts/
```

固件 Profile 中启用：

```json
{
  "enabled_mods": ["tool-tts"]
}
```
