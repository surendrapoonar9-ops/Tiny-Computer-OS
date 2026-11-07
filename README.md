# Tiny Computer OS

A custom operating system built from scratch using x86 Assembly and C. No existing kernel is used.

## Features
- Custom bootloader (16-bit Real Mode to 32-bit Protected Mode)
- Freestanding C kernel
- Interrupt handling (IDT) with keyboard input
- Basic dynamic memory management (malloc)
- Apps: Calculator, Text Editor, Snake Game, System Info, About

## Build and Run
Requires NASM, GCC (32-bit support) and QEMU.

    nasm -f elf32 kernel_entry.asm -o kernel_entry.o
    nasm -f elf32 idt_load.asm -o idt_load.o
    nasm -f elf32 keyboard.asm -o keyboard_asm.o
    gcc -m32 -ffreestanding -fno-pie -c kernel.c idt.c memory.c calculator.c texteditor.c snake.c about.c sysinfo.c
    gcc -m32 -ffreestanding -fno-pie -c keyboard.c -o keyboard_c.o
    ld -m elf_i386 -T linker.ld -o kernel.bin kernel_entry.o kernel.o idt.o idt_load.o keyboard_c.o keyboard_asm.o memory.o calculator.o texteditor.o snake.o about.o sysinfo.o --oformat binary -nostdlib
    nasm -f bin boot.asm -o boot.bin
    cat boot.bin kernel.bin > os-image.bin
    qemu-system-x86_64 -fda os-image.bin

## Author
Surendra Poonar - College Project 2026
