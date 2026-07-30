# Module Framework

TRT-core defines a generic module abstraction for reusable board add-ons and peripheral packs.

## Module Contract

Every module provides metadata through `ModuleDescriptor`:

- Module type
- Module revision
- Module name
- Capability list

`IModule` is interface-only and contains no hardware behavior.

## Module Manager

`ModuleManager` is responsible for:

- Module registration
- Exposing registered module inventory

Board startup code is expected to register present modules in `BoardContext`.

## Planned Module Families

- Relay board modules
- LCD modules
- Neopixel/LED modules
- DAC modules
- Analog I/O modules
- Digital I/O modules

This repository provides architecture and contracts only. Real module behavior is deferred to board integrations and command packs.
