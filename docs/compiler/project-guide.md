# He³ Project Development Guide

## Project Structure

### Executable Project
```
my-app/
├── src/
│   ├── main.he3              # Entry point
│   ├── domain/
│   │   ├── app/
│   │   │   ├── core/
│   │   │   │   ├── models.he3
│   │   │   │   └── services.he3
│   │   │   └── ui/
│   │   │       └── components.he3
│   │   └── payment/
│   │       └── secure/
│   │           └── processor.he3
│   ├── events/
│   │   └── system.he3
│   └── config/
│       └── settings.he3
├── lib/                      # External dependencies
│   └── payment-module.helium3
├── keys/                     # Security keys (gitignored)
│   └── payment.key
├── build/                    # Generated files
│   ├── *.ohe3               # Object files
│   └── my-app.bx            # Final bytecode
└── he3project.json          # Project configuration
```

### Library Project
```
my-library/
├── src/
│   ├── domain/
│   │   └── mylib/
│   │       └── core/
│   │           ├── api.he3
│   │           └── impl.he3
│   └── events/
│       └── library.he3
├── keys/
│   └── library.key
├── build/
│   └── my-library.helium3   # Published module
└── he3project.json
```

## Project Configuration (he3project.json)

```json
{
  "name": "my-app",
  "version": "1.0.0",
  "type": "executable",
  "entryPoint": "src/main.he3",
  "domains": [
    "app.core",
    "app.ui", 
    "payment.secure"
  ],
  "dependencies": [
    {
      "name": "payment-module",
      "version": "2.1.0",
      "path": "lib/payment-module.helium3"
    }
  ],
  "security": {
    "keys": {
      "payment": "keys/payment.key",
      "database": "keys/database.key"
    }
  },
  "build": {
    "output": "build/my-app.bx",
    "optimize": true,
    "debug": false
  }
}
```

## Entry Point (main.he3)

```he3
domain app.main;

import app.core.*;
import app.ui.*;
import payment.secure.*;
import system.events.*;

class Program {
  function main(): integer {
    // Initialize security modules
    initializeSecurity();
    
    // Set up event system
    setupEvents();
    
    // Start application
    let app = new Application();
    return app.run();
  }
  
  private function initializeSecurity() {
    // Load security keys
    let paymentKey = SecurityManager.loadKey("payment");
    let dbKey = SecurityManager.loadKey("database");
    
    // Establish secure sessions
    match (PaymentProcessor.handshake(paymentKey)) {
      Ok(token) => {
        GlobalContext.paymentToken = Some(token);
        print("Payment module authenticated");
      }
      Err(error) => {
        print("Payment authentication failed: " + error.message);
        System.exit(1);
      }
    }
  }
  
  private function setupEvents() {
    // Subscribe to system events
    SystemShutdownEvent.subscribe(function(event) {
      print("System shutting down: " + event.reason);
      cleanup();
    });
  }
}
```

## Compilation Process

### 1. Compile Source Files
```bash
# Compile all .he3 files to .ohe3 objects
he3 compile src/**/*.he3 -o build/

# Or compile specific files
he3 compile src/main.he3 src/domain/app/core/models.he3 -o build/
```

### 2. Link Object Files
```bash
# Link objects into executable bytecode
he3 link build/*.ohe3 -o build/my-app.bx

# Or use project file
he3 build
```

### 3. Run Executable
```bash
# Execute bytecode
he3vm build/my-app.bx

# With arguments
he3vm build/my-app.bx --config=production --debug
```

## Build Commands

### Quick Build
```bash
# Build and run in one command
he3 run src/main.he3

# Build with project configuration
he3 build
he3vm build/my-app.bx
```

### Development Build
```bash
# Debug build with symbols
he3 build --debug
he3vm build/my-app.bx --debug

# Watch mode for development
he3 watch src/ --output build/
```

### Library Build
```bash
# Build library module
he3 build --library -o build/my-library.helium3

# Install library locally
he3 install build/my-library.helium3
```

## File Organization Rules

### Domain Structure
- Each `.he3` file contains one domain
- Domain name matches file path: `src/domain/app/core/models.he3` → `domain app.core`
- Related classes grouped in same file
- One main class per file (optional)

### Import System
```he3
// Import specific classes
import app.core.User;
import app.core.PaymentService;

// Import entire domain
import app.core.*;

// Import with alias
import payment.secure.PaymentProcessor as PP;
```

### Security Keys
- Store keys in `keys/` directory
- Add `keys/` to `.gitignore`
- Reference keys in `he3project.json`
- Use environment variables for production

### Event Organization
- Define events in `events/` directory
- One event file per domain or feature
- Use descriptive event names: `user.login`, `payment.processed`

## Module Dependencies

