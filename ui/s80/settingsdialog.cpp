/*    settingsdialog.cpp
 *
 * Settings dialog
 *
 * Copyright 2003,2009 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikedwin.h>
#include <eikmfne.h>
#include <eikchlst.h>
#include <eikfsel.h>
#include <coeutils.h>
#include <eikbtgpc.h>
#include <ckndgopn.h>
#include <e32svr.h>
#include <eikbtgpc.h>
#include <eikchkbx.h>
#include <badesca.h>
#include <cknconf.h>
#include "puttyui.hrh"
#include <putty.rsg>
extern "C" {
#include "putty.h" // struct Config
}
#include "settingsdialog.h"
#include "puttyengine.h"
#include "palettes.h"

_LIT(KAssertPanic, "settingsdialog.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

_LIT8(KSmallFontName, "small");
_LIT8(KLargeFontName, "large");
static const TInt KNormalSmallWidth = 80;
static const TInt KNormalSmallHeight = 24;
static const TInt KNormalLargeWidth = 74;
static const TInt KNormalLargeHeight = 14;
static const TInt KFullSmallWidth = 106;
static const TInt KFullSmallHeight = 25;
static const TInt KFullLargeWidth = 91;
static const TInt KFullLargeHeight = 14;

static const TInt KSmallFontIndex = 0;
static const TInt KLargeFontIndex = 1;

// Cipher list when the user prefers Blowfish (default)
static const int KCiphersBlowfish[CIPHER_MAX] = {
    CIPHER_BLOWFISH,
    CIPHER_AES,
    CIPHER_3DES,
    CIPHER_WARN,
    CIPHER_ARCFOUR,
    CIPHER_DES
};

// Cipher list when the user prefers AES (default)
static const int KCiphersAes[CIPHER_MAX] = {
    CIPHER_AES,
    CIPHER_BLOWFISH,
    CIPHER_3DES,
    CIPHER_WARN,
    CIPHER_ARCFOUR,
    CIPHER_DES
};


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
CSettingsDialog::CSettingsDialog(TDes &aProfileName, TBool aIsDefault,
                                 Config *aConfig, CPuttyEngine *aPutty)
    : iProfileName(aProfileName),
      iIsDefault(aIsDefault) {
    iConfig = aConfig;
    iPutty = aPutty;
}


// Destructor
CSettingsDialog::~CSettingsDialog() {
    delete iPalettes;
}


// Dialog init, populates the dialog with current configuration
void CSettingsDialog::PreLayoutDynInitL() {

    HBufC *buf = HBufC::NewLC(256);
    TPtr ptr = buf->Des();

    // Profile
    CEikEdwin *profileEdwin = ((CEikEdwin*)Control(ESettingsProfile));
    profileEdwin->SetTextL(&iProfileName);
    profileEdwin->SetReadOnly(iIsDefault);
    if ( iIsDefault ) {
        SetLineDimmedNow(ESettingsProfile, ETrue);
    }

    // Hostname
    StringToDes(iConfig->host, ptr);
    ((CEikEdwin*)Control(ESettingsHost))->SetTextL(&ptr);

    // Port number
    ((CEikNumberEditor*)Control(ESettingsPort))->SetNumber(iConfig->port);

    // SSH version
    assert((iConfig->sshprot >= 0) && (iConfig->sshprot <= 3));
    ((CEikChoiceList*)Control(ESettingsSshVersion))
        ->SetCurrentItem(iConfig->sshprot);

    // Compression
    ((CEikCheckBox*)Control(ESettingsCompression))
        ->SetState(iConfig->compression ?
                   CEikCheckBox::ESet : CEikCheckBox::EClear);

    // Cipher
    if ( iConfig->ssh_cipherlist[0] == CIPHER_AES ) {
        ((CEikChoiceList*)Control(ESettingsSshCipher))->SetCurrentItem(1);
    } else {
        ((CEikChoiceList*)Control(ESettingsSshCipher))->SetCurrentItem(0);
    }

    // SSH Keepalive interval
    ((CEikNumberEditor*)Control(ESettingsKeepalive))
        ->SetNumber(iConfig->ping_interval);

    // Username
    StringToDes(iConfig->username, ptr);
    ((CEikEdwin*)Control(ESettingsUsername))->SetTextL(&ptr);

    // Private key file.
    // FIXME: This won't work with non-ASCII characters in the path!
    StringToDes(iConfig->keyfile.path, ptr);
    ((CEikEdwin*)Control(ESettingsPrivateKey))->SetTextL(&ptr);

    // Font
    TPtrC8 fontDes((const TUint8*)iConfig->font.name);
    TBool largeFont;
    CEikChoiceList *fontList = (CEikChoiceList*)Control(ESettingsFont);
    if ( fontDes.CompareF(KLargeFontName) == 0 ) {
        fontList->SetCurrentItem(KLargeFontIndex);
        largeFont = ETrue;
    } else {
        fontList->SetCurrentItem(KSmallFontIndex);
        largeFont = EFalse;
    }

    // Full screen
    CEikCheckBox::TState state = CEikCheckBox::EClear;
    if ( largeFont ) {
        if ( (iConfig->width == KFullLargeWidth) &&
             (iConfig->height == KFullLargeHeight) ) {
            state = CEikCheckBox::ESet;
        }
    } else {
        if ( (iConfig->width == KFullSmallWidth) &&
             (iConfig->height == KFullSmallHeight) ) {
            state = CEikCheckBox::ESet;
        }
    }
    ((CEikCheckBox*)Control(ESettingsFullScreen))->SetState(state);

    // Palette
    iPalettes = CPalettes::NewL(R_PUTTY_PALETTE_NAMES, R_PUTTY_PALETTES);
    TInt curpal = iPalettes->IdentifyPalette(
        (const unsigned char*) iConfig->colours);
    CEikChoiceList *palList = ((CEikChoiceList*)Control(ESettingsPalette));
    CDesCArrayFlat *arr = new (ELeave) CDesCArrayFlat(iPalettes->NumPalettes());
    for ( TInt i = 0; i < iPalettes->NumPalettes(); i++ ) {
        arr->AppendL(iPalettes->PaletteName(i));
    }
    palList->SetArrayL(arr);
    palList->SetCurrentItem(curpal);

    // Backspace key
    ((CEikChoiceList*)Control(ESettingsBackspace))->SetCurrentItem(
        iConfig->bksp_is_delete);

    // Character set
    // FIXME: This is the only thing we need the engine for -- consider another solution
    iCharSets = iPutty->SupportedCharacterSetsL();
    CEikChoiceList *csList = ((CEikChoiceList*)Control(ESettingsCharacterSet));
    StringToDes(iConfig->line_codepage, ptr);
    TInt curcs;
    if ( iCharSets->Find(ptr, curcs) != 0 ) {
        curcs = 0;
    }
    csList->SetArrayL(iCharSets);
    csList->SetCurrentItem(curcs);

    // Logging type
    assert((iConfig->logtype >= 0) && (iConfig->logtype <= 3));
    ((CEikChoiceList*)Control(ESettingsLogType))
        ->SetCurrentItem(iConfig->logtype);

    // Log file
    // FIXME: This won't work with non-ASCII characters in the path!
    StringToDes(iConfig->logfilename.path, ptr);
    ((CEikEdwin*)Control(ESettingsLogFile))->SetTextL(&ptr);

    CleanupStack::PopAndDestroy(); // buf

    // Disable delete button for the default profile
    if ( iIsDefault ) {
        ButtonGroupContainer().MakeCommandVisible(ECmdSettingsDelete, EFalse);
    }
    
    ButtonGroupContainer().SetDefaultCommand(EEikBidOk);
}


// Dialog close, reads dialog data and writes it to the configuration
TBool CSettingsDialog::OkToExitL(TInt aButtonId) {

    // Confirm profile deletion
    if ( aButtonId == ECmdSettingsDelete ) {
        if ( CCknConfirmationDialog::RunDlgLD(
                 R_STR_DELETE_PROFILE_CONFIRM_TITLE,
                 R_STR_DELETE_PROFILE_CONFIRM_TEXT) ) {
            return ETrue;
        }
        return EFalse;
    }

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

    // Profile
    if ( !iIsDefault ) {
        ((CEikEdwin*)Control(ESettingsProfile))->GetText(iProfileName);
    }

    // Hostname
    ((CEikEdwin*)Control(ESettingsHost))->GetText(ptr);
    DesToString(ptr, iConfig->host, sizeof(iConfig->host));

    // Port number
    iConfig->port = ((CEikNumberEditor*)Control(ESettingsPort))->Number();

    // SSH version
    iConfig->sshprot =
        ((CEikChoiceList*)Control(ESettingsSshVersion))->CurrentItem();
    assert((iConfig->sshprot >= 0) && (iConfig->sshprot <= 3));

    // Compression
    if ( ((CEikCheckBox*)Control(ESettingsCompression))->State() ==
         CEikCheckBox::ESet ) {
        iConfig->compression = 1;
    } else {
        iConfig->compression = 0;
    }

    // Cipher
    const int *ciphers = KCiphersBlowfish;
    if ( ((CEikChoiceList*)Control(ESettingsSshCipher))->CurrentItem() == 1 ) {
        ciphers = KCiphersAes;
    }
    Mem::Copy(iConfig->ssh_cipherlist, ciphers, sizeof(int)*CIPHER_MAX);

    // SSH Keepalive interval
    iConfig->ping_interval =
        ((CEikNumberEditor*)Control(ESettingsKeepalive))->Number();

    // Username
    ((CEikEdwin*)Control(ESettingsUsername))->GetText(ptr);
    DesToString(ptr, iConfig->username, sizeof(iConfig->username));

    // Private key file
    // FIXME: This won't work with non-ASCII characters in the path!
    ((CEikEdwin*)Control(ESettingsPrivateKey))->GetText(ptr);
    DesToString(ptr, iConfig->keyfile.path, sizeof(iConfig->keyfile.path));

    // Font
    TPtr8 fontPtr((TUint8*)iConfig->font.name, sizeof(iConfig->font.name));
    TBool largeFont = EFalse;
    switch ( ((CEikChoiceList*)Control(ESettingsFont))->CurrentItem() ) {
        case KSmallFontIndex:
            fontPtr = KSmallFontName;
            break;

        case KLargeFontIndex:
            fontPtr = KLargeFontName;
            largeFont = ETrue;
            break;
    }
    fontPtr.Append('\0');

    // Full screen
    if ( ((CEikCheckBox*)Control(ESettingsFullScreen))->State() ==
         CEikCheckBox::ESet ) {
        if ( largeFont ) {
            iConfig->width = KFullLargeWidth;
            iConfig->height = KFullLargeHeight;
        } else {
            iConfig->width = KFullSmallWidth;
            iConfig->height = KFullSmallHeight;
        }
    } else {
        if ( largeFont ) {
            iConfig->width = KNormalLargeWidth;
            iConfig->height = KNormalLargeHeight;
        } else {
            iConfig->width = KNormalSmallWidth;
            iConfig->height = KNormalSmallHeight;
        }
    }

    // Palette
    CEikChoiceList *palList = ((CEikChoiceList*)Control(ESettingsPalette));
    iPalettes->GetPalette(palList->CurrentItem(),
                          (unsigned char*) iConfig->colours);
    delete iPalettes;
    iPalettes = NULL;

    // Backspace key
    iConfig->bksp_is_delete =
        ((CEikChoiceList*)Control(ESettingsBackspace))->CurrentItem();

    // Character set
    CEikChoiceList *csList = ((CEikChoiceList*)Control(ESettingsCharacterSet));
    DesToString((*iCharSets)[csList->CurrentItem()], iConfig->line_codepage,
                sizeof(iConfig->line_codepage));    
    
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
}


// Called when a control loses focus. Changes the "Browse" button back to "OK"
void CSettingsDialog::PrepareForFocusTransitionL() {

    if ( iKeyLine ) {
        CEikButtonGroupContainer &buttons = ButtonGroupContainer();
        buttons.RemoveCommandFromStack(0, ECmdSettingsBrowseKeyFile);
        buttons.SetDefaultCommand(EEikBidOk);
        buttons.ButtonById(EEikBidOk)->DrawDeferred();
        iKeyLine = EFalse;
    }
}
