/* Enable interactive debugger */
#define ENABLE_DEBUGGER

/* Enable live emulator tracing*/
#define ENABLE_TRACE

/* Enable post execution CPU dump*/
#define ENABLE_CPU_DUMP

/* Enable post execution memory dump */
#define ENABLE_DUMP_MEM
/* Specify memory dump limit (measured in bytes) */
#define DUMP_MEM_LIM 0x200 >> 1

/* Constraints */
#if defined(ENABLE_DEBUGGER) && !defined(ENABLE_TRACE)
#error Debugger support requires tracing
#endif
