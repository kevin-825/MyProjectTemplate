#include "uart.h"

#define UART0_BASE      0x10000000UL

/* 16550 register offsets (byte offsets) */
#define UART_RBR        0x00    /* Receive Buffer Register (read) */
#define UART_THR        0x00    /* Transmit Holding Register (write) */
#define UART_DLL        0x00    /* Divisor Latch Low (when DLAB=1) */
#define UART_IER        0x01    /* Interrupt Enable Register */
#define UART_DLM        0x01    /* Divisor Latch High (when DLAB=1) */
#define UART_IIR        0x02    /* Interrupt Identification Register (read) */
#define UART_FCR        0x02    /* FIFO Control Register (write) */
#define UART_LCR        0x03    /* Line Control Register */
#define UART_MCR        0x04    /* Modem Control Register */
#define UART_LSR        0x05    /* Line Status Register */
#define UART_MSR        0x06    /* Modem Status Register */
#define UART_SCR        0x07    /* Scratch Register */

/* LCR bits */
#define LCR_DLAB        (1u << 7)   /* Divisor Latch Access Bit */
#define LCR_8N1         0x03        /* 8 bits, no parity, 1 stop bit */

/* LSR bits */
#define LSR_DR          (1u << 0)   /* Data Ready */
#define LSR_THRE        (1u << 5)   /* Transmitter Holding Register Empty */

/* MCR bits */
#define MCR_DTR         (1u << 0)
#define MCR_RTS         (1u << 1)
#define MCR_OUT2        (1u << 3)   /* Often needed to enable interrupts */

/* Convenience macros for MMIO access */
static inline void uart_write_reg(uint32_t offset, uint8_t value)
{
    volatile uint8_t *addr = (uint8_t *)(UART0_BASE + offset);
    *addr = value;
}

static inline uint8_t uart_read_reg(uint32_t offset)
{
    volatile uint8_t *addr = (uint8_t *)(UART0_BASE + offset);
    return *addr;
}

/*
 * QEMU virt UART clock is typically 3.6864 MHz or similar.
 * For simplicity, we can leave the divisor at its reset value and
 * accept the default baud rate, since QEMU doesn’t care.
 *
 * If you want to be explicit, you can set:
 *   baud = clock / (16 * divisor)
 * but for QEMU console, it’s not critical.
 */
static void uart_set_baud_divisor(uint16_t divisor)
{
    uint8_t lcr = uart_read_reg(UART_LCR);

    /* Enable access to DLL/DLM */
    uart_write_reg(UART_LCR, lcr | LCR_DLAB);
    uart_write_reg(UART_DLL, (uint8_t)(divisor & 0xFF));
    uart_write_reg(UART_DLM, (uint8_t)((divisor >> 8) & 0xFF));

    /* Restore LCR (clear DLAB) */
    uart_write_reg(UART_LCR, lcr & ~LCR_DLAB);
}

void uart_init(void)
{
    /* 1. Disable interrupts */
    uart_write_reg(UART_IER, 0x00);

    /* 2. Set baud rate divisor if you care; otherwise leave default.
     * Example: assume 3.6864 MHz clock, 115200 baud:
     *   divisor = 3686400 / (16 * 115200) = 2
     *
     * For QEMU, you can skip this or set divisor=2 explicitly.
     */
    uart_set_baud_divisor(2);

    /* 3. 8 data bits, no parity, 1 stop bit */
    uart_write_reg(UART_LCR, LCR_8N1);

    /* 4. Enable FIFO, clear RX/TX queues, set FIFO trigger level */
    uart_write_reg(UART_FCR, 0x07); /* Enable FIFO, clear RX/TX */

    /* 5. Modem control: set DTR, RTS, OUT2 (OUT2 often enables interrupts) */
    uart_write_reg(UART_MCR, MCR_DTR | MCR_RTS | MCR_OUT2);

    /* 6. Optionally enable interrupts in IER if you plan to use them.
     * For polling-only, leave IER = 0.
     */
}

/* Blocking transmit of a single character */
void uart_putc(char c)
{
    /* Wait until THR is empty */
    while ((uart_read_reg(UART_LSR) & LSR_THRE) == 0)
        ;

    uart_write_reg(UART_THR, (uint8_t)c);
}

/* Blocking receive of a single character */
char uart_getc_blocking(void)
{
    /* Wait until data is ready */
    while ((uart_read_reg(UART_LSR) & LSR_DR) == 0)
        ;

    return (char)uart_read_reg(UART_RBR);
}

/*
 * Non-blocking receive:
 *   returns 1 if a character was read and stored in *out
 *   returns 0 if no data available
 */
int uart_getc_nonblocking(char *out)
{
    if ((uart_read_reg(UART_LSR) & LSR_DR) == 0) {
        return 0;
    }

    if (out) {
        *out = (char)uart_read_reg(UART_RBR);
    } else {
        (void)uart_read_reg(UART_RBR); /* drain anyway */
    }

    return 1;
}

/* Write a null-terminated string */
void uart_write(const char *s)
{
    if (!s) return;

    while (*s) {
        if (*s == '\n') {
            uart_putc('\r'); /* optional CRLF */
        }
        uart_putc(*s++);
    }
}

/* Write a buffer of known length */
void uart_write_len(const char *s, size_t len)
{
    if (!s) return;

    for (size_t i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\n') {
            uart_putc('\r');
        }
        uart_putc(c);
    }
}
