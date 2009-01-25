/*    profileeditsettinglistbase.cpp
 *
 * Putty profile edit view setting list base class
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "profileeditsettinglistbase.h"
#include "profileeditview.h"
#include "puttyui.hrh"


// Constructor
CProfileEditSettingListBase::CProfileEditSettingListBase(
    CPuttyEngine &aPutty, CProfileEditView &aView)
    : iPutty(aPutty),
      iView(aView) {
}


// Destructor
CProfileEditSettingListBase::~CProfileEditSettingListBase() {
}


// CCoeControl::SizeChanged()
void CProfileEditSettingListBase::SizeChanged() {
    // Seems we need to propagate size changes to the listbox manually
    CEikFormattedCellListBox *listbox = ListBox();
    if ( listbox ) {
        listbox->SetRect(Rect());
    }
}


// CCoeControl::OfferKeyEventL()
TKeyResponse CProfileEditSettingListBase::OfferKeyEventL(
    const TKeyEvent &aKeyEvent, TEventCode aType) {

    // Left and right arrows switch between setting lists.
    if ( aType == EEventKey ) {
        switch ( aKeyEvent.iCode ) {
            case EKeyLeftArrow:
                iView.HandleCommandL(EPuttyCmdProfileEditPrevSettingList);
                return EKeyWasConsumed;

            case EKeyRightArrow:
                iView.HandleCommandL(EPuttyCmdProfileEditNextSettingList);
                return EKeyWasConsumed;
        }
    }
    
    // Send other key events to the base class
    return CAknSettingItemList::OfferKeyEventL(aKeyEvent, aType);         
}
