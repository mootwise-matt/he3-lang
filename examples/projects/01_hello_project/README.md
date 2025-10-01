# Hello Project

A simple He³ project demonstrating basic language features and project structure.

## Project Structure

```
01_hello_project/
├── he3project.json          # Project configuration
├── src/
│   ├── main.he3            # Entry point
│   └── utils.he3           # Utility classes
├── lib/                    # External dependencies (empty)
├── keys/                   # Security keys (empty)
├── build/                  # Generated files
└── README.md              # This file
```

## Features Demonstrated

- **Domain declarations**: `domain app.hello` and `domain app.utils`
- **Class definitions**: `Program` and `Greeter` classes
- **Method declarations**: Functions with parameters and return types
- **String operations**: String concatenation and manipulation
- **Import statements**: Cross-domain imports
- **Project configuration**: JSON-based project setup

## Building the Project

```bash
# Compile the project
he3 build

# Run the executable
he3vm build/hello-project.bx
```

## Expected Output

```
Hello, He³ Developer!
Welcome to the He³ programming language!
```

## Files

- `src/main.he3`: Main entry point with Program class
- `src/utils.he3`: Utility classes for string operations
- `he3project.json`: Project configuration and build settings
