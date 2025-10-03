---
layout: page
title: objects
---
# Object File Format (.bx)

## Purpose
- Each `.bx` = compiled domain/file.
- Contains local types + bytecode instructions.
- Packager merges `.bx` files → `.helium3` executable modules.
- Published modules use `.helium3` format.

## Bytecode File Structure
```
┌─────────────────────────────────────┐
│            File Header              │
├─────────────────────────────────────┤
│         String Table                │
├─────────────────────────────────────┤
│         Constant Table              │
├─────────────────────────────────────┤
│         Type Table                  │
├─────────────────────────────────────┤
│         Method Table                │
├─────────────────────────────────────┤
│         Bytecode Section            │
└─────────────────────────────────────┘
```

## ImportEntry (for future cross-module support)
```
nameOffset u32
kind u16 (0=type,1=method,2=field)
```

## LocalTypeEntry
```
nameOffset u32
kind u16
parentImportIdx u32
parentLocalIdx u32
size u32
methodStart u32
methodCount u32
```
