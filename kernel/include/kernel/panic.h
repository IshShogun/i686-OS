#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H

__attribute__((noreturn)) void panic(const char *msg);
__attribute__((noreturn)) void panic_without_error();

#endif
