# TRT-core Configuration Architecture

## Purpose

Configuration is the single source of truth for runtime behavior. TRT-core must avoid hardcoded behavior spread across services and state logic.

## Architecture Placement

```text
TRT Core
|- Configuration Manager
|- State Machine
|- Services
|- Events
|- Protocol
```

All services and runtime policies read configuration through Configuration Manager.

## Configuration Domains

### System

- Board Name
- Board Type
- Board Revision
- Firmware Version
- Protocol Version
- Serial Number

### Communication

- Device Address
- Debug Level

### LED Ring

- Enabled
- Pixel Count
- Brightness
- Default Animation
- Default Color

### Encoder

- Enabled
- Reverse Direction
- Steps Per Detent
- Acceleration Enabled

### Modules

- Enabled Modules
- Slot Configuration

### Debug

- Debug Enabled
- Event Tracing
- Packet Tracing

## Configuration Sources

### Compile-Time Defaults

- Source file: config_defaults.h
- Purpose: guaranteed baseline behavior when no persistent config exists.

### Persistent Configuration

- Storage backend: Flash, EEPROM, or FRAM depending on platform.
- Purpose: preserve runtime tuning across reset/power cycles.

### Future Host Configuration API

Planned command families:

- CONFIG_GET
- CONFIG_SET
- CONFIG_SAVE
- CONFIG_LOAD
- CONFIG_RESET

These commands are not implemented in this phase.

## Runtime Policy

1. Boot loads defaults.
2. Persistent overrides are applied if valid.
3. Services consume immutable snapshot or versioned view per loop.
4. State machine behavior references configuration instead of constants.

## Benefits

1. Predictable and portable behavior across board families.
2. Reduced duplication of policy constants.
3. Easier simulator parity and test automation.
4. Safer future host-side configuration workflows.
