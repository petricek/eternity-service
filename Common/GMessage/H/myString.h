#ifndef MYSTRING_H
#define MYSTRING_H

#include "./../../H/common.h"

/// strlen that handles NULL argument
int MyStrLen(const char *string);
/// Modified version of strcpy
Char *MyStrCpy(Char *c1, Char *c2, Size len,
               Int firstPos_c1 = 0, Int firstPos_c2 = 0);

/// Modified version of strcmp
Int MyStrCmp(Char *c1, Char *c2, Size len);

/// Finds an occurance of character in file
Size MyStrChar(Char *aBuffer, Char aChar, Size len);

#endif
