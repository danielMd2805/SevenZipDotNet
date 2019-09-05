// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#include <atlbase.h>


#if defined(_MSC_VER) && (_MSC_VER == 1200) && !defined(_WIN64)
#define MY_ARRAY_NEW(p, T, size) p = new T[(size > (unsigned)0xFFFFFFFF / sizeof(T)) ? (unsigned)0xFFFFFFFF / sizeof(T) : size];
#else
#define MY_ARRAY_NEW(p, T, size) p = new T[size];
#endif

//#include "../include/7z/CPP/Common/Common.h"
//#include "7z/CPP/Common/Common.h"

// reference additional headers your program requires here
