/*    testappui.cpp
 *
 * Test Application UI class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "testappui.h"
#include "testappview.h"
#include "testui.hrh"
#include <test.rsg>


CTestAppUi::CTestAppUi() {
}


void CTestAppUi::ConstructL() {
    BaseConstructL();
    iAppView = new (ELeave) CTestAppView(*this);
    iAppView->ConstructL(ClientRect());
    AddToStackL(iAppView);
}


CTestAppUi::~CTestAppUi() {
    RemoveFromStack(iAppView);
    delete iAppView;
}


void CTestAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {

        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit: {
            Exit();
            break;
        }

        default:
            break;
    }    
}
