/*    terminalcontrol.cpp
 *
 * A terminal UI control class
 *
 * Copyright 2002-2005 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <eikenv.h>
#include <e32svr.h>
#include "terminalcontrol.h"
#ifdef PUTTY_S60
#include "termfepext1.h"
#include <aknedsts.h>
#endif
#include "logfile.h"

_LIT(KPanic, "TerminalControl");

const TUint KNewline = 0x2028; // unicode forced line break

_LIT(KTerminalControl, "tc");

#ifdef LOGFILE_ENABLED
static void AssertFail(TInt aLine) {
    LFPRINT((_L("ASSERT FAIL: terminalcontrol.cpp %d"), aLine));
    User::Panic(KTerminalControl, aLine);
}
#define assert(x) __ASSERT_ALWAYS(x, AssertFail(__LINE__))
#else
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KTerminalControl, __LINE__))
#endif

#define TRACE LFPRINT((_L("terminalcontrol.cpp %d"), __LINE__))


// Cursor and selection color XOR bit masks
// 0x00bbggrr
#define KCursorFgXor TRgb(0x00ffffff)
#define KCursorBgXor TRgb(0x00ffffff)
#define KSelectionFgXor TRgb(0x0000ffff)
#define KSelectionBgXor TRgb(0x0000ffff)
#define KFepEditFgXor TRgb(0x00ffffff)
#define KFepEditBgXor TRgb(0x00ffffff)


CTerminalControl::CTerminalControl(MTerminalObserver &aObserver)
    : iObserver(aObserver) {
    iGrayed = EFalse;
    iCursorX = -1;
    iCursorY = -1;
    iDefaultFg = KRgbBlack;
    iDefaultBg = KRgbWhite;
}


CTerminalControl::~CTerminalControl() {

    delete [] iChars;
    delete [] iAttributes;
#ifdef PUTTY_S60
    delete iFepExt1;
#endif
}


void CTerminalControl::ConstructL(const TRect &aRect,
                                  RWindow &aContainerWindow) {

    SetContainerWindowL(aContainerWindow);
    SetRect(aRect);
    Resize();
    AllocateBuffersL();
    Clear();
#ifdef PUTTY_S60
    iFepExt1 = CTermFepExt1::NewL(*this);
#endif
}


void CTerminalControl::SetGrayed(TBool aGrayed) {
    iGrayed = aGrayed;
    DrawDeferred();
}


// Resizes the terminal in response to size or font change
void CTerminalControl::Resize() {

    TRect rect = Rect();
    TInt width = rect.iBr.iX - rect.iTl.iX;
    TInt height = rect.iBr.iY - rect.iTl.iY;

    // Determine new size in characters
    assert((iFontWidth > 0) && (iFontHeight > 0));
    assert((width >= iFontWidth) && (height >= iFontHeight));
    TInt newWidth = width / iFontWidth;
    TInt newHeight = height / iFontHeight;

    // If the size has actually changed, notify the observer, realllocate
    // buffers and make sure the cursor is still within the terminal.
    if ( (newWidth != iCharWidth) || (newHeight != iCharHeight) ) {
        iCharWidth = newWidth;
        iCharHeight = newHeight;
        TRAPD(error, AllocateBuffersL());
        iObserver.TerminalSizeChanged(iCharWidth, iCharHeight);
        if ( error != KErrNone ) {
            User::Panic(KPanic, error);
        }
    
        if ( iCursorX >= iCharWidth ) {
            iCursorX = iCharWidth - 1;
        }
        if ( iCursorY >= iCharHeight ) {
            iCursorY = iCharHeight - 1;
        }
        if ( iSelectX >= iCharWidth ) {
            iSelectX = iCharWidth - 1;
        }
        if ( iSelectY >= iCharHeight ) {
            iSelectY = iCharHeight - 1;
        }
        if ( iMarkX >= iCharWidth ) {
            iMarkX = iCharWidth - 1;
        }
        if ( iMarkY >= iCharHeight ) {
            iMarkY = iCharHeight - 1;
        }
        if ( iFepEditX >= 0 ) {
            if ( iFepEditX >= iCharWidth ) {
                iFepEditX = iCharWidth - 1;
            }
            if ( (iFepEditX+iFepEditDisplayLen) > iCharWidth ) {
                iFepEditX = iCharWidth - iFepEditDisplayLen;
                if ( iFepEditX < 0 ) {
                    iFepEditX = 0;
                    iFepEditDisplayLen = iCharWidth;
                }
            }
        }
        if ( iFepEditY >= iCharHeight ) {
            iFepEditY = iCharHeight - 1;
        }
    }
}


// (re)allocate character and attribute buffers
void CTerminalControl::AllocateBuffersL() {
    delete [] iChars;
    iChars = NULL;
    delete [] iAttributes;
    iAttributes = NULL;

    iChars = new (ELeave) TText[iCharWidth*iCharHeight];
    iAttributes = new (ELeave) TTerminalAttribute[iCharWidth*iCharHeight];

    Clear();
}


// Clear the buffers
void CTerminalControl::Clear() {
    TText *c = iChars;
    TTerminalAttribute *a = iAttributes;
    TInt num = iCharWidth * iCharHeight;
    while ( num-- ) {
        *c++ = ' ';
        a->iFgColor = iDefaultFg;
        a->iBgColor = iDefaultBg;
        a->iBold = EFalse;
        a->iUnderline = EFalse;
        a++;
    }
}


// Draws text on the terminal window. The coordinates are zero-based
// character coordinates inside the terminal.
void CTerminalControl::DrawText(TInt aX, TInt aY, const TDesC &aText,
                                TBool aBold, TBool aUnderline,
                                TRgb aForeground, TRgb aBackground) {
    // Don't draw text while grayed out
    if ( iGrayed ) {
        return;
    }

    // Check that we are at least partially on screen
    if ( (aX < 0) || (aY < 0) || (aX >= iCharWidth) || (aY >= iCharHeight) ) {
        return;
    }

    // Write the text to our character and attribute buffers
    TInt numChars = aText.Length();
    if ( numChars > (iCharWidth - aX) ) {
        numChars = iCharWidth - aX;
    }
    TText *c = &iChars[aX + aY*iCharWidth];
    TTerminalAttribute *a = &iAttributes[aX + aY * iCharWidth];
    for ( TInt i = 0; i < numChars; i++ ) {
        *c++ = aText[i];
        a->iFgColor = aForeground;
        a->iBgColor = aBackground;
        a->iBold = aBold;
        a->iUnderline = aUnderline;
        a++;
    }
    
    // Actually draw the text on screen
    UpdateDisplay(aX, aY, numChars);
}


// Set cursor position
void CTerminalControl::SetCursor(TInt aX, TInt aY) {

    // Remember old position, set new
    TInt oldCursorX = iCursorX;
    TInt oldCursorY = iCursorY;
    iCursorX = aX;
    iCursorY = aY;

    if ( iGrayed ) {
        return;
    }

    // Redraw the affected character cells if they are inside the terminal
    if ( (oldCursorX >= 0) && (oldCursorX < iCharWidth) &&
         (oldCursorY >= 0) && (oldCursorY < iCharHeight) ) {
        UpdateDisplay(oldCursorX, oldCursorY, 1);
    }

    if ( (iCursorX >= 0) && (iCursorX < iCharWidth) &&
         (iCursorY >= 0) && (iCursorY < iCharHeight) ) {
        UpdateDisplay(iCursorX, iCursorY, 1);
    }
}


// Set modifiers for next key event
void CTerminalControl::SetNextKeyModifiers(TUint aModifiers) {
    iNextKeyModifiers = aModifiers;
}


// Set select mode
void CTerminalControl::SetSelectMode(TBool aSelectMode) {
    
    if ( iHaveSelection ) {
        RemoveMark();
    }
    TBool oldSelectMode = iSelectMode;
    iSelectMode = aSelectMode;
    if ( oldSelectMode ) {
        UpdateDisplay(iSelectX, iSelectY, 1);
    }
    
    iSelectX = iCursorX;
    iSelectY = iCursorY;
    if ( iSelectX < 0 ) {
        iSelectX = 0;
    }
    if ( iSelectX >= iCharWidth ) {
        iSelectX = iCharWidth - 1;
    }
    if ( iSelectY < 0 ) {
        iSelectY = 0;
    }
    if ( iSelectY >= iCharHeight ) {
        iSelectY = iCharHeight - 1;
    }
    UpdateDisplay(iSelectX, iSelectY, 1);
}


// Set mark
void CTerminalControl::SetMark() {
    if ( !iSelectMode ) {
        return;
    }
    if ( iHaveSelection ) {
        RemoveMark();
    }
    iHaveSelection = ETrue;
    iMarkX = iSelectX;
    iMarkY = iSelectY;
    UpdateDisplay(iMarkX, iMarkY, 1);
}


// Remove selection
void CTerminalControl::RemoveMark() {
    iHaveSelection = EFalse;
    
    // Update the area covered by the selection
    TInt x1, y1, x2, y2;
    GetSelectionInScanOrder(x1, y1, x2, y2);
    if ( y1 == y2 ) {
        if ( x2 > x1 ) {
            UpdateDisplay(x1, y1, x2-x1);
        }
    } else {
        UpdateDisplay(x1, y1, iCharWidth-x1);
        for ( TInt y = y1; y < y2; y++ ) {
            UpdateDisplay(0, y, iCharWidth);
        }
        if ( x2 > 0 ) {
            UpdateDisplay(0, y2, x2);
        }
    }
    
    if ( iSelectX >= iCharWidth ) {
        iSelectX = iCharWidth - 1;
    }

    UpdateDisplay(iSelectX, iSelectY, 1);
}


// Copy current selection
const HBufC *CTerminalControl::CopySelectionLC() {

    if ( !iHaveSelection ) {
        return HBufC::NewLC(0);
    }

    TInt x1, y1, x2, y2;
    GetSelectionInScanOrder(x1, y1, x2, y2);

    // Calculate a (fairly accurate) upper bound on the output text size.
    // The final text size may be less as the algorithm removes trailing
    // whitespace from lines.
    TInt maxTextLen = 0;
    if ( y1 == y2 ) {
        maxTextLen = x2 - x1;
        assert(maxTextLen >= 0);
    } else {
        assert(y2 > y1);
        // Note that we need to add one character per linefeed
        maxTextLen = (iCharWidth - x1 + 1 +
                      (iCharWidth+1) * (y2 - y1 - 1) +
                      x2);
    }

    // Allocate the buffer
    HBufC *buf = HBufC::NewLC(maxTextLen);    
    if ( maxTextLen == 0 ) {
        return buf;
    }
    TPtr16 des = buf->Des();

    // Copy text to the buffer, appending a newline after each line and
    // removing trailing whitespace. One exception: If the selection is on
    // a single line and contains nothing but spaces, we'll copy it as such,
    // since we can only assume the user wanted to copy spaces.
    if ( y1 == y2 ) {
        // Single-line selection
        TInt len = x2-x1;
        TText *chars = &iChars[y1*iCharWidth + x1];
        while ( (chars[len-1] == ' ') && (len > 0) ) {
            len--;
        }
        if ( len == 0 ) {
            // Nothing but spaces -- let's copy them all
            len = x2-x1;
        }
        des.Append(TPtrC(chars, len));
    } else {

        // Multi-line selection.

        // Start of the selection to the end of the first line
        TInt len = iCharWidth - x1;
        TText *chars = &iChars[y1*iCharWidth + x1];
        while ( (chars[len-1] == ' ') && (len > 0) ) {
            len--;
        }
        des.Append(TPtrC(chars, len));
        des.Append(KNewline);

        // Full lines
        for ( TInt y = (y1+1); y < y2; y++ ) {
            len = iCharWidth;
            chars = &iChars[y*iCharWidth];
            while ( (chars[len-1] == ' ') && (len > 0) ) {
                len--;
            }
            des.Append(TPtrC(chars, len));
            des.Append(KNewline);
        }

        // Start of the last line to the end of the selection
        len = x2;
        chars = &iChars[y2*iCharWidth];
        while ( (chars[len-1] == ' ') && (len > 0) ) {
            len--;
        }
        des.Append(TPtrC(chars, len));
    }
    
    return buf;
}


// Gets the current selection in raster scan order
void CTerminalControl::GetSelectionInScanOrder(
    TInt &aStartX, TInt &aStartY, TInt &aEndX, TInt &aEndY) const {
    
    // Flip mark and selection cursor coordinates as necessary so that we
    // have the selection in raster scan order
    aStartY = iSelectY;
    aStartX = iSelectX;
    if ( iMarkY < aStartY ) {
        aStartY = iMarkY;
        aStartX = iMarkX;
    }
    aEndY = iMarkY;
    aEndX = iMarkX;
    if ( iSelectY > aEndY ) {
        aEndY = iSelectY;
        aEndX = iSelectX;
    }
    if ( (aEndY == aStartY) && (aStartX > aEndX) ) {
        TInt tmp = aEndX;
        aEndX = aStartX;
        aStartX = tmp;
    }
}


// Set default colors
void CTerminalControl::SetDefaultColors(TRgb aForeground, TRgb aBackground) {
    iDefaultFg = aForeground;
    iDefaultBg = aBackground;
    Clear();
    DrawDeferred();
}


// Calculate the final colors for a character cell
void CTerminalControl::GetFinalColors(TInt aX, TInt aY, TRgb &aForeground,
                                      TRgb &aBackground) const {

    assert((aX >= 0) && (aX < iCharWidth));
    assert((aY >= 0) && (aY < iCharHeight));

    // Basic character colors
    TTerminalAttribute *attribs = &iAttributes[aY * iCharWidth + aX];
    TRgb fg = attribs->iFgColor;
    TRgb bg = attribs->iBgColor;

    // FEP editor
    if ( iFepEditActive && (aY == iFepEditY) &&
         (aX >= iFepEditX) && (aX < (iFepEditX + iFepEditDisplayLen)) ) {
        // Note that we use the FEP edit start position as a reference for the
        // whole FEP editor display. This way the color stays constant
        // regardless of the underlying content
        attribs = &iAttributes[iFepEditOrigY * iCharWidth + iFepEditOrigX];

        // Selection and cursor are not visible in the FEP editor
        aForeground = attribs->iFgColor ^ KFepEditFgXor;
        aBackground = attribs->iBgColor ^ KFepEditBgXor;
        return;
    }

    // Selection
    if ( iHaveSelection ) {
        
        TInt selStartX, firstSelLine, selEndX, lastSelLine;
        GetSelectionInScanOrder(selStartX, firstSelLine, selEndX, lastSelLine);
        TBool inSelection = EFalse;

        if ( aY == firstSelLine ) {
            // On the first selected line
            if ( aY == lastSelLine ) {
                // This line is both the first and the last selected line
                if ( (aX >= selStartX) && (aX < selEndX) ) {
                    inSelection = ETrue;
                }
            } else {
                if ( aX >= selStartX ) {
                    inSelection = ETrue;
                }
            }
        } else if ( aY == lastSelLine ) {
            // Last selected line
            if ( aX < selEndX ) {
                inSelection = ETrue;
            }
        } else if ( (aY > firstSelLine) && (aY < lastSelLine) ) {
            // On a fully selected line
            inSelection = ETrue;
        }

        if ( inSelection ) {
            fg = fg ^ KSelectionFgXor;
            bg = bg ^ KSelectionBgXor;
        }
    }

    // Cursor
    if ( (aX == iCursorX) && (aY == iCursorY) ) {
        fg = fg ^ KCursorFgXor;
        bg = bg ^ KCursorBgXor;
    }

    aForeground = fg;
    aBackground = bg;
}


// Get the final character for a character cell
TText CTerminalControl::FinalChar(TInt aX, TInt aY) const {

    assert((aX >= 0) && (aX < iCharWidth));
    assert((aY >= 0) && (aY < iCharHeight));
    
    // FEP editor
    if ( iFepEditActive && (aY == iFepEditY) &&
         (aX >= iFepEditX) && (aX < (iFepEditX + iFepEditDisplayLen)) ) {
        TText c = iFepEditBuf[aX - iFepEditX];
        // Map Enter characters or symbols from the the S60 FEP uses to a
        // unicode line break. See DoCommitFepInlineEditL() below for more
        // info.
        if ( (c == 0x21b2) || (c == 0xe125) ) {
            c = 0x2028;
        }
        return c;
    }

    return iChars[aX + aY * iCharWidth];
}


// Update FEP editor display
void CTerminalControl::UpdateFepEditDisplay() {

    TInt oldX = iFepEditX;
    TInt oldLen = iFepEditDisplayLen;

    if ( (iFepEditOrigX == -1) || (iFepEditOrigY == -1) ) {
        return;
    }

    // Determine new FEP editor display position and length
    iFepEditY = iFepEditOrigY;
    TInt len = iFepEditBuf.Length();
    if ( iFepEditOrigX + len > iCharWidth ) {
        iFepEditX = iCharWidth - len;
        if ( iFepEditX < 0 ) {
            iFepEditX = 0;
            iFepEditDisplayLen = iCharWidth;
        } else {
            iFepEditDisplayLen = len;
        }
    } else {
        iFepEditX = iFepEditOrigX;
        iFepEditDisplayLen = len;
    }

    // Update the area covered by both the old and the new FEP edit display
    TInt updateX = iFepEditX;
    TInt updateLen = iFepEditDisplayLen;
    if ( (oldX != -1) && (oldX < updateX) ) {
        updateX = oldX;
    }
    if ( (oldX != -1) && ((oldX + oldLen) > (updateX + updateLen)) ) {
        updateLen = (oldX + oldLen) - updateX;
    }
    UpdateDisplay(updateX, iFepEditY, updateLen);
}


// CCoeControl::OfferKeyEventL()
TKeyResponse CTerminalControl::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                              TEventCode aType) {

    LFPRINT((_L("OfferKeyEvent(%d, %d)"), (TInt)aKeyEvent.iCode, (TInt)aType));

    if ( aType == EEventKey ) {
        TUint code = aKeyEvent.iCode;
        iCtrlDown = EFalse;

        // Handle cursor keys in selection mode
        if ( iSelectMode ) {
            switch ( code ) {
                case EKeyUpArrow:
                    if ( iSelectY > 0 ) {
                        iSelectY--;
                        if ( iHaveSelection ) {
                            // FIXME: Could be optimized
                            UpdateDisplay(0, iSelectY, iCharWidth);
                            UpdateDisplay(0, iSelectY+1, iCharWidth);
                        } else {
                            UpdateDisplay(iSelectX, iSelectY, 1);
                            UpdateDisplay(iSelectX, iSelectY+1, 1);
                        }
                    }
                    break;

                case EKeyDownArrow:
                    if ( iSelectY < (iCharHeight-1) ) {
                        iSelectY++;
                        if ( iHaveSelection ) {
                            // FIXME: Could be optimized
                            UpdateDisplay(0, iSelectY-1, iCharWidth);
                            UpdateDisplay(0, iSelectY, iCharWidth);
                        } else {
                            UpdateDisplay(iSelectX, iSelectY-1, 1);
                            UpdateDisplay(iSelectX, iSelectY, 1);
                        }
                    }
                    break;

                case EKeyLeftArrow:
                    if ( iSelectX > 0 ) {
                        iSelectX--;
                        if ( iSelectX < (iCharWidth-1) ) {
                            UpdateDisplay(iSelectX, iSelectY, 2);
                        } else {
                            UpdateDisplay(iSelectX, iSelectY, 1);
                        }
                    }
                    break;

                case EKeyRightArrow:
                    if ( iHaveSelection ) {
                        // Note that when there is a selection we'll let the
                        // mark cursor go past the right-hand side of the
                        // terminal by one character. This lets the user select
                        // a line all the way to its end.
                        if ( iSelectX < iCharWidth ) {
                            iSelectX++;
                            if ( iSelectX < iCharWidth ) {
                                UpdateDisplay(iSelectX-1, iSelectY, 2);
                            } else {
                                UpdateDisplay(iSelectX-1, iSelectY, 1);
                            }
                        }
                    } else {
                        if ( iSelectX < (iCharWidth-1) ) {
                            iSelectX++;
                            UpdateDisplay(iSelectX-1, iSelectY, 2);
                        }
                    }
                    break;

                default:
                    // Pass other keypresses to the observer
                    // FIXME: Is this a good idea after all?
                    iObserver.KeyPressed((TKeyCode)code,
                                         (aKeyEvent.iModifiers |
                                          iNextKeyModifiers));
                    iNextKeyModifiers = 0;
                    break;
            }
            
        } else {
            // Normal mode, pass keypress to the observer
            iObserver.KeyPressed((TKeyCode)code,
                                 aKeyEvent.iModifiers | iNextKeyModifiers);
            iNextKeyModifiers = 0;
            return EKeyWasConsumed;
        }
        
    } else if ( (aType == EEventKeyDown) && (aKeyEvent.iCode == EKeyNull) &&
                ((aKeyEvent.iModifiers & EModifierCtrl) != 0) ) {
        iCtrlDown = ETrue;
    } else if ( iCtrlDown &&
                (aType == EEventKeyUp) &&
                (aKeyEvent.iCode == EKeyNull) &&
                ((aKeyEvent.iModifiers & EModifierCtrl) == 0) ) {
        // Ctrl up/down without other keys -- add Ctrl modifier to the next
        // key. This makes Ctrl behave like a prefix key similar to Shift,
        // which should improve usability in general and enables Ctrl+key
        // input on all keys on an E61.
        iCtrlDown = EFalse;
        iNextKeyModifiers |= EModifierCtrl;
        return EKeyWasConsumed;
    } else {
        iCtrlDown = EFalse;
    }    

    return EKeyWasNotConsumed;
}


// CCoeControl::SizeChanged()
void CTerminalControl::SizeChanged() {
    Resize();
}


// CCoeControl::InputCapabilities()
TCoeInputCapabilities CTerminalControl::InputCapabilities() const {
    return TCoeInputCapabilities(TCoeInputCapabilities::EAllText |
                                 TCoeInputCapabilities::ENavigation,
                                 const_cast<CTerminalControl*>(this),
                                 NULL);
}


#ifdef PUTTY_S90
// Notify CEikonEnv of pointer events. This ensures the text input window
// pops up when the user taps the terminal control.
void CTerminalControl::HandlePointerEventL(const TPointerEvent& aPointerEvent) {
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down ) {
        iEikonEnv->InformFepOfPenDownEvent();
    }
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up ) {
		iEikonEnv->InformFepOfPenUpEvent();
    }
    CCoeControl::HandlePointerEventL(aPointerEvent);
}
#endif


// MCoeFepAwareTextEditor::StartFepInlineEditL()
void CTerminalControl::StartFepInlineEditL(
    const TDesC &aInitialInlineText,
    TInt aPositionOfInsertionPointInInlineText,
    TBool aCursorVisibility,
    const MFormCustomDraw * /*aCustomDraw*/,
    MFepInlineTextFormatRetriever & /*aInlineTextFormatRetriever*/,
    MFepPointerEventHandlerDuringInlineEdit & /*aPointerEventHandlerDuringInlineEdit*/) {

    LFPRINT((_L("StartFepInlineEditL()")));

    iFepEditActive = ETrue;
    iFepCursorVisible = aCursorVisibility;

    if ( aInitialInlineText.Length() <= iFepEditBuf.MaxLength() ) {
        iFepEditBuf = aInitialInlineText;
    } else {
        iFepEditBuf = aInitialInlineText.Left(iFepEditBuf.MaxLength());        
    }
    
    iFepCursorPos = aPositionOfInsertionPointInInlineText;
    if ( iFepCursorPos >= iFepEditBuf.Length() ) {
        iFepCursorPos = iFepEditBuf.Length() - 1;
    }    

    // Determine FEP editor display location
    iFepEditOrigX = iCursorX;
    iFepEditOrigY = iCursorY;
    iFepEditX = -1;
    iFepEditY = -1;
    iFepEditDisplayLen = 0;
    UpdateFepEditDisplay();
    TRACE;
}


