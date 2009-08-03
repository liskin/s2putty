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
#include "profileedittoolbarsettinglist.h"
#include "profileeditview.h"
#include "dynamicenumtextsettingitem.h"
#include "puttyengine.h"
#include "stringutils.h"
#include "palettes.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"


// Factory
CProfileEditToolbarSettingList *CProfileEditToolbarSettingList::NewL(
    CPuttyEngine &aPutty, CProfileEditView &aView) {
    CProfileEditToolbarSettingList *self =
        new (ELeave) CProfileEditToolbarSettingList(aPutty, aView);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// Constructor
CProfileEditToolbarSettingList::CProfileEditToolbarSettingList(
    CPuttyEngine &aPutty, CProfileEditView &aView)
    : CProfileEditSettingListBase(aPutty, aView) {
}


// Second-phase constructor
void CProfileEditToolbarSettingList::ConstructL() {
    //ConstructFromResourceL(R_PUTTY_PROFILEEDIT_TOUCH_SETTINGLIST);
    ConstructFromResourceL(R_PUTTY_PROFILEEDIT_TOOLBAR_SETTINGLIST);
    ActivateL();
}


// Destructor
CProfileEditToolbarSettingList::~CProfileEditToolbarSettingList() {
}


// CAknSettingItemList::CreateSettingItemL()
CAknSettingItem *CProfileEditToolbarSettingList::CreateSettingItemL(
    TInt aIdentifier) {

    switch ( aIdentifier ) {
        case EPuttySettingsTouchToolBarButton1:
            iToolBarButton1 = iTouchSettings.GetTbButton1();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton1);
        case EPuttySettingsTouchToolBarButton2:
            iToolBarButton2 = iTouchSettings.GetTbButton2();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton2);
        case EPuttySettingsTouchToolBarButton3:
            iToolBarButton3 = iTouchSettings.GetTbButton3();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton3);
        case EPuttySettingsTouchToolBarButton4:
            iToolBarButton4 = iTouchSettings.GetTbButton4();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton4);
        case EPuttySettingsTouchToolBarButton5:
            iToolBarButton5 = iTouchSettings.GetTbButton5();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton5);
        case EPuttySettingsTouchToolBarButton6:
            iToolBarButton6 = iTouchSettings.GetTbButton6();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton6);
        case EPuttySettingsTouchToolBarButton7:
            iToolBarButton7 = iTouchSettings.GetTbButton7();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton7);
        case EPuttySettingsTouchToolBarButton8:
            iToolBarButton8 = iTouchSettings.GetTbButton8();
            return new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier, iToolBarButton8);
    }

    return NULL;
}


// CAknSettingItemList::EditItemL()
void CProfileEditToolbarSettingList::EditItemL(TInt aIndex,
                                               TBool aCalledFromMenu) {
    TInt id = (*SettingItemArray())[aIndex]->Identifier();
    TInt cmd = 0;
    TInt button = -1;
    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);

    // Always store changes to the variable that the item uses
    (*SettingItemArray())[aIndex]->StoreL();

    switch ( id ) {
        case EPuttySettingsTouchToolBarButton1: {
            cmd = iToolBarButton1;
            button = 0;
            if ( iTouchSettings.GetTbButton1() == iToolBarButton1) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton2: {
            cmd = iToolBarButton2;
            button = 1;
            if ( iTouchSettings.GetTbButton2() == iToolBarButton2) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton3: {
            cmd = iToolBarButton3;
            button = 2;
            if ( iTouchSettings.GetTbButton3() == iToolBarButton3) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton4: {
            cmd = iToolBarButton4;
            button = 3;
            if ( iTouchSettings.GetTbButton4() == iToolBarButton4) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton5: {
            cmd = iToolBarButton5;
            button = 4;
            if ( iTouchSettings.GetTbButton5() == iToolBarButton5) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton6: {
            cmd = iToolBarButton6;
            button = 5;
            if ( iTouchSettings.GetTbButton6() == iToolBarButton6) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton7: {
            cmd = iToolBarButton7;
            button = 6;
            if ( iTouchSettings.GetTbButton7() == iToolBarButton7) {
                return;
            }
            break;
        }
        case EPuttySettingsTouchToolBarButton8: {
            cmd = iToolBarButton8;
            button = 7;
            if ( iTouchSettings.GetTbButton8() == iToolBarButton8) {
                return;
            }
            break;
        }
            
        default:
            ;   
    }
    
    if ( iTouchSettings.TestIfToolBarActionAllreadySet(cmd) ) {
        //Show note
        if ( CAknQueryDialog::NewL()->ExecuteLD( R_PUTTY_TBBUTTON_EXISTS_QUERY_DIALOG) ) {
        // do swap else return
            if ( iTouchSettings.SwapButtons(button, cmd) ) {
                iTouchSettings.WriteSettingFileL();
            } 
        }
        //Update all changed / unchanged settings
        iToolBarButton1 = iTouchSettings.GetTbButton1();
        iToolBarButton2 = iTouchSettings.GetTbButton2();
        iToolBarButton3 = iTouchSettings.GetTbButton3();
        iToolBarButton4 = iTouchSettings.GetTbButton4();
        iToolBarButton5 = iTouchSettings.GetTbButton5();
        iToolBarButton6 = iTouchSettings.GetTbButton6();
        iToolBarButton7 = iTouchSettings.GetTbButton7();
        iToolBarButton8 = iTouchSettings.GetTbButton8();
        for ( int i = 0 ; i < SettingItemArray()->Count(); i++) {
            (*SettingItemArray())[i]->LoadL();
            (*SettingItemArray())[i]->UpdateListBoxTextL();       
        }
        this->DrawNow(); // Refresh
        return;
    }
    
    // Store the change to Touch config if needed
    switch ( id ) {
        case EPuttySettingsTouchToolBarButton1: {
            iTouchSettings.SetTbButton1(iToolBarButton1);
            break;
        }
        case EPuttySettingsTouchToolBarButton2: {
            iTouchSettings.SetTbButton2(iToolBarButton2);
            break;
        }
        case EPuttySettingsTouchToolBarButton3: {
            iTouchSettings.SetTbButton3(iToolBarButton3);    
            break;
        }
        case EPuttySettingsTouchToolBarButton4: {
            iTouchSettings.SetTbButton4(iToolBarButton4);
            break;
        }
        case EPuttySettingsTouchToolBarButton5: {
            iTouchSettings.SetTbButton5(iToolBarButton5);
            break;
        }
        case EPuttySettingsTouchToolBarButton6: {
            iTouchSettings.SetTbButton6(iToolBarButton6);
            break;
        }
        case EPuttySettingsTouchToolBarButton7: {
            iTouchSettings.SetTbButton7(iToolBarButton7);
            break;
        }
        case EPuttySettingsTouchToolBarButton8: {
            iTouchSettings.SetTbButton8(iToolBarButton8);
            break;
        }
            
        default:
            ;
    }
    iTouchSettings.WriteSettingFileL(); // Lets write the stuff down to the file
}
