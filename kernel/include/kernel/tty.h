#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


struct framebuffer_t {
  uint32_t *address;
  uint32_t bytes_per_fb_row;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
};

/* --- in kernel/tty.h --- */
bool terminal_ready(void);
void terminal_initialise(struct framebuffer_t);
void terminal_putchar(char c);
void terminal_write(const char *data, size_t len);
void terminal_writestring(const char *s);

#endif
