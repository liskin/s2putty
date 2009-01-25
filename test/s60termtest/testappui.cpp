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
    iAppView = CTestAppView::NewL(*this, *this, ClientRect());
    iAppView->SetMopParent(this);
    iTerminal = iAppView->Terminal();
    iTerminal->SetCursor(0, 0);
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


void CTestAppUi::TerminalSizeChanged(TInt aWidth, TInt aHeight) {
    iTermWidth = aWidth;
    iTermHeight = aHeight;
    if ( iTermX >= iTermWidth ) {
        iTermX = iTermWidth - 1;
    }
    if ( iTermY >= iTermHeight ) {
        iTermY = iTermHeight - 1;
    }
}


void CTestAppUi::KeyPressed(TKeyCode aCode, TUint /*aModifiers*/) {
    TChar c((TUint)aCode);
    if ( c.IsPrint() ) {
        TBuf<1> buf;
        buf.Append(c);
        iTerminal->DrawText(iTermX, iTermY, buf, EFalse, EFalse,
                            KRgbBlack, KRgbWhite);
        iTermX++;
        if ( iTermX >= iTermWidth ) {
            iTermX = 0;
            iTermY++;
            if ( iTermY >= iTermHeight ) {
                iTermY = 0;
            }
        }
        iTerminal->SetCursor(iTermX, iTermY);
    } else {
        switch ( aCode ) {
            case EKeyEnter: 
                iTermX = 0;
                iTermY++;
                if ( iTermY >= iTermHeight ) {
                    iTermY = 0;
                }
                iTerminal->SetCursor(iTermX, iTermY);
                break;

            case EKeyBackspace:
                if ( (iTermX > 0) || (iTermY > 0) ) {
                    iTermX--;
                    if ( iTermX < 0 ) {
                        iTermX = iTermWidth - 1;
                        iTermY--;
                    }                    
                    iTerminal->SetCursor(iTermX, iTermY);
                    iTerminal->DrawText(iTermX, iTermY, _L(" "), EFalse,
                                        EFalse, KRgbBlack, KRgbWhite);
                }
                break;

            case EKeyUpArrow:
                if ( iTermY > 0 ) {
                    iTermY--;
                    iTerminal->SetCursor(iTermX, iTermY);
                }
                break;

            case EKeyDownArrow:
                if ( iTermY < (iTermHeight-1) ) {
                    iTermY++;
                    iTerminal->SetCursor(iTermX, iTermY);
                }
                break;

            case EKeyLeftArrow:
                if ( iTermX > 0 ) {
                    iTermX--;
                    iTerminal->SetCursor(iTermX, iTermY);
                }
                break;
                
            case EKeyRightArrow:
                if ( iTermX < (iTermWidth-1) ) {
                    iTermX++;
                    iTerminal->SetCursor(iTermX, iTermY);
                }
                break;

            default:
                break;
        }
    }
}
