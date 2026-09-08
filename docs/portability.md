# Unified TRT-Core Portability Architecture

## One Codebase

TRT-core remains one codebase. It does not split into TRT-Lite, TRT-Full, or target-specific core variants. Target support is selected through compile-time feature flags, static memory limits, and runtime capability declarations.

All targets share the same TRT wire format, sync word, protocol version, command identifiers, response identifiers, error identifiers, byte order, and payload contracts. A host must not need a target-specific protocol.

## Current Dependency Analysis

| Dependency | Current locations | Classification | AVR impact |
|---|---|---|---|
| `std::string` | `BoardInfo`, logging, modules | Optional | Replace discovery metadata with `const char*` or bounded char arrays when `TRT_HAS_STL=0`. |
| `std::vector` | `Frame`, parser, encoder, `CommandResult`, modules | Required by the current general implementation; optional in the architecture | Replace with caller-owned fixed buffers and explicit lengths. |
| `std::array` | `SystemService`, Arduino frame buffer | Optional | Replace with plain fixed arrays. |
| `std::unordered_map` | `CommandRegistry`; unused include in `BoardContext` | Feature-specific: dynamic command registry | Replace with a static command table or `switch` in static-memory builds. |
| `std::unordered_set` | `CapabilityManager` | Feature-specific: dynamic capability registry | Replace with a fixed capability bitset or a bounded array. |
| `std::function` | `CommandHandler` | Feature-specific: dynamically registered handlers | Replace with function pointers and fixed command entries. |
| `std::shared_ptr` | `BoardContext`, `ModuleManager`, simulator, Arduino runtime | Feature-specific: dynamic ownership | Replace with non-owning pointers/references and static lifetime objects. |
| Dynamic allocation | Implicit in STL containers, strings, functions, and shared pointers | Optional | Disallow in static-memory builds. |
| `<cstdint>`, `<cstddef>` | Protocol and interface headers | Required everywhere as type semantics, not as C++ wrapper headers | Use `<stdint.h>` and `<stddef.h>` in portable low-level headers when `TRT_HAS_STL=0`. |

The current Arduino Uno failure occurs because the active general runtime requires `string`, `vector`, `array`, `unordered_set`, `unordered_map`, `functional`, and `memory`. The AVR toolchain used by the target does not provide the required C++ standard-library headers, and the Uno's 2 KB RAM budget is incompatible with the runtime's dynamic allocation model.

## Feature Configuration

Provide one checked-in configuration header, for example `trt-core/core/config/features.h`. Board integration supplies target values through compiler definitions or a small generated target header.

```cpp
#ifndef TRT_HAS_STL
#define TRT_HAS_STL 1
#endif

#ifndef TRT_HAS_DYNAMIC_MEMORY
#define TRT_HAS_DYNAMIC_MEMORY 1
#endif

#ifndef TRT_HAS_MODULES
#define TRT_HAS_MODULES 1
#endif

#ifndef TRT_MAX_FRAME_SIZE
#define TRT_MAX_FRAME_SIZE 256
#endif

#ifndef TRT_MAX_COMMANDS
#define TRT_MAX_COMMANDS 16
#endif
```

Required configuration matrix:

| Target | `TRT_HAS_STL` | `TRT_HAS_DYNAMIC_MEMORY` | `TRT_HAS_MODULES` | Initial capability set |
|---|---:|---:|---:|---|
| Arduino Uno / AVR | `0` | `0` | `0` | `SYSTEM` |
| ESP32 | `1` | `1` | `1` | Board-declared full set |
| STM32 | `1` | `1` | `1` | Board-declared full set |
| Linux / simulator | `1` | `1` | `1` | Board-declared full set |

The flags configure implementation strategy and compiled features. `CapabilityManager` remains the runtime authority for what a particular configured firmware advertises. Compile-time exclusion must prevent an unavailable feature from registering a command or capability.

## AVR Discovery Configuration

For `TRT_HAS_STL=0`, compile the same protocol identifiers and the same discovery command semantics with the following implementation choices:

