# TRT-core

TRT-core (Tool Runtime Terminal core) is the portable runtime framework shared by all TRT-compatible boards.

It is not an application, not a board-specific firmware project, and not the CLI. It is the runtime architecture between TRT protocol communication and board implementations.

## Mission

Define one production runtime model that works for STM32, ESP32, Arduino, RP2040, simulator targets, and future TRT hardware without redesign.

The board remains autonomous and keeps operating:

1. When no host PC is connected.
2. While commands are being received.
3. While events are being transmitted.

## Ownership Boundaries

TRT-cli owns:

1. User experience
2. Discovery workflows
3. Command generation
4. Response/event decoding for presentation

TRT-core owns:

1. Protocol integration (parse and encode contracts)
2. Queue-based command/event runtime flow
3. Non-blocking execution model
4. State machine governance
5. Capability management
6. Module framework
7. Configuration manager architecture
8. Board abstraction through interfaces
9. Mandatory protocol compatibility command contracts (V0.1)

Board integrations own:

1. Hardware drivers and peripheral control
2. Platform-specific transport adapters
3. Hardware timing constraints and low-level behavior

## Layer Overview

```text
TRT-cli
  -> TRT Protocol
  -> TRT-core
  -> Board Implementation
  -> Hardware
```

TRT-core never branches on board family names. It depends on interfaces and runtime capability declarations.

## Execution Philosophy

TRT boards are autonomous devices with internal logic, services, and state behavior. Communication is a layer, not a control loop owner.

Forbidden model:

```cpp
while (1) {
    wait_for_message();
}
```

Required model:

```cpp
while (1) {
    update_services();
    process_events();
    process_commands();
    update_state_machine();
    transmit_pending_messages();
}
```

This design prevents runtime stalls and preserves local behavior during host disconnects.

## Core Runtime Components

1. Configuration Manager
2. State Machine
3. Service Layer
4. Event System
5. Command System
6. Protocol Integration
7. Transport Interface

## Capability-Driven Behavior

Boards declare capabilities at startup, for example GPIO, PWM, ADC, DAC, SPI, I2C, LCD, LED, RELAY.

Commands remain generic. Unsupported functionality returns ERROR_UNSUPPORTED_COMMAND.

The CLI must not apply board-specific support decisions.

## Transport Model

Transport is abstracted by ITransport:

1. send()
2. receive()
3. connect()
4. disconnect()

Protocol integration does not depend on USB CDC, UART, CAN FD, TCP/IP, or simulator-specific details.

## Repository Structure and Purpose

- core/: parser/encoder contracts, dispatcher, capabilities, board context, errors, logging, runtime architecture stubs
- interfaces/: hardware and peripheral interface contracts consumed by core
- modules/: module abstraction and module registry/management architecture
- transports/: transport abstraction and transport-family integration locations
- boards/: board-family integration boundaries and porting contracts
- simulator/: first executable TRT-compatible simulator board runtime
- docs/: official runtime architecture documentation
- configs/: compile-time and reference runtime configuration artifacts
- examples/: integration examples and test harness entrypoints
- tests/: protocol validation tests for simulator interoperability

## Documentation Index

- docs/architecture.md
- docs/execution-model.md
- docs/state-machine.md
- docs/services.md
- docs/configuration.md
- docs/event-system.md
- docs/host-communication.md
- docs/firmware-v0.1-roadmap.md
- docs/porting-guide.md
- docs/protocol.md
- docs/protocol-integration.md
- docs/protocol-v0.1-wire-format.md
- docs/command-identifiers.md
- docs/response-model.md
- docs/error-model.md
- docs/simulator.md
- docs/protocol-readiness-checklist.md
- docs/commands-v0.1.md
- docs/capabilities.md
- docs/modules.md
- docs/boards.md

## TRT Protocol V0.1 Baseline

Protocol milestone 0.1.0 defines the first official mandatory command set for TRT compatibility:

1. 0x0001 PING
2. 0x0002 FW_VERSION
3. 0x0003 BOARD_VERSION
4. 0x0004 INFO
5. 0x0005 STATUS
6. 0x0006 CAPABILITIES

Response identifiers:

1. 0x8001 ACK
2. 0x8002 NACK
3. 0x8003 DATA

Error identifiers:

1. 0x0001 ERROR_UNKNOWN_COMMAND
2. 0x0002 ERROR_UNSUPPORTED_COMMAND
3. 0x0003 ERROR_INVALID_ARGUMENT
4. 0x0004 ERROR_BUSY
5. 0x0005 ERROR_TIMEOUT
6. 0x0006 ERROR_INTERNAL

This baseline validates protocol and transport architecture, discovery, board identification, status reporting, and capability reporting before peripheral-control commands are introduced.

In this protocol validation phase, wire format and identifiers are frozen to support deterministic simulator-first interoperability testing with TRT-cli.



## Debug Levels (Planned)

1. -d: general execution flow
2. -dd: transmitted packets
3. -ddd: transmitted packets + decoded commands
4. -dddd: TX, RX, decoded packets, state transitions, event generation, queue activity

Debug level behavior is documented and reserved for future implementation.

