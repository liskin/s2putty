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


/**
 * Settings dialog. Prompts the user to enter various PuTTY configuration
 * settings. Uses the resource structure R_SETTINGS_DIALOG.
 */
class CSettingsDialog : public CEikDialog {
    
public:
    /** 
     * Constructor.
     * @param aConfig PuTTY configuration to use and modify.
     */
    CSettingsDialog(Config *aConfig);
    
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);
    void LineChangedL(TInt aControlId);
    void PrepareForFocusTransitionL();

private:
    Config *iConfig;
    TBool iKeyLine;
};


#endif
