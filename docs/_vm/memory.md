---
layout: page
title: memory
---
# Memory Model

## Object Layout
```
+----------------+----------------+------------------+
| typeId (u32)   | flags (u32)    | vtable* (u64)    |
+----------------+----------------+------------------+
| field0         | field1         | ...              |
```

## Records
- Value types (inline storage).
- Boxed into heap with header when needed.

## Strings
- typeId=System.String
- Fields: length, UTF-8 byte array.
