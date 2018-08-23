#ifndef PORT_H_
#define PORT_H_

#include <ff.h>

// Port of File Descriptor Type
#define FILE_T FIL
// Port of File Mode Type
#define FILE_MODE_T	BYTE
// Port of File Open Modes
#define FILE_READ_ONLY				FA_READ
#define FILE_READ_WRITE				FA_READ | FA_WRITE
#define FILE_WRITE_CREATE			FA_CREATE_ALWAYS | FA_WRITE
#define FILE_READ_WRITE_CREATE		FA_CREATE_ALWAYS | FA_WRITE | FA_READ
#define FILE_APPEND					FA_OPEN_APPEND | FA_WRITE
#define FILE_READ_APPEND			FA_OPEN_APPEND | FA_WRITE | FA_READ
#define FILE_WRITE_NEW				FA_CREATE_NEW | FA_WRITE
#define FILE_READ_WRITE_NEW			FA_CREATE_NEW | FA_WRITE | FA_READ

#endif /* PORT_H_ */