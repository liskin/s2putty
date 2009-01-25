/** @file settingsdialog.h
 *
 * Settings dialog
 *
 * Copyright 2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __SETTINGSDIALOG_H__
#define __SETTINGSDIALOG_H__

#include <e32std.h>
#include <eikdialg.h>
#include <eikcmobs.h>
extern "C" {
#include "putty.h" // struct Config
}


class CPuttyEngine;
class CPalettes;


/**
 * Settings dialog. Prompts the user to enter various PuTTY configuration
 * settings. Uses the resource structure R_SETTINGS_DIALOG.
 */
class CSettingsDialog : public CEikDialog {
    
public:
    /** 
     * Constructor.
     * @param aProfileName Profile name, modified if appropriate
     * @param aIsDefault ETrue if this is the default profile.
     *                   Default profile cannot be renamed or deleted.
     * @param aConfig PuTTY configuration to use and modify.
     * @param aPutty PuTTY engine instance to use
     */
    CSettingsDialog(TDes &aProfileName, TBool aIsDefault, Config *aConfig,
                    CPuttyEngine *aPutty);

    ~CSettingsDialog();    
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);
    void LineChangedL(TInt aControlId);
    void PrepareForFocusTransitionL();

private:
    Config *iConfig;
    CPuttyEngine *iPutty;
    TBool iKeyLine;
    CDesCArray *iCharSets;
    TDes &iProfileName;
    TBool iIsDefault;
    CPalettes *iPalettes;
};


#endif
