/*    testapp.cpp
 *
 * Test Application class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "testapp.h"
#include "testdoc.h"

const TUid KUidTest = { 0x011f9001 };

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

// DLL entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}
