# STM32 Board Family Integration

This directory is the integration boundary for STM32 board variants that run TRT-core.

Integration responsibilities:

1. Implement board interfaces consumed by TRT-core.
2. Provide one or more ITransport adapters.
3. Register capabilities and modules at startup.
4. Bind configuration defaults and persistent storage.
5. Run the non-blocking main loop defined by TRT-core execution model.

TRT-core architecture remains unchanged; only STM32 adapters and board-specific wiring live here.

