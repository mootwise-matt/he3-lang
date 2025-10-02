---
layout: page
title: syntax
---
# Syntax & Grammar

## Source & Extensions
- **Source files**: `.he3`
- **Modules** (published libraries): `.helium3`
- **Object files** (compiled intermediate): `.ohe3`
- **Executable bytecode**: `.bx`

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
