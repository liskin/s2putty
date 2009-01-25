/** @file epocmemory.h
 *
 * Symbian OS memory management routines for PuTTY
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __EPOCMEMORY_H__
#define __EPOCMEMORY_H__

/** 
 * Initializes the PuTTY Symbian OS memory management. Allocates the PuTTY
 * heap.
 */
void epoc_memory_init();


/** 
 * Uninitializes the PuTTY Symbian OS memory management. Frees the PuTTY
 * heap.
 */
void epoc_memory_free();

#endif