| General implementation | Static-memory implementation |
|---|---|
| `BoardInfo` strings | `const char*` metadata defined by generated profile |
| Semantic-version string parsing | Three generated `uint8_t` version fields |
| `Frame` and `CommandResult` vectors | Fixed payload array plus `uint16_t payload_length` |
| Parser accepts `std::vector` | Parser accepts `const uint8_t*` and length |
| Encoder returns `std::vector` | Encoder writes to a caller-owned output buffer |
| `unordered_map` / `std::function` registry | Fixed command descriptor array of `{command_id, handler_function}` |
| `unordered_set` capabilities | `uint16_t` capability bitmask; AVR sets only `SYSTEM` |
| `shared_ptr<ITransport>` | Static `ArduinoSerialTransport` reference |

The Uno configuration uses a fixed RX/TX buffer no larger than the selected `TRT_MAX_FRAME_SIZE`; 64 bytes is sufficient for the four discovery commands. There is no heap allocation, module manager, logging string construction, or peripheral interface binding.

### Phase 1 Implementation Status

Phase 1 is implemented in the single TRT-core repository. The feature-selected paths provide:

- Fixed `Frame` and `CommandResult` payload buffers.
- Pointer-and-length parser input and caller-owned encoder output.
- A four-entry function-pointer command table.
- A fixed capability bitmap.
- Static `BoardContext`, transport, dispatcher, and runtime objects.
- `SystemService` payloads for version, board revision, `SYSTEM` capabilities, and six-digit build ID.

The generated Uno PlatformIO configuration defines:

```text
TRT_HAS_STL=0
TRT_HAS_DYNAMIC_MEMORY=0
TRT_HAS_MODULES=0
TRT_MAX_FRAME_SIZE=64
```

The successful AVR build for the `ledringandencoder` application measured `429 / 2048` bytes RAM (20.9%) and `7478 / 32256` bytes flash (23.2%). It was flashed and verified by `avrdude` on an ATmega328P. These figures include the wrapped application and its Adafruit NeoPixel dependency, not only TRT-Core.

For an Uno, the generated wrapper routes application `Serial` calls to a no-op sink unless `REDIRECT_APPLICATION_SERIAL_TO_SERIAL1` is enabled. This preserves exclusive ownership of the only USB serial port for TRT frames. `Serial1` redirection is for multi-UART boards only.

It must implement the same protocol commands:

| Host operation | Shared command |
|---|---:|
| `GET_VERSION` | `FW_VERSION` (`0x0002`) |
| `GET_ID` | `BOARD_VERSION` (`0x0003`) |
| `GET_CAPABILITIES` | `CAPABILITIES` (`0x0006`) |
| `GET_BUILD_ID` | `BUILD_ID` (`0x0007`) |

The payload formats, response IDs, and error behavior remain identical to full TRT-Core builds.

## Migration Plan

1. Extract protocol scalar constants and fixed-buffer parser/encoder API into headers that use `<stdint.h>` and `<stddef.h>` only.
2. Retain the present STL parser/encoder behind `TRT_HAS_STL=1`; add a fixed-buffer implementation behind `TRT_HAS_STL=0`.
3. Make `BoardInfo` a feature-selected storage type while preserving a uniform accessor interface for `SystemService`.
4. Make `CommandResult` feature-selected storage and update `SystemService` to fill a supplied response buffer in static-memory mode.
5. Add a static command registry implementation with the same `register`/`dispatch` contract or use a compile-time command descriptor table.
6. Make `CapabilityManager` use a bitmask in static-memory mode and preserve its public `supports()` API.
7. Compile modules, dynamic logging, and hardware interfaces only when their feature flags are enabled. AVR compiles none of them.
8. Change `tools/upload.py` to select compiler definitions from the target board profile. For Uno: `TRT_HAS_STL=0`, `TRT_HAS_DYNAMIC_MEMORY=0`, `TRT_HAS_MODULES=0`, and `TRT_MAX_FRAME_SIZE=64`.
9. Add host-side tests that run the same discovery vectors against both the STL and fixed-buffer implementations. Add an AVR PlatformIO build-only test before upload testing.
10. Validate an Uno over USB serial with all four discovery operations before enabling any peripheral capability.