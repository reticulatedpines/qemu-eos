#ifndef QEMU_LOG_H
#define QEMU_LOG_H

/* A small part of this API is split into its own header */
#include "qemu/log-for-trace.h"

/* Private global variable, don't use */
extern FILE *qemu_logfile;

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

/* Returns true if qemu_log() will write somewhere else than stderr
 */
static inline bool qemu_log_separate(void)
{
    return qemu_logfile != NULL && qemu_logfile != stderr;
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
#define CPU_LOG_PAGE       (1 << 14)
/* LOG_TRACE (1 << 15) is defined in log-for-trace.h */
#define CPU_LOG_TB_OP_IND  (1 << 16)
#define CPU_LOG_TB_FPU     (1 << 17)
#define CPU_LOG_PLUGIN     (1 << 18)

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

/* Lock output for a series of related logs.  Since this is not needed
 * for a single qemu_log / qemu_log_mask / qemu_log_mask_and_addr, we
 * assume that qemu_loglevel_mask has already been tested, and that
 * qemu_loglevel is never set when qemu_logfile is unset.
 */

static inline void qemu_log_lock(void)
{
    qemu_flockfile(qemu_logfile);
}

static inline void qemu_log_unlock(void)
{
    qemu_funlockfile(qemu_logfile);
}

/* Logging functions: */

/* vfprintf-like logging function
 */
static inline void GCC_FMT_ATTR(1, 0)
qemu_log_vprintf(const char *fmt, va_list va)
{
    if (qemu_logfile) {
        vfprintf(qemu_logfile, fmt, va);
    }
}

/* log only if a bit is set on the current loglevel mask:
 * @mask: bit to check in the mask
 * @fmt: printf-style format string
 * @args: optional arguments for format string
 */
#define qemu_log_mask(MASK, FMT, ...)                   \
    do {                                                \
        if (unlikely(qemu_loglevel_mask(MASK))) {       \
            qemu_log(FMT, ## __VA_ARGS__);              \
        }                                               \
    } while (0)

/* log only if a bit is set on the current loglevel mask
 * and we are in the address range we care about:
 * @mask: bit to check in the mask
 * @addr: address to check in dfilter
 * @fmt: printf-style format string
 * @args: optional arguments for format string
 */
#define qemu_log_mask_and_addr(MASK, ADDR, FMT, ...)    \
    do {                                                \
        if (unlikely(qemu_loglevel_mask(MASK)) &&       \
                     qemu_log_in_addr_range(ADDR)) {    \
            qemu_log(FMT, ## __VA_ARGS__);              \
        }                                               \
    } while (0)

/* Maintenance: */

/* define log items */
typedef struct QEMULogItem {
    uint64_t mask;
    const char *name;
    const char *help;
} QEMULogItem;

extern const QEMULogItem qemu_log_items[];

void qemu_set_log(uint64_t log_flags);
void qemu_log_needs_buffers(void);
void qemu_set_log_filename(const char *filename, Error **errp);
void qemu_set_dfilter_ranges(const char *ranges, Error **errp);
bool qemu_log_in_addr_range(uint64_t addr);
uint64_t qemu_str_to_log_mask(const char *str);

/* Print a usage message listing all the valid logging categories
 * to the specified FILE*.
 */
void qemu_print_log_usage(FILE *f);

/* fflush() the log file */
void qemu_log_flush(void);
/* Close the log file */
void qemu_log_close(void);

#endif
