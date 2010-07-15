/*    puttyappui.cpp
 *
 * Putty UI Application UI class
 *
 * Copyright 2003-2004 Sergei Khloupnov
 * Copyright 2002-2004,2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <bautils.h>
#include <aknnavi.h>
#include <badesca.h>
#include "puttyappui.h"
#include "profilelistview.h"
#include "terminalview.h"
#include "puttyuids.hrh"


_LIT(KProfileDirFormat, "%c:\\private\\%08x\\profiles\\");
_LIT(KDataDirFormat,  "%c:\\private\\%08x\\data\\");
_LIT(KFontDirFormat, "%c:\\resource\\puttyfonts\\");

// Previous PuTTY versions kept setting and host key files at
// "c:\system\apps\putty". We'll try to migrate them to the new locations.
_LIT(KOldSettingsFile, "c:\\system\\apps\\putty\\defaults");
_LIT(KOldHostKeysFile, "c:\\system\\apps\\putty\\hostkeys.dat");
_LIT(KNewDefaultProfileFile, "Default");
_LIT(KNewHostKeysFile, "hostkeys.dat");


// Second-phase constructor
void CPuttyAppUi::ConstructL() {
#ifdef PUTTY_S60V3
    #ifdef PUTTY_SYM3
        BaseConstructL(CAknAppUi::EAknEnableSkin | EAknTouchCompatible | EAknSingleClickCompatible);
    #else
        BaseConstructL(CAknAppUi::EAknEnableSkin);
    #endif
#else
    BaseConstructL();
#endif

    // Determine profile, data and font directories based on the executable
    // installation location. The files are on the same drive as the
    // executable, except if the exe is in ROM (z:), in which case profiles and
    // data use c:.
    TFileName name;
    name = RProcess().FileName();
    TParse parsa;
    parsa.SetNoWild(name, NULL, NULL);
    TUint drive = parsa.Drive()[0];

    // Font directory -- "<drv>:\resource\puttyfonts\"
    iFontDirectory.Format(KFontDirFormat, drive);

    // Fix drive for profiles and data
    if ( (drive == 'z') || (drive == 'Z') ) {
        drive = 'c';
    }

    // Data directory -- "<drv>:\private\<SID>\data\"
    // If the data directory doesn't exist, create it and attempt to migrate
    // host keys from a previous installation
    iDataDirectory.Format(KDataDirFormat, drive, RProcess().SecureId().iId);
    RFs &fs = CEikonEnv::Static()->FsSession();
    if ( !BaflUtils::FolderExists(fs, iDataDirectory) ) {
        BaflUtils::EnsurePathExistsL(fs, iDataDirectory);
        if ( BaflUtils::FileExists(fs, KOldHostKeysFile) ) {
            name = iDataDirectory;
            name.Append(KNewHostKeysFile);
            BaflUtils::CopyFile(fs, KOldHostKeysFile, name);
        }
    }

    // Profile directory -- "<drv>:\private\<SID>\profiles\"
    // If the profile directory doesn't exist, create it and attempt to migrate
    // default settings from a previous installation
    iProfileDirectory.Format(KProfileDirFormat, drive,
                             RProcess().SecureId().iId);
    if ( !BaflUtils::FolderExists(fs, iProfileDirectory) ) {
        BaflUtils::EnsurePathExistsL(fs, iProfileDirectory);
        if ( BaflUtils::FileExists(fs, KOldSettingsFile) ) {
            name = iProfileDirectory;
            name.Append(KNewDefaultProfileFile);
            BaflUtils::CopyFile(fs, KOldSettingsFile, name);
        }
    }

    // Create navi pane
    iNaviPane = (CAknNavigationControlContainer*)
        (StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));

    // Build a list of available fonts
    iFonts = new CDesC16ArrayFlat(8);
    CDir *dir;
    User::LeaveIfError(
        CEikonEnv::Static()->FsSession().GetDir(iFontDirectory,
                                                KEntryAttNormal,
                                                ESortByName, dir));
    CleanupStack::PushL(dir);
    for ( TInt i = 0; i < dir->Count(); i++ ) {
        parsa.SetNoWild((*dir)[i].iName, NULL, NULL);
        iFonts->AppendL(parsa.Name());
    }
    CleanupStack::PopAndDestroy(); //dir    

    // Build views
    iProfileListView = CProfileListView::NewL();
    AddViewL(iProfileListView);
    iProfileEditView = CProfileEditView::NewL();
    AddViewL(iProfileEditView);
    iTerminalView = CTerminalView::NewL();
    AddViewL(iTerminalView);

    // Start from the profile list view.
    SetDefaultViewL(*iProfileListView);
}


// Destructor
CPuttyAppUi::~CPuttyAppUi() {
    delete iFonts;
}


// Handle menu commands forwarded from views
void CPuttyAppUi::HandleCommandL(TInt aCommand) {

    switch (aCommand) {

        case EEikCmdExit:
        case EAknSoftkeyExit:
            // Exit
            Exit();
            break;
            
        default:
            break;
            User::Invariant();
    }    
}


// Activate profile list view
void CPuttyAppUi::ActivateProfileListViewL() {
    ActivateLocalViewL(TUid::Uid(KUidPuttyProfileListViewDefine));    
}


// Activate profile edit view
void CPuttyAppUi::ActivateProfileEditViewL(CPuttyEngine &aPutty,
                                           TDes &aProfileName) {
    iProfileEditPutty = &aPutty;
    iProfileEditName = &aProfileName;
    ActivateLocalViewL(TUid::Uid(KUidPuttyProfileEditViewDefine));    
}


// Get profile edit view data
void CPuttyAppUi::GetProfileEditDataL(CPuttyEngine *&aPutty,
                                      TDes *&aProfileName) {
    aPutty = iProfileEditPutty;
    aProfileName = iProfileEditName;
}


// Activate terminal view
void CPuttyAppUi::ActivateTerminalViewL(TDesC &aProfileFile) {
    iTerminalProfileFile = aProfileFile;
    ActivateLocalViewL(TUid::Uid(KUidPuttyTerminalViewDefine));
}


// Get connection profile file
const TDesC &CPuttyAppUi::TerminalProfileFile() {
    return iTerminalProfileFile;
}


// Get profile directory
const TDesC &CPuttyAppUi::ProfileDirectory() {
    return iProfileDirectory;
}

// Get data directory
const TDesC &CPuttyAppUi::DataDirectory() {
    return iDataDirectory;
}

// Get font directory
const TDesC &CPuttyAppUi::FontDirectory() {
    return iFontDirectory;
}


// Get navi pane
CAknNavigationControlContainer &CPuttyAppUi::NaviPane() {
    return *iNaviPane;
}


// Get fonts
const CDesCArray &CPuttyAppUi::Fonts() {
    return *iFonts;
}

