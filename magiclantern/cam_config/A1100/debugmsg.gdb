# ./run_canon_fw.sh A1100 -s -S & arm-none-eabi-gdb -x A1100/debugmsg.gdb

source -v debug-logging.gdb

macro define CURRENT_TASK 0x195C
macro define CURRENT_ISR  (MEM(0x670) ? MEM(0x674) >> 2 : 0)

# CHDK stubs levent_table
macro define LEVENT_TABLE 0xffe93cdc

# global error flag set by panic, assert, HardwareDefect, silences subsequent errors
macro define ERROR_FLAG MEM(0x2954)

# table structure is
# char *name // may point to empty string
# int event_id
# int param
# terminated by null (not empty) name, event id
define ps_print_event_name
  if LEVENT_TABLE != -1
    set $p_ev_name = LEVENT_TABLE
	while *(char **)($p_ev_name) != 0
	  set $ev_name = *(char **)($p_ev_name)
	  if *(int *)($p_ev_name + 4) == $arg0
		if ((char *)$ev_name)[0] != 0
		  printf "%s",$ev_name
		end
		loop_break
	  end
	  set $p_ev_name = $p_ev_name + 12
	end
  end
end
document ps_print_event_name
Print name of numbered event if found in LEVENT_TABLE
end

define ps_event_dispatch_log
  commands
    silent
    print_current_location
    printf "Event(0x%X, 0x%X, 0x%X) ", $r0, $r1, $r2
	ps_print_event_name $r0
	printf "\n"
    c
  end
end
document ps_event_dispatch_log
Log powershot controller events
end

define ps_event_post_log
  commands
    silent
    print_current_location
    printf "PostEvent(0x%X, 0x%X) ", $r0, $r1
	ps_print_event_name $r0
	printf "\n"
    c
  end
end
document ps_event_post_log
Log powershot controller events posted from PostLogicalEventToUI and friends
end

define ps_cameracon_set_state_log
  commands
    silent
    print_current_location
    printf "cameracon_set_state(0x%X)\n", $r0
    c
  end
end
document ps_cameracon_set_state_log
Log powershot cameracon state changes
end

define exception_logX
  commands
    silent
    print_current_location
    KRED
    printf "exception(0x%X, 0x%X ,0x%X)\n", $r0, $r1, $r2
    KRESET
    c
  end
end
document exception_logX
Log exception function called early in exception vector code
end

define exception_log
  commands
    silent
    print_current_location
    KRED
    printf "exception(0x%X) errflag=%d\n", $r0, ERROR_FLAG
    KRESET
    c
  end
end
document exception_log
Log exception function for DryOS handler registered by task_Startup:ffc15f3c
end

define hwdefect_log
  commands
    silent
    print_current_location
    KRED
    printf "HardwareDefect(0x%X) errflag=%d\n", $r0, ERROR_FLAG
    KRESET
    c
  end
end
document hwdefect_log
Log HardwareDefect
end

define errflag_log
  commands
    silent
    print_current_location
    KRED
    printf "SetErrFlag\n"
    KRESET
    c
  end
end
document errflag_log
Log function that sets global error flag
end

define asserthandler_log
  commands
    silent
    print_current_location
    KRED
    printf "Assert handler(0x%X,%d) errflag=%d\n", $r0, $r1, ERROR_FLAG
    KRESET
    c
  end
end
document asserthandler_log
Log Dryos assert handler registered in task_Startup:ffc15f3c
end

define panic_log
  commands
    silent
    print_current_location
    KRED
    printf "Panic(0x%X,0x%X) errflag=%d\n", $r0, $r1, ERROR_FLAG
    KRESET
    c
  end
end
document panic_log
Log Dryos panic
end


define printf_log0
  commands
    silent
    print_current_location
    print_formatted_string $r0 $r1 $r2 $r3 MEM($sp) MEM($sp+4) MEM($sp+8) MEM($sp+12) MEM($sp+16) MEM($sp+20)
    c
  end
end
document printf_log0
Log calls to plain printf with format in r0.
end