// MCoeFepAwareTextEditor::UpdateFepInlineTextL()
void CTerminalControl::UpdateFepInlineTextL(
    const TDesC& aNewInlineText,
    TInt aPositionOfInsertionPointInInlineText) {

    if ( !iFepEditActive ) {
        // The S60 T9 FEP can send spurious text updates even when an edit
        // is not active when switching from numeric to alpha input.
        return;
    }

#ifdef LOGFILE_ENABLED
    TBuf<128> buf;
    buf = _L("UpdateFepInlineTextL({");
    for ( TInt i = 0; i < aNewInlineText.Length(); i++ ) {
        buf.AppendFormat(_L("0x%04x"), (TInt) aNewInlineText[i]);
        if ( i < (aNewInlineText.Length()-1) ) {
            buf.Append(_L(", "));
        }
    }
    buf.AppendFormat(_L("}, %d)"), aPositionOfInsertionPointInInlineText);
    LFPRINT((buf));
#endif
    
    if ( aNewInlineText.Length() <= iFepEditBuf.MaxLength() ) {
        iFepEditBuf = aNewInlineText;
    } else {
        iFepEditBuf = aNewInlineText.Left(iFepEditBuf.MaxLength());
    }
    
    iFepCursorPos = aPositionOfInsertionPointInInlineText;
    if ( iFepCursorPos >= iFepEditBuf.Length() ) {
        iFepCursorPos = iFepEditBuf.Length() - 1;
    }

    TRACE;
    UpdateFepEditDisplay();
    TRACE;
}


