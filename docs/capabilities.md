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

The V0.1 `CAPABILITIES` response uses a count byte followed by big-endian
uint16 identifiers. The identifiers are stable and map to the declared
capability families:

| Identifier | Capability |
|---|---|
| `0x0001` | SYSTEM |
| `0x0002` | GPIO |
| `0x0003` | PWM |
| `0x0004` | ADC |
| `0x0005` | DAC |
| `0x0006` | SPI |
| `0x0007` | I2C |
| `0x0008` | LCD |
| `0x0009` | LED |
| `0x000A` | RELAY |

Only capabilities registered by the board are included in the response.

## Unsupported Behavior

If a command requires a capability not present on the active board context, the command path must return:

- `ERROR_UNSUPPORTED_COMMAND`

This policy is enforced in TRT-core runtime flow and not delegated to TRT-cli.

## Design Benefits

1. One protocol runtime works across many boards.
2. Feature variability is board-owned and explicit.
3. New board families can be integrated without changing TRT-cli behavior.
