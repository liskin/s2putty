/*    testappview.cpp
 *
 * Test View class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <aknutils.h> 
#include "testappui.h"
#include "testappview.h"
#include "terminalcontrols2font.h"
#include "s2font.h"

_LIT(KAssertPanic, "testappview.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

#if defined(__WINS__) || defined(__WINSCW__)
_LIT(KFontFile, "z:\\resource\\puttyfonts\\fixed6x13.s2f");
#else
_LIT(KFontFile, "c:\\resource\\puttyfonts\\fixed6x13.s2f");
#endif



CTestAppView *CTestAppView::NewL(CTestAppUi &aAppUi,
                                MTerminalObserver &aObserver,
                                const TRect &aRect) {
    CTestAppView *self = new (ELeave) CTestAppView(aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL(aObserver, aRect);
    CleanupStack::Pop();
    return self;
}


CTestAppView::CTestAppView(CTestAppUi &aAppUi)
    : iAppUi(aAppUi) {
}


CTestAppView::~CTestAppView() {
    delete iFont;
    delete iTerminal;
}


void CTestAppView::ConstructL(MTerminalObserver &aObserver,
                              const TRect &aRect) {
    CreateWindowL();
    iFont = CS2Font::NewL(KFontFile);
    SetRect(aRect);
    GetTerminalRect(iTermRect);
    iTerminal = CTerminalControlS2Font::NewL(aObserver, iTermRect, Window(),
                                             *iFont);
    iTerminal->SetMopParent(this);
    ActivateL();
    iTerminal->SetFocus(ETrue);
}


void CTestAppView::GetTerminalRect(TRect &aRect) {
    
    // Get font dimensions
    TInt fontHeight = iFont->FontSize().iHeight;
    TInt fontWidth = iFont->FontSize().iWidth;

    // Terminal maximum size
    TInt termWidth = Rect().Width();
    TInt termHeight = Rect().Height();

    // Round size down to the largest possible terminal that contains whole
    // characters
    termWidth = fontWidth * (termWidth / fontWidth);
    termHeight = fontHeight * (termHeight / fontHeight);
    assert((termWidth > 0) && (termHeight > 0));

    // Set terminal size and position
    TInt termX = Rect().iTl.iX + (Rect().Width() - termWidth) / 2;
    TInt termY = Rect().iTl.iY + (Rect().Height() - termHeight) / 2;
    aRect.SetRect(TPoint(termX, termY), TSize(termWidth, termHeight));
}


void CTestAppView::Draw(const TRect & /*aRect*/) const {
    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbWhite);    
    gc.Clear(Rect());

    const CFont *font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);    
    gc.SetPenColor(KRgbBlack);
    gc.UseFont(font);
    gc.DrawText(_L("1 Primary"), Rect().iTl + TPoint(10, 30));
    gc.DiscardFont();

    font = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    gc.UseFont(font);
    gc.DrawText(_L("Secondary"), Rect().iTl + TPoint(10, 60));
    gc.DiscardFont();

    font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    gc.UseFont(font);
    gc.DrawText(_L("Primary small"), Rect().iTl + TPoint(10, 80));
    gc.DiscardFont();
}



TInt CTestAppView::CountComponentControls() const {
//    return 1;
    return 0;
}


CCoeControl *CTestAppView::ComponentControl(TInt aIndex) const {
#if 0
    switch ( aIndex ) {
        case 0:
            return iTerminal;

        default:
            assert(EFalse);
    }
#endif
    return NULL;
}


CTerminalControl *CTestAppView::Terminal() {
    return iTerminal;
}


TKeyResponse CTestAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                          TEventCode aType) {

    return iTerminal->OfferKeyEventL(aKeyEvent, aType);
}
