
.extern risc_v_exception_handler
.extern risc_v_interrupt_handler
.extern risc_v_mtimer_interrupt_handler

.section .vectors
.balign 128, 0
.option norvc
.global risc_v_vector_table
risc_v_vector_table:
IRQ_0:
        j risc_v_exception_handler
IRQ_1:
        j risc_v_interrupt_handler
IRQ_2:
        j risc_v_interrupt_handler
IRQ_3:
        j risc_v_interrupt_handler
IRQ_4:
        j risc_v_interrupt_handler
IRQ_5:
        j risc_v_interrupt_handler
IRQ_6:
        j risc_v_interrupt_handler
IRQ_7:
        j risc_v_mtimer_interrupt_handler
IRQ_8:
        j risc_v_interrupt_handler
IRQ_9:
        j risc_v_interrupt_handler
IRQ_10:
        j risc_v_interrupt_handler
IRQ_11:
        j risc_v_interrupt_handler
IRQ_12:
        j risc_v_interrupt_handler
IRQ_13:
        j risc_v_interrupt_handler
IRQ_14:
        j risc_v_interrupt_handler
IRQ_15:
        j risc_v_interrupt_handler
IRQ_LC0:
        j risc_v_interrupt_handler
IRQ_LC1:
        j risc_v_interrupt_handler
IRQ_LC2:
        j risc_v_interrupt_handler
IRQ_LC3:
        j risc_v_interrupt_handler
IRQ_LC4:
        j risc_v_interrupt_handler
IRQ_LC5:
        j risc_v_interrupt_handler
IRQ_LC6:
        j risc_v_interrupt_handler
IRQ_LC7:
        j risc_v_interrupt_handler
IRQ_LC8:
        j risc_v_interrupt_handler
IRQ_LC9:
        j risc_v_interrupt_handler
IRQ_LC10:
        j risc_v_interrupt_handler
IRQ_LC11:
        j risc_v_interrupt_handler
IRQ_LC12:
        j risc_v_interrupt_handler
IRQ_LC13:
        j risc_v_interrupt_handler
IRQ_LC14:
        j risc_v_interrupt_handler
IRQ_LC15:
        j risc_v_interrupt_handler
