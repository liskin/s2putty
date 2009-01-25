/*    profileeditdisplaysettinglist.h
 *
 * Putty profile edit view dosåöau setting list
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITDISPLAYSETTINGLIST_H
#define PROFILEEDITDISPLAYSETTINGLIST_H

#include <coecntrl.h>
#include <aknsettingitemlist.h>
#include "profileeditsettinglistbase.h"
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
class CProfileEditDisplaySettingList : public CProfileEditSettingListBase {

public:
    /** 
     * Factory method.
     * 
     * @param aPutty The PuTTY engine instance that contains the settings to
     *               be edited
     * @param aView The profile edit view that contains this setting list
     * @param aFonts The list of available fonts
     * 
     * @return A new CProfileEditDisplaySettingList instance
     */
    static CProfileEditDisplaySettingList *NewL(CPuttyEngine &aPutty,
                                                CProfileEditView &aView,
                                                const CDesCArray &aFonts);

    /** 
     * Destructor
     */
    ~CProfileEditDisplaySettingList();


private: // From CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void EditItemL(TInt aIndex, TBool aCalledFromMenu);

private: // Constructors
    CProfileEditDisplaySettingList(CPuttyEngine &aPutty,
                                   CProfileEditView &aView,
                                   const CDesCArray &aFonts);
    void ConstructL();
    
    
private:
    Config *iConfig;
    const CDesCArray &iFonts;
    TInt iFontValue;
    TBool iFullScreen;
    CDesCArray *iCharSets; // owned by the setting item
    CPalettes *iPalettes;
    TInt iCharSetValue;
    TInt iPaletteValue;
};


#endif
