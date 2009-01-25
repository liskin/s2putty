/*    filelistdialog.cpp
 *
 * A file selection dialog
 *
 * Copyright 2004 Sergei Khloupnov
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "filelistdialog.h"
#include <coemain.h>
#include <putty.rsg>
#include <f32file.h>

_LIT(KFDParentDir, "../");
_LIT(KFDPhoneRoot, "c:\\");
_LIT(KFDCardRoot, "e:\\");

#define KFDMaskFilesOnly KEntryAttNormal
#define KFDMaskFilesAndDirs KEntryAttMaskSupported

_LIT(KAssertPanic, "filelistdialog.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


CFileListDialogWrapper *CFileListDialogWrapper::NewL(TInt *aIndex,
                                                     TFileName *aFile,
                                                     TBool aNewFile) {
    CFileListDialogWrapper *self =
        CFileListDialogWrapper::NewLC(aIndex, aFile, aNewFile);
    CleanupStack::Pop();
    return self;
}


CFileListDialogWrapper *CFileListDialogWrapper::NewLC(TInt *aIndex,
                                                      TFileName *aFile,
                                                      TBool aNewFile) {
    CFileListDialogWrapper *self =
        new (ELeave) CFileListDialogWrapper(aIndex, aFile, aNewFile);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


CFileListDialogWrapper::CFileListDialogWrapper(TInt *aIndex, TFileName *aFile,
                                               TBool aNewFile)
    : CAknListQueryDialog(aIndex) {
    iIndex = aIndex;
    iResult = aFile;
    iNewFile = aNewFile;
}


void CFileListDialogWrapper::ConstructL() {
    iPath.Copy(*iResult);
    iDirectoryEntries = new (ELeave) CDesCArrayFlat(16);
}


CFileListDialogWrapper::~CFileListDialogWrapper() {	
    //delete iDirectoryEntries; // apparently CAknListQueryDialog code takes care of it
}


TBool CFileListDialogWrapper::OkToExitL(TInt aKeycode) {
    // Called when dialog is finished
    // Get selected item and store it at iResult place

    CAknListQueryDialog::OkToExitL(aKeycode);

    CEikListBox *listBox = ListBox();
    TInt selected = listBox->CurrentItemIndex();
    TPtrC item = listBox->Model()->MatchableTextArray()->MdcaPoint(selected);

    iResult->Copy(item);
    return(ETrue);
}


void CFileListDialogWrapper::PreLayoutDynInitL() {

    CCoeEnv *coe = CCoeEnv::Static();
    RFs &fs = coe->FsSession();
	
    CAknListQueryDialog::PreLayoutDynInitL();

    // Populate listbox with dir entries
    // Dirs ending with "/", files not

    CDir *dirList = 0;
    TInt i;

    iDirectoryEntries->Reset();

    // Add "new file" selection as the first one
    if ( iNewFile ) {
        iDirectoryEntries->AppendL(
            *(coe->AllocReadResourceLC(R_STR_FILE_LIST_NEW_FILE)));
        CleanupStack::PopAndDestroy();
    }

    // Check if we are at the root directory. If yes, make the parent directory
    // the first entry. Otherwise list the drives if the device has a memory
    // card
    if ( iPath.Locate('\\') != (iPath.Length() - 1) ) {
        iDirectoryEntries->AppendL(KFDParentDir);
    } else {
        TDriveList drvList;
        User::LeaveIfError(fs.DriveList(drvList));
        if ( drvList[EDriveC] && drvList[EDriveE] ) {
            iDirectoryEntries->AppendL(
                *(coe->AllocReadResourceLC(R_STR_FILE_LIST_PHONE_MEMORY)));
            CleanupStack::PopAndDestroy();
            iDirectoryEntries->AppendL(
                *(coe->AllocReadResourceLC(R_STR_FILE_LIST_MEMORY_CARD)));
            CleanupStack::PopAndDestroy();
        }
    }

    // Directories, add a trailing slash, and add to the list
    User::LeaveIfError(fs.GetDir(iPath, KEntryAttMatchExclusive|KEntryAttDir,
                                 ESortByName, dirList));
    CleanupStack::PushL(dirList);
    for ( i = 0; i < dirList->Count(); i++ ) {
        TFileName fileName = (*dirList)[i].iName;
        fileName.Append('/');
        iDirectoryEntries->AppendL(fileName);
    }
    CleanupStack::PopAndDestroy(); // dirlist

    // List only files and add to the list
    User::LeaveIfError(fs.GetDir(iPath, KEntryAttNormal, ESortByName,
                                 dirList));
    CleanupStack::PushL(dirList);
    for ( i = 0; i < dirList->Count(); i++ ) {
        iDirectoryEntries->AppendL((*dirList)[i].iName);
    }
    CleanupStack::PopAndDestroy(); // dirlist

    SetItemTextArray(iDirectoryEntries); // takes ownership
}



TBool CFileListDialog::RunDlgLD(TDes &aPath, TBool aCreateFile) {

    TInt index(0);
    TInt returnCode(EFalse);
    TBool done(EFalse);
    TFileName path; // current directory, always with a trailing slash
    TFileName tmp;

    CCoeEnv *coe = CCoeEnv::Static();    
    RFs &fsSession = coe->FsSession();

    // Start from the default path
#ifdef EKA2
    //FIXME: Use system file dialogs in S60 v2+
    path = _L("c:\\");
#else
    User::LeaveIfError(fsSession.DefaultPath(path));
#endif

    // Check if a valid start directory was set. If yes, start from that.
    if ( aPath.Length() > 0  ) {
        TUint attributes;
        tmp.Copy(aPath);        
        // Trim filenames from the path until a valid directory is found
        while ( fsSession.Att(tmp, attributes) == KErrNone ) {
            if ( attributes & KEntryAttDir ) {
                path.Copy(tmp);
                break;
            } else {
                tmp = tmp.Left( tmp.LocateReverse('\\') + 1 );
            }
        }
    }

    // Load strings for comparisons
    HBufC *phoneDrive = coe->AllocReadResourceLC(R_STR_FILE_LIST_PHONE_MEMORY);
    HBufC *cardDrive = coe->AllocReadResourceLC(R_STR_FILE_LIST_MEMORY_CARD);
    HBufC *newFile = coe->AllocReadResourceLC(R_STR_FILE_LIST_NEW_FILE);

    // File selection main loop. We'll run a fresh instance of the selection
    // dialog in the current directory, moving to a new directory as
    // requested, until a selection has been made
    while ( !done ) {

        tmp.Copy(path);
        CFileListDialogWrapper *wrappedDialog =
            CFileListDialogWrapper::NewL(&index, &tmp, aCreateFile);

        if ( wrappedDialog->ExecuteLD(R_FILE_SELECTION_DIALOG) ) {
            // User selected an entry, follow it

            if ( tmp.Compare(*phoneDrive) == 0 ) {
                // Phone memory root
                path = KFDPhoneRoot;
                
            } else if ( tmp.Compare(*cardDrive) == 0 ) {
                // Memory card root
                path = KFDCardRoot;
                
            } else if ( tmp.Compare(KFDParentDir) == 0 ) {
                // Parent directory
                assert(path.Length() > 1);
                path.SetLength(path.Length()-1); // remove trailing slash
                TInt slash = path.LocateReverse('\\');
                assert(slash != KErrNotFound);
                path.SetLength(slash + 1);

            } else if ( tmp.Compare(*newFile) == 0 ) {
                // Create a new file
                tmp.Zero();
                CAknTextQueryDialog *dlg = CAknTextQueryDialog::NewL(tmp);
                HBufC *prompt = coe->AllocReadResourceLC(R_STR_NEW_FILE_NAME);		
                if( dlg->ExecuteLD(R_NEW_FILE_DIALOG, *prompt) ) {
                    aPath.Copy(path);
                    aPath.Append(tmp);
                    returnCode = ETrue;
                    done = ETrue;
                }
                CleanupStack::PopAndDestroy(); // prompt
                
            } else if ( tmp[tmp.Length()-1] == '/' ) {
                // The entry has a trailing slash -- directory selected
                // No trailing slash -- file selected
                tmp.SetLength(tmp.Length()-1);
                path.Append(tmp);
                path.Append('\\');
                
            } else {
                // File selected
                aPath.Copy(path);
                aPath.Append(tmp);
                returnCode = ETrue;
                done = ETrue;
            }

        } else {                      
            // Dialog cancelled
            done = ETrue;
            returnCode = EFalse;

        }
        wrappedDialog = NULL;
    }

    CleanupStack::PopAndDestroy(3); // newFile, cardDrive, phoneDrive

    return returnCode;
}
