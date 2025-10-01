# Exporter: LLVM Backend

## Approach
- Translate `.bx` → LLVM IR.
- Use `llc` or `clang` for optimisation + native code.

## Example
```llvm
define i64 @main() {
entry:
  %a = add i64 2, 40
  ret i64 %a
}
```
