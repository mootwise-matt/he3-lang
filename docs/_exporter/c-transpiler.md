---
layout: page
title: c-transpiler
---
# Exporter: C Transpiler

## Purpose
- Translate `.bx` bytecode → C.
- Compile with system compiler → native executable.

## Example Output
Source:
```he3
function main(): integer {
  return 2 + 40;
}
```

Bytecode:
```
PUSH_I64 2
PUSH_I64 40
ADD
RET
```

Transpiled C:
```c
#include <stdint.h>

int64_t main_fn() {
  return 2 + 40;
}

int main() {
  return (int) main_fn();
}
```
