# Arduino Board Family Placeholder

This folder will host Arduino-specific TRT-core integrations.

Expected future content:

- Interface implementations (`IGpio`, `IPwm`, `IAdc`, and others based on board)
- Transport adapters (for example UART or USB serial)
- Board bootstrap code that composes `BoardContext`

No real drivers are included in TRT-core at this stage.
