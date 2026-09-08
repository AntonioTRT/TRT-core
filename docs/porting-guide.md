# TRT-core Porting Guide

## Purpose

This guide defines the exact minimum implementation requirements to bring TRT-core onto a new board family for Protocol V0.1.

It applies to Arduino, STM32, ESP32, RP2040, simulator, and future boards.

## Porting Outcome

A board port is complete for V0.1 when the board can boot, run autonomous non-blocking runtime, and respond to mandatory commands:

1. PING
2. BOARD_VERSION
3. FW_VERSION
4. INFO
5. STATUS
6. CAPABILITIES

Unknown commands must return ERROR_UNSUPPORTED_COMMAND.

## Minimum Components to Implement

### 1. BoardContext Composition

You must construct and wire BoardContext with:

1. BoardInfo
2. CapabilityManager
3. ModuleManager (may be empty for V0.1)
4. ITransport implementation
5. Required interface adapters relevant to the board

### 2. BoardInfo Provider

Provide deterministic metadata:

1. board_id
2. family
3. name
4. revision
5. firmware_version

These fields feed BOARD_VERSION, FW_VERSION, and INFO responses.

### 3. ITransport Adapter

Implement one transport adapter with these methods:

1. connect()
2. disconnect()
3. send()
4. receive()

V0.1 requires only one working transport path per board for initial validation.

### 4. Configuration Backend

Implement configuration loading with two stages:

1. Compile-time defaults from config_defaults.h
2. Optional persistent overrides (Flash/EEPROM/FRAM)

Configuration is the runtime source of truth and should feed INFO/STATUS behavior.

### 5. Capability Registration

Declare supported capabilities during startup.

CAPABILITIES response must reflect runtime-registered values.

### 6. Command Dispatch Wiring

Wire parser and dispatcher through queues:

1. RX Queue
2. Command Queue
3. TX Queue

Ensure mandatory commands are registered and routed.

### 7. State Machine Bootstrap

Initialize state machine and guarantee valid startup sequence:

1. BOOT
2. IDLE or READY

STATUS command must expose current state.

## Main Loop Requirements

A compliant port must run a non-blocking cooperative loop:

```cpp
while (1) {
    update_services();
    process_events();
    process_commands();
    update_state_machine();
    transmit_pending_messages();
}
```

Blocking message-wait loops are not allowed.

## Mandatory Command Behavior Summary

### PING

- Report liveness.

### BOARD_VERSION

- Report board family/type/revision identity.

### FW_VERSION

- Report firmware version and protocol compatibility marker.

### INFO

- Report consolidated identity and serial metadata.

### STATUS

- Report current state and runtime health summary.

### CAPABILITIES

- Report registered board capabilities.

### Unknown Commands

- Return ERROR_UNSUPPORTED_COMMAND.

## Recommended Bring-Up Order

1. Transport receive/transmit smoke test.
2. PING round-trip.
3. BOARD_VERSION and FW_VERSION correctness.
4. INFO and STATUS correctness.
5. CAPABILITIES correctness.
6. Unknown-command error path verification.

## Port Validation Checklist

1. Device boots without host connected.
2. Mandatory command responses are deterministic.
3. Host reconnect does not require reboot.
4. Runtime loop remains active during command traffic.
5. CAPABILITIES output matches board registration.
6. Unknown commands return ERROR_UNSUPPORTED_COMMAND.
