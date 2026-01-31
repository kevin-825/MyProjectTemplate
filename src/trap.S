// #include "Context.h"
#include "portContext.h"
.global risc_v_trap_handler
.global risc_v_exception_handler
.global risc_v_interrupt_handler
.global risc_v_mtimer_interrupt_handler

.extern vTaskSwitchContext
.extern risc_v_ecall_handler
.weak risc_v_application_exception_handler
.weak risc_v_application_interrupt_handler
.weak risc_v_application_mtimer_interrupt_handler


/*-----------------------------------------------------------*/

risc_v_application_exception_handler:
    csrr t0, mcause     /* For viewing in the debugger only. */
    csrr t1, mepc       /* For viewing in the debugger only */
    csrr t2, mstatus    /* For viewing in the debugger only */
    j .
/*-----------------------------------------------------------*/

risc_v_application_interrupt_handler:
    csrr t0, mcause     /* For viewing in the debugger only. */
    csrr t1, mepc       /* For viewing in the debugger only */
    csrr t2, mstatus    /* For viewing in the debugger only */
    j .

risc_v_application_mtimer_interrupt_handler:
    csrr t0, mcause     /* For viewing in the debugger only. */
    csrr t1, mepc       /* For viewing in the debugger only */
    csrr t2, mstatus    /* For viewing in the debugger only */
    j .
/*-----------------------------------------------------------*/

.section .text.risc_v_exception_handler
risc_v_exception_handler:
    portcontextSAVE_EXCEPTION_CONTEXT
    /* a0 now contains mcause. */
    li t0, 11                           /* 11 == environment call. */
    bne a0, t0, other_exception         /* Not an M environment call, so some other exception. */
    /*call vTaskSwitchContext*/
    call risc_v_ecall_handler
    portcontextRESTORE_CONTEXT

other_exception:
    call risc_v_application_exception_handler
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/

.section .text.risc_v_interrupt_handler
risc_v_interrupt_handler:
    portcontextSAVE_INTERRUPT_CONTEXT
    call risc_v_application_interrupt_handler
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/

.section .text.risc_v_mtimer_interrupt_handler
risc_v_mtimer_interrupt_handler:
    portcontextSAVE_INTERRUPT_CONTEXT
    // portUPDATE_MTIMER_COMPARE_REGISTER
    // call xTaskIncrementTick
    // beqz a0, exit_without_context_switch    /* Don't switch context if incrementing tick didn't unblock a task. */
    // call vTaskSwitchContext
    call risc_v_application_mtimer_interrupt_handler
exit_without_context_switch:
    portcontextRESTORE_CONTEXT