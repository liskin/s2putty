/*    epocmisc.cpp
 *
 * Miscellaneous Symbian OS funtions and definitions for PuTTY
 *
 * Copyright 2002,2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <e32svr.h>

extern "C" {
#include "putty.h"
#include "ssh.h"
#include "assert.h"
}


const char platform_x11_best_transport[] = "localhost";


DWORD GetTickCount() {
    TTime time;
    time.UniversalTime();
    TInt64 t64 = time.Int64();
#ifdef EKA2
    return I64LOW((t64 / TInt64(1000)));
#else    
    return (t64 / TInt64(1000)).GetTInt();
#endif
}


unsigned GetCaretBlinkTime() {
    return 500;
}


FontSpec platform_default_fontspec(const char * /*name*/) {
    FontSpec ret;
    ret.name[0] = '\0';
    return ret;
}

Filename platform_default_filename(const char *name) {
    Filename ret;
    if (!strcmp(name, "LogFileName"))
	strcpy(ret.path, "putty.log");
    else
	*ret.path = '\0';
    return ret;
}

char *platform_default_s(const char * /*name*/) {
    return NULL;
}

int platform_default_i(const char * /*name*/, int def) {
    return def;
}

void platform_get_x11_auth(char * /*display*/, int * /*proto*/,
                           unsigned char * /*data*/, int * /*datalen*/) {
    /* We don't support this at all under Symbian OS. */
}


char *platform_get_x_display(void) {
    /* No X11 support */
    return dupstr("");
}


Filename filename_from_str(const char *str) {
    Filename ret;
    strncpy(ret.path, str, sizeof(ret.path));
    ret.path[sizeof(ret.path)-1] = '\0';
    return ret;
}

const char *filename_to_str(const Filename *fn) {
    return fn->path;
}

int filename_equal(Filename f1, Filename f2) {
    return !strcmp(f1.path, f2.path);
}

int filename_is_null(Filename fn) {
    return !*fn.path;
}


void set_statics_tls(Statics* statics) {
    Dll::SetTls((TAny*)statics);
}

Statics *const statics_tls() {
    return (Statics *const) Dll::Tls();
}

void remove_statics_tls() {
    Dll::SetTls(NULL);
}
