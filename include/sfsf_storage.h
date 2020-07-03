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
#ifndef SFSF_STORAGE_H_
#define SFSF_STORAGE_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_storage.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file	sfsf_storage.h
 * @brief	API for Storage Service

Storage Service
=================

Features Summary
-------------
- Open and close files
- Write and read bytes into files
- Retrieve file stats


Module Description
-----------------
The storage service provides the essential functions for handling files.
Other services, like the Param Service, Log Service, Housekeeping Service,
may require this functions for providing persistence features, if so
specific in the configuration file. Note that all the functions for
managing files shall be ported, see sfsf_port.h.

*/


/**
 * @brief	Open or create a file
 * @param	fp			Pointer to a File descriptor to store file info
 * @param	path		Pathname of file to open or create
 * @param	mode		Mode to open or create file
 * @return	-1 if fails, 0 if OK
*/
int file_open(FILE_T * fp, const char *path, FILE_MODE_T mode);

/**
 * @brief	Close a file descriptor
 * @param	fp		File descriptor of open file
 * @return	-1 if fails, 0 if OK
*/
int file_close(FILE_T * fp);

/**
 * @brief	Read a string (until new-line or end-of-file) from a file descriptor
 * @param	fp		File descriptor of open file
 * @param	buff	Destination buffer to store read bytes
 * @param	len		Limit bytes to read
 * @return	-1 if fails, the number of bytes read if OK
*/
char * file_read( FILE_T * fp, char * buff,  int len);

/**
 * @brief	Write a string to a file descriptor
 * @param	fp		File descriptor of open file
 * @param	str		Source buffer whit sting to write into file
 * @return	-1 if fails, the number of bytes written if OK
*/
int file_write( FILE_T* fp, const char* str );


/**
 * @brief	Remove a file
 * @param	path		Pathname of file to remove
 * @return	-1 if fails, 0 if OK
*/
int file_remove(const char *path);


#ifdef __cplusplus
}
#endif

#endif /* SFSF_STORAGE_H_ */
