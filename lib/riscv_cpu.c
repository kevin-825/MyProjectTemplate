#include <stdio.h>
#include <stdint.h>
#include "encoding.h"
#include "riscv_cpu.h"


uintptr_t riscv_cpu_myhart_id(void) {
    uintptr_t myhart;
    __asm__ volatile("csrr %0, mhartid" : "=r"(myhart));
    return myhart;
}

void riscv_cpu_interrupt_global_enable(void) {
    uintptr_t m;
    __asm__ volatile("csrrs %0, mstatus, %1"
                     : "=r"(m)
                     : "r"(MSTATUS_MIE));
}

void riscv_cpu_interrupt_global_disable(void) {
    uintptr_t m;
    __asm__ volatile("csrrc %0, mstatus, %1"
                     : "=r"(m)
                     : "r"(MSTATUS_MIE));
}


// Function to save and disable global interrupts
uint32_t riscv_cpu_save_and_disable_global_int(void) {
    uint32_t mstatus;
    // Atomically read mstatus and clear the MIE (Machine Interrupt Enable) bit
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r"(mstatus) : "r"(MSTATUS_MIE));
    return mstatus & MSTATUS_MIE; // Return the original mstatus value      
}


void riscv_cpu_restore_global_interrupts(uint32_t saved_mstatus_mie) {
    if (saved_mstatus_mie & MSTATUS_MIE) {
        // Set the MIE bit if it was set in the saved value
        __asm__ volatile("csrs mstatus, %0" : : "r"(MSTATUS_MIE));
    } else {
        // Clear the MIE bit if it was cleared in the saved value
        __asm__ volatile("csrc mstatus, %0" : : "r"(MSTATUS_MIE));
    }
}



void riscv_cpu_interrupt_software_enable(void) {
    uintptr_t m;
    __asm__ volatile("csrrs %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_SW));
}

void riscv_cpu_interrupt_software_disable(void) {
    uintptr_t m;
    __asm__ volatile("csrrc %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_SW));
}

void riscv_cpu_interrupt_mtimer_enable(void) {
    uintptr_t m;
    __asm__ volatile("csrrs %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_TMR));
}

void riscv_cpu_interrupt_mtimer_disable(void) {
    uintptr_t m;
    __asm__ volatile("csrrc %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_TMR));
}

void riscv_cpu_interrupt_external_enable(void) {
    uintptr_t m;
    __asm__ volatile("csrrs %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_EXT));
}

void riscv_cpu_interrupt_external_disable(void) {
    unsigned long m;
    __asm__ volatile("csrrc %0, mie, %1"
                     : "=r"(m)
                     : "r"(RISCV_CPU_LOCAL_INTERRUPT_EXT));
}

void riscv_cpu_interrupt_local_enable(riscv_cpu_interrupt_id_e id) {
    uintptr_t b = 1 << id;
    uintptr_t m;
    __asm__ volatile("csrrs %0, mie, %1" : "=r"(m) : "r"(b));
}

void riscv_cpu_interrupt_local_disable(riscv_cpu_interrupt_id_e id) {
    uintptr_t b = 1 << id;
    uintptr_t m;
    __asm__ volatile("csrrc %0, mie, %1" : "=r"(m) : "r"(b));
}



unsigned long long riscv_cpu_mcycle_get(uint32_t cpu) {
    unsigned long long val = 0;

#if __riscv_xlen == 32
    unsigned long hi, hi1, lo;

    do {
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi));
        __asm__ volatile("csrr %0, mcycle" : "=r"(lo));
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi1));
        /* hi != hi1 means mcycle overflow during we get value,
         * so we must retry. */
    } while (hi != hi1);

    val = ((unsigned long long)hi << 32) | lo;
#else
    __asm__ volatile("csrr %0, mcycle" : "=r"(val));
#endif

    return val;
}






// Print mstatus in the required format
void print_mstatus(void) {
    mstatus_t mstatus;
    mstatus.value = read_csr(mstatus); // Read the mstatus register

    printf("MSTATUS: 0x%08x (uie:%u sie:%u mie:%u mpie:%u spie:%u spp:%u mpp:%u fs:%u xs:%u mprv:%u sum:%u mxr:%u tvm:%u tw:%u tsr:%u sd:%u)\n",
           (unsigned int)mstatus.value,
           mstatus.bits.uie,    // User Interrupt Enable
           mstatus.bits.sie,    // Supervisor Interrupt Enable
           mstatus.bits.mie,    // Machine Interrupt Enable
           mstatus.bits.mpie,   // Machine Previous Interrupt Enable
           mstatus.bits.spie,   // Supervisor Previous Interrupt Enable
           mstatus.bits.spp,    // Supervisor Previous Privilege Mode
           mstatus.bits.mpp,    // Machine Previous Privilege Mode
           mstatus.bits.fs,     // Floating-Point Status
           mstatus.bits.xs,     // Extension Status
           mstatus.bits.mprv,   // Modify Privilege
           mstatus.bits.sum,    // Supervisor User Memory Access
           mstatus.bits.mxr,    // Make Executable Readable
           mstatus.bits.tvm,    // Trap Virtual Memory
           mstatus.bits.tw,     // Timeout Wait
           mstatus.bits.tsr,    // Trap SRET
           mstatus.bits.sd      // State Dirty
    );
}


void riscv_cpu_interrupt_regs_info(void) {
    unsigned long mie_val = read_csr(mie);
    unsigned long mip_val = read_csr(mip);

    // Print MIE register and its critical fields
    printf("MIE :0x%016lx  (msie:%lu  mtie:%lu  meie:%lu)\n",
           mie_val,
           (mie_val >> 3) & 1,  // MSIE (bit 3)
           (mie_val >> 7) & 1,  // MTIE (bit 7)
           (mie_val >> 11) & 1  // MEIE (bit 11)
    );

    // Print MIP register and its critical fields
    printf("MIP :0x%016lx  (msip:%lu  mtip:%lu  meip:%lu)\n",
           mip_val,
           (mip_val >> 3) & 1,  // MSIP (bit 3)
           (mip_val >> 7) & 1,  // MTIP (bit 7)
           (mip_val >> 11) & 1  // MEIP (bit 11)
    );
    print_mstatus();
}

