#include <stdint.h>     
#include <kernel/tty.h>          
#include <kernel/panic.h>        
#include <kernel/kernel.h>       
#include "multiboot2.h"   

//see gnu multiboot2 specification
void read_boot_info_multiboot2(unsigned long multiboot2_magic, unsigned long multiboot2_info_addr)
{  
	struct multiboot_tag *tag; //in mulitboot.h, by specification
	unsigned size; //shorthand for unsigned int

	if (multiboot2_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		panic_without_error();
	}

	if (multiboot2_info_addr & 7)
	{
		panic_without_error();
	}

	//the first thing at the info addr is just a uint32 total_size by the specification - header. so we dereference it to  get first value
	size = *(unsigned *) multiboot2_info_addr;

	for (tag = (struct multiboot_tag *) (multiboot2_info_addr + 8); 
			tag->type != MULTIBOOT_TAG_TYPE_END; 
			tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
				+ ((tag->size + 7) & ~7)))
	{
		// printf ("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_MMAP:
				{
					/* TODO: feed into physical memory manager */
					break;
				}
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				{
					multiboot_uint32_t color;
					unsigned i;
					struct multiboot_tag_framebuffer *tagfb
						= (struct multiboot_tag_framebuffer *) tag;

					//compound literal. (fun fact: you can use it in a declaration FRAMEBUFFER = {literal} to enforce a copy elision. Compiler sees literal not used, just stores in framebuffer. (not pushed to stack)
					terminal_initialise((struct framebuffer_t){
							.address          = (uint32_t *)(uintptr_t)tagfb->common.framebuffer_addr,
							.bytes_per_fb_row = tagfb->common.framebuffer_pitch,
							.width            = tagfb->common.framebuffer_width,
							.height           = tagfb->common.framebuffer_height,
							.bpp              = tagfb->common.framebuffer_bpp,
							});
					break;
				} 

		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
			+ ((tag->size + 7) & ~7));
}

void kernel_entry(unsigned long multiboot2_magic, unsigned long multiboot2_info_addr)
{
	read_boot_info_multiboot2(multiboot2_magic, multiboot2_info_addr);
	kernel_main();
}

