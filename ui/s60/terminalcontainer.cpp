/*    terminalcontainer.cpp
 *
 * Putty UI container class for the terminal view
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002,2004,2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <eikenv.h>
#include <apgtask.h>
#include <eikspane.h>
#include <eikbtgpc.h>
#include <eikmenub.h>
#include "puttyappui.h"
#include "terminalview.h"
#include "terminalcontainer.h"
#include "puttyui.hrh"
#include "puttyengine.h"
#include "s2font.h"

#include <putty.rsg>
#include <aknnotewrappers.h> 

_LIT(KAssertPanic, "terminalcontainer.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


// Factory method
CTerminalContainer *CTerminalContainer::NewL(const TRect &aRect,
                                             MTerminalObserver *aTerminalObserver,
                                             CTerminalView *aView,
                                             const TDesC &aFontFile) {
    
    CTerminalContainer *self = new (ELeave) CTerminalContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aTerminalObserver, aFontFile);
    CleanupStack::Pop(self);
    return self;
}


// Constructor
CTerminalContainer::CTerminalContainer(CTerminalView *aView) {
    iView = aView;
    iDefaultFg = KRgbBlack;
    iDefaultBg = KRgbWhite;
}


// Desctructor
CTerminalContainer::~CTerminalContainer() {
    delete iTerminal;
    delete iFont;
}


// Second-phase constructor
void CTerminalContainer::ConstructL(const TRect &aRect,
                                    MTerminalObserver *aTerminalObserver,
                                    const TDesC &aFontFile) {
    CreateWindowL();
    iFont = CS2Font::NewL(aFontFile);
    SetRect(aRect);

    GetTerminalRect(iTermRect);
    iTerminal = CTerminalControlS2Font::NewL(*aTerminalObserver, iTermRect,
                                             Window(), *iFont);
    iTerminal->SetMopParent(this);
    iTerminal->SetFocus(ETrue);
}


// Set the font to use
void CTerminalContainer::SetFontL(const TDesC &aFontFile) {

    CS2Font *newFont = CS2Font::NewL(aFontFile);
    CleanupStack::PushL(newFont);    
    iTerminal->SetFontL(*newFont);
    delete iFont;
    iFont = newFont;
    CleanupStack::Pop();
    SizeChanged(); // recalculate terminal rect
}


// Set default colors
void CTerminalContainer::SetDefaultColors(TRgb aForeground, TRgb aBackground) {
    iDefaultFg = aForeground;
    iDefaultBg = aBackground;
    if ( iTerminal ) {
        iTerminal->SetDefaultColors(aForeground, aBackground);
    }
}


void CTerminalContainer::Draw(const TRect & /*aRect*/) const {

    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.SetClippingRect(Rect());

    // Determine terminal window borders
    TRect borderRect = iTermRect;

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


TInt CTerminalContainer::CountComponentControls() const {
    return 1;
}


CCoeControl *CTerminalContainer::ComponentControl(TInt aIndex) const {
    
    switch ( aIndex ) {
        case 0:
            return iTerminal;

        default:
            assert(EFalse);
    }

    return NULL;
}


TKeyResponse CTerminalContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                           TEventCode aType) {

    /*
    // debug help, shows what key was pressed
    CEikonEnv *eenv = CEikonEnv::Static();    
    HBufC *msg = HBufC::NewLC(512);
    TPtr msgp = msg->Des();
    msgp.Format(*eenv->AllocReadResourceLC(R_KEY_EVENT), aKeyEvent.iScanCode, aType);
    CAknInformationNote* dlg = new( ELeave ) CAknInformationNote();
    //dlg->SetTextL( msgp );
    dlg->ExecuteLD(msgp);
    CleanupStack::PopAndDestroy(2); // msg, fmt string
    */

    // Handle a couple of special keys
    if ( aType == EEventKey ) {
        switch ( aKeyEvent.iScanCode ) {
            case EStdKeyDevice3: { // Center of joystick
                // Check if the view wishes to handle this (for copy/paste)
                if ( iView->HandleEnterL() ) {
                    return EKeyWasConsumed;
                } else {                
                    // Send an Enter event to the terminal
                    TKeyEvent event;
                    event.iCode = EKeyEnter;
                    event.iModifiers = 0;
                    event.iRepeats = 0;
                    event.iScanCode = EStdKeyEnter;
                    iTerminal->OfferKeyEventL(event, EEventKey);
                    return EKeyWasConsumed;
                }
            }
                
            case EStdKeyDial:
            case EStdKeyYes: // Green dial button 
                iView->HandleCommandL( EPuttyCmdRepeatLast ); // repeat last command
                return EKeyWasConsumed;
        }
    }

    // Let the terminal handle other keys
    return iTerminal->OfferKeyEventL(aKeyEvent, aType);
}


CTerminalControl &CTerminalContainer::Terminal() {
    return *iTerminal;
}


void CTerminalContainer::GetTerminalRect(TRect &aRect) {
    
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


void CTerminalContainer::SizeChanged() {
    GetTerminalRect(iTermRect);
    if ( iTerminal ) {
        iTerminal->SetRect(iTermRect);
    }
    DrawDeferred();
}

TCoeInputCapabilities CTerminalContainer::InputCapabilities() const {
    return TCoeInputCapabilities(TCoeInputCapabilities::EAllText |
                                 TCoeInputCapabilities::ENavigation);
}
