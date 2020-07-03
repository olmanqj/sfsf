#include <stdio.h>

// Port of File Descriptor Type
#define FILE_T                      FILE *
// Port of File Mode Type
#define FILE_MODE_T	               const char *
// Port of File Open Modes
#define FILE_READ_ONLY              "r"
#define FILE_READ_WRITE             "r+"
#define FILE_WRITE_CREATE           "w"
#define FILE_READ_WRITE_CREATE			"w+"
#define FILE_APPEND                 "a"
#define FILE_READ_APPEND            "a+"
#define FILE_WRITE_NEW              "wx"
#define FILE_READ_WRITE_NEW         "w+x"
