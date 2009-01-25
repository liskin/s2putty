#include "fonttest.h"


/*************************
 * Application
 *************************/

const TUid KUidTest = { 0x01420074 };

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
    iAppView = new (ELeave) CTestAppView;
    iAppView->ConstructL(ClientRect());
}

CTestAppUi::~CTestAppUi() {
    delete iAppView;
}


void CTestAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {
        case ECommand0:
            break;

        case ECommand1:
            break;

        case ECommand2:
            break;
        
	case EEikCmdExit: 
            Exit();
            break;
    }
}



/*************************
 * View
 *************************/

CTestAppView::CTestAppView() {
}

CTestAppView::~CTestAppView() {
}

void CTestAppView::ConstructL(const TRect &aRect) {
    CreateWindowL();
    SetRect(aRect);
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
    TRect rect = Rect();
    CEikonEnv *eikonEnv=CEikonEnv::Static();
    CGraphicsDevice *dev = eikonEnv->ScreenDevice();

    gc.Reset();
    gc.Clear();

    TInt numTypefaces = dev->NumTypefaces();

    TInt x = 10;
    TInt y = rect.iTl.iY + 4;
    TBuf<128> buf;
    
    for ( TInt i = 0; i < numTypefaces; i++ ) {
        TTypefaceSupport tfs;
        dev->TypefaceSupport(tfs, i);
        if ( tfs.iTypeface.IsProportional() || tfs.iTypeface.IsSymbol() ) {
            continue;
        }

        int maxHeight = 0;
        x = 10;
        for ( TInt height = 8; height < 17; height += 1 ) {
            TInt twheight = dev->VerticalPixelsToTwips(height);
            TFontSpec spec(tfs.iTypeface.iName, twheight);
            CFont *font;
            PanicIfError(dev->GetNearestFontInTwips(font, spec));
            
            _LIT(KFontFormat, "%S(%d - %dx%d) ");
            TInt realHeight = font->HeightInPixels();
            TInt realWidth = font->WidthZeroInPixels();
            buf.Format(KFontFormat, &tfs.iTypeface.iName, height, realWidth,
                       realHeight);
            
            gc.UseFont(font);

            TPoint pos(x, y + font->AscentInPixels());
            gc.DrawText(buf, pos);

            // x += font->TextWidthInPixels(buf);
            x = 10;
            y += realHeight + 2;

            if ( font->HeightInPixels() > maxHeight ) {
                maxHeight = font->HeightInPixels();
            }

            gc.DiscardFont();
            dev->ReleaseFont(font);        
        }

//        y += maxHeight + 4;
#if 0
        _LIT(KFontFormat,"%S: minh %d, maxh %d, numh %d, sc %d, sy %d");
        buf.Format(KFontFormat, &tfs.iTypeface.iName, tfs.iMinHeightInTwips,
                   tfs.iMaxHeightInTwips, tfs.iNumHeights,
                   (TInt) tfs.iIsScalable, (TInt) tfs.iTypeface.IsSymbol());

        TInt height = tfs.iMinHeightInTwips;
        if ( height < 95 ) {
            height = dev->VerticalPixelsToTwips(16);
        }
        
        TFontSpec spec(tfs.iTypeface.iName, height);
        CFont *font;
        PanicIfError(dev->GetNearestFontInTwips(font, spec));
        gc.UseFont(font);

        TPoint pos(x, y + font->AscentInPixels());
        gc.DrawText(buf, pos);

        y += font->HeightInPixels() + 4;

        gc.DiscardFont();
        dev->ReleaseFont(font);
        
#endif
    }
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

