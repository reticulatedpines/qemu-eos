/*
 * Copyright (C) 2022 Magic Lantern Team
 *
 * License: GNU GPL, version 2.
 *   See the COPYING file in the top-level directory.
 */
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <glib.h>

#include <qemu-plugin.h>

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

static uint32_t DebugMsg_addr = 0;
void DebugMsg_log(unsigned int cpu_index);
static void vcpu_insn_exec_before(unsigned int cpu_index, void *udata)
{
    DebugMsg_log(cpu_index);
}

static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
    size_t n = qemu_plugin_tb_n_insns(tb);
    size_t i;

    static uint64_t prev_vaddr = 0;
    for (i = 0; i < n; i++) {
        struct qemu_plugin_insn *insn = qemu_plugin_tb_get_insn(tb, i);
        uint64_t vaddr = qemu_plugin_insn_vaddr(insn);

        if (vaddr == DebugMsg_addr
            && prev_vaddr != vaddr 
            && vaddr)
        {
            qemu_plugin_register_vcpu_insn_exec_cb(
                insn, vcpu_insn_exec_before, QEMU_PLUGIN_CB_NO_REGS, NULL);
        }
        prev_vaddr = vaddr;
    }
}

QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
                                           const qemu_info_t *info,
                                           int argc, char **argv)
{
    // how to supply arguments to the plugin:
    // -plugin libmagiclantern.so,arg="beepboop=0",arg="hello=yes"
    // would give you argc == 2, argv[0] == 'beepboop=0', argv[1] == 'hello=yes'
    for(int i = 0; i < argc; i++)
    {
        char *opt = argv[i];
        if (g_str_has_prefix(opt, "debugmsg_addr="))
        {
            DebugMsg_addr = g_ascii_strtoull(opt + 14, NULL, 16);
        }
        else
        {
            fprintf(stderr, "plugin option parsing failed: %s\n", opt);
        }
    }
    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    return 0;
}