// MCoeFepAwareTextEditor::UpdateFepInlineTextL()
void CTerminalControl::SetInlineEditingCursorVisibilityL(
    TBool aCursorVisibility) {
    iFepCursorVisible = aCursorVisibility;
    LFPRINT((_L("SetInlineEditingCursorVisibilityL(%d)"), (TInt) aCursorVisibility));
}


// MCoeFepAwareTextEditor::CommitFepInlineEditL()
void CTerminalControl::CommitFepInlineEditL(CCoeEnv& /*aConeEnvironment*/) {    
    LFPRINT((_L("CommitFepInlineEditL()")));
}


// MCoeFepAwareTextEditor::CancelFepInlineEdit()
void CTerminalControl::CancelFepInlineEdit() {
    
    iFepEditActive = EFalse;
    LFPRINT((_L("CancelFepInlineEdit()")));
    if ( (iFepEditX != -1) && (iFepEditY != -1) ) {
        TRACE;
        UpdateDisplay(iFepEditX, iFepEditY, iFepEditDisplayLen);
        TRACE;
    }
}


// MCoeFepAwareTextEditor::DocumentLengthForFep()
TInt CTerminalControl::DocumentLengthForFep() const {
    LFPRINT((_L("DocumentLengthForFep()")));
    if ( iFepEditActive ) {
        LFPRINT((_L(" - returning %d"), iFepEditBuf.Length()));
        return iFepEditBuf.Length();
    } else {
        LFPRINT((_L(" - returning 0 (not active)"), iFepEditBuf.Length()));
        return 0;
    }
}


