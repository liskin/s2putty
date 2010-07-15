/*    profileeditdisplaysettinglist.cpp
 *
 * Putty profile edit view touch setting list
 *
 * Copyright 2009 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <barsread.h>
#include <putty.rsg>
#include <akncommondialogs.h>
#include <badesca.h>
#include "profileedittouchsettinglist.h"
#include "profileeditview.h"
#include "dynamicenumtextsettingitem.h"
#include "puttyengine.h"
#include "stringutils.h"
#include "palettes.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"


// Factory
CProfileEditTouchSettingList *CProfileEditTouchSettingList::NewL(
    CPuttyEngine &aPutty, CProfileEditView &aView) {
    CProfileEditTouchSettingList *self =
        new (ELeave) CProfileEditTouchSettingList(aPutty, aView);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// Constructor
CProfileEditTouchSettingList::CProfileEditTouchSettingList(
    CPuttyEngine &aPutty, CProfileEditView &aView)
    : CProfileEditSettingListBase(aPutty, aView) {
}


// Second-phase constructor
void CProfileEditTouchSettingList::ConstructL() {
    ConstructFromResourceL(R_PUTTY_PROFILEEDIT_TOUCH_SETTINGLIST);
    ActivateL();
}


// Destructor
CProfileEditTouchSettingList::~CProfileEditTouchSettingList() {
}


// CAknSettingItemList::CreateSettingItemL()
CAknSettingItem *CProfileEditTouchSettingList::CreateSettingItemL(
    TInt aIdentifier) {

    switch ( aIdentifier ) {
        case EPuttySettingsTouchAllowMouseGrab:
            iTouchAllowMouseGrab = iTouchSettings.GetAllowMouseGrab();
            return new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iTouchAllowMouseGrab);
        case EPuttySettingsTouchClick:
            iTouchClick = iTouchSettings.GetSingleTap();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchClick);
        case EPuttySettingsTouchDoubleClick:
            iTouchDoubleClick = iTouchSettings.GetDoubleTap();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchDoubleClick);
        case EPuttySettingsTouchLongClick:
            iTouchLongClick = iTouchSettings.GetLongTap();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchLongClick);
        case EPuttySettingsTouchSwipeUp:
            iTouchSwipeUp = iTouchSettings.GetSwipeUp();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchSwipeUp);
        case EPuttySettingsTouchSwipeDown:
            iTouchSwipeDown = iTouchSettings.GetSwipeDown();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchSwipeDown);
        case EPuttySettingsTouchSwipeLeft:
            iTouchSwipeLeft = iTouchSettings.GetSwipeLeft();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchSwipeLeft);
        case EPuttySettingsTouchSwipeRight:
            iTouchSwipeRight = iTouchSettings.GetSwipeRight();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iTouchSwipeRight);
     }

    return NULL;
}


// CAknSettingItemList::EditItemL()
void CProfileEditTouchSettingList::EditItemL(TInt aIndex,
                                               TBool aCalledFromMenu) {
    TInt id = (*SettingItemArray())[aIndex]->Identifier();
    
    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
    
    // Always store changes to the variable that the item uses
    (*SettingItemArray())[aIndex]->StoreL();

    // Store the change to Touch config if needed
    switch ( id ) {
        case EPuttySettingsTouchAllowMouseGrab: {
            iTouchSettings.SetAllowMouseGrab(iTouchAllowMouseGrab);
            break;
        }
        case EPuttySettingsTouchClick: {
            iTouchSettings.SetSingleTap(iTouchClick);
            break;
        }
        case EPuttySettingsTouchDoubleClick: {
            iTouchSettings.SetDoubleTap(iTouchDoubleClick);
            break;
        }
        case EPuttySettingsTouchLongClick: {
            iTouchSettings.SetLongTap(iTouchLongClick);
            break;
        }
        case EPuttySettingsTouchSwipeUp: {
            iTouchSettings.SetSwipeUp(iTouchSwipeUp);
            break;
        }
        case EPuttySettingsTouchSwipeDown: {
            iTouchSettings.SetSwipeDown(iTouchSwipeDown);
            break;
        }
        case EPuttySettingsTouchSwipeLeft: {
            iTouchSettings.SetSwipeLeft(iTouchSwipeLeft);
            break;
        }
        case EPuttySettingsTouchSwipeRight: {
            iTouchSettings.SetSwipeRight(iTouchSwipeRight);
            break;
        }
        default:
            ;
    }
    iTouchSettings.WriteSettingFileL(); // Lets write the stuff down to the file
}
