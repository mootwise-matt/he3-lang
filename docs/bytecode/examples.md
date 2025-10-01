# Bytecode Examples

## Source
```he3
class Adder {
  function add(a: integer, b: integer): integer {
    return a + b;
  }
}

class Program {
  function main(): integer {
    return Adder.add(2, 40);
  }
}
```

## Adder.add
```
LOAD_ARG 0
LOAD_ARG 1
ADD
RET
```

## Program.main
```
PUSH_I64 2
PUSH_I64 40
CALL methodId=0
RET
```
