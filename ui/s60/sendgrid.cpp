/*    sendgrid.cpp
 *
 * S60 "Send" grid control
 *
 * Copyright 2008 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <akngrid.h>
#include <aknutils.h>
#include <barsread.h>
#include <aknlists.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <aknsutils.h>
#include "sendgrid.h"

_LIT(KPanic, "SENDGRID");
const TInt KPanicBadResource = 1;


// The text to use as a reference for grid item width
_LIT(KWidthRefText, " MMMMMM ");

// Grid item labels
char KGridItemLabels[KSendGridNumItems] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '*', '0', '#'
};


// Get recommended size in current UI layout
void CSendGrid::GetRecommendedSize(TSize &aSize) {
    const CFont *font = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    aSize.iHeight = 4 * ((7 * font->FontMaxHeight())/3) + 2;
    aSize.iWidth = 3 * (font->TextWidthInPixels(KWidthRefText)) + 2;
}


// Factory
CSendGrid* CSendGrid::NewL(const TRect &aRect, TInt aResourceId,
                           MSendGridObserver &aObserver) {
    CSendGrid *self = new (ELeave) CSendGrid(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aResourceId);
    CleanupStack::Pop();
    return self;
}


// Constructor
CSendGrid::CSendGrid(MSendGridObserver &aObserver)
    : iObserver(aObserver) {
}


// Destructor
CSendGrid::~CSendGrid() {
    if ( iSetCba ) {
        iCba->RemoveCommandFromStack(0, EAknSoftkeySelect);
        iCba->RemoveCommandFromStack(2, EAknSoftkeyCancel);
        iCba->DrawDeferred();
    }
    if ( iObsCba0 ) {
        iCba->RemoveCommandObserver(0);
    }                        
    if ( iObsCba1 ) {
        iCba->RemoveCommandObserver(2);
    }                        
    delete iGrid;
    iGridStack.Close();
}


// Second-phase constructor
void CSendGrid::ConstructL(const TRect &aRect, TInt aResourceId) {
    CreateWindowL();
    SetRect(aRect);

    // Create grid and grid model
    iGrid = new (ELeave) CAknGrid;
            
    CAknGridM *gridM = new (ELeave) CAknGridM;
    iGrid->SetModel(gridM);    
    iGrid->ConstructL(this, EAknListBoxSelectionGrid);

    // Item size and grid layout
    TSize itemSize((aRect.Width()-2)/3, (aRect.Height()-2)/4);
    iGrid->SetLayoutL(EFalse, ETrue, ETrue, 3, 4, itemSize);
    iGrid->SetPrimaryScrollingType(CAknGridView::EScrollIncrementLineAndLoops);
    iGrid->SetSecondaryScrollingType(CAknGridView::EScrollIncrementLineAndLoops);
    iGrid->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);

    // Set up grid layout
    AknListBoxLayouts::SetupStandardGrid(*iGrid);

    // Set initial grid content
    SetItemsL(aResourceId);

    // Label text cell, contains the key symbol
    const CFont *font = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    TPoint text1Start(0, 0);
    TPoint text1End(itemSize.iWidth, font->FontMaxHeight());
    AknListBoxLayouts::SetupFormTextCell(*iGrid, iGrid->ItemDrawer(),
                                         1, // text pos
                                         font, 
                                         215, // color
                                         0, // left margin
                                         0, // right margin
                                         font->FontMaxAscent(), // baseline
                                         itemSize.iWidth, // width
                                         CGraphicsContext::ECenter, // alignment
                                         text1Start, 
                                         text1End);
    
    // Text cell for cell text from the resource file
    TPoint text2Start(0, font->FontMaxHeight());
    TPoint text2End(itemSize.iWidth, itemSize.iHeight);
    AknListBoxLayouts::SetupFormTextCell(*iGrid, iGrid->ItemDrawer(),
                                         2, // text pos
                                         font, 
                                         215, // color
                                         0, // left margin
                                         0, // right margin
                                         text2Start.iY + font->FontMaxAscent(), // baseline
                                         itemSize.iWidth, // width
                                         CGraphicsContext::ECenter, // alignment
                                         text2Start, 
                                         text2End);
    
    // Set size before changing colors -- according to the Forum Nokia
    // Knowledge Base article TSS000596 CAknGrid::SizeChanged() may override
    // this
    iGrid->SetRect(TRect(TPoint(1,1), Rect().Size() - TSize(2,2)));

    // Get grid item colors from the current skin. See Forum Nokia TSS000596.
    // Now if somebody could just tell me why CAknGrid doesn't do this by
    // default -- all other controls do...
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    CFormattedCellListBoxData::TColors colors;
    AknsUtils::GetCachedColor(skin, colors.iText, KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG9);
    AknsUtils::GetCachedColor(skin, colors.iHighlightedText,
                              KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG11);
    iGrid->ItemDrawer()->FormattedCellData()->SetSubCellColorsL(1, colors);
    iGrid->ItemDrawer()->FormattedCellData()->SetSubCellColorsL(2, colors);

    // Enable and make visible
    iGrid->MakeVisible(ETrue);
    iGrid->SetFocus(ETrue);    
    ActivateL();
    iGrid->ActivateL();

    // Set new softkeys
    iCba = CEikButtonGroupContainer::Current();
    iCba->AddCommandSetToStackL(R_AVKON_SOFTKEYS_SELECT_BACK);
    iSetCba = ETrue;
    iCba->UpdateCommandObserverL(0, *this);
    iObsCba0 = ETrue;
    iCba->UpdateCommandObserverL(2, *this);
    iObsCba1 = ETrue;
    iCba->DrawDeferred();
}



// CCoeControl::CountComponentControls()
TInt CSendGrid::CountComponentControls() const {
    return 1;
}


// CCoeControl::ComponentControl()
CCoeControl *CSendGrid::ComponentControl(TInt aIndex) const {
    return iGrid;
}


// Set grid items from a resource
void CSendGrid::SetItemsL(TInt aResourceId) {

    // Clear previous items if we already have an item array, otherwise create
    // it
    if ( iItemArray ) {
        iGrid->SetCurrentItemIndex(0);
        iItemArray->Reset();
        iGrid->HandleItemRemovalL();
    } else {
        iItemArray = new (ELeave) CDesCArrayFlat(12);
        iGrid->Model()->SetItemTextArray(iItemArray);
    }
    
    // Read items from the resource file
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, aResourceId);
    TInt count = reader.ReadInt16();
    __ASSERT_ALWAYS(count == KSendGridNumItems,
                    User::Panic(KPanic, KPanicBadResource));
    for ( TInt i = 0; i < count; i++ ) {
        iCommands[i] = reader.ReadInt32();
        iSubGrids[i] = reader.ReadInt32();
        TBuf<32> buf;
        buf.Append('\t');
        buf.Append(KGridItemLabels[i]);
        buf.Append('\t');
        TPtrC ptr = reader.ReadTPtrC();
        __ASSERT_ALWAYS(ptr.Length() <= 30,
                    User::Panic(KPanic, KPanicBadResource));
        buf.Append(ptr);
        iItemArray->AppendL(buf);
    }    
    CleanupStack::PopAndDestroy(); // reader
    iGrid->HandleItemAdditionL();
    iGrid->SetCurrentItemIndex(0);
    iCurrentGrid = aResourceId;
}


// Handle user selection of a grid item
void CSendGrid::HandleSelectionL(TInt aIndex) {
    if ( iSubGrids[aIndex] != 0 ) {
        iGridStack.Append(iCurrentGrid);
        SetItemsL(iSubGrids[aIndex]);
    } else if ( iCommands[aIndex] != 0 ) {
        iObserver.MsgoCommandL(iCommands[aIndex]);
    }
}


// CCoeControl::OfferKeyEventL()
TKeyResponse CSendGrid::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                       TEventCode aType) {

    if ( aType == EEventKey ) {
        switch ( aKeyEvent.iScanCode ) {
            case EStdKeyDevice3: // Center of joystick -- select current
                HandleSelectionL(iGrid->CurrentDataIndex());
                return EKeyWasConsumed;

            // Selection shortcuts
            // Note that E71 returns QWERTY keycodes while earlier S60 QWERTY
            // devices return the numbers printed in the keys. We'll now accept
            // both
#define MAPKEY1(key1, index) case key1: HandleSelectionL(index); return EKeyWasConsumed;
#define MAPKEY2(key1, key2, index) case key1: case key2: HandleSelectionL(index); return EKeyWasConsumed;
            MAPKEY2('1', 'R', 0);
            MAPKEY2('2', 'T', 1);
            MAPKEY2('3', 'Y', 2);
            MAPKEY2('4', 'F', 3);
            MAPKEY2('5', 'G', 4);
            MAPKEY2('6', 'H', 5);
            MAPKEY2('7', 'V', 6);
            MAPKEY2('8', 'B', 7);
            MAPKEY2('9', 'N', 8);
            MAPKEY2(EStdKeyNkpAsterisk, 'U', 9);
            MAPKEY1('*', 9);
            MAPKEY2('0', 'M', 10);
            MAPKEY2(EStdKeyHash, 'J', 11);
            MAPKEY1('#', 9);
        }
    }
    
    return iGrid->OfferKeyEventL(aKeyEvent, aType);
}


// MEikCommandObserver::ProcessCommandL()
void CSendGrid::ProcessCommandL(TInt aCommand) {
    switch ( aCommand ) {
        case EAknSoftkeyCancel: {
            // Go back to the previous grid if we have one, otherwise terminate
            TInt count = iGridStack.Count();
            if ( count > 0 ) {
                TInt id = iGridStack[count - 1];
                iGridStack.Remove(count - 1);
                SetItemsL(id);
            } else {
                iObserver.MsgoTerminated();
            }
            return;
        }

        case EAknSoftkeySelect:
            HandleSelectionL(iGrid->CurrentDataIndex());
            return;
    }
}


// CCoeControl::Draw()
void CSendGrid::Draw(const TRect &aRect) const {
    // Draw a one-pixel rectangle around the grid
    CWindowGc& gc = SystemGc();
    gc.Reset();
    gc.DrawRect(Rect());
}
