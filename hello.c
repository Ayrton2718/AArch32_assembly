#include <stdio.h>
// #include <stdint.h>

#define __BR  "\n\t"

void wide_add(void)
{
    int rd1, rd2, cpsr;

    // [r5, r4] = [r1, r0] + [r3, r2]
    __asm__ (
        "ldr r0, =0x80000002"__BR
        "ldr r1, =0x00000000"__BR

        "ldr r2, =0x80000001"__BR
        "ldr r3, =0x00000000"__BR

        "adds r4, r2, r0"__BR
        "adc  r5, r1, r3"__BR
        "mov  %[Rd1], r4"__BR
        "mov  %[Rd2], r5"__BR
        "mrs  %[Rcpsr], cpsr"__BR
        : [Rd1] "=r" (rd1), [Rd2] "=r" (rd2), [Rcpsr] "=r" (cpsr)
        :
        : "r0", "r1", "r2", "r3", "r4", "r5"
    );

    printf("r4 = 0x%08X\n", rd1);
    printf("r5 = 0x%08x\n", rd2);
    printf("flag : %s\n", )
}

int routine(void)
{
    int a = 1, b = 2, val;

    __asm__(
        "   mov r0, %[Ra]"__BR
        "   mov r1, %[Rb]"__BR
        "   bl  addvalues"__BR
        "   mov %[Rval], r0"__BR
        "   b   finish"__BR
        "addvalues:"__BR
        "   add r0, r0, r1"__BR
        "   mov pc, lr"__BR
        "finish:"__BR
        : [Rval] "=r" (val)
        : [Ra] "r" (a), [Rb] "r" (b)
        : "r0", "r1", "lr", "pc"
    );

    return val;
}

int sum(int limit)
{
    int i, total=0;

    __asm__ ( 
        "   mov r0, #0"__BR
        "   mov r1, %[Rs1]"__BR
        "loop:"__BR
        "   add r0, r0, r1"__BR
        "   subs r1, r1, #1"__BR
        "   bge loop"__BR
        "   mov %[Rd], r0"__BR
        : [Rd] "=r" (total)
        : [Rs1] "r" (limit)
    );
    return total;
}


int tmp(void)
{
    int val1 = 1;
    int result;

    float a = 2;

    __asm__ (
        "vldr.f32 s0, [%[Ra], #-0]"__BR
        "vmul.f32 s0, s0, s0"__BR

        "mov r0, %[Rs1]"__BR
        "add r0, r0, r0"__BR
        "mov %[Rd], r0"
        : [Rd] "=r" (result)
        : [Rs1] "r" (val1), [Ra] "r" (&a)
        : "r0", "s0"
    );

    return result;
}

int main(void) 
{ 
    printf("tmp = %d\n", tmp());
    printf("sum = %d\n", sum(5));
    printf("sum = %d\n", routine());

    wide_add();

}
