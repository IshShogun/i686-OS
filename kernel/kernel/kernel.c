#include <stdio.h>
#include <kernel/tty.h>
#include "multiboot2.h"


void panic(const char *msg) {
		if (terminal_ready())
			terminal_writestring(msg);
    asm volatile("cli");
    for (;;) asm volatile("hlt");
}

void panic_without_error() {
    asm volatile("cli");
    for (;;) asm volatile("hlt");
}

void kernel_main(unsigned long multiboot2_magic, unsigned long multiboot2_info_addr)
{
	//GOAL: have these two on top of eachother. 
	//after first draw_string we should have Hello WOrlds and an empty row under it
	terminal_writestring("Hello, world\n");
	printf("Hello, world2\n");
	printf("\n\n\n"
	"         _nnnn_\n"
	"        dGGGGMMb\n"
	"       @p~qp~~qMb\n"
	"       M|@||@) M|\n"
	"       @,----.JM|\n"
	"      JS^\\__/  qKL\n"
	"     dZP        qKRb\n"
	"    dZP          qKKb\n"
	"   fZP            SMMb\n"
	"   HZM            MMMM\n"
	"   FqM            MMMM\n"
	" __| \".        |\\dS\"qML\n"
	" |    `.       | `' \\Zq\n"
	"_)      \\.___.,|     .'\n"
	"\\____   )MMMMMP|   .'\n"
	"     `-'       `--' hjm\n"
);
}
