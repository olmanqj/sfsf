/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 


// Print a string of characters on the debug output.
void print_debug(const char *str)
{
	extern void print_debug_port(const char *str) __attribute__((__weak__));
	if(print_debug_port) print_debug_port(str);
}

// Print a character on the debug output
void print_debug_char(char c)
{
	extern void print_debug_char_port(char c) __attribute__((__weak__));
	if(print_debug_char) print_debug_char(c);
}

// Print a char as hex representation on the  debug output.
void print_debug_hex(char c)
{
	extern void print_debug_hex_port(char c) __attribute__((__weak__));
	if(print_debug_hex) print_debug_hex(c);
}

// Prints an unsigned integer on the debug output.
void print_debug_uint(unsigned int n)
{
	extern void print_debug_uint_port(unsigned int n) __attribute__((__weak__));
	if(print_debug_uint) print_debug_uint(n);
}