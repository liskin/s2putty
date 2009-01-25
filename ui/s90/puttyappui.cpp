/*    puttyappui.cpp
 *
 * Putty UI Application UI class
 *
 * Copyright 2002,2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikenv.h>
#include <eikon.hrh>
#include <ckninfo.h>
#include <cknconf.h>
#include <es_sock.h>
#include <eikmenup.h>
#include <eikdoc.h>
#include <ckndgopn.h>
#include <ckndgsve.h>
#include <eikmisdg.h>
#include <eikapp.h>
#include <apgwgnam.h>
#include <apgtask.h>

#include "puttyappui.h"
#include "puttyappview.h"
#include "puttyengine.h"
#include "puttyui.hrh"
#include "connectiondialog.h"
#include "settingsdialog.h"
#include "authdialog.h"
#include <putty.rsg>
extern "C" {
#include "putty.h"
}

const TInt KInitAudioLength = 32000;
const TInt KExitReason = -1;

_LIT(KAssertPanic, "puttyappui.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

_LIT(KRandomFile,"random.dat");
_LIT(KDefaultSettingsFile, "defaults");

#ifdef __WINS__
_LIT(KPuttyEngine, "puttyengine.dll");
_LIT(KTempParamFile, "c:\\puttystarttemp");
#else
_LIT(KPuttyEngine, "puttyengine.exe");
#endif

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


//#include <e32svr.h>
//#define DEBUGPRINT(x) RDebug::Print x
#define DEBUGPRINT(x)


CPuttyAppUi::CPuttyAppUi() : iAudioRecordDes(NULL, 0, 0) {
    iTermWidth = 80;
    iTermHeight = 24;
    iLargeFont = EFalse;
}


void CPuttyAppUi::ConstructL() {

    CEikonEnv::Static()->DisableExitChecks(ETrue);
    BaseConstructL();

    iFatalErrorPanic =
        CEikonEnv::Static()->AllocReadResourceL(R_STR_FATAL_ERROR);

    iDialler = CDialler::NewL(this);
    iRecorder = CAudioRecorder::NewL(this);
    
    // Determine application installation path
    HBufC *appDll = HBufC::NewLC(KMaxFileName);
    *appDll = iDocument->Application()->AppFullName();
    TParse parsa;
    User::LeaveIfError(parsa.SetNoWild(*appDll, NULL, NULL));
    iInstallPath = parsa.DriveAndPath();
    iDataPath = iInstallPath;

    // If the application is in ROM (drive Z:), put settings on C:
    if ( (iDataPath[0] == 'Z') || (iDataPath[0] == 'z') ) {
        iDataPath[0] = 'C';
    }

    // Make sure the path ends with a backslash
    if ( iDataPath[iDataPath.Length()-1] != '\\' ) {
        iDataPath.Append('\\');
    }
    DEBUGPRINT((_L("ProcessCommandParametersL: iDataPath = %S"), &iDataPath));

    // Build the terminal view
    iAppView = new (ELeave) CPuttyAppView(this, this);
    iAppView->ConstructL(iInstallPath, ClientRect());
    AddToStackL(iAppView);

    iAppView->Terminal()->SetGrayed(ETrue);

    // Check if the random seed already exists. If not, we are probably running
    // PuTTY for the first time and the generator should be seeded
    // We'll need to check this before initializing the engine, since it will
    // create the file at startup.
    HBufC *seedFileBuf = HBufC::NewLC(KMaxFileName);
    TPtr seedFile = seedFileBuf->Des();
    seedFile = iDataPath;
    seedFile.Append(KRandomFile);
    TBool randomExists = EFalse;
    TEntry dummy;
    if ( CEikonEnv::Static()->FsSession().Entry(seedFile, dummy)
         == KErrNone ) {
        randomExists = ETrue;
    }
    
    // Create and initialize the engine
    iEngine = CPuttyEngine::NewL(this, iDataPath);
    iEngine->SetTerminalSize(iTermWidth, iTermHeight);

    // Check if the default settings file exists. If yes, read it, otherwise
    // create one
    HBufC *settingsFileBuf = HBufC::NewLC(KMaxFileName);
    TPtr settingsFile = settingsFileBuf->Des();
    settingsFile = iDataPath;
    settingsFile.Append(KDefaultSettingsFile);
    if ( CEikonEnv::Static()->FsSession().Entry(settingsFile, dummy)
         == KErrNone ) {
        iEngine->ReadConfigFileL(settingsFile);
        ReadUiSettingsL(iEngine->GetConfig());
    } else {
        iEngine->SetDefaults();
        iEngine->WriteConfigFileL(settingsFile);
    }
        
    // Initialize the random number generator if necessary
    if ( !randomExists ) {
        
        DEBUGPRINT((_L("ProcessCommandParametersL: Initializing random number generator")));
        if ( CCknConfirmationDialog::RunDlgLD(R_STR_INITIAL_RANDOM_TITLE,
                                              R_STR_INITIAL_RANDOM_TEXT,
                                              NULL,
                                              R_STR_OK_CONFIRM) ) {
            HandleCommandL(ECmdInitRandomGenerator);
        }
    }

    CleanupStack::PopAndDestroy(3); // appDll, settingsFileBuf, seedFileBuf
}


CPuttyAppUi::~CPuttyAppUi() {
    if ( iAppView ) {
        RemoveFromStack(iAppView);
    }
    delete iAppView;
    delete iRecorder;
    delete iAudio;
    delete iDialler;
    delete iEngine;
    delete iFatalErrorPanic;
}


TBool CPuttyAppUi::ProcessCommandParametersL(TApaCommand aCommand,
                                             TFileName & aDocumentName,
                                             const TDesC8 &aTail) {

    // If we got a config file as a parameter, read it
    if ( aCommand == EApaCommandOpen ) {
        DEBUGPRINT((_L("ProcessCommandParametersL: Reading config file")));
        iEngine->ReadConfigFileL(aDocumentName);
    } else {
        // Try to load default settings
        
    }

    // Final hack, keep the system happy. This is necessary since we aren't
    // really a proper file-based application.
    DEBUGPRINT((_L("ProcessCommandParametersL: Init done")));
    return CEikAppUi::ProcessCommandParametersL(aCommand, aDocumentName,
                                                aTail);
}



void CPuttyAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {

        case ECmdConnect: {
            if ( iState != EStateNone ) {
                break;
            }
            assert(iEngine);
            Config *cfg = iEngine->GetConfig();

            THostName hostName;
            char *c = cfg->host;
            while ( *c ) {
                hostName.Append(*c++);
            }
            
            CConnectionDialog *dlg = new (ELeave) CConnectionDialog(hostName);
            
            if ( dlg->ExecuteLD(R_CONNECTION_DIALOG)) {
                c = cfg->host;
                for ( TInt i = 0; i < hostName.Length(); i++ ) {
                    *c++ = (char) hostName[i];
                }
                *c++ = 0;

                TRAPD(error, CEikonEnv::Static()->BusyMsgL(R_STR_DIALING));
                iState = EStateDialing;
                iDialler->DialL();
            }
            break;
        }

        case ECmdLargeFont: {
            if ( iLargeFont ) {
                iLargeFont = EFalse;
            } else {
                iLargeFont = ETrue;
            }
            iAppView->SetFontL(iLargeFont);
            break;
        }
        
        case ECmdFullScreen: {
            if ( iFullScreen ) {
                iFullScreen = EFalse;
            } else {
                iFullScreen = ETrue;
            }
            iAppView->SetFullScreenL(iFullScreen);
            break;
        }

        case ECmdInitRandomGenerator: {
            if ( iRecording ) {
                break;
            }
            
            if ( CCknConfirmationDialog::RunDlgLD(R_STR_RECORD_CONFIRM_TITLE,
                                                  R_STR_RECORD_CONFIRM_TEXT,
                                                  NULL,
                                                  R_STR_OK_CONFIRM) ) {
                delete iAudio;
                iAudio = NULL;
                iAudio = HBufC8::NewL(KInitAudioLength);
                iAudioRecordDes.Set(iAudio->Des());
                iRecorder->RecordL(iAudioRecordDes);
                iRecording = ETrue;
                CEikonEnv::Static()->BusyMsgL(R_STR_RECORDING);
            }
            
            break;
        }

        case ECmdSettings: {
            assert(iState == EStateNone);
            CSettingsDialog *dlg =
                new (ELeave) CSettingsDialog(iEngine->GetConfig());
            dlg->ExecuteLD(R_SETTINGS_DIALOG);
            break;
        }

        case ECmdLoadSettings: {
            assert(iState == EStateNone);
            TFileName fileName;
            TUid uid = { 0x101f9075 };
            if ( CCknOpenFileDialog::RunDlgLD(
                     fileName,
                     R_STR_SHOW_PUTTY_CONFIG_FILES,
                     CCknFileListDialogBase::EShowAllDrives,
                     uid) ) {
                iEngine->ReadConfigFileL(fileName);
                ReadUiSettingsL(iEngine->GetConfig());
            }
            break;
        }

        case ECmdSaveSettings: {
            assert(iState == EStateNone);
            TFileName fileName;
            if ( CCknSaveAsFileDialog::RunDlgLD(fileName) ) {
                WriteUiSettingsL(iEngine->GetConfig());
                iEngine->WriteConfigFileL(fileName);
            }
            break;
        }

        case ECmdSendSpecialCharacter:
            if ( iState == EStateConnected ) {
                CEikCharMapDialog::RunDlgLD(iAppView->Terminal());
            }
            break;

        case ECmdSendPipe:
            if ( iState == EStateConnected ) {
                iEngine->SendKeypress((TKeyCode)'|', 0);
            }
            break;
                
        case ECmdSendBackquote:
            if ( iState == EStateConnected ) {
                iEngine->SendKeypress((TKeyCode)'`', 0);
            }
            break;

        case ECmdSendF1:
        case ECmdSendF2:
        case ECmdSendF3:
        case ECmdSendF4:
        case ECmdSendF5:
        case ECmdSendF6:
        case ECmdSendF7:
        case ECmdSendF8:
        case ECmdSendF9:
        case ECmdSendF10:
            if ( iState == EStateConnected ) {
                iEngine->SendKeypress(
                    (TKeyCode) (EKeyF1 + (aCommand - ECmdSendF1)), 0);
            }
            break;

        case ECmdSaveSettingsAsDefault: {
            TFileName fileName;
            fileName = iDataPath;
            fileName.Append(KDefaultSettingsFile);
            WriteUiSettingsL(iEngine->GetConfig());
            iEngine->WriteConfigFileL(fileName);
            break;
        }

        case ECmdResetDefaultSettings: {
            iEngine->SetDefaults();
            ReadUiSettingsL(iEngine->GetConfig());
            HandleCommandL(ECmdSaveSettingsAsDefault);
            break;
        }

        case ECmdAbout: {
            CCknInfoDialog::RunDlgLD(
                *CEikonEnv::Static()->AllocReadResourceLC(R_STR_ABOUT_TITLE),
                *CEikonEnv::Static()->AllocReadResourceLC(R_STR_ABOUT_TEXT));
            CleanupStack::PopAndDestroy(2); // title, text
            break;
        }
                
	case EEikCmdExit: 
            Exit();
            break;
    }    
}


void CPuttyAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane) {    
    
    if ( aResourceId == R_PUTTY_VIEW_MENU ) {
        if ( iLargeFont ) {
            aMenuPane->SetItemButtonState(ECmdLargeFont, EEikMenuItemSymbolOn);
        } else {
            aMenuPane->SetItemButtonState(ECmdLargeFont,
                                          EEikMenuItemSymbolIndeterminate);
        }
        
        if ( iFullScreen ) {
            aMenuPane->SetItemButtonState(ECmdFullScreen,
                                          EEikMenuItemSymbolOn);
        } else {
            aMenuPane->SetItemButtonState(ECmdFullScreen,
                                          EEikMenuItemSymbolIndeterminate);
        }
        
    } else if ( aResourceId == R_PUTTY_SETTINGS_MENU ) {
        aMenuPane->SetItemDimmed(ECmdLoadSettings, (iState != EStateNone));
        aMenuPane->SetItemDimmed(ECmdSettings, (iState != EStateNone));
        aMenuPane->SetItemDimmed(ECmdSaveSettings, (iState != EStateNone));
        aMenuPane->SetItemDimmed(ECmdSaveSettingsAsDefault,
                                 (iState != EStateNone));
        aMenuPane->SetItemDimmed(ECmdResetDefaultSettings,
                                 (iState != EStateNone));
        
    } else if ( aResourceId == R_PUTTY_TOOLS_MENU ) {
        aMenuPane->SetItemDimmed(ECmdSendSpecialCharacter,
                                 (iState != EStateConnected));
        
    } else if ( aResourceId == R_PUTTY_SEND_CHARACTER_MENU ) {
        aMenuPane->SetItemDimmed(ECmdSendPipe,
                                 (iState != EStateConnected));
        aMenuPane->SetItemDimmed(ECmdSendBackquote,
                                 (iState != EStateConnected));
        
    } else if ( aResourceId == R_PUTTY_SEND_FUNCTION_KEY_MENU ) {
        TBool dimmed = (iState != EStateConnected);
        aMenuPane->SetItemDimmed(ECmdSendF1, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF2, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF3, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF4, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF5, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF6, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF7, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF8, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF9, dimmed);
        aMenuPane->SetItemDimmed(ECmdSendF10, dimmed);
    }
}


// Reads the UI settings (font size, full screen flag) from a config structure
void CPuttyAppUi::ReadUiSettingsL(Config *aConfig) {

    TPtrC8 fontDes((const TUint8*)aConfig->font.name);
    if ( fontDes.CompareF(KLargeFontName) == 0 ) {
        iLargeFont = ETrue;
    } else {
        iLargeFont = EFalse;
    }
    iAppView->SetFontL(iLargeFont);

    if ( iLargeFont ) {
        if ( (aConfig->width == KFullLargeWidth) &&
             (aConfig->height == KFullLargeHeight) ) {
            iFullScreen = ETrue;
        } else {
            iFullScreen = EFalse;
        }
    } else {
        if ( (aConfig->width == KFullSmallWidth) &&
             (aConfig->height == KFullSmallHeight) ) {
            iFullScreen = ETrue;
        } else {
            iFullScreen = EFalse;
        }
    }
    iAppView->SetFullScreenL(iFullScreen);
}


// Writes the UI settings (font size, full screen flag) to a config structure
void CPuttyAppUi::WriteUiSettingsL(Config *aConfig) {

    TPtr8 fontPtr((TUint8*)aConfig->font.name, sizeof(aConfig->font.name));

    if ( iLargeFont ) {
        fontPtr = KLargeFontName;
    } else {
        fontPtr = KSmallFontName;
    }
    fontPtr.Append('\0');

    if ( iLargeFont ) {
        if ( iFullScreen ) {
            aConfig->width = KFullLargeWidth;
            aConfig->height = KFullLargeHeight;
        } else {
            aConfig->width = KNormalLargeWidth;
            aConfig->height = KNormalLargeHeight;
        }
    } else {
        if ( iFullScreen ) {
            aConfig->width = KFullSmallWidth;
            aConfig->height = KFullSmallHeight;
        } else {
            aConfig->width = KNormalSmallWidth;
            aConfig->height = KNormalSmallHeight;
        }
    }
}


// MDialObserver::DialCompleted()
void CPuttyAppUi::DialCompleted(TInt aError) {

    assert(iState == EStateDialing);
    CEikonEnv *eenv = CEikonEnv::Static();    
    eenv->BusyMsgCancel();
    
    if ( aError != KErrNone ) {
        HBufC *msg = HBufC::NewLC(512);
        TPtr msgp = msg->Des();
        HBufC *err = HBufC::NewLC(128);
        TPtr errp = err->Des();
        
        eenv->GetErrorText(errp, aError);
        msgp.Format(*eenv->AllocReadResourceLC(R_STR_DIALUP_FAILED),
                    &errp);
        HBufC *title = eenv->AllocReadResourceLC(R_STR_CONNECTION_ERROR_TITLE);
        
        CCknInfoDialog::RunDlgLD(*title, msgp);
        CleanupStack::PopAndDestroy(4); // title, formatstring, err, msg

        iState = EStateNone;
        return;
    }

    TRAPD(error, eenv->BusyMsgL(R_STR_CONNECTING));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
    iState = EStateConnecting;

    TInt err = iEngine->Connect();
    if ( err != KErrNone ) {
        TBuf<128> msg;
        iEngine->GetErrorMessage(msg);
        ConnectionErrorL(msg);
        // FIXME: We should be in a state where we can exit more cleanly
        User::Exit(KExitReason);
    }

    iState = EStateConnected;
    eenv->BusyMsgCancel();
    iAppView->Terminal()->SetGrayed(EFalse);
}


// MRecordObserver::RecordCompleted()
void CPuttyAppUi::RecordCompleted(TInt aError) {

    CEikonEnv *eenv = CEikonEnv::Static();
    eenv->BusyMsgCancel();

    // If the audio device was reserved, prompt to try again
    if ( aError == KErrInUse ) {

        if ( CCknConfirmationDialog::RunDlgLD(R_STR_RECORD_ERROR_TITLE,
                                              R_STR_AUDIO_DEVICE_IN_USE,
                                              NULL,
                                              R_STR_OK_CONFIRM) ) {
            iRecorder->CancelRecord();
            iAudioRecordDes.SetLength(0);
            iRecorder->RecordL(iAudioRecordDes);
            iRecording = ETrue;
            CEikonEnv::Static()->BusyMsgL(R_STR_RECORDING);
            return;
        }
        
    } else if ( aError != KErrNone ) {
        
        // Handle other errors                    
        iRecorder->CancelRecord();
        
        HBufC *title =
            CEikonEnv::Static()->AllocReadResourceLC(R_STR_RECORD_ERROR_TITLE);

        HBufC *msg = HBufC::NewLC(512);
        TPtr msgp = msg->Des();
        HBufC *err = HBufC::NewLC(128);
        TPtr errp = err->Des();        
        eenv->GetErrorText(errp, aError);
        msgp.Format(*eenv->AllocReadResourceLC(R_STR_RECORD_FAILED),
                    &errp);
        
        CCknInfoDialog::RunDlgLD(*title, msgp);
        CleanupStack::PopAndDestroy(4); // formatstring, err, msg, title

    } else {
        // Use the data as random number seed noise.
        iEngine->AddRandomNoise(iAudioRecordDes);
        eenv->InfoMsg(R_STR_RANDOMIZED);
    }

    delete iAudio;
    iAudio = NULL;
    iRecording = EFalse;        
}


// MPuttyClient::DrawText()
void CPuttyAppUi::DrawText(TInt aX, TInt aY, const TDesC &aText,
                           TBool aBold, TBool aUnderline,
                           TRgb aForeground, TRgb aBackground) {
    
    iAppView->Terminal()->DrawText(aX, aY, aText, aBold, aUnderline,
                                   aForeground, aBackground);
}


// MPuttyClient::SetCursor()
void CPuttyAppUi::SetCursor(TInt aX, TInt aY) {
    iAppView->Terminal()->SetCursor(aX, aY);
}


// MPuttyClient::ConnectionError()
void CPuttyAppUi::ConnectionError(const TDesC &aMessage) {

    TRAPD(error, ConnectionErrorL(aMessage));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
    iAppView->Terminal()->SetGrayed(ETrue);
}

void CPuttyAppUi::ConnectionErrorL(const TDesC &aMessage) {

    HBufC *title =
        CEikonEnv::Static()->AllocReadResourceLC(R_STR_CONNECTION_ERROR_TITLE);
    CCknInfoDialog::RunDlgLD(*title, aMessage);
    CleanupStack::PopAndDestroy();
    User::Exit(KExitReason);
}


// MPuttyClient::FatalError()
void CPuttyAppUi::FatalError(const TDesC &aMessage) {

    TRAPD(error, FatalErrorL(aMessage));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
}

void CPuttyAppUi::FatalErrorL(const TDesC &aMessage) {

    HBufC *title =
        CEikonEnv::Static()->AllocReadResourceLC(R_STR_FATAL_ERROR_TITLE);
    CCknInfoDialog::RunDlgLD(*title, aMessage);
    CleanupStack::PopAndDestroy();
    User::Exit(KExitReason);
}


// MPuttyClient::ConnectionClosed()
void CPuttyAppUi::ConnectionClosed() {
    CEikonEnv::Static()->InfoMsg(R_STR_CONNECTION_CLOSED);
    iAppView->Terminal()->SetGrayed(ETrue);
    iState = EStateDisconnected;
}


// MPuttyClient::UnknownHostKey()
//THostKeyResponse CPuttyAppUi::UnknownHostKey(const TDesC &aFingerprint) {
MPuttyClient::THostKeyResponse CPuttyAppUi::UnknownHostKey(
    const TDesC &aFingerprint) {
    
    MPuttyClient::THostKeyResponse resp = EAbadonConnection;
    TRAPD(error, resp = HostKeyDialogL(aFingerprint,
                                       R_STR_UNKNOWN_HOST_KEY_TITLE,
                                       R_STR_UNKNOWN_HOST_KEY_DLG_FMT));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
    return resp;
}


// MPuttyClient::DifferentHostKey()
MPuttyClient::THostKeyResponse CPuttyAppUi::DifferentHostKey(
    const TDesC &aFingerprint) {
    
    MPuttyClient::THostKeyResponse resp = EAbadonConnection;
    TRAPD(error, resp = HostKeyDialogL(aFingerprint,
                                       R_STR_DIFFERENT_HOST_KEY_TITLE,
                                       R_STR_DIFFERENT_HOST_KEY_DLG_FMT));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
    return resp;
}


MPuttyClient::THostKeyResponse CPuttyAppUi::HostKeyDialogL(
    const TDesC &aFingerprint, TInt aDialogTitleRes, TInt aDialogFormatRes) {
    CEikonEnv *env = CEikonEnv::Static();

    HBufC *title = env->AllocReadResourceLC(aDialogTitleRes);
    HBufC *fmt = env->AllocReadResourceLC(aDialogFormatRes);

    HBufC *contents = HBufC::NewLC(fmt->Length() + aFingerprint.Length());
    contents->Des().Format(*fmt, &aFingerprint);

    TInt res = CCknConfirmationDialog::RunDlgLD(
        *title, *contents, R_UNKNOWN_HOST_KEY_DLG_BUTTONS);

    MPuttyClient::THostKeyResponse resp = EAbadonConnection;

    switch ( res ) {
        case ECmdHostKeyAcceptAndSave:
            resp = EAcceptAndStore;
            break;
        
        case ECmdHostKeyAcceptOnce:
            resp = EAcceptTemporarily;
            break;

        case ECmdHostKeyReject:
            resp = EAbadonConnection;
            break;

        default:
            assert(EFalse);
    }

    CleanupStack::PopAndDestroy(3); // contents, fmt, title

    return resp;
}


// MPuttyClient::AcceptCipher()
TBool CPuttyAppUi::AcceptCipher(const TDesC &aCipherName,
                                TCipherDirection aDirection) {
    TBool resp = EFalse;
    TRAPD(error, resp = AcceptCipherL(aCipherName,
                                      aDirection));
    if ( error != KErrNone ) {
        User::Panic(*iFatalErrorPanic, error);
    }
    return resp;
}

TBool CPuttyAppUi::AcceptCipherL(const TDesC &aCipherName,
                                 TCipherDirection aDirection) {
    
    CEikonEnv *env = CEikonEnv::Static();

    HBufC *title = env->AllocReadResourceLC(R_STR_ACCEPT_CIPHER_TITLE);
    HBufC *fmt = env->AllocReadResourceLC(R_STR_ACCEPT_CIPHER_DLG_FMT);
    HBufC *dir = NULL;

    switch ( aDirection ) {
        case EBothDirections:
            dir = env->AllocReadResourceLC(R_STR_ACCEPT_CIPHER_DIR_BOTH);
            break;

        case EClientToServer:
            dir = env->AllocReadResourceLC(
                R_STR_ACCEPT_CIPHER_CLIENT_TO_SERVER);
            break;

        case EServerToClient:
            dir = env->AllocReadResourceLC(
                R_STR_ACCEPT_CIPHER_SERVER_TO_CLIENT);
            break;

        default:
            assert(EFalse);
    }

    HBufC *contents = HBufC::NewLC(fmt->Length() + aCipherName.Length() +
                                   dir->Length());
    contents->Des().Format(*fmt, &aCipherName, dir);

    TBool res = CCknConfirmationDialog::RunDlgLD(*title, *contents);

    CleanupStack::PopAndDestroy(4); // contents, fir, fmt, title

    return res;
}


// MPuttyClient::AuthenticationPrompt
TBool CPuttyAppUi::AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                                        TBool aSecret) {
    TBool res = EFalse;
    TRAPD(err,
          res = CAuthenticationDialog::DoPromptL(aPrompt, aTarget, aSecret));
    if ( err != KErrNone ) {
        User::Panic(*iFatalErrorPanic, err);
    }
    return res;
}


// MTerminalObserver::TerminalSizeChanged()
void CPuttyAppUi::TerminalSizeChanged(TInt aWidth, TInt aHeight) {
    assert((aWidth > 1) && (aHeight > 1));
    iTermWidth = aWidth;
    iTermHeight = aHeight;
    if ( iEngine ) {
        iEngine->SetTerminalSize(aWidth, aHeight);
    }
}


// MTerminalObserver::KeyPressed()
void CPuttyAppUi::KeyPressed(TKeyCode aCode, TUint aModifiers) {
    if ( iEngine ) {
        iEngine->SendKeypress(aCode, aModifiers);
    }
}


// MTerminalObserver::RePaintWindow()
void CPuttyAppUi::RePaintWindow() {
    if ( iEngine ) {
        iEngine->RePaintWindow();
    }
}
