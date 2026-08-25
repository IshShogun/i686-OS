#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "multiboot2.h"
#include <kernel/tty.h>
#include <kernel/panic.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
/* The osdev wiki assumes you're running linux, but macos would be __APPLE__, if its windows who f*cking cares
	 it doesnt deserve to run anyway */

#define PIXEL uint32_t   /* pixel pointer */

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

struct PSF1_Header {
    uint16_t magic; // Magic bytes for identification.
    uint8_t font_mode; // PSF font mode.
    uint8_t character_height; // PSF character size.
} ;

//make it readable without comments
extern char _binary_font_psf_start;
extern char _binary_font_psf_size_;

struct framebuffer_t FRAMEBUFFER;

int cx;
int cy;

uint32_t fg = 0x00FFFFFF;
uint32_t bg = 0x00000000;

//psf1 has a width of 8 bits and character size bits height. so its character size width 
char* font_glyphs;
uint8_t character_height;
uint8_t character_width = 8;

static bool terminal_initialised = false;


bool terminal_ready(){
	return terminal_initialised;
}

void parse_font()
{
	struct PSF1_Header *header = (struct PSF1_Header*)&_binary_font_psf_start;
	if(header->magic != 0x0436)
		panic("FONT MAGIC NUMBER ERROR");

	character_height = header->character_height;
	font_glyphs = (char*)header + sizeof(struct PSF1_Header);
}

void terminal_initialise(struct framebuffer_t _framebuffer)
{
	parse_font();          /* must come first: sets character_height */
	cx = 0;
	cy = 0;

	terminal_initialised = true;

	FRAMEBUFFER = _framebuffer;

	//To test draw from bottom
	// cy = FRAMEBUFFER.height - character_height;
}


//4 bytes per pixel. 32 bbp. - i can represent more colors
void put_pixel(int pos_x, int pos_y, uint32_t color)
{
    PIXEL* location = (PIXEL*)((char*)FRAMEBUFFER.address + FRAMEBUFFER.bytes_per_fb_row * pos_y + pos_x * 4);
    *location = color;
}

void scroll_terminal()
{
	void* dest = (void*)FRAMEBUFFER.address;
	//1 row from here 
	void* src = (void*)((char*)FRAMEBUFFER.address + (FRAMEBUFFER.bytes_per_fb_row * character_height));

	size_t count = FRAMEBUFFER.bytes_per_fb_row * (FRAMEBUFFER.height - character_height);
	memcpy(dest, src, count);

	//clear final row
	uint32_t i = 0;
	PIXEL* final_character_row_start = (PIXEL*)((char*)FRAMEBUFFER.address + 
			(FRAMEBUFFER.height - character_height) * FRAMEBUFFER.bytes_per_fb_row); 

	while(i < (FRAMEBUFFER.bytes_per_fb_row / sizeof(PIXEL)) * character_height)
	{
		final_character_row_start[i] = bg;
		i++;
	}
}

void increment_character_row()
{
	cx = 0;
	cy += character_height;
	//cant reliably write another character height wise
	if(cy >= FRAMEBUFFER.height - character_height)
	{
		scroll_terminal();
		cy -= character_height;
		return;
	}
}


//move framebuffer up. so from row worth of bytes from the top. copy to where we're at. then copy them to the top.
void draw_char(char input)
{
	//lets get the start byte for that glyph. char = 0 -> glyph one and so one
	char* starting_glyph_byte = (font_glyphs + input*character_height);

	for(size_t y = 0; y < character_height; y++)
	{
		for(size_t x = 0; x < character_width; x ++)
		{
			if(*(starting_glyph_byte + y) & (0x80 >> x))
				put_pixel(cx + x, cy + y, fg);
		}
	}
}

/* the ONLY place cursor state is touched */
void terminal_putchar(char c)
{
	if (c == '\n') {
		increment_character_row();
		return;
	}

	if (c == '\r') {
		cx = 0;
		return;
	}

	draw_char(c);
	cx += character_width;

	if (cx + character_width > FRAMEBUFFER.width)
		increment_character_row();
}

//we have two as we may want to write a slice vs until null terminator
void terminal_write(const char *data, size_t len)
{
	for (size_t i = 0; i < len; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char *s)
{
	terminal_write(s, strlen(s));
}

