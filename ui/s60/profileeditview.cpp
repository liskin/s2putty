/*    profileeditview.cpp
 *
 * Putty profile edit view
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <aknviewappui.h>
#include <aknlists.h>
#include <badesca.h>
#include <putty.rsg>
#include <akntitle.h>
#include <bautils.h>
#include <f32file.h>
#include <aknnotedialog.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include "profileeditview.h"
#include "puttyappui.h"
#include "profileeditgeneralsettinglist.h"
#include "profileeditsshsettinglist.h"
#include "profileeditdisplaysettinglist.h"
#include "profileeditloggingsettinglist.h"
#include "puttyengine.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"

_LIT(KAssertPanic, "pev");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

// CCoeControl::CreateWindowL() is protected, so we will need to subclass for
// the simple reason of making the listbox a window-owning control.
class CWindowOwningAknSingleStyleListBox : public CAknSingleStyleListBox {
public:
    void DoCreateWindowL() { CreateWindowL(); }
};


// Factory
CProfileEditView *CProfileEditView::NewL() {
    CProfileEditView *self = new (ELeave) CProfileEditView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
CProfileEditView::CProfileEditView() {
}


// Second-phase constructor
void CProfileEditView::ConstructL() {
    BaseConstructL(R_PUTTY_PROFILEEDIT_VIEW);
    iSwitcher = new (ELeave) CViewSwitcher(*this);

    // Create a navi decorator with tabs for switching between setting edit
    // lists
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC(
        reader, R_PUTTY_SETTINGS_NAVI_DECORATOR);
    iDecorator = ((CPuttyAppUi*)AppUi())->NaviPane().
        ConstructNavigationDecoratorFromResourceL(reader);
    CleanupStack::PopAndDestroy(); //reader
}


// Destructor
CProfileEditView::~CProfileEditView() {
    delete iSwitcher;
    delete iDecorator;
}


// CAknView::Id()
TUid CProfileEditView::Id() const {
    return TUid::Uid(KUidPuttyProfileEditViewDefine);
}


// CAknView::HandleCommandL
void CProfileEditView::HandleCommandL(TInt aCommand) {
    
    switch ( aCommand ) {

        case EPuttyCmdProfileEditOpen:
            // Open a setting list
            if ( (iView == EViewPageList) && iControl ) {
                CEikListBox *box = (CEikListBox*) iControl;
                SetViewL((TView)(EViewGeneral + box->CurrentItemIndex()));
            } else if ( iControl ) {
                CAknSettingItemList *list = (CAknSettingItemList*) iControl;
                list->EditItemL(list->ListBox()->CurrentItemIndex(), ETrue);
            }
            break;
            
        case EAknSoftkeyBack:
            if ( iView == EViewPageList ) {
                AppUi()->ActivateLocalViewL(
                    TUid::Uid(KUidPuttyProfileListViewDefine));
            } else {
                SetViewL(EViewPageList);
            }
            break;

        case EPuttyCmdProfileEditNextSettingList:
            assert(iView != EViewPageList);
            if ( iView < EViewLogging ) {
                SetViewL((TView)(iView + 1));
            }
            break;

        case EPuttyCmdProfileEditPrevSettingList:
            assert(iView != EViewPageList);
            if ( iView > EViewGeneral ) {
                SetViewL((TView)(iView - 1));
            }
            break;

        case EPuttyCmdProfileEditClear:
            if ( (iView == EViewSsh) && iControl  ) {
                CProfileEditSshSettingList *list =
                    (CProfileEditSshSettingList*) iControl;
                list->ClearPrivateKey();
            }            
            break;
        
        default:
            break;
    }
}


// CAknView::DoActivateL
void CProfileEditView::DoActivateL(const TVwsViewId & /*aPrevViewId*/,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8 & /*aCustomMessage*/) {

    // Get profile edit data from the app ui
    ((CPuttyAppUi*)AppUi())->GetProfileEditDataL(iPutty, iProfileName);

    // Create the settings page selection listbox as the first control
    SetViewL(EViewPageList);

    // Set title
    HBufC *title = CEikonEnv::Static()->AllocReadResourceL(
        R_PUTTY_STR_PROFILEEDIT_TITLE);
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
        (StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
    titlePane->SetText(title); //takes ownership
}


// CAknView::DoDeactivate()
void CProfileEditView::DoDeactivate() {

    if ( iView != EViewPageList ) {
        ((CPuttyAppUi*)AppUi())->NaviPane().Pop();
    }
    if ( iControl ) {
        AppUi()->RemoveFromStack(iControl);
        delete iControl;
        iControl = NULL;
    }
}


// CAknView::HandleStatusPaneSizeChange()
void CProfileEditView::HandleStatusPaneSizeChange() {
    if ( iControl ) {
        iControl->SetRect(ClientRect());
    }
}


// MEikListBoxObserver::HandleListBoxEventL()
void CProfileEditView::HandleListBoxEventL(CEikListBox *aListBox,
                                           TListBoxEvent aEventType) {
    if ( (aEventType == EEventEnterKeyPressed) ||
         (aEventType == EEventItemDoubleClicked) ) {
        iSwitcher->SwitchView((TView)(EViewGeneral +
                                      aListBox->CurrentItemIndex()));
    }
}


// CAknView::DynInitMenuPaneL
void CProfileEditView::DynInitMenuPaneL(TInt aResourceId,
                                        CEikMenuPane *aMenuPane) {
    switch ( aResourceId ) {
        case R_PUTTY_PROFILEEDIT_MENU_PANE:
            if ( iView == EViewPageList ) {
                aMenuPane->SetItemTextL(EPuttyCmdProfileEditOpen,
                                        R_PUTTY_STR_PROFILEEDIT_PAGELIST_OPEN);
            } else {
                aMenuPane->SetItemTextL(
                    EPuttyCmdProfileEditOpen,
                    R_PUTTY_STR_PROFILEEDIT_SETTINGLIST_EDIT);
            }
            if ( iView == EViewSsh && iControl ) {
                CAknSettingItemList *list = (CAknSettingItemList*) iControl;
                TInt idx = list->ListBox()->CurrentItemIndex();
                TInt id = (*(list->SettingItemArray()))[idx]->Identifier();
                if ( id == EPuttySettingSshPrivateKey ) {
                    aMenuPane->SetItemDimmed(EPuttyCmdProfileEditClear, EFalse);
                } else {
                aMenuPane->SetItemDimmed(EPuttyCmdProfileEditClear, ETrue);
                }
            } else {
                aMenuPane->SetItemDimmed(EPuttyCmdProfileEditClear, ETrue);
            }
            
            break;
    }
}



void CProfileEditView::SetViewL(CProfileEditView::TView aView) {

    TView oldView = iView;    

    CCoeControl *oldControl = iControl;
    if ( oldControl ) {
        AppUi()->RemoveFromViewStack(*this, oldControl);
        iControl = NULL;
        CleanupStack::PushL(oldControl);
    }

    if ( (oldView != EViewPageList) && (aView == EViewPageList) ) {
        // Remove navi pane tabs if going back to the page list
        ((CPuttyAppUi*)AppUi())->NaviPane().Pop();
    }

    switch ( aView ) {
        
        case EViewPageList: {
            // Profile list listbox
            CWindowOwningAknSingleStyleListBox *box =
                new (ELeave) CWindowOwningAknSingleStyleListBox;
            CleanupStack::PushL(box);
            box->SetMopParent(this);
            box->DoCreateWindowL();
            TResourceReader rr;
            CEikonEnv::Static()->CreateResourceReaderLC(
                rr, R_PUTTY_PROFILEEDIT_PAGESELECT_LISTBOX);
            box->ConstructFromResourceL(rr);
            CleanupStack::PopAndDestroy(); //rr
            box->CreateScrollBarFrameL(ETrue);
            box->ScrollBarFrame()->SetScrollBarVisibilityL(
                CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto);
            box->SetRect(ClientRect());
            box->SetListBoxObserver(this);
            if ( oldView != EViewPageList ) {
                box->SetCurrentItemIndex(oldView - EViewGeneral);
            }
            AppUi()->AddToViewStackL(*this, box);
            box->ActivateL();
            CleanupStack::Pop(); // box
            
            iControl = box;
            iView = EViewPageList;
            break;
        }

        case EViewGeneral: {
            CProfileEditGeneralSettingList *list =
                CProfileEditGeneralSettingList::NewL(*iPutty, *this,
                                                     *iProfileName);
            CleanupStack::PushL(list);
            list->SetMopParent(this);
            list->SetRect(ClientRect());
            AppUi()->AddToViewStackL(*this, list);
            CleanupStack::Pop(); // list
            iControl = list;
            iView = EViewGeneral;
            break;
        }

        case EViewSsh: {
            CProfileEditSshSettingList *list =
                CProfileEditSshSettingList::NewL(*iPutty, *this);
            CleanupStack::PushL(list);
            list->SetMopParent(this);
            list->SetRect(ClientRect());
            AppUi()->AddToViewStackL(*this, list);
            CleanupStack::Pop(); // list
            iControl = list;
            iView = EViewSsh;
            break;
        }

        case EViewDisplay: {
            CProfileEditDisplaySettingList *list =
                CProfileEditDisplaySettingList::NewL(
                    *iPutty, *this, ((CPuttyAppUi*)AppUi())->Fonts());
            CleanupStack::PushL(list);
            list->SetMopParent(this);
            list->SetRect(ClientRect());
            AppUi()->AddToViewStackL(*this, list);
            CleanupStack::Pop(); // list
            iControl = list;
            iView = EViewDisplay;
            break;
        }

        case EViewLogging: {
            CProfileEditLoggingSettingList *list =
                CProfileEditLoggingSettingList::NewL(*iPutty, *this);
            CleanupStack::PushL(list);
            list->SetMopParent(this);
            list->SetRect(ClientRect());
            AppUi()->AddToViewStackL(*this, list);
            CleanupStack::Pop(); // list
            iControl = list;
            iView = EViewLogging;
            break;
        }

        default:
            assert(0);
    }
    
    if ( iView != EViewPageList ) {
        if ( oldView == EViewPageList ) {
            // Add navi pane tabs if going from page list to a setting list
            ((CPuttyAppUi*)AppUi())->NaviPane().PushL(*iDecorator);
        }
        // Activate the right tab
	CAknTabGroup* tabGroup = (CAknTabGroup*) iDecorator->DecoratedControl();
	tabGroup->SetActiveTabByIndex(iView - EViewGeneral);
    }

    if ( oldControl ) {
        CleanupStack::PopAndDestroy(); //oldControl
    }
}



// Helper class for asynchronous view switches
CProfileEditView::CViewSwitcher::CViewSwitcher(CProfileEditView &aView)
    : CActive(EPriorityStandard),
      iView(aView) {
    CActiveScheduler::Add(this);
}

CProfileEditView::CViewSwitcher::~CViewSwitcher() {
    Cancel();
}

void CProfileEditView::CViewSwitcher::SwitchView(
    CProfileEditView::TView aNewView) {
    if ( !IsActive() ) {
        iNewView = aNewView;
        TRequestStatus *stat = &iStatus;
        User::RequestComplete(stat, NULL);
        SetActive();
    }
}

void CProfileEditView::CViewSwitcher::RunL() {
    iView.SetViewL(iNewView);    
}

void CProfileEditView::CViewSwitcher::DoCancel() {
    assert(0);
}
