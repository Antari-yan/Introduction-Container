#include <stdio.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* ---------------------------------------------------------------------
 * Compile-time detection (build target)
 * ------------------------------------------------------------------- */

/* Detect OS */
#if defined(_WIN32)
    #define BUILD_OS "Windows"
#elif defined(__APPLE__) && defined(__MACH__)
    #define BUILD_OS "macOS"        /* runtime uname reports "Darwin" */
#elif defined(__linux__)
    #define BUILD_OS "Linux"
#elif defined(__FreeBSD__)
    #define BUILD_OS "FreeBSD"
#elif defined(__NetBSD__)
    #define BUILD_OS "NetBSD"
#elif defined(__OpenBSD__)
    #define BUILD_OS "OpenBSD"
#elif defined(__DragonFly__)
    #define BUILD_OS "DragonFly BSD"
#elif defined(__unix__)
    #define BUILD_OS "Unix"
#else
    #define BUILD_OS "UNKNOWN"
#endif

/* Detect architecture */

/* AMD/Intel */
#if defined(__x86_64__) || defined(_M_X64)
    #define BUILD_ARCH "x86_64"
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define BUILD_ARCH "x86_32"

/* ARM */
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define BUILD_ARCH "ARM64"
#elif defined(__ARM_ARCH_8M_MAIN__)
    #define BUILD_ARCH "ARMv8-M Mainline"
#elif defined(__ARM_ARCH_8M_BASE__)
    #define BUILD_ARCH "ARMv8-M Baseline"
#elif defined(__ARM_ARCH_8_2A__)
    #define BUILD_ARCH "ARMv8.2-A"
#elif defined(__ARM_ARCH_8_1A__)
    #define BUILD_ARCH "ARMv8.1-A"
#elif defined(__ARM_ARCH_8A__)
    #define BUILD_ARCH "ARMv8-A"
#elif defined(__ARM_ARCH_7S__)
    #define BUILD_ARCH "ARMv7S"
#elif defined(__ARM_ARCH_7M__)
    #define BUILD_ARCH "ARMv7M"
#elif defined(__ARM_ARCH_7R__)
    #define BUILD_ARCH "ARMv7R"
#elif defined(__ARM_ARCH_7A__)
    #define BUILD_ARCH "ARMv7A"
#elif defined(__ARM_ARCH_7__)
    #define BUILD_ARCH "ARMv7"
#elif defined(__ARM_ARCH_6T2__)
    #define BUILD_ARCH "ARMv6T2"
#elif defined(__ARM_ARCH_6J__)
    #define BUILD_ARCH "ARMv6J"
#elif defined(__ARM_ARCH_6K__)
    #define BUILD_ARCH "ARMv6K"
#elif defined(__ARM_ARCH_6Z__)
    #define BUILD_ARCH "ARMv6Z"
#elif defined(__ARM_ARCH_6ZK__)
    #define BUILD_ARCH "ARMv6ZK"
#elif defined(__ARM_ARCH_6__)
    #define BUILD_ARCH "ARMv6"
#elif defined(__ARM_ARCH_5E__)
    #define BUILD_ARCH "ARMv5E"
#elif defined(__ARM_ARCH_5__)
    #define BUILD_ARCH "ARMv5"
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
    #define BUILD_ARCH "ARMv4T"
#elif defined(__ARM_ARCH_3M__)
    #define BUILD_ARCH "ARMv3M"
#elif defined(__ARM_ARCH_3__)
    #define BUILD_ARCH "ARMv3"
#elif defined(__ARM_ARCH_2__)
    #define BUILD_ARCH "ARMv2"
#elif defined(__ARM_ARCH)
    /* ACLE fallback (GCC >= 4.8, Clang): catches 32-bit ARM versions that have no specific macro above, e.g. ARMv8.3-A and newer. */
    #define BUILD_ARCH "ARMv" STR(__ARM_ARCH)
#elif defined(__arm__) || defined(_M_ARM)
    /* Last resort: toolchain defined neither a version macro nor __ARM_ARCH (common with plain `gcc` on ARM when no -march=/-mcpu= is given). */
    #define BUILD_ARCH "ARM"

/* RISC-V */
#elif defined(__riscv) || defined(__riscv__)
    #if defined(__riscv_xlen) && __riscv_xlen == 64
        #define BUILD_ARCH "RISC-V64"
    #elif defined(__riscv_xlen) && __riscv_xlen == 32
        #define BUILD_ARCH "RISC-V32"
    #else
        #define BUILD_ARCH "RISC-V"
    #endif

/* Other architectures */
#elif defined(__mips64)
    #define BUILD_ARCH "MIPS64"
#elif defined(mips) || defined(__mips__) || defined(__mips)
    #define BUILD_ARCH "MIPS"
