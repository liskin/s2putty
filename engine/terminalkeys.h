/** @file terminalkeys.h
 *
 * Translates keypresses according to current terminal emulation settings.
 * This really should be a part of the terminal emulation code, and not in the
 * frontend!
 *
 * The keyboard translation code also should use some kind of Putty-internal
 * key codes, instead of system-specific codes, which would make it portable.
 * Now each port has to handle keyboard completely separately.
 *
 * Based on TranslateKey() in window.c.
 *
 * Copyright 2002, 2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TERMINALKEYS_H__
#define __TERMINALKEYS_H__

#include <e32std.h>
#include <e32keys.h>

/**
 * Translates a control character keypress.
 *
 * @param aCfg Current PuTTY configuration
 * @param aTerm Current terminal in use
 * @param aKey The key
 * @param aModifiers Key modifiers (TKeyEvent::iModifiers)
 * @param aOutput Output buffer, 16 bytes is enough
 *
 * @return The number of bytes of output written, -1 if nothing was translated.
 */
TInt TranslateKey(Config *aCfg, Terminal *aTerm, TKeyCode aKey,
                  TUint aModifiers, char *aOutput);


#endif
