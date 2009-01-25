/*    puttyapp.cpp
 *
 * Putty UI Application class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "puttyapp.h"
#include "puttydoc.h"

const TUid KUidPutty = { 0x101f9075 };

TUid CPuttyApplication::AppDllUid() const {
    return KUidPutty;
}

CApaDocument *CPuttyApplication::CreateDocumentL() {
    return new (ELeave) CPuttyDocument(*this);
}


// Application entry point
EXPORT_C CApaApplication *NewApplication() {
    return new CPuttyApplication;
}

// DLL entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}
