/*    profileedittoolbarsettinglist.h
 *
 * Putty profile edit view touch toolbar setting list
 *
 * Copyright 2009 Avila Risto
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITTOOLBARSETTINGLIST_H
#define PROFILEEDITTOOLBARSETTINGLIST_H

#include <coecntrl.h>
#include <aknsettingitemlist.h>
#include "profileeditsettinglistbase.h"
#include "touchuisettings.h"
extern "C" {
#include "putty.h" // struct Config
}

// Forward declarations
class CPuttyEngine;
class CProfileEditView;
class CPalettes;

/**
 * Setting list for display settings.
 */
class CProfileEditToolbarSettingList : public CProfileEditSettingListBase {

public:
    /** 
     * Factory method.
     * 
     * @param aPutty The PuTTY engine instance that contains the settings to
     *               be edited
     * @param aView The profile edit view that contains this setting list
     * @param aFonts The list of available fonts
     * 
     * @return A new CProfileEditToolbarSettingList instance
     */
    static CProfileEditToolbarSettingList *NewL(CPuttyEngine &aPutty,
                                                CProfileEditView &aView );

    /** 
     * Destructor
     */
    ~CProfileEditToolbarSettingList();


private: // From CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void EditItemL(TInt aIndex, TBool aCalledFromMenu);

private: // Constructors
    CProfileEditToolbarSettingList(CPuttyEngine &aPutty,
                                   CProfileEditView &aView);
    void ConstructL();
    
    
private:    
    TInt iToolBarButton1;
    TInt iToolBarButton2;
    TInt iToolBarButton3;
    TInt iToolBarButton4;
    TInt iToolBarButton5;
    TInt iToolBarButton6;
    TInt iToolBarButton7;
    TInt iToolBarButton8;
    TTouchSettings iTouchSettings;
    
};


#endif
