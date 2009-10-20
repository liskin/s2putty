/*    profileeditgeneralsettinglist.cpp
 *
 * Putty profile edit view general setting list
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <barsread.h>
#include <putty.rsg>
#include "profileeditgeneralsettinglist.h"
#include "profileeditview.h"
#include "puttyengine.h"
#include "stringutils.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"


// Factory
CProfileEditGeneralSettingList *CProfileEditGeneralSettingList::NewL(
    CPuttyEngine &aPutty, CProfileEditView &aView, TDes &aProfileName) {
    CProfileEditGeneralSettingList *self =
        new (ELeave) CProfileEditGeneralSettingList(aPutty, aView,
                                                    aProfileName);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
CProfileEditGeneralSettingList::CProfileEditGeneralSettingList(
    CPuttyEngine &aPutty, CProfileEditView &aView, TDes &aProfileName)
    : CProfileEditSettingListBase(aPutty, aView),
      iProfileName(aProfileName) {
}


// Second-phase constructor
void CProfileEditGeneralSettingList::ConstructL() {
    iConfig = iPutty.GetConfig();

    iIAPList = new CDesC16ArrayFlat(8);
    iIAPidList = new CArrayFixFlat<TUint32>(8);

    CCommsDatabase* iCommsDB=CCommsDatabase::NewL();
    TBuf<52> iapfromtable;
    TInt err = KErrNone;
    CleanupStack::PushL(iCommsDB);

#ifdef PUTTY_S60V3  
    CCommsDbTableView* iIAPView = iCommsDB->OpenIAPTableViewMatchingBearerSetLC(
            ECommDbBearerGPRS|ECommDbBearerWLAN|ECommDbBearerVirtual,
            ECommDbConnectionDirectionOutgoing); 
#else  
    CCommsDbTableView* iIAPView = iCommsDB->OpenTableLC((TPtrC(IAP)));
#endif
    
    iIAPList->AppendL(_L("Allways ask"));
    iIAPList->AppendL(_L("Default Internet"));
    if ( iIAPView->GotoFirstRecord() == KErrNone ){
        do
        {
            iIAPView->ReadTextL(TPtrC(COMMDB_NAME), iapfromtable);
            iIAPList->AppendL(iapfromtable);
            TUint32 iapID;
            iIAPView->ReadUintL(TPtrC(COMMDB_ID), iapID);
            iIAPidList->AppendL(iapID);
        } while ( err = iIAPView->GotoNextRecord(), err == KErrNone);
    }
    
    CleanupStack::PopAndDestroy(); // view
    CleanupStack::PopAndDestroy(); // commDB
    
    ConstructFromResourceL(R_PUTTY_PROFILEEDIT_GENERAL_SETTINGLIST);
    ActivateL();
}


// Destructor
CProfileEditGeneralSettingList::~CProfileEditGeneralSettingList() {
    if (iIAPList) {
        iIAPList->Reset();
        delete iIAPList;
    }
    if ( iIAPidList ) {
        iIAPidList->Reset();
        delete iIAPidList;
    }
}


// CAknSettingItemList::CreateSettingItemL()
CAknSettingItem *CProfileEditGeneralSettingList::CreateSettingItemL(
    TInt aIdentifier) {

    switch ( aIdentifier ) {
        case EPuttySettingGeneralProfileName:
            return new (ELeave) CAknTextSettingItem(aIdentifier, iProfileName);
            
        case EPuttySettingGeneralHost:
            StringToDes(iConfig->host, iHost);
            return new (ELeave) CAknTextSettingItem(aIdentifier, iHost);
            
        case EPuttySettingGeneralUsername:
            StringToDes(iConfig->username, iUsername);
            return new (ELeave) CAknTextSettingItem(aIdentifier, iUsername);
        case EPuttySettingGeneralPromptAP:
            iPromptAP = iConfig->accesspoint;            
            const CDesCArray &iTmp = *iIAPList;
            return new (ELeave) CDynamicEnumTextSettingItem( aIdentifier, iTmp, iPromptAP);
            break;
    }

    return NULL;
}


// CAknSettingItemList::EditItemL()
void CProfileEditGeneralSettingList::EditItemL(TInt aIndex,
                                               TBool aCalledFromMenu) {
    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
    
    // Always store changes to the variable that the item uses
    (*SettingItemArray())[aIndex]->StoreL();

    // Store the change to PuTTY config if needed
    switch ( (*SettingItemArray())[aIndex]->Identifier() ) {
        case EPuttySettingGeneralHost:
            DesToString(iHost, iConfig->host, sizeof(iConfig->host));
            break;
            
        case EPuttySettingGeneralUsername:
            DesToString(iUsername, iConfig->username, sizeof(iConfig->username));
            break;
        case EPuttySettingGeneralPromptAP:
            iConfig->accesspoint = (int) iPromptAP;
            break;             
        default:
            ;
    }
}
