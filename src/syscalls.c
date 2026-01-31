// lib/syscalls.c
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include "uart.h"
// Adjust these as needed
extern char _end;          // from linker script: end of .bss
static char *heap_end = 0;

void *_sbrk(ptrdiff_t incr)
{
    if (heap_end == 0) {
        heap_end = &_end;
    }

    char *prev = heap_end;
    heap_end += incr;

    // TODO: optionally check against _eheap from linker script

    return (void *)prev;
}

int _close(int fd)
{
    (void)fd;
    errno = ENOSYS;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    if (st) {
        st->st_mode = S_IFCHR;  // pretend it's a char device
    }
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;   // say "yes" so stdio works
}

off_t _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = ENOSYS;
    return (off_t)-1;
}

// TODO: wire these to your UART/console later
int _read(int fd, void *buf, size_t len)
{
    (void)fd;
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}

int _write(int fd, const void *buf, size_t len)
{
    (void)fd;
    (void)buf;
    if (fd != 1 && fd != 2) { 
        return -1; 
    } 
    uart_write_len((const char *)buf, len);
    return (int)len;
}




void _exit(int status) { while (1); }
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }

