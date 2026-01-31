
#ifndef EXC_H_
#define EXC_H_

// Define the CSR registers for RISC-V
// Define the CSR registers for RISC-V
#define CSR_MCAUSE 0x342
#define CSR_MEPC 0x341
#define CSR_MTVAL 0x343
#define CSR_MSTATUS 0x300

// Define the indices for the stack frame with ABI names
// Define the indices for the stack frame with ABI names
#define IDX_MEPC_ADDED_4       0
#define IDX_MSTATUS            1

#define IDX_X1_RA              2
#define IDX_X5_T0              3
#define IDX_X6_T1              4
#define IDX_X7_T2              5
#define IDX_X8_S0_FP           6
#define IDX_X9_S1              7
#define IDX_X10_A0             8
#define IDX_X11_A1             9
#define IDX_X12_A2             10
#define IDX_X13_A3             11
#define IDX_X14_A4             12
#define IDX_X15_A5             13

#ifndef __riscv_32e
    #define IDX_X16_A6         14
    #define IDX_X17_A7         15
    #define IDX_X18_S2         16
    #define IDX_X19_S3         17
    #define IDX_X20_S4         18
    #define IDX_X21_S5         19
    #define IDX_X22_S6         20
    #define IDX_X23_S7         21
    #define IDX_X24_S8         22
    #define IDX_X25_S9         23
    #define IDX_X26_S10        24
    #define IDX_X27_S11        25
    #define IDX_X28_T3         26
    #define IDX_X29_T4         27
    #define IDX_X30_T5         28
    #define IDX_X31_T6         29
    #define IDX_XCRITICALNESTING   30
#else
    #define IDX_XCRITICALNESTING   14
#endif


/* IDX_MSTATUS already defined as 1 */


extern uint32_t g_exception_nest_cnt;
extern uint32_t g_current_stack_pointer;
extern uint32_t g_critical_nesting_cnt;
extern uint32_t *p_critical_nesting_cnt;

void risc_v_ecall_handler( uint32_t ulMcause );
#endif
