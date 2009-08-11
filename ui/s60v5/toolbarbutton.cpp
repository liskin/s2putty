/*    customtoolbar.h
 *
 * Button class for custom toolbar
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <e32cmn.h>
#include <aknutils.h>
#include "toolbarbutton.h"

CCustomToolbarButton::CCustomToolbarButton () {
    // No implementation required
}

CCustomToolbarButton::~CCustomToolbarButton ( ) {
    ClearIcons(); //Delete button icons
}


CCustomToolbarButton* CCustomToolbarButton::NewLC (TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel) {
    CCustomToolbarButton* self = new (ELeave)CCustomToolbarButton();
    CleanupStack::PushL (self );
    self->ConstructL (aItemRect, aAction, aLabel);
    return self;
}

CCustomToolbarButton* CCustomToolbarButton::NewL (TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel) {
    CCustomToolbarButton* self=CCustomToolbarButton::NewLC (aItemRect, aAction, aLabel);
    CleanupStack::Pop ( ); // self;
    return self;
}

void CCustomToolbarButton::ConstructL (TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel) {
    iItem = aItemRect;
    iButtonData.iAction = aAction;
    iFontSize = 0;
    iOptimalFontSize = 0;
    iButtonData.iUpLabel.Copy(aLabel.Left(iButtonData.iUpLabel.MaxLength()));
    iButtonData.iDownLabel.Copy(aLabel.Left(iButtonData.iDownLabel.MaxLength()));
    iIconsAvailable = EFalse;
    GenerateOptimalFontSizeL();
    iDown = EFalse;
}

CCustomToolbarButton* CCustomToolbarButton::NewLC (TRect aItemRect, toolbarButtonData aButtonData) {
    CCustomToolbarButton* self = new (ELeave)CCustomToolbarButton();
    CleanupStack::PushL (self );
    self->ConstructL (aItemRect, aButtonData);
    return self;
}

CCustomToolbarButton* CCustomToolbarButton::NewL (TRect aItemRect, toolbarButtonData aButtonData) {
    CCustomToolbarButton* self=CCustomToolbarButton::NewLC (aItemRect, aButtonData);
    CleanupStack::Pop ( ); // self;
    return self;
}

void CCustomToolbarButton::ConstructL (TRect aItemRect, toolbarButtonData aButtonData) {
    iItem = aItemRect;
    iButtonData = aButtonData;
    iFontSize = 0;
    iOptimalFontSize = 0;
    GenerateOptimalFontSizeL();
    iIconsAvailable = EFalse;
    iDown = EFalse;
}


void CCustomToolbarButton::ClearIcons() {
    if ( iButtonDownBitmap ) {
        delete iButtonDownBitmap;
    }
    if ( iButtonDownBitmapMask ) {
        delete iButtonDownBitmapMask;
    }
    
    if ( iButtonUpBitmap ) {
        delete iButtonUpBitmap;
    }
    if ( iButtonUpBitmapMask ) {
        delete iButtonUpBitmapMask;
    }
}

void CCustomToolbarButton::GenerateIconL() {
    GenerateIconL(iFontSize);
}

void CCustomToolbarButton::GenerateIconL(TInt aFontSize) {
    //Up (255 * 0.70) = 179, Text = 255
    //Down (255 * 0.23) = 59, Text (255 * 0.5) = 148
    GenerateIconUpL(aFontSize, 179, 250);
    GenerateIconDownL(aFontSize, 59, 148);
}

void CCustomToolbarButton::GenerateIconUpL(TInt fontSize, TInt backgroundTransparency, TInt TextTransparency) {
    TInt requestedFontSize = fontSize;
    if ( iButtonUpBitmap ) {
        delete iButtonUpBitmap;
    }
    if ( iButtonUpBitmapMask ) {
        delete iButtonUpBitmapMask;
    }
    iButtonUpBitmap = CreateBitmapL(iButtonData.iUpLabel, iItem.Size(), &requestedFontSize, EFalse, 0 ,0);
    iButtonUpBitmapMask = CreateBitmapL(iButtonData.iUpLabel, iItem.Size(), &requestedFontSize, ETrue, backgroundTransparency,TextTransparency);
    iFontSize = requestedFontSize;
}

void CCustomToolbarButton::GenerateIconDownL(TInt fontSize, TInt backgroundTransparency, TInt TextTransparency) {
    TInt requestedFontSize = fontSize;
    if ( iButtonDownBitmap ) {
        delete iButtonDownBitmap;
    }
    if ( iButtonDownBitmapMask ) {
        delete iButtonDownBitmapMask;
    }
    iButtonDownBitmap = CreateBitmapL(iButtonData.iDownLabel, iItem.Size(), &requestedFontSize, EFalse, 0 ,0);
    iButtonDownBitmapMask = CreateBitmapL(iButtonData.iDownLabel, iItem.Size(), &requestedFontSize, ETrue, backgroundTransparency,TextTransparency);
    iFontSize = requestedFontSize;
}

void CCustomToolbarButton::GenerateOptimalFontSizeL() {
    CFbsBitmap*                     iIcon;
    CFbsBitmapDevice*               iIconDevice;
    CFbsBitGc*                      iIconContext;
    
    TInt upFont;
    TInt downFont;

    //ButtonUp
    iIcon = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iIcon->Create(iItem.Size(),EGray256)); // 16 bpp

    iIconDevice = CFbsBitmapDevice::NewL(iIcon);
    //User::LeaveIfError(iIconDevice->CreateContext(iIconContext));
    //iIconContext->SetPenStyle( CGraphicsContext::ENullPen );
    //iIconContext->SetBrushStyle( CGraphicsContext::ESolidBrush );

    //TRect buttonRect = TRect(TPoint(2,2),TSize(aTargetSize.iHeight-4,aTargetSize.iWidth-4));
    const CFont* origFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);

    //Determine correct font size for up text
    TInt fontSize = 0;
    TFontSpec myFontSpec = origFont->FontSpecInTwips();
    CFont* selectedfont = NULL;
    TInt correctFontSize = 0;
    do {
        fontSize++;
        myFontSpec.iHeight = fontSize;
        iIconDevice->GetNearestFontInTwips(selectedfont,myFontSpec);
        correctFontSize = selectedfont->TextCount(iButtonData.iUpLabel , iItem.Width() - 6 );
        iIconDevice->ReleaseFont(selectedfont);
    } while ( correctFontSize == iButtonData.iUpLabel.Length() );
    fontSize--;
    
    upFont = fontSize;
    
    //Determine correct font size for down text
    fontSize = 0;
    selectedfont = NULL;
    myFontSpec = origFont->FontSpecInTwips();
    correctFontSize = 0;
    do {
        fontSize++;
        myFontSpec.iHeight = fontSize;
        iIconDevice->GetNearestFontInTwips(selectedfont,myFontSpec);
        correctFontSize = selectedfont->TextCount(iButtonData.iUpLabel , iItem.Width() - 6);
        iIconDevice->ReleaseFont(selectedfont);
    } while ( correctFontSize == iButtonData.iUpLabel.Length() );
    fontSize--;
    
    downFont = fontSize;
    
    if ( upFont > downFont ) {
        iOptimalFontSize = upFont;
    } else {
        iOptimalFontSize = downFont;
    }
    
    //delete iIconContext;
    delete iIconDevice;
    delete iIcon;
    
}

CFbsBitmap*  CCustomToolbarButton::CreateBitmapL( TDes &aText, TSize aTargetSize, TInt* aFontSize, TBool aMask, TInt BackGroundTransparency, TInt TextTransparency ) {
    CFbsBitmap*                     iIcon;
    CFbsBitmapDevice*               iIconDevice;
    CFbsBitGc*                      iIconContext;
    TSize                           iIconSize = aTargetSize;
    TSize                           iIconSizeInPixels;

    //Button icon
    iIcon = new (ELeave) CFbsBitmap;
    if ( aMask ) {
        User::LeaveIfError( iIcon->Create(iIconSize,EGray256)); // 16 bpp
    } else {
        User::LeaveIfError( iIcon->Create(iIconSize,EColor64K)); // 16 bpp
    }
    iIconDevice = CFbsBitmapDevice::NewL(iIcon);
    User::LeaveIfError(iIconDevice->CreateContext(iIconContext));
    iIconContext->SetPenStyle( CGraphicsContext::ENullPen );
    iIconContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iIconSizeInPixels = iIcon->SizeInPixels();

    TRect buttonRect = TRect(TPoint(2,2),TSize(aTargetSize.iHeight-4,aTargetSize.iWidth-4));
    const CFont* origFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    
    //Determine correct font size
    TInt fontSize = 0;
    TFontSpec myFontSpec = origFont->FontSpecInTwips();
    CFont* selectedfont;
    TInt correctFontSize = 0;
    if ( *aFontSize == 0 ) {
        do {
            fontSize++;
            myFontSpec.iHeight = fontSize;
            iIconDevice->GetNearestFontInTwips(selectedfont,myFontSpec);
            correctFontSize = selectedfont->TextCount(aText, aTargetSize.iWidth-6);
            iIconDevice->ReleaseFont(selectedfont);
        } while ( correctFontSize == aText.Length() );
        fontSize--;
        *aFontSize = fontSize;
    } else {
        fontSize = *aFontSize;
    }
    myFontSpec.iHeight = fontSize;
    iIconDevice->GetNearestFontInTwips(selectedfont,myFontSpec);
    
    if ( !aMask ) {
        TRect ring1 = TRect(TPoint(0,0),aTargetSize);
        iIconContext->SetBrushColor( KRgbBlack );
        iIconContext->DrawRect(ring1);
        TRect ring2 = TRect(TPoint(1,1),TSize(aTargetSize.iHeight-2,aTargetSize.iWidth-2));
        iIconContext->SetBrushColor( KRgbWhite );
        iIconContext->DrawRect(ring2);
    
        iIconContext->UseFont(selectedfont);
        iIconContext->SetBrushColor( KRgbBlue );
        iIconContext->SetPenColor(KRgbYellow);
    } else {
        iIconContext->SetBrushColor( KRgbBlack );
    }
    if ( !aMask ) {
        //iIconContext->SetFadingParameters(50,255);
    } else {
        //For mask black & white
        iIconContext->SetFaded(ETrue);
        iIconContext->UseFont(selectedfont);
        
        iIconContext->SetFadingParameters( BackGroundTransparency,255 );
    }
    iIconContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iIconContext->DrawRect(buttonRect);
    
    iIconContext->SetPenStyle( CGraphicsContext::ESolidPen );
    iIconContext->SetBrushStyle( CGraphicsContext::ENullBrush );
    
    if ( !aMask ) {
        //iIconContext->SetFadingParameters(160,255);
    } else {
        //for mask black & white
        
        iIconContext->SetFadingParameters(TextTransparency,255);
    }
                 
    iIconContext->DrawText(aText, buttonRect, ( buttonRect.Height() /2 + selectedfont->FontMaxAscent()/2 ) , CGraphicsContext::ECenter, 0);
    iIconDevice->ReleaseFont(selectedfont);
    
    iIconContext->SetPenStyle( CGraphicsContext::ENullPen );
    iIconContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    
    delete iIconContext;
    delete iIconDevice;
    
    return iIcon;
}

