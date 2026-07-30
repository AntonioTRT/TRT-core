# TRT-core

TRT-core (Tool Runtime Terminal core) is a portable embedded firmware framework shared by all TRT-compatible boards.

It is not a board firmware application and not a CLI. It is the reusable middle layer between TRT protocol messages and board-specific implementations.

## Scope

TRT-core provides architecture for:

- Protocol parser integration
- Protocol encoder integration
- Command dispatch flow
- Capability registration and checks
- Board abstraction through interfaces
- Module registration and discovery
- Transport abstraction

TRT-core intentionally does **not** provide:

- Real hardware drivers
- Board-specific GPIO/PWM/ADC/SPI/I2C implementations
- Concrete protocol command handlers

## TRT-cli vs TRT-core

TRT-cli owns:

- User experience
- Discovery flows
- Command generation
- Response decoding and display

TRT-core owns:

- Protocol parsing integration
- Command routing
- Capability-driven runtime checks
- Module lifecycle abstractions
- Runtime board context and service access

Board implementations own:

- GPIO/PWM/ADC/DAC/SPI/I2C/UART and other hardware access
- Transport implementation details
- Hardware logic and constraints

## Layer Overview

```text
TRT-cli
  -> TRT Protocol
  -> TRT-core
  -> Board Implementation
  -> Hardware
```

TRT-core never knows whether it runs on STM32, ESP32, Arduino, RP2040, Raspberry Pi class systems, simulator, or future TRT hardware. It only targets interfaces.

## Board Abstraction Philosophy

Core logic depends on interfaces such as:

- IGpio
- IPwm
- IAdc
- IDac
- II2c
- ISpi
- ITransport

Boards provide concrete classes (for example STM32_Gpio, ESP32_Gpio, Arduino_Gpio, Simulator_Gpio) and bind them into `BoardContext` at startup.

## Capability-Driven Design

Capabilities are dynamic and board-owned. At startup, board code registers supported capabilities. During dispatch, command handlers check capabilities before execution.

If unsupported, the framework returns:

- `ERROR_UNSUPPORTED_COMMAND`

This keeps support policy in board/runtime context rather than in TRT-cli.

## Module System

TRT-core defines a generic module abstraction with:

- Module type
- Module revision
- Module capability list

This supports future module families (relay boards, LCD, LED rings, DAC modules, and custom modules) without coupling command logic to one hardware family.

## Transport System

Transport is abstracted behind `ITransport` with:

- `connect()`
- `disconnect()`
- `send()`
- `receive()`

Future transports include USB CDC, UART, CAN FD, and TCP/IP.

## Simulator Target

The simulator target is a first-class board integration. It is used for protocol/dispatcher testing without physical hardware and is expected to provide mock implementations for GPIO/PWM/ADC.

No simulator behavior is implemented yet in this repository; only architecture and contracts are provided.

## Repository Guide

- `core/`: protocol integration, dispatcher, capabilities, board context, errors, logging abstraction
- `interfaces/`: board-facing hardware interface contracts
- `modules/`: module abstractions and module manager
- `transports/`: transport abstraction and future transport folders
- `boards/`: board-family integration placeholders and onboarding notes
- `docs/`: architecture and integration documentation

## Future Roadmap

1. Add reference board adapters (STM32, ESP32, Arduino, RP2040, simulator).
2. Add concrete transport adapters (USB CDC, UART, CAN FD, TCP).
3. Add command handler packs driven by capability policies.
4. Add protocol conformance and simulator-based integration tests.
5. Add debug-level output plumbing for `-d`, `-dd`, `-ddd`, `-dddd`.
