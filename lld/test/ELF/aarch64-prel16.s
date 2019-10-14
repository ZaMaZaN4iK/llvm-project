// REQUIRES: aarch64
// RUN: llvm-mc -filetype=obj -triple=aarch64-none-freebsd %s -o %t.o
// RUN: llvm-mc -filetype=obj -triple=aarch64-none-freebsd %S/Inputs/abs255.s -o %t255.o
// RUN: llvm-mc -filetype=obj -triple=aarch64-none-freebsd %S/Inputs/abs256.s -o %t256.o
// RUN: llvm-mc -filetype=obj -triple=aarch64-none-freebsd %S/Inputs/abs257.s -o %t257.o

.globl _start
_start:
.data
  .hword foo - . + 0x212057
  .hword foo - . + 0x1fa05a

// Note: If this test fails, it probably happens because of
//       the change of the address of the .data section.
//       You may found the correct address in the aarch64_abs16.s test,
//       if it is already fixed. Then, update addends accordingly.
// RUN: ld.lld -z max-page-size=4096 %t.o %t256.o -o %t2
// RUN: llvm-objdump -s -section=.data %t2 | FileCheck %s

// CHECK: Contents of section .data:
// 202158: S = 0x100, A = 0x212157, P = 0x202158
//         S + A - P = 0xffff
// 212a5a: S = 0x100, A = 0x1fa05a, P = 0x20215a
//         S + A - P = 0x8000
// CHECK-NEXT: 202158 ffff0080

// RUN: not ld.lld -z max-page-size=4096 %t.o %t255.o -o %t2 2>&1 | FileCheck %s --check-prefix=OVERFLOW1
// OVERFLOW1: relocation R_AARCH64_PREL16 out of range: -32769 is not in [-32768, 65535]

// RUN: not ld.lld -z max-page-size=4096 %t.o %t257.o -o %t2 2>&1 | FileCheck %s --check-prefix=OVERFLOW2
// OVERFLOW2: relocation R_AARCH64_PREL16 out of range: 65536 is not in [-32768, 65535]
