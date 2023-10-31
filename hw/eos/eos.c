#include "qemu/osdep.h"
#include "hw/hw.h"
#include "hw/loader.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "exec/exec-all.h"
#include "exec/address-spaces.h"
#include "exec/memory-internal.h"
#include "migration/vmstate.h"
#include "exec/ram_addr.h"
#include "hw/sysbus.h"
#include "ui/console.h"
#include "ui/pixel_ops.h"
#include "hw/display/framebuffer.h"
#include "hw/sd/sd.h"
//#include "chardev/char.h"
//#include "hw/ide/internal.h"
//#include "hw/arm/arm.h"
#include "hw/arm/armv7m.h"
#include "hw/eos/eos.h"
#include "hw/eos/dbi/logging.h"

#include "hw/eos/model_list.h"
#include "hw/eos/eos_ml_helpers.h"
#include "hw/eos/mpu.h"
#include "hw/eos/serial_flash.h"
#include "hw/eos/eos_utils.h"
#include "hw/eos/eos_bufcon_100D.h"
#include "hw/eos/engine.h"

#define IGNORE_CONNECT_POLL

#define DIGIC_TIMER_STEP 0x100
#define DIGIC_TIMER20_MASK (0x000FFFFF & ~(DIGIC_TIMER_STEP-1))
#define DIGIC_TIMER32_MASK (0xFFFFFFFF & ~(DIGIC_TIMER_STEP-1))

static void eos_init_common(void);
static void *eos_init_cpu(void);

#define TYPE_EOS "eos"

static void eos_init(MachineState *machine)
{
    DeviceState *dev;

    MachineClass *mc = MACHINE_GET_CLASS(qdev_get_machine());
    machine->smp.max_cpus = mc->max_cpus;

    // This looks up our TypeInfo by name, TYPE_EOS, then kicks off eos_initfn(),
    // because that's part of eos_info struct.
    dev = qdev_create(NULL, TYPE_EOS);

    // Set the options for the EOS device before realising it, but,
    // don't seem to need any after the refactor?  Not yet well tested.
//    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("arm946-eos"));

    // This triggers calling the realize function
    object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);
}


static void eos_5D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D";
    mc->init = eos_init;
}

static void eos_400D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 400D";
    mc->init = eos_init;
}

static void eos_40D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 40D";
    mc->init = eos_init;
}

static void eos_450D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 450D";
    mc->init = eos_init;
}

static void eos_1000D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 1000D";
    mc->init = eos_init;
}

static void eos_50D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 50D";
    mc->init = eos_init;
//    mc->ignore_memory_transaction_failures = true;
}

static void eos_5D2_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D2";
    mc->init = eos_init;
}

static void eos_500D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 500D";
    mc->init = eos_init;
}

static void eos_550D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 550D";
    mc->init = eos_init;
}

static void eos_7D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 7D";
    mc->init = eos_init;
}

static void eos_60D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 60D";
    mc->init = eos_init;
}

static void eos_600D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 600D";
    mc->init = eos_init;
}

static void eos_1100D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 1100D";
    mc->init = eos_init;
}

static void eos_1200D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 1200D";
    mc->init = eos_init;
}

static void eos_1300D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 1300D";
    mc->init = eos_init;
}

static void eos_A1100_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS A1100";
    mc->init = eos_init;
}

static void eos_5D3_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D3";
    mc->init = eos_init;
}

static void eos_5D3eeko_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D3eeko";
    mc->init = eos_init;
}

static void eos_6D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 6D";
    mc->init = eos_init;
}

static void eos_650D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 650D";
    mc->init = eos_init;
}

static void eos_700D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 700D";
    mc->init = eos_init;
}

static void eos_EOSM_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M";
    mc->init = eos_init;
}

static void eos_EOSM2_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M2";
    mc->init = eos_init;
}

static void eos_100D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 100D";
    mc->init = eos_init;
}

static void eos_70D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 70D";
    mc->init = eos_init;
}

static void eos_80D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 80D";
    mc->init = eos_init;
}

static void eos_750D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 750D";
    mc->init = eos_init;
}

static void eos_760D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 760D";
    mc->init = eos_init;
}

static void eos_7D2_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 7D2";
    mc->init = eos_init;
}

static void eos_7D2S_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 7D2S";
    mc->init = eos_init;
}

static void eos_5D4_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D4";
    mc->init = eos_init;
}

static void eos_5D4AE_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 5D4AE";
    mc->init = eos_init;
}

static void eos_EOSM3_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M3";
    mc->init = eos_init;
}

static void eos_EOSM10_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M10";
    mc->init = eos_init;
}

