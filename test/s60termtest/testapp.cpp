/*    testapp.cpp
 *
 * Test Application class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifdef EKA2
#include <eikstart.h>
#endif
#include "testapp.h"
#include "testdoc.h"

#ifdef EKA2
const TUid KUidTest = { 0xf11f9001 };
#else
const TUid KUidTest = { 0x011f9001 };
#endif

TUid CTestApplication::AppDllUid() const {
    return KUidTest;
}

CApaDocument* CTestApplication::CreateDocumentL() {
    return new (ELeave) CTestDocument(*this);
}


// Application entry point
EXPORT_C CApaApplication *NewApplication() {
    return new CTestApplication;
}

#ifdef EKA2
// EXE entry point
GLDEF_C TInt E32Main() {
    return EikStart::RunApplication( NewApplication );
}
#else
// DLL entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}
#endif
