# TRT-core State Machine

## Purpose

The state machine is the execution authority for board behavior. Commands, events, and service outputs are interpreted through state rules instead of directly toggling hardware behavior.

This keeps board logic deterministic, testable, and autonomous.

## Canonical States

### BOOT

- Initial startup sequence.
- Initializes configuration, capabilities, services, and modules.
- Transition target: IDLE or ERROR.

### IDLE

- Minimal baseline operation.
- Device is alive and responsive.
- Transition target: READY, WARNING, ERROR.

### READY

- Normal operation state.
- User interaction accepted.
- Commands accepted based on capability and policy.
- Typical visual behavior: stable ready indication.

### BUSY

- Active operation in progress.
- Board may limit conflicting commands.
- Services continue to run; communication continues.

### WARNING

- Degraded condition detected.
- Non-critical issues are exposed to host via events/status.
- Board remains functional while signaling caution.

### ERROR

- Critical fault condition.
- Board enters safe behavior and reports fault.
- Recovery may require command, condition clear, or reboot.

### FIRMWARE_UPDATE

- Dedicated mode for update workflows.
- Normal runtime actions are restricted.
- Strict state-entry and exit conditions are required.

## State Effects

A state can affect:

1. LED ring color and animation policy.
2. User interaction interpretation.
3. Allowed command categories.
4. Event emission policy and severity mapping.
5. Service execution mode.

## Command Integration Rule

Commands must request state intent, not direct hardware behavior.

Good pattern:

- SET_STATE(BUSY)

Bad pattern:

- LED_SET_RED

The state machine decides how outputs behave for each state.

## Example Flow: Encoder + LED Ring

Initial condition:

1. State = READY.
2. LED ring policy for READY = blue.

Interaction:

1. User presses encoder button.
2. Button service emits BUTTON_CLICK event.
3. State machine consumes event and transitions READY -> BUSY.
4. LED ring service applies BUSY policy = red.
5. Operation starts and progresses autonomously.

Completion:

1. Operation service emits OPERATION_DONE event.
2. State machine transitions BUSY -> READY.
3. LED ring returns to READY policy = blue.
4. Event is queued for host notification.

This flow does not require host approval to operate.

## State Transition Governance

Transition logic should define:

1. Entry actions.
2. Exit actions.
3. Guards/conditions.
4. Timeouts.
5. Recovery transitions.

These rules are runtime policy and should be externally configurable where practical.