static void eos_200D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 200D";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_6D2_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 6D2";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_77D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 77D";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_800D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 800D";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_850D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 850D";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_90D_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS 90D";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSM5_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M5";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSM50_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M50";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSM6mk2_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS M6mk2";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSR_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS R";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSR5_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS R5";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSR6_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS R6";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_EOSRP_machine_init(MachineClass *mc)
{
    mc->desc = "Canon EOS RP";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_SX70_machine_init(MachineClass *mc)
{
    mc->desc = "Canon SX70";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

static void eos_SX740_machine_init(MachineClass *mc)
{
    mc->desc = "Canon SX740";
    mc->init = eos_init;
    mc->max_cpus = 2; // wants to be in sync with value in model_list.c
}

// This macro hides three function definitions and a call.
// I don't like this but it's the QOM style I believe.
//
// Triggers each eos_XXX_machine_init function and
// registers the machine types with Qemu.
DEFINE_MACHINE(MODEL_NAME_5D, eos_5D_machine_init)
DEFINE_MACHINE(MODEL_NAME_400D, eos_400D_machine_init)
DEFINE_MACHINE(MODEL_NAME_40D, eos_40D_machine_init)
DEFINE_MACHINE(MODEL_NAME_450D, eos_450D_machine_init)
DEFINE_MACHINE(MODEL_NAME_1000D, eos_1000D_machine_init)
DEFINE_MACHINE(MODEL_NAME_50D, eos_50D_machine_init)
DEFINE_MACHINE(MODEL_NAME_5D2, eos_5D2_machine_init)
DEFINE_MACHINE(MODEL_NAME_500D, eos_500D_machine_init)
DEFINE_MACHINE(MODEL_NAME_550D, eos_550D_machine_init)
DEFINE_MACHINE(MODEL_NAME_7D, eos_7D_machine_init)
DEFINE_MACHINE(MODEL_NAME_60D, eos_60D_machine_init)
DEFINE_MACHINE(MODEL_NAME_600D, eos_600D_machine_init)
DEFINE_MACHINE(MODEL_NAME_1100D, eos_1100D_machine_init)
DEFINE_MACHINE(MODEL_NAME_1200D, eos_1200D_machine_init)
DEFINE_MACHINE(MODEL_NAME_1300D, eos_1300D_machine_init)
DEFINE_MACHINE(MODEL_NAME_A1100, eos_A1100_machine_init)
DEFINE_MACHINE(MODEL_NAME_5D3, eos_5D3_machine_init)
DEFINE_MACHINE(MODEL_NAME_5D3eeko, eos_5D3eeko_machine_init)
DEFINE_MACHINE(MODEL_NAME_6D, eos_6D_machine_init)
DEFINE_MACHINE(MODEL_NAME_650D, eos_650D_machine_init)
DEFINE_MACHINE(MODEL_NAME_700D, eos_700D_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM, eos_EOSM_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM2, eos_EOSM2_machine_init)
DEFINE_MACHINE(MODEL_NAME_100D, eos_100D_machine_init)
DEFINE_MACHINE(MODEL_NAME_70D, eos_70D_machine_init)
DEFINE_MACHINE(MODEL_NAME_80D, eos_80D_machine_init)
DEFINE_MACHINE(MODEL_NAME_750D, eos_750D_machine_init)
DEFINE_MACHINE(MODEL_NAME_760D, eos_760D_machine_init)
DEFINE_MACHINE(MODEL_NAME_7D2, eos_7D2_machine_init)
DEFINE_MACHINE(MODEL_NAME_7D2S, eos_7D2S_machine_init)
DEFINE_MACHINE(MODEL_NAME_5D4, eos_5D4_machine_init)
DEFINE_MACHINE(MODEL_NAME_5D4AE, eos_5D4AE_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM3, eos_EOSM3_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM10, eos_EOSM10_machine_init)
DEFINE_MACHINE(MODEL_NAME_200D, eos_200D_machine_init)
DEFINE_MACHINE(MODEL_NAME_6D2, eos_6D2_machine_init)
DEFINE_MACHINE(MODEL_NAME_77D, eos_77D_machine_init)
DEFINE_MACHINE(MODEL_NAME_800D, eos_800D_machine_init)
DEFINE_MACHINE(MODEL_NAME_850D, eos_850D_machine_init)
DEFINE_MACHINE(MODEL_NAME_90D, eos_90D_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM5, eos_EOSM5_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM50, eos_EOSM50_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSM6mk2, eos_EOSM6mk2_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSR, eos_EOSR_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSR5, eos_EOSR5_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSR6, eos_EOSR6_machine_init)
DEFINE_MACHINE(MODEL_NAME_EOSRP, eos_EOSRP_machine_init)
DEFINE_MACHINE(MODEL_NAME_SX70, eos_SX70_machine_init)
DEFINE_MACHINE(MODEL_NAME_SX740, eos_SX740_machine_init)

EOSState *eos_state;

static void eos_initfn(Object *obj)
{
    eos_state = OBJECT_CHECK(EOSState, (obj), TYPE_EOS);

    sysbus_init_child_obj(obj, "uart", &eos_state->uart, sizeof(eos_state->uart),
                          TYPE_DIGIC_UART);

    // FIXME do we need other devices here?  Most of the work is done
    // in eos_init_common(), which is old style.  But it works, I think.
}

// Takes a model name, e.g., "50D" / MODEL_NAME_50D, finds it in model_list,
// from model_list.c, returns an initialised model
static struct eos_model_desc get_model_from_name(char *name)
{
    struct eos_model_desc model;

    // find the right model, copy it
    const struct eos_model_desc *m = eos_model_list;
    while(m->digic_version) // end item has version == 0
    {
        if (m->name && strcmp(m->name, name) == 0)
        {
            model = *m;
            break;
        }
        m++;
    }

    // step back to find the generic entries for that digic version,
    // generic items have no name
    while(m->name)
    {
        m--;
    }

    // where model is empty, copy fields from generic
    for (int i = 0; i < COUNT(model.params); i++)
    {
        if (model.params[i] == 0)
        {
            // fprintf(stderr, "%s: params[%d] = %x\n", model->name, i, generic->params[i]);
            model.params[i] = m->params[i];
        }
    }
    return model;
}

static void eos_realize(DeviceState *dev, Error **errp)
{
    EOSState *s = OBJECT_CHECK(EOSState, (dev), TYPE_EOS);
    Error *err = NULL;

    MachineClass *mc = MACHINE_GET_CLASS(qdev_get_machine());
    s->model = calloc(sizeof(*s->model), 1);
    *(s->model) = get_model_from_name(mc->name);
    assert(s->model->digic_version != 0); // name not found

    eos_init_common();

    if (err != NULL) {
        error_propagate(errp, err);
        return;
    }
}

static Property eos_properties[] = {
//    DEFINE_PROP_STRING("cpu-type", EOSState, cpu_type),
//    DEFINE_PROP_CHR("chardev", DigicUartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void eos_class_init(ObjectClass *class, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(class);

    dc->realize = eos_realize;
    dc->props = eos_properties;
}

static const TypeInfo eos_info = {
    .name = TYPE_EOS,
    .parent = TYPE_SYS_BUS_DEVICE,
//    .abstract = true,
    .instance_size = sizeof(EOSState),
    .instance_init = eos_initfn,
    .class_init = eos_class_init,
};

static void eos_types(void)
{
    type_register_static(&eos_info);
}

type_init(eos_types)

// Machine class
typedef struct {
    MachineClass parent;
    struct eos_model_desc * model;
} EosMachineClass;

#define EOS_DESC_BASE    "Canon EOS"
#define TYPE_EOS_MACHINE "eos"
#define EOS_MACHINE_GET_CLASS(obj) \
    OBJECT_GET_CLASS(EosMachineClass, obj, TYPE_EOS_MACHINE)
#define EOS_MACHINE_CLASS(klass) \
    OBJECT_CLASS_CHECK(EosMachineClass, klass, TYPE_EOS_MACHINE)

EOSRegionHandler eos_handlers[] =
{
    { "FlashControl", 0xC0000000, 0xC0001FFF, eos_handle_flashctrl, 0 },
  //{ "ROM0",         0xF8000000, 0xFFFFFFFF, eos_handle_rom, 0 },
  //{ "ROM1",         0xF0000000, 0xF7FFFFFF, eos_handle_rom, 1 },
    { "Interrupt",    0xC0200000, 0xC02000FF, eos_handle_intengine_vx, 0 }, // mostly used on D2/3, but also 60D
    { "Interrupt",    0xC0201000, 0xC0201FFF, eos_handle_intengine, 0 },    // <= D5
    { "Interrupt",    0xD4011000, 0xD4011FFF, eos_handle_intengine, 1 },    // D6; first core in D7
    { "Interrupt",    0xD5011000, 0xD5011FFF, eos_handle_intengine, 2 },    // second core in D7
    { "Interrupt",    0xD02C0200, 0xD02C02FF, eos_handle_intengine, 3 },    // 5D3 eeko
    { "Interrupt",    0xC1000000, 0xC100FFFF, eos_handle_intengine_gic, 7 },// D7
    { "Interrupt",    0xD0211000, 0xD0211FFF, eos_handle_intengine, 8 },    /* first core in D8 */
    { "Interrupt",    0xD0231000, 0xD0231FFF, eos_handle_intengine, 9 },    /* D8 CPU1 */
    { "Interrupt",    0xD231A000, 0xD231AFFF, eos_handle_intengine, 4 },    /* DX CPU0 */
    { "Interrupt",    0xD233A000, 0xD233AFFF, eos_handle_intengine, 5 },    /* DX CPU1 */
    { "Multicore",    0xC1100000, 0xC110FFFF, eos_handle_multicore, 7 },    // D7
    { "Timers",       0xC0210000, 0xC0210FFF, eos_handle_timers, 0 },       // DIGIC 4/5/6 countdown timers
    { "Timers",       0xD02C1500, 0xD02C15FF, eos_handle_timers, 2 },       // Eeko countdown timer
    { "Timer",        0xC0242014, 0xC0242014, eos_handle_digic_timer, 0 },
    { "Timer",        0xD400000C, 0xD400000C, eos_handle_digic_timer, 1 },
    { "Timer",        0xD9820014, 0xD9820014, eos_handle_digic_timer, 2 },  // D7: maybe? firmware waits for this register to change
    { "Timer",        0xD020000C, 0xD020000C, eos_handle_digic_timer, 3 },  /* D8 */
    { "UTimer",       0xD4000240, 0xD4000440, eos_handle_utimer, 1 },       // D6: timers 9...16
    { "UTimer",       0xD0200240, 0xD0200440, eos_handle_utimer, 2 },       /* D8: same? */
    { "UTimer",       0xD2300240, 0xD2300440, eos_handle_utimer, 3 },       /* DX: same? */
    { "HPTimer",      0xC0243000, 0xC0243FFF, eos_handle_hptimer, 0 },      // DIGIC 2/3/4/5/6 HPTimers
    { "GPIO",         0xC0220000, 0xC022FFFF, eos_handle_gpio, 0 },
    { "Basic",        0xC0100000, 0xC0100FFF, eos_handle_basic, 0 },
    { "Basic",        0xC0400000, 0xC0400FFF, eos_handle_basic, 1 },
    { "Basic",        0xC0720000, 0xC0720FFF, eos_handle_basic, 2 },
    { "SDIO0",        0xC0C00000, 0xC0C00FFF, eos_handle_sdio, 0 },
    { "SDIO1",        0xC0C10000, 0xC0C10FFF, eos_handle_sdio, 1 },
    { "SDIO2",        0xC0C20000, 0xC0C20FFF, eos_handle_sdio, 2 },
    { "SFIO4",        0xC0C40000, 0xC0C40FFF, eos_handle_sfio, 4 },
    { "SDIO85",       0xC8050000, 0xC8050FFF, eos_handle_sdio, 0x85 },
    { "SDIO86",       0xC8060000, 0xC8060FFF, eos_handle_sdio, 0x86 },
    { "SFIO87",       0xC8070000, 0xC8070FFF, eos_handle_sfio, 0x87 },
    { "SFIO88",       0xC8080000, 0xC8080FFF, eos_handle_sfio, 0x88 },
    { "SDIOM50",      0xD0740000, 0xD0740FFF, eos_handle_sdio, 0x50 },

    { "SDIOR6_1",     0xD2B10000, 0xD2B10FFF, eos_handle_sdio, 0x50 },
    /* below are commented out as qemu implements only one SD controller
     * This should be easy to overcome, but was not needed for now. */
    //{ "SDDMAR6_1",    0xD2B11000, 0xD2B11FFF, eos_handle_sddma_dx, 0x51 },
    //{ "SDIOR6_2",     0xD2B20000, 0xD2B20FFF, eos_handle_sdio, 0x50 },
    //{ "SDDMAR6_2",    0xD2B21000, 0xD2B11FFF, eos_handle_sddma_dx, 0x51 },

    { "ADTGDMA",      0xC0500060, 0xC050007F, eos_handle_adtg_dma, 0 },
    { "UartDMA",      0xC05000C0, 0xC05000DF, eos_handle_uart_dma, 0 },
    { "CFDMA0*",      0xC0500000, 0xC05000FF, eos_handle_cfdma, 0x0F },
    { "CFDMA10",      0xC0510000, 0xC051001F, eos_handle_cfdma, 0x10 },
    { "SDDMA1*",      0xC0510000, 0xC05100FF, eos_handle_sddma, 0x1F },
    { "CFDMA30",      0xC0530000, 0xC053001F, eos_handle_cfdma, 0x30 },
  //{ "SDDMA31",      0xC0530020, 0xC053003F, eos_handle_sddma, 0x31 },
  //{ "SDDMA32",      0xC0530040, 0xC053005F, eos_handle_sddma, 0x32 },
    { "SFDMA33",      0xC0530060, 0xC053007F, eos_handle_sfdma, 0x33 },
    { "SDDMA3*",      0xC0530000, 0xC05300FF, eos_handle_sddma, 0x3F },
    { "SDDMA82*",     0xC8020000, 0xC80200FF, eos_handle_sddma, 0x82F },
    { "SFDMA83*",     0xC8030000, 0xC80300FF, eos_handle_sfdma, 0x83F },
    { "SFDMA84*",     0xC8040000, 0xC80400FF, eos_handle_sfdma, 0x84F },
    { "SDDMAM50",     0xD0710000, 0xD0710FFF, eos_handle_sddma, 0x50 },
    { "CFATA0",       0xC0600000, 0xC060FFFF, eos_handle_cfata, 0 },
    { "CFATA2",       0xC0620000, 0xC062FFFF, eos_handle_cfata, 2 },
    { "CFATA16",      0xC0700000, 0xC070FFFF, eos_handle_cfata, 0x10 },
    { "UART",         0xC0800000, 0xC08000FF, eos_handle_uart, 0 },
    { "UART",         0xC0810000, 0xC08100FF, eos_handle_uart, 1 },
    { "UART",         0xC0270000, 0xC027000F, eos_handle_uart, 2 },
    { "I2C",          0xC0090000, 0xC00900FF, eos_handle_i2c, 0 },
    { "SIO0",         0xC0820000, 0xC08200FF, eos_handle_sio, 0 },
    { "SIO1",         0xC0820100, 0xC08201FF, eos_handle_sio, 1 },
    { "SIO2",         0xC0820200, 0xC08202FF, eos_handle_sio, 2 },
    { "SIO3",         0xC0820300, 0xC08203FF, eos_handle_sio3, 3 },
    { "SIO4",         0xC0820400, 0xC08204FF, eos_handle_sio, 4 },
    { "SIO6",         0xC0820600, 0xC08206FF, eos_handle_sio, 6 },
    { "SIO7",         0xC0820700, 0xC08207FF, eos_handle_sio, 7 },
    { "SIO8",         0xC0820800, 0xC08208FF, eos_handle_sio, 8 },
    { "SIO9",         0xC0820900, 0xC08209FF, eos_handle_sio, 9 },
    { "SIO10",        0xC0820A00, 0xC0820AFF, eos_handle_sio, 10 },
    // Digic 2-5 P&S ADC
    { "ADC",          0xC0900040, 0xC09000D4, eos_handle_adc, 1 },
    { "MREQ",         0xC0203000, 0xC02030FF, eos_handle_mreq, 0 },
    { "DMA1",         0xC0A10000, 0xC0A100FF, eos_handle_dma, 1 },
    { "DMA2",         0xC0A20000, 0xC0A200FF, eos_handle_dma, 2 },
    { "DMA3",         0xC0A30000, 0xC0A300FF, eos_handle_dma, 3 },
    { "DMA4",         0xC0A40000, 0xC0A400FF, eos_handle_dma, 4 },
    { "DMA5",         0xC0A50000, 0xC0A500FF, eos_handle_dma, 5 },
    { "DMA6",         0xC0A60000, 0xC0A600FF, eos_handle_dma, 6 },
    { "DMA7",         0xC0A70000, 0xC0A700FF, eos_handle_dma, 7 },
    { "DMA8",         0xC0A80000, 0xC0A800FF, eos_handle_dma, 8 },
    { "CHSW",         0xC0F05000, 0xC0F05FFF, eos_handle_edmac_chsw, 0 },
    { "EDMAC",        0xC0F04000, 0xC0F04FFF, eos_handle_edmac, 0 },
    { "EDMAC",        0xC0F26000, 0xC0F26FFF, eos_handle_edmac, 1 },
    { "EDMAC",        0xC0F30000, 0xC0F30FFF, eos_handle_edmac, 2 },
    { "PREPRO",       0xC0F08000, 0xC0F08FFF, eos_handle_prepro, 0 },
    { "HEAD",         0xC0F07048, 0xC0F0705B, eos_handle_head, 1 },
    { "HEAD",         0xC0F0705C, 0xC0F0706F, eos_handle_head, 2 },
    { "HEAD",         0xC0F07134, 0xC0F07147, eos_handle_head, 3 },
    { "HEAD",         0xC0F07148, 0xC0F0715B, eos_handle_head, 4 },
    { "CARTRIDGE",    0xC0F24000, 0xC0F24FFF, eos_handle_cartridge, 0 },
    { "ASIF",         0xC0920000, 0xC0920FFF, eos_handle_asif, 4 },
    { "Display",      0xC0F14000, 0xC0F14FFF, eos_handle_display, 0 },
    { "Display",      0xC0F31000, 0xC0F31FFF, eos_handle_display, 1 },
    { "Power",        0xC0F01000, 0xC0F010FF, eos_handle_power_control, 1 },
    { "ADC",          0xD9800000, 0xD9800068, eos_handle_adc, 0 },
    { "JP51",         0xC0E00000, 0xC0E0FFFF, eos_handle_jpcore, 0 },
    { "JP62",         0xC0E10000, 0xC0E1FFFF, eos_handle_jpcore, 1 },
    { "JP57",         0xC0E20000, 0xC0E2FFFF, eos_handle_jpcore, 2 },

    { "EEKO",         0xD02C2000, 0xD02C243F, eos_handle_eeko_comm, 0 },

    // generic catch-all for everything unhandled from this range
    { "ENGIO",        0xC0F00000, 0xC0FFFFFF, eos_handle_engio, 0 },

    { "XDMAC",        0xD6030000, 0xD603002F, eos_handle_xdmac, 0 },
    { "XDMAC",        0xD6030030, 0xD603005F, eos_handle_xdmac, 1 },
    { "XDMAC",        0xD6030060, 0xD603008F, eos_handle_xdmac, 2 },
    { "XDMAC",        0xD6030090, 0xD60300BF, eos_handle_xdmac, 3 },
    { "XDMAC7",       0xC9200000, 0xC920003F, eos_handle_xdmac7, 0 },
    { "XDMAC7",       0xC9200040, 0xC920007F, eos_handle_xdmac7, 1 },
    { "XDMAC7",       0xC9200080, 0xC92000BF, eos_handle_xdmac7, 2 },
  //{ "XDMAC8",       0xC9200D00, 0xC9200D3F, eos_handle_xdmac8, 0 },   /* not implemented */
  //{ "XDMAC8",       0xC9200D40, 0xC9200D7F, eos_handle_xdmac8, 1 },
  //{ "XDMAC8",       0xC9200D80, 0xC9200DBF, eos_handle_xdmac8, 2 },

    { "MEMDIV",       0xD9001600, 0xD900FFFF, eos_handle_memdiv, 0 },

    { "ROMID",        0xBFE01FD0, 0xBFE01FDF, eos_handle_rom_id, 0 },
    { "ROMID",        0xD5100010, 0xD5100010, eos_handle_rom_id, 1 },
    { "ROMID",        0xDFFC4FB0, 0xDFFC4FBF, eos_handle_rom_id, 2 }, // digic X

    { "DIGICX",       0xd2100000, 0xd21fffff, eos_handle_digicX, 0 },
    { "DIGICX",       0xd2210000, 0xd22fffff, eos_handle_digicX, 0 },
    { "DIGICX",       0xd2600000, 0xd26fffff, eos_handle_digicX, 1 },
    { "DIGICX",       0xd2a00000, 0xd2afffff, eos_handle_digicX, 2 },
    { "DIGICX",       0xd2c00000, 0xd2cfffff, eos_handle_digicX, 3 },

    { "DUMMYX",       0xca340000, 0xca34ffff, eos_handle_dummy_dev_digicX, 0 },
    { "DUMMYX",       0xcc340010, 0xcc34ffff, eos_handle_dummy_dev_digicX, 1 },
    { "DUMMYX",       0xce340010, 0xce34ffff, eos_handle_dummy_dev_digicX, 2 },
    { "DUMMYX",       0xd0340010, 0xd034ffff, eos_handle_dummy_dev_digicX, 3 },

    { "DIGIC6",       0xD0000000, 0xDFFFFFFF, eos_handle_digic6, 0 },
    { "DIGIC6",       0xC8100000, 0xC8100FFF, eos_handle_digic6, 1 },

    { "BOOT8",        0xBFE01FC4, 0xBFE01FCF, eos_handle_boot_digic8, 0 },
    { "BOOTX",        0xDFFC4FA0, 0xDFFC4FAF, eos_handle_boot_digicX, 0 },
    { "BOOTX",        0xdffc0000, 0xDFFC48ff, eos_handle_boot_digicX, 0 },

    { "ML helpers",   0xCF123000, 0xCF1230FF, eos_handle_ml_helpers, 0 },
    { "ML helpers",   0xC0123400, 0xC01234FF, eos_handle_ml_helpers, 1 },
};

// io range access
static uint64_t eos_io_read(void *opaque, hwaddr addr, uint32_t size)
{
    addr += MMIO_ADDR;

    uint32_t type = MODE_READ;

    return eos_handler(addr, type, 0);
}

static void eos_io_write(void *opaque, hwaddr addr, uint64_t val, uint32_t size)
{
    addr += MMIO_ADDR;

    uint32_t type = MODE_WRITE;

    eos_handler(addr, type, val);
}

static const MemoryRegionOps mmio_ops = {
    .read = eos_io_read,
    .write = eos_io_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

// fixme: how to get this called?
// no luck with memory_region_rom_device_set_romd...
static uint64_t eos_rom_read(void *opaque, hwaddr addr, uint32_t size)
{
    fprintf(stderr, "ROM read: %x %x\n", (int)addr, (int)size);
    return 0;
}

static void eos_rom_write(void *opaque, hwaddr addr, uint64_t value, uint32_t size)
{
    const char *msg = NULL;
    EOSState *s = (EOSState *)((intptr_t) opaque & ~1);
    uint32_t rom_id = (intptr_t) opaque & 1;
    uint32_t rom_addr = (rom_id) ? ROM1_ADDR : ROM0_ADDR;;
    uint32_t address = rom_addr + addr;

    if (strcmp(s->model->name, MODEL_NAME_1300D) == 0)
    {
        if (address == 0xF8000000 && size == 1 && value == 6)
        {
            // Reading flash model ID?
            // Startup code writes to this address, but expects to read
            // different values: C2 25 39, 20 BB 19 or 01 02 19.
            msg = "Flash model ID?";
            uint32_t model_id = 0x003925C2;
            MEM_WRITE_ROM(address, (uint8_t *) &model_id, 4);
            goto end;
        }
    }

    // flash control registers at 0xffc00aaa and 0xffc00554, see ffdf4e58 and
    // ffdf4dec which are copied to ITCM at 0x1b0 by ffdf5024 depending on
    // operation
    // writes to registers must be ignored to avoid firmware corruption
    // more correct implementation would detect commands and only allow writes
    // when enabled
    // Almost certainly applies to many other cams
    if (strcmp(s->model->name, MODEL_NAME_A1100) == 0) {
        // actual firmware addresses are ffc* as above, but qemu sees 0xf8*
        if(address == 0xf8000aaa ||  address == 0xf8000554)  {
            msg = "Flash control";
            goto end;
        }
    }

    switch(size)
    {
        case 1:
            MEM_WRITE_ROM(address, (uint8_t *) &value, 1);
            break;
        case 2:
            MEM_WRITE_ROM(address, (uint8_t *) &value, 2);
            break;
        case 4:
            MEM_WRITE_ROM(address, (uint8_t *) &value, 4);
            break;
    }

end:;
    // log all ROM writes
    char name[16];
    snprintf(name, sizeof(name), "ROM%d:%d", rom_id, size);
    io_log(name, address, MODE_WRITE, value, 0, msg, 0, 0);

    // make sure we execute the latest code
    // fixme: shouldn't this be handled internally by QEMU?!
    tb_invalidate_phys_addr(&address_space_memory, address,
                            MEMTXATTRS_UNSPECIFIED);
}

static const MemoryRegionOps rom_ops = {
    .read = eos_rom_read,
    .write = eos_rom_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

void eos_mem_read(hwaddr addr, void *buf, int size)
{
    cpu_physical_memory_read(addr, buf, size);

    if (qemu_loglevel_mask(EOS_LOG_MEM_R))
    {
        // fixme: can be optimized
        for (int i = 0; i < size; i++)
        {
            eos_log_mem(addr + i, *(uint8_t*)(buf + i), 1, NOCHK_LOG);
        }
    }
}

void eos_mem_write(hwaddr addr, void *buf, int size)
{
    if (qemu_loglevel_mask(EOS_LOG_MEM_W))
    {
        // fixme: can be optimized
        for (int i = 0; i < size; i ++)
        {
            eos_log_mem(addr + i, *(uint8_t*)(buf + i), 1, 1 | NOCHK_LOG);
        }
    }

    cpu_physical_memory_write(addr, buf, size);
}

const char *eos_get_cam_path(const char *file_rel)
{
    // all files are loaded from $QEMU_EOS_WORKDIR/CAM/
    // or $QEMU_EOS_WORKDIR/CAM/FIRM_VER/ if specified
    static char file[1024];

    if (eos_state->model->firmware_version)
    {
        // load from the firmware version directory, if specified
        snprintf(file, sizeof(file), "%s/%s/%d/%s", eos_state->workdir,
                 eos_state->model->name, eos_state->model->firmware_version, file_rel);
    }
    else
    {
        // or from the camera directory, if no firmware version is specified
        snprintf(file, sizeof(file), "%s/%s/%s", eos_state->workdir,
                 eos_state->model->name, file_rel);
    }

    return file;
}

static int check_rom_mirroring(void *buf, int size, int full_size)
{
    if (size / 2 && memcmp(buf, buf + size / 2, size / 2) == 0)
    {
        // identical halves? check recursively to find the lowest size with unique data
        if (!check_rom_mirroring(buf, size / 2, full_size))
        {
            fprintf(stderr, "[EOS] mirrored data; unique 0x%X bytes repeated 0x%X times\n", size / 2, full_size / (size / 2));
            assert(0);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void eos_load_image(const char *file_rel, int offset, int max_size, uint32_t addr, int swap_endian)
{
    const char *file = eos_get_cam_path(file_rel);

    int size = get_image_size(file);
    if (size < 0)
    {
        fprintf(stderr, "%s: file not found '%s'\n", __func__, file);
        abort();
    }

    if (size < offset) {
        fprintf(stderr, "%s: file '%s': offset '%d' is too big\n", __func__,
            file, offset);
        abort();
    }

    fprintf(stderr, "[EOS] loading '%s'", file);

    uint8_t *buf = malloc(size);
    if (!buf)
    {
        fprintf(stderr, "%s: malloc error loading '%s'\n", __func__, file);
        abort();
    }

    if (load_image_size(file, buf, size) != size)
    {
        fprintf(stderr, "%s: error loading '%s'\n", __func__, file);
        abort();
    }

    if ((max_size > 0) && (size + offset != max_size)) {
        fprintf(stderr, " (expected size 0x%08X, got 0x%08X)", max_size, size);
    }

    size = size - offset;

    if ((max_size > 0) && (size > max_size)) {
        size = max_size;
    }
    
    fprintf(stderr, " to 0x%08X-0x%08X", addr, size + addr - 1);
    
    if (offset)
    {
        fprintf(stderr, " (offset 0x%X)", offset);
    }
    
    fprintf(stderr, "\n");

    check_rom_mirroring(buf + offset, size, size);

    if (swap_endian) {
        reverse_bytes_order(buf + offset, size);
    }

    MEM_WRITE_ROM(addr, buf + offset, size);

    free(buf);
}

static int cfdma_read_data(CFState *cf);
static int cfdma_write_data(CFState *cf);
static void cfdma_trigger_interrupt(void);


static void eos_interrupt_timer_body(void)
{
    {
        uint32_t pos;

        // don't loop thread if cpu stopped in gdb
        if (eos_state->cpu0 && cpu_is_stopped(CPU(eos_state->cpu0))) {
            return;
        }

        if (eos_state->cpu1 && cpu_is_stopped(CPU(eos_state->cpu1))) {
            return;
        }

        eos_state->digic_timer20 += DIGIC_TIMER_STEP;
        eos_state->digic_timer20 &= DIGIC_TIMER20_MASK;
        eos_state->digic_timer32 += DIGIC_TIMER_STEP;
        eos_state->digic_timer32 &= DIGIC_TIMER32_MASK;

        for (pos = 0; pos < COUNT(eos_state->timer_enabled); pos++)
        {
            if (eos_state->timer_enabled[pos])
            {
                eos_state->timer_current_value[pos] += DIGIC_TIMER_STEP;

                if (eos_state->timer_current_value[pos] > eos_state->timer_reload_value[pos])
                {
                    eos_state->timer_current_value[pos] = 0;
                }
            }
        }

        // go through all interrupts and check if they are pending/scheduled
        for(pos = INT_ENTRIES-1; pos > 0; pos--)
        {
            // it is pending, so trigger int and set to 0
            if(eos_state->irq_schedule[pos] == 1)
            {
                //g_assert(pos != 80);
                // wait, its not enabled. keep it pending
                if(eos_state->irq_enabled[pos] && !eos_state->irq_id)
                {
                    // timer interrupt will re-fire periodically
                    if(pos == TIMER_INTERRUPT)
                    {
                        if (qemu_loglevel_mask(CPU_LOG_INT) &&
                            qemu_loglevel_mask(EOS_LOG_VERBOSE))
                        {
                            // timer interrupt, quiet
                            fprintf(stderr, "[EOS] trigger int 0x%02X (delayed)\n", pos);    // quiet
                        }
                        eos_state->irq_schedule[pos] = eos_state->timer_reload_value[DRYOS_TIMER_ID] >> 8;
                    }
                    else
                    {
                        if (qemu_loglevel_mask(CPU_LOG_INT)) {
                            fprintf(stderr, "[EOS] trigger int 0x%02X (delayed)\n", pos);
                        }
                        eos_state->irq_schedule[pos] = 0;
                    }

                    eos_state->irq_id = pos;
                    eos_state->irq_enabled[eos_state->irq_id] = 0;

                    cpu_interrupt(CPU(CURRENT_CPU), CPU_INTERRUPT_HARD);
                }
            }

            // still counting down?
            if(eos_state->irq_schedule[pos] > 1)
            {
                eos_state->irq_schedule[pos]--;
            }
        }

        // check all UTimers
        int utimer_interrupts[COUNT(eos_state->UTimers)] = {
            0x0E, 0x1E, 0x2E, 0x3E, 0x4E, 0x5E, 0x6E, 0x7E,
        };

        for (int id = 0; id < COUNT(eos_state->UTimers); id++)
        {
            if (eos_state->UTimers[id].active
                && eos_state->UTimers[id].output_compare
                == eos_state->digic_timer32)
            {
                if (qemu_loglevel_mask(EOS_LOG_IO)) {
                    fprintf(stderr, "[TIMER] Firing UTimer #%d\n", id);
                }
                eos_state->UTimers[id].triggered = 1;
                eos_trigger_int(utimer_interrupts[id], 0);
            }
        }

        // also check all HPTimers
        // note: we can trigger multiple HPTimers on a single interrupt
        int trigger_hptimers[64] = {0};
        int hptimer_interrupts[COUNT(eos_state->HPTimers)] = {
            0x18, 0x1A, 0x1C, 0x1E, 0, 0,
            HPTIMER_INTERRUPT, HPTIMER_INTERRUPT, HPTIMER_INTERRUPT, HPTIMER_INTERRUPT,
            HPTIMER_INTERRUPT, HPTIMER_INTERRUPT, HPTIMER_INTERRUPT, HPTIMER_INTERRUPT,
        };
        
        for (pos = 0; pos < COUNT(eos_state->HPTimers); pos++)
        {
            if (eos_state->HPTimers[pos].active
                && eos_state->HPTimers[pos].output_compare
                == eos_state->digic_timer20)
            {
                if (qemu_loglevel_mask(EOS_LOG_IO)) {
                    fprintf(stderr, "[HPTimer] Firing HPTimer #%d\n", pos);
                }
                eos_state->HPTimers[pos].triggered = 1;
                int interrupt = hptimer_interrupts[pos];
                assert(interrupt > 0);
                assert(interrupt < COUNT(trigger_hptimers));
                trigger_hptimers[hptimer_interrupts[pos]] = 1;
            }
        }

        for (int i = 1; i < COUNT(trigger_hptimers); i++)
        {
            if (trigger_hptimers[i])
            {
                eos_trigger_int(i, 0);
            }
        }

        if (eos_state->cf.dma_read_request)
        {
            eos_state->cf.dma_read_request = cfdma_read_data(&eos_state->cf);
        }

        if (eos_state->cf.dma_write_request)
        {
            eos_state->cf.dma_write_request = cfdma_write_data(&eos_state->cf);
        }
        
        if (eos_state->cf.pending_interrupt && eos_state->cf.interrupt_enabled == 1)
        {
            cfdma_trigger_interrupt();
            eos_state->cf.pending_interrupt = 0;
        }
    }

}

static void eos_interrupt_timer_cb(void *parm)
{
    eos_interrupt_timer_body();
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer_mod_anticipate_ns(eos_state->interrupt_timer, now + DIGIC_TIMER_STEP*1000); // DIGIC_TIMER_STEP us
}




// FRAMEBUFFER & DISPLAY (move to separate file?)




// precompute some parts of YUV to RGB computations
static int yuv2rgb_RV[256];
static int yuv2rgb_GU[256];
static int yuv2rgb_GV[256];
static int yuv2rgb_BU[256];

// http://www.martinreddy.net/gfx/faqs/colorconv.faq
// BT 601:
// R'= Y' + 0.000*U' + 1.403*V'
// G'= Y' - 0.344*U' - 0.714*V'
// B'= Y' + 1.773*U' + 0.000*V'
// 
// BT 709:
// R'= Y' + 0.0000*Cb + 1.5701*Cr
// G'= Y' - 0.1870*Cb - 0.4664*Cr
// B'= Y' - 1.8556*Cb + 0.0000*Cr


static void precompute_yuv2rgb(int rec709)
{
    int u, v;
    if (rec709)
    {
        //
        // R = *Y + 1608 * V / 1024;
        // G = *Y -  191 * U / 1024 - 478 * V / 1024;
        // B = *Y + 1900 * U / 1024;
        //
        for (u = 0; u < 256; u++)
        {
            int8_t U = u;
            yuv2rgb_GU[u] = (-191 * U) >> 10;
            yuv2rgb_BU[u] = (1900 * U) >> 10;
        }

        for (v = 0; v < 256; v++)
        {
            int8_t V = v;
            yuv2rgb_RV[v] = (1608 * V) >> 10;
            yuv2rgb_GV[v] = (-478 * V) >> 10;
        }
    }
    else // REC 601
    {
        //
        // R = *Y + ((1437 * V) >> 10);
        // G = *Y -  ((352 * U) >> 10) - ((731 * V) >> 10);
        // B = *Y + ((1812 * U) >> 10);
        //
        for (u = 0; u < 256; u++)
        {
            int8_t U = u;
            yuv2rgb_GU[u] = (-352 * U) >> 10;
            yuv2rgb_BU[u] = (1812 * U) >> 10;
        }

        for (v = 0; v < 256; v++)
        {
            int8_t V = v;
            yuv2rgb_RV[v] = (1437 * V) >> 10;
            yuv2rgb_GV[v] = (-731 * V) >> 10;
        }
    }
}

#define COERCE(x,lo,hi) MAX(MIN((x),(hi)),(lo))

static void yuv2rgb(int Y, int U, int V, int* R, int* G, int* B)
{
    const int v_and_ff = V & 0xFF;
    const int u_and_ff = U & 0xFF;
    int v = Y + yuv2rgb_RV[v_and_ff];
    *R = COERCE(v, 0, 255);
    v = Y + yuv2rgb_GU[u_and_ff] + yuv2rgb_GV[v_and_ff];
    *G = COERCE(v, 0, 255);
    v = Y + yuv2rgb_BU[u_and_ff];
    *B = COERCE(v, 0, 255);
}

#define UYVY_GET_Y1(uyvy) (((uyvy) >>  8) & 0xFF)
#define UYVY_GET_Y2(uyvy) (((uyvy) >> 24) & 0xFF)
#define UYVY_GET_U(uyvy)  (((uyvy)      ) & 0xFF)
#define UYVY_GET_V(uyvy)  (((uyvy) >> 16) & 0xFF)


// todo: supoort other bith depths

typedef void (*drawfn_bmp_yuv)(void *, uint8_t *, const uint8_t *, const uint8_t*, int, int, int);

static void draw_line8_32(void *opaque,
                uint8_t *d, const uint8_t *s, int width, int deststep)
{
    uint8_t v, r, g, b;
    EOSState* ws = (EOSState*) opaque;
    
    do {
        v = ldub_p((void *) s);
        if (v)
        {
            r = ws->disp.palette_8bit[v].R;
            g = ws->disp.palette_8bit[v].G;
            b = ws->disp.palette_8bit[v].B;
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
        }
        else
        {
            r = g = b = 128;
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
        }
        s ++;
        d += 4;
    } while (-- width != 0);
}

static uint8_t clip_yuv(int v) {
    if (v<0) return 0;
    if (v>255) return 255;
    return v;
}

static uint8_t yuv_to_r(uint8_t y, int8_t v) {
    return clip_yuv(((y<<12) +          v*5743 + 2048)>>12);
}

static uint8_t yuv_to_g(uint8_t y, int8_t u, int8_t v) {
    return clip_yuv(((y<<12) - u*1411 - v*2925 + 2048)>>12);
}

static uint8_t yuv_to_b(uint8_t y, int8_t u) {
    return clip_yuv(((y<<12) + u*7258          + 2048)>>12);
}

static void draw_line_YUV8B_32(void *opaque,
                uint8_t *d, const uint8_t *s, int width, int deststep)
{
    uint8_t v, r, g, b;
    width = width / 2;
    do {
        v = ldub_p((void *) s);
        if (v)
        {
            uint8_t p2 = s[2] - 0x80;
            uint8_t p0 = s[0] - 0x80;
            r = yuv_to_r(s[1],p2);
            g = yuv_to_g(s[1],p0,p2);
            b = yuv_to_b(s[1],p0);
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
                        
            d += 4;
            r = yuv_to_r(s[3],p2);
            g = yuv_to_g(s[3],p0,p2);
            b = yuv_to_b(s[3],p0);
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
            d += 4;
        }
        else
        {
            r = g = b = 128;
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
            d += 4;
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
            d += 4;
        }
        s +=4;
    } while (-- width != 0);
}

static void draw_line4_32(void *opaque,
                uint8_t *d, const uint8_t *s, int width, int deststep)
{
    uint8_t v, r, g, b;
    EOSState* ws = (EOSState*) opaque;
    void * d0 = d;
    
    do {
        v = ldub_p((void *) s);
        v = ((uintptr_t)d/4 % 2) ? (v >> 4) & 0xF : v & 0xF;
        
        r = ws->disp.palette_4bit[v].R;
        g = ws->disp.palette_4bit[v].G;
        b = ws->disp.palette_4bit[v].B;
        ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);

        if ((uintptr_t)d/4 % 2) s ++;
        d += 4;
    } while (-- width != 0);

    if (ws->model->digic_version < 4)
    {
        // double each line
        memcpy(d, d0, (void *) d - d0);
    }
}

static void draw_line8_32_bmp_yuv(void *opaque,
                uint8_t *d, const uint8_t *bmp, const uint8_t *yuv, int width, int deststep, int yuvstep)
{
    EOSState* ws = (EOSState*) opaque;
    
    do {
        uint8_t v = ldub_p((void *) bmp);
        int r = ws->disp.palette_8bit[v].R;
        int g = ws->disp.palette_8bit[v].G;
        int b = ws->disp.palette_8bit[v].B;
        int o = ws->disp.palette_8bit[v].opacity;

        if (o == 3)
        {
            // opaque
            ((uint32_t *) d)[0] = rgb_to_pixel32(r, g, b);
        }
        else
        {
            // some sort of transparency
            uint32_t uyvy =  ldl_p((void*)((uintptr_t)yuv & ~3));
            int Y = (uintptr_t)yuv & 3 ? UYVY_GET_Y2(uyvy) : UYVY_GET_Y1(uyvy);
            int U = UYVY_GET_U(uyvy);
            int V = UYVY_GET_V(uyvy);
            int R, G, B;
            yuv2rgb(Y, U, V, &R, &G, &B);
            
            if (o == 0 && r == 255 && g == 255 && b == 255)
            {
                // fully transparent (nothing to do)
                // is this an edge case in Canon firmware?
            }
            else
            {
                // assume semi-transparent
                // 5D3: 2 bits, 4 transparency levels
                // 0 = somewhat transparent, 3 = opaque,
                // 0 with Y=255 (R=G=B=255) = fully transparent
                // black image => bitmap overlay looks as if it were opaque
                // (colors not altered, except for the fully transparent special case)
                // white image => bitmap overlay washed out (except for o=3)
                // red image => bitmap overlay hue-shifted (except for o=3)
                // the following is just a rough approximation that looks reasonably well
                int bmp_weight = (o & 3) + 2;
                int yuv_weight = 5 - bmp_weight;
                R = (R * yuv_weight + r * bmp_weight) / 5;
                G = (G * yuv_weight + g * bmp_weight) / 5;
                B = (B * yuv_weight + b * bmp_weight) / 5;
            }

            ((uint32_t *) d)[0] = rgb_to_pixel32(R, G, B);
        }
        bmp ++;
        yuv += yuvstep;
        d += 4;
    } while (-- width != 0);
}

// similar to QEMU's framebuffer_update_display, but with two image planes
// main plane is BMP (8-bit, same size as output), secondary plane is YUV (scaled to match the BMP one)
static void framebuffer_update_display_bmp_yuv(
    DisplaySurface *ds,
    MemoryRegion *address_space,
    hwaddr base_bmp,
    hwaddr base_yuv,
    int cols, // Width in pixels.
    int rows_bmp, // Height in pixels.
    int rows_yuv,
    int src_width_bmp, // Length of source line, in bytes.
    int src_width_yuv,
    int dest_row_pitch, // Bytes between adjacent horizontal output pixels.
    int dest_col_pitch, // Bytes between adjacent vertical output pixels.
    int invalidate, // nonzero to redraw the whole image.
    drawfn_bmp_yuv fn,
    void *opaque,
    int *first_row, // Input and output.
    int *last_row) // Output only
{
    hwaddr src_len_bmp;
    hwaddr src_len_yuv;
    uint8_t *dest;
    uint8_t *src_bmp;
    uint8_t *src_yuv;
    uint8_t *src_base_bmp;
    uint8_t *src_base_yuv;
    int first, last = 0;
    //int dirty;
    int i;
    ram_addr_t addr_bmp;
    ram_addr_t addr_yuv;
    ram_addr_t addr_base_yuv;
    MemoryRegionSection mem_section_bmp;
    MemoryRegionSection mem_section_yuv;
    MemoryRegion *mem_bmp;
    MemoryRegion *mem_yuv;

    i = *first_row;
    *first_row = -1;
    src_len_bmp = src_width_bmp * rows_bmp;
    src_len_yuv = src_width_yuv * rows_yuv;

    mem_section_bmp = memory_region_find(address_space, base_bmp, src_len_bmp);
    mem_section_yuv = memory_region_find(address_space, base_yuv, src_len_yuv);
    mem_bmp = mem_section_bmp.mr;
    mem_yuv = mem_section_yuv.mr;
    if (int128_get64(mem_section_bmp.size) != src_len_bmp ||
            !memory_region_is_ram(mem_section_bmp.mr)) {
        goto out;
    }
    assert(mem_bmp);
    assert(mem_section_bmp.offset_within_address_space == base_bmp);

    if (int128_get64(mem_section_yuv.size) != src_len_yuv ||
            !memory_region_is_ram(mem_section_yuv.mr)) {
        goto out;
    }
    assert(mem_yuv);
    assert(mem_section_yuv.offset_within_address_space == base_yuv);

    //memory_region_sync_dirty_bitmap(mem_bmp);
    //memory_region_sync_dirty_bitmap(mem_yuv);
    src_base_bmp = cpu_physical_memory_map(base_bmp, &src_len_bmp, 0);
    src_base_yuv = cpu_physical_memory_map(base_yuv, &src_len_yuv, 0);
    // If we can't map the framebuffer then bail.  We could try hard
    //   but it's not really worth it as dirty flag tracking will probably
    //   already have failed above.
    if (!src_base_bmp)
        goto out;
    if (!src_base_yuv)
        goto out;
    if (src_len_bmp != src_width_bmp * rows_bmp) {
        cpu_physical_memory_unmap(src_base_bmp, src_len_bmp, 0, 0);
        goto out;
    }
    if (src_len_yuv != src_width_yuv * rows_yuv) {
        cpu_physical_memory_unmap(src_base_yuv, src_len_yuv, 0, 0);
        goto out;
    }
    src_bmp = src_base_bmp;
    src_yuv = src_base_yuv;
    dest = surface_data(ds);
    if (dest_col_pitch < 0)
        dest -= dest_col_pitch * (cols - 1);
    if (dest_row_pitch < 0) {
        dest -= dest_row_pitch * (rows_bmp - 1);
    }
    first = -1;
    addr_bmp = mem_section_bmp.offset_within_region;
    addr_yuv = mem_section_yuv.offset_within_region;
    addr_base_yuv = addr_yuv;

    int j = i * rows_yuv / rows_bmp;
    addr_bmp += i * src_width_bmp;
    src_bmp += i * src_width_bmp;
    addr_yuv = addr_base_yuv + j * src_width_yuv;
    src_yuv = src_base_yuv + j * src_width_yuv;
    dest += i * dest_row_pitch;
    
    // fixme: only works for integer factors
    int src_yuv_pitch = src_width_yuv / cols;

    for (; i < rows_bmp; i++) {
        //dirty = memory_region_get_dirty(mem_bmp, addr_bmp, src_width_bmp,
        //                                     DIRTY_MEMORY_VGA);
        //dirty |= memory_region_get_dirty(mem_yuv, addr_yuv, src_width_yuv,
        //                                     DIRTY_MEMORY_VGA);
        //if (dirty || invalidate) {
        if (invalidate) {
            fn(opaque, dest, src_bmp, src_yuv, cols, dest_col_pitch, src_yuv_pitch);
            if (first == -1)
                first = i;
            last = i;
        }

        int j = i * rows_yuv / rows_bmp;
        addr_bmp += src_width_bmp;
        src_bmp += src_width_bmp;
        addr_yuv = addr_base_yuv + j * src_width_yuv;
        src_yuv = src_base_yuv + j * src_width_yuv;
        dest += dest_row_pitch;
    }
    cpu_physical_memory_unmap(src_base_bmp, src_len_bmp, 0, 0);
    cpu_physical_memory_unmap(src_base_yuv, src_len_yuv, 0, 0);
    if (first < 0) {
        goto out;
    }
    memory_region_reset_dirty(mem_bmp, mem_section_bmp.offset_within_region, src_len_bmp,
                              DIRTY_MEMORY_VGA);
    memory_region_reset_dirty(mem_yuv, mem_section_yuv.offset_within_region, src_len_yuv,
                              DIRTY_MEMORY_VGA);
    *first_row = first;
    *last_row = last;
out:
    memory_region_unref(mem_bmp);
    memory_region_unref(mem_yuv);
}

static void eos_update_display(void *parm)
{
    EOSState *s = (EOSState *)parm;

    DisplaySurface *surface = qemu_console_surface(s->disp.con);
    
    // these numbers need double-checking
    //                  LCD    HDMI-1080   HDMI-480    SD-PAL      SD-NTSC
    int widths[]      = {   720,   960,        720,        720,        720     };
    int heights[]     = {   480,   540,        480,        576,        480     };
    int yuv_widths[]  = {   720,  1920,        720,        540,        540     };
    int yuv_heights[] = {   480,  1080,        480,        572,        480     };
    
    int width       = widths     [s->disp.type];
    int height      = heights    [s->disp.type];
    int yuv_width   = yuv_widths [s->disp.type];
    int yuv_height  = yuv_heights[s->disp.type];

    int height_multiplier = 1;
    int out_height = height;

    // VxWorks models and some PowerShots have 720x240 screens stretched vertically
    if (s->model->digic_version < 4 || strcmp(s->model->name, MODEL_NAME_A1100) == 0)
    {
        height_multiplier = 2;
        height /= height_multiplier;
        assert(out_height == height * height_multiplier);
    }

    if (strcmp(s->model->name, MODEL_NAME_1100D) == 0)
    {
        // half-size YUV buffer
        yuv_height /= 2;
    }

    if (s->disp.width && s->disp.height)
    {
        // did we manage to get them from registers? override the above stuff
        width = s->disp.width;
        out_height = height = s->disp.height;
    }

    if (width != surface_width(surface) || out_height != surface_height(surface))
    {
        qemu_console_resize(s->disp.con, width, out_height);
        surface = qemu_console_surface(s->disp.con);
        s->disp.invalidate = 1;
    }

    if (s->card_led)
    {
        // fixme: inefficient (redraws non-dirty areas)
        s->disp.invalidate = 1;
    }

    int first, last;
    
    first = 0;
    int linesize = surface_stride(surface) * height_multiplier;

    if (s->disp.is_4bit)
    {
        // bootloader config, 4 bpp
        uint64_t size = height * width / 2;
        MemoryRegionSection section = memory_region_find(
            s->system_mem,
            s->disp.bmp_vram ? s->disp.bmp_vram : 0x08000000,
            size
        );
        framebuffer_update_display(
            surface,
            &section,
            width, height,
            s->disp.bmp_pitch, linesize, 0, 1,
            draw_line4_32, s,
            &first, &last
        );
    }
    else if (s->disp.img_vram)
    {
        framebuffer_update_display_bmp_yuv(
            surface,
            s->system_mem,
            s->disp.bmp_vram,
            s->disp.img_vram,
            width, height, yuv_height,
            s->disp.bmp_pitch, yuv_width*2, linesize, 0, s->disp.invalidate,
            draw_line8_32_bmp_yuv, s,
            &first, &last
        );
    }
    else if (strcmp(s->model->name, MODEL_NAME_EOSM3) == 0 ||
             strcmp(s->model->name, MODEL_NAME_EOSM50) == 0 ||
             strcmp(s->model->name, MODEL_NAME_EOSRP) == 0)
    {
        uint64_t size = height * s->disp.bmp_pitch;
        MemoryRegionSection section = memory_region_find(
            s->system_mem,
            s->disp.bmp_vram ? s->disp.bmp_vram : 0x08000000,
            size
        );
        framebuffer_update_display(
            surface,
            &section,
            width , height,
            s->disp.bmp_pitch, linesize, 0, 1,
            draw_line_YUV8B_32, s,
            &first, &last
        );
    }
    else
    {
        uint64_t size = height * width;
        MemoryRegionSection section = memory_region_find(
            s->system_mem,
            s->disp.bmp_vram ? s->disp.bmp_vram : 0x08000000,
            size
        );
        framebuffer_update_display(
            surface,
            &section,
            width, height,
            s->disp.bmp_pitch, linesize, 0, 1,
            draw_line8_32, s,
            &first, &last
        );
    }

    if (s->card_led)
    {
        // draw the LED at the bottom-right corner of the screen
        int x_led = width - 8;
        int y_led = out_height - 8;
        uint8_t * dest = surface_data(surface);
        for (int dy = -5; dy <= 5; dy++)
        {
            for (int dx = -5; dx <= 5; dx++)
            {
                int r2 = dx*dx + dy*dy;
                if (r2 < 5*5)
                {
                    ((uint32_t *) dest)[x_led+dx + width*(y_led+dy)] =
                        (r2 >= 4*4)         ? rgb_to_pixel32(0, 0, 0)       :
                        (s->card_led == 1)  ? rgb_to_pixel32(255, 0, 0)     :
                                              rgb_to_pixel32(64, 64, 64) ;
                }
            }
        }
    }

    first *= height_multiplier;
    last *= height_multiplier;

    if (first >= 0) {
        dpy_gfx_update(s->disp.con, 0, first, width, last - first + 1);
    }
    
    s->disp.invalidate = 0;
}

static void eos_invalidate_display(void *parm)
{
    EOSState *s = (EOSState *)parm;
    s->disp.invalidate = 1;
}

static const GraphicHwOps eos_display_ops = {
    .invalidate  = eos_invalidate_display,
    .gfx_update  = eos_update_display,
};

static void eos_key_event(void *parm, int keycode)
{
    // keys sent to guest machine
    mpu_send_keypress(keycode);
    //s->keyb.buf[(s->keyb.tail++) & 15] = keycode;
}

//
// UART code taken from hw/char/digic-uart.c
// (sorry, couldn't figure out how to reuse it...)
//

enum {
    ST_RX_RDY = (1 << 0),
    ST_TX_RDY = (1 << 1),
};

static int eos_uart_can_rx(void *opaque)
{
    DigicUartState *s = opaque;
    EOSState *es = (EOSState *)(opaque - offsetof(EOSState, uart));

    // fixme: make it work without this workaround
    if (es->uart_just_received)
    {
        // extra wait states to work around buffer issues; test code follows
        // ( sleep 5; echo "akashimorino" ) | ./run_canon_fw.sh 750D -serial stdio
        es->uart_just_received--;
        return 0;
    }

    return !(s->reg_st & ST_RX_RDY);
}

static void eos_uart_rx(void *opaque, const uint8_t *buf, int size)
{
    DigicUartState *s = opaque;

    assert(size == 1);
    assert(eos_uart_can_rx(opaque));

    s->reg_st |= ST_RX_RDY;
    s->reg_rx = *buf;

    assert(!eos_uart_can_rx(opaque));

    assert(eos_state->model->uart_rx_interrupt);
    // fixme: why it locks up without a delay?
    eos_trigger_int(eos_state->model->uart_rx_interrupt, 10);
}

static void eos_uart_event(void *opaque, int event)
{
}

static void eos_uart_reset(DigicUartState *s)
{
    s->reg_rx = 0;
    s->reg_st = ST_TX_RDY;
}

// EOS CPU SETUP 

static void eos_init_rom(int rom_id, MemoryRegion *rom, uint32_t rom_addr, uint32_t rom_size, uint64_t rom_limit)
{
    char name[32];
    uint32_t start_addr = rom_addr;

    // main ROM
    sprintf(name, "eos.rom%d", rom_id);
    // fixme: not a very nice way to pass both EOSState * and rom ID
    //
    // SJE FIXME: now that eos_state is global, the following can be simplified,
    // when the points of use for rom_ops_arg are identified.
    void *rom_ops_arg = (void *)((uintptr_t)eos_state | rom_id);
    memory_region_init_rom_device(rom, NULL, &rom_ops, rom_ops_arg, name, rom_size, &error_abort);
    memory_region_add_subregion(eos_state->system_mem, start_addr, rom);

    // mirrored ROMs (aliases: changing one will update all others)
    for(uint64_t offset = start_addr + rom_size; offset < rom_limit; offset += rom_size)
    {
        sprintf(name, "eos.rom%d_mirror", rom_id);
        MemoryRegion *image = g_new(MemoryRegion, 1);
        memory_region_init_alias(image, NULL, name, rom, 0x00000000, rom_size);
        memory_region_add_subregion(eos_state->system_mem, offset, image);
    }
}

static void *eos_init_cpu(void)
{
    eos_state->workdir = getenv("QEMU_EOS_WORKDIR");
    if (!eos_state->workdir)
        eos_state->workdir = ".";

    const char *cpu_name = 
        (eos_state->model->digic_version <= 4) ? "arm946-eos-arm-cpu"    :  // apparently the same for DIGIC 2, 3 and 4
        (eos_state->model->digic_version == 5) ? "arm946-eos5-arm-cpu"   :  // minor differences
        (eos_state->model->digic_version == 7) ? "cortex-a9-eos-arm-cpu" :  // dual core
        (eos_state->model->digic_version == 8) ? "cortex-a9-eos-arm-cpu" :  // same as D7?
        (eos_state->model->digic_version == 10) ? "cortex-a9-eos-arm-cpu":  // same as D7?
        (eos_state->model->digic_version >= 6) ? "cortex-r4-eos-arm-cpu" :  // also used on Eeko (fake version 50)
                                                 "arm946-arm-cpu";          // unused here
    
    eos_state->cpu0 = ARM_CPU(cpu_create(cpu_name));
    assert(eos_state->cpu0);

    if (eos_state->model->max_cpus > 1)
    {
        eos_state->cpu1 = ARM_CPU(cpu_create(cpu_name));
        assert(eos_state->cpu1);
        CPU(eos_state->cpu1)->halted = 0;
        assert(eos_state->model->max_cpus < 3); // not yet supported, none exist yet
    }

    eos_state->verbosity = 0xFFFFFFFF;
    eos_state->tio_rxbyte = 0x100;

    eos_state->system_mem = get_system_memory();

    if (ATCM_SIZE)
    {
        memory_region_init_ram(&eos_state->tcm_code, NULL, "eos.tcm_code", ATCM_SIZE, &error_abort);
        memory_region_add_subregion(eos_state->system_mem, ATCM_ADDR, &eos_state->tcm_code);
    }

    if (BTCM_SIZE)
    {
        memory_region_init_ram(&eos_state->tcm_data, NULL, "eos.tcm_data", BTCM_SIZE, &error_abort);
        memory_region_add_subregion(eos_state->system_mem, BTCM_ADDR, &eos_state->tcm_data);
    }

    // set up RAM, cached and uncached
    // main RAM starts at 0
    // the ATCM overlaps the RAM (so far all models);
    // the BTCM may or may not overlap the uncached RAM (model-dependent)
    assert(ATCM_ADDR == 0);
    
    if (BTCM_ADDR == CACHING_BIT)
    {
        // not sure what to do if both TCMs overlap the RAM,
        // when they have different sizes
        assert(ATCM_SIZE == BTCM_SIZE);
    }

    assert(RAM_SIZE);
    memory_region_init_ram(&eos_state->ram, NULL, "eos.ram", RAM_SIZE - ATCM_SIZE, &error_abort);
    memory_region_add_subregion(eos_state->system_mem, 0 + ATCM_SIZE, &eos_state->ram);
    memory_region_init_alias(&eos_state->ram_uncached, NULL, "eos.ram_uncached", &eos_state->ram, 0x00000000, RAM_SIZE - ATCM_SIZE);
    memory_region_add_subregion(eos_state->system_mem, CACHING_BIT + ATCM_SIZE, &eos_state->ram_uncached);
    
    if (ATCM_SIZE && (BTCM_ADDR != CACHING_BIT))
    {
        // I believe there's a small section of RAM visible only as uncacheable (to be tested)
        memory_region_init_ram(&eos_state->ram_uncached0, NULL, "eos.ram_uncached0", ATCM_SIZE, &error_abort);
        memory_region_add_subregion(eos_state->system_mem, CACHING_BIT, &eos_state->ram_uncached0);
    }
    
    char ram_region_name[32] = "";
    for(size_t i = 0;
        i < ram_extra_array_len;
        i++)
    {
        sprintf(ram_region_name, "eos.ram_extra_%ld", i);
        if (eos_state->model->ram_extra_addr[i] != 0)
        {
            memory_region_init_ram(&eos_state->ram_extra[i], NULL, ram_region_name,
                                   eos_state->model->ram_extra_size[i], &error_abort);
            memory_region_add_subregion(eos_state->system_mem, eos_state->model->ram_extra_addr[i],
                                        &eos_state->ram_extra[i]);
        }
    }

    // set up ROM0
    if (ROM0_SIZE)
    {
        eos_init_rom(0, &eos_state->rom0, ROM0_ADDR, ROM0_SIZE, ROM1_ADDR);
    }

    if (ROM1_SIZE)
    {
        eos_init_rom(1, &eos_state->rom1, ROM1_ADDR, ROM1_SIZE, 0x100000000);
    }

    //memory_region_init_ram(&s->rom1, "eos.rom", 0x10000000, &error_abort);
    //memory_region_add_subregion(s->system_mem, 0xF0000000, &s->rom1);

    // set up io space
    memory_region_init_io(&eos_state->mmio, NULL, &mmio_ops, eos_state, "eos.mmio", MMIO_SIZE);
    memory_region_add_subregion(eos_state->system_mem, MMIO_ADDR, &eos_state->mmio);

    // ROMState *rom0 = eos_rom_register(0xF8000000, NULL, "ROM1", ROM1_SIZE,
    //                            NULL,
    //                            0x100, 0x100, 32,
    //                            0, 0, 0, 0, 0);

    vmstate_register_ram_global(&eos_state->ram);

    // initialize RTC registers, compatible to Ricoh R2062 etc
    eos_state->rtc.transfer_format = RTC_INACTIVE;
    eos_state->rtc.regs[0x00] = 0x00;   /* second (BCD) */
    eos_state->rtc.regs[0x01] = 0x15;   /* minute (BCD) */
    eos_state->rtc.regs[0x02] = 0x12;   /* hour (BCD) */
    eos_state->rtc.regs[0x03] = 0x01;   /* day of week */
    eos_state->rtc.regs[0x04] = 0x30;   /* day (BCD) */
    eos_state->rtc.regs[0x05] = 0x09;   /* month (BCD), century bit (2000) */
    eos_state->rtc.regs[0x06] = 0x17;   /* year (BCD since 2000) */
    eos_state->rtc.regs[0x07] = eos_state->model->rtc_time_correct;     /* Oscillation Adjustment Register */
    eos_state->rtc.regs[0x0E] = 0x20;                           /* Control Register 1: 24-hour mode, no alarms */
    eos_state->rtc.regs[0x0F] = eos_state->model->rtc_control_reg_2;    /* Control Register 2: XST (model-specific), PON... */

    if (strcmp(eos_state->model->name, MODEL_NAME_400D) == 0)
    {
        /* fixme: RTC protocol unknown, but returning 0xC everywhere brings the GUI */
        eos_state->rtc.regs[0x00] = 0xC;
    }
    else if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0)
    {
        // values observed on D10, in response to command 2 which has similar code
        // See A1100 ffc2f1d4 D10 ff845960
        eos_state->rtc.regs[0x00] = 0x23;   /* year (BCD since 2000) */
        eos_state->rtc.regs[0x01] = 0x01;   /* month (BCD) */
        eos_state->rtc.regs[0x02] = 0x31;   /* day (BCD) */
        eos_state->rtc.regs[0x03] = 0x02;   /* unk */
        eos_state->rtc.regs[0x04] = 0x58;   /* hour (BCD) | 0x40 */
        eos_state->rtc.regs[0x05] = 0x28;   /* minute (BCD) */
        eos_state->rtc.regs[0x06] = 0x00;   /* second (BCD) */
        eos_state->rtc.regs[0x06] = 0; // unused
        eos_state->rtc.regs[0x07] = 0;
        eos_state->rtc.regs[0x0E] = 0;
        eos_state->rtc.regs[0x0F] = 0;
    }

    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    eos_state->interrupt_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, eos_interrupt_timer_cb, eos_state);
    timer_mod_anticipate_ns(eos_state->interrupt_timer, now + DIGIC_TIMER_STEP * 1000);

    /* init display */
    precompute_yuv2rgb(1);
    eos_state->disp.con = graphic_console_init(NULL, 0, &eos_display_ops, eos_state);
    eos_state->disp.bmp_pitch = 960; /* fixme: get it from registers */

    /* init keys */
    qemu_add_kbd_event_handler(eos_key_event, eos_state);

    /* start logging (see the dbi subdir) */
    eos_logging_init();

    return eos_state;
}

#if 1
static void patch_200D(void)
{
    uint8_t *buf = NULL;
    buf = calloc(0xce24, 1);
    address_space_read(&address_space_memory, 0xe11c11c0,
                       MEMTXATTRS_UNSPECIFIED, buf,
                       0xce24);
    address_space_write(&address_space_memory, 0xdf002800,
                        MEMTXATTRS_UNSPECIFIED, buf,
                        0xce24);
    return;
}
#endif

static void patch_EOSM3(void)
{
    fprintf(stderr, "Patching 0xFCC637A8 (enabling TIO)\n");
    uint32_t one = 1;
    MEM_WRITE_ROM(0xFCC637A8, (uint8_t*) &one, 4);

    fprintf(stderr, "Patching 0xFC0F45B8 (InitExDrivers, locks up)\n");
    uint32_t bx_lr = 0x4770;
    MEM_WRITE_ROM(0xFC0F45B8, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC1F455C (DcdcDrv, assert i2c)\n");
    MEM_WRITE_ROM(0xFC1F455C, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC4FE848 (JpCore, assert)\n");
    MEM_WRITE_ROM(0xFC4FE848, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC284B20 and 0xFC284B80 (Hdmi_comm, assert)\n");
    MEM_WRITE_ROM(0xFC284B20, (uint8_t*) &bx_lr, 2);
    MEM_WRITE_ROM(0xFC284B80, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC10C1A4 and 0xFC10C2B2 (DefMarkManLeo, assert)\n");
    MEM_WRITE_ROM(0xFC10C1A4, (uint8_t*) &bx_lr, 2);
    MEM_WRITE_ROM(0xFC10C2B2, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC2A0F38 (SoundTsk, assert)\n");
    MEM_WRITE_ROM(0xFC2A0F38, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC1847E4 (MechaCPUFirmTransfer, assert)\n");
    MEM_WRITE_ROM(0xFC1847E4, (uint8_t*) &bx_lr, 2);

    fprintf(stderr, "Patching 0xFC3F1110 (MZRM send and wait)\n");
    uint32_t pldrstr = 0x62A06920;
    uint32_t pmovs_r0_1 = 0x2001;
    MEM_WRITE_ROM(0xFC3F1110, (uint8_t*) &pldrstr, 4);
    MEM_WRITE_ROM(0xFC3F1114, (uint8_t*) &pmovs_r0_1, 2);
    
    fprintf(stderr, "Patching 0xFC3F1178 (MZRM wait)\n");
    uint32_t pdword0x0 = 0x00000000;
    MEM_WRITE_ROM(0xFC3F1178, (uint8_t*) &pdword0x0, 4);
    
    fprintf(stderr, "Patching 0xFC10A312 (BmpDDev.c:554 assert)\n");
    MEM_WRITE_ROM(0xFC10A312, (uint8_t*) &pdword0x0, 4);    
}

static void patch_EOSM10(void)
{
    fprintf(stderr, "Patching 0xFCE642A8 (enabling TIO)\n");
    uint32_t one = 1;
    MEM_WRITE_ROM(0xFCE642A8, (uint8_t*) &one, 4);
}

static void patch_EOSM5(void)
{
    /* 0x4060, in the block copied from 0xE001B2E4 to 0x4000 */
    fprintf(stderr, "Patching 0xE001B2E4+0x60 (enabling TIO on DryOs #1)\n");
    uint32_t one = 1;
    MEM_WRITE_ROM(0xE001B2E4+0x60, (uint8_t*) &one, 4);

    /* 0x8098, in the block copied from 0xE115CF88 to 0x8000 */
    fprintf(stderr, "Patching 0xE115CF88+0x98 (enabling TIO on DryOs #2)\n");
    MEM_WRITE_ROM(0xE115CF88+0x98, (uint8_t*) &one, 4);
}


static void eos_init_common(void)
{
    eos_init_cpu();

    /* hijack machine option "firmware" to pass command-line parameters */
    /* e.g. ./run_canon_fw 5D3,firmware="113;boot=1" */
    /* fixme: better way to expose machine-specific options? */
    QemuOpts *machine_opts = qemu_get_machine_opts();
    const char *options = qemu_opt_get(machine_opts, "firmware");
    if (options)
    {
        /* first numeric argument is firmware version (e.g. 113) */
        eos_state->model->firmware_version = atoi(options);
    }

    /* populate ROM0 */
    if (ROM0_SIZE)
    {
        eos_load_image("ROM0.BIN", 0, ROM0_SIZE, ROM0_ADDR, 0);
    }
    
    /* populate ROM1 */
    if (ROM1_SIZE)
    {
        eos_load_image("ROM1.BIN", 0, ROM1_SIZE, ROM1_ADDR, 0);
    }

    /* init SD card */
    DriveInfo *di;
    /* FIXME use a qdev drive property instead of drive_get_next() */
    di = drive_get_next(IF_SD);
    eos_state->sd.card = sd_init(di ? blk_by_legacy_dinfo(di) : NULL, false);
    if (!eos_state->sd.card) {
        fprintf(stderr, "SD init failed\n");
        exit(1);
    }
    
    /* init CF card */
    DriveInfo *dj;
    dj = drive_get_next(IF_IDE);
    if (!dj) {
        fprintf(stderr, "CF init failed\n");
        exit(1);
    }

    ide_bus_new(&eos_state->cf.bus, sizeof(eos_state->cf.bus), DEVICE(eos_state), 0, 2);
    ide_init2(&eos_state->cf.bus, eos_state->interrupt);
    ide_create_drive(&eos_state->cf.bus, 0, dj);
    eos_state->cf.bus.ifs[0].drive_kind = IDE_CFATA;


    /* nkls: init SF */
    if (eos_state->model->serial_flash_size)
    {
        const char *sf_filename = eos_get_cam_path("SFDATA.BIN");
        eos_state->sf = serial_flash_init(sf_filename, eos_state->model->serial_flash_size);
    }
    
    /* init UART */
    qdev_prop_set_chr(DEVICE(&eos_state->uart), "chardev", serial_hd(0));
    qemu_chr_fe_set_handlers(&eos_state->uart.chr, eos_uart_can_rx, eos_uart_rx,
                             eos_uart_event, NULL, &eos_state->uart, NULL, true);
    eos_uart_reset(&eos_state->uart);

    /* init MPU */
    mpu_spells_init();

    /* init image processing engine */
    engine_init();

    if (strcmp(eos_state->model->name, MODEL_NAME_7D) == 0)
    {
        fprintf(stderr, "Disabling IPC (boot flag 0x24)\n");
        uint32_t flag = 0;
        MEM_WRITE_ROM(eos_state->model->bootflags_addr + 0x24, (uint8_t *)&flag, 4);
    }
    
#if 1
    if (strcmp(eos_state->model->name, MODEL_NAME_200D) == 0)
    {
        printf(" ==== patching 200D\n");
        patch_200D();
    }
#endif

    if (strcmp(eos_state->model->name, MODEL_NAME_EOSM3) == 0)
    {
        patch_EOSM3();
    }

    if (strcmp(eos_state->model->name, MODEL_NAME_EOSM10) == 0)
    {
        patch_EOSM10();
    }

    if (strcmp(eos_state->model->name, MODEL_NAME_EOSM5) == 0)
    {
        patch_EOSM5();
    }

    if (eos_state->model->digic_version == 6)
    {
        /* fixme: initial PC should probably be set in cpu.c */
        /* note: DIGIC 4 and 5 start execution at FFFF0000 (hivecs) */
        cpu_physical_memory_read(0xfc000000, &(eos_state->cpu0->env.regs[15]), 4);
        fprintf(stderr, "Start address: 0x%08X\n", eos_state->cpu0->env.regs[15]);
    }

    if (eos_state->model->digic_version == 7 ||
        eos_state->model->digic_version == 8 ||
        eos_state->model->digic_version == 10)
    {
        /* fixme: what configures this address as startup? */
        eos_state->cpu0->env.regs[15] = 0xE0000000;
        eos_state->cpu1->env.regs[15] = 0xE0000000;
        fprintf(stderr, "Start address: 0x%08X\n", eos_state->cpu0->env.regs[15]);
    }

    if (strcmp(eos_state->model->name, MODEL_NAME_5D3eeko) == 0)
    {
        /* see EekoBltDmac calls (5D3 1.1.3)
         * EekoBltDmac(0x0, 0xd0288000, 0xff99541c, 0x6b8c,  0xff508e78, 0x0), from ff508f30
         * EekoBltDmac(0x0, 0x1e80000,  0xff99c164, 0x10e8,  0xff508e78, 0x0), from ff508fd0
         * EekoBltDmac(0x0, 0x1e00000,  0xff8bf888, 0x4ef14, 0xff217de8, 0x0), from ff217e34
         * EekoBltDmac(0x0, 0xd0280000, 0xff99bfa8, 0x1bc,   0xff508e78, 0x0), from ff508fd0
         */
        
        /* all dumps must be made before starting the Eeko core, but after the above copy calls
         * 5D3 1.1.3: 0xFF508F78 (right before writing 7 to C022320C) */
        eos_load_image("D0288000.DMP", 0, 0x008000, 0,          0);
        eos_load_image("D0280000.DMP", 0, 0x004000, 0x40000000, 0);
        eos_load_image("1E00000.DMP",  0, 0x120000, 0x1E00000,  0); /* overlaps 2 regions */
        eos_load_image("1F20000.DMP",  0, 0x020000, 0x1F20000,  0); /* non-shareable device */
        eos_state->cpu0->env.regs[15] = 0;
        eos_state->cpu0->env.thumb = 1;
    }
    
    if (options)
    {
        /* fixme: reinventing the wheel */
        if (strstr(options, "boot=1") || strstr(options, "boot=0"))
        {
            /* change the boot flag */
            uint32_t flag = strstr(options, "boot=1") ? 0xFFFFFFFF : 0;
            fprintf(stderr, "Setting BOOTDISK flag at %X to %X\n",
                    eos_state->model->bootflags_addr + 4,
                    flag);
            MEM_WRITE_ROM(eos_state->model->bootflags_addr + 4, (uint8_t*) &flag, 4);
        }
    }
}

char *eos_get_current_task_name(void)
{
    if (!eos_state->model->current_task_addr)
    {
        return 0;
    }
    
    uint32_t current_task_ptr;
    uint32_t current_task[0x50/4];
    static char task_name[100];
    cpu_physical_memory_read(eos_state->model->current_task_addr, &current_task_ptr, 4);
    if (current_task_ptr && (current_task_ptr & ~eos_state->model->caching_bit) < 0x1000000)
    {
        assert(eos_state->model->current_task_name_offs);
        int off = eos_state->model->current_task_name_offs;
        cpu_physical_memory_read(current_task_ptr, current_task, sizeof(current_task));
        cpu_physical_memory_read(current_task[off], task_name, sizeof(task_name));

        /* task name must be printable */
        for (char *p = task_name; *p; p++)
        {
            unsigned char c = *p;
            if (c < 32 || c > 127)
            {
                return 0;
            }
        }

        return task_name;
    }
    
    return 0;
}

uint8_t eos_get_current_task_id(void)
{
    if (!eos_state->model->current_task_addr)
    {
        return 0xFF;
    }
    
    uint32_t current_task_ptr;
    uint32_t current_task;
    cpu_physical_memory_read(eos_state->model->current_task_addr, &current_task_ptr, 4);
    if (current_task_ptr && current_task_ptr < 0x1000000)
    {
        if (eos_state->model->digic_version < 4)
        {
            // VxWorks doesn't seem to use unique task IDs
            // workaround: assume unique current_task_ptr, and assign our own ID
            static uint32_t current_tasks[0xFE]; // two special IDs, don't use them
            for (int i = 0; i < 0xFE; i++)
            {
                if (current_tasks[i] == current_task_ptr)
                {
                    return i;
                }

                if (current_tasks[i] == 0)
                {
                    current_tasks[i] = current_task_ptr;
                    return i;
                }
            }

            // let's hope we don't require so many unique tasks
            assert(0);
        }

        cpu_physical_memory_read(current_task_ptr + 0x40, &current_task, 4);
        return current_task & 0xFF;
    }
    
    return 0xFF;
}

int eos_get_current_task_stack(uint32_t *top, uint32_t *bottom)
{
    if (!eos_state->model->current_task_addr)
    {
        return 0;
    }
    
    uint32_t current_task_ptr;
    uint32_t current_stack[2];
    cpu_physical_memory_read(eos_state->model->current_task_addr, &current_task_ptr, 4);
    if (current_task_ptr && current_task_ptr < 0x1000000)
    {
        cpu_physical_memory_read(current_task_ptr + 0x1c, current_stack, 8);
        *bottom = current_stack[0];
        *top = current_stack[0] + current_stack[1];
        return 1;
    }
    
    return 0;
}

/* return 1 if you want this address or group to be highlighted */
static int io_highlight(unsigned int address, unsigned char type, const char *module_name, const char *task_name)
{
    /* example: highlight RTC and UART messages (requires -d io,uart) */
    return
        strcmp(module_name, "RTC") == 0 ||
        strcmp(module_name, "UART") == 0 ||
        strcmp(module_name, "UartDMA") == 0 ;

    /* example: highlight JPCORE/JP51/JPwhatever and EDMAC */
    return
        strncmp(module_name, "JP", 2) == 0 ||
        strncmp(module_name, "EDMAC", 5) == 0 ;

    return 1;
}

void io_log(const char *module_name, unsigned int address, unsigned char type, unsigned int in_value, unsigned int out_value, const char *msg, intptr_t msg_arg1, intptr_t msg_arg2)
{
    /* log I/O when "-d io" is specified on the command line */
    if (!qemu_loglevel_mask(EOS_LOG_IO) && !(type & FORCE_LOG)) {
        return;
    }

    /* on multicore machines, print CPU index for each message */
    char cpu_name[] = "[CPU0] ";
    if (CPU_NEXT(first_cpu)) {
        cpu_name[4] = '0' + current_cpu->cpu_index;
    } else {
        cpu_name[0] = 0;
    }
    
    unsigned int pc = CURRENT_CPU->env.regs[15];
    unsigned int lr = CURRENT_CPU->env.regs[14];
    if (!module_name) module_name = "???";
    if (!msg) msg = "???";
    
    char *task_name = eos_get_current_task_name();

    const char *color = io_highlight(address, type, module_name, task_name)
        ? (type & MODE_WRITE ? KYLW : KLGRN) : "";

    char mod_name[24];
    char mod_name_and_pc[72];
    int indent = eos_callstack_get_indent();
    char indent_spaces[] = "                ";
    indent_spaces[MIN(indent, sizeof(indent_spaces)-1)] = 0;
    snprintf(mod_name, sizeof(mod_name), "%s[%s]", indent_spaces, module_name);

    if (task_name)
    {
        /* trim task name or pad with spaces for alignment */
        /* note: task_name size is 100 chars, in get_current_task_name */
        task_name[MAX(5, 15 - (int)strlen(mod_name))] = 0;
        char spaces[] = "           ";
        spaces[MAX(0, 15 - (int)strlen(mod_name) - (int)strlen(task_name))] = 0;
        snprintf(mod_name_and_pc, sizeof(mod_name_and_pc), "%s%s%s%s at %s:%08X:%08X", color, mod_name, KRESET, spaces, task_name, pc, lr);
    }
    else
    {
        snprintf(mod_name_and_pc, sizeof(mod_name_and_pc), "%-14s at 0x%08X:%08X", mod_name, pc, lr);
    }
    
    /* description may have two optional integer arguments */
    char desc[200];
    snprintf(desc, sizeof(desc), msg, msg_arg1, msg_arg2);

    fprintf(stderr, "%s%-28s %s[0x%08X] %s 0x%-8X"KRESET"%s%s\n",
        cpu_name,
        mod_name_and_pc,
        color,
        address,
        type & MODE_WRITE ? "<-" : "->",
        type & MODE_WRITE ? in_value : out_value,
        strlen(msg) ? ": " : "",
        desc
    );

    /* print MMIO reads as dm-spy entries (dm-spy-experiments branch)
     * so you can cross-check the values with the ones from actual hardware */
    if (qemu_loglevel_mask(EOS_LOG_IO_LOG))
    {
        uint32_t insn;
        cpu_physical_memory_read(pc, &insn, sizeof(insn));
        uint32_t reg = (insn >> 12) & 0xF;
        fprintf(stderr, "    { 0x%-8X, \"0x%X\", R(%d), mmio_log }, "
                        "    /* %s %s at %s:%X (0x%x)*/\n",
            pc + 4, address, reg,
            mod_name, desc, task_name, pc, out_value
        );
    }
}


/** HANDLES **/

unsigned int eos_default_handle(unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int data = 0;

    if (type & MODE_WRITE)
        cpu_physical_memory_write(address, &value, 4);
    else
        cpu_physical_memory_read(address, &data, 4);

    /* do not log ram/flash access */
    if(((address & 0xF0000000) == 0) || ((address & 0xF0000000) == 0xF0000000) || ((address & 0xF0000000) == 0x40000000))
    {
        return data;
    }

    if (type & MODE_WRITE)
    {
        if(eos_state->verbosity & 1)
        {
            io_log("MEM", address, type, value, 0, "", 0, 0);
        }
    }
    else
    {
        static int mod = 0;
        mod++;
        mod %= 2;

        if(mod)
        {
            data = ~data;
        }
        if(eos_state->verbosity & 1)
        {
            io_log("MEM", address, type, 0, data, "", 0, 0);
        }
    }
    return data;
}

EOSRegionHandler *eos_find_handler(unsigned int address)
{
    int pos = 0;
    for(pos = 0; pos < sizeof(eos_handlers) / sizeof(eos_handlers[0]); pos++)
    {
        if(eos_handlers[pos].start <= address && eos_handlers[pos].end >= address)
        {
            return &eos_handlers[pos];
        }
    }

    return NULL;
}

unsigned int eos_handler(unsigned int address, unsigned char type, unsigned int value)
{
    EOSRegionHandler *handler = eos_find_handler(address);

    if(handler)
    {
        return handler->handle(handler->parm, address, type, value);
    }
    else
    {
        io_log("*unk*", address, type, value, 0, 0, 0, 0);
    }
    return 0;
}

unsigned int eos_trigger_int(unsigned int id, unsigned int delay)
{
    assert(id);

    usleep(250);
    // This sleep prevents interrupt handlers triggering resource
    // starvation of other qemu threads, should the handler trigger
    // its own interrupt.
    //
    // Without this, on qemu 4, MPU SIO3 interrupt handling
    // slows to a crawl.  Qemu 2 is okay.
    // I'm not sure on the root cause, but qemu did change their iothread
    // and cpu thread to work simultaneously around this period, so that's
    // plausible.
    //
    // Value found via experimentation on one system only, no idea
    // if it's sane for different hosts.  Lower values, for me, trigger
    // repeated "[MPU] Request more data, index: 0x0", for many seconds per
    // MPU message, or sometimes hanging indefinitely.  These are still
    // seen but in much lower numbers now (needs -d mpu).  Qemu 2 had
    // almost none.
    //
    // SJE TODO this doesn't feel a very "qemu" way to solve the problem.
    // Notably they don't use usleep anywhere.  Maybe some pthread function
    // would be better?  Maybe later Qemu solves this for us?

    if(!delay && eos_state->irq_enabled[id] && !eos_state->irq_id)
    {
        if (qemu_loglevel_mask(CPU_LOG_INT)) {
            fprintf(stderr, "[EOS] trigger int 0x%02X\n", id);
        }
        eos_state->irq_id = id;
        eos_state->irq_enabled[eos_state->irq_id] = 0;
        cpu_interrupt(CPU(CURRENT_CPU), CPU_INTERRUPT_HARD);
    }
    else
    {
        if (qemu_loglevel_mask(CPU_LOG_INT)) {
            fprintf(stderr, "[EOS] trigger int 0x%02X (delayed!)\n", id);
        }
        if(!eos_state->irq_enabled[id])
        {
            delay = 1;
        }
        eos_state->irq_schedule[id] = MAX(delay, 1);
    }
    return 0;
}

static int eos_handle_card_led(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = "Card LED";
    unsigned int ret = 0;
    static int stored_value = 0;

    MMIO_VAR(stored_value);

    if (type & MODE_WRITE)
    {
        if (eos_state->model->digic_version == 10)
        {
            eos_state->card_led =
                ((value & 0xFFF000F) == 0x24D0002) ?  1 : // led on
                ((value & 0xFFF000F) == 0x24C0003) ? -1 : // led off
                (value == 0x24C0033)               ? -1 : // initial val set by bootloader on R6
                (value == 0x240003C)               ? -1 : 0; // initial val set by bootloader on R5
        }
        else if (eos_state->model->digic_version == 6 ||
            eos_state->model->digic_version == 7 ||
            eos_state->model->digic_version == 8)
        {
            eos_state->card_led =
                ((value & 0x0F000F) == 0x0D0002) ?  1 :
                ((value & 0x0F000F) == 0x0C0003) ? -1 :
                (value == 0x8A0075)              ? -1 : 0;
        }
        else
        {
            eos_state->card_led =
                (value == 0x46 || value == 0x138800
                               || value == 0x138000  /* 7D */
                               || value == 0x93D800) ?  1 :
                (value == 0x44 || value == 0x838C00 ||
                 value == 0x40 || value == 0x038C00
                               || value == 0x83DC00
                               || value == 0x800C00   /* 7D */
                               || value == 0xE000000) ? -1 : 0;
        }

        /* this will trigger if somebody writes an invalid LED ON/OFF code */
        assert(eos_state->card_led);
    }

    io_log("GPIO", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_dummy_dev_digicX(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = 0;
    unsigned int ret = 0;

    static int devices_init[0xF];

    /* R6 initializes 4 regions in the same way:
     * 0xca340000, 0xcc340010, 0xce340010, 0xd0340010
     * First, code waits for +0x10 to be 0 (then something is written)
     * Later, code waits for +0x10 to become 1 (then it continues)
     */
    if (address < 0xD1000000)
    {
        if (address & 0xc8)
        {
            msg = "device init? 0xc8";
            ret = 0xFF; //maybe 0x7?
        }
        else if (address & 0x10)
        {
            int base = ((address > 0x1C) & 0xF);
            msg = "device init? 0x10";
            ret = devices_init[base];
            // dummy toggle
            devices_init[base] = !devices_init[base];
        }
    }

    io_log("DUMMYX", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_digicX(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = 0;
    unsigned int ret = 0;

    if (address >= 0xD2230000 && address <= 0xD223FFFF) {
        /* 0x0xD223xxxxx, 0xD22390C2 on R6 */
        if (address == eos_state->model->card_led_address)
        {
            return eos_handle_card_led(parm, address, type, value);
        }
        msg = "R6 GPIO?";
        ret = 0;
    }

    switch (address)
    {
        case 0xD2100600:
        case 0xD2120600:
        case 0xD2C10600:
        case 0xD2A00600:
        case 0xD2600600:
            msg = "MEMIF init?";
            ret = 0xFFFFFFFF;
            break;
        case 0xD2210008: /* CLOCK_ENABLE */
            msg = "CLOCK_ENABLE";
            MMIO_VAR(eos_state->clock_enable_6);
            break;
        case 0xd2220404:
            msg = "Wake up CPU1?";       /* R6: wake up the second CPU? */
            assert(eos_state->cpu1);
            #if 0
            CPU(eos_state->cpu1)->halted = 0;
            printf(KLRED"Wake up CPU1"KRESET"\n");
            #endif
            ret = 1;
            break;
        case 0xD2030000:
            //msg = "bootloader, card mout related - for CFe";
            return 1; // loop with 11000000 iterations, takes a minute if prints are enabled
            break;
        /* below are values needed to progress through 1st stage bootloader
         * without any known context what exactly those do */
        case 0xd2010000:
            ret = 0x80000000;
            break;
        case 0xD2010070:
            ret = 1;
            break;
        case 0xd20100A0:
            ret = 0x1A5B34;
            break;
        case 0xd2100248:
            ret = 0x1;
            break;
        case 0xD26105C0:
            ret = 0x10000;
            break;
        case 0xd21105c0:
        case 0xd21305c0:
        case 0xd2a105c0:
        case 0xd2c205C0:
            ret = 0x10000;
            break;
    }

    io_log("DIGICX", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_multicore(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *module = "MULTICORE";
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    unsigned int ret = 0;

    switch (address & 0xFFFF)
    {
        case 0x730:
            msg = "sync caches?";
            break;

        case 0x7B0:
            msg = "sync cache address?";
            break;

        case 0x100:
            msg = "Wake Up CPU1?";
            assert(eos_state->cpu1);
            #if 0
            CPU(s->cpu1)->halted = 0;
            printf(KLRED"Wake Up CPU1"KRESET"\n");
            #endif
            break;

        case 0x214:
            msg = "Signal to CPU1?";
            break;
    }

    if (qemu_loglevel_mask(CPU_LOG_INT))
    {
        io_log(module, address, type, value, ret, msg, msg_arg1, msg_arg2);
    }
    return ret;
}

/* this appears to be an older interface for the same interrupt controller */
unsigned int eos_handle_intengine_vx(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    unsigned int ret = 0;

    switch(address & 0xF)
    {
        case 0xC:
            {
                msg = "Reset interrupts %Xh (%Xh)";
                msg_arg1 = value;

                for (int i = 0; i < 32; i++)
                {
                    if (value & (1u << i))
                    {
                        msg_arg2 = ((address & 0xF0) >> 1) + i;
                        if (msg_arg2 < COUNT(eos_state->irq_enabled))
                        {
                            eos_state->irq_enabled[msg_arg2] = 0;
                        }
                    }
                }
            }
            break;

        case 0x8:
            if(type & MODE_WRITE)
            {
                msg = "Enabled interrupts %Xh (%Xh)";
                msg_arg1 = value;

                for (int i = 0; i < 32; i++)
                {
                    if (value & (1u << i))
                    {
                        msg_arg2 = ((address & 0xF0) >> 1) + i;
                        if (msg_arg2 < COUNT(eos_state->irq_enabled))
                        {
                            eos_state->irq_enabled[msg_arg2] = 1;
                        }
                    }
                }
            }
            break;
    }

    if (qemu_loglevel_mask(CPU_LOG_INT))
    {
        io_log("INTvx", address, type, value, ret, msg, msg_arg1, msg_arg2);
    }
    return ret;
}

unsigned int eos_handle_intengine(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    unsigned int ret = 0;

    switch(address)
    {
        case 0xC0201000:    /* DIGIC 2,3 */
        case 0xC0201004:    /* DIGIC 4,5 (returns irq_id << 2) */
        case 0xD4011000:    /* DIGIC 6,7 */
        case 0xD0211000:    /* DIGIC 8 */
        case 0xD231A000:    /* DIGIC X */

        case 0xD02C0290:    /* 5D3 EEKO */
            if(type & MODE_WRITE)
            {
                msg = "Wrote int reason ???";
            }
            else
            {
                msg = "Requested int reason %x (INT %02Xh)";
                msg_arg1 = eos_state->irq_id << 2;
                msg_arg2 = eos_state->irq_id;
                ret = eos_state->irq_id << ((address & 0xF) ? 2 : 0);

                /* this register resets on read (subsequent reads should report 0) */
                eos_state->irq_id = 0;
                cpu_reset_interrupt(CPU(CURRENT_CPU), CPU_INTERRUPT_HARD);

                if (msg_arg2 == TIMER_INTERRUPT && 
                    !(qemu_loglevel_mask(CPU_LOG_INT) &&
                      qemu_loglevel_mask(EOS_LOG_VERBOSE)))
                {
                    /* timer interrupt, quiet */
                    return ret;
                }
            }
            break;

        case 0xC0201010:    /* DIGIC <= 5 */
        case 0xD4011010:    /* DIGIC 6,7 */
        case 0xD0211010:    /* DIGIC 8 */
        case 0xD231A010:    /* DIGIC X */
        case 0xD02C029C:    /* 5D3 EEKO */
            if(type & MODE_WRITE)
            {
                msg = "Enabled interrupt %02Xh";
                msg_arg1 = value;
                eos_state->irq_enabled[value] = 1;

                /* we shouldn't reset s->irq_id here (we already reset it on read) */
                /* if we reset it here also, it will trigger interrupt 0 incorrectly (on race conditions) */

                if (value == TIMER_INTERRUPT &&
                    !(qemu_loglevel_mask(CPU_LOG_INT) &&
                      qemu_loglevel_mask(EOS_LOG_VERBOSE)))
                {
                    /* timer interrupt, quiet */
                    return 0;
                }
            }
            else
            {
                /* DIGIC 6: interrupt handler reads this register after writing */
                /* value seems unused */
                return 0;
            }
            break;

        case 0xC0201200:    /* DIGIC <= 5 */
        case 0xD4011200:    /* DIGIC 6,7 */
        case 0xD0211200:    /* DIGIC 8 */
        case 0xD231A200:    /* DIGIC X */
        case 0xD02C02CC:    /* 5D3 EEKO */
            if(type & MODE_WRITE)
            {
                if (value)
                {
                    msg = "Reset IRQ?";
                    eos_state->irq_id = 0;
                    cpu_reset_interrupt(CPU(CURRENT_CPU), CPU_INTERRUPT_HARD);
                }
            }
            else
            {
                msg = "Read after enabling interrupts";
            }
            break;
    }

    if (qemu_loglevel_mask(CPU_LOG_INT))
    {
        io_log("INT", address, type, value, ret, msg, msg_arg1, msg_arg2);
    }
    return ret;
}

/* Private memory region for Cortex A9, used in EOS M5 */
/* http://www.csc.lsu.edu/~whaley/teach/FHPO_F11/ARM/CortAProgGuide.pdf#G26.1058874 */
/* fixme: reuse QEMU implementation from intc/arm_gic.c */
unsigned int eos_handle_intengine_gic(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *module = "PRIV";
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    unsigned int ret = 0;

    static int enabled[32] = {0};
    static int target[1024] = {0};
    static int iar = 0x20;

    switch (address & 0xFFFF)
    {
        /* Snoop Control Unit (SCU) */
        case 0x0000 ... 0x00FF:
        {
            module = "SCU";
            break;
        }

        /* Interrupt Controller CPU Interface */
        case 0x0100 ... 0x01FF:
        {
            module = "GIC CPUint";
            switch (address & 0xFF)
            {
                case 0:
                {
                    msg = "GICC_CTLR";
                    break;
                }
                case 4:
                {
                    msg = "GICC_PMR";
                    break;
                }
                case 0x0C:
                {
                    msg = "GICC_IAR";
                    //ret = (current_cpu->cpu_index) ? 0x0A : 0x20;
                    ret = 0x20;
                    if (type & MODE_READ)
                    {
                        if (current_cpu->cpu_index == 0
                            && value == 0
                            && iar == 0x20)
                        { // this is very spammy, seems "normal" and does nothing?
                        }
                        else
                        {
                            fprintf(stderr, "0x%x: cpu %d ack SGI 0x%x, iar: 0x%x\n",
                                    current_cpu->cpu_index ? eos_state->cpu1->env.regs[14] :
                                                             eos_state->cpu0->env.regs[14],
                                    current_cpu->cpu_index,
                                    value,
                                    iar);
                        }
                        if (iar != 0x20)
                        { // some valid SGI, which we should have raised,
                          // via cpu_interrupt(), further down in the 0xf00 case

                            // reading from GICC_IAR clears the interrupt
                            assert(current_cpu->cpu_index < 2);
                            if (current_cpu->cpu_index == 0)
                                cpu_reset_interrupt(CPU(eos_state->cpu0), CPU_INTERRUPT_HARD);
                            else
                                cpu_reset_interrupt(CPU(eos_state->cpu1), CPU_INTERRUPT_HARD);
                        }
                        ret = iar;
                    }
                    //fprintf(stderr, "GICC_IAR : cpu %d, iar: %d, ret: %d\n",
                    //        current_cpu->cpu_index,
                    //        iar,
                    //        ret);
                    break;
                }
                case 0x10:
                {
                    msg = "GICC_EOIR";
                    if (type & MODE_WRITE)
                    {
                        ret = value;  // unsure
                        iar = 0x20;
                    }
                    //fprintf(stderr, "GICC_EOIR: cpu %d, iar: %d, ret: %d\n",
                    //        current_cpu->cpu_index,
                    //        iar,
                    //        ret);
                    break;
                }
            }
            break;
        }

        /* Interrupt Controller Distributor */
        case 0x1000 ... 0x1FFF:
        {
            module = "GICD";
            switch (address & 0xFFF)
            {
                case 0:
                {
                    msg = "ICDDCR";
                    break;
                }
                case 0x100 ... 0x17C:
                {
                    // enables forwarding
                    msg = "GICD_ISENABLER%d (1C0+%02Xh)";
                    int word = ((address & 0xFFF) - 0x100) / 4;
                    msg_arg1 = word;
                    msg_arg2 = word * 32;
                    assert(word < COUNT(enabled));
                    MMIO_VAR(enabled[word]);
                    break;
                }
                case 0x180 ... 0x1FC:
                {
                    // clears forwarding
                    msg = "GICD_ICENABLER%d (1C0+%02Xh)";
                    int word = ((address & 0xFFF) - 0x180) / 4;
                    msg_arg1 = word;
                    msg_arg2 = word * 32;
                    assert(word < COUNT(enabled));
                    if(type & MODE_WRITE) {
                        enabled[word] &= ~value;
                    }
                    break;
                }
                case 0x400 ... 0x4FF:
                {
                    msg = "ICDIPRn";
                    break;
                }
                case 0x800 ... 0x880:
                {
                    // target cpu
                    msg = "GICD_ITARGETSR%d (1C0+%02Xh)";
                    int id = ((address & 0xFFFF) - 0x1800);
                    msg_arg1 = id;
                    msg_arg2 = id;
                    MMIO_VAR(target[id]);
                    break;
                }

                case 0xf00:
                { // software generated interrupt
                    msg = "ICDSGIR";
                    int target_int = value & 0xf;
                    if(type && MODE_WRITE)
                    {
                        MMIO_VAR(enabled[target_int]);
                        iar = target_int;

                        // 0xa is required to wake cpu1 from a wfi loop
                        // while cpu0 does early init.  See e.g. 200D 1.0.1
                        // 0xe0004d30
                        fprintf(stderr, "0x%x: cpu %d sending SGI 0x%x\n",
                                current_cpu->cpu_index ? eos_state->cpu1->env.regs[14] :
                                                         eos_state->cpu0->env.regs[14],
                                current_cpu->cpu_index,
                                value & 0xffff);
                        assert(current_cpu->cpu_index < 2);
                        if (current_cpu->cpu_index == 0)
                            cpu_interrupt(CPU(eos_state->cpu1), CPU_INTERRUPT_HARD);
                        else
                            cpu_interrupt(CPU(eos_state->cpu0), CPU_INTERRUPT_HARD);
                    }
                }

            }
            break;
        }
    }

    if (qemu_loglevel_mask(CPU_LOG_INT))
    {
        io_log(module, address, type, value, ret, msg, msg_arg1, msg_arg2);
    }
    return ret;
}

unsigned int eos_handle_timers_(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int pc = CURRENT_CPU->env.regs[15];

    if(type & MODE_WRITE)
    {
        fprintf(stderr, "[Timer?] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        return 0;
    }
    return 0;
}

unsigned int eos_handle_timers(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;

    int timer_id = 
        (parm == 0) ? ((address & 0xF00) >> 8)     :    /* DIGIC 4/5 timers (0,1,2...5) */
        (parm == 2) ? 11 :                              /* 5D3 Eeko DryOS timer */
                      -1 ;
    assert(timer_id >= 0);

    msg_arg1 = timer_id;
    
    if (timer_id < COUNT(eos_state->timer_enabled))
    {
        switch(address & 0x1F)
        {
            case 0x00:
                if(type & MODE_WRITE)
                {
                    if(value & 1)
                    {
                        if (DRYOS_TIMER_ID && timer_id == DRYOS_TIMER_ID)
                        {
                            msg = "Timer #%d: starting triggering";
                            eos_trigger_int(TIMER_INTERRUPT, eos_state->timer_reload_value[timer_id] >> 8);   /* digic timer */
                        }
                        else
                        {
                            msg = "Timer #%d: starting";
                        }

                        eos_state->timer_enabled[timer_id] = 1;
                    }
                    else
                    {
                        msg = "Timer #%d: stopped";
                        eos_state->timer_enabled[timer_id] = 0;
                        eos_state->timer_current_value[timer_id] = 0;
                    }
                }
                else
                {
                    msg = "Timer #%d: ready";
                }
                break;
            
            case 0x08:
                MMIO_VAR(eos_state->timer_reload_value[timer_id]);

                if(type & MODE_WRITE)
                {
                    msg = "Timer #%d: will trigger after %d ms";
                    msg_arg2 = ((uint64_t)value + 1) / 1000;
                }
                break;
            
            case 0x0C:
                msg = "Timer #%d: current value";
                ret = eos_state->timer_current_value[timer_id];
                break;
            
            case 0x10:
                if(type & MODE_WRITE)
                {
                    msg = "Timer #%d: interrupt enable?";
                }
                break;
        }
    }

    io_log("TIMER", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

/* DIGIC 6 UTimer (they look like regular timers, but behave like HPTimers) */
unsigned int eos_handle_utimer(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = "UTimer #%d: ???";
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    
    unsigned int ret = 0;
    int timer_id = ((address & 0xFC0) >> 6) - 9;
    msg_arg1 = timer_id;

    switch(address & 0x3F)
    {

        case 0x00:
            MMIO_VAR(eos_state->UTimers[timer_id].active);
            msg = value == 1 ? "UTimer #%d: active" :
                  value == 0 ? "UTimer #%d: inactive" :
                               "UTimer #%d: ?!";
            break;

        case 0x08:
            /* fixme: duplicate code (same as HPTimer offset 1x4) */
            if(type & MODE_WRITE)
            {
                /* upper rounding, to test for equality with digic_timer32 */
                uint32_t rounded = (value + DIGIC_TIMER_STEP) & DIGIC_TIMER32_MASK;
                eos_state->UTimers[timer_id].output_compare = rounded;

                /* for some reason, the value set to output compare
                 * is sometimes a little behind digic_timer32 */
                int actual_delay = (int32_t)(rounded - eos_state->digic_timer32);

                if (actual_delay < 0)
                {
                    /* workaround: when this happens, trigger right away */
                    eos_state->UTimers[timer_id].output_compare = eos_state->digic_timer32 + DIGIC_TIMER_STEP;
                }

                msg = "UTimer #%d: output compare (delay %d microseconds)";
                msg_arg2 = value - eos_state->digic_timer32_last_read;
            }
            else
            {
                ret = eos_state->UTimers[timer_id].output_compare;
                msg = "UTimer #%d: output compare";
            }
            break;

        case 0x0C:
            if(type & MODE_WRITE)
            {
                msg = value == 1 ? "UTimer #%d: start" :
                      value == 0 ? "UTimer #%d: stop" :
                                   "UTimer #%d: ?!";
            }
            else
            {
                msg = "UTimer #%d: status";
            }
            break;

        case 0x10:
            MMIO_VAR(eos_state->UTimers[timer_id].triggered);
            msg = "UTimer #%d: triggered?";
            break;
    }

    io_log("TIMER", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

unsigned int eos_handle_hptimer(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;
    
    unsigned int ret = 0;
    int timer_id = (address & 0x0F0) >> 4;
    msg_arg1 = timer_id;

    switch(address & 0xF0F)
    {
        case 0x100:
            if(type & MODE_WRITE)
            {
                msg = value == 1 ? "HPTimer #%d: enabled?" :
                      value == 0 ? "HPTimer #%d: disabled?" :
                                   "HPTimer #%d: ?!";
            }
            else
            {
                msg = "HPTimer #%d: status?";
            }
            break;

        case 0x104:
            if(type & MODE_WRITE)
            {
                /* upper rounding, to test for equality with digic_timer20 */
                int rounded = (value + DIGIC_TIMER_STEP) & DIGIC_TIMER20_MASK;
                eos_state->HPTimers[timer_id].output_compare = rounded;
                
                /* for some reason, the value set to output compare
                 * is sometimes a little behind digic_timer20 */
                int actual_delay = ((int32_t)(rounded - eos_state->digic_timer20) << 12) >> 12;

                if (actual_delay < 0)
                {
                    /* workaround: when this happens, trigger right away */
                    eos_state->HPTimers[timer_id].output_compare = eos_state->digic_timer20 + DIGIC_TIMER_STEP;
                }

                msg = "HPTimer #%d: output compare (delay %d microseconds)";
                msg_arg2 = value - eos_state->digic_timer20_last_read;
            }
            else
            {
                ret = eos_state->HPTimers[timer_id].output_compare;
                msg = "HPTimer #%d: output compare";
            }
            break;

        case 0x200:
            MMIO_VAR(eos_state->HPTimers[timer_id].active);
            msg = value == 1 ? "HPTimer #%d: active" :
                  value == 0 ? "HPTimer #%d: inactive" :
                  value == 3 ? "HPTimer #%d: periodic?" :
                               "HPTimer #%d: ?!";
            break;

        case 0x204:
            msg = "HPTimer #%d: ???";
            if(type & MODE_WRITE)
            {
                msg = "HPTimer #%d: reset trigger?";
                eos_state->HPTimers[timer_id].triggered = 0;
            }
            break;
        
        case 0x300:
            if(type & MODE_WRITE)
            {
                msg = "?!";
            }
            else
            {
                ret = 0;
                int i;
                for (i = 0; i < 8; i++)
                    if (eos_state->HPTimers[6+i].triggered)
                        ret |= 1 << (2*i+4);
                
                msg = "Which timer(s) triggered";
            }
            break;
    }

    io_log("HPTimer", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}


// 100D Set_AVS
static
unsigned int avs_handle(int address, int type, int val)
{
    // Actual values from a live 100D, possibly reads from an ADC and 
    // the voltage levels set by some voltage supply. If the wrong 
    // values are used there will be a divide-by-zero error in Canon
    // firmware, resulting in assert(0) @ Stub.c.
    const uint32_t avs_reply[][3] = {
        { 0x000C00, 0x200400, 0xE8D3 },
        { 0x000C00, 0x300000, 0x00AA },
        { 0x100800, 0x200400, 0xBC94 },
        { 0x100800, 0x300000, 0x0099 },
    };
    static int regA = 0, regB = 0;
    unsigned int ret = 0;
    const char *msg = "unknown";

    if (type & MODE_WRITE) {
        switch (address & 0xFFFF) {
            case 0xC288:
                msg = "reg A";
                regA = val;
                break;
            case 0xC28C:
                msg = "reg B";
                regB = val;
                break;
        }
    } else {
        switch (address & 0xFFFF) {
            case 0xF498:
                for (int i = 0; i < sizeof(avs_reply)/sizeof(avs_reply[0]); i++) {
                    if (regA == avs_reply[i][0] && regB == avs_reply[i][1]) {
                        ret = avs_reply[i][2];
                        msg = "pattern match!";
                        regA = 0; regB = 0;
                        break;
                    }
                }
                break;
        }
    }
    io_log("AVS", address, type, val, ret, msg, 0, 0);
    return ret;
}

static int eos_handle_rtc_cs(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = "[RTC] CS";
    unsigned int ret = 0;

    if (type & MODE_WRITE)
    {
        unsigned int cs_active;
        if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0)
        {
            cs_active = ((value & 0x800) == 0x800);
        }
        else
        {
            cs_active = ((value & 0x06) == 0x06 || (value & 0x0100000) == 0x100000);
        }
        if(cs_active)
        {
            msg = "[RTC] CS set";
            eos_state->rtc.transfer_format = RTC_READY;
        }
        else
        {
            msg = "[RTC] CS reset";
            eos_state->rtc.transfer_format = RTC_INACTIVE;
        }
    }

    io_log("GPIO", address, type, value, ret, msg, 0, 0);
    return ret;
}

static int eos_handle_serial_flash_cs(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = "Serial flash CS";
    unsigned int ret = 0;

    if (type & MODE_WRITE)
    {
        serial_flash_set_CS(eos_state->sf, (value & eos_state->model->serial_flash_cs_bitmask) ? 1 : 0);
    }

    io_log("GPIO", address, type, value, ret, msg, 0, 0);
    return ret;
}

static unsigned int eos_handle_imgpowdet(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;

    static uint32_t imgpowcfg_written = 0;
    static uint32_t imgpowdet_written = 0;
    static uint32_t imgpowdet_enabled = 0;

    if (address == eos_state->model->imgpowdet_register)
    {
        msg = "ImgPowDet";
        MMIO_VAR(imgpowdet_written);

        if (!(type & MODE_WRITE))
        {
            ret = (imgpowdet_written & ~eos_state->model->imgpowdet_register_bit) |
                  (imgpowdet_enabled &  eos_state->model->imgpowdet_register_bit) ;
        }
    }

    if (address == eos_state->model->imgpowcfg_register)
    {
        msg = "InitializePcfgPort";
        MMIO_VAR(imgpowcfg_written);

        if (type & MODE_WRITE)
        {
            /* to double-check: if you swap the values here,
             * all the FRSP tests should print "Image Power Failure" */
            imgpowdet_enabled = (value & eos_state->model->imgpowcfg_register_bit)
                ? eos_state->model->imgpowdet_register_bit : 0;

            if (imgpowdet_enabled && eos_state->model->imgpowdet_interrupt)
            {
                eos_trigger_int(eos_state->model->imgpowdet_interrupt, 1);
            }
        }
    }
    
    io_log("IMGPOW", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_gpio(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 1;
    const char *msg = NULL;
    const char *msg_lookup = 0;
    static int unk = 0;

    /* 0xC022009C/BC/C06C/D06C, depending on camera model */
    if (address == eos_state->model->mpu_request_register ||
        address == eos_state->model->mpu_status_register)
    {
        return eos_handle_mpu(parm, address, type, value);
    }

    /* 0xC0220134/BC/6C/C188/C184, depending on model */
    if (address == eos_state->model->card_led_address)
    {
        return eos_handle_card_led(parm, address, type, value);
    }

    /* 0xC02200B0/005C/0128/01D4/01F8/C020/C0C4 */
    if (address == eos_state->model->rtc_cs_register)
    {
        return eos_handle_rtc_cs(parm, address, type, value);
    }

    /* 0xC022002C, 0xC022C0D4 */
    if (eos_state->sf && address == eos_state->model->serial_flash_cs_register)
    {
        return eos_handle_serial_flash_cs(parm, address, type, value);
    }

    /* 0xC0220008, 0xC022001C, 0xC0220124; 0xC0220118 */
    if (address == eos_state->model->imgpowdet_register ||
        address == eos_state->model->imgpowcfg_register)
    {
        return eos_handle_imgpowdet(parm, address, type, value);
    }

    switch (address & 0xFFFF)
    {
        case 0x0068:
            // RTC related, seems to be set *after* setup register so not usable as rtc_cs
            if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
                if(type & MODE_WRITE) {
                    if(value == 0x46) {
                        msg = "RTC com on";
                    } else if (value == 0x44) {
                        msg = "RTC com off";
                    } else {
                        msg = "RTC com??";
                    }
                } else {
                    msg = "RTC com??";
                }
            }
            break;
        case 0xCB6C: /* 5D3/6D expect this one to be 0x10 in bootloader (6D:FFFF0544) */
            msg = "5D3/6D expected to be 0x10";
            ret = 0x10;
            break;
        
        case 0xFA04:
            msg = "6D expected to be 0";
            ret = 0;
            break;

        case 0xFA0C:
            msg = "100D: locks up";
            ret = rand();
            break;

        case 0xF100:
            if(type & MODE_WRITE)
            {
            }
            else
            {
                ret = unk;
                unk++;
                unk %= 2;
            }
            break;

        case 0xF198:
            if(type & MODE_WRITE)
            {
            }
            else
            {
                ret = unk;
                unk++;
                unk %= 2;
            }
            break;

        /*
        0xC022F480 [32]  Other VSW Status
           0x40000 /VSW_OPEN Hi
           0x80000 /VSW_REVO Hi
        */
        case 0xF480:
            if(type & MODE_WRITE)
            {
            }
            else
            {
                if (strcmp(eos_state->model->name, MODEL_NAME_5D2) == 0 ||
                    strcmp(eos_state->model->name, MODEL_NAME_50D) == 0)
                {
                    ret = 0x6000;
                    msg = "VSW_STATUS 5D2/50D";
                }
                else
                {
                    ret = 0x40000 | 0x80000;
                    msg = "VSW_STATUS";
                    /* 70D: SD detect (0x10)
                     * 100D: SD detect (0x8) */
                }
            }
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

      case 0xF48C:
            if(strcmp(eos_state->model->name, MODEL_NAME_1300D) == 0)
            {
                /* 1300D: return 0 here to bypass "System & Display Check & Adjustment program" */
                /* 0x4000000 = HDMI disconnected */
                msg = "System check";
                ret = 0x4000000;    
            }  
            else 
            {        
                /* 6D: return -1 here to launch "System & Display Check & Adjustment program" */
                /* 6D SD detect (0x2) */
                /* 70D IFE init (0x108) */
                msg = "System check, SD detect, IFE init";
                ret = 0x10C;
            }
            break;
    
        case 0x019C: /* 5D3: return 1 to launch "System & Display Check & Adjustment program" */
        case 0x0080: /* same for 1000D */
            msg = "System check";
            ret = 0;
            break;

        case 0x00DC:
            msg = "abort situation for FROMUTIL on 600D";
            ret = 0;
            break;

        case 0x00B0:
            msg = "FUNC SW OFF on 7D";
            ret = 0;
            break;
            
        case 0x0024:
            msg = "master woke up on 7D";
            ret = 0;
            break;
        
        case 0x0108:
            /* ERASE SW OFF on 600D */
            msg = "ERASE SW OFF";
            ret = 1;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x010C:
            msg = "something from hotplug task on 60D";
            ret = 1;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x012C:
            msg = "1000D display";
            ret = rand();
            break;
    
        case 0x0034:
            if (eos_state->model->digic_version < 4)
            {
                msg = "400D init";
                ret = rand();
                break;
            }
            else
            {
                /* USB on 600D */
                msg = "600D USB CONNECT";
                ret = 0;
#ifdef IGNORE_CONNECT_POLL
                return ret;
#endif
                break;
            }

        case 0x014:
            /* /VSW_ON on 600D */
            msg = "/VSW_ON";
            ret = 0;
            break;

        case 0x0098:
        {
            static int last_value = 1;
            MMIO_VAR(last_value);
            msg = (value & 0x02) ? "SRM_SetBusy" 
                                 : "SRM_ClearBusy" ;
            break;
        }

        case 0x00B8:
        {
            static int last_value = 1;
            MMIO_VAR(last_value);
            msg = (value & 0x02) ? "SRM_SetBusy" 
                                 : "SRM_ClearBusy" ;
            break;
        }

        case 0x00A0:    /* DIGIC 4 (most models) */
        case 0x004C:    /* 700D, 100D */
        case 0x00D0:    /* 6D */
        case 0x0168:    /* 70D */
        case 0x01FC:    /* 5D3 */
        case 0x0120:    /* 450D */
            msg = "WriteProtect";
            ret = 0;
            break;
        
        
        case 0x301C:    /* D3, D4, older D5, 5D3 CF */
            /* set low => CF/SD present */
            msg = "CF/SD detect";
            ret = 0;
            break;

        case 0x3020:    /* 5D3 */
            /* set low => SD present */
            msg = "SD detect";
            ret = 0;
            break;


        /* 100D */
        //case 0xC0DC: // [0xC022C0DC] <- 0x83DC00  : GPIO_12
        case 0xC0E0:   // [0xC022C0E0] <- 0xA3D400  : GPIO_13
            if ((type & MODE_WRITE) && value == 0xA3D400) {
                msg = "100D Serial flash DMA start?";
                ret = 0;
            }

            break;

        case 0x011C:    /* 40D, 450D */
            msg = "VIDEO CONNECT";
            ret = (strcmp(eos_state->model->name, MODEL_NAME_40D) == 0) ? 0 : 1;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x0070:    /* 600D, 60D */
        case 0x0164:
        case 0x0174:    /* 5D3 */
            msg = "VIDEO CONNECT";
            ret = 1;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x00E8:    /* 600D, 60D */
        case 0x0160:
        case 0x016C:    /* 5D3 */
        case 0x0134:    /* EOSM */
            msg = "MIC CONNECT";
            // A1100 this is related to startup key press (ffc3040c)
            if((strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) && ((address & 0xFFFF) == 0x134)) {
                msg = "A1100 start key";
                ret = 0;
                break;
            } else {
                ret = 1;
            }
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;
        
        case 0x015C:
        case 0x017C:    /* 5D3 */
        case 0x0130:    /* EOSM; 40D erase switch */
        case 0x0100:    /* 40D, 450D, 1000D */
            msg = "USB CONNECT";
            ret = 0;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x0128:    /* 40D TOE (Ceres) */
            msg = "TOE CONNECT";
            ret = 1;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x003C:    /* 5D2, 50D */
        case 0x0124:    /* 100D? */
        case 0x0150:    /* 5D3 */
        case 0x0158:    /* 6D */
            msg = "HDMI CONNECT";
            ret = 0;
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        case 0x0138:
            if (eos_state->model->digic_version == 5) {
                msg = "VIDEO CONNECT";      /* EOSM; likely other D5 models */
                ret = 1;                    /* negative logic */
            } else if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0 ) {
                msg = "PB startup";       /* indicates play switch startup ffc3040c */
                ret = 1;
                break;
            } else {
                msg = "HDMI CONNECT";       /* 600D; likely other D4 models */
                ret = 0;
            }
#ifdef IGNORE_CONNECT_POLL
            return ret;
#endif
            break;

        // A1100 keyboard etc GPO (read by GetKbdState ffc30b28, bits from CHDK kbd.c)
        case 0x200:
        case 0x204:
        case 0x208:
            if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0 ) {
                uint32_t physw_mmio_bits[] = {
                    0x00000000, // unknown
                    0x0001FF00, // 0xFF00 = unpressed state of D-pad, Menu, PRINT,
                                // + 0x10000 unknown, avoids most keys disabled at startup by ffc11184
                                // setting event 0x8000001a, checked in ffc5c918
                    0x000090F0, // unpressed state of zoom, shoot.
                                // 0x1000 indicates video cable not connected
                                // 0x8000 is battery / card door, must be set to boot (see ffc601cc)
                                // upper half word of 3rd MMIO is ignored, corresponding
                                // physw_status bits come from kbd_read_keys_r2 (ffc304a4)
                };
                ret = physw_mmio_bits[(address&0xc) >> 2];
            }
            break;

        case 0x320C:
            msg = "Eeko WakeUp";
            if (type & MODE_WRITE)
            {
                if (value == 7)
                {
                    eos_trigger_int(0x111, 0);
                }
            }
            break;

        // 100D Set_AVS
        case 0xC288:
        case 0xC28C:
        case 0xF498:
            return avs_handle(address, type, value);
    }

    msg_lookup = get_bufcon_label(bufcon_label_100D, address);
    if (msg_lookup != NULL && msg != NULL)
    {
        char tmp[128];
        snprintf(tmp, sizeof(tmp), "%s (%s)", msg_lookup, msg);
        io_log("GPIO", address, type, value, ret, tmp, 0, 0);
    }
    else
    {
        if (msg == NULL)
            msg = msg_lookup;
        io_log("GPIO", address, type, value, ret, msg, 0, 0);
    }
    return ret;
}

unsigned int eos_handle_ram(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    int ret = eos_default_handle(address, type, value);

    /* not tested; appears unused */
    io_log("RAM", address, type, value, ret, 0, 0, 0);

    return ret;
}

unsigned int eos_handle_power_control(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    if (address == eos_state->model->imgpowcfg_register)
    {
        return eos_handle_imgpowdet(parm, address, type, value);
    }

    unsigned int ret = 0;
    static uint32_t data[0x100 >> 2];
    uint32_t index = (address & 0xFF) >> 2;
    
    MMIO_VAR(data[index]);
    
    io_log("Power", address, type, value, ret, 0, 0, 0);
    return ret;
}


unsigned int eos_handle_adc(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    int msg_arg1 = 0;
    unsigned int ret = 0;
    
    if(type & MODE_WRITE)
    {
    }
    else
    {
        // digic 6 style
        if(parm == 0) {
            int channel = (address & 0xFF) >> 2;
            msg = "channel #%d";
            msg_arg1 = channel;
            
            if (strcmp(eos_state->model->name, MODEL_NAME_EOSM3) == 0 ||
                strcmp(eos_state->model->name, MODEL_NAME_EOSM10) == 0)
            {
                /* values from Ant123's camera (M3) */
                uint32_t adc_values[] = {
                    0x0000de40, 0x00008c00, 0x00008300, 0x00003ca0,
                    0x00003eb0, 0x00003f00, 0x0000aa90, 0x00000050,
                    0x00003c20, 0x0000fd60, 0x0000f720, 0x00000030,
                    0x00008a80, 0x0000a440, 0x00000020, 0x00000030,
                    0x00000030, 0x00008900, 0x0000fd60, 0x0000fed0,
                    0x0000fed0, 0x00000310, 0x00000020, 0x00000020,
                    0x00000020, 0x00000020, 0x00000010, 0x00000000
                };

                if (channel < COUNT(adc_values))
                {
                    ret = adc_values[channel];
                }
            }
        }
        // digic 2 - 5 P&S style
        else if (parm == 1)
        {
            unsigned int off = (address & 0xFF);
            if(off >= 0x40 && off <= 0x56) {
                int channel = (off - 0x40) >> 1;
                msg = "channel #%d+"; // can't distinguish channels in high and low words
                msg_arg1 = channel;
                if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0)
                {
                    // digic 2 - 5 P&S style, each ADC is half word, but fw reads full word and shifts/masks
                    // representative values from d10, channel usage seems
                    // mostly consistent for digic 2-5, but valid voltage
                    // may vary and AA battery cams don't have tbat and
                    // may use different order for temps
                    // see https://chdk.setepontos.com/index.php?topic=10385.msg102943#msg102943
                    uint32_t adc_values[] = {
                       0,   // channel  0 0xc0900040 0 0x0
                       1,   // channel  1 0xc0900042 1 0x1
    //                   803, // channel  2 0xc0900044 803 0x323 < vbat ~4.037v (from LiPo battery D10)
                       497, // channel  2 value from A540 ~2.215v (2x AA battery cam like A1100)
                       471, // channel  3 0xc0900046 471 0x1d7 < tccd ~15c
                       448, // channel  4 0xc0900048 448 0x1c0 < topt ~13c
                       422, // channel  5 0xc090004a 422 0x1a6 < tbat ~ 17c
                       1,   // channel  6 0xc090004c 1 0x1
                       1,   // channel  7 0xc090004e 1 0x1
                       1,   // channel  8 0xc0900050 1 0x1
                       1,   // channel  9 0xc0900052 1 0x1
                       565, // channel 10 0xc0900054 565 0x235
                       1,   // channel 11 0xc0900056 1 (USB not present) - 524 0x20c (USB present)
                    };
                    if (channel >= 0 && channel < COUNT(adc_values))
                    {
                        ret = adc_values[channel & ~1] | (adc_values[channel | 1]  << 16);
                    }
                }
            } else if (off == 0xb0) {
                msg = "bat voltage";
                if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
                    // avoids bad return value from ffc106ec, related to battery
                    // value seen on a540, 2xAA battery at 2.6v. LiPo camera at 4.2 = 0x2033c
                    ret = 0x2024f;
                }
            } else if (off == 0xd4) {
                msg = "ADC ready?";
                ret = 0x0ffe000a; // A1100 ffc2dd28, related to ADC setup, avoid long busy loop polling MMIO. Value from D10
            }
        }
    }
    
    io_log("ADC", address, type, value, ret, msg, msg_arg1, 0);
    return ret;
}

unsigned int eos_handle_dma(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;
    static unsigned int srcAddr = 0;
    static unsigned int dstAddr = 0;
    static unsigned int count = 0;
    unsigned int interruptId[] = {0x00, 0x2f, 0x74, 0x75, 0x76, 0xA0, 0xA1, 0xA8, 0xA9};

    switch(address & 0xFF)
    {
        case 0x08:
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    msg = "Start DMA";
                    fprintf(stderr, "[DMA%i] Copy [0x%08X] -> [0x%08X], length [0x%08X], flags [0x%08X]\r\n", parm, srcAddr, dstAddr, count, value);

                    uint32_t blocksize = 8192;
                    uint8_t *buf = malloc(blocksize);
                    uint32_t remain = count;
                    
                    uint32_t src = srcAddr;
                    uint32_t dst = dstAddr;

                    while(remain)
                    {
                        uint32_t transfer = (remain > blocksize) ? blocksize : remain;

                        eos_mem_read(src, buf, transfer);
                        eos_mem_write(dst, buf, transfer);

                        remain -= transfer;
                        src += transfer;
                        dst += transfer;
                    }
                    free(buf);

                    fprintf(stderr, "[DMA%i] OK\n", parm);

                    /* 1200D assumes the DMA transfer are not instant */
                    /* (otherwise, assert in Startup task - cannot find property 0x2) */
                    eos_trigger_int(interruptId[parm], count / 10000);
                    
                    if (!qemu_loglevel_mask(EOS_LOG_VERBOSE)) {
                        /* quiet (fixme: -d dma) */
                        return 0;
                    }
                }
            }
            break;

        case 0x18:
            msg = "srcAddr";
            MMIO_VAR(srcAddr);
            break;

        case 0x1C:
            msg = "dstAddr";
            MMIO_VAR(dstAddr);
            break;

        case 0x20:
            msg = "count";
            MMIO_VAR(count);
            break;
    }

    char dma_name[5];
    snprintf(dma_name, sizeof(dma_name), "DMA%i", parm);
    io_log(dma_name, address, type, value, ret, msg, 0, 0);

    return ret;
}

unsigned int eos_handle_xdmac(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;
    static unsigned int srcAddr = 0;
    static unsigned int dstAddr = 0;
    static unsigned int count = 0;
    unsigned int interruptId[] = {0x13E, 0x14E, 0x15E, 0x16E};

    switch ((address & 0xFF) % 0x30)
    {
        case 0x00:
        {
            static int last = 0;
            MMIO_VAR(last);
            break;
        }

        case 0x28:
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    msg = "Start DMA";
                    fprintf(stderr, "[XDMAC%i] Copy [0x%08X] -> [0x%08X], length [0x%08X], flags [0x%08X]\r\n", parm, srcAddr, dstAddr, count, value);

                    uint32_t blocksize = 8192;
                    uint8_t *buf = malloc(blocksize);
                    uint32_t remain = count;
                    
                    uint32_t src = srcAddr;
                    uint32_t dst = dstAddr;

                    while(remain)
                    {
                        uint32_t transfer = (remain > blocksize) ? blocksize : remain;

                        eos_mem_read(src, buf, transfer);
                        eos_mem_write(dst, buf, transfer);

                        remain -= transfer;
                        src += transfer;
                        dst += transfer;
                    }
                    free(buf);

                    fprintf(stderr, "[XDMAC%i] OK\n", parm);

                    eos_trigger_int(interruptId[parm], count / 10000);
                }
            }
            break;

        case 0x14:
            msg = "srcAddr";
            MMIO_VAR(srcAddr);
            break;

        case 0x18:
            msg = "dstAddr";
            MMIO_VAR(dstAddr);
            break;

        case 0x10:
            msg = "count";
            MMIO_VAR(count);
            break;
    }

    char dma_name[16];
    snprintf(dma_name, sizeof(dma_name), "XDMAC%i", parm);
    io_log(dma_name, address, type, value, ret, msg, 0, 0);

    return ret;
}

unsigned int eos_handle_xdmac7(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;
    static unsigned int srcAddr = 0;
    static unsigned int dstAddr = 0;
    static unsigned int count = 0;
    unsigned int interruptId[] = {0x11E, 0x12E, 0x13E};

    switch (address & 0x3F)
    {
        case 0x28:
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    msg = "Start DMA";
                    fprintf(stderr, "[XDMAC%i] Copy [0x%08X] -> [0x%08X], length [0x%08X], flags [0x%08X]\r\n", parm, srcAddr, dstAddr, count, value);

                    uint32_t blocksize = 8192;
                    uint8_t *buf = malloc(blocksize);
                    uint32_t remain = count;
                    
                    uint32_t src = srcAddr;
                    uint32_t dst = dstAddr;

                    while(remain)
                    {
                        uint32_t transfer = (remain > blocksize) ? blocksize : remain;

                        eos_mem_read(src, buf, transfer);
                        eos_mem_write(dst, buf, transfer);

                        remain -= transfer;
                        src += transfer;
                        dst += transfer;
                    }
                    free(buf);

                    fprintf(stderr, "[XDMAC%i] OK\n", parm);

                    eos_trigger_int(interruptId[parm], count / 10000);
                }
            }
            break;

        case 0x00:
            msg = "srcAddr";
            MMIO_VAR(srcAddr);
            break;

        case 0x04:
            msg = "dstAddr";
            MMIO_VAR(dstAddr);
            break;

        case 0x08:
            msg = "count";
            MMIO_VAR(count);
            break;
    }

    char dma_name[16];
    snprintf(dma_name, sizeof(dma_name), "XDMAC%i", parm);
    io_log(dma_name, address, type, value, ret, msg, 0, 0);

    return ret;
}

unsigned int eos_handle_uart(unsigned int parm, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int ret = 1;
    const char *msg = NULL;
    int msg_arg1 = 0;
    static int enable_tio_interrupt = 0;
    static int flags = 0;

    if ((address & ~0xF) == 0xC0270000)
    {
        /* this looks like a 16-char ring buffer (?!) */
        static uint32_t uart_buf[16];
        MMIO_VAR(uart_buf[address & 0xF]);
        goto end;
    }

    switch(address & 0xFF)
    {
        case 0x00:
            if(type & MODE_WRITE)
            {
                msg = "Write char";
                assert(value == (value & 0xFF));
                
                qemu_chr_fe_write_all(&eos_state->uart.chr, (void*) &value, 1);
                
                /* fixme: better way to check whether the serial is printing to console? */
                if (strcmp(eos_state->uart.chr.chr->filename, "stdio") != 0 &&
                    strcmp(eos_state->uart.chr.chr->filename, "mux") != 0 &&
                    strcmp(eos_state->uart.chr.chr->filename, "file") != 0)
                {
                    fprintf(stderr, "%c", value);
                }

                /* 0 written during initialization */
                if (enable_tio_interrupt)
                {
                    static int warned = 0;
                    if (!eos_state->model->uart_tx_interrupt)
                    {
                        if (!warned)
                        {
                            fprintf(stderr, "FIXME: uart_tx_interrupt unknown\n");
                            warned = 1;
                        }
                        break;
                    }

                    eos_trigger_int(eos_state->model->uart_tx_interrupt, 1);
                }
            }
            else
            {
                ret = 0;
            }
            break;

        case 0x04:
            msg = "Read char";
            eos_state->uart.reg_st &= ~(ST_RX_RDY);
            ret = eos_state->uart.reg_rx;
            break;

        case 0x08:
        {
            msg = "Flags?";
            MMIO_VAR(flags);
            flags &= ~0x800;
            break;
        }

        case 0x14:
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    msg = "Reset RX indicator";
                    eos_state->uart.reg_st &= ~(ST_RX_RDY);
                    eos_state->uart_just_received = 100;
                }
                else
                {
                    eos_state->uart.reg_st = value;
                }
            }
            else
            {
                msg = "Status: 1 = char available, 2 = can write";
                ret = eos_state->uart.reg_st;
            }
            break;

        case 0x18:
        {
            msg = "interrupt flags?";
            static int status = 0;
            MMIO_VAR(status);

            if(type & MODE_WRITE)
            {
                /* 1000D expects interrupt 0x3A to be triggered after writing each char */
                /* most other cameras are upset by this interrupt */
                if (value == 0xFFFFFFC4)
                {
                    msg = "enable interrupt?";
                    enable_tio_interrupt = 1;
                }
                else if (strcmp(eos_state->model->name, MODEL_NAME_EOSM3) != 0)
                {
                    enable_tio_interrupt = value & 1;
                }
            }
            break;
        }
    }

end:
    if (qemu_loglevel_mask(EOS_LOG_UART))
    {
        io_log("UART", address, type, value, ret, msg, msg_arg1, 0);
    }
    return ret;
}

unsigned int eos_handle_i2c(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    intptr_t msg_arg1 = 0;
    intptr_t msg_arg2 = 0;

    static unsigned int last_i2c_status = 0;
    
    static unsigned int last_i2c_rxpos = 0;
    static unsigned char last_i2c_rxdata[1024];
    
    static unsigned int last_i2c_txpos = 0;
    static unsigned char last_i2c_txdata[1024];
    
    static unsigned int last_i2c_addr = 0;
    static unsigned int last_i2c_length = 0;
    static unsigned int last_i2c_config = 0;
    
    switch(address & 0xFF)
    {
        case 0x08: /* status register */
            if(type & MODE_WRITE)
            {
            }
            else
            {
                /* 0x1000 busy */
                /* 0x0010 transmit data ready */
                /* 0x0020 stop condition */
                msg = "status";
                ret = last_i2c_status;
            }
            break;
        
        case 0x14: /* length */
            msg = "length";
            MMIO_VAR(last_i2c_length);
            break;
        
        case 0x18: /* rx data */
            if(type & MODE_WRITE)
            {
            }
            else
            {
                msg = "RX data";
                if(last_i2c_txpos < COUNT(last_i2c_txdata))
                {
                    ret = last_i2c_rxdata[last_i2c_rxpos++];
                }
                else
                {
                    ret = 0;
                }
            }
            break;
        
        case 0x1C: /* slave address */
            msg = "slave address";
            MMIO_VAR(last_i2c_addr);
            break;
        
        case 0x20: /* tx data */
            msg = "TX data (%d)";
            msg_arg1 = last_i2c_txpos;

            if(type & MODE_WRITE)
            {
                /* buffer data */
                if(last_i2c_txpos < COUNT(last_i2c_txdata))
                {
                    last_i2c_txdata[last_i2c_txpos] = value;
                }
                
                last_i2c_txpos++;
                if(last_i2c_txpos >= last_i2c_length)
                {
                    last_i2c_status |= 0x20;
                    last_i2c_status |= 0x08; /* receive data ready */
                }
                last_i2c_status |= 0x10;
            }
            else
            {
                ret = last_i2c_txdata[last_i2c_txpos];
            }
            break;
        
        case 0x24: /* some config? write:0x2E20 read:0xAC20,0x2420,0x8C20 */
            msg = "config? addr: %02X %s";
            msg_arg1 = last_i2c_addr;
            msg_arg2 = (intptr_t) "";

            if(type & MODE_WRITE)
            {
                last_i2c_config = value;
                
                /* set module inactive? */
                if(!(value & 0x20))
                {
                    char data[1024] = "";

                    if (last_i2c_txpos)
                    {
                        STR_APPEND(data, "\n[I2C] sent:");
                        for (int pos = 0; pos < last_i2c_txpos; pos++)
                        {
                            STR_APPEND(data, " %02X", last_i2c_txdata[pos]);
                        }
                    }

                    if (last_i2c_rxpos)
                    {
                        STR_APPEND(data, "\n[I2C] recv:");
                        for (int pos = 0; pos < last_i2c_rxpos; pos++)
                        {
                            STR_APPEND(data, " %02X", last_i2c_rxdata[pos]);
                        }
                    }

                    last_i2c_status = 0;
                    last_i2c_txpos = 0;
                    last_i2c_rxpos = 0;
                    msg_arg2 = (intptr_t)data;
                }
                /* set receive mode */
                else if(!(value & 0x200))
                {
                    switch(last_i2c_addr)
                    {
                        case 0x3D:
                            switch(last_i2c_txdata[0])
                            {
                                case 0x62:
                                    last_i2c_rxdata[0] = 0x00;
                                    last_i2c_rxdata[1] = 0x00;
                                    break;
                                    
                            }
                            break;
                            
                        case 0x38:
                            switch(last_i2c_txdata[0])
                            {
                                case 0x02:
                                    last_i2c_rxdata[0] = 0x00;
                                    last_i2c_rxdata[1] = 0x00;
                                    break;
                                    
                                case 0x04:
                                    last_i2c_rxdata[0] = 0x00;
                                    break;
                                    
                                case 0x1F:
                                    last_i2c_rxdata[0] = 0x01;
                                    break;
                                    
                                case 0x90:
                                    last_i2c_rxdata[0] = 0x01;
                                    break;
                                    
                                case 0x97:
                                    last_i2c_rxdata[0] = 0x10;
                                    break;
                                    
                            }
                            break;
                    }
                }
            }
            else
            {
                ret = last_i2c_config;
            }
            break;
    }

    io_log("I2C", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

static unsigned int eos_handle_rtc(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    int msg_arg1 = 0;
    int msg_arg2 = 0;

    static unsigned int last_sio_txdata = 0;
    static unsigned int last_sio_rxdata = 0;
    static unsigned int last_sio_setup1 = 0;
    static unsigned int last_sio_setup2 = 0;
    static unsigned int last_sio_setup3 = 0;

    switch(address & 0xFF)
    {
        case 0x04:
            if((type & MODE_WRITE) && (value & 1))
            {
                static char default_msg[100];
                snprintf(default_msg, sizeof(default_msg),
                    "Transmit: 0x%08X, setup 0x%08X 0x%08X 0x%08X",
                    last_sio_txdata, last_sio_setup1, last_sio_setup2, last_sio_setup3
                );
                msg = default_msg;

                switch(eos_state->rtc.transfer_format)
                {
                    /* CS inactive, do nothing */
                    case RTC_INACTIVE:
                    {
                        assert(0);
                        break;
                    }
                    
                    /* waiting for a command byte */
                    case RTC_READY:
                    {
                        uint8_t cmd = last_sio_txdata & 0x0F;
                        uint8_t reg = (last_sio_txdata>>4) & 0x0F;
                        if (!strcmp(eos_state->model->name, MODEL_NAME_5D2) ||
                            !strcmp(eos_state->model->name, MODEL_NAME_50D) ||
                            !strcmp(eos_state->model->name, MODEL_NAME_40D))
                        {
                            reg = last_sio_txdata & 0x0F;
                            cmd = (last_sio_txdata>>4) & 0x0F;
                        }
                        eos_state->rtc.transfer_format = cmd;
                        eos_state->rtc.current_reg = reg;

                        switch(cmd)
                        {
                            case RTC_WRITE_BURST:
                            case RTC_WRITE_BURST2:
                                msg = "Initiate WB (%02X)";
                                msg_arg1 = last_sio_txdata;
                                break;
                                
                            case RTC_READ_BURST:
                            case RTC_READ_BURST2:
                                msg = "Initiate RB (%02X)";
                                msg_arg1 = last_sio_txdata;
                                break;
                                
                            case RTC_WRITE_SINGLE:
                                msg = "Initiate WS (%02X)";
                                msg_arg1 = last_sio_txdata;
                                break;
                                
                            case RTC_READ_SINGLE:
                                msg = "Initiate RS (%02X)";
                                msg_arg1 = last_sio_txdata;
                                break;

                            default:
                                msg = "Requested invalid transfer mode 0x%02X";
                                msg_arg1 = last_sio_txdata;
                                break;
                        }
                        break;
                    }

                    /* burst writing */
                    case RTC_WRITE_BURST:
                    case RTC_WRITE_BURST2:
                        eos_state->rtc.regs[eos_state->rtc.current_reg] = last_sio_txdata;
                        msg = "WB %02X <- %02X";
                        msg_arg1 = eos_state->rtc.current_reg;
                        msg_arg2 = last_sio_txdata & 0xFF;
                        eos_state->rtc.current_reg++;
                        eos_state->rtc.current_reg %= 0x10;
                        break;

                    /* burst reading */
                    case RTC_READ_BURST:
                    case RTC_READ_BURST2:
                        last_sio_rxdata = eos_state->rtc.regs[eos_state->rtc.current_reg];
                        msg = "RB %02X -> %02X";
                        msg_arg1 = eos_state->rtc.current_reg;
                        msg_arg2 = last_sio_rxdata;
                        eos_state->rtc.current_reg++;
                        eos_state->rtc.current_reg %= 0x10;
                        break;

                    /* 1 byte writing */
                    case RTC_WRITE_SINGLE:
                        eos_state->rtc.regs[eos_state->rtc.current_reg] = last_sio_txdata;
                        msg = "WS %02X <- %02X";
                        msg_arg1 = eos_state->rtc.current_reg;
                        msg_arg2 = last_sio_txdata & 0xFF;
                        eos_state->rtc.transfer_format = RTC_READY;
                        break;

                    /* 1 byte reading */
                    case RTC_READ_SINGLE:
                        last_sio_rxdata = eos_state->rtc.regs[eos_state->rtc.current_reg];
                        msg = "RS %02X -> %02X";
                        msg_arg1 = eos_state->rtc.current_reg;
                        msg_arg2 = last_sio_rxdata;
                        eos_state->rtc.transfer_format = RTC_READY;
                        break;

                    default:
                        break;
                }
            }
            else
            {
                ret = 0;
            }
            break;

        case 0x0C:
            msg = "setup 1";
            MMIO_VAR(last_sio_setup1);
            break;

        case 0x10:
            msg = "setup 2";
            MMIO_VAR(last_sio_setup2);
            break;

        case 0x14:
            msg = "setup 3";
            MMIO_VAR(last_sio_setup3);
            break;

        case 0x18:
            msg = "TX register";
            MMIO_VAR(last_sio_txdata);
            break;

        case 0x1C:
            msg = "RX register";
            MMIO_VAR(last_sio_rxdata);
            break;
    }

    io_log("RTC", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

/*
handle SIO related to optical image stabilization system
probably common for other DryOS R31 era P&S with OIS. Later Digic IV cams are different
communication is generally like other SIO, but with some IS specific interrupts and registers
*/
static unsigned int eos_handle_A1100_IS_com(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    static unsigned int last_sio_txdata = 0;
    static unsigned int last_sio_rxdata = 0;
    static unsigned int last_sio_setup1 = 0;
    static unsigned int last_sio_setup2 = 0;
    static unsigned int last_sio_setup3 = 0;

    // 0x...28, used in IS setup, meaning unclear
    static unsigned int last_conf = 0;

    static uint8_t resp_data[3];
    static unsigned int resp_bytes = 0;

    switch(address & 0xFF)
    {
        case 0x04:
            if((type & MODE_WRITE) && (value & 1))
            {
                static char default_msg[100];
                const char *extra_msg = "";
                // unclear what it should be if cmd isn't read, default to 0
                last_sio_rxdata = 0;
                // command
                switch(last_sio_txdata) {
                    // read response of last non-zero command
                    case 0:
                        // setup1 appears to specify the number of bits, but IS functions appear to always use 8
                        if(resp_bytes > 0) {
                            resp_bytes--;
                            last_sio_rxdata = resp_data[resp_bytes];
                            extra_msg = " read resp";
                        } else {
                            extra_msg = " unexpected read";
                        }
                        break;
                    // IS firmware checksum calculated in ffcf5bf8
                    case 0xF0:
                        extra_msg = " checksum";
                        resp_data[1] = 0x14;
                        resp_data[0] = 0x09;
                        resp_bytes = 2;
                        break;

                    // used in setup ffcf5bf8, no resp data
                    case 0XF1:
                        resp_bytes = 0;
                        extra_msg = " unk F1";
                        break;

                    case 0xF:
                        // horrible hack to make the logic around ffcf58e0 "work"
                        // following call to ffcf573c expects to get the id of the previous command,
                        // which in this case, happens to be 3
                        extra_msg = " unk F";
                        resp_data[0] = 3;
                        resp_bytes=1;
                        break;

                    default:
                        extra_msg = " unk cmd";
                        // FUN_ffcf5808 appears to expect 2 ignored reads (or 1 based on *(param+6)), followed by last command
                        // for at least 3, 6, 7, 0xa6
                        resp_data[2] = 0;
                        resp_data[1] = 0;
                        resp_data[0] = (uint8_t)last_sio_txdata;
                        resp_bytes=3;
                        break;
                }
                // firmware appears to expect an interrupt after each command, required to release semaphores
                // 0x37 appears to be the standard interrupt for SIO channel 4 (see ffc2d0b8), but is only used in early setup
                // and the default handler for 0x37 uses different semaphore and MMIO
                int int_num;
                if(last_conf == 1) {
                    // int that releases semaphore 0x55ac in IS setup function ffcf5bf8
                    // also default for SIO 4. Unclear whether interrupts are not generated after setup, or just ignored
                    int_num = 0x37;
                } else {
                    // int that releases semaphore 0x55a4 used by IS com functions ffcf57a0, ffcf573c and setup ffcf5bf8
                    int_num = 0x51;
                }
                // nasty hack:
                // without delay, TryTakeSemaphore in ffcf5f38 acquires sem 0x55ac, which causes subsequent calls to fail
                // 10 seemed to fail occasionally
                eos_trigger_int(int_num, 20);
                snprintf(default_msg, sizeof(default_msg),
                    "Transmit: CMD 0x%02X, setup 0x%08X 0x%08X 0x%08X 0x%08X INT %02x%s",
                    last_sio_txdata, last_sio_setup1, last_sio_setup2, last_sio_setup3,
                    last_conf, int_num, extra_msg
                );
                msg = default_msg;
            }
            else
            {
                // firmware waits for 0x04 to go to 0 after sending command
                msg = "TX done?";
                ret = 0;
            }
            break;

        case 0x0C:
            msg = "setup 1";
            MMIO_VAR(last_sio_setup1);
            break;

        case 0x10:
            msg = "setup 2";
            MMIO_VAR(last_sio_setup2);
            break;

        case 0x14:
            msg = "setup 3";
            MMIO_VAR(last_sio_setup3);
            break;

        case 0x18:
            msg = "TX register";
            MMIO_VAR(last_sio_txdata);
            break;

        case 0x1C:
            msg = "RX register";
            MMIO_VAR(last_sio_rxdata);
            break;

        case 0x28:
            msg = "IS conf?";
            MMIO_VAR(last_conf);
            break;
    }

    io_log("IS", address, type, value, ret, msg, 0, 0);
    return ret;
}

static unsigned int eos_handle_A1100_rtc(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    char msg[100] = "";

    static unsigned int last_sio_txdata = 0;
    static unsigned int last_sio_rxdata = 0;
    static unsigned int last_sio_setup1 = 0;
    static unsigned int last_sio_setup2 = 0;
    static unsigned int last_sio_setup3 = 0;
    unsigned int pc = CURRENT_CPU->env.regs[15];

    switch(address & 0xFF)
    {
        case 0x04:
            if((type & MODE_WRITE) && (value & 1))
            {
                uint8_t cmd = (last_sio_txdata >> 4) & 0x7;
                snprintf(msg, sizeof(msg), "Transmit: CMD 0x%X 0x%08X, setup 0x%08X 0x%08X 0x%08X PC: 0x%08X",cmd, last_sio_txdata, last_sio_setup1, last_sio_setup2, last_sio_setup3, pc);
                // command 2 reads back 7 bytes with date / time, called from A1100 100c ff845960
                // commands observed are 0,1,2,3,4,7 but only 2 appears needed to set clock
                // and avoid date/time prompt
                if(cmd == 2) {
                    // issue command
                    if(last_sio_setup1 & 0x80000000)
                    {
                        eos_state->rtc.current_reg = 0;
                    }
                    else  // fetch result
                    {
                        last_sio_rxdata = eos_state->rtc.regs[eos_state->rtc.current_reg];
                        eos_state->rtc.current_reg++;
                        eos_state->rtc.current_reg %= 7;
                    }
                }
                else
                {
                    last_sio_rxdata = 0;
                }
            }
            else
            {
                ret = 0;
            }
            break;

        case 0x0C:
            MMIO_VAR(last_sio_setup1);
            break;

        case 0x10:
            MMIO_VAR(last_sio_setup2);
            break;

        case 0x14:
            MMIO_VAR(last_sio_setup3);
            break;

        case 0x18:
            snprintf(msg, sizeof(msg), "TX register");
            MMIO_VAR(last_sio_txdata);
            break;

        case 0x1C:
            snprintf(msg, sizeof(msg), "RX register");
            MMIO_VAR(last_sio_rxdata);
            break;
    }

    io_log("RTC", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_sio(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    if ((address & 0xFFFFFF00) == 0xC0820400 && strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
        return eos_handle_A1100_IS_com(parm, address, type, value);
    }

    if (eos_state->sf && parm == eos_state->model->serial_flash_sio_ch)
    {
        /* serial flash (SFIO) */
        return eos_handle_sio_serialflash(parm, address, type, value);
    }

    // A1100 only treat SIO2 0xC08202** as RTC, unclear if other SIO could be active at same time
    // SIO2 appears to be shared with something else referenced from AudioTsk and StartupImage tasks
    if ((address & 0xFFFFFF00) == 0xC0820200 && strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
        if(eos_state->rtc.transfer_format != RTC_INACTIVE)
        {
            return eos_handle_A1100_rtc(parm, address, type, value);
        }
    }
    else if (eos_state->rtc.transfer_format != RTC_INACTIVE)
    {
        /* RTC CS active? */
        return eos_handle_rtc(parm, address, type, value);
    }

    /* unknown SIO device? generic handler */

    unsigned int ret = 0;
    char msg[100] = "";
    char mod[10];
    
    snprintf(mod, sizeof(mod), "SIO%i", parm);

    static unsigned int last_sio_txdata = 0;
    static unsigned int last_sio_rxdata = 0;
    static unsigned int last_sio_setup1 = 0;
    static unsigned int last_sio_setup2 = 0;
    static unsigned int last_sio_setup3 = 0;
    unsigned int pc = CURRENT_CPU->env.regs[15];

    switch(address & 0xFF)
    {
        case 0x04:
            if((type & MODE_WRITE) && (value & 1))
            {
                snprintf(msg, sizeof(msg), "Transmit: 0x%08X, setup 0x%08X 0x%08X 0x%08X PC: 0x%08X", last_sio_txdata, last_sio_setup1, last_sio_setup2, last_sio_setup3, pc);
            }
            else
            {
                ret = 0;
            }
            break;

        case 0x0C:
            MMIO_VAR(last_sio_setup1);
            break;

        case 0x10:
            MMIO_VAR(last_sio_setup2);
            break;

        case 0x14:
            MMIO_VAR(last_sio_setup3);
            break;

        case 0x18:
            snprintf(msg, sizeof(msg), "TX register");
            MMIO_VAR(last_sio_txdata);
            break;

        case 0x1C:
            snprintf(msg, sizeof(msg), "RX register");
            MMIO_VAR(last_sio_rxdata);
            break;
    }

    io_log(mod, address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_digic_timer(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = "DIGIC clock";

    if(type & MODE_WRITE)
    {
    }
    else
    {
        if (parm) {
            ret = eos_state->digic_timer32_last_read = eos_state->digic_timer32;
        } else {
            ret = eos_state->digic_timer20_last_read = eos_state->digic_timer20;
        }

        if (!(qemu_loglevel_mask(CPU_LOG_INT) &&
              qemu_loglevel_mask(EOS_LOG_VERBOSE)))
        {
            return ret; /* be quiet */
        }
    }

    io_log("TIMER", address, type, value, ret, msg, 0, 0);
    return ret;
}

/*
wrapper for sd_do_command to handle special cases. Currently, A1100
*/
static int eos_sd_do_command(SDState *sd, SDRequest *req, uint8_t *response)
{
/*
    A1100 expects to send at least two CMD55 (APP_CMD), ACMD41 (SD_SEND_OP_COND)
    sequences at startup.
    SD1stInit task ffce41b4 calls ffde26b0 to send CMD55, ffde278c to send ACMD41
    Startup task ffce42d8 calls ffde27e0, which calls ffde26b0 to send CMD55, then
    sends ACMD41
    Qemu sd.c considers CMD55 and ACMD41 illegal after the 'ready' state is entered,
    and with the parameters A1100 uses, the card enters 'ready' on the first
    ACMD41. This causes sd.c to return an error result on the second CMD55, which in
    turn causes the global SD error flag (0x2628 == 0) to be set in the Canon firmware,
    which disables subsequent SD access and displays "Memory Card Error" on the screen
    The workaround intercepts the extra CMD55, ACMD41 sequence without sending to the sd
    code, and sends a canned response from the initial calls
    Adjusting logic in sd.c might be preferable, but the correct logic is unclear,
    and this avoids impact on other cameras
    Other PowerShots of similar generation likely require the same workaround
*/
    if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
        static uint8_t last_acmd41_resp[20];
        static uint8_t last_cmd55_resp[20];
        static enum {
            ACMD41_NONE,
            ACMD41_GOT55,
            ACMD41_GOT41,
            ACMD41_IGNORE41,
        } acmd41_state = ACMD41_NONE;
        switch(acmd41_state) {
            case ACMD41_NONE:
                // first 55, send command and save response
                if(req->cmd == 55) {
                    acmd41_state = ACMD41_GOT55;
                    int rlen = sd_do_command(sd, req, response);
                    memcpy(last_cmd55_resp,response,sizeof(last_cmd55_resp));
                    return rlen;
                }
                break;
            case ACMD41_GOT55:
                // first 41, send command and save response
                if(req->cmd == 41) {
                    acmd41_state = ACMD41_GOT41;
                    int rlen = sd_do_command(sd, req, response);
                    memcpy(last_acmd41_resp,response,sizeof(last_acmd41_resp));
                    return rlen;
                }
                break;
            case ACMD41_GOT41:
                // 55 following 41, ignore and set to ignore next 41
                // note this would break things if a *different* ACMD were sent immediately after 41
                // but qemu would flag that as illegal anyway
                if(req->cmd == 55) {
                    acmd41_state = ACMD41_IGNORE41;
                    memcpy(response,last_cmd55_resp,sizeof(last_cmd55_resp));
                    return 4;
                }
                break;
            case ACMD41_IGNORE41:
                // 41 following ignored 55, ignore
                // Note qemu did NOT instantly set the card to ready (bit 31=1, initialization complete)
                // this would fail, since it would send the non-busy response. But then
                // the whole workaround wouldn't be needed
                if(req->cmd == 41) {
                    // reset state. This limits to one extra 55/41 sequence, could drop back to GOT41 to
                    // allow multiple, but not needed on A1100
                    acmd41_state = ACMD41_NONE;
                    memcpy(response,last_acmd41_resp,sizeof(last_acmd41_resp));
                    return 4;
                }
                break;
            default:
                fprintf(stderr, "[EOS] invalid acmd41_state %d\n", acmd41_state);
                assert(0);

        }
        // anything else, reset sequence and send normal command
        acmd41_state = ACMD41_NONE;
    }
    return sd_do_command(sd, req, response);
}

/* based on pl181_send_command from hw/sd/pl181.c */
#define SD_EPRINTF(fmt, ...) EPRINTF("[SDIO] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define SD_DPRINTF(fmt, ...) DPRINTF("[SDIO] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define SDIO_STATUS_OK              0x1
#define SDIO_STATUS_ERROR           0x2
#define SDIO_STATUS_DATA_AVAILABLE  0x200000

static void sdio_send_command(SDIOState *sd)
{
    SDRequest request;
    uint8_t response[24] = {0};
    int rlen;

    uint32_t cmd_hi = sd->cmd_hi;
    uint32_t cmd = (cmd_hi >> 8) & ~0x40;
    uint64_t param_hi = sd->cmd_hi & 0xFF;
    uint64_t param_lo = sd->cmd_lo >> 8;
    uint64_t param = param_lo | (param_hi << 24);

    request.cmd = cmd;
    request.arg = param;
    SD_DPRINTF("Command %d %08x\n", request.cmd, request.arg);
    rlen = eos_sd_do_command(sd->card, &request, response+4);
    if (rlen < 0)
        goto error;

    if (sd->cmd_flags != 0x11) {
#define RWORD(n) (((uint32_t)response[n] << 24) | (response[n + 1] << 16) \
                  | (response[n + 2] << 8) | response[n + 3])
        if (rlen == 0)
            goto error;
        if (rlen != 4 && rlen != 16)
            goto error;
        
        if (rlen == 4) {
            /* response bytes are shifted by one, but only for rlen=4 ?! */
            sd->response[0] = RWORD(5);
            sd->response[1] = RWORD(1);
            sd->response[2] = sd->response[3] = 0;
        } else {
            sd->response[0] = RWORD(16);
            sd->response[1] = RWORD(12);
            sd->response[2] = RWORD(8);
            sd->response[3] = RWORD(4);
        }
        SD_DPRINTF("Response received\n");
        sd->status |= SDIO_STATUS_OK;
#undef RWORD
    } else {
        SD_DPRINTF("Command sent\n");
        sd->status |= SDIO_STATUS_OK;
    }
    return;

error:
    SD_EPRINTF("Error\n");
    sd->status |= SDIO_STATUS_ERROR;
}

/* inspired from pl181_fifo_run from hw/sd/pl181.c */
/* only DMA transfers implemented */
static void sdio_read_data(void)
{
    SDIOState *sd = &eos_state->sd;
    int i;

    if (sd->status & SDIO_STATUS_DATA_AVAILABLE)
    {
        SD_EPRINTF("ERROR: read already done (%x)\n", sd->status);
        return;
    }
    
    if (!sd_data_ready(sd->card))
    {
        SD_EPRINTF("ERROR: no data available\n");
        return;
    }

    if (!sd->dma_enabled)
    {
        SD_EPRINTF("Reading %dx%d bytes without DMA (not implemented)\n", sd->transfer_count, sd->read_block_size);
        for (i = 0; i < sd->transfer_count * sd->read_block_size; i++)
        {
            /* dummy read, ignore this data */
            /* todo: send it on the 0x6C register? */
            sd_read_data(sd->card);
        }
        return;
    }

    SD_DPRINTF("Reading %d bytes to %x\n", sd->dma_count, sd->dma_addr);

    for (i = 0; i < sd->dma_count/4; i++)
    {
        uint32_t value1 = sd_read_data(sd->card);
        uint32_t value2 = sd_read_data(sd->card);
        uint32_t value3 = sd_read_data(sd->card);
        uint32_t value4 = sd_read_data(sd->card);
        uint32_t value = (value1 << 0) | (value2 << 8) | (value3 << 16) | (value4 << 24);
        
        uint32_t addr = sd->dma_addr + i*4; 
        eos_mem_write(addr, &value, 4);
    }

    sd->status |= SDIO_STATUS_DATA_AVAILABLE;
    sd->dma_transferred_bytes = sd->dma_count;
}

static void sdio_write_data(void)
{
    SDIOState *sd = &eos_state->sd;
    int i;

    if (sd->status & SDIO_STATUS_DATA_AVAILABLE)
    {
        SD_EPRINTF("ERROR: write already done (%x)\n", sd->status);
        return;
    }

    if (!sd->dma_enabled)
    {
        SD_EPRINTF("ERROR!!! Writing %dx%d bytes without DMA (not implemented)\n", sd->transfer_count, sd->read_block_size);
        SD_EPRINTF("Cannot continue without risking corruption on the SD card image.\n");
        exit(1);
    }

    SD_DPRINTF("Writing %d bytes from %x\n", sd->dma_count, sd->dma_addr);

    for (i = 0; i < sd->dma_count/4; i++)
    {
        uint32_t addr = sd->dma_addr + i*4; 
        uint32_t value;
        eos_mem_read(addr, &value, 4);
        
        sd_write_data(sd->card, (value >>  0) & 0xFF);
        sd_write_data(sd->card, (value >>  8) & 0xFF);
        sd_write_data(sd->card, (value >> 16) & 0xFF);
        sd_write_data(sd->card, (value >> 24) & 0xFF);
    }

    /* not sure */
    sd->status |= SDIO_STATUS_DATA_AVAILABLE;
    sd->dma_transferred_bytes = sd->dma_count;
}

static void sdio_trigger_interrupt(void)
{
    SDIOState *sd = &eos_state->sd;

    /* after a successful operation, trigger interrupt if requested */
    if ((sd->cmd_flags == 0x13 || sd->cmd_flags == 0x14)
        && !(sd->status & SDIO_STATUS_DATA_AVAILABLE))
    {
        /* if the current command does a data transfer, don't trigger until complete */
        SD_DPRINTF("Warning: data transfer not yet complete\n");
        return;
    }

    if (!sd->irq_flags)
    {
        /* no interrupt requested */
        return;
    }

    if (sd->status & 3)
    {
        assert(eos_state->model->sd_driver_interrupt);
        eos_trigger_int(eos_state->model->sd_driver_interrupt, 0);
        
        if (sd->dma_enabled)
        {
            assert(eos_state->model->sd_dma_interrupt);
            eos_trigger_int(eos_state->model->sd_dma_interrupt, 0);
        }
    }
    else if (sd->status)
    {
        SD_DPRINTF("Warning: not triggering interrupt (status=%x)\n", sd->status);
    }
}

unsigned int eos_handle_sdio(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    intptr_t msg_arg1 = 0;
    intptr_t msg_arg2 = 0;

    if (0)
    {
        /* FIXME: only working in bootloader context */
        assert(eos_state->clock_enable & 0x00000008);   /* DIGIC 3/4 */
        assert(eos_state->clock_enable & 0x10000000);   /* DIGIC 4, but not 4+ */
    }

    switch(address & 0xFFF)
    {
        case 0x08:
            msg = "DMA";
            MMIO_VAR(eos_state->sd.dma_enabled);
            break;

        case 0x0C:
            msg = "Command flags?";
            if(type & MODE_WRITE)
            {
                /* must return 0? something else?
                 * maybe clear some flags after executing a command?
                 */
                eos_state->sd.cmd_flags = value;
                
                /* reset status before doing any command */
                eos_state->sd.status = 0;
                
                /* interpret this command */
                sdio_send_command(&eos_state->sd);
                
                if (value == 0x14 || value == 0x4)
                {
                    /* read transfer */
                    eos_state->sd.pio_transferred_bytes = 0;
                    eos_state->sd.dma_transferred_bytes = 0;
                    
                    if (eos_state->sd.dma_enabled)
                    {
                        /* DMA read transfer */
                        sdio_read_data();
                        sdio_trigger_interrupt();
                    }
                    else
                    {
                        /* PIO read transfer */
                        eos_state->sd.status |= SDIO_STATUS_DATA_AVAILABLE;
                    }
                }
                else
                {
                    if (value == 0x13)
                    {
                        /* write transfer */
                        eos_state->sd.pio_transferred_bytes = 0;
                        eos_state->sd.dma_transferred_bytes = 0;
                    }

                    /* non-data or write transfer */
                    sdio_trigger_interrupt();
                }
            }
            else
            {
                ret = 0x10;
            }
            break;

        case 0x10:
            msg = "Status";
            /**
             * 0x00000001 => command complete
             * 0x00000002 => error
             * 0x00200000 => data available?
             **/
            if(type & MODE_WRITE)
            {
                /* writes to this register appear to clear status bits */
                eos_state->sd.status &= value;
            }
            else
            {
                ret = eos_state->sd.status;
            }
            break;

        case 0x14:
            msg = "irq enable?";
            MMIO_VAR(eos_state->sd.irq_flags);
            /* A1100 uses both the SDDMA reg 0x10 as described below and this one, break to avoid double write */
            if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
                break;
            }

            /* sometimes, a write command ends with this register
             * other times, it ends with SDDMA register 0x10 (mask 0x1F)
             */
            if (eos_state->sd.cmd_flags == 0x13 && value)
            {
                sdio_write_data();
            }

            /* sometimes this register is configured after the transfer is started */
            /* since in our implementation, transfers are instant, this would miss the interrupt,
             * so we trigger it from here too. */
            sdio_trigger_interrupt();
            break;

        case 0x18:
            msg = "init?";
            break;

        case 0x20:
            msg = "cmd_lo";
            MMIO_VAR(eos_state->sd.cmd_lo);
            break;

        case 0x24:
            msg = "cmd_hi";
            MMIO_VAR(eos_state->sd.cmd_hi);
            break;

        case 0x28:
            msg = "Response size (bits)";
            break;

        case 0x2c:
            msg = "response setup?";
            break;

        case 0x34:
            msg = "Response[0]";
            ret = eos_state->sd.response[0];
            break;

        case 0x38:
            msg = "Response[1]";
            ret = eos_state->sd.response[1];
            break;

        case 0x3C:
            msg = "Response[2]";
            ret = eos_state->sd.response[2];
            break;

        case 0x40:
            msg = "Response[3]";
            ret = eos_state->sd.response[3];
            break;

        case 0x58:
            msg = "bus width";
            break;

        case 0x5c:
            msg = "write block size";
            MMIO_VAR(eos_state->sd.write_block_size);
            break;

        case 0x64:
            msg = "bus width";
            break;

        case 0x68:
            msg = "read block size";
            MMIO_VAR(eos_state->sd.read_block_size);
            break;

        case 0x6C:
            msg = "FIFO data";
            if(type & MODE_WRITE)
            {
            }
            else
            {
                if (sd_data_ready(eos_state->sd.card))
                {
                    uint32_t value1 = sd_read_data(eos_state->sd.card);
                    uint32_t value2 = sd_read_data(eos_state->sd.card);
                    uint32_t value3 = sd_read_data(eos_state->sd.card);
                    uint32_t value4 = sd_read_data(eos_state->sd.card);
                    uint32_t value = (value1 << 0) | (value2 << 8) | (value3 << 16) | (value4 << 24);
                    ret = value;
                    eos_state->sd.pio_transferred_bytes += 4;
                    
                    /* note: CMD18 does not report !sd_data_ready when finished */
                    if (eos_state->sd.pio_transferred_bytes
                        >= eos_state->sd.transfer_count * eos_state->sd.read_block_size)
                    {
                        SD_DPRINTF("PIO transfer completed.\n");
                        eos_state->sd.status |= SDIO_STATUS_DATA_AVAILABLE;
                        eos_state->sd.status |= SDIO_STATUS_OK;
                        sdio_trigger_interrupt();
                    }
                }
                else
                {
                    SD_EPRINTF("PIO: no data available.\n");
                }
            }
            break;

        case 0x70:
            msg = "transfer status?";
            break;

        case 0x7c:
            msg = "transfer block count";
            MMIO_VAR(eos_state->sd.transfer_count);
            break;

        case 0x80:
            msg = "transferred blocks";
            /* Goro is very strong. Goro never fails. */
            ret = eos_state->sd.transfer_count;
            break;

        case 0x84:
            msg = "SDREP: Status register/error codes";
            break;

        case 0x88:
            msg = "SDBUFCTR: Set to 0x03 before reading";
            break;

        case 0xD4:
            msg = "Data bus monitor (?)";
            break;
    }

    io_log("SDIO", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

unsigned int eos_handle_sddma(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    switch(address & 0x1F)
    {
        case 0x00:
            msg = "Transfer memory address";
            MMIO_VAR(eos_state->sd.dma_addr);
            break;
        case 0x04:
            msg = "Transfer byte count";
            if (type & MODE_WRITE)
            {
                eos_state->sd.dma_count = value;
            }
            else
            {
                ret = (eos_state->sd.dma_enabled)
                    ? eos_state->sd.dma_transferred_bytes
                    : eos_state->sd.pio_transferred_bytes;
                
                /* fixme: M3 fails with the above */
                ret = 0;
            }
            break;
        case 0x10:
            msg = "Command/Status?";
            if (type & MODE_WRITE)
            {
                eos_state->sd.dma_enabled = value & 1;

                /* DMA transfer? */
                if (eos_state->sd.cmd_flags == 0x13 && eos_state->sd.dma_enabled)
                {
                    sdio_write_data();
                    sdio_trigger_interrupt();
                }
            }
            break;
        case 0x14:
            msg = "Status?";
            ret = (eos_state->sd.dma_enabled) ? 0x81 : 0;
            break;
        case 0x18:
            break;
    }

    io_log("SDDMA", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_sddma_dx(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = 0;

    switch(address & 0x1FFF)
    {
        /* There are many more registers written (as compared to Digic 8)
         * but only those were sufficient to run autoexec on R6 */
        case 0x1208:
            /* This very differs from older SDDMA implementation.
             * Now instead of multiple commands with specific values, just one
             * is used. Value is ram address to SDDMAInfo structure, which
             * holds all the details.
             */
            msg = "Command";
            if (type & MODE_WRITE)
            {
                SDDMAInfo dmainfo;
                // read structure from address
                eos_mem_read((hwaddr)value, &dmainfo, sizeof(SDDMAInfo));
                SD_DPRINTF("DMA CMD: %x ADDR: %x \n", dmainfo.cmd, dmainfo.dma_addr);

                eos_state->sd.dma_enabled = dmainfo.dma_enable & 1; // I'm not sure if this field is correct
                eos_state->sd.dma_count = dmainfo.block_count;
                eos_state->sd.dma_addr = dmainfo.dma_addr;

                /* DMA transfer? */
                if (eos_state->sd.cmd_flags == 0x13 && eos_state->sd.dma_enabled)
                {
                    sdio_write_data();
                    sdio_trigger_interrupt();
                }
            }
            break;
        case 0x1004:
            msg = "Status?";
            /* No idea what this does. Early on 0x3FFF is written there;
             * Later when value is read, 0x3FFF meets the criteria that are
             * needed to progress.
             * Maybe it should be MMIO variable, hard to tell for now.*/
            ret = 0x3fff;
            break;
        case 0x18:
            break;
    }

    io_log("SDDMA_X", address, type, value, ret, msg, 0, 0);
    return ret;
}

#undef SD_DPRINTF
#undef SD_EPRINTF

#define CFD_EPRINTF(fmt, ...) EPRINTF("[CFDMA] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define CFD_DPRINTF(fmt, ...) DPRINTF("[CFDMA] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define CFD_VPRINTF(fmt, ...) VPRINTF("[CFDMA] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define CFA_EPRINTF(fmt, ...) EPRINTF("[CFATA] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)
#define CFA_DPRINTF(fmt, ...) DPRINTF("[CFATA] ", EOS_LOG_SDCF, fmt, ## __VA_ARGS__)

static int cfdma_read_data(CFState *cf)
{
    CFD_DPRINTF("Reading %d of %d bytes to %x\n", cf->dma_count - cf->dma_read, cf->dma_count, cf->dma_addr + cf->dma_read);

    assert(cf->dma_count % 4 == 0);
    
    /* for some reason, reading many values in a loop sometimes fails */
    /* in this case, the status register has the DRQ bit cleared */
    /* and we need to wait until new data arrives in the buffer */
    while ((cf->dma_read < cf->dma_count) &&
           (ide_status_read(&cf->bus, 0) & 0x08))    /* DRQ_STAT */
    {
        uint32_t value = ide_data_readl(&cf->bus, 0);
        uint32_t addr = cf->dma_addr + cf->dma_read; 
        eos_mem_write(addr, &value, 4);
        CFD_VPRINTF("%08x: %08x\n", addr, value);
        cf->dma_read += 4;
    }

    cf->dma_wait--;

    if (cf->dma_read == cf->dma_count && cf->dma_wait <= 0)
    {
        /* finished? */
        assert(cf->dma_wait == 0 || !use_icount);
        cfdma_trigger_interrupt();
        return 0;
    }

    return 1;
}

static int cfdma_write_data(CFState *cf)
{
    CFD_DPRINTF("Writing %d of %d bytes from %x\n", cf->dma_count - cf->dma_written, cf->dma_count, cf->dma_addr + cf->dma_written);

    assert(cf->dma_count % 4 == 0);

    /* it appears to accept one sector at a time, for some reason */
    while ((cf->dma_written < cf->dma_count) &&
           (ide_status_read(&cf->bus, 0) & 0x08))    /* DRQ_STAT */
    {
        uint32_t value;
        uint32_t addr = cf->dma_addr + cf->dma_written; 
        eos_mem_read(addr, &value, 4);
        ide_data_writel(&cf->bus, 0, value);
        cf->dma_written += 4;
    }

    cf->dma_wait--;

    if (cf->dma_written == cf->dma_count && cf->dma_wait <= 0)
    {
        /* finished? */
        assert(cf->dma_wait == 0 || !use_icount);
        cfdma_trigger_interrupt();
        return 0;
    }

    return 1;
}

static void cfdma_trigger_interrupt(void)
{
    CFD_DPRINTF("trigger interrupt? %x\n", eos_state->cf.interrupt_enabled);

    if (eos_state->cf.interrupt_enabled & 0x2000001)
    {
        assert(eos_state->model->cf_driver_interrupt);
        eos_trigger_int(eos_state->model->cf_driver_interrupt, 0);
    }

    if (eos_state->cf.interrupt_enabled & 0x10000)
    {
        assert(eos_state->model->cf_dma_interrupt);
        eos_trigger_int(eos_state->model->cf_dma_interrupt, 0);
    }
}

unsigned int eos_handle_uart_dma(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    static uint32_t addr;
    static uint32_t count;
    static uint32_t status;

    switch(address & 0x1F)
    {
        case 0x00:
        case 0x08:
            msg = "Transfer memory address";
            MMIO_VAR(addr);
            break;

        case 0x04:
        case 0x0C:
            msg = "Transfer byte count";
            MMIO_VAR(count);
            break;

        case 0x10:
            msg = "Transfer command / status?";
            if (value == 0x10023)
            {
                /* read char? */
                count = 0;
                cpu_physical_memory_write(addr, &eos_state->uart.reg_rx, 1);
                status = 0x10;

                /* guess: initialization? */
                static int first_time = 1;
                if (first_time)
                {
                    if (!eos_state->model->uart_rx_interrupt)
                    {
                        fprintf(stderr, "FIXME: uart_rx_interrupt unknown\n");
                        break;
                    }

                    eos_trigger_int(eos_state->model->uart_rx_interrupt, 0);
                    first_time = 0;
                }
            }
            ret = 0x20;
            break;

        case 0x14:
            msg = "DMA status?";
            if (eos_state->uart.reg_st & ST_RX_RDY) {
                status |= 0x4;
            }
            MMIO_VAR(status);
            break;
    }

    io_log("UartDMA", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_adtg_dma(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    static uint32_t addr;
    static uint32_t count;
    static uint32_t status;

    switch(address & 0x1F)
    {
        case 0x00:
        case 0x08:
            msg = "Transfer memory address";
            MMIO_VAR(addr);
            break;

        case 0x04:
        case 0x0C:
            msg = "Transfer byte count";
            MMIO_VAR(count);
            break;

        case 0x10:
            msg = "Transfer command / status?";
            if (value == 0x3000025)
            {
                eos_trigger_int(0x37, 100);
            }
            break;

        case 0x14:
            msg = "DMA status?";
            MMIO_VAR(status);
            break;
    }

    io_log("ADTGDMA", address, type, value, ret, msg, 0, 0);
    return ret;
}

/*
A1100 appears to use MMIOs 0xc0500040-0xc0500058 to load optical image stabilization firmware, see ffcf5bf8
Note 0xc05000A0 - 0xc05000B0 are used for apparently similar transfers for other devices in ffc32830
*/
static unsigned int eos_handle_A1100_IS_init(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    static uint32_t xfer_addr;
    static uint32_t xfer_size;
    static uint32_t unk1;
    static int init_done = 0;
    switch(address & 0xFF)
    {
        case 0x40:
            msg = "Transfer memory address";
            MMIO_VAR(xfer_addr);
            break;
        case 0x44:
            msg = "Transfer byte count";
            MMIO_VAR(xfer_size);
            break;
        case 0x50:
            msg = "Unk1";
            MMIO_VAR(unk1);
            break;
        case 0x58:
            if(init_done) {
                msg = "ISInit unk2 after done";
            } else if (unk1 == 0x25 && xfer_addr && xfer_size) {
                msg = "ISInit unk2 trigger int";
                // int that releases semaphore 0x55a4 in ffcf5bf8, unclear whether actually
                // triggered by this transfer in real firmware
                // this int is also triggered from eos_handle_A1100_IS_com, but only later
                eos_trigger_int(0x51, 0);
                init_done = 1;
            } else {
                msg = "ISInit unk2 trigger not init";
            }
            break;
    }
    io_log("IS", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_cfdma(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    // A1100 uses 0xc0500040 - 58, related to IS system, see ffcf5bf8
    if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0
        && address >= 0xc0500040 &&  address <= 0xc0500058) {
        return eos_handle_A1100_IS_init(parm,address,type,value);
    }

    switch(address & 0x1F)
    {
        case 0x00:
            msg = "Transfer memory address";
            MMIO_VAR(eos_state->cf.dma_addr);
            break;
        case 0x04:
            msg = "Transfer byte count";
            if(type & MODE_WRITE)
            {
                eos_state->cf.dma_count = value;

                /* each iteration of cfdma_read_data / cfdma_write_data usually processes 1 block (512 bytes)
                 * however, a few iterations will just wait, nondeterministically, for unclear reasons
                 * to get deterministic execution with -icount, required for tests, we'll slow down the execution
                 * by forcing some more iterations than actually needed, so total times would be deterministic
                 * note: under heavy I/O load (e.g. parallel tests) we need to slow down a lot more! */
                eos_state->cf.dma_wait = (use_icount) ? value / 512 * 2 + 10 : 0;
            }
            else
            {
                ret = eos_state->cf.dma_read;
            }
            break;
        case 0x10:
            msg = "Unknown transfer command";
            
            if(type & MODE_WRITE)
            {
                if (value == 0x3D || value == 0x2D)
                {
                    msg = "DMA write start";
                    eos_state->cf.dma_written = 0;
                    eos_state->cf.dma_write_request = 1;
                }
                else if (value == 0x39 || value == 0x29 || value == 0x21)
                {
                    msg = "DMA read start";
                    eos_state->cf.dma_read = 0;
                    
                    /* for some reason, trying to read large blocks at once
                     * may fail; not sure what's the proper way to fix it
                     * workaround: do this in the interrupt timer callback,
                     * where we may retry as needed */
                    eos_state->cf.dma_read_request = 1;
                }
            }
            break;
        case 0x14:
            msg = "DMA status?";
            ret = 3;
            break;
    }

    io_log("CFDMA", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_cfata(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    intptr_t msg_arg1 = 0;
    intptr_t msg_arg2 = 0;

    if (0)
    {
        /* DIGIC 4 and earlier */
        /* FIXME: only working in bootloader context */
        assert(eos_state->clock_enable & 0x40);
    }

    switch(address & 0xFFFF)
    {
        case 0x8104:
            msg = "CFDMA ready maybe?";
            ret = (eos_state->cf.dma_read_request || eos_state->cf.dma_write_request) ? 0 : 4;
            break;
        
        case 0x8040:
            msg = "Interrupt enable?";
            MMIO_VAR(eos_state->cf.interrupt_enabled);
            break;
        
        case 0x8044:
            msg = "Interrupt related?";
            if(type & MODE_WRITE)
            {
            }
            else
            {
                /* should return what was written to 0x8040?! */
                ret = eos_state->cf.interrupt_enabled;
            }
            break;

        case 0x8048:
            msg = "DMA interrupt enable?";
            if(type & MODE_WRITE)
            {
                if (value & 1) {
                    eos_state->cf.interrupt_enabled |= 0x10000;
                } else {
                    eos_state->cf.interrupt_enabled &= ~0x10000;
                }
            }
            break;

        case 0x21F0:
        case 0x2000:
            msg = "ATA data port";
            
            if(type & MODE_WRITE)
            {
                ide_data_writew(&eos_state->cf.bus, 0, value);

                if (!qemu_loglevel_mask(EOS_LOG_SDCF)) {
                    return 0;
                }
            }
            else
            {
                ret = ide_data_readw(&eos_state->cf.bus, 0);

                if (!qemu_loglevel_mask(EOS_LOG_SDCF)) {
                    return ret;
                }
            }
            break;

        case 0x21F1:
        case 0x21F2:
        case 0x21F3:
        case 0x21F4:
        case 0x21F5:
        case 0x21F6:
        case 0x21F7:
        case 0x2001:
        case 0x2002:
        case 0x2003:
        case 0x2004:
        case 0x2005:
        case 0x2006:
        case 0x2007:
        {
            int offset = address & 0xF;

            const char * regnames[16] = {
                [1] = "ATA feature/error",
                [2] = "ATA sector count",
                [3] = "ATA LBAlo",
                [4] = "ATA LBAmid",
                [5] = "ATA LBAhi",
                [6] = "ATA drive/head port",
                [7] = "ATA command/status",
            };
            msg = regnames[offset];

            if(type & MODE_WRITE)
            {
                ide_ioport_write(&eos_state->cf.bus, offset, value);
                if (offset == 7 && eos_state->cf.ata_interrupt_enabled)
                {
                    /* a command for which interrupts were requested? */
                    eos_state->cf.pending_interrupt = 1;
                }
            }
            else
            {
                ret = ide_ioport_read(&eos_state->cf.bus, offset);
                if (offset == 7)
                {
                    /* reading the status register clears peding interrupt */
                    /* unsure actually - 40D doesn't like this */
                    //s->cf.pending_interrupt = 0;
                }
            }
            break;
        }

        case 0x23F6:
        case 0x200E:
            if(type & MODE_WRITE)
            {
                msg = "ATA device control: int %s%s";
                msg_arg1 = (intptr_t) ((value & 2) ? "disable" : "enable");
                msg_arg2 = (intptr_t) ((value & 4) ? ", soft reset" : "");
                ide_cmd_write(&eos_state->cf.bus, 0, value & 2);
                eos_state->cf.ata_interrupt_enabled = !(value & 2);
            }
            else
            {
                msg = "ATA alternate status";
                ret = ide_status_read(&eos_state->cf.bus, 0);
            }
            break;
    }
    io_log("CFATA", address, type, value, ret, msg, msg_arg1, msg_arg2);
    return ret;
}

#undef CFA_DPRINTF
#undef CFA_EPRINTF
#undef CFD_DPRINTF
#undef CFD_EPRINTF

static char *format_clock_enable(int value)
{
    const char *clock_modules[] = {
        "???",  "LCLK", "ASIF?", "SD1",     // 1 2 4 8
        "???",  "???",  "CF",    "???",     // 10 20 40 80
        "PWM",  "???",  "Tmr0",  "Tmr1",    // 100 200 400 800
        "Tmr2", "???",  "???",   "???",     // ...
        "???",  "???",  "???",   "???",
        "???",  "SIO",  "???",   "???",
        "DMA0", "ASIF", "???",   "???",
        "SD2",  "???",  "???",   "???"
    };
    static char clock_msg[100];
    snprintf(clock_msg, sizeof(clock_msg), "CLOCK_ENABLE: ");
    int i;
    for (i = 0; i < 32; i++)
    {
        if (value & (1u << i))
        {
            STR_APPEND(clock_msg, "%s ", clock_modules[i]);
        }
    }
    return clock_msg;
}

unsigned int eos_handle_basic(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;
    
    /* from C0100000 */
    if (parm == 0)
    {
        switch(address & 0xFFF)
        {
            case 0x00C:
            {
                /* 40D: expects 2 at 0xFF819AA0 */
                /* GUI locks up without it after entering PowerSave */
                msg = "Powersave related?";
                ret = 2;
                break;
            }

            case 0x01C:
            {
                /* 5D classic: expects 1 at 0xFFFF01A4 */
                ret = 1;
                break;
            }

            case 0x110:
            {
                /* 1300D: expects 0x80000000 at 0xFE0C038C */
                /* GUI locks up without it after entering PowerSave */
                msg = "Powersave related?";
                ret = 0x80000000;
                break;
            }
        }
        io_log("BASIC", address, type, value, ret, msg, 0, 0);
        return ret;
    }

    /* from C0720000 */
    if (parm == 2)
    {
        if ((address & 0xFFF) == 8)
        {
            msg = "SUSPEND_BIT";
            ret = 0x100;
            io_log("BASIC", address, type, value, ret, msg, 0, 0);
        }
        return ret;
    }
    
    /* from C0400000 */
    switch(address & 0xFFF)
    {
        case 0x008: /* CLOCK_ENABLE */
            MMIO_VAR(eos_state->clock_enable);
            msg = format_clock_enable(eos_state->clock_enable);
            break;
        
        case 0xA4:
            /* A1100: expects 3 at 0xFFFF0060 */
            msg = "A1100 init";
            ret = 3;
            break;
        
        case 0x244:
            /* idk, expected to be so in 5D3 123 */
            ret = 1;
            break;
        case 0x204:
            /* idk, expected to be so in 5D3 bootloader */
            ret = 2;
            break;
        
        case 0x284:
            msg = "5D3 display init?";
            ret = 1;
            break;
    }

    io_log("BASIC", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_asif(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;

    if(type & MODE_WRITE)
    {
    }
    else
    {
        switch(address & 0xFFF)
        {
            case 0x110:
            case 0x114:
                /* audio meters; don't print messages, since it will flood the console */
                return 0;
        }
    }

    io_log("ASIF", address, type, value, ret, 0, 0, 0);
    return ret;
}

static void process_palette_entry(uint32_t value, struct palette_entry *palette_entry, int palette_index, const char **msg)
{
    uint32_t pal = value;

    int opacity = (pal >> 24) & 0xFF;
    uint8_t Y = (pal >> 16) & 0xFF;
    int8_t  U = (pal >>  8) & 0xFF;
    int8_t  V = (pal >>  0) & 0xFF;
    int R, G, B;
    yuv2rgb(Y, U, V, &R, &G, &B);

    static char msg_pal[64];

    snprintf(msg_pal, sizeof(msg_pal), 
        "Palette[%X] -> R%03d G%03d B%03d %s",
        palette_index, R, G, B,
        opacity == 3 ? "" : 
            pal == 0x00FF0000 ? "transparent" :
            pal == 0x00000000 ? "transparent black" :
            opacity == 1 &&
             R == G && G == B ? "transparent gray" :
                                "transparent?"
    );
    *msg = msg_pal;

    palette_entry->R = R;
    palette_entry->G = G;
    palette_entry->B = B;
    palette_entry->opacity = opacity;
}

unsigned int eos_handle_display(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int ret = 0;
    const char *msg = NULL;

    if (parm == 1)
    {
        /* 5D3 1.2.3 - only handle palette registers
         * other registers are not the same as before */
        switch (address & 0xFFF)
        {
            case 0x400 ... 0x7FC:
            case 0x800 ... 0xBFC:
                break;

            default:
                goto end;
        }
    }

    switch (address & 0xFFF)
    {
        case 0x014:
        {
            // A1100 expects 0x4 or 0x8, 0x4 appears to be normal case in INT 0x68 handler ffc2ba64
            // avoids assert from TakeSemaphoreStrictly(0x2680) in ffc404e8
            if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
                ret = 0x4;
            } else {
            /* 5D3 1.2.3: expects 0x10 for built-in LCD and 0x4 for HDMI? */
                ret = 0x10;
            }
            break;
        }

        case 0x01C:
        case 0x31C:
        {
            /* not sure this is the register that actually triggers the interrupt */
            msg = "interrupt enable?";
            if (value == 0) {
                /* nothing to do */
            } else if (value == 4 || value == 0x14) {
                /* 60D: EnableBitmapVBufferForPlayBackAndWait */
                /* 5D3 1.2.3 writes 0x14 to 0xC0F1431C */
                eos_trigger_int(0x68, 10);
            } else {
                assert(0);
            }
            break;
        }   

        case 0x0D0: /* most models */
        case 0x0D4: /* double-buffered? */
        case 0x350: /* 5D3 1.2.3 */
        case 0x354:
            msg = "BMP VRAM";
            MMIO_VAR(eos_state->disp.bmp_vram);
            break;

        case 0x0E0:
        case 0x0E4:
            msg = "YUV VRAM";
            MMIO_VAR(eos_state->disp.img_vram);
            break;

        case 0x080 ... 0x0BC:
            msg = "4-bit palette";
            if(type & MODE_WRITE)
            {
                int entry = ((address & 0xFFF) - 0x80) / 4;
                process_palette_entry(value, &eos_state->disp.palette_4bit[entry], entry, &msg);
                eos_state->disp.is_4bit = 1;
                eos_state->disp.bmp_pitch = 360;
            }
            break;

        case 0x400 ... 0x7FC:
        case 0x800 ... 0xBFC:
            msg = "8-bit palette";
            if(type & MODE_WRITE)
            {
                int entry = (((address & 0xFFF) - 0x400) / 4) % 0x100;
                process_palette_entry(value, &eos_state->disp.palette_8bit[entry], entry, &msg);
                eos_state->disp.is_4bit = 0;
                if (strcmp(eos_state->model->name, MODEL_NAME_A1100) == 0) {
                    eos_state->disp.bmp_pitch = 720;
                } else {
                    eos_state->disp.bmp_pitch = 960;
                }
            }
            break;
    }

end:
    io_log("Display", address, type, value, ret, msg, 0, 0);
    return ret;
}

#define FLASH_STATE_READ      0
#define FLASH_STATE_UNLOCK_2  1
#define FLASH_STATE_UNLOCKED  2
#define FLASH_STATE_ERASE_1   3
#define FLASH_STATE_ERASE_2   4
#define FLASH_STATE_ERASE_3   5
#define FLASH_STATE_PROGRAM   6
#define FLASH_STATE_UNLOCK_BYPASS   7
#define FLASH_STATE_UNLOCK_BYPASS_RESET 8
#define FLASH_STATE_UNLOCK_BYPASS_ERASE 9
#define FLASH_STATE_BLOCK_ERASE_BUSY 10

unsigned int flash_get_blocksize(unsigned int rom, unsigned int size, unsigned int word_offset)
{
    switch(size)
    {
        /* 32mbit flash x16 */
        case 0x00400000:
            if((word_offset < 0x8000) || (word_offset > 0x1F0000))
            {
                /* 4 kwords */
                return 4 * 1024 * 2;
            }
            else
            {
                /* 32 kwords */
                return 32 * 1024 * 2;
            }
            break;

        default:
            return 0;
    }
}

unsigned int eos_handle_rom(unsigned int rom, unsigned int address, unsigned char type, unsigned int value)
{
    unsigned int pc = CURRENT_CPU->env.regs[15];
    unsigned int ret = 0;
    unsigned int real_address = 0;
    unsigned int byte_offset = 0;
    unsigned int word_offset = 0;
    unsigned int base = 0;
    unsigned int size = 0;
    unsigned int fail = 0;

    static int block_erase_counter = 0;
    static int state[2] = {FLASH_STATE_READ, FLASH_STATE_READ};

    switch(rom)
    {
        case 0:
            base = 0xF8000000;
            size = ROM1_SIZE;
            break;
        case 1:
            base = 0xF0000000;
            size = ROM0_SIZE;
            break;
    }

    /* the offset relative from flash chip start */
    byte_offset = (address - base) & (size - 1);
    word_offset = byte_offset >> 1;

    /* the address of the flash data in memory space */
    real_address = base + byte_offset;

    if(!eos_state->flash_state_machine)
    {
        return eos_default_handle(real_address, type, value);
    }

    if(type & MODE_WRITE)
    {
        switch(state[rom])
        {
            case FLASH_STATE_READ:
                if(value == 0xF0)
                {
                    state[rom] = FLASH_STATE_READ;
                }
                else if(word_offset == 0x555 && value == 0xAA)
                {
                    state[rom] = FLASH_STATE_UNLOCK_2;
                }
                else if(value == 0xA0)
                {
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS PROGRAM\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_PROGRAM;
                }
                else if(value == 0x80)
                {
                    state[rom] = FLASH_STATE_UNLOCK_BYPASS_ERASE;
                }
                else if(value == 0x90)
                {
                    state[rom] = FLASH_STATE_UNLOCK_BYPASS_RESET;
                }
                else if(value == 0x98)
                {
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS CFI unhandled\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_BYPASS:
                fprintf(stderr, "[ROM%i:%i] at [0x%04X]       2nd UNLOCK BYPASS [0x%08X] -> [0x%08X] unhandled\r\n", rom, state[rom], pc, value, word_offset);
                state[rom] = FLASH_STATE_READ;
                break;


            case FLASH_STATE_UNLOCK_BYPASS_RESET:
                if(value == 0x00)
                {
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS RESET\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_2:
                if(word_offset == 0x2AA && value == 0x55)
                {
                    state[rom] = FLASH_STATE_UNLOCKED;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCKED:
                if(value == 0x90)
                {
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] [0x%08X] -> [0x%08X] in autoselect unhandled\r\n", rom, state[rom], pc, value, word_offset);
                    state[rom] = FLASH_STATE_READ;
                }
                else if(word_offset == 0x555 && value == 0xA0)
                {
                    //fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: PROGRAM\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_PROGRAM;
                }
                else if(word_offset == 0x555 && value == 0x20)
                {
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else if(word_offset == 0x555 && value == 0x80)
                {
                    state[rom] = FLASH_STATE_ERASE_1;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_1:
                if(word_offset == 0x555 && value == 0xAA)
                {
                    state[rom] = FLASH_STATE_ERASE_2;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_2:
                if(word_offset == 0x2AA && value == 0x55)
                {
                    state[rom] = FLASH_STATE_ERASE_3;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_BYPASS_ERASE:
                if(value == 0x30)
                {
                    int pos = 0;
                    int block_size = flash_get_blocksize(rom, size, word_offset);

                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS BLOCK ERASE [0x%08X]\r\n", rom, state[rom], pc, real_address);
                    for(pos = 0; pos < block_size; pos += 2)
                    {
                        const uint32_t val = 0xffff;
                        cpu_physical_memory_write(real_address + pos, &val, 4);
                    }
                    block_erase_counter = 0;
                    state[rom] = FLASH_STATE_BLOCK_ERASE_BUSY;
                }
                else if(value == 0x10)
                {
                    int pos = 0;

                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS CHIP ERASE\r\n", rom, state[rom], pc);
                    for(pos = 0; pos < size; pos += 2)
                    {
                        const uint32_t val = 0xffff;
                        cpu_physical_memory_write(base + pos, &val, 4);
                    }
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_3:
                if(word_offset == 0x555 && value == 0x10)
                {
                    int pos = 0;
                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: CHIP ERASE\r\n", rom, state[rom], pc);
                    for(pos = 0; pos < size; pos += 2)
                    {
                        const uint32_t val = 0xffff;
                        cpu_physical_memory_write(base + pos, &val, 4);
                    }
                    state[rom] = FLASH_STATE_READ;
                }
                else if(value == 0x30)
                {
                    int pos = 0;
                    int block_size = flash_get_blocksize(rom, size, word_offset);

                    fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: BLOCK ERASE [0x%08X]\r\n", rom, state[rom], pc, real_address);
                    for(pos = 0; pos < block_size; pos += 2)
                    {
                        const uint32_t val = 0xffff;
                        cpu_physical_memory_write(real_address + pos, &val, 4);
                    }
                    block_erase_counter = 0;
                    state[rom] = FLASH_STATE_BLOCK_ERASE_BUSY;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_PROGRAM:
                fprintf(stderr, "[ROM%i:%i] at [0x%04X] Command: PROGRAM [0x%04X] -> [0x%08X]\r\n", rom, state[rom], pc, value, real_address);
                cpu_physical_memory_write(real_address, &value, 4);
                state[rom] = FLASH_STATE_READ;
                break;
        }
        if(fail)
        {
            fprintf(stderr, "[ROM%i:%i] at [0x%04X] [0x%08X] -> [0x%08X]\r\n", rom, state[rom], pc, value, word_offset);
        }
    }
    else
    {

        switch(state[rom])
        {
            case FLASH_STATE_READ:
                ret = eos_default_handle(real_address, type, value);
                break;

            case FLASH_STATE_BLOCK_ERASE_BUSY:
                if(block_erase_counter < 0x10)
                {
                    block_erase_counter++;
                    ret = ((block_erase_counter&1)<<6) | ((block_erase_counter&1)<<2);
                }
                else
                {
                    ret = 0x80;
                    state[rom] = FLASH_STATE_READ;
                }
                break;

            default:
                fprintf(stderr, "[ROM%i:%i] at [0x%04X] read in unknown state [0x%08X] <- [0x%08X]\r\n", rom, state[rom], pc, ret, word_offset);
                break;
        }
    }

    return ret;
}


unsigned int eos_handle_flashctrl(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;

    switch(address & 0x1FF)
    {
        case 0x10:
            if(type & MODE_WRITE)
            {
                if(((value | (value >> 16)) & 0xFFFF) == 0xD9C5)
                {
                    msg = "'Write enable' enabled";
                }
                else if(value == 0x0)
                {
                    msg = "'Write enable' disabled";
                }
                else
                {
                    msg = "unknown command";
                }
            }
            else
            {
                ret = 1;
            }
            break;
    }

    io_log("FlashIF", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_eeko_comm(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = "INT%Xh: ???";
    int msg_arg1 = 0;
    unsigned int ret = 0;

    /* these interrupts are in pairs, e.g. 0x101, 0x102, 0x109, 0x10A ... */
    /* even indices / odd interrupts (reg offset 0x00, 0x40 ...) are from eeko to icu */
    /* odd indices / even interrupts (reg offset 0x20, 0x60 ...) are from icu to eeko */
    const int interrupt_map[] = {
        0x101, 0x109, 0x111, 0x119, 0x121, 0x129, 0x131, 0x139,
        0x0FF, 0x107, 0x10F, 0x117, 0x11F, 0x127, 0x12F, 0x137,
        0x123,
    };
    
    int interrupt_index = (address >> 5) & 0x3F;
    assert(interrupt_index/2 < COUNT(interrupt_map));
    int interrupt_id = interrupt_map[interrupt_index/2] + interrupt_index % 2;
    msg_arg1 = interrupt_id;
    
    switch (address & 0x1F)
    {
        case 0x04:
            msg = "INT%Xh: interrupt acknowledged";
            break;
        case 0x08:
            msg = "INT%Xh: setup interrupts? (1)";
            break;
        case 0x10:
            msg = "INT%Xh: trigger interrupt?";
            break;
        case 0x18:
            msg = "INT%Xh: setup interrupts? (B)";
            break;
    }

    io_log("EEKO", address, type, value, ret, msg, msg_arg1, 0);
    return ret;
}

unsigned int eos_handle_memdiv(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;

    switch (address & 0xFFFF)
    {
        case 0x1604:
        {
            msg = "MEMDIV_SETUP";
            ret = 0x5A;
            break;
        }
        default:
        {
            /* 0x1600 ... 0xFFFF */
            /* firmware expects to read back what it has written earlier? */
            static uint32_t shm[0x10000];
            MMIO_VAR(shm[address & (COUNT(shm)-1)]);
        }
    }

    io_log("MEMDIV", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_rom_id(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;

    switch (address)
    {
        case 0xBFE01FD0:
            msg = "SROM ID";
            ret = 0x0020;
            break;

        case 0xBFE01FD2:
            msg = "SROM ID";
            ret = 0x00BB;
            break;

        case 0xBFE01FD4:
            msg = "SROM ID";
            ret = 0x0019;
            break;

        case 0xD5100010:
        {
            msg = "ROM ID";
            const int rom_id[3] = {0x20, 0xBB, 0x18};
            static int i = 0;
            if (type & MODE_WRITE)
            {
                if (value == 0x9F)
                {
                    i = 0;
                }
            }
            else
            {
                ret = rom_id[i % 3];
                i++;
            }
        }
    }

    io_log("ROMID", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_boot_digic8(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;

    static uint32_t boot_addr;
    static uint32_t boot_flags;

    switch (address)
    {
        case 0xBFE01FC4:
            msg = "Flags?";
            MMIO_VAR(boot_flags);
            break;

        case 0xBFE01FC8:
            msg = "Boot address?";
            MMIO_VAR(boot_addr);
            break;
    }

    io_log("BOOT8", address, type, value, ret, msg, 0, 0);
    return ret;
}

unsigned int eos_handle_boot_digicX(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = 0;
    unsigned int ret = 0;
    unsigned int tables[0x4800] = {0};
    unsigned int i = 0;

    switch (address)
    {
        case 0xdffc4fa0: // is it valid only for R6?
            msg = "CPU1 Boot address";
            break;
        case 0xdffc0000 ... 0xdffc48ff: // mmu tables ram copy
            // SJE FIXME this is just a normal ram region.
            // We should probably define it as such, as well as the other
            // 0xdfXX_XXXX TCM code and data regions on D78X
            i = address - 0xdffc0000;
            if(type & MODE_WRITE)
            {
                tables[i] = value;
            }
            else
            {
                ret = tables[i];
            }
            break;
    }

    io_log("BOOTX", address, type, value, ret, msg, 0, 0);
    return ret;
}


unsigned int eos_handle_digic6(unsigned int parm, unsigned int address, unsigned char type, unsigned int value)
{
    const char *msg = NULL;
    unsigned int ret = 0;
    
    static uint32_t palette_addr = 0;
    
    /* 0xD20B0A24/C34/994/224, depending on model */
    if (address == eos_state->model->card_led_address)
    {
        return eos_handle_card_led(parm, address, type, value);
    }

    /* 0xD20B0884/084, 0xD20B02A4/22A4 */
    if (address == eos_state->model->mpu_request_register ||
        address == eos_state->model->mpu_status_register)
    {
        return eos_handle_mpu(parm, address, type, value);
    }

    /* 0xD4013008, 0xD4013048 */
    if (address == eos_state->model->mpu_control_register)
    {
        return eos_handle_mreq(parm, address, type, value);
    }

    /* 0xD20B0D8C, 0xD20B037C */
    if (eos_state->sf && address == eos_state->model->serial_flash_cs_register)
    {
        return eos_handle_serial_flash_cs(parm, address, type, value);
    }

    /* 0xD20B004C, 0xD20B2294, 0xD20B21DC */
    if (address == eos_state->model->imgpowdet_register ||
        address == eos_state->model->imgpowcfg_register)
    {
        return eos_handle_imgpowdet(parm, address, type, value);
    }

    switch (address)
    {
        case 0xD20B071C:
        case 0xD0034068:
        case 0xD0034020:
            msg = "7D2 comm";
            ret = rand();
            break;

        case 0xD203046C:
        case 0xD203086C:
            msg = "7D2 init";
            ret = 1;
            break;

        case 0xD2030000:    /* M3: memif_wait_us */
        case 0xD20F0000:    /* M3: many reads from FC000382, value seems ignored */
            return 0;

        case 0xD0304238:    /* M50 */
            value = (value & 0xFFFF) / 2 | (value & 0xFFFF0000);
            /* fall through */
        case 0xD2013800:    /* D6 */
        case 0xD201381C:    /* D6 */
        case 0xD2018200:    /* 5D4 */
        case 0xD2018230:    /* 5D4 */
        case 0xD20138BC:    /* M3 */
        case 0xD2060044:    /* D7 */
            msg = "Display resolution";
            MMIO_VAR_2x16(eos_state->disp.width, eos_state->disp.height);
            break;
        
        case 0xD2030108:    /* D6 */
        case 0xD2060048:    /* D7 */
            if (strcmp(eos_state->model->name, MODEL_NAME_EOSM3) == 0)
            {
                if ((value != 0x17410) && (value != 0x18010)) eos_state->disp.bmp_vram = value << 8;
                eos_state->disp.bmp_pitch = (eos_state->disp.width + 16) * 2;
                msg = "BMP VRAM EOS M3";
            }
            else
            {
                eos_state->disp.bmp_vram = value << 8;
                eos_state->disp.bmp_pitch = eos_state->disp.width;
                msg = "BMP VRAM";
            }   
            break;

        case 0xD2018228:    /* 5D4 */
        case 0xD0304230:    /* M50 */
            msg = "BMP VRAM";
            MMIO_VAR(eos_state->disp.bmp_vram);
            break;
        
        case 0xD201822C:    /* 5D4 */
        case 0xD0304234:    /* M50 */
            msg = "BMP pitch";
            MMIO_VAR(eos_state->disp.bmp_pitch);
            break;

        case 0xD20139A8:    /* D6 */
        case 0xD2018398:    /* 5D4 */
        {
            msg = "Bootloader palette address";
            palette_addr = value << 4;
            break;
        }
        case 0xD20139A0:
        case 0xD2018390:
        {
            msg = "Bootloader palette confirm";
            for (int i = 0; i < 16; i++)
            {
                uint32_t entry;
                cpu_physical_memory_read(palette_addr + i * 4, &entry, 4);
                /* palette entry is different; adjust it to match DIGIC 4/5 routines */
                uint8_t *ovuy = (uint8_t*)&entry;
                ovuy[1] -= 128; ovuy[2] -= 128;
                entry = (entry >> 8) | 0x3000000;
                const char *msg;
                process_palette_entry(entry, &eos_state->disp.palette_8bit[i], i, &msg);
                fprintf(stderr, "%08X: %s\n", entry, msg);
            }
            break;
        }
        
        case 0xD203040C:
        {
            msg = "MR (RAM manufacturer ID)";
            static int last = 0;
            if(type & MODE_WRITE)
            {
                last = value;
            }
            else
            {
                /* these should match the values saved in ROM at FC080010 */
                uint32_t mr = eos_state->model->ram_manufacturer_id;
                int mr_index = (last >> 8) - 5;
                ret = (mr >> (mr_index * 8)) & 0xFF;
            }
            break;
        }

        case 0xD20822E8:
            msg = "D7 System Adjustment";
            //ret = 0x10000;
            break;

        case 0xD2090008: /* CLOCK_ENABLE */
            msg = "CLOCK_ENABLE";
            MMIO_VAR(eos_state->clock_enable_6);
            break;

        case 0xD20B053C:
            msg = "PhySwBootSD";        /* M3: card write protect switch? */
            ret = 0x10000;
            break;

        case 0xD20BF4A0:
            msg = "PhySwKeyboard 0";    /* M3: keyboard  */
            ret = 0x10077ffb;
            break;
        
        case 0xD20BF4B0:
            msg = "PhySw 1";            /* M3:  */
            ret = 0x00001425;
            break;

        case 0xD20BF4D8:
            msg = "PhySw 2";            /* M3:  */
            ret = 0x20bb4d30;
            break;

        case 0xD20BF4F0:
            msg = "PhySw Internal Flash + ";    /* M3: Flash + */
            ret = 0x00000840;
            break;

        case 0xD20B0400:                /* 80D: 0x10000 = no card present */
        case 0xD20B22A8:                /* 5D4: same */
            msg = "SD detect";
            ret = 0;
            break;

        case 0xD20B210C:
            msg = "CF detect";          /* 5D4: same as above */
            ret = 0x10000;
            break;

        case 0xD6040000:                /* M3: appears to expect 0x3008000 or 0x3108000 */
            ret = 0x3008000;
            break;

        case 0xD5202018:                /* M5: expects 1 at 0xE0009E9C */
        case 0xD5203018:                /* M5: expects 1 at 0xE0009EBA */
            ret = 1;
            break;

        case 0xD6050000:
        {
            static int last = 0;
            if(type & MODE_WRITE)
            {
                last = value;
            }
            else
            {
                msg = "I2C status?";
                if (strcmp(eos_state->model->name, MODEL_NAME_EOSM10) == 0)
                {
                    ret = rand();
                }
                else
                {
                    ret = (last & 0x8000) ? 0x2100100 : 0x20000;
                }
            }
            break;
        }

        case 0xD6060000:
            msg = "E-FUSE";
            break;

        case 0xD9890014:
            msg = "Battery level maybe (ADC?)";     /* M3: called from Battery init  */
            ret = 0x00020310;
            break;

        // 100D AVS
        case 0xd02c3004: // TST 8
        case 0xd02c3024: // TST 1
        case 0xd02c4004: // TST 8
        case 0xd02c4024: // TST 1
            msg = "AVS??";
            ret = 0xff;
            break;

        case 0xC8100154:
            msg = "IPC?";
            ret = 0x10001;              /* M5: expects 0x10001 at 0xE0009E66 */
            break;

        case 0xD2101504:
            msg = "Wake up CPU1?";       /* M5: wake up the second CPU? */
            assert(eos_state->cpu1);
            //CPU(eos_state->cpu1)->halted = 0;
            printf(KLRED"Wake up CPU1"KRESET"\n");
            break;

        case 0xD0110404:
            msg = "Wake up CPU1?";       /* M50: wake up the second CPU? */
            assert(eos_state->cpu1);
            #if 0
            CPU(eos_state->cpu1)->halted = 0;
            printf(KLRED"Wake up CPU1"KRESET"\n");
            #endif
            ret = 1;
            break;

        case 0xD7100014:
        case 0xD7100020:
        case 0xD7100000:
        case 0xD0740010:
        case 0xD98000BC:
        case 0xDE000000:
        case 0xDE000014:
        case 0xDE000020:
        case 0xD7301000:
            msg = "M50 loop";
            ret = rand();
            break;

        case 0xDEF00014:
        case 0xDEF00020:
        case 0xDEF00000:
            msg = "R6 loop";
            ret = rand();
            break;

        case 0xD01302B4:
            msg = "EEP_CS2";
            break;

        case 0xD01322B4:
            msg = "EEP_CS2 ack";
            ret = (rand() & 1) ? 0xD0002 : 0xC0003;
            break;

        case 0xD0213024:
            msg = "SubCPU ack?";
            break;

        case 0xD02100AC:
            msg = "SubCPU wakeup?";
            //~ eos_trigger_int(s, 0x19A, 1000);
            break;

        case 0xD0040000:
            msg = "Busy waiting?";
            if (!qemu_loglevel_mask(EOS_LOG_VERBOSE)) {
                /* quiet */
                return 0;
            }
            break;

        case 0xD0132280:
            msg = "M50 SD detect";
            ret = 0;
            //~ ret = 0x10000;
    }

    if (address >= 0xD0130000 && address <= 0xD0130FFF) {
        msg = "RP GPIO";
        ret = 0;
    }

    io_log("DIGIC6", address, type, value, ret, msg, 0, 0);
    return ret;
}




/** EOS ROM DEVICE **/



/* its not done yet */
#if defined(EOS_ROM_DEVICE_IMPLEMENTED)
ROMState *eos_rom_register(hwaddr base, DeviceState *qdev, const char *name, hwaddr size,
                                BlockDriverState *bs,
                                uint32_t sector_len, int nb_blocs, int width,
                                uint16_t id0, uint16_t id1,
                                uint16_t id2, uint16_t id3, int be)
{
    DeviceState *dev = qdev_create(NULL, "eos.rom");
    SysBusDevice *busdev = SYS_BUS_DEVICE(dev);
    ROMState *pfl = (ROMState *)object_dynamic_cast(OBJECT(dev),
                                                    "cfi.pflash01");

    if (bs && qdev_prop_set_drive(dev, "drive", bs)) {
        abort();
    }
    qdev_prop_set_uint32(dev, "num-blocks", nb_blocs);
    qdev_prop_set_uint64(dev, "sector-length", sector_len);
    qdev_prop_set_uint8(dev, "width", width);
    qdev_prop_set_uint8(dev, "big-endian", !!be);
    qdev_prop_set_uint16(dev, "id0", id0);
    qdev_prop_set_uint16(dev, "id1", id1);
    qdev_prop_set_uint16(dev, "id2", id2);
    qdev_prop_set_uint16(dev, "id3", id3);
    qdev_prop_set_string(dev, "name", name);
    qdev_init_nofail(dev);

    sysbus_mmio_map(busdev, 0, base);
    return pfl;
}

static const MemoryRegionOps eos_rom_ops = {
/*    .old_mmio = {
        .read = { pflash_readb_be, pflash_readw_be, pflash_readl_be, },
        .write = { pflash_writeb_be, pflash_writew_be, pflash_writel_be, },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,*/
};

static int eos_rom_init(SysBusDevice *dev)
{
    ROMState *pfl = FROM_SYSBUS(typeof(*pfl), dev);
    uint64_t total_len = 0x00100000;
    int ret = 0;

    memory_region_init_rom_device(&pfl->mem, &eos_rom_ops, pfl, pfl->name, total_len);
    vmstate_register_ram(&pfl->mem, DEVICE(pfl));
    pfl->storage = memory_region_get_ram_ptr(&pfl->mem);
    sysbus_init_mmio(dev, &pfl->mem);

    if (pfl->bs) {
        /* read the initial flash content */
        ret = bdrv_read(pfl->bs, 0, pfl->storage, total_len >> 9);

        if (ret < 0) {
            vmstate_unregister_ram(&pfl->mem, DEVICE(pfl));
            memory_region_destroy(&pfl->mem);
            return 1;
        }
    }

    return ret;
}

static Property eos_rom_properties[] = {
    DEFINE_PROP_DRIVE("drive", ROMState, bs),
    DEFINE_PROP_UINT32("num-blocks", ROMState, nb_blocs, 0),
    DEFINE_PROP_UINT64("sector-length", ROMState, sector_len, 0),
    DEFINE_PROP_UINT8("width", ROMState, width, 0),
    DEFINE_PROP_UINT8("big-endian", ROMState, be, 0),
    DEFINE_PROP_UINT16("id0", ROMState, ident0, 0),
    DEFINE_PROP_UINT16("id1", ROMState, ident1, 0),
    DEFINE_PROP_UINT16("id2", ROMState, ident2, 0),
    DEFINE_PROP_UINT16("id3", ROMState, ident3, 0),
    DEFINE_PROP_STRING("name", ROMState, name),
    DEFINE_PROP_END_OF_LIST(),
};

static const TypeInfo eos_rom_info = {
    .name           = "eos.rom",
    .parent         = TYPE_SYS_BUS_DEVICE,
    .instance_size  = sizeof(ROMState),
    .class_init     = eos_rom_class_init,
};

static void eos_rom_class_init(ObjectClass *class, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(class);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(class);

    k->init = eos_rom_init;
    dc->props = eos_rom_properties;
}

static void eos_rom_register_types(void)
{
    type_register_static(&eos_rom_info);
}

type_init(eos_rom_register_types)
#endif
