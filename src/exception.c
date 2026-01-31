
#include <stdio.h>
#include <stdint.h>
#include "exception.h"
#include "riscv_cpu.h"
#include "encoding.h"

//#include "coredump.h"

uint32_t g_exception_nest_cnt=0;
uint32_t g_current_stack_pointer;
uint32_t g_critical_nesting_cnt;
uint32_t *p_critical_nesting_cnt = &g_critical_nesting_cnt;

uint32_t xISRStackTop[1024] __attribute__((aligned(16))); // Define an ISR stack

const char *mcause_messages[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Environment call from VS-mode",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Store/AMO page fault",
    "Unknown cause"
};


void print_mcause(uint32_t mcause) {
    if (mcause >= 0 && mcause < 15) {
        printf("%s\n", mcause_messages[mcause]);
    } else {
        printf("%s\n", mcause_messages[15]); // Unknown cause
    }
}

void exception_show_exc_stack_frame_registers(uint32_t* stack_frame) 
{
    uint32_t mcause = read_csr(mcause);
    uint32_t mepc = read_csr(mepc);
    uint32_t mtval = read_csr(mtval);
    uint32_t mstatus = read_csr(mstatus);
    uint32_t mtvec = read_csr(mtvec);
    uint32_t mip = read_csr(mip);
    uint32_t mie = read_csr(mie);

    // Print the exception-related registers
    printf("Exception occurred! machine register info:\n");
    printf("mcause: 0x%08x   ", (unsigned int)mcause);
    print_mcause(mcause);
    printf("mepc: 0x%08x\n", (unsigned int)mepc);
    printf("mtval: 0x%08x\n", (unsigned int)mtval);
    printf("mstatus: 0x%08x\n", (unsigned int)mstatus);
    printf("mtvec: 0x%08x\n", (unsigned int)mtvec);
    printf("mip: 0x%08x\n", (unsigned int)mip);
    printf("mie: 0x%08x\n\n", (unsigned int)mie);
    //printf("tp: 0x%08x\n", stack_frame[IDX_X4_TP]);
    //printf("gp: 0x%08x\n", stack_frame[IDX_X3_GP]);
    
    // Print the register stack frame
    //printf("pvParameters: 0x%08x\n", stack_frame[IDX_PVPARAMETERS]);
    register void *gp_value __asm__("gp");
    register void *tp_value __asm__("tp");
    printf("Exception StackFrame Info:\n");
    printf("sp: 0x%08x\n", (unsigned int)stack_frame);
    printf("gp: 0x%08x\n", (unsigned int)gp_value);
    printf("tp: 0x%08x\n", (unsigned int)tp_value);
    printf("xCriticalNesting: 0x%08x\n", (unsigned int)stack_frame[IDX_XCRITICALNESTING]);
    printf("mstatus: 0x%08x\n", (unsigned int)stack_frame[IDX_MSTATUS]);
    printf("mepc: 0x%08x\n", (unsigned int)stack_frame[IDX_MEPC_ADDED_4]);
        // Print ra
    printf("ra: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X1_RA]);
    // Print t0-t6
    printf("t0: 0x%08x\n", (unsigned int)stack_frame[IDX_X5_T0]);
    printf("t1: 0x%08x\n", (unsigned int)stack_frame[IDX_X6_T1]);
    printf("t2: 0x%08x\n", (unsigned int)stack_frame[IDX_X7_T2]);
    printf("t3: 0x%08x\n", (unsigned int)stack_frame[IDX_X28_T3]);
    printf("t4: 0x%08x\n", (unsigned int)stack_frame[IDX_X29_T4]);
    printf("t5: 0x%08x\n", (unsigned int)stack_frame[IDX_X30_T5]);
    printf("t6: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X31_T6]);
    
    // Print a0-a7
    printf("a0: 0x%08x\n", (unsigned int)stack_frame[IDX_X10_A0]);
    printf("a1: 0x%08x\n", (unsigned int)stack_frame[IDX_X11_A1]);
    printf("a2: 0x%08x\n", (unsigned int)stack_frame[IDX_X12_A2]);
    printf("a3: 0x%08x\n", (unsigned int)stack_frame[IDX_X13_A3]);
    printf("a4: 0x%08x\n", (unsigned int)stack_frame[IDX_X14_A4]);
    printf("a5: 0x%08x\n", (unsigned int)stack_frame[IDX_X15_A5]);
    #ifndef __riscv_32e
        printf("a6: 0x%08x\n", (unsigned int)stack_frame[IDX_X16_A6]);
        printf("a7: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X17_A7]);
    #endif /* ifndef __riscv_32e */
    
    // Print s0-s11
    printf("s0/fp: 0x%08x\n", (unsigned int)stack_frame[IDX_X8_S0_FP]);
    printf("s1: 0x%08x\n", (unsigned int)stack_frame[IDX_X9_S1]);
    #ifndef __riscv_32e
        printf("s2: 0x%08x\n", (unsigned int)stack_frame[IDX_X18_S2]);
        printf("s3: 0x%08x\n", (unsigned int)stack_frame[IDX_X19_S3]);
        printf("s4: 0x%08x\n", (unsigned int)stack_frame[IDX_X20_S4]);
        printf("s5: 0x%08x\n", (unsigned int)stack_frame[IDX_X21_S5]);
        printf("s6: 0x%08x\n", (unsigned int)stack_frame[IDX_X22_S6]);
        printf("s7: 0x%08x\n", (unsigned int)stack_frame[IDX_X23_S7]);
        printf("s8: 0x%08x\n", (unsigned int)stack_frame[IDX_X24_S8]);
        printf("s9: 0x%08x\n", (unsigned int)stack_frame[IDX_X25_S9]);
        printf("s10: 0x%08x\n", (unsigned int)stack_frame[IDX_X26_S10]);
        printf("s11: 0x%08x\n", (unsigned int)stack_frame[IDX_X27_S11]);
    #endif /* ifndef __riscv_32e */
    
}






void risc_v_application_exception_handler( uint32_t ulMcause )
{
    riscv_cpu_interrupt_global_disable();
    ( void ) ulMcause;

	print_mcause(ulMcause);
    //extern TaskHandle_t pxCurrentTCB;
    uint32_t *pxCurrentTCB=(uint32_t *)0;
    uint32_t *p_exc_stac_frame=(uint32_t *)0;
    if (pxCurrentTCB) {
        p_exc_stac_frame = (uint32_t *)(*(uint32_t *)pxCurrentTCB);
    } else {
        p_exc_stac_frame = (uint32_t *)g_current_stack_pointer;
    }

    exception_show_exc_stack_frame_registers(p_exc_stac_frame);



    //CpuContext context;

    // Capture CPU context (replace with actual retrieval code)
    //memset(&context, 0, sizeof(CpuContext));
    //context.mcause = 0; // Replace with actual cause
    //context.mepc = 0;   // Replace with actual program counter

    //serialize_and_dump(&context);
    while(1){
        
	}
}

void risc_v_ecall_handler( uint32_t ulMcause )
{
    printf("risc_v_ecall_handler ulMcause:0x%x \n", (unsigned int)ulMcause);
}





