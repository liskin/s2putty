/*    profileedittoolbarsettinglist.h
 *
 * Putty profile edit view touch toolbar setting list
 *
 * Copyright 2009 Avila Risto
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITGENERALTOOLBARSETTINGLIST_H
#define PROFILEEDITGENERALTOOLBARSETTINGLIST_H

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
class CProfileEditGeneralToolbarSettingList : public CProfileEditSettingListBase {

public:
    /** 
     * Factory method.
     * 
     * @param aPutty The PuTTY engine instance that contains the settings to
     *               be edited
     * @param aView The profile edit view that contains this setting list
     * @param aFonts The list of available fonts
     * 
     * @return A new CProfileEditGeneralToolbarSettingList instance
     */
    static CProfileEditGeneralToolbarSettingList *NewL(CPuttyEngine &aPutty,
                                                CProfileEditView &aView );

    /** 
     * Destructor
     */
    ~CProfileEditGeneralToolbarSettingList();


private: // From CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void EditItemL(TInt aIndex, TBool aCalledFromMenu);

private: // Constructors
    CProfileEditGeneralToolbarSettingList(CPuttyEngine &aPutty,
                                   CProfileEditView &aView);
    void ConstructL();
    
    
private:    
    TInt iToolBarButtonCount;
    TInt iShowToolbar;
    TInt iToolBarButtonWidth;
    TInt iToolBarButtonHeigth;

    TInt iButtonUpBackgroundTransparency;
    TInt iButtonUpTextTransparency;
    TInt iButtonDownBackgroundTransparency;
    TInt iButtonDownTextTransparency;
    TInt iButtonFontSize;
    TTouchSettings iTouchSettings;
    
};


#endif