### External Libraries
```he3
// In he3project.json
"dependencies": [
  {
    "name": "http-client",
    "version": "1.2.0",
    "source": "helium3-registry"
  }
]

// In source code
import http.client.HttpClient;
```

### Internal Modules
```he3
// Reference other domains
import app.core.UserService;
import app.ui.UserInterface;

// Use in code
let userService = new UserService();
let ui = new UserInterface(userService);
```

## Build Output

### Executable (.bx)
- Contains all linked bytecode
- Includes security keys and event definitions
- Ready for VM execution
- Can be exported to native code

### Library (.helium3)
- Compiled module for distribution
- Contains public API only
- Includes metadata and dependencies
- Can be imported by other projects

## Development Workflow

1. **Create Project**: `he3 init my-app`
2. **Add Files**: Create `.he3` files in `src/`
3. **Configure**: Edit `he3project.json`
4. **Build**: `he3 build`
5. **Test**: `he3vm build/my-app.bx`
6. **Debug**: `he3 build --debug && he3vm build/my-app.bx --debug`
7. **Deploy**: Export to native or distribute `.bx`

## Common Patterns

### Application Class
```he3
class Application {
  function run(): integer {
    // Main application logic
    return 0;
  }
}
```

### Service Initialization
```he3
class ServiceManager {
  static function initialize() {
    // Initialize all services
    DatabaseService.initialize();
    EventManager.initialize();
    SecurityManager.initialize();
  }
}
```

### Error Handling
```he3
function processData(data: string): Result<ProcessedData, ProcessingError> {
  match (validateInput(data)) {
    Ok(validData) => {
      // Process valid data
      return Ok(process(validData));
    }
    Err(error) => {
      return Err(ProcessingError.ValidationFailed(error.message));
    }
  }
}
```

## Security Integration

### Key Management
```he3
// Load keys from project configuration
let keys = SecurityManager.loadKeysFromProject("he3project.json");

// Validate security context
if (!SecurityManager.validateContext(keys)) {
  throw new SecurityException("Invalid security configuration");
}
```

### Secure Module Access
```he3
// Access secure modules with proper authentication
match (SecureModule.authenticate(moduleKey)) {
  Ok(session) => {
    let result = SecureModule.performOperation(data, session);
    // Handle result
  }
  Err(error) => {
    logSecurityError("Module access denied", error);
  }
}
```

## Event System Integration

### Event Setup
```he3
// Initialize event system
EventManager.initialize();

// Create application events
let appEvents = EventManager.createEvent<ApplicationEvent>("app.events");

// Subscribe to system events
SystemEvents.subscribe(function(event) {
  appEvents.publish(ApplicationEvent {
    type: "system_notification",
    message: event.message,
    timestamp: System.currentTimeMillis()
  });
});
```

### Cross-Module Communication
```he3
// Module A publishes events
PaymentModule.processPayment(amount).then(function(result) {
  PaymentEvents.publish(PaymentProcessedEvent {
    orderId: result.orderId,
    amount: result.amount,
    status: result.status
  });
});

// Module B subscribes to events
PaymentEvents.subscribe(function(event) {
  NotificationModule.sendReceipt(event.orderId);
  AnalyticsModule.recordPayment(event);
});
```

## Compiler Source Structure

The He³ compiler and virtual machine are organized with a **shared architecture** to ensure consistency and maintainability:

### **Source Directory Structure**
```
src/
├── shared/                    # Shared components between compiler and VM
│   ├── ast/                  # Unified AST definitions
│   │   ├── ast.h            # AST node types and functions
│   │   └── ast.c            # AST implementation
│   ├── tokens.h             # Common token definitions
│   ├── bytecode/            # Shared bytecode format
│   │   ├── bytecode_format.h
│   │   ├── opcodes.h
│   │   └── constant_table.c
│   └── types/               # Common type definitions
│       ├── types.h
│       └── typetable.h
├── compiler/                 # Compiler-specific components
│   ├── lexer/               # Lexical analysis
│   ├── parser/              # Syntax analysis
│   ├── ir/                  # Intermediate representation
│   ├── emitter/             # Code generation
│   └── main.c               # Compiler entry point
└── vm/                      # Virtual machine components
    ├── execution/           # VM execution engine
    ├── memory/              # Memory management
    ├── objects/             # Object system
    └── main.c               # VM entry point
```

### **Shared Architecture Benefits**
- **Consistency**: Compiler and VM use identical data structures
- **Maintainability**: Single source of truth for common definitions
- **Reliability**: Eliminates compatibility issues between components
- **Development Speed**: Changes to shared components automatically propagate

### **Key Shared Components**
- **AST System**: Unified tree representation used by both compiler and VM
- **Token Definitions**: Common token types for lexer and parser
- **Bytecode Format**: Shared instruction set and file format
- **Type System**: Common type definitions and utilities
