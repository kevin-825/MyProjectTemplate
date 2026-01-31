
# Source the virtual environment
#python import sys
#python sys.path.append('/opt/venv3-10/lib/python3.10/site-packages')
#python sys.path.append('/opt/riscv-gnu-toolchain-u22/share/gdb/python/gdb/command')
#python import freertos




#file $file1
#set arch riscv:rv32
set remotetimeout 250
set output-radix 16
set history filename ./.gdb_history
set history save on
set history size unlimited
#set disassembly-flavor intel


# Enable pretty-printing
set print pretty on

# Enable logging
set logging on
set logging file gdb.log
set logging overwrite on

define schl_on
set scheduler-locking on
end
define schl_off
set scheduler-locking off
end

set $elfpath_0="/mnt/wsl/disk2/MyProjectTemplate/build/riscv32/"

define prog0
eval "file  %s/program_riscv32_debug_v1.0.0",$elfpath_0
end


prog0
target remote localhost:1234

flushregs
#maintenance flush register-cache



# Show thread info
info threads

# Set breakpoint by hardware thread num and func
define bptf
  set $thread_num = $arg0
  set $func_name = $arg1
  echo arg0:$arg0
  echo arg1:$arg1
  echo arg2:$arg2
  thread apply $thread_num break $func_name
end


# Set breakpoint

define bps
    thread apply 1.1 break printf
    thread apply 1.1 break spin_lock
    thread apply 1.1 break spin_unlock
end

#set break point at exception and int
#b freertos_risc_v_application_interrupt_handler
b main
#break _sbrk
#break _close
#break _fstat
#break _fstat_r
#break _isatty
#break _lseek
#break _read
#break _write




# Automatically display registers
define set_disp
  display /r $ra
  display /r $a0
  display /r $a1
  display /r $a2
  display /r $a3
  display /r $a4
  display /r $a5
  display /r $a6
  display /r $a7
  display /r $mstatus
  display /r $mie
  display /r $t0
  display /r $t1
  display /r $t2
  display /r $t3
  display /r $t5
  display /r $t6
#  display &uart_lock.arch_lock.lock
#  display &uart_lock.arch_lock.h.serving_now
#  display &uart_lock.arch_lock.h.ticket
end
# Show current instruction
#display /i $pc
# Automatically display local variables

define infr
    info registers $ra
    info registers $a0
    info registers $a1
    info registers $a2
    info registers $a3
    info registers $a4
    info registers $a5
    info registers $a6
    info registers $a7
    info registers $t0
    info registers $t1
    info registers $t2
    info registers $t3
    info registers $t5
    info registers $t6
end

define info_machine_reg
    #info thread
    info registers mcause
    info registers mepc
    info registers mtval
    info registers mtvec
    info registers mstatus
    info registers mie

end

define hook-stop
  info_machine_reg
  info registers sp 
  info registers gp
  info registers tp
  #info locals
end

define dprio
  x/4x 0x0C000000UL+$arg0*4
end

define hoop-stop1
  x/33x g_exception_stack_fram
end

# multicore debuging case



info break

# define sr
# target remote localhost:1234
# display /r info locals
# b main
# b arch_spin_trylock
# end

define setreg
set $a0 = 0xa0
set $a1 = 0xa1
set $a2 = 0xa2
set $a3 = 0xa3
set $a4 = 0xa4
set $a5 = 0xa5
set $a6 = 0xa6
set $a7 = 0xa7
set $t0 = 0xb0
set $t1 = 0xb1
set $t2 = 0xb2
set $t3 = 0xb3
set $t4 = 0xb4
set $t5 = 0xb5
set $t6 = 0xb6
set $s0 = 0xc0
set $s1 = 0xc1
set $s2 = 0xc2
set $s3 = 0xc3
set $s4 = 0xc4
set $s5 = 0xc5
set $s6 = 0xc6
set $s7 = 0xc7
set $s8 = 0xc8
set $s9 = 0xc9
set $s10 = 0xca
set $s11 = 0xcb
end

