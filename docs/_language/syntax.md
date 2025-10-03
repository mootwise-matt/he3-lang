---
layout: page
title: syntax
---
# Syntax & Grammar

## Source & Extensions
- **Source files**: `.he3`
- **Raw bytecode** (compiled intermediate): `.bx`
- **Modules** (executable libraries): `.helium3`

## Example Program
```he3
domain app.core;

class Program {
  function main(): integer {
    var a: integer := 2;
    var b: integer := 40;
    return a + b;
  }
}
```

## Security Syntax
```he3
// Security key declaration
security key MyModuleKey = "a1b2c3d4e5f6...";

// Secure domain
domain payment.core @secure(PaymentModuleKey) {
  class PaymentProcessor {
    function processPayment(amount: float): Result<PaymentResult, PaymentError> {
      // Implementation
    }
  }
}

// Handshake and secure calls
let session = PaymentProcessor.handshake(PaymentModuleKey);
match (session) {
  Ok(token) => {
    let result = PaymentProcessor.processPayment(100.0, token);
  }
  Err(error) => throw new SecurityException("Handshake failed");
}
```

## Operators

### Arithmetic Operators
```he3
let a: integer = 10;
let b: integer = 3;

// Basic arithmetic
let sum: integer = a + b;        // 13
let diff: integer = a - b;       // 7
let prod: integer = a * b;       // 30
let quot: integer = a / b;       // 3
let mod: integer = a % b;        // 1
let neg: integer = -a;           // -10

// Mixed types (integer + float)
let mixed: float = a + 3.14;     // 13.14
```

### Comparison Operators
```he3
let x: integer = 15;
let y: integer = 4;

// Comparison operations
let eq: boolean = (x == y);      // false
let ne: boolean = (x != y);      // true
let lt: boolean = (x < y);       // false
let le: boolean = (x <= y);      // false
let gt: boolean = (x > y);       // true
let ge: boolean = (x >= y);      // true

// String comparisons
let str1: string = "hello";
let str2: string = "world";
let str_eq: boolean = (str1 == str2);  // false
```

### Logical Operators
```he3
let p: boolean = true;
let q: boolean = false;

// Logical operations
let and_result: boolean = p && q;     // false
let or_result: boolean = p || q;      // true
let not_result: boolean = !p;         // false

// Complex logical expressions
let complex: boolean = (x > 10) && (y < 5) || (p && !q);  // true
```

## Event Syntax
```he3
// Event creation
let userLoginEvent = EventManager.createEvent<UserLoginEvent>("user.login");

// Event publishing
userLoginEvent.publish(UserLoginEvent {
  userId: "user123",
  timestamp: System.currentTimeMillis(),
  ipAddress: "192.168.1.1"
});

// Event subscription
userLoginEvent.subscribe(function(event: UserLoginEvent) {
  print("User logged in: " + event.userId);
});
```
