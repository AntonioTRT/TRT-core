# TRT Core Upload Workflow

Configure the constants at the top of `upload.py`, then run:

```powershell
python tools/upload.py
```

The tool leaves each source sketch under `TRT-arduino/` untouched. It stages an isolated PlatformIO project in `.trt-build/<project>/`, generates `trt-board.json`, `src/trt_board_profile.h`, and a wrapper, then invokes PlatformIO upload.

`BOARD` is passed directly to the generated `.trt-build/<project>/platformio_wrapper.py` launcher, which runs `platformio project init --board`. The launcher injects the operating system trust store with `truststore` before importing PlatformIO, allowing PlatformIO downloads to use corporate SSL-inspection certificates. PlatformIO resolves the platform, toolchain, and board definition, so the upload tool does not maintain a board allowlist. Invalid board IDs are reported by PlatformIO.

The generated project uses the Arduino framework because it wraps an Arduino `.ino` application. It can target Arduino-framework PlatformIO boards, including AVR, ESP32, and STM32 Arduino targets. A non-Arduino framework requires a different wrapper and runtime adapter.

The serial redirect is optional. Set `REDIRECT_APPLICATION_SERIAL_TO_SERIAL1` to `True` only for boards and applications where `Serial1` exists and application logs must be separate from the TRT transport. The upload tool does not validate this; the adapter/runtime owns serial capability decisions.

A `build_registry.csv` row is appended only after PlatformIO reports that both compilation and upload succeeded. Failed attempts reuse the next unconsumed build ID.

The generated project stages TRT-Core's protocol, dispatcher, board context, capability manager, module registry, and `SystemService`. It also stages the generic Arduino runtime from `boards/arduino/runtime`. This runtime owns the primary `Serial` transport, registers only `SYSTEM`, and responds to `FW_VERSION`, `BOARD_VERSION`, `CAPABILITIES`, and `BUILD_ID`.

The current TRT-Core discovery runtime uses the C++ standard library and is not AVR-safe. It cannot build on an Arduino Uno until an AVR discovery subset replaces dynamic containers, `std::function`, smart pointers, strings, and C++ standard-wrapper headers. AVR support must use fixed-capacity frame/payload buffers, a static command table, a fixed capability bitset, C strings, and `<stdint.h>` / `<stddef.h>`.

The generated wrapper preserves the application lifecycle by renaming its `setup()` and `loop()`:

```cpp
void setup() {
	trt_runtime_setup();
	trt_application_setup();
}

void loop() {
	trt_application_loop();
	trt_runtime_poll();
}
```

TRT owns the primary `Serial` transport. When `REDIRECT_APPLICATION_SERIAL_TO_SERIAL1` is enabled, wrapped application output is redirected to `Serial1`; that setting is appropriate only for multi-UART boards. When it is disabled, wrapped application `Serial` calls are routed to a no-op sink so an Uno application cannot reconfigure or corrupt TRT's only USB serial stream.

Install PlatformIO before running the workflow:

```powershell
python -m pip install platformio
python -m pip install truststore
```