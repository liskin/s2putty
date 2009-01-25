/*    logfile.h
 *
 * File logging macros. Slow but should work in any situation.
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#ifdef LOGFILE_ENABLED

#include <e32std.h>

void LogFilePrint(TRefByValue<const TDesC> aFormat, ...);
#define LFPRINT(x) LogFilePrint x

#else

#define LFPRINT(x)

#endif

#endif