// MCoeFepAwareTextEditor::DocumentMaximumLengthForFep()
TInt CTerminalControl::DocumentMaximumLengthForFep() const {
    LFPRINT((_L("DocumentMaximumLengthForFep(), returning %d"), iFepEditBuf.MaxLength()));
    return iFepEditBuf.MaxLength();
}


// MCoeFepAwareTextEditor::SetCursorSelectionForFepL()
void CTerminalControl::SetCursorSelectionForFepL(
    const TCursorSelection & /*aCursorSelection*/) {
    LFPRINT((_L("SetCursorSelectionForFepL()")));
}


// MCoeFepAwareTextEditor::GetCursorSelectionForFep()
void CTerminalControl::GetCursorSelectionForFep(
    TCursorSelection& aCursorSelection) const {
    LFPRINT((_L("GetCursorSelectionForFepL()")));
    if ( iFepEditActive ) {
        aCursorSelection.iAnchorPos = iFepCursorPos;
        aCursorSelection.iCursorPos = iFepCursorPos;
    } else {
        aCursorSelection.iAnchorPos = 0;
        aCursorSelection.iCursorPos = 0;
    }
}


// MCoeFepAwareTextEditor::GetEditorContentForFep()
void CTerminalControl::GetEditorContentForFep(
    TDes &aEditorContent, TInt aDocumentPosition,
    TInt aLengthToRetrieve) const {

    LFPRINT((_L("GetEditorContentForFepL(des, %d, %d)"), aDocumentPosition, aLengthToRetrieve));
    if ( iFepEditActive ) {
        if ( (aDocumentPosition == -1) && (aLengthToRetrieve == 1) ) {
            aEditorContent = _L(" ");
            LFPRINT((_L(" - Returning \" \"")));
            return;
        }

        if ( aDocumentPosition < 0 ) {
            aLengthToRetrieve += aDocumentPosition;
            aDocumentPosition = 0;
        }
        if ( aLengthToRetrieve <= 0 ) {
            aLengthToRetrieve = 0;
            aEditorContent.Zero();
            LFPRINT((_L(" - Returning empty")));
            return;
        }
        TInt bufLen = iFepEditBuf.Length();
        if ( aDocumentPosition >= bufLen ) {
            aDocumentPosition = bufLen - 1;
        }
        if ( (aDocumentPosition + aLengthToRetrieve) > bufLen ) {
            aLengthToRetrieve = bufLen - aDocumentPosition;
        }
        aEditorContent = iFepEditBuf.Mid(aDocumentPosition, aLengthToRetrieve);
#ifdef LOGFILE_ENABLED
        TBuf<128> buf;
        buf = _L(" - Returning {");
        for ( TInt i = 0; i < aEditorContent.Length(); i++ ) {
            buf.AppendFormat(_L("0x%04x"), (TInt) aEditorContent[i]);
            if ( i < (aEditorContent.Length()-1) ) {
                buf.Append(_L(", "));
            }
        }
        buf.Append(_L("}"));
        LFPRINT((buf));
#endif
    
    } else {
        LFPRINT((_L(" - Not active, returning empty")));
        aEditorContent.Zero();
    }
}


