# ESP32 Board Family Placeholder

This folder will host ESP32-specific TRT-core integrations.

Expected future content:

- Interface implementations (`IGpio`, `IPwm`, `IAdc`, `IDac`, `II2c`, `ISpi`, ...)
- Transport adapters (for example UART, USB, TCP)
- Board bootstrap code that composes `BoardContext`

No real drivers are included in TRT-core at this stage.
