/** @file assert.h
 * 
 * A new assert.h to override a broken one in the 9210i SDK version 1.2.
 * Panics the application if an assertion fails.
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTY_ASSERT_H__
#define __PUTTY_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DO_ASSERT_ALWAYS

/**
 * @def assert(e)
 *
 * A replacement assert macro. Panics the application if the expression
 * is false, using the current source file name as the panic category
 * and the line number as the panic code.
*/

#if defined(NDEBUG) && !defined(DO_ASSERT_ALWAYS)
#define assert(e)		( )
#else
#define assert(e) ((e) ? (void)0 : assert_failed(__FILE__, __LINE__, #e))
#endif

void assert_failed(const char *file, int line, const char *expr);

#ifdef __cplusplus
}
#endif


#endif