// MCoeFepAwareTextEditor::GetFormatForFep()
void CTerminalControl::GetFormatForFep(TCharFormat& /*aFormat*/,
                                       TInt /*aDocumentPosition*/) const {    
    LFPRINT((_L("GetFormatForFep()")));
}


// MCoeFepAwareTextEditor::GetScreenCoordinatesForFepL()
void CTerminalControl::GetScreenCoordinatesForFepL(
    TPoint& aLeftSideOfBaseLine,
    TInt& aHeight, TInt& aAscent,
    TInt /*aDocumentPosition*/) const {

    LFPRINT((_L("GetScreenCoordinatesForFepL()")));
    aLeftSideOfBaseLine = TPoint(0,0);
    aHeight = 0;
    aAscent = 0;    
}


// MCoeFepAwareTextEditor::DoCommitFepInlineEditL()
void CTerminalControl::DoCommitFepInlineEditL() {

    LFPRINT((_L("DoCommitFepInlineEditL()")));
    for ( TInt i = 0; i < iFepEditBuf.Length(); i++ ) {
        TText c = iFepEditBuf[i];
        // S60 1.2 SDK sends Enter from the FEP by sending a 0xe125 character
        // (part of Unicode private use area). Presumably at least some S60
        // v1 phones do that too. Nokia 6630 and probably other S60 v2.x
        // phones send 0x21b2 (Unicode arrow symbol, the same shown in
        // regular editors) in their FEP updates, but cancel the FEP edit
        // and just send an Enter keypress instead (to OfferKeyEventL()).
        // 
        // To be on the safe side, we map both these codes and the Unicode
        // forced line feed to Enter here. FinalChar() maps those to 0x2028,
        // and the concrete terminal control must map that to a valid character
        // in the fonts available.
        if ( (c == 0x21b2) || (c == 0xe125) || (c == 0x2028) ) {
            c = (TText)EKeyEnter;
        }
        LFPRINT((_L(" - Sending 0x%04x"), (TInt) c));
        iObserver.KeyPressed((TKeyCode)c, iNextKeyModifiers);
    }
    iNextKeyModifiers = 0;
    iFepEditActive = EFalse;
    TRACE;
    if ( (iFepEditX != -1) && (iFepEditY != -1) ) {
        TRACE;
        UpdateDisplay(iFepEditX, iFepEditY, iFepEditDisplayLen);
        TRACE;
    }
}


#ifdef PUTTY_S60

// MCoeFepAwareTextEditor::Extension1()
MCoeFepAwareTextEditor_Extension1 *CTerminalControl::Extension1(TBool &aSetToTrue) {
//    LFPRINT((_L("Extension1()")));
    aSetToTrue = ETrue;
    return iFepExt1;
}

#endif
