/*    profileeditgeneralsettinglist.h
 *
 * Putty profile edit view general setting list
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITGENERALSETTINGLIST_H
#define PROFILEEDITGENERALSETTINGLIST_H

#include <coecntrl.h>
#include <aknsettingitemlist.h>
#include "profileeditsettinglistbase.h"
#ifdef PUTTY_S60TOUCH
    #include "touchuisettings.h"
#endif
extern "C" {
#include "putty.h" // struct Config
}

// Forward declarations
class CPuttyEngine;
class CProfileEditView;

/**
 * Setting list for general profile settings.
 */
class CProfileEditGeneralSettingList : public CProfileEditSettingListBase {

public:
    /** 
     * Factory method.
     * 
     * @param aPutty The PuTTY engine instance that contains the settings to
     *               be edited
     * @param aView The profile edit view that contains this setting list
     * @param aProfileName Profile name, will be modified
     * 
     * @return A new CProfileEditGeneralSettingList instance
     */
    static CProfileEditGeneralSettingList *NewL(CPuttyEngine &aPutty,
                                                CProfileEditView &aView,
                                                TDes &aProfileName);

    /** 
     * Destructor
     */
    ~CProfileEditGeneralSettingList();


private: // From CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void EditItemL(TInt aIndex, TBool aCalledFromMenu);

private: // Constructors
    CProfileEditGeneralSettingList(CPuttyEngine &aPutty,
                                   CProfileEditView &aView,
                                   TDes &aProfileName);
    void ConstructL();
    
    
private:
    TDes &iProfileName;
    TBuf<511> iHost;
    TBuf<99> iUsername;
#ifdef PUTTY_S60TOUCH
    TInt iPromptAP;
    TTouchSettings iTouchSettings;
#endif
    Config *iConfig;
};


#endif
