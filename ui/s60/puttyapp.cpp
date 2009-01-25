/*    puttyapp.cpp
 *
 * Putty UI Application class
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002,2006,2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifdef EKA2
#include <eikstart.h>
#endif
#include "puttyapp.h"
#include "puttydoc.h"
#include "puttyuids.hrh"

const TUid KUidPutty = { KUidPuttyAppDefine };

#ifndef PUTTY_S60
#error Symbol PUTTY_S60 not defined -- build environment is incorrect
#endif

TUid CPuttyApplication::AppDllUid() const {
    return KUidPutty;
}

CApaDocument* CPuttyApplication::CreateDocumentL() {
    return new (ELeave) CPuttyDocument(*this);
}


// Application entry point
EXPORT_C CApaApplication *NewApplication() {
    return new CPuttyApplication;
}

#ifdef EKA2

// EXE point
GLDEF_C TInt E32Main() {
    return EikStart::RunApplication( NewApplication );
}

#else

// DLL entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}

#endif
