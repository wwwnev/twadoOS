#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// With C in freestanding mode, some headers can be included as they are part of the compiler

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color { // After, those are variable. Using VGA_COLOR_BLACK as argument will be like using 0.
                 // enums are basically int replacements. Here, writing = 0, etc. is redundant i think.
                 // Try to never return an int like 0,1 or 2. Replace with an enum return (typedef it beforehand?)
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

// uint8_t is the same as a byte. A type of unsigned integer of length 8 bits
// uint8_t is equal to unsigned char. Basically just a byte, but representing a char.
// uint 16_t is equal to unsigned short. 2 bytes
// uint32_t is equal to unsigned int. 4 bytes
// uint64_t is equal to unsigned long long. 8 bytes
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) // these enum vga_color are basically 4 bits (leading 4 bits always 0)
{
  return fg | bg << 4; // The shift has precedence over the bitwise OR
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

// size_t a type which is used to represent the size of objects in bytes and is therefore used as the return type by the sizeof operator
// const char* is a mutable pointer to an immutable character/string
size_t strlen(const char* str) 
{
  // returns the length of a string
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
// static outisde a function = scope limited to inside the .c file
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;


// somehow, these variables are not popped from the stack ever? So we can just go ahead and modify them (even use their name?) in following functions?
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer; // pointer, as this is an array
                           // my_array[index] is syntaxic sugar for: *(my_array + index)
                           // since this is not actually an array, it doesn't have a fixed size
                           // Dynamic size => possible segfault
                           // to avoid segfault: malloc(sizeof(my_type * size_of_array))
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000; // why this address?
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color); // vga_entry returns a uint16_t, so it takes 2 bytes to store each. it's ok to use an incrementing index,
                                                                                 // I think because terminal_buffer is of type uint16_t* 
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\n");
}
