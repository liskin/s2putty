/*    profileedittouchsettinglist.cpp
 *
 * Putty profile edit view touch setting list
 *
 * Copyright 2009 Avila Risto
 *
 * See license.txt for full copyright and license information.
*/

#include <barsread.h>
#include <putty.rsg>
#include <akncommondialogs.h>
#include <aknquerydialog.h>
#include <badesca.h>
#include "profileeditgeneraltoolbarsettinglist.h"
#include "profileeditview.h"
#include "dynamicenumtextsettingitem.h"
#include "puttyengine.h"
#include "stringutils.h"
#include "palettes.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"


// Factory
CProfileEditGeneralToolbarSettingList *CProfileEditGeneralToolbarSettingList::NewL(
    CPuttyEngine &aPutty, CProfileEditView &aView) {
    CProfileEditGeneralToolbarSettingList *self =
        new (ELeave) CProfileEditGeneralToolbarSettingList(aPutty, aView);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// Constructor
CProfileEditGeneralToolbarSettingList::CProfileEditGeneralToolbarSettingList(
    CPuttyEngine &aPutty, CProfileEditView &aView)
    : CProfileEditSettingListBase(aPutty, aView) {
}


// Second-phase constructor
void CProfileEditGeneralToolbarSettingList::ConstructL() {
    //ConstructFromResourceL(r_putty_profileedit_general_toolbar_settinglist);
    ConstructFromResourceL(R_PUTTY_PROFILEEDIT_GENERAL_TOOLBAR_SETTINGLIST);
    ActivateL();
}


// Destructor
CProfileEditGeneralToolbarSettingList::~CProfileEditGeneralToolbarSettingList() {
}


// CAknSettingItemList::CreateSettingItemL()
CAknSettingItem *CProfileEditGeneralToolbarSettingList::CreateSettingItemL(
    TInt aIdentifier) {

    switch ( aIdentifier ) {
        case EPuttySettingsTouchShowToolbarStartup:
            iShowToolbar = iTouchSettings.GetShowToolbar();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iShowToolbar);
        case EPuttySettingsTocuhToolBarNumberOfButtons:
            iToolBarButtonCount = iTouchSettings.GetTbButtonCount();
            return new (ELeave) CAknIntegerEdwinSettingItem(aIdentifier, iToolBarButtonCount);
        case EPuttySettingsTocuhToolBarButtonWidth:
            iToolBarButtonWidth = iTouchSettings.GetTbButtonWidth();
            return new (ELeave) CAknIntegerEdwinSettingItem(aIdentifier, iToolBarButtonWidth);
        case EPuttySettingsTocuhToolBarButtonHeigth:
            iToolBarButtonHeigth = iTouchSettings.GetTbButtonHeigth();
            return new (ELeave) CAknIntegerEdwinSettingItem(aIdentifier, iToolBarButtonHeigth);
    }

    return NULL;
}


// CAknSettingItemList::EditItemL()
void CProfileEditGeneralToolbarSettingList::EditItemL(TInt aIndex,
                                               TBool aCalledFromMenu) {
    TInt id = (*SettingItemArray())[aIndex]->Identifier();

    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);

    // Always store changes to the variable that the item uses
    (*SettingItemArray())[aIndex]->StoreL();


    
    // Store the change to Touch config if needed
    switch ( id ) {
        case EPuttySettingsTouchShowToolbarStartup: {
            iTouchSettings.SetShowToolbar(iShowToolbar);
            break;
        }
        case EPuttySettingsTocuhToolBarNumberOfButtons: {
            iTouchSettings.SetTbButtonCount(iToolBarButtonCount);
            break;
        }
        case EPuttySettingsTocuhToolBarButtonWidth: {
            iTouchSettings.SetTbButtonWidth(iToolBarButtonWidth);    
            break;
        }
        case EPuttySettingsTocuhToolBarButtonHeigth: {
            iTouchSettings.SetTbButtonHeigth(iToolBarButtonHeigth);
            break;
        }
        
        default:
            ;
    }
    iTouchSettings.WriteSettingFileL(); // Lets write the stuff down to the file
}
