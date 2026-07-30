# RP2040 Board Family Placeholder

This folder will host RP2040-specific TRT-core integrations.

Expected future content:

- Interface implementations (`IGpio`, `IPwm`, `IAdc`, `II2c`, `ISpi`, ...)
- Transport adapters (for example USB CDC, UART)
- Board bootstrap code that composes `BoardContext`

No real drivers are included in TRT-core at this stage.
