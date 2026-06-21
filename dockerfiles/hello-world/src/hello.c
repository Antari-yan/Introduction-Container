#include <stdio.h>

int main() {

    // Get Operating System
    #ifdef _WIN32
        char os[] = "Windows";
    #elif __linux__
        char os[] = "Linux";
    #elif __unix__
        char os[] = "Unix";
    #else
        char os[] = "UNKNOWN";
    #endif

    // Get current architecture

    /* x86 */
    #if defined(__x86_64__) || defined(_M_X64)
        char arch[] = "x86_64";
    #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        char arch[] = "x86_32";

    /* ARM 32-bit (ordered from oldest to newest; more specific first) */
    #elif defined(__ARM_ARCH_2__)
        char arch[] = "ARMv2";
    #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        char arch[] = "ARMv3";
    #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        char arch[] = "ARMv4T";
    #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        char arch[] = "ARMv5";
    #elif defined(__ARM_ARCH_6T2_)
        char arch[] = "ARMv6T2";
    #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || \
          defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        char arch[] = "ARMv6";
    /* ARMv7 sub-profiles (must appear before the generic ARMv7 catch-all) */
    #elif defined(__ARM_ARCH_7S__)
        char arch[] = "ARMv7S";
    #elif defined(__ARM_ARCH_7M__)
        char arch[] = "ARMv7M";
    #elif defined(__ARM_ARCH_7R__)
        char arch[] = "ARMv7R";
    #elif defined(__ARM_ARCH_7A__)
        char arch[] = "ARMv7A";
    #elif defined(__ARM_ARCH_7__)
        char arch[] = "ARMv7";
    /* ARMv8 32-bit (AArch32 with ARMv8 ISA, e.g. Raspberry Pi 4 running a 32-bit OS) */
    #elif defined(__ARM_ARCH_8A__) || \
          (defined(__ARM_ARCH) && __ARM_ARCH == 8 && !defined(__aarch64__))
        char arch[] = "ARMv8-A (32-bit)";

    /* ARM 64-bit - AArch64 / ARM64/v8 */
    #elif defined(__aarch64__) || defined(_M_ARM64)
        char arch[] = "ARM64/v8 (AArch64)";

    /* MIPS - check 64-bit before 32-bit */
    #elif defined(__mips64) || defined(__mips64__)
        char arch[] = "MIPS64";
    #elif defined(mips) || defined(__mips__) || defined(__mips)
        char arch[] = "MIPS";

    /* RISC-V - distinguish 32 and 64-bit word width */
    #elif (defined(__riscv) || defined(__riscv__)) && \
           defined(__riscv_xlen) && __riscv_xlen == 64
        char arch[] = "RISC-V 64";
    #elif defined(__riscv) || defined(__riscv32) || defined(__riscv__) || defined(_riscv)
        char arch[] = "RISC-V 32";

    /* PowerPC - check 64-bit before 32-bit */
    #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64) || \
          defined(__powerpc64__)
        char arch[] = "POWERPC64";
    #elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || \
          defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
        char arch[] = "POWERPC";

    /* SPARC - check 64-bit before 32-bit */
    #elif defined(__sparcv9) || defined(__sparc_v9__)
        char arch[] = "SPARC64";
    #elif defined(__sparc__) || defined(__sparc)
        char arch[] = "SPARC";

    /* IBM Z (s390) */
    #elif defined(__s390x__)
        char arch[] = "s390x";
    #elif defined(__s390__)
        char arch[] = "s390";

    /* SuperH */
    #elif defined(__sh__)
        char arch[] = "SUPERH";

    /* Motorola 68k */
    #elif defined(__m68k__)
        char arch[] = "M68K";

    /* IA-64 (Itanium) */
    #elif defined(__ia64__) || defined(_M_IA64) || defined(__ia64)
        char arch[] = "IA-64";

    /* DEC Alpha */
    #elif defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
        char arch[] = "Alpha";

    /* LoongArch (mainline Linux kernel since 5.19) */
    #elif defined(__loongarch64)
        char arch[] = "LoongArch64";
    #elif defined(__loongarch__)
        char arch[] = "LoongArch";

    /* WebAssembly */
    #elif defined(__wasm64__)
        char arch[] = "WASM64";
    #elif defined(__wasm32__) || defined(__wasm__)
        char arch[] = "WASM32";

    /* AVR microcontrollers */
    #elif defined(__AVR__)
        char arch[] = "AVR";

    #else
        char arch[] = "UNKNOWN";
    #endif

    printf("C Container. Running on: %s - %s\n", os, arch);
    return 0;
}
