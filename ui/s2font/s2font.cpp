/*    s2font.cpp
 *
 * Text rendering class for s2font bitmap fonts
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <fbs.h>
#include <f32file.h>
#include <coemain.h>
#include "s2font.h"

_LIT(KPanic, "S2Font");
const TInt KPanicUnsupportedBitmapFormat = 1;
const TInt KPanicOutOfBitmapBounds = 2;

const TUint32 KS2FontMagic = 0x53324f4e; // S2FN
const TUint32 KFormatVersion = 0x00010000;


// Read an 8-bit unsigned integer from font data file
static inline TUint8 ReadU8(TUint8 *aData) {
    return *aData;
}

// Read a 16-bit unsigned integer from font data file 
static inline TUint16 ReadU16(TUint8 *aData) {
    return (TUint16) ((((TUint16)aData[0]) << 8) | ((TUint16) aData[1]));
}

// Read a 32-bit unsigned integer from font data file 
static inline TUint32 ReadU32(TUint8 *aData) {
    return ((((TUint32)aData[0]) << 24) |
            (((TUint32)aData[1]) << 16) |
            (((TUint32)aData[2]) << 8) |
            ((TUint32)aData[3]));
}


// Factory
CS2Font *CS2Font::NewL(const TDesC &aFileName) {
    CS2Font *self = new (ELeave) CS2Font();
    CleanupStack::PushL(self);
    self->ConstructL(aFileName);
    CleanupStack::Pop();
    return self;
}


// Constructor
CS2Font::CS2Font() {
    Mem::FillZ(iFontPtr, 256 * sizeof(TUint32*));
}


// Destructor
CS2Font::~CS2Font() {
    TInt i;
    for ( i = 0; i < 256; i++ ) {
        delete [] iFontPtr[i];
    }
    delete [] iFontData;
    delete [] iUnknownCharData;
}


// Constructor
void CS2Font::ConstructL(const TDesC &aFileName) {

    // Read the font file into memory
    RFile file;
    User::LeaveIfError(file.Open(CCoeEnv::Static()->FsSession(),
                                 aFileName,
                                 EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(file);
    TInt fileSize;
    User::LeaveIfError(file.Size(fileSize));
    iFontData = new (ELeave) TUint8[fileSize];
    TPtr8 fontPtr(iFontData, fileSize);
    User::LeaveIfError(file.Read(fontPtr));
    if ( fontPtr.Size() != fileSize ) {
        User::Leave(KErrGeneral);
    }
    CleanupStack::PopAndDestroy(); // file

    // Parse the file header
    TUint8 *p = iFontData;
    if ( fileSize < 24 ) {
        User::Leave(KErrCorrupt);
    }
    
    if ( ReadU32(p) != KS2FontMagic ) {
        User::Leave(KErrCorrupt);
    }
    p += 4;
    
    if ( ReadU32(p) != KFormatVersion ) {
        User::Leave(KErrNotSupported);
    }
    p += 4;
    
    iNumChars = (TInt) ReadU32(p);
    if ( iNumChars > 65536 ) {
        User::Leave(KErrCorrupt);
    }
    p += 4;

    iCharWidth = (TInt) ReadU32(p);
    iCharHeight = (TInt) ReadU32(p+4);
    iByteWidth = (iCharWidth+7) / 8;
    if ( (iCharWidth < 0) || (iCharHeight < 0) ) {
        User::Leave(KErrCorrupt);
    }
    p += 8;

    // Skip the encoding for now
    while ( (*p != 0) && ((p-iFontData) < fileSize) ) {
        p++;
    }
    p++;
    if ( p >= (iFontData+fileSize) ) {
        User::Leave(KErrCorrupt);
    }

    // Check that the length is right
    if ( (p + iNumChars*(2+iByteWidth*iCharHeight)) !=
         (iFontData + fileSize) ) {
        User::Leave(KErrCorrupt);
    }

    // Build pointers to the real character data
    TInt n = iNumChars;
    while ( n-- ) {
        TUint c = ReadU16(p);
        p += 2;
        TUint hi = c >> 8;
        TUint lo = c & 0xff;
        if ( hi > 256 ) {
            User::Leave(KErrCorrupt);
        }
        if ( iFontPtr[hi] == NULL ) {
            iFontPtr[hi] = new (ELeave) TUint8*[256];
            Mem::FillZ(iFontPtr[hi], 256*sizeof(TUint8*));
        }
        iFontPtr[hi][lo] = p;
        p += iByteWidth * iCharHeight;
    }

    // Build blank data for unknown characters
    iUnknownCharData = new (ELeave) TUint8[iCharHeight*iByteWidth];
    Mem::FillZ(iUnknownCharData, iCharHeight*iByteWidth);
}


// Font size in pixels
TSize CS2Font::FontSize() const {
    return TSize(iCharWidth, iCharHeight);
}


// Render text
void CS2Font::RenderText(CFbsBitmap &aBitmap, const TDesC &aText,
                         TInt aXOffset, TInt aYOffset, TRgb aFgColor,
                         TRgb aBgColor) const {
    
    switch ( aBitmap.DisplayMode() ) {
        case EColor4K: {
            TBitmapUtil util(&aBitmap);
            util.Begin(TPoint(0,0));
            RenderTextLockedColor4K(aBitmap, aText, aXOffset, aYOffset,
                                    aFgColor, aBgColor);
            util.End();
            break;
        }

        case EColor64K: {
            TBitmapUtil util(&aBitmap);
            util.Begin(TPoint(0,0));
            RenderTextLockedColor64K(aBitmap, aText, aXOffset, aYOffset,
                                     aFgColor, aBgColor);
            util.End();
            break;
        }

        default:
            User::Panic(KPanic, KPanicUnsupportedBitmapFormat);
    }
}


// Render text to a locked EColor4K bitmap
void CS2Font::RenderTextLockedColor4K(CFbsBitmap &aBitmap, const TDesC &aText,
                                      TInt aXOffset, TInt aYOffset,
                                      TRgb aFgColor, TRgb aBgColor) const {

    RenderTextLocked16bpp(aBitmap, aText, aXOffset, aYOffset,
                          (TUint16)aFgColor.Color4K(),
                          (TUint16)aBgColor.Color4K());
}


// Render text to a locked EColor64K bitmap
void CS2Font::RenderTextLockedColor64K(CFbsBitmap &aBitmap, const TDesC &aText,
                                       TInt aXOffset, TInt aYOffset,
                                       TRgb aFgColor, TRgb aBgColor) const {

    RenderTextLocked16bpp(aBitmap, aText, aXOffset, aYOffset,
                          (TUint16)aFgColor.Color64K(),
                          (TUint16)aBgColor.Color64K());
}


// Render text to a locked 16-bpp bitmap
void CS2Font::RenderTextLocked16bpp(CFbsBitmap &aBitmap, const TDesC &aText,
                                    TInt aXOffset, TInt aYOffset,
                                    TUint16 aFgData, TUint16 aBgData) const {

    // Check that the text fits inside the bitmap
    TInt textLen = aText.Length();
    TSize size = aBitmap.SizeInPixels();
    if ( (aXOffset < 0) || (aYOffset < 0) ||
         ((aXOffset + textLen*iCharWidth) > size.iWidth) ||
         ((aYOffset + iCharHeight) > size.iHeight) ) {
        User::Panic(KPanic, KPanicOutOfBitmapBounds);
    }

    // Build pointers
    TUint8 *data = (TUint8*) aBitmap.DataAddress();
    TInt lineLen = CFbsBitmap::ScanLineLength(aBitmap.SizeInPixels().iWidth,
                                              EColor4K);
    TUint8 *dest = data + aYOffset*lineLen + aXOffset*2;

    // Create color bitmap data
    TUint8 fg0 = (TUint8) (aFgData & 0xff);
    TUint8 fg1 = (TUint8) ((aFgData >> 8) & 0xff);
    TUint8 bg0 = (TUint8) (aBgData & 0xff);
    TUint8 bg1 = (TUint8) ((aBgData >> 8) & 0xff);

    // Render the text
    for ( TInt i = 0; i < textLen; i++ ) {

        // Find font data for this character
        TUint c = aText[i];
        TUint hi = c >> 8;
        TUint lo = c & 0xff;
        TUint8 *s;
        if ( (c > 65536) || (iFontPtr[hi] == NULL) ||
             (iFontPtr[hi][lo] == NULL) ) {
            // Unknown character, just fill with background color
            s = iUnknownCharData;
        } else {
            // Valid character
            s = iFontPtr[hi][lo];
        }

        // Draw all lines
        TUint8 *dl = dest;
        TInt rows = iCharHeight;
        while ( rows-- ) {
            // Draw all bytes on the line
            TUint8 *d = dl;
            TInt cols = iCharWidth;
            TInt bytes = iByteWidth;
            while ( bytes-- ) {
                TUint b = *s++;
                TInt now = cols;
                if ( now > 8 ) {
                    now = 8;
                }
                cols -= now;
                // Draw all pixels from this byte
                while ( now-- ) {
                    if ( b & 0x80 ) {
                        d[0] = fg0;
                        d[1] = fg1;
                    } else {
                        d[0] = bg0;
                        d[1] = bg1;
                    }
                    d += 2;
                    b = b << 1;
                }
            }
            dl += lineLen;
        }

        dest += iCharWidth*2;
    }
}
