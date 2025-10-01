# Object-Oriented Model

## Classes
```he3
class Animal {
  var name: string;
  function speak(): string { return "??"; }
}

class Dog extends Animal {
  override function speak(): string { return "Woof " + self.name; }
}
```

## Interfaces
```he3
interface PaymentProvider {
  async function charge(p: Product): Result<PaymentStatus, string>;
}
```

## Properties
```he3
class Account {
  var _balance: float;

  property balance: float {
    get { return _balance; }
    set { _balance := value; }
  }
}
```

## Object Layout
```
+----------------+----------------+------------------+
| typeId (u32)   | flags (u32)    | vtable* (u64)    |
+----------------+----------------+------------------+
| field0         | field1         | ...              |
```

## Dispatch
- `CALL` = static call.
- `CALLV` = virtual call via vtable slot.
