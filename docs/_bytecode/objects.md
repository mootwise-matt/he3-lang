---
layout: page
title: objects
---
# Object File Format (.ohe3)


## Purpose
- Each `.ohe3` = compiled domain/file.
- Contains local types + unresolved imports.
- Linker merges `.ohe3` → `.bx` executable.
- Published modules use `.helium3`.

## ImportEntry
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
