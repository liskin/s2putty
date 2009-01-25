/*    s2font.h
 *
 * Text rendering class for s2font bitmap fonts
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __S2FONT_H__
#define __S2FONT_H__

#include <e32base.h>
#include <gdi.h>


class CFbsBitmap;


/**
 * A text rendering class for s2font bitmap fonts. CS2Font loads fonts from
 * files, manages them, and renders text to bitmaps.
 */
class CS2Font : public CBase {

public:
    /** 
     * Factory method, creates a new CS2Font object from a font file.
     * 
     * @param aFileName Font file name
     * 
     * @return New CS2Font object
     */
    static CS2Font *NewL(const TDesC &aFileName);

    /** 
     * Destructor.
     */
    ~CS2Font();

    /** 
     * Gets the font size in pixels.
     * 
     * @return Font size in pixels.
     */
    TSize FontSize() const;


    /** 
     * Renders text to a bitmap. The text must fit entirely into the
     * bitmap, or the method will panic. Currently only EColor4K and
     * EColor64K bitmaps are supported, and the method will leave with
     * KErrNotSupported if the bitmap format is something else.
     * 
     * @param aBitmap The bitmap to render to.
     * @param aText The text to render
     * @param aXOffset X-coordinate offset in the bitmap
     * @param aYOffset Y-coordinate offset in the bitmap
     * @param aFgColor Text foreground color
     * @param aBgColor Text background color
     */
    void RenderText(CFbsBitmap &aBitmap, const TDesC &aText, TInt aXOffset,
                    TInt aYOffset, TRgb aFgColor, TRgb aBgColor) const;
    
    /** 
     * Renders text to a EColor4K bitmap that is already locked. RenderTextL()
     * uses this method internally, and external users can use it as an
     * optimization.
     * 
     * @param aBitmap The bitmap to render to.
     * @param aText The text to render
     * @param aXOffset X-coordinate offset in the bitmap
     * @param aYOffset Y-coordinate offset in the bitmap
     * @param aFgColor Text foreground color
     * @param aBgColor Text background color
     */
    void RenderTextLockedColor4K(CFbsBitmap &aBitmap, const TDesC &aText,
                                 TInt aXOffset, TInt aYOffset, TRgb aFgColor,
                                 TRgb aBgColor) const;

    /** 
     * Renders text to a EColor64K bitmap that is already locked. RenderTextL()
     * uses this method internally, and external users can use it as an
     * optimization.
     * 
     * @param aBitmap The bitmap to render to.
     * @param aText The text to render
     * @param aXOffset X-coordinate offset in the bitmap
     * @param aYOffset Y-coordinate offset in the bitmap
     * @param aFgColor Text foreground color
     * @param aBgColor Text background color
     */
    void RenderTextLockedColor64K(CFbsBitmap &aBitmap, const TDesC &aText,
                                  TInt aXOffset, TInt aYOffset, TRgb aFgColor,
                                  TRgb aBgColor) const;

private:
    /** 
     * Constructor
     */
    CS2Font();

    /** 
     * Second-phase constructor.
     * 
     * @param aFileName Font file name
     */
    void ConstructL(const TDesC &aFileName);


    /** 
     * Renders text to a 16-bpp bitmap that is already locked. This method
     * works for both EColor4K and EColor64K bitmaps, the only difference
     * is the data to use for aFgData and aBgData. Use TRgb::Color4K() for
     * EColor4K bitmaps and TRgb::Color64K() for EColor64K ones.
     * 
     * @param aBitmap The bitmap to render to.
     * @param aText The text to render
     * @param aXOffset X-coordinate offset in the bitmap
     * @param aYOffset Y-coordinate offset in the bitmap
     * @param aFgData Foreground pixel data
     * @param aBgData Background pixel data
     */
    void RenderTextLocked16bpp(CFbsBitmap &aBitmap, const TDesC &aText,
                               TInt aXOffset, TInt aYOffset,
                               TUint16 aFgData, TUint16 aBgData) const;



private:
    /* Font data pointers for each 256-character block in UCS-2. */
    TUint8 **iFontPtr[256];

    TUint8 *iFontData; /* The real font data */
    TUint8 *iUnknownCharData; /* Font data for unknown characters */
    TInt iNumChars;
    TInt iCharWidth;
    TInt iCharHeight;
    TInt iByteWidth;
};


#endif
