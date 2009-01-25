/*    terminalkeys.cpp
 *
 * Sends a key to the output, taking terminal emulation settings into account.
 * This really should be a part of the terminal emulation code, and not in the
 * frontend!
 *
 * The keyboard translation code also should use some kind of Putty-internal
 * key codes, instead of system-specific codes, which would make it portable.
 * Now each port has to handle keyboard completely separately.
 *
 * Based on TranslateKey() in window.c.
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * PuTTY is copyright 1997-2001 Simon Tatham.
 *
 * Portions copyright Robert de Bath, Joris van Rantwijk, Delian
 * Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
 * and CORE SDI S.A.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <e32std.h>
#include <e32cons.h>

extern "C" {
    #include "putty.h"
}
#include <assert.h>

#include "sshtestconsole.h"

int TranslateKey(TKeyCode aKey, TUint aModifiers, char *output);


// Translates a key and sends it
void SendKey(TKeyCode aCode, TUint aModifiers) {

    char buf[16];

    // Reset scrollback
    seen_key_event = 1;

    // Interrupt paste
    term_nopaste();

    // Try to translate as a control character
    int xbytes = TranslateKey(aCode, aModifiers, buf);
    assert(xbytes < 16);
    if ( xbytes > 0 ) {
        // Send control key codes as is
        ldisc_send(buf, xbytes, 1);
        
    } else if ( xbytes == -1 ) {
        // Not translated, probably a normal letter
        if ( aCode < ENonCharacterKeyBase ) {
            wchar_t wc = (wchar_t) aCode;
            luni_send(&wc, 1, 1);
        }
    }
}


// Translates control character keypress. Returns the number of bytes of output
// written. Returns -1 if nothing was translated.
int TranslateKey(TKeyCode aKey, TUint aModifiers, char *output) {
    
    char *p = output;
    TUint shiftMod = aModifiers &
        (EModifierFunc | EModifierCtrl | EModifierShift);

    // First some basic control characters

    if ( (aKey == EKeyBackspace) && (shiftMod == 0) ) {
        // Backspace
        *p++ = (char) (cfg.bksp_is_delete ? 0x7F : 0x08);
        return 1;
    }
    if ( (aKey == EKeyTab) && (shiftMod == EModifierShift) ) {
        // Shift-Tab
        *p++ = 0x1B;
        *p++ = '[';
        *p++ = 'Z';
        return p - output;
    }
    if ( (aKey == EKeySpace) && (shiftMod == EModifierCtrl) ) {
        // Ctrl-Space
        *p++ = 0;
        return p - output;
    }
    if ( (aKey == EKeySpace) && (shiftMod == (EModifierCtrl | EModifierShift)) ) {
        // Ctrl-Shift-Space
        *p++ = 0;
        return p - output;
    }
    if ( (shiftMod == EModifierCtrl) && (aKey >= '2') && (aKey <= '8') ) {
        // Ctrl-2 through Ctrl-8
        *p++ = "\000\033\034\035\036\037\177"[aKey - '2'];
        return p - output;
    }
    if ( (aKey == EKeyEnter) && cr_lf_return ) {
        // Enter (if it should send CR + LF)
        *p++ = '\r';
        *p++ = '\n';
        return p - output;
    }

    // Page up / page down (use chr+up/down)
    int code = 0;
    if ( (aKey == EKeyPageUp) ||
         ((shiftMod & EModifierFunc) && (aKey == EKeyUpArrow)) ) {
        code = 5;
    }
    if ( (aKey == EKeyPageDown) ||
         ((shiftMod & EModifierFunc) && (aKey == EKeyDownArrow)) ) {
        code = 6;
    }
    if ( code > 0 ) {
	/* Reorder edit keys to physical order */
	if (cfg.funky_type == 3 && code <= 6)
	    code = "\0\2\1\4\5\3\6"[code];
        
	if (vt52_mode && code > 0 && code <= 6) {
	    p += sprintf((char *) p, "\x1B%c", " HLMEIG"[code]);
	    return p - output;
	}
        
	if (cfg.funky_type == 5 &&     /* SCO small keypad */
	    code >= 1 && code <= 6) {
	    char codes[] = "HL.FIG";
	    if (code == 3) {
		*p++ = '\x7F';
	    } else {
		p += sprintf((char *) p, "\x1B[%c", codes[code-1]);
	    }
	    return p - output;
	}
        
        p += sprintf((char *) p, "\x1B[%d~", code);
        return p - output;
    }

    // Arrows
    char xkey = 0;
    switch ( aKey ) {
        case EKeyUpArrow:
            xkey = 'A';
            break;
        case EKeyDownArrow:
            xkey = 'B';
            break;
        case EKeyRightArrow:
            xkey = 'C';
            break;
        case EKeyLeftArrow:
            xkey = 'D';
            break;
    }
    if ( xkey ) {
        if (vt52_mode)
            p += sprintf((char *) p, "\x1B%c", xkey);
        else {
            int app_flg = (app_cursor_keys && !cfg.no_applic_c);
            
            /* Useful mapping of Ctrl-arrows */
            if ( shiftMod == EModifierCtrl )
                app_flg = !app_flg;

            if (app_flg)
                p += sprintf((char *) p, "\x1BO%c", xkey);
            else
                p += sprintf((char *) p, "\x1B[%c", xkey);
        }
        return p - output;
    }

    // OK, handle it as a normal unicode key
    return -1;
}
