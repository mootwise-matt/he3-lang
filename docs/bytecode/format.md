# He³ Bytecode Format (.bx)

## 🚀 Implementation Status

**Note**: The bytecode format and virtual machine are currently in the design phase. The compiler generates AST but does not yet emit bytecode. This document describes the planned bytecode format.

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
