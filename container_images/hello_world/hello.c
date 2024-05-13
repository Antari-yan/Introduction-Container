#include <stdio.h>
 
int main() {

    #ifdef _WIN32
	    char os[] = "Windows";
	#elif __linux__
	    char os[] = "Linux";
	#elif __unix__
	    char os[] = "Unix";
	#else
	    char os[] = "UNKNOWN";
	#endif

    //Get current architecture
    #if defined(__x86_64__) || defined(_M_X64)
        char arch[] = "x86_64";
    #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        char arch[] = "x86_32";
    #elif defined(__ARM_ARCH_2__)
        char arch[] = "ARMv2";
    #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        char arch[] = "ARMv3";
    #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        char arch[] = "ARMv4T";
    #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        char arch[] = "ARMv5"
    #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
        char arch[] = "ARMv6T2";
    #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        char arch[] = "ARMv6";
    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        char arch[] = "ARMv7";
    #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        char arch[] = "ARMv7A";
    #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        char arch[] = "ARMv7R";
    #elif defined(__ARM_ARCH_7M__)
        char arch[] = "ARMv7M";
    #elif defined(__ARM_ARCH_7S__)
        char arch[] = "ARMv7S";
    #elif defined(__aarch64__) || defined(_M_ARM64)
        char arch[] = "ARM64";
    #elif defined(mips) || defined(__mips__) || defined(__mips)
        char arch[] = "MIPS";
    #elif defined(__sh__)
        char arch[] = "SUPERH";
    #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
        char arch[] = "POWERPC";
    #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        char arch[] = "POWERPC64";
    #elif defined(__sparc__) || defined(__sparc)
        char arch[] = "SPARC";
    #elif defined(__m68k__)
        char arch[] = "M68K";
    #elif defined(__riscv) || defined(__riscv32) || defined(__riscv__) || defined(_riscv)
        char arch[] = "RISC-V";
    #else
        char arch[] = "UNKNOWN";
    #endif

    printf("Go Go C Container. Running on: %s - %s\n", os, arch);
    return 0;
}