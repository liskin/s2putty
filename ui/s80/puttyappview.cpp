/*    puttyappview.cpp
 *
 * Putty UI View class
 *
 * Copyright 2002,2009 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <eikenv.h>
#include <apgtask.h>
#include <eikspane.h>
#include <eikbtgpc.h>
#include "puttyappui.h"
#include "puttyappview.h"
#include "puttyui.hrh"

_LIT(KAssertPanic, "puttyappview.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

static const TInt KSmallFontWidth = 6;
static const TInt KSmallFontHeight = 8;

static const TInt KLargeFontWidth = 7;
static const TInt KLargeFontHeight = 14;

static const TInt KDefaultTermWidth = 80;
static const TInt KDefaultTermHeight = 24;


CPuttyAppView::CPuttyAppView(MTerminalObserver *anObserver,
                             CPuttyAppUi *aAppUi) {
    iTerminalObserver = anObserver;
    iAppUi = aAppUi;
}

CPuttyAppView::~CPuttyAppView() {
    delete iTerminal;
}


void CPuttyAppView::ConstructL(const TRect &aRect) {
    CreateWindowL();
    SetRect(aRect);
    iDefaultFg = KRgbBlack;
    iDefaultBg = KRgbWhite;

    // Determine the correct size and position for the terminal. We'll aim
    // at a default size initially, instead of making it as big as possible
    TInt termWidth = Rect().Width();
    TInt termHeight = Rect().Height();
    if ( termWidth > (KDefaultTermWidth * KSmallFontWidth) ) {
        termWidth = KDefaultTermWidth * KSmallFontWidth;
    }
    if ( termHeight > (KDefaultTermHeight * KSmallFontHeight) ) {
        termHeight = KDefaultTermHeight * KSmallFontHeight;
    }
    TInt termX = Rect().iTl.iX + (Rect().Width() - termWidth) / 2;
    TInt termY = Rect().iTl.iY + (Rect().Height() - termHeight) / 2;
    iTermRect = TRect(TPoint(termX, termY), TSize(termWidth, termHeight));
    
    iTerminal = CTerminalControlSystemFont::NewL(
        *iTerminalObserver, iTermRect, Window());
    ActivateL();
    iTerminal->SetFocus(ETrue);
}


void CPuttyAppView::Draw(const TRect & /*aRect*/) const {

    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.SetClippingRect(Rect());

    // Determine terminal window borders. If we're using the small font in
    // non full screen mode, draw a rectangle around it to mark the area since
    // it is significantly smaller than the window. Otherwise we won't bother
    // since the rectangle wouldn't look too good...
    TRect borderRect = iTermRect;
    if ( (!iFullScreen) && (!iLargeFont) ) {
        gc.SetPenColor(iDefaultFg);
        borderRect.Grow(1, 1);
        gc.DrawRect(borderRect);
    }

    // Clear everything outside the terminal
    TRegionFix<5> clearReg(Rect());
    clearReg.SubRect(borderRect);
    assert(!clearReg.CheckError());
    const TRect *rects = clearReg.RectangleList();
    TInt numRects = clearReg.Count();
    
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(iDefaultBg);
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    while ( numRects-- ) {
        gc.DrawRect(*(rects++));
    }
}


TInt CPuttyAppView::CountComponentControls() const {
    return 1;
}


CCoeControl *CPuttyAppView::ComponentControl(TInt aIndex) const {
    
    switch ( aIndex ) {
        case 0:
            return iTerminal;

        default:
            assert(EFalse);
    }

    return NULL;
}


TKeyResponse CPuttyAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                          TEventCode aType) {

    // Normally we'll just let the terminal control handle the event, but
    // there are some exceptions:
    // - We'll offer the select key on a 9300/9500 first to the application UI
    //   so that it can be used to mark/copy in select mode. Normally it acts
    //   as Enter.
    // (The system FEP will handle Chr and open the character selection dialog)

    // Selection key
    if ( (aType == EEventKey) && (aKeyEvent.iScanCode == EStdKeyDeviceA) ) {
        if ( iAppUi->OfferSelectKeyL() ) {
            return EKeyWasConsumed;
        }
    }

    // Default -- let the terminal control handle it
    return iTerminal->OfferKeyEventL(aKeyEvent, aType);
}


CTerminalControl *CPuttyAppView::Terminal() {
    return iTerminal;
}


void CPuttyAppView::SetFontL(TBool aLargeFont) {

    // Set font to the terminal control
    iLargeFont = aLargeFont;
    iTerminal->SetFontL(iLargeFont);

    ResizeTerminal();
}


void CPuttyAppView::ResizeTerminal() {

    if ( !iTerminal ) {
        return;
    }
    
    // Determine the terminal window size
    TInt fontWidth = KSmallFontWidth;
    TInt fontHeight = KSmallFontHeight;
    if ( iLargeFont ) {
        fontWidth = KLargeFontWidth;
        fontHeight = KLargeFontHeight;
    }

    TInt termWidth = Rect().Width();
    TInt termHeight = Rect().Height();

    if ( (!iFullScreen) && (!iLargeFont) ) {
        // Small font and non-fullscreen: use default size
        if ( termWidth > (KDefaultTermWidth * fontWidth) ) {
            termWidth = KDefaultTermWidth * fontWidth;
        }
        if ( termHeight > (KDefaultTermHeight * fontHeight) ) {
            termHeight = KDefaultTermHeight * fontHeight;
        }
        
    }

    // Round size down to the largest possible terminal that contains whole
    // characters
    termWidth = fontWidth * (termWidth / fontWidth);
    termHeight = fontHeight * (termHeight / fontHeight);
    assert((termWidth > 0) && (termHeight > 0));

    // Set terminal size and position
    TInt termX = Rect().iTl.iX + (Rect().Width() - termWidth) / 2;
    TInt termY = Rect().iTl.iY + (Rect().Height() - termHeight) / 2;
    iTermRect = TRect(TPoint(termX, termY), TSize(termWidth, termHeight));
    iTerminal->SetRect(iTermRect);
    DrawDeferred();
}


void CPuttyAppView::SetFullScreenL(TBool aFullScreen) {

    CEikonEnv *eikonEnv = CEikonEnv::Static();

    iFullScreen = aFullScreen;
    
    if ( iFullScreen ) {
        // Hide indicator bar and CBA:
        eikonEnv->AppUiFactory()->StatusPane()->MakeVisible(EFalse);
        eikonEnv->AppUiFactory()->ToolBar()->MakeVisible(EFalse);

        // Set view to cover full screen
        SetRect(iAppUi->ClientRect());
        
    } else {
        // Show indicator bar and CBA:
        eikonEnv->AppUiFactory()->StatusPane()->MakeVisible(ETrue);
        eikonEnv->AppUiFactory()->ToolBar()->MakeVisible(ETrue);

        // Set view to cover the remaining area
        SetRect(iAppUi->ClientRect());
    }

    DrawDeferred();
}


// Set default colors
void CPuttyAppView::SetDefaultColors(TRgb aForeground, TRgb aBackground) {
    iDefaultFg = aForeground;
    iDefaultBg = aBackground;
    if ( iTerminal ) {
        iTerminal->SetDefaultColors(aForeground, aBackground);
    }
    DrawDeferred();
}


void CPuttyAppView::SizeChanged() {
    ResizeTerminal();
}


TCoeInputCapabilities CPuttyAppView::InputCapabilities() const {
    return TCoeInputCapabilities(TCoeInputCapabilities::EAllText |
                                 TCoeInputCapabilities::ENavigation);
}
