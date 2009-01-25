/*    settingsdialog.cpp
 *
 * Settings dialog
 *
 * Copyright 2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikedwin.h>
#include <eikmfne.h>
#include <eikchlst.h>
#include <coeutils.h>
#include <eikbtgpc.h>
#include <ckndgopn.h>
#include <e32svr.h>
#include <eikbtgpc.h>
#include "puttyui.hrh"
#include <putty.rsg>
extern "C" {
#include "putty.h" // struct Config
}
#include "settingsdialog.h"

_LIT(KAssertPanic, "settingsdialog.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))



// Converts a null-terminated string to a descriptor. Doesn't support anything
// except 7-bit ASCII.
// FIXME: Move to a separate source module in the UI?
static void StringToDes(const char *aStr, TDes &aTarget) {
    aTarget.SetLength(0);
    while ( *aStr ) {
        TChar c = *aStr++;
        if ( c > 0x7f ) {
            c = '?';
        }
        aTarget.Append(c);
    }
}

// Converts a descriptor to a null-terminated C string.
static void DesToString(const TDesC &aDes, char *aTarget, int targetLen) {
    int i = 0;
    int len = aDes.Length();
    assert(len < (targetLen-1));
    while ( i < len ) {
        TChar c = aDes[i];
        if ( c > 0x7f ) {
            c = '?';
        }
        *aTarget++ = (char) c;
        i++;
    }
    *aTarget = 0;
}



// Dialog constructor
CSettingsDialog::CSettingsDialog(Config *aConfig) {
    iConfig = aConfig;
}


// Dialog init, populates the dialog with current configuration
void CSettingsDialog::PreLayoutDynInitL() {

    HBufC *buf = HBufC::NewLC(256);
    TPtr ptr = buf->Des();

    // Hostname
    StringToDes(iConfig->host, ptr);
    ((CEikEdwin*)Control(ESettingsHost))->SetTextL(&ptr);

    // Port number
    ((CEikNumberEditor*)Control(ESettingsPort))->SetNumber(iConfig->port);

    // SSH version
    assert((iConfig->sshprot >= 0) && (iConfig->sshprot <= 3));
    ((CEikChoiceList*)Control(ESettingsSshVersion))
        ->SetCurrentItem(iConfig->sshprot);

    // Username
    StringToDes(iConfig->username, ptr);
    ((CEikEdwin*)Control(ESettingsUsername))->SetTextL(&ptr);

    // Private key file.
    // FIXME: This won't work with non-ASCII characters in the path!
    StringToDes(iConfig->keyfile.path, ptr);
    ((CEikEdwin*)Control(ESettingsPrivateKey))->SetTextL(&ptr);

    // Logging type
    assert((iConfig->logtype >= 0) && (iConfig->logtype <= 3));
    ((CEikChoiceList*)Control(ESettingsLogType))
        ->SetCurrentItem(iConfig->logtype);

    // Log file
    // FIXME: This won't work with non-ASCII characters in the path!
    StringToDes(iConfig->logfilename.path, ptr);
    ((CEikEdwin*)Control(ESettingsLogFile))->SetTextL(&ptr);

    CleanupStack::PopAndDestroy(); // buf
    
    ButtonGroupContainer().SetDefaultCommand(EEikBidOk);
}


// Dialog close, reads dialog data and writes it to the configuration
TBool CSettingsDialog::OkToExitL(TInt aButtonId) {

    // If the "Browse" button was pressed, just show a file selection dialog
    if ( aButtonId == ECmdSettingsBrowseKeyFile ) {
        TFileName *fileName = new TFileName;
        if ( CCknOpenFileDialog::RunSourceDlgLD(
                 *fileName, R_STR_KEY_FILE_DIALOG_TITLE) ) {
            ((CEikEdwin*)Control(ESettingsPrivateKey))->SetTextL(fileName);
        }
        if ( !iKeyLine ) {
            // Put the browse button back
            CEikButtonGroupContainer &buttons = ButtonGroupContainer();
            buttons.AddCommandToStackL(0, R_SETTINGS_BROWSE_KEY_BUTTON);
            buttons.SetDefaultCommand(ECmdSettingsBrowseKeyFile);
            buttons.ButtonById(ECmdSettingsBrowseKeyFile)->DrawDeferred();
            iKeyLine = ETrue;
        }
        delete fileName;
        return EFalse;
    }
    
    HBufC *buf = HBufC::NewLC(256);
    TPtr ptr = buf->Des();

    // Hostname
    ((CEikEdwin*)Control(ESettingsHost))->GetText(ptr);
    DesToString(ptr, iConfig->host, sizeof(iConfig->host));

    // Port number
    iConfig->port = ((CEikNumberEditor*)Control(ESettingsPort))->Number();

    // SSH version
    iConfig->sshprot =
        ((CEikChoiceList*)Control(ESettingsSshVersion))->CurrentItem();
    assert((iConfig->sshprot >= 0) && (iConfig->sshprot <= 3));

    // Username
    ((CEikEdwin*)Control(ESettingsUsername))->GetText(ptr);
    DesToString(ptr, iConfig->username, sizeof(iConfig->username));

    // Private key file
    // FIXME: This won't work with non-ASCII characters in the path!
    ((CEikEdwin*)Control(ESettingsPrivateKey))->GetText(ptr);
    DesToString(ptr, iConfig->keyfile.path, sizeof(iConfig->keyfile.path));
    
    // Logging type
    iConfig->logtype =
        ((CEikChoiceList*)Control(ESettingsLogType))->CurrentItem();
    assert((iConfig->logtype >= 0) && (iConfig->logtype <= 3));

    // Log file
    // FIXME: This won't work with non-ASCII characters in the path!
    ((CEikEdwin*)Control(ESettingsLogFile))->GetText(ptr);
    DesToString(ptr, iConfig->logfilename.path, sizeof(iConfig->logfilename.path));

    CleanupStack::PopAndDestroy(); // buf
    
    return ETrue;
}


// Called when the active dialog line changes. Shows a "Browse" CBA button
// when the private key line is active
void CSettingsDialog::LineChangedL(TInt aControlId) {

#if 0
    // FIXME
    CEikButtonGroupContainer &buttons = ButtonGroupContainer();
    if ( aControlId == ESettingsPrivateKey ) {
        if ( !iKeyLine ) {
            buttons.AddCommandToStackL(0, R_SETTINGS_BROWSE_KEY_BUTTON);
            buttons.SetDefaultCommand(ECmdSettingsBrowseKeyFile);
            buttons.ButtonById(ECmdSettingsBrowseKeyFile)->DrawDeferred();
            iKeyLine = ETrue;
        }
    } else {
        if ( iKeyLine ) {
            buttons.RemoveCommandFromStack(0, ECmdSettingsBrowseKeyFile);
            buttons.SetDefaultCommand(EEikBidOk);
            buttons.ButtonById(EEikBidOk)->DrawDeferred();
            iKeyLine = EFalse;
        }
    }
#endif
}


// Called when a control loses focus. Changes the "Browse" button back to "OK"
void CSettingsDialog::PrepareForFocusTransitionL() {

#if 0
    // FIXME
    if ( iKeyLine ) {
        CEikButtonGroupContainer &buttons = ButtonGroupContainer();
        buttons.RemoveCommandFromStack(0, ECmdSettingsBrowseKeyFile);
        buttons.SetDefaultCommand(EEikBidOk);
        buttons.ButtonById(EEikBidOk)->DrawDeferred();
        iKeyLine = EFalse;
    }
#endif
}
