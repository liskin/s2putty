/*    puttyappview.cpp
 *
 * Putty UI View class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <eikenv.h>
#include <apgtask.h>
#include <eikbtgpc.h>
#include <putty.rsg>
#include "puttyappui.h"
#include "puttyappview.h"
#include "puttyui.hrh"
#include "s2font.h"

_LIT(KAssertPanic, "puttyappview.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

_LIT(KFontName, "fixed6x12.s2f");

static const TInt KDefaultTermWidth = 80;
static const TInt KDefaultTermHeight = 24;


CPuttyAppView::CPuttyAppView(MTerminalObserver *anObserver,
                             CPuttyAppUi *aAppUi) {
    iTerminalObserver = anObserver;
    iAppUi = aAppUi;
}

CPuttyAppView::~CPuttyAppView() {
    delete iTerminal;
    delete iFont;
    delete iFontFile;
    delete iFontDir;
}


void CPuttyAppView::ConstructL(const TDesC &aFontDir, const TRect &aRect) {

    // Determine the font file to use
    iFontDir = HBufC::NewL(aFontDir.Length()+1);
    *iFontDir = aFontDir;
    if ( (*iFontDir)[iFontDir->Length()-1] != '\\' ) {
        (iFontDir->Des()).Append('\\');
    }
    iFontFile = HBufC::NewL(KMaxFileName);
    *iFontFile = *iFontDir;
    iFontFile->Des().Append(KFontName);

    CreateWindowL();
    SetRect(aRect);

    // Load font
    iFont = CS2Font::NewL(*iFontFile);
    iFontWidth = iFont->FontSize().iWidth;
    iFontHeight = iFont->FontSize().iHeight;

    // Determine the correct size and position for the terminal. We'll aim
    // at a default size initially, instead of making it as big as possible
    TInt termWidth = Rect().Width();
    TInt termHeight = Rect().Height();
    if ( termWidth > (KDefaultTermWidth * iFontWidth) ) {
        termWidth = KDefaultTermWidth * iFontWidth;
    }
    if ( termHeight > (KDefaultTermHeight * iFontHeight) ) {
        termHeight = KDefaultTermHeight * iFontHeight;
    }
    TInt termX = Rect().iTl.iX + (Rect().Width() - termWidth) / 2;
    TInt termY = Rect().iTl.iY + (Rect().Height() - termHeight) / 2;
    iTermRect = TRect(TPoint(termX, termY), TSize(termWidth, termHeight));

    iTerminal = CTerminalControlS2Font::NewL(*iTerminalObserver, iTermRect,
                                             Window(), *iFont);
    ActivateL();
    iTerminal->SetFocus(ETrue);
}


void CPuttyAppView::Draw(const TRect & /*aRect*/) const {

    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.SetClippingRect(Rect());

    // Determine terminal window borders and draw a rectangle around it
    TRect borderRect = iTermRect;
    borderRect.Grow(1, 1);
    gc.DrawRect(borderRect);

    // Clear everything outside the terminal
    TRegionFix<5> clearReg(Rect());
    clearReg.SubRect(borderRect);
    assert(!clearReg.CheckError());
    const TRect *rects = clearReg.RectangleList();
    TInt numRects = clearReg.Count();
    
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbWhite);
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
    // - If the user presses Chr without other keys,
    //   we'll launch the character selection dialog.
    // - The A key (upper right device key) toggles the Ctrl modifier
    //   for the next character
    if ( (aType == EEventKeyDown) && (aKeyEvent.iCode == EKeyNull) &&
         ((aKeyEvent.iModifiers & EModifierLeftFunc) != 0) ) {
        iChrDown = ETrue;
    } else if ( iChrDown &&
                (aType == EEventKeyUp) &&
                (aKeyEvent.iCode == EKeyNull) &&
                ((aKeyEvent.iModifiers & EModifierLeftFunc) == 0) ) {
        // Chr up/down without other keys
        iChrDown = EFalse;
        iAppUi->HandleCommandL(ECmdSendSpecialCharacter);
        return EKeyWasConsumed;
    } else {
        iChrDown = EFalse;
    }

    if ( (aType == EEventKey) && (aKeyEvent.iCode == EKeyDevice4) ) {
        CEikonEnv *eikonEnv = CEikonEnv::Static();
        if ( iCtrlToggle ) {
            iCtrlToggle = EFalse;
            eikonEnv->InfoMsg(R_STR_CTRL_OFF);
            iTerminal->SetNextKeyModifiers(0);
        } else {
            iCtrlToggle = ETrue;
            eikonEnv->InfoMsg(R_STR_CTRL_ON);
            iTerminal->SetNextKeyModifiers(EModifierCtrl);
        }
        return EKeyWasConsumed;
    } else if ( aType == EEventKey ) {
        iCtrlToggle = EFalse;
    }
    
    return iTerminal->OfferKeyEventL(aKeyEvent, aType);
}


TCoeInputCapabilities CPuttyAppView::InputCapabilities() const {
    return TCoeInputCapabilities(TCoeInputCapabilities::EAllText |
                                 TCoeInputCapabilities::ENavigation);
}


CTerminalControl *CPuttyAppView::Terminal() {
    return iTerminal;
}


void CPuttyAppView::SetFontL(TBool aLargeFont) {

    // FIXME
    iLargeFont = aLargeFont;
#if 0
    // Set font to the terminal control
    iTerminal->SetFontL(iLargeFont);

    ResizeTerminal();
#endif
}


void CPuttyAppView::ResizeTerminal() {

    if ( !iTerminal ) {
        return;
    }
    
    // Determine the terminal window size
    TInt termWidth = Rect().Width();
    TInt termHeight = Rect().Height();

    if ( (!iFullScreen) && (!iLargeFont) ) {
        // Small font and non-fullscreen: use default size
        if ( termWidth > (KDefaultTermWidth * iFontWidth) ) {
            termWidth = KDefaultTermWidth * iFontWidth;
        }
        if ( termHeight > (KDefaultTermHeight * iFontHeight) ) {
            termHeight = KDefaultTermHeight * iFontHeight;
        }
        
    }

    // Round size down to the largest possible terminal that contains whole
    // characters
    termWidth = iFontWidth * (termWidth / iFontWidth);
    termHeight = iFontHeight * (termHeight / iFontHeight);
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
        eikonEnv->AppUiFactory()->ToolBar()->MakeVisible(EFalse);

        // Set view to cover full screen
        SetRect(iAppUi->ClientRect());
        
    } else {
        // Show indicator bar and CBA:
        eikonEnv->AppUiFactory()->ToolBar()->MakeVisible(ETrue);

        // Set view to cover the remaining area
        SetRect(iAppUi->ClientRect());
    }

    DrawDeferred();
}


void CPuttyAppView::SizeChanged() {
    ResizeTerminal();
}
