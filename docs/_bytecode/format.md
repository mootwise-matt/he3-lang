---
layout: page
title: format
---
# He³ Bytecode Format (.bx)

## 🚀 Implementation Status

**✅ IMPLEMENTED**: The shared bytecode format is now fully implemented and integrated with the VM. The compiler and VM use unified structures defined in `src/shared/bytecode/bytecode_format.h` and `src/shared/bytecode/opcodes.h`. This document describes the implemented bytecode format.

## Related Documentation

- **[.helium Module Format](helium-module-format.md)** - Complete module file format specification
- **[Object Model](../vm/object-model.md)** - Runtime object representation and memory management
- **[Memory Management](../vm/memory-management.md)** - Garbage collection and memory safety
- **[Instruction Set](instruction-set.md)** - Complete bytecode instruction set reference
- **[Opcode Reference](opcodes.md)** - Quick opcode reference

(Adapted from He3 bytecode-specification.md)

## Header
```
Magic "BX01"
Version u16
Flags u16
DomainNameOffset u32
CrowdListOffset u32
ImportCount u32
ImportTableOffset u32
TypeCount u32
TypeTableOffset u32
MethodCount u32
MethodTableOffset u32
StringTableOffset u32
StringTableSize u32
```
Extended for He³:
- UTF-8 string table
- Crowd membership encoded per domain
- 64-bit alignment
