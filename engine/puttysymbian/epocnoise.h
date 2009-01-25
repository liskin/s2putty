/** @file epocnoise.h
 *
 * Symbian OS implementation of the noise generation for PuTTY's
 * cryptographic random number generator.
 *
 * Mostly copied from noise_epoc.c in the the SyOSsh EPOC Release 5 port by
 * Gabor Keresztfalvi, some additional fixes for the current Symbian port by
 * Petteri Kangaslampi.
 * Originally baset on noise.c in the PuTTY distribution.
 *
 * [No copyright messages in the original, assumed to be copyrighted by
 *  Gabor Keresztfavli and available through the PuTTY license]
 *
 * Portions copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __EPOCNOISE_H__
#define __EPOCNOISE_H__


/** 
 * Initializes noise generation.
 */
void epoc_noise_init();


/** 
 * Frees any memory and handles associated with the noise generator.
 * 
 */
void epoc_noise_free(void);


#endif
