/** @file epocstore.h
 *
 * Symbian OS implementation of PuTTY settings storage interface.
 *
 * Mostly copied from the SyOSsh EPOC Release 5 port by Gabor Keresztfalvi,
 * some additional fixes for the current Symbian port by Petteri Kangaslampi.
 * Originally based on winstore.c in the PuTTY distribution.
 *
 * [No copyright messages in the original, assumed to be copyrighted by
 *  Gabor Keresztfavli and available through the PuTTY license]
 *
 * Portions copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __EPOCSTORE_H__
#define __EPOCSTORE_H__

/**
 * Initializes the settings storage subsystem.
 * 
 * @param aDataPath The directory to use for stored files. Typically this is
 *                  the PuTTY application installation path. Note that this
 *                  is used for the random seed and host key files only, the
 *                  settings files always use an absolute pathname.
 */
void epoc_store_init(const TDesC &aDataPath);

/** 
 * Uninitializes the settings storage subsystem. Must be called before the
 * application exits.
 * 
 */
void epoc_store_free();

#endif
