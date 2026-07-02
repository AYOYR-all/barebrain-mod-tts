# BareBrain WonderEcho Voice Tool

`tool-tts` targets the Hiwonder WonderEcho CI1302 integrated AI voice
interaction module. The historic name is TTS, but this module does not
synthesize arbitrary text from the ESP32. It plays phrases that already exist
in the WonderEcho firmware, and it can expose the module's speech recognition
result byte to BareBrain.

## Hardware

Module pins:

```text
5V GND SDA SCL
```

- `5V` -> ESP32-S3 `5V`
- `GND` -> ESP32-S3 `GND`
- `SDA` / `SCL` -> configure in BareBrain Manager as `tts.i2c_sda` and
  `tts.i2c_scl`

The factory I2C address is `0x34`.

## WonderEcho Registers

From the vendor examples under:

```text
D:\BaiduNetdiskDownload\一体式AI语音交互模块（WonderEcho)
```

- recognition result register: `0x64`
- broadcast register: `0x6E`
- command phrase type: `0x00`
- announcement phrase type: `0xFF`

The vendor serial protocol list uses frames such as `AA 55 00 01 FB` and
`AA 55 FF 01 FB`. Over I2C, BareBrain writes only the middle two bytes
(`type`, `phrase_id`) to register `0x6E`.

## Chinese Firmware

The Chinese factory firmware is in the vendor package:

```text
D:\BaiduNetdiskDownload\一体式AI语音交互模块（WonderEcho)\2.软件工具&固件\2.出厂固件\2.1 CI1302_中文_单麦_V00729_UART1_115200_2M.bin
```

The flashing tool is in:

```text
D:\BaiduNetdiskDownload\一体式AI语音交互模块（WonderEcho)\2.软件工具&固件\1.烧录工具\ci-tool-kit.exe
```

Flash the Chinese firmware to the WonderEcho module with the vendor tool before
expecting the phrase IDs in the Chinese protocol spreadsheet to match.

## Tools

### `tts_status`

Probe the WonderEcho module at I2C address `0x34`.

```json
{}
```

### `tts_speak`

Play a preconfigured phrase from the WonderEcho firmware.

```json
{"type":"announcement","phrase_id":1}
```

`type` accepts:

- `command`: writes `0x00, phrase_id`
- `general`, `announcement`, or `announcer`: writes `0xFF, phrase_id`

### `tts_read_recognition`

Read one byte from recognition result register `0x64`.

```json
{}
```

The value corresponds to the command IDs in the firmware's protocol
spreadsheet.

## BareBrain Integration

Install or copy this plugin to:

```text
BareBrain/main/external_mods/tool-tts/
```

Enable it in the firmware profile:

```json
{
  "enabled_mods": ["tool-tts"]
}
```

The plugin registers its tools even if the module is not connected at boot.
Use `tts_status` after wiring or power cycling the module; once it reports
connected, `tts_speak` and `tts_read_recognition` can be called directly
through the BareBrain tool chain.
