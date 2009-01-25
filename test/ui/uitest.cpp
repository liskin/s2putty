#include <e32math.h>
#include "uitest.h"

_LIT(KUITest, "uitest.cpp");
static const TInt KDefaultFontHeight = 8;

#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KUITest, __LINE__))


/*************************
 * Application
 *************************/

const TUid KUidTest = { 0x01420075 };

TUid CTestApplication::AppDllUid() const {
    return KUidTest;
}

CApaDocument *CTestApplication::CreateDocumentL() {
    return new (ELeave) CTestDocument(*this);
}



/*************************
 * Document
 *************************/

CTestDocument::CTestDocument(CEikApplication &anApp)
    : CEikDocument(anApp) {
}

CEikAppUi *CTestDocument::CreateAppUiL() {
    return new (ELeave) CTestAppUi;
}



/*************************
 * Application UI
 *************************/

CTestAppUi::CTestAppUi() {
}

void CTestAppUi::ConstructL() {
    BaseConstructL();
    iWriteLine = 0;
    iTermWidth = 80;
    iTermHeight = 24;
    iTermBuf = new (ELeave) TUint16[iTermWidth * iTermHeight];
    for ( TInt i = 0; i < iTermWidth*iTermHeight; i++ ) {
        iTermBuf[i] = (TUint16) ' ';
    }
    iAppView = new (ELeave) CTestAppView(this);
    iAppView->ConstructL(ClientRect());
    AddToStackL(iAppView);
}


CTestAppUi::~CTestAppUi() {
    delete iAppView;
    delete [] iTermBuf;
}


void CTestAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {
        case ECmdWrite:
        {
            TUint16 *p = &iTermBuf[iWriteLine * iTermWidth];
            for ( TInt i = 0; i < iTermWidth; i++ ) {
                *p++ = (TUint16) (' ' + (Math::Rand(iSeed) & 63));
            }
            DrawLine(iWriteLine);
            iWriteLine++;
            if ( iWriteLine >= iTermHeight ) {
                iWriteLine = 0;
            }
            break;
        }

        case ECmdScroll:
        {
            Mem::Copy(iTermBuf, &iTermBuf[iTermWidth],
                      sizeof(TUint16) * (iTermWidth * (iTermHeight-1)));
            TUint16 *p = &iTermBuf[iTermWidth * (iTermHeight-1)];
            for ( TInt i = 0; i < iTermWidth; i++ ) {
                *p++ = (TUint16) (' ' + (Math::Rand(iSeed) & 63));
            }
            for ( TInt line = 0; line < iTermHeight; line++ ) {
                DrawLine(line);
            }
            break;
        }
        
        case ECommand2:
            break;
        
	case EEikCmdExit: 
            Exit();
            break;
    }
}


void CTestAppUi::TerminalSizeChanged(TInt aWidth, TInt aHeight) {
    iTermWidth = aWidth;
    iTermHeight = aHeight;
    if ( iWriteLine >= iTermHeight ) {
        iWriteLine = iTermHeight - 1;
    }
    delete [] iTermBuf;
    iTermBuf = new (ELeave) TUint16[iTermWidth * iTermHeight];
    for ( TInt i = 0; i < iTermWidth*iTermHeight; i++ ) {
        iTermBuf[i] = (TUint16) ' ';
    }
}


void CTestAppUi::KeyPressed(TKeyCode aCode, TUint aModifiers) {
    TBuf<32> buf;
    _LIT(KFormat, "code %d, mod %08x");
    buf.Format(KFormat, (TInt) aCode, aModifiers);
    CEikonEnv::Static()->InfoMsg(buf);
}


void CTestAppUi::RePaintWindow() {
    for ( TInt i = 0; i < iTermHeight; i++ ) {
        DrawLine(i);
    }
}


void CTestAppUi::DrawLine(TInt aLine) {

    assert(aLine < iTermHeight);
    assert(aLine >= 0);

    TInt first = iTermWidth/2;    
    TPtrC ptr(&iTermBuf[aLine * iTermWidth], first);
    TRgb bg((255 * aLine) / iTermHeight, 0, 0);
    TRgb fg(0, (255 * (iTermHeight - aLine)) / iTermHeight, 0);
    iAppView->Terminal()->DrawText(0, aLine, ptr, EFalse, EFalse, fg, bg);
    ptr.Set(&iTermBuf[(aLine * iTermWidth) + first], iTermWidth - first);
    iAppView->Terminal()->DrawText(first, aLine, ptr, EFalse, ETrue, bg, fg);            
}





/*************************
 * View
 *************************/

CTestAppView::CTestAppView(MTerminalObserver *anObserver) {
    iTerminalObserver = anObserver;
}

CTestAppView::~CTestAppView() {
    delete iTerminal;
}

void CTestAppView::ConstructL(const TRect &aRect) {
    CreateWindowL();
    SetRect(aRect);
    iTerminal = new (ELeave) CTerminalControl(iTerminalObserver);
    iTerminal->ConstructL(Rect(), Window());
    ActivateL();
}


void PanicIfError(TInt anError) {
    if ( anError != KErrNone ) {
        _LIT(KPanic, "Error");
        User::Panic(KPanic, anError);
    }
}


void CTestAppView::Draw(const TRect & /*aRect*/) const {
    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.Clear();
}


TInt CTestAppView::CountComponentControls() const {
    return 1;
}


CCoeControl *CTestAppView::ComponentControl(TInt aIndex) const {
    
    switch ( aIndex ) {
        case 0:
            return iTerminal;

        default:
            assert(EFalse);
    }

    return NULL;
}


TKeyResponse CTestAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                          TEventCode aType) {
    return iTerminal->OfferKeyEventL(aKeyEvent, aType);
}


CTerminalControl *CTestAppView::Terminal() {
    return iTerminal;
}




/*************************
 * Static methods
 *************************/

// Application entry point
EXPORT_C CApaApplication *NewApplication() {
    return new CTestApplication;
}

// DLL entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}

