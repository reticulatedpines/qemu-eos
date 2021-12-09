#ifndef QEMU_LOG_H
#define QEMU_LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include "qemu/compiler.h"
#include "qom/cpu.h"
#ifdef NEED_CPU_H
#include "disas/disas.h"
#endif

/* Private global variables, don't use */
extern FILE *qemu_logfile;
extern uint64_t qemu_loglevel;

/* 
 * The new API:
 *
 */

/* Log settings checking macros: */

/* Returns true if qemu_log() will really write somewhere
 */
static inline bool qemu_log_enabled(void)
{
    return qemu_logfile != NULL;
}

#define CPU_LOG_TB_OUT_ASM (1 << 0)
#define CPU_LOG_TB_IN_ASM  (1 << 1)
#define CPU_LOG_TB_OP      (1 << 2)
#define CPU_LOG_TB_OP_OPT  (1 << 3)
#define CPU_LOG_INT        (1 << 4)
#define CPU_LOG_EXEC       (1 << 5)
#define CPU_LOG_PCALL      (1 << 6)
#define CPU_LOG_TB_CPU     (1 << 8)
#define CPU_LOG_RESET      (1 << 9)
#define LOG_UNIMP          (1 << 10)
#define LOG_GUEST_ERROR    (1 << 11)
#define CPU_LOG_MMU        (1 << 12)
#define CPU_LOG_TB_NOCHAIN (1 << 13)

/* various EOS options */
/* some of them are just defined for future use */
#define EOS_LOG_IO         (1LL << 21)
#define EOS_LOG_IO_LOG     (1LL << 22)
#define EOS_LOG_UART       (1LL << 23)
#define EOS_LOG_MPU        (1LL << 24)
#define EOS_LOG_SDCF       (1LL << 25)
#define EOS_LOG_SFLASH     (1LL << 26)
#define EOS_LOG_PFLASH     (1LL << 27)
#define EOS_LOG_DMA        (1LL << 28)
#define EOS_LOG_EDMAC      (1LL << 29)

#define EOS_LOG_VERBOSE    (1LL << 32)
#define EOS_LOG_AUTOEXEC   (1LL << 33)

/* guest memory tracing (logging) */
#define EOS_LOG_RAM_R      (1LL << 40)      /* memory logging backends */
#define EOS_LOG_RAM_W      (1LL << 41)      /* used by other analysis tools, but not printed directly */
#define EOS_LOG_ROM_R      (1LL << 42)      /* these backends have additional overhead on generated code */
#define EOS_LOG_ROM_W      (1LL << 43)      /* so they are only enabled when actually used */
#define EOS_LOG_RAM        (EOS_LOG_RAM_R | EOS_LOG_RAM_W)
#define EOS_LOG_ROM        (EOS_LOG_ROM_R | EOS_LOG_ROM_W)
#define EOS_LOG_MEM_R      (EOS_LOG_RAM_R | EOS_LOG_ROM_R)
#define EOS_LOG_MEM_W      (EOS_LOG_RAM_W | EOS_LOG_ROM_W)
#define EOS_LOG_MEM        (EOS_LOG_RAM   | EOS_LOG_ROM)
#define EOS_PR(mem_flag)  ((mem_flag) << 4) /* memory logging printed in logs (duplicate the above flags) */
                                            /* this will take 4 bits: 40-43 => 44-47 */
#define EOS_LOG_RAM_DBG    (1LL << 48)      /* self-test */
#define EOS_LOG_TASKS      (1LL << 49)      /* task switches */
#define EOS_LOG_DEBUGMSG   (1LL << 50)      /* DebugMsg calls */

/* analysis tools */
#define EOS_LOG_CALLSTACK  (1LL << 51)      /* backend: provide call stack to other tools */
#define EOS_LOG_CALLS      (1LL << 52)      /* log all calls and returns to console */
#define EOS_LOG_IDC        (1LL << 53)      /* export unique calls to IDA */
#define EOS_LOG_RAM_MEMCHK (1LL << 54)      /* like valgrind memcheck */
#define EOS_LOG_RAM_TSKMEM (1LL << 55)      /* check task memory ownership assumptions */
#define EOS_LOG_RAM_SEMCHK (1LL << 56)      /* check semaphore usage (like helgrind) */
#define EOS_LOG_ROMCPY     (1LL << 57)      /* find memory blocks copied from ROM to RAM */
#define EOS_LOG_NO_TAIL_CALLS (1LL << 58)   /* don't attempt to identify tail calls */

