/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
 
#include <stdlib.h>
#include <sfsf_port.h>



int file_open(FILE_T * fp, const char *path, FILE_MODE_T mode)
{
	extern int file_open_port(FILE_T * fp, const char *path, FILE_MODE_T mode) __attribute__((__weak__));
	if(file_open_port) return file_open_port(fp, path, mode);
	else return -1;
}

int file_close(FILE_T * fp)
{
	extern int file_close_port(FILE_T * fp) __attribute__((__weak__));
	if(file_close_port) return file_close_port(fp);
	else return -1;
}

char * file_read(FILE_T * fp, char * buff,  int len)
{
	extern char * file_read_port( FILE_T * fp, char * buff,  int len) __attribute__((__weak__));
	if(file_read_port) return file_read_port(fp, buff,  len);
	else return NULL;
}

int file_write( FILE_T* fp , const char* str )
{
	extern int file_write_port( FILE_T* fp, const char* str ) __attribute__((__weak__));
	if(file_write_port) return file_write_port( fp, str );
	else return -1;
}


int file_remove(const char *path)
{
	extern int file_remove_port( const char* path ) __attribute__((__weak__));
	if(file_remove_port) return file_remove_port(  path );
	else return -1;
}