define func_log
  print_current_location
  set $i = 0
  while $i < $argc
    eval "echo $arg%d", $i
	printf " "
	set $i = $i + 1
  end
  printf "P=0x%08x S=0x%08x I=%d M=0x%02x\n", $pc, $sp, ($cpsr & 0x80) >> 8, $cpsr&0x1f
  printf "R0=0x%08x R1=0x%08x R2=0x%08x R3=0x%08x R4=0x%08x R5=0x%08x R6=0x%08x\n", $r0, $r1, $r2, $r3, $r4, $r5, $r6
  x/12wx $sp
end
document func_log
log optional message, summary of regs, stack
end

define func_logb
  print_current_location
  set $i = 0
  while $i < $argc
    eval "echo $arg%d", $i
	printf " "
	set $i = $i + 1
  end
  printf "P=0x%08x S=0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", $pc, $sp, $r0, $r1, $r2, $r3
end
document func_log
log optional message, brief summary of r0-r4
end

define bootparam_set_log
  commands
    silent
    print_current_location
    KGRN
    printf "bootparam_set(0x%X, 0x%X)\n", $r0, $r1
    KRESET
    c
  end
end
document bootparam_set_log
Log calls bootparam set function
end

define ps_controller_log
  commands
    silent
    print_current_location
    KGRN
    printf "controller(0x%X, 0x%X ,0x%X, 0x%X) @0x%x\n", $r0, $r1, $r2, $r4, $pc
    KRESET
    c
  end
end
document ps_controller_log
log controller calls
end

define dump_sio_com_struct
 set $i=0
 while $i < 8
  set $ptr=(0x80000580 + 48*$i)
  if $arg0 == *(int *)$ptr
   printf "found SIO %d index %d\n", $arg0, $i
   x/12wx $ptr
   set $i=9
  else
   set $i=$i+1
  end
 end
 if $i == 8
  printf "sio %d not found\n", $arg0
 end
end
document dump_sio_com_struct
debug print tcm structure used for SIO com by 0x120 and friends
end

define dump_all_sio_com_struct
 set $i=0
 while $i < 8
  set $ptr=(0x80000580 + 48*$i)
  printf "index %d SIO %d\n", $i, *($ptr)
  x/12wx $ptr
  set $i=$i+1
 end
end
document dump_all_sio_com_struct
debug print tcm structures used for SIO com by 0x120 and friends
end

macro define CURRENT_TASK 0x195C
macro define CURRENT_ISR  (MEM(0x670) ? MEM(0x674) >> 2 : 0)

# LogCameraEvent
b *0xffc56598
DebugMsg1_log

b *0xFFC0B284
assert0_log

b *0xFFC0AFAC
task_create_log

b *0xFFC0AF44
register_interrupt_log

b *0xFFC55494
register_func_log

b *0xffc5bf38
ps_event_dispatch_log

# PostLogicalEventToUi
b *0xffc5c35c
ps_event_post_log

# PostLogicalEventForNotPowerType
b *0xffc5c310
ps_event_post_log

# PostEventShootSeqToUI
b *0xffc5c3d4
ps_event_post_log

# Unknown event post
b *0xffd187a0
ps_event_post_log

b *0xffc5e1a8
ps_cameracon_set_state_log

# used by exception, dryos init errors
b *0xffc03530
printf_log0

# on exception in handler called from exception vector
#b *0xffc00d3c
#exception_logX

# on exception in handler registered at dryos startup
b *0xffc4f9cc
exception_log

b *0xffc5cf84
hwdefect_log

b *0xffc4f5bc
errflag_log

b *0xffc4f620
asserthandler_log

b *0xffc4fd2c
panic_log

# set value in bootParam.c
b *0xffc5d864
bootparam_set_log

# controller that deals with many startup events
#b *0xffc19014
#ps_controller_log

# WriteToRom_FW
b *0xffcff10c
commands
 silent
 func_logb WriteToRom
 c
end

# fix playback startup flag in CHDK
# this is a workaround for a likely (but not yet confirmed on real hardware) bug
# in the CHDK A1100 port, which causes CHDK to boot in shooting mode when
# the key used to start the camera is unknown
# In emulation, the firmware errors in shooting mode because lens hardware
# is not emulated.
b *0x34e5bc
commands
 set MEM(0x2234) = 0x200000
 c
end

cont