/* Returns true if a bit is set in the current loglevel mask
 */
static inline bool qemu_loglevel_mask(uint64_t mask)
{
    return (qemu_loglevel & mask) != 0;
}

/* Logging functions: */

/* main logging function
 */
void GCC_FMT_ATTR(1, 2) qemu_log(const char *fmt, ...);

/* vfprintf-like logging function
 */
static inline void GCC_FMT_ATTR(1, 0)
qemu_log_vprintf(const char *fmt, va_list va)
{
    if (qemu_logfile) {
        vfprintf(qemu_logfile, fmt, va);
    }
}

/* log only if a bit is set on the current loglevel mask
 */
void GCC_FMT_ATTR(2, 3) qemu_log_mask(uint64_t mask, const char *fmt, ...);


/* Special cases: */

/* cpu_dump_state() logging functions: */
/**
 * log_cpu_state:
 * @cpu: The CPU whose state is to be logged.
 * @flags: Flags what to log.
 *
 * Logs the output of cpu_dump_state().
 */
static inline void log_cpu_state(CPUState *cpu, int flags)
{
    if (qemu_log_enabled()) {
        cpu_dump_state(cpu, qemu_logfile, fprintf, flags);
    }
}

/**
 * log_cpu_state_mask:
 * @mask: Mask when to log.
 * @cpu: The CPU whose state is to be logged.
 * @flags: Flags what to log.
 *
 * Logs the output of cpu_dump_state() if loglevel includes @mask.
 */
static inline void log_cpu_state_mask(uint64_t mask, CPUState *cpu, int flags)
{
    if (qemu_loglevel & mask) {
        log_cpu_state(cpu, flags);
    }
}

#ifdef NEED_CPU_H
/* disas() and target_disas() to qemu_logfile: */
static inline void log_target_disas(CPUState *cpu, target_ulong start,
                                    target_ulong len, int flags)
{
    target_disas(qemu_logfile, cpu, start, len, flags);
}

static inline void log_disas(void *code, unsigned long size)
{
    disas(qemu_logfile, code, size);
}

#if defined(CONFIG_USER_ONLY)
/* page_dump() output to the log file: */
static inline void log_page_dump(void)
{
    page_dump(qemu_logfile);
}
#endif
#endif


/* Maintenance: */

/* fflush() the log file */
static inline void qemu_log_flush(void)
{
    fflush(qemu_logfile);
}

/* Close the log file */
static inline void qemu_log_close(void)
{
    if (qemu_logfile) {
        if (qemu_logfile != stderr) {
            fclose(qemu_logfile);
        }
        qemu_logfile = NULL;
    }
}

/* Set up a new log file */
static inline void qemu_log_set_file(FILE *f)
{
    qemu_logfile = f;
}

/* define log items */
typedef struct QEMULogItem {
    uint64_t mask;
    const char *name;
    const char *help;
} QEMULogItem;

extern const QEMULogItem qemu_log_items[];

/* This is the function that actually does the work of
 * changing the log level; it should only be accessed via
 * the qemu_set_log() wrapper.
 */
void do_qemu_set_log(uint64_t log_flags, bool use_own_buffers);

static inline void qemu_set_log(uint64_t log_flags)
{
#ifdef CONFIG_USER_ONLY
    do_qemu_set_log(log_flags, true);
#else
    do_qemu_set_log(log_flags, false);
#endif
}

void qemu_set_log_filename(const char *filename);
uint64_t qemu_str_to_log_mask(const char *str);

/* Print a usage message listing all the valid logging categories
 * to the specified FILE*.
 */
void qemu_print_log_usage(FILE *f);

#endif
