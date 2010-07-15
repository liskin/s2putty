/*    profileeditdisplaysettinglist.h
 *
 * Putty profile edit view dosåöau setting list
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITTOUCHSETTINGLIST_H
#define PROFILEEDITTOUCHSETTINGLIST_H

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
class CProfileEditTouchSettingList : public CProfileEditSettingListBase {

public:
    /** 
     * Factory method.
     * 
     * @param aPutty The PuTTY engine instance that contains the settings to
     *               be edited
     * @param aView The profile edit view that contains this setting list
     * @param aFonts The list of available fonts
     * 
     * @return A new CProfileEditTouchSettingList instance
     */
    static CProfileEditTouchSettingList *NewL(CPuttyEngine &aPutty,
                                                CProfileEditView &aView );

    /** 
     * Destructor
     */
    ~CProfileEditTouchSettingList();


private: // From CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void EditItemL(TInt aIndex, TBool aCalledFromMenu);

private: // Constructors
    CProfileEditTouchSettingList(CPuttyEngine &aPutty,
                                   CProfileEditView &aView);
    void ConstructL();
    
    
private:
    TInt iTouchAllowMouseGrab;
    TInt iTouchClick;
    TInt iTouchDoubleClick;
    TInt iTouchLongClick;
    TInt iTouchSwipeLeft;
    TInt iTouchSwipeRight;
    TInt iTouchSwipeUp;
    TInt iTouchSwipeDown;
    
    TTouchSettings iTouchSettings;
    
};


#endif
