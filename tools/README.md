# TRT Core Upload Workflow

Configure the constants at the top of `upload.py`, then run:

```powershell
python tools/upload.py
```

The tool leaves each source sketch under `TRT-arduino/` untouched. It stages an isolated PlatformIO project in `.trt-build/<project>/`, generates `trt-board.json`, `src/trt_board_profile.h`, and a wrapper, then invokes PlatformIO upload.

`BOARD` is passed directly to `python -m platformio project init --board`. PlatformIO resolves the platform, toolchain, and board definition, so the upload tool does not maintain a board allowlist. Invalid board IDs are reported by PlatformIO.

The generated project uses the Arduino framework because it wraps an Arduino `.ino` application. It can target Arduino-framework PlatformIO boards, including AVR, ESP32, and STM32 Arduino targets. A non-Arduino framework requires a different wrapper and runtime adapter.

The serial redirect is optional. Set `REDIRECT_APPLICATION_SERIAL_TO_SERIAL1` to `True` only for boards and applications where `Serial1` exists and application logs must be separate from the TRT transport. The upload tool does not validate this; the adapter/runtime owns serial capability decisions.

A `build_registry.csv` row is appended only after PlatformIO reports that both compilation and upload succeeded. Failed attempts reuse the next unconsumed build ID.

The generated wrapper preserves the application lifecycle by renaming its `setup()` and `loop()`. The current wrapper remains an integration scaffold until it stages TRT-Core and selects a board-specific transport/runtime adapter. The existing Mega adapter is not automatically selected by `upload.py` and must not be used for Uno, ESP32, STM32, or other board families.

Install PlatformIO before running the workflow:

```powershell
python -m pip install platformio
```