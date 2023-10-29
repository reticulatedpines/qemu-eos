#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "cpu.h"

#include "../eos.h"
#include "../model_list.h"
#include "logging.h"

// colors for foreground, string format arguments, and errors
#define KFG KBLU
#define KFMT KCYN
#define KERR KRED

// This wrapper is kind of ugly, but it allows us to print most debug messages to stdout
// without using guest code injection. It is also available in pure GDB, but this one is much faster.

// (adapted from nkls' debug_message_helper.c)

void DebugMsg_log(unsigned int cpu_index)
{
    ARMCPU *cpu = ARM_CPU(qemu_get_cpu(cpu_index));

    uint32_t r0 = cpu->env.regs[0]; // id 1
    uint32_t r1 = cpu->env.regs[1]; // id 2
    uint32_t r2 = cpu->env.regs[2]; // format string address
    uint32_t r3 = cpu->env.regs[3]; // first argument
    uint32_t sp = cpu->env.regs[13]; // stack pointer

    if (!qemu_loglevel_mask(EOS_LOG_DEBUGMSG))
        return;

    char out[512];
    int len = 0;
    #define APPEND(fmt,...) do { len += scnprintf(out + len, sizeof(out) - len, fmt, ## __VA_ARGS__); } while(0);

    char format_string[128]; // 128 bytes should be enough for anyone...
    int arg_i = 0;
    char c;
    uint32_t address = r2;
    int spaces = eos_print_location_gdb();
    APPEND("(%02x:%02x) ", r0, r1);
    spaces += len;

    cpu_physical_memory_read(address++, &c, 1);
    while (c)
    {
        // Print until '%' or '\0'
        while (c != '\0' && c != '%')
        {
            if (c == '\n') {
                APPEND("\n");
                for (int i = 0; i < spaces; i++) {
                    APPEND(" ");
                }
            }
            else if (c != '\r') {
                APPEND("%c", c);
            }
            cpu_physical_memory_read(address++, &c, 1);
        }
        
        if (c == '%')
        {
            int n = 1;
            int is_long;
            int format;
            uint32_t arg;
            format_string[0] = '%';
            do
            {
                cpu_physical_memory_read(address++, &c, 1);
                format_string[n++] = c;
            } while (n < COUNT(format_string) && c != '\0' && !strchr("diuoxXsSpc%", c));

            if (c == '\0' || c == 'S')
            {
                APPEND("%s", format_string);
                continue;
            }

            cpu_physical_memory_read(address++, &c, 1);

            // Skip if it fills format buffer or is {long long} or {short} type
            // (I've never seen those in EOS code)
            if (n == COUNT(format_string) || (n >= 4 && (format_string[n-3] == 'h' || format_string[n-3] == 'l')))
            {
                APPEND(KERR);
                APPEND("[FORMATTING_ERROR]");
                APPEND(KRESET);
                APPEND("%.*s", n, format_string);
                continue;
            }

            format_string[n] = '\0';
            format  = format_string[n-1];
            is_long = (strchr("sp%", format) == NULL && (format_string[n-2] == 'l'));

            // Only parse "%s", other variants (eg "%20s") may expect additional parameters
            // or non zero-terminated strings.
            if (format == 's' && strcmp(format_string, "%s") != 0)
            {
                APPEND(KERR);
                APPEND("[FORMATTING_ERROR]");
                APPEND(KRESET);
                APPEND("%s", format_string);
                break;
            }

            // note: all ARM types {int, long, void*} are of size 32 bits,
            //       and {char, short} should be expanded to 32 bits.
            //       the {long long} is not included in this code.
            if (arg_i == 0)
                arg = r3;
            else
                cpu_physical_memory_read(sp + 4 * (arg_i-1), &arg, 4);
            arg_i++;

            //APPEND("[%s|%lX] ", format_string, arg);
            if (format == 's')
            {
                uint32_t sarg = arg;
                char t;
                cpu_physical_memory_read(sarg++, &t, 1);
                while (t != '\0')
                {
                    if (t == '\n') {
                        APPEND("\n[DMSG:%d,%d] ", r0, r1);
                    }
                    else if (t != '\r')
                        APPEND("%c", t);
                    cpu_physical_memory_read(sarg++, &t, 1);
                }
            }
            else if (!is_long)
            {
                APPEND(format_string, (unsigned int)arg);
            }
            else 
            {
                APPEND(format_string, (unsigned long int)arg);
            }

            //APPEND("[%s:%X]",format_string,(uint32_t)arg);
        }
    }
    fprintf(stderr, "%s\n", out);
}
