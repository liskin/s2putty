/*    touchuisettings.cpp
 *
 * Putty touch ui settings file
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include <eikdef.h>
#include <aknview.h>
#include <aknviewappui.h>

#include "touchuisettings.h"
#include "customtoolbar.h"
#include "puttyui.hrh"

_LIT(KTouchSettingFile,"touchuisettings.dat");
_LIT(KDataDirFormat,  "%c:\\private\\%08x\\data\\");

const TInt KTouchUiSettingsVersion = 4;

TTouchSettings::TTouchSettings() {
    TFileName name;
    name = RProcess().FileName();
    TParse parsa;
    parsa.SetNoWild(name, NULL, NULL);
    TUint drive = parsa.Drive()[0];

    // Fix drive for profiles and data
    if ( (drive == 'z') || (drive == 'Z') ) {
       drive = 'c';
    }

    iDataDirectory.Format(KDataDirFormat, drive, RProcess().SecureId().iId);
    iSettingFile = iDataDirectory;
    iSettingFile.Append(KTouchSettingFile);
    
    SetDefault();
}

TTouchSettings::~TTouchSettings() {

}

TBool TTouchSettings::TestIfToolBarActionAllreadySet(TInt aCmd) {
    TBool ret = EFalse;
    
    if (itbButton1 == aCmd) {
        ret = ETrue;
    }
    if (itbButton2 == aCmd) {
        ret = ETrue;
    }
    if (itbButton3 == aCmd) {
        ret = ETrue;
    }
    if (itbButton4 == aCmd) {
        ret = ETrue;
    }
    if (itbButton5 == aCmd) {
        ret = ETrue;
    }
    if (itbButton6 == aCmd) {
        ret = ETrue;
    }
    if (itbButton7 == aCmd) {
        ret = ETrue;
    }
    if (itbButton8 == aCmd) {
        ret = ETrue;
    }
    return ret;
}

TBool TTouchSettings::SwapButtons(TInt aButton, TInt aCmd) {

    TInt *ButtonToSwap = NULL;
    TInt *ButtonHasCmd = NULL;
    switch (aButton) {
        case 0:
            ButtonToSwap = &itbButton1;
            break;
        case 1:
            ButtonToSwap = &itbButton2;
            break;
        case 2:
            ButtonToSwap = &itbButton3;
            break;
        case 3:
            ButtonToSwap = &itbButton4;
            break;
        case 4:
            ButtonToSwap = &itbButton5;
            break;
        case 5:
            ButtonToSwap = &itbButton6;
            break;
        case 6:
            ButtonToSwap = &itbButton7;
            break;
        case 7:
            ButtonToSwap = &itbButton8;
            break;
            
    }
    
    if (itbButton1 == aCmd) {
        ButtonHasCmd = &itbButton1;   
    } else if (itbButton2 == aCmd) {
        ButtonHasCmd = &itbButton2;   
    } else if (itbButton3 == aCmd) {
        ButtonHasCmd = &itbButton3;   
    } else if (itbButton4 == aCmd) {
        ButtonHasCmd = &itbButton4;   
    } else if (itbButton5 == aCmd) {
        ButtonHasCmd = &itbButton5;   
    } else if (itbButton6 == aCmd) {
        ButtonHasCmd = &itbButton6;   
    } else if (itbButton7 == aCmd) {
        ButtonHasCmd = &itbButton7;    
    } else if (itbButton8 == aCmd) {
        ButtonHasCmd = &itbButton8;    
    } else {
        return EFalse;
    }
    TInt tmp = *ButtonToSwap;
    *ButtonToSwap = *ButtonHasCmd;
    *ButtonHasCmd = tmp;
    return ETrue;
}

void TTouchSettings::SetDefault() {
    iShowtoolbar = 1; //1 show, 0 no show
    iSingleTap = EPuttyCmdStartVKB;
    iDoubleTap = EPuttyCmdSend;
    iLongTap = EPuttyCmdOpenPopUpMenu;
    iSwipeLeft = EPuttyCmdToggleToolbar;
    iSwipeRight = EPuttyCmdToggleToolbar;
    iSwipeUp = EPuttyCmdSendPageUp;
    iSwipeDown = EPuttyCmdSendPageDown;

    itbButton1 = EPuttyToolbarTab;
    itbButton2 = EPuttyToolbarAltP;
    itbButton3 = EPuttyToolbarCtrlP;
    itbButton4 = EPuttyToolbarPipe;
    itbButton5 = EPuttyToolbarLock;
    itbButton6 = EPuttyToolbarSelect;
    itbButton7 = EPuttyToolbarCopy;
    itbButton8 = EPuttyToolbarPaste;
    
    itbButtonCount = 8;
    itbButtonWidth = 60;
    itbButtonHeigth = 60;
    
    //These are up values 
    iButtonUpBackgroundTransparency = 179;
    iButtonUpTextTransparency = 250;
    iButtonDownBackgroundTransparency = 59;
    iButtonDownTextTransparency = 148;
    iButtonFontSize = 0;
    
    ReadSettingFileL();
}

void TTouchSettings::ReadSettingFileL() {
    CEikonEnv *eikenv = CEikonEnv::Static();
    // connect to the file server
    RFs fs = eikenv->FsSession();
    User::LeaveIfError(fs.Connect());
    
    if ( !BaflUtils::FileExists(fs, iSettingFile) ) {
        fs.Close();
        WriteSettingFileL(); // Lets write new setting file
        return;
    }
   
    RFileReadStream fRead;
    User::LeaveIfError( fRead.Open( fs, iSettingFile, EFileRead ) );
    CleanupClosePushL(fRead);
    // Write to current file position: start of file
    TInt iVersion = fRead.ReadInt16L();
    if ( iVersion != KTouchUiSettingsVersion ) {
        CleanupStack::PopAndDestroy(&fRead);
        fs.Close();
        WriteSettingFileL(); // Write defaults if version number doesn't match
        return;
    }
    iShowtoolbar = fRead.ReadInt16L();
    iSingleTap = fRead.ReadInt16L();
    iDoubleTap = fRead.ReadInt16L();
    iLongTap = fRead.ReadInt16L();
    iSwipeLeft = fRead.ReadInt16L();
    iSwipeRight = fRead.ReadInt16L();
    iSwipeUp = fRead.ReadInt16L();
    iSwipeDown = fRead.ReadInt16L();
    
    itbButton1 = fRead.ReadInt16L();
    itbButton2 = fRead.ReadInt16L();
    itbButton3 = fRead.ReadInt16L();
    itbButton4 = fRead.ReadInt16L();
    itbButton5 = fRead.ReadInt16L();
    itbButton6 = fRead.ReadInt16L();
    itbButton7 = fRead.ReadInt16L();
    itbButton8 = fRead.ReadInt16L();
    
    itbButtonCount = fRead.ReadInt16L();
    itbButtonWidth = fRead.ReadInt16L();
    itbButtonHeigth = fRead.ReadInt16L();
    iButtonUpBackgroundTransparency = fRead.ReadInt16L();
    iButtonUpTextTransparency = fRead.ReadInt16L();
    iButtonDownBackgroundTransparency = fRead.ReadInt16L();
    iButtonDownTextTransparency = fRead.ReadInt16L();
    iButtonFontSize = fRead.ReadInt16L(); 
    
    CleanupStack::PopAndDestroy(&fRead);

    fs.Close();
}

void TTouchSettings::WriteSettingFileL() {
    CEikonEnv *eikenv = CEikonEnv::Static();
    // connect to the file server
    RFs fs = eikenv->FsSession();
    User::LeaveIfError(fs.Connect());

    fs.Delete(iSettingFile); //allways create new file
    RFile myFile;
    
    RFileWriteStream fWrite;
    
    User::LeaveIfError( fWrite.Create( fs, iSettingFile, EFileWrite ) );
    
    CleanupClosePushL(fWrite);
    // Write to current file position: start of file
    // first write setting file version
    fWrite.WriteInt16L(KTouchUiSettingsVersion);
    // then start writing settings
    fWrite.WriteInt16L(iShowtoolbar);
    fWrite.WriteInt16L(iSingleTap);
    fWrite.WriteInt16L(iDoubleTap);
    fWrite.WriteInt16L(iLongTap);
    fWrite.WriteInt16L(iSwipeLeft);
    fWrite.WriteInt16L(iSwipeRight);
    fWrite.WriteInt16L(iSwipeUp);
    fWrite.WriteInt16L(iSwipeDown);
    
    fWrite.WriteInt16L(itbButton1);
    fWrite.WriteInt16L(itbButton2);
    fWrite.WriteInt16L(itbButton3);
    fWrite.WriteInt16L(itbButton4);
    fWrite.WriteInt16L(itbButton5);
    fWrite.WriteInt16L(itbButton6);
    fWrite.WriteInt16L(itbButton7);
    fWrite.WriteInt16L(itbButton8);
    
    fWrite.WriteInt16L(itbButtonCount);
    fWrite.WriteInt16L(itbButtonWidth);
    fWrite.WriteInt16L(itbButtonHeigth);  

    fWrite.WriteInt16L(iButtonUpBackgroundTransparency);
    fWrite.WriteInt16L(iButtonUpTextTransparency);
    fWrite.WriteInt16L(iButtonDownBackgroundTransparency);
    fWrite.WriteInt16L(iButtonDownTextTransparency);
    fWrite.WriteInt16L(iButtonFontSize);
    
    fWrite.CommitL();
    CleanupStack::PopAndDestroy(&fWrite);
   
    fs.Close();
}
