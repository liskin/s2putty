/*    assert.cpp
 *
 * A new assert.h to override a broken one in the 9210i SDK version 1.2.
 * Panics the application if an assertion fails.
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <assert.h>
#include "charutil.h"

extern "C" void assert_failed(const char *file, int line,
                              const char * /*expr*/) {

    TPtr *fdes = CreateDes(file);
    if ( fdes->Length() < 16 ) {
        User::Panic(*fdes, line);
    } else {
        User::Panic(fdes->Right(16), line);
    }
}
