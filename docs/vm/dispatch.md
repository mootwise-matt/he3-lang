# Dispatch Rules

## Static Call
- `CALL <methodId>` jumps directly to target.

## Virtual Call
- `CALLV <slot>` looks up slot in object’s vtable.
- Example:
  - Animal.vtable[0] = Animal.speak
  - Dog.vtable[0] = Dog.speak (override)

## Interfaces
- VM builds interface maps at load.
- Dispatch via indirection.
