#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>

void uart_init(void);
void uart_putc(char c);
char uart_getc_blocking(void);
int  uart_getc_nonblocking(char *out);
void uart_write(const char *s);
void uart_write_len(const char *s, size_t len);

#endif /* UART_H */