#elif defined(__sh__)
    #define BUILD_ARCH "SUPERH"
#elif defined(__powerpc64__) || defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
    #define BUILD_ARCH "POWERPC64"
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
    #define BUILD_ARCH "POWERPC"
#elif defined(__s390x__)
    #define BUILD_ARCH "s390x"
#elif defined(__s390__)
    #define BUILD_ARCH "s390"
#elif defined(__loongarch64)
    #define BUILD_ARCH "LoongArch64"
#elif defined(__loongarch__)
    #define BUILD_ARCH "LoongArch"
#elif defined(__sparc__) || defined(__sparc)
    #define BUILD_ARCH "SPARC"
#elif defined(__m68k__)
    #define BUILD_ARCH "M68K"

/* Catch unknown architectures */
#else
    #define BUILD_ARCH "UNKNOWN"
#endif

/* ---------------------------------------------------------------------
 * Runtime detection (actual running system)
 * ------------------------------------------------------------------- */

/* Keeps the two output lines column-aligned. */
#define LABEL_FMT "%-19s"

#if defined(_WIN32)
    #include <windows.h>

    /* Older MinGW headers may lack these definitions. */
    #ifndef PROCESSOR_ARCHITECTURE_ARM64
        #define PROCESSOR_ARCHITECTURE_ARM64 12
    #endif
    #ifndef IMAGE_FILE_MACHINE_ARM64
        #define IMAGE_FILE_MACHINE_ARM64 0xAA64
    #endif

    typedef BOOL (WINAPI *IsWow64Process2_t)(HANDLE, USHORT *, USHORT *);

    static const char *image_machine_name(USHORT machine) {
        switch (machine) {
            case IMAGE_FILE_MACHINE_AMD64: return "x86_64";
            case IMAGE_FILE_MACHINE_ARM64: return "ARM64";
            case IMAGE_FILE_MACHINE_ARMNT: return "ARM";
            case IMAGE_FILE_MACHINE_I386:  return "x86_32";
            default:                       return "UNKNOWN";
        }
    }

    static void print_runtime_info(void) {
        const char *arch = "UNKNOWN";

        /* Prefer IsWow64Process2 (Windows 10 1709+):
         * Unlike GetNativeSystemInfo, it reports the true native machine.
         * Fall back to GetNativeSystemInfo for older systems. */
        IsWow64Process2_t is_wow64_process2 =
            (IsWow64Process2_t)(void (*)(void))GetProcAddress(
                GetModuleHandleW(L"kernel32.dll"), "IsWow64Process2");

        USHORT process_machine = 0;
        USHORT native_machine  = 0;

        if (is_wow64_process2 != NULL &&
            is_wow64_process2(GetCurrentProcess(),
                              &process_machine, &native_machine)) {
            arch = image_machine_name(native_machine);
        } else {
            SYSTEM_INFO si;
            GetNativeSystemInfo(&si);
            switch (si.wProcessorArchitecture) {
                case PROCESSOR_ARCHITECTURE_AMD64: arch = "x86_64"; break;
                case PROCESSOR_ARCHITECTURE_ARM64: arch = "ARM64";  break;
                case PROCESSOR_ARCHITECTURE_ARM:   arch = "ARM";    break;
                case PROCESSOR_ARCHITECTURE_INTEL: arch = "x86_32"; break;
                default:                                            break;
            }
        }

        printf(LABEL_FMT "%s - %s\n", "Runtime detected:", "Windows", arch);
    }
#else
    #include <sys/utsname.h>

    static void print_runtime_info(void) {
        struct utsname sys_info;

        if (uname(&sys_info) != 0) {
            perror("uname");
            printf(LABEL_FMT "%s - %s\n", "Runtime detected:",
                   "UNKNOWN", "UNKNOWN");
            return;
        }

        /* sysname: "Linux", "Darwin", "FreeBSD", ...
         * machine: "x86_64", "aarch64", "armv7l", "riscv64", "s390x", ...
         * Note: under qemu-user/binfmt emulation, uname reports the emulated (image) architecture, not the host's. */
        printf(LABEL_FMT "%s - %s\n", "Runtime detected:",
               sys_info.sysname, sys_info.machine);
    }
#endif

int main(void) {
    printf("\nC Container\n");
    printf(LABEL_FMT "%s - %s\n", "Compiled for:", BUILD_OS, BUILD_ARCH);

    /* NOTE:
     *   "Runtime detected" can sometimes differ from "Compiled for".
     *   When using qemu-user/binfm uname is derived from the default emulated core,
     *   this can e.g. flattening all 32-bit ARMto a higher version (e.g. compiled for ARMv6ZK, runtime detected as armv7l).
     *   As long as the "Compiled for" output is the intended one, it should work on the actual physical hardware. */
    print_runtime_info();
    return 0;
}
