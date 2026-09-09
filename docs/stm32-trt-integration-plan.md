# STM32 TRT-Core Integration Plan

## Current TRT-Core State

TRT-core (`trt-core/`) is a portable runtime framework, not board-specific firmware. Its protocol, command dispatch, capability/module management, and mandatory V0.1 command handlers (`trt-core/core/services/system_service.cpp`) are implemented once and are platform-independent by design: command IDs, wire format, response/error IDs, and payload contracts are shared across every board. A single compile-time flag set (`TRT_HAS_STL`, `TRT_HAS_DYNAMIC_MEMORY`, `TRT_HAS_MODULES`, `TRT_MAX_FRAME_SIZE` in `trt-core/core/config/features.h`) selects between a full STL/dynamic-memory runtime path and a constrained static-memory path — there is no per-target fork of the core codebase. Real hardware boards implement `ITransport` and compose a `BoardContext`; the only board implementation living inside `trt-core/` itself is the simulator.

## Current Arduino Implementation

Arduino is the only hardware target integrated end-to-end today. `boards/arduino/runtime/` provides the Arduino-specific `ITransport` (`ArduinoSerialTransport`), a hand-rolled frame/poll loop (`ArduinoFrameRuntime`), and `trt_runtime_setup()/trt_runtime_poll()` entry points wired into the sketch's `setup()/loop()`. `tools/upload.py` drives the whole pipeline: it stages a PlatformIO project under `.trt-build/<project>/`, copies in the relevant `trt-core/core/*` sources plus `boards/arduino/runtime/`, generates a board-profile header, and calls PlatformIO to build and upload. This works because PlatformIO can fully own an `framework=arduino` build; the script is hardcoded to one board/project and uses the AVR static-memory flag set (`TRT_HAS_STL=0`, etc.) required by the Uno's 2 KB RAM budget.

## Current STM32 Workspace Status

Two STM32-related things exist and are currently unconnected:

- `boards/stm32/` — the architecturally-designated TRT-core integration boundary. Contains only a README and `.gitkeep`; no code.
- `TRT-stm32/am modulation/` — a standalone STM32CubeIDE (CMake/Ninja) project for an STM32L073RZ. It has a generic `main.c` launcher (`app_init()`/`app_loop()`) selecting between placeholder "programs" via `app_selector.h`, and its `CMakeLists.txt` auto-discovers `Sources/*.c`. It has no dependency on `trt-core`, implements no `ITransport`, and is not touched by `tools/upload.py`.

## Why STM32 Should Integrate TRT-Core as a Library

STM32CubeIDE owns clock configuration, HAL/CubeMX-generated code, build, flashing, and debugging for this project, and that ownership is intentional and non-negotiable — TRT-core must be *linked into* that build, not generate a build around it. This mirrors how `boards/arduino/runtime/` is an adapter consumed by an existing build (PlatformIO), not a generator of one. A generate-and-build model for STM32 would mean fighting or duplicating CubeIDE's own project/build model for no protocol-level benefit, since the protocol layer requires no target-specific handling.

## Why STM32 Should Continue Building Through STM32CubeIDE

STM32CubeIDE is the only validated build/flash/debug environment for this hardware. No PlatformIO, CLI-only CMake, or VSCode build path has been established or is planned to replace it. The CubeIDE project's existing CMake integration (glob-based source discovery, `CONFIGURE_DEPENDS`) already supports adding new source directories — including a TRT-core adapter — without changing CubeIDE project settings, startup files, or linker scripts.

## What Parts Are Already Shared

- Protocol: frame layout, command/response/error identifiers, payload formats (`docs/protocol.md`, `docs/commands-v0.1.md`).
- Command handlers: GET_VERSION, GET_ID, GET_CAPABILITIES, GET_BUILD_ID and the rest of the mandatory V0.1 set, implemented once in `trt-core/core/services/system_service.cpp`.
- Core abstractions: `ITransport`, `BoardContext`, `CommandDispatcher`/`CommandRegistry`, `CapabilityManager`, `ModuleManager`, peripheral interfaces (`interfaces/*.h`).
- Feature-flag model: `docs/portability.md`'s target matrix already places STM32 in the full-STL/full-runtime bucket, the same bucket as ESP32 and the simulator — no AVR-style static-memory work is needed for STM32.

## What Parts Still Need Implementation

- An STM32 `ITransport` adapter (USB CDC or UART via HAL).
- An STM32 frame-runtime/poll loop equivalent to `ArduinoFrameRuntime`.
- `trt_runtime_setup()`/`trt_runtime_poll()`-style entry points wired into the CubeIDE project's `app_init()`/`app_loop()`.
- A way for the CubeIDE project's CMake to consume `trt-core`'s shared sources (as a referenced path or library target) without editing CubeIDE project settings, startup files, or linker scripts.
- Capability/module registration at startup for whatever peripherals the STM32 board exposes.

## Recommended Future Integration Path

1. Validate a minimal STM32 `ITransport` sending/receiving raw bytes, independent of the rest of TRT-core.
2. Add `boards/stm32/runtime/` (mirroring `boards/arduino/runtime/`) with the transport, frame-runtime, and `trt_runtime_setup()/poll()`.
3. Wire a new "program" slot in the existing CubeIDE project (`TRT-stm32/am modulation/`) whose `app_init()`/`app_loop()` call into `trt_runtime_setup()/poll()`, and reference `trt-core`'s existing sources from that project's `CMakeLists.txt`.
4. Bring commands up in order: PING → BOARD_VERSION/FW_VERSION → INFO/STATUS → CAPABILITIES → unknown-command NACK path.
5. Validate cross-family parity against the Arduino and simulator implementations, per `docs/protocol.md`.
6. Leave `tools/upload.py` and the Arduino/PlatformIO pipeline untouched; do not attempt to make it drive STM32CubeIDE builds.
