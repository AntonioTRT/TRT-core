# Capability Model

Capabilities are dynamic runtime declarations owned by board integrations.

## Why Capabilities Exist

Not every TRT-compatible board supports every peripheral family. Capabilities allow one shared command architecture to adapt at runtime.

## Registration

During board initialization, board code registers supported capabilities with `CapabilityManager`.

Examples:

- GPIO
- PWM
- ADC
- DAC
- SPI
- I2C
- LCD
- LED
- RELAY

## Discovery

Command handlers and runtime services query capability support through `CapabilityManager::supports()`.

No board-specific type checks are needed in core dispatch logic.

## Unsupported Behavior

If a command requires a capability not present on the active board context, the command path must return:

- `ERROR_UNSUPPORTED_COMMAND`

This policy is enforced in TRT-core runtime flow and not delegated to TRT-cli.

## Design Benefits

1. One protocol runtime works across many boards.
2. Feature variability is board-owned and explicit.
3. New board families can be integrated without changing TRT-cli behavior.
