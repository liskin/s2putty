/*    testdoc.cpp
 *
 * Test UI Document class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "testdoc.h"
#include "testappui.h"


CTestDocument::CTestDocument(CAknApplication &aApp)
    : CAknDocument(aApp) {
}

CEikAppUi* CTestDocument::CreateAppUiL() {
    return new (ELeave) CTestAppUi;
}
