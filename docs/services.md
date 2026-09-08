# TRT-core Service Architecture

## Purpose

Services encapsulate reusable runtime behaviors updated from the main loop. They translate hardware-facing signals and internal timers into state-machine-relevant events and actions.

Services are platform-agnostic by contract and use interface abstractions for board integration.

## Service Principles

1. Updated periodically from the non-blocking main loop.
2. Do not block waiting for I/O or host commands.
3. Do not contain transport-specific logic.
4. Emit events and state intents rather than forcing direct hardware outcomes.
5. Read runtime policy through Configuration Manager.

## Baseline Service Set

### Encoder Service

- Reads encoder movement through interface adapters.
- Applies configuration such as direction reversal and detent settings.
- Emits events such as ENCODER_CW and ENCODER_CCW.

### Button Service

- Debounces and classifies button interactions.
- Emits BUTTON_CLICK and BUTTON_LONG_PRESS.

### LED Ring Service

- Applies visual policy from current state machine state.
- Reads brightness, animation, and enable policy from configuration.

### Communication Service

- Moves packets across parser, queues, dispatcher, and transport TX.
- Preserves non-blocking execution and bounded queue draining.

### Module Service

- Maintains module lifecycle and module-generated events.
- Coordinates module registration and state-aware activation.

### Configuration Service

- Loads defaults and persistent values into Configuration Manager.
- Exposes runtime configuration snapshots to other services.

## Service Lifecycle in Main Loop

Recommended per-iteration order:

1. Update input services (encoder/button).
2. Update operation services (modules/business behavior).
3. Update communication service.
4. Update visualization services (LED ring).

Exact ordering can vary by platform timing needs, but all services must progress regardless of host traffic.

## Extensibility

New services may be added without changing protocol definitions, as long as they honor queue contracts and state-machine governance.
