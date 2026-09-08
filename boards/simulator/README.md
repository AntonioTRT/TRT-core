# Simulator Integration Target

The simulator is a first-class TRT board implementation used for development and validation without physical hardware.

Simulator responsibilities:

1. Implement mock interfaces for GPIO, PWM, ADC, and additional services as needed.
2. Provide in-memory or loopback transport adapters.
3. Execute the same state machine, queue model, and service update loop as hardware boards.
4. Support protocol, state-machine, service, and CLI workflow testing.

The simulator follows the same runtime contracts as all hardware targets.

