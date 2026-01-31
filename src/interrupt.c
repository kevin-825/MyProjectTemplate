/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "exception.h"
#include "riscv_cpu.h"
#include "encoding.h"

#define METAL_MCAUSE_INTR 0x80000000UL
#define METAL_MCAUSE_CAUSE 0x000003FFUL
#define METAL_LOCAL_INTR(X) (16 + X)
#define METAL_MCAUSE_EVAL(cause) (cause & METAL_MCAUSE_INTR)
#define METAL_INTERRUPT(cause) (METAL_MCAUSE_EVAL(cause) ? 1 : 0)
#define METAL_EXCEPTION(cause) (METAL_MCAUSE_EVAL(cause) ? 0 : 1)
#define METAL_SW_INTR_EXCEPTION (METAL_MCAUSE_INTR + 3)
#define METAL_TMR_INTR_EXCEPTION (METAL_MCAUSE_INTR + 7)
#define METAL_EXT_INTR_EXCEPTION (METAL_MCAUSE_INTR + 11)



#define USER_SOFTWARE_INTERRUP        (0 | 1 << 31)     
#define SUPERVISOR_SOFTWARE_INTERRUP  (1 | 1 << 31)
#define MACHINE_SOFTWARE_INTERRUP     (3 | 1 << 31)
#define USER_TIMER_INTERRUP           (4 | 1 << 31)
#define SUPERVISOR_TIMER_INTERRUP     (5 | 1 << 31)
#define MACHINE_TIMER_INTERRUP        (7 | 1 << 31)
#define USER_EXTERNAL_INTERRUP        (8 | 1 << 31)
#define SUPERVISOR_EXTERNAL_INTERRUP  (9 | 1 << 31)
#define MACHINE_EXTARNAL_INTERRUP     (11 | 1 << 31)
#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT       1
#define ILLEGAL_INSTRUCTION            2
#define BREAKPOINT                     3  
#define LOAD_ADDRSS_MISALIGNED         4
#define LOAD_ACCESS_FAULT              5
#define STORE_OR_AMO_ADDRESS_MISALIGNED 6
#define STORE_OR_AMO_ACCESS_FAULT      7
#define ENVIRONMENT_CALL_FROM_U_MODE   8
#define ENVIRONMENT_CALL_FROM_S_MODE   9
#define ENVIRONMENT_CALL_FROM_M_MODE   11
#define INSTRUCTION_PAGE_FAULT         12
#define LOAD_PAGE_FAULT                13
#define STORE_OR_AMO_PAGE_FAULT        15

__attribute__ ((weak)) void exception_hook_pre (void){
}
__attribute__ ((weak)) void exception_hook_post (void){
}
__attribute__ ((weak)) void user_software_interrupt (void){
}
__attribute__ ((weak)) void supervisor_software_interrupt (void){
}
__attribute__ ((weak)) void machine_software_interrupt (void){
	uint32_t HART_ID=riscv_cpu_myhart_id();
	printf( "\r\n\r\n[Core%lu ] machine_software_interrupt \r\n" , HART_ID);
	// test_delay_handle_irq(MACHINE_SOFTWARE_INTERRUP,0);
	// if(HART_ID != 0) {
	// 	trigger_software_interrupt(0);
	// }
	// clear_software_interrupt(HART_ID);
}
__attribute__ ((weak)) void user_timer_interrupt (void){
}
__attribute__ ((weak)) void supervisor_timer_interrupt (void){
}
__attribute__ ((weak)) void machine_timer_interrupt (void){
}
__attribute__ ((weak)) void user_external_interrupt (void){
}
__attribute__ ((weak)) void supervisor_external_interrupt (void){
}
__attribute__ ((weak)) void machine_external_interrupt_handler (void){
}
__attribute__ ((weak)) void instruction_address_misaligned (void){
}
__attribute__ ((weak)) void instruction_access_fault (void){
}
__attribute__ ((weak)) void illegal_instruction (void){
}
__attribute__ ((weak)) void breakpoint (void){
}
__attribute__ ((weak)) void load_address_misaligned (void){
}
__attribute__ ((weak)) void load_access_fault (void){
}
__attribute__ ((weak)) void store_or_amo_address_misaligned (void){
}
__attribute__ ((weak)) void store_or_amo_access_fault (void){
}
__attribute__ ((weak)) void environment_call_from_u_mode (void){
}
__attribute__ ((weak)) void environment_call_from_s_mode (void){
}
__attribute__ ((weak)) void environment_call_from_m_mode (void){
}
__attribute__ ((weak)) void instruction_page_fault (void){
}
__attribute__ ((weak)) void load_page_fault (void){
}
__attribute__ ((weak)) void store_or_amo_page_fault (void){
}





int g_intCnt=0;
void risc_v_application_interrupt_handler( uint32_t ulMcause )
{
    //char pcCause[ 20 ];

    /* Not implemented yet! */
    printf("ulMcause:0x%x intCnt:%d g_exception_nest_cnt:%lu \n", (unsigned int)ulMcause, g_intCnt, g_exception_nest_cnt);
    exception_hook_pre();

    switch(ulMcause) {
		case (USER_SOFTWARE_INTERRUP):         user_software_interrupt(); break;
		case (SUPERVISOR_SOFTWARE_INTERRUP):   supervisor_software_interrupt(); break;
		case (MACHINE_SOFTWARE_INTERRUP):      machine_software_interrupt(); break;
		case (USER_TIMER_INTERRUP):            user_timer_interrupt(); break;
		case (SUPERVISOR_TIMER_INTERRUP):      supervisor_timer_interrupt(); break;
		case (MACHINE_TIMER_INTERRUP):         machine_timer_interrupt(); break;
		case (USER_EXTERNAL_INTERRUP):         user_external_interrupt(); break;
		case (SUPERVISOR_EXTERNAL_INTERRUP):   supervisor_external_interrupt(); break;
		case (MACHINE_EXTARNAL_INTERRUP):      machine_external_interrupt_handler(); break;
		case (INSTRUCTION_ADDRESS_MISALIGNED): instruction_address_misaligned(); break;
		case (INSTRUCTION_ACCESS_FAULT):       instruction_access_fault(); break;
		case (ILLEGAL_INSTRUCTION):            illegal_instruction(); break;
		case (BREAKPOINT):                     breakpoint(); break;
		case (LOAD_ADDRSS_MISALIGNED):         load_address_misaligned(); break;
		case (LOAD_ACCESS_FAULT):              load_access_fault(); break;
		case (STORE_OR_AMO_ADDRESS_MISALIGNED):store_or_amo_address_misaligned(); break;
		case (STORE_OR_AMO_ACCESS_FAULT):      store_or_amo_access_fault(); break;
		case (ENVIRONMENT_CALL_FROM_U_MODE):   environment_call_from_u_mode(); break;
		case (ENVIRONMENT_CALL_FROM_S_MODE):   environment_call_from_s_mode(); break;
		case (ENVIRONMENT_CALL_FROM_M_MODE):   environment_call_from_m_mode(); break;
		case (INSTRUCTION_PAGE_FAULT):         instruction_page_fault(); break;
		case (LOAD_PAGE_FAULT):                load_page_fault(); break;
		case (STORE_OR_AMO_PAGE_FAULT):        store_or_amo_page_fault(); break;
		default: break;
    }
    exception_hook_post();
}