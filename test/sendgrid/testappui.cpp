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
#include "sendgrid.h"
#include <test.rsg>
#include <aknpopup.h>
#include <akngrid.h>
#include <aknlists.h>
#include <aknutils.h>
#include <aknnotewrappers.h>


CTestAppUi::CTestAppUi() {
}


void CTestAppUi::ConstructL() {
    BaseConstructL(CAknAppUi::EAknEnableSkin);
    iAppView = CTestAppView::NewL(*this, *this, ClientRect());
    iAppView->SetMopParent(this);
    iTerminal = iAppView->Terminal();
    iTerminal->SetCursor(0, 0);
    AddToStackL(iAppView);
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
}


CTestAppUi::~CTestAppUi() {
    if ( iGrid ) {
        RemoveFromStack(iGrid);
        delete iGrid;
    }
    RemoveFromStack(iAppView);
    delete iAppView;
    delete iIdle;
}


void CTestAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {

        case ETestCmdShowGrid:
            if ( !iGrid ) {
                TSize size;
                CSendGrid::GetRecommendedSize(size);
                TRect cr = ClientRect();
                if ( size.iWidth > cr.Width() ) {
                    size.iWidth = cr.Width();
                }
                if ( size.iHeight > cr.Height() ) {
                    size.iHeight = cr.Height();
                }
                TPoint tl = cr.iTl + TSize((cr.Width() - size.iWidth) / 2,
                                           (3*(cr.Height() - size.iHeight)) / 4);
                TPoint br = cr.iBr - TSize((cr.Width() - size.iWidth) / 2,
                                           (cr.Height() - size.iHeight) / 4);
                iGrid = CSendGrid::NewL(TRect(tl, br), R_TEST_GRID, *this);
                AddToStackL(iGrid);
            }
            break;

        case ETestCmdRemoveGrid:
            if ( iGrid ) {
                RemoveFromStack(iGrid);
                delete iGrid;
                iGrid = NULL;
            }
            break;
            
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


void CTestAppUi::MsgoCommandL(TInt aCommand) {
    
    CAknInformationNote *note = new (ELeave) CAknInformationNote(ETrue);
    note->SetTone(CAknInformationNote::ENoTone);
    note->SetTimeout(CAknInformationNote::EShortTimeout);
    TBuf<32> buf;
    buf.Format(_L("Command %d"), aCommand);
    note->ExecuteLD(buf);
    
    if ( iGrid ) {
        RemoveFromStack(iGrid);
        delete iGrid;
        iGrid = NULL;
    }
}


void CTestAppUi::MsgoTerminated() {
    if ( iGrid ) {
        RemoveFromStack(iGrid);
        delete iGrid;
        iGrid = NULL;
    }
}


void CTestAppUi::IdleL() {

}


TInt CTestAppUi::IdleCallback(TAny *aAny) {
    TRAPD(error, ((CTestAppUi*)aAny)->IdleL());
    if ( error != KErrNone ) {
        User::Panic(_L("Error"), error);
    }
    return 0;
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
