# Exporter: Native Assembly

## Direct Export
- Emit x86-64 or ARM64 assembly.
- Assemble with system tools.

## Example (x86-64 SysV)
```asm
global main
section .text

main:
    mov rax, 2
    mov rbx, 40
    add rax, rbx
    ret
```
