/*    terminalsystemfont.cpp
 *
 * A terminal UI control using a system font
 *
 * Copyright 2002,2004-2006 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include <eikenv.h>
#include <e32svr.h>
#include "terminalcontrolsystemfont.h"

// Small: 6x8
// Large: 7x14
// The bold version is one pixel wider, and thus unusable!

_LIT(KTerminalFont, "Terminal");
static const TInt KSmallFontHeight = 8;
static const TInt KLargeFontHeight = 14;
static const TInt KDefaultLargeFont = EFalse;

#define KSelectCursorXor TRgb(0x00ffff00)

_LIT(KTerminalControlSystemFont, "terminalcontrolsystemfont.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KTerminalControlSystemFont, __LINE__))


// Factory method
CTerminalControlSystemFont *CTerminalControlSystemFont::NewL(
    MTerminalObserver &aObserver, const TRect &aRect,
    RWindow &aContainerWindow) {

    CTerminalControlSystemFont *self =
        new (ELeave) CTerminalControlSystemFont(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aContainerWindow);
    CleanupStack::Pop(self);
    return self;
}


// Constructor
CTerminalControlSystemFont::CTerminalControlSystemFont(
    MTerminalObserver &aObserver)
    : CTerminalControl(aObserver) {
}


// Second-phase constructor
void CTerminalControlSystemFont::ConstructL(const TRect &aRect,
                                            RWindow &aContainerWindow) {
    // Set the default font
    SetFontL(KDefaultLargeFont);

    CTerminalControl::ConstructL(aRect, aContainerWindow);    
}


// Destructor
CTerminalControlSystemFont::~CTerminalControlSystemFont() {
    
    if ( iFont != NULL ) {
        CEikonEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
        iFont = NULL;
    }
    delete iRowTextBuf;
}


// Set font
void CTerminalControlSystemFont::SetFontL(TBool aLargeFont) {

    TInt fontHeight = KSmallFontHeight;    
    if ( aLargeFont ) {
        fontHeight = KLargeFontHeight;
    }

    // Release old fonts
    if ( iFont != NULL ) {
        CEikonEnv::Static()->ScreenDevice()->ReleaseFont(iFont);
        iFont = NULL;
    }
    
    // Get the font
    CGraphicsDevice *dev = CEikonEnv::Static()->ScreenDevice();    
    TInt twips = dev->VerticalPixelsToTwips(fontHeight);
    TFontSpec spec(KTerminalFont, twips);
    User::LeaveIfError(dev->GetNearestFontInTwips(iFont, spec));

    // Get font info and resize terminal
    iFontHeight = iFont->HeightInPixels();
    iFontWidth = iFont->WidthZeroInPixels();
}


// (re)allocate buffers
void CTerminalControlSystemFont::AllocateBuffersL() {
    
    delete iRowTextBuf;
    iRowTextBuf = NULL;
    iRowTextBuf = HBufC::NewL(iCharWidth);
    
    CTerminalControl::AllocateBuffersL();
}


// Updates one more characters on a line to the display
void CTerminalControlSystemFont::UpdateDisplay(TInt aX, TInt aY,
                                               TInt aLength) {
    
    CWindowGc &gc = SystemGc();
    gc.Activate(Window());
    gc.UseFont(iFont);

    UpdateWithGc(gc, aX, aY, aLength);

    gc.DiscardFont();
    gc.Deactivate();
}


// Updates one more characters on a line to the display
// This method requires a valid graphics context, with the correct font
// selected
void CTerminalControlSystemFont::UpdateWithGc(CGraphicsContext &aGc, TInt aX,
                                              TInt aY, TInt aLength) const {
    assert((aX >= 0) && ((aX + aLength) <= iCharWidth));
    assert((aY >= 0) && (aY < iCharHeight));
    
    // We'll find the longest possible run of identical attributes on
    // each line and draw those at one go
    
    TTerminalAttribute *attribs = &iAttributes[aY * iCharWidth + aX];
    TInt x = 0;

    while ( x < aLength ) {
        // Initial attributes and colors
        TTerminalAttribute &a0 = attribs[x];
        TRgb fg0, bg0;
        GetFinalColors(aX+x, aY, fg0, bg0);

        // Attributes and colors for this character
        TTerminalAttribute *a = &attribs[x];
        TRgb fg, bg;
        GetFinalColors(aX+x, aY, fg, bg);

        TInt num = 0;
        do {
            num++;
            if ( (x+num) < aLength ) {
                a++;
                GetFinalColors(aX+x+num, aY, fg, bg);
                if ( (fg != fg0) || (bg != bg0) ||
                     (a->iBold != a0.iBold) ||
                     (a->iUnderline != a0.iUnderline) ) {
                    break;
                }
            }
        } while ( (x+num) < aLength );

        // Determine the real text to render, including possible FEP
        // in-line editor content
        TPtr text = iRowTextBuf->Des();
        text.Zero();
        for ( TInt i = 0; i < num; i++ ) {
            text.Append(FinalChar(aX+x+i, aY));
        }

        DoDraw(aGc, x+aX, aY, text, a0.iBold, a0.iUnderline, fg0, bg0);
        x += num;
    }
    
}


// Actually draw text on screen. Also draws the selection cursor if it overlaps
// with the text
void CTerminalControlSystemFont::DoDraw(CGraphicsContext &aGc, TInt aX,
                                        TInt aY, const TDesC &aText,
                                        TBool /*aBold*/, TBool aUnderline,
                                        TRgb aForeground,
                                        TRgb aBackground) const {

    TInt x = Rect().iTl.iX;
    TInt y = Rect().iTl.iY;
    
    // We'll draw the text inside a filled box. This should avoid flickering
    // compared to filling first and drawing afterwards

    // Brush is used for background
    aGc.SetBrushColor(aBackground);
    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);

    // Optimization: Draw leading and trailing whitespace as clear rectangles
    // instead of text
    int textLen = aText.Length();

    // Leading whitespace
    TInt skipStart = 0;
    while ( (skipStart < textLen) && (aText[skipStart] == ' ') ) {
        skipStart++;
    }
    if ( skipStart > 0 ) {
        aGc.SetPenStyle(CGraphicsContext::ENullPen);
        aGc.DrawRect(TRect(TPoint(x + aX*iFontWidth, y + aY*iFontHeight),
                           TSize(skipStart*iFontWidth, iFontHeight)));
    }

    // Trailing whitespace
    TInt skipEnd = 0;
    while ( (skipEnd < (textLen-skipStart)) &&
            (aText[textLen-skipEnd-1] == ' ') ) {
        skipEnd++;
    }
    if ( skipEnd > 0 ) {
        aGc.SetPenStyle(CGraphicsContext::ENullPen);
        aGc.DrawRect(TRect(TPoint(x + ((aX + textLen - skipEnd) * iFontWidth),
                                  y + aY*iFontHeight),
                           TSize(skipEnd*iFontWidth, iFontHeight)));
    }

    // And the text

    if ( (skipStart + skipEnd) < textLen ) {
        TInt textLeft = textLen - skipStart - skipEnd;
        // Pen is used for foreground
        aGc.SetPenColor(aForeground);
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);

        // Handle underline. Sadly we cannot use a bold version of the Terminal
        // font to do real bold, since it is one pixel wider than the normal
        // version
        if ( aUnderline ) {
            aGc.SetUnderlineStyle(EUnderlineOn);
        }    

        // Draw the text inside a box
        TInt ascent = iFont->AscentInPixels();
        TRect textRect(x + (aX+skipStart) * iFontWidth, y + aY * iFontHeight,
                       x + ((aX+skipStart) + textLeft) * iFontWidth,
                       y + (aY+1) * iFontHeight);
        aGc.DrawText(aText.Mid(skipStart, textLeft), textRect, ascent);

        // Reset underline attribute
        if ( aUnderline ) {
            aGc.SetUnderlineStyle(EUnderlineOff);
        }
    }

    // Draw the selection cursor if it overlaps with the text.
    // Note that this may flicker if timing is unfortunate!
    if ( iSelectMode && ((iSelectY == aY) &&
                       (iSelectX >= aX) && (iSelectX < (aX + aText.Length()))) ) {
        aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
        aGc.SetDrawMode(CGraphicsContext::EDrawModeXOR);
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);
        aGc.SetPenColor(KSelectCursorXor);
        aGc.DrawRect(TRect(TPoint(x + (iSelectX * iFontWidth),
                                  y + aY*iFontHeight),
                           TSize(iFontWidth, iFontHeight)));
        aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
    }
}


// CCoeControl::Draw()
void CTerminalControlSystemFont::Draw(const TRect & /*aRect*/) const {

    CWindowGc &gc = SystemGc();
    gc.Reset();

    if ( iGrayed ) {
        gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        gc.SetBrushColor(TRgb(0xcccccc));
        gc.SetPenStyle(CGraphicsContext::ENullPen);
        gc.DrawRect(Rect());
    }
    else {
        gc.UseFont(iFont);

        for ( TInt y = 0; y < iCharHeight; y++ ) {
            UpdateWithGc(gc, 0, y, iCharWidth);
        }

        gc.DiscardFont();        
    }
}
