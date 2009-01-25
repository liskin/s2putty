/*    epocmemory.cpp
 *
 * Symbian OS memory management routines for PuTTY
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <e32svr.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#include "putty.h"
#include "assert.h"
}


void epoc_memory_init() {
}


void epoc_memory_free() {
}


void *safemalloc(size_t n, size_t size)
{
    void *p;
    if (n > 0x7fffffff / size) {
	p = NULL;
    } else {
	size *= n;
	if (size == 0) size = 1;
        p = User::Alloc((TInt)size);
    }
    if ( !p ) {
        fatalbox("Out of memory");
        exit(1);
    }

    return p;
}

void *saferealloc(void *ptr, size_t n, size_t size)
{
    void *p;
    
    if (n > 0x7fffffff / size) {
	p = NULL;
    } else {
	size *= n;
        if (!ptr) {
            p = User::Alloc((TInt)size);
        } else {
            p = User::ReAlloc(ptr, (TInt) size);
        }
    }
        
    if (!p) {
        fatalbox("Out of memory");
        exit(1);
    }

    return p;
}

void safefree(void *ptr)
{
    if ( ptr ) {
        User::Free(ptr);
    }
}
