# VM Design (Merged)

## Model
- Stack-based interpreter.
- Registers: PC, SP, FP.
- Each frame stores locals + operand stack.
- Security manager for module authentication.
- Event manager for cross-thread communication.

## Linking & Dynamic Linking
- `.ohe3` files carry imports.
- Linker resolves imports across domains.
- Crowds restrict cross-domain calls.
- `.helium3` modules = prelinked libraries.

## Interpreter Loop (C pseudocode)
```c
while (1) {
  uint8_t op = fetch(pc++);
  switch (op) {
    case OP_PUSH_I64: push(read_i64()); break;
    case OP_ADD: { long b=pop(); long a=pop(); push(a+b); } break;
    case OP_CALL: callMethod(read_u32()); break;
    case OP_CALLV: callVirtual(read_u32()); break;
    case OP_RET: returnFromMethod(); break;
    case OP_SECURE_CALL: secureCall(read_u32()); break;
    case OP_PUBLISH_EVENT: publishEvent(read_u32()); break;
    case OP_SUBSCRIBE_EVENT: subscribeEvent(read_u32()); break;
  }
}
```

## Security Manager
- Validates security keys at module load time.
- Manages session tokens for secure module access.
- Enforces crowd-based security boundaries.
- Throws SecurityException for invalid access attempts.

## Event Manager
- Thread-safe event publishing and subscription.
- Cross-thread callback execution.
- Event context management with mutex protection.
- Secure event support with authentication.
