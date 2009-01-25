/** @file epocmisc.h
 *
 * Miscellaneous Symbian OS funtions and definitions for PuTTY
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __EPOCMISC_H__
#define __EPOCMISC_H__

/* Disable some warnings when compiling the original PuTTY C source code.
   The Symbian build tools run Visual C at warning level 4, and the code
   isn't really written for that... */
#ifndef __cplusplus
#ifdef __WINS__
#pragma warning(disable : 4127) /* conditional expression is constant */
#pragma warning(disable : 4100) /* unreferenced formal parameter */
#pragma warning(disable : 4057) /* XX differs in indirection to slightly different base types from YY */
#pragma warning(disable : 4244) /* conversion from X to Y, possible loss of data */
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int DWORD;
typedef wchar_t WCHAR;
typedef unsigned char BYTE;
#ifndef min
#define min(x,y) ((x) <= (y) ? (x) : (y))
#define max(x,y) ((x) >= (y) ? (x) : (y))
#endif
#define CP_ACP 0x12345678

DWORD GetTickCount();
unsigned GetCaretBlinkTime();

    

#ifdef __cplusplus
}
#endif

#endif
