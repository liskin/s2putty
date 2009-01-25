/*    connectiondialog.cpp
 *
 * Connection open dialog
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikedwin.h>
#include <eikbtgpc.h>
#include <eiktxlbx.h> //CEikTextListBox
#include <eiktxlbm.h> //CTextListBoxModel
#include <eikenv.h>
#include <eiksbfrm.h> //CEikScrollBarFrame
#include <f32file.h>
#include <putty.rsg>
#include "puttyui.hrh"
#include "profilelistdialog.h"
#include "settingsdialog.h"
#include "puttyengine.h"

_LIT(KDefaultProfileName, "Default");
_LIT(KBlankProfileName, "Profile");
_LIT(KNewProfileName, "New Profile");
_LIT(KUniqueProfileFormat, "%S (%d)");
const TInt KDefaultProfileIndex = 0;


CProfileListDialog::CProfileListDialog(const TDesC &aProfileDirectory,
                                       const TDesC &aDefaultProfileFile,
                                       TDes &aProfileFile,
                                       CPuttyEngine *aPutty)
    : iProfileDirectory(aProfileDirectory),
      iDefaultProfileFile(aDefaultProfileFile),
      iProfileFile(aProfileFile),
      iPutty(aPutty) {
}


CProfileListDialog::~CProfileListDialog() {
    delete iProfileArray;
}


void CProfileListDialog::PreLayoutDynInitL() {
    ButtonGroupContainer().SetDefaultCommand(ECmdProfileListConnect);

    iProfileArray = new (ELeave) CDesCArrayFlat(8);

    // Add default as the first profile
    iProfileArray->AppendL(KDefaultProfileName);

    // Find all profile files from the profile directory and add them to the
    // list    
    RFs &fs = CEikonEnv::Static()->FsSession();
    CDir *dir;
    User::LeaveIfError(fs.GetDir(iProfileDirectory, KEntryAttNormal,
                                 ESortByName, dir));
    CleanupStack::PushL(dir);
    for ( TInt i = 0; i < dir->Count(); i++ ) {
        iProfileArray->AppendL((*dir)[i].iName);
    }
    CleanupStack::PopAndDestroy(); //dir

    // Set profiles to the listbox
    CEikTextListBox *lbox = ((CEikTextListBox*)Control(EProfileListDlgProfileList));
    CTextListBoxModel *lbm = lbox->Model();
    lbm->SetItemTextArray(iProfileArray);
    lbm->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Enable scroll bars
    CEikScrollBarFrame *sbf = lbox->CreateScrollBarFrameL(ETrue);
    sbf->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,
                                 CEikScrollBarFrame::EAuto);

    ButtonGroupContainer().SetDefaultCommand(ECmdProfileListConnect);
}


TBool CProfileListDialog::OkToExitL(TInt aButtonId) {

    TBool okToExit = EFalse;   
    CEikTextListBox *lbox = ((CEikTextListBox*)Control(EProfileListDlgProfileList));

    // Buffers for current and new file names and a name
    HBufC *fileNameBuf = HBufC::NewLC(KMaxFileName);
    TPtr fileName = fileNameBuf->Des();
    HBufC *nameBuf = HBufC::NewLC(KMaxFileName);
    TPtr name = nameBuf->Des();
    
    switch ( aButtonId ) {
        case ECmdProfileListConnect: {
            // Connect to the selected profile
            TInt sel = lbox->CurrentItemIndex();
            if ( sel == KDefaultProfileIndex ) {
                iProfileFile = iDefaultProfileFile;
            } else {
                iProfileFile = iProfileDirectory;
                iProfileFile.Append((*iProfileArray)[sel]);
            }
            okToExit = ETrue;
            break;
        }
            
        case ECmdProfileListEdit: {
            // Edit the selected profile
            TInt sel = lbox->CurrentItemIndex();
            if ( sel == KDefaultProfileIndex ) {
                name = KDefaultProfileName;
                fileName = iDefaultProfileFile;
            } else {
                name = (*iProfileArray)[sel];
                fileName = iProfileDirectory;
                fileName.Append(name);
            }

            // Edit settings
            iPutty->ReadConfigFileL(fileName);
            Config *cfg = iPutty->GetConfig();
            CSettingsDialog *dlg = new (ELeave)
                CSettingsDialog(name, (sel == KDefaultProfileIndex), cfg,
                                iPutty);
            switch ( dlg->ExecuteLD(R_SETTINGS_DIALOG) ) {
                case EEikBidOk:
                    // Handle rename first
                    if ( (sel != KDefaultProfileIndex) &&
                         (name.Compare((*iProfileArray)[sel]) != 0) ) {
                        MakeNameLegal(name);
                        // Delete current file so that we can rename to a file
                        // that maps to the same name (e.g. changes in case)
                        User::LeaveIfError(
                            CEikonEnv::Static()->FsSession().Delete(fileName));
                        MakeNameUniqueL(name);
                        fileName = iProfileDirectory;
                        fileName.Append(name);
                        
                        // Update profile list and listbox
                        iProfileArray->Delete(sel);
                        iProfileArray->InsertL(sel, name);
                        lbox->DrawNow();
                    }
                    // Just write, the name can be an old one or a renamed one
                    iPutty->WriteConfigFileL(fileName);
                    break;

                case ECmdSettingsDelete:
                    if ( sel != KDefaultProfileIndex ) {
                        // Remove the profile from the list and update
                        // listbox
                        iProfileArray->Delete(sel);
                        lbox->HandleItemRemovalL();
                        if ( sel >= iProfileArray->Count() ) {
                            sel = iProfileArray->Count() - 1;
                        }
                        lbox->SetCurrentItemIndexAndDraw(sel);

                        // Delete profile file
                        User::LeaveIfError(
                            CEikonEnv::Static()->FsSession().Delete(fileName));
                    }
                    break;

                default:
                    break;
            }
            okToExit = EFalse;
            break;
        }
            
        case ECmdProfileListNew: {
            // New profile -- start with defaults, but with a new name
            iPutty->ReadConfigFileL(iDefaultProfileFile);
            Config *cfg = iPutty->GetConfig();
            name = KNewProfileName;
            MakeNameUniqueL(name);

            CSettingsDialog *dlg = new (ELeave)
                CSettingsDialog(name, EFalse, cfg, iPutty);
            if ( dlg->ExecuteLD(R_SETTINGS_DIALOG) == EEikBidOk ) {
                // Finalize name
                MakeNameLegal(name);
                MakeNameUniqueL(name);
                fileName = iProfileDirectory;
                fileName.Append(name);
                
                // Update the listbox
                iProfileArray->AppendL(name);
                lbox->HandleItemAdditionL();
                lbox->SetCurrentItemIndexAndDraw(iProfileArray->Count() - 1);

                // Write out new settings
                iPutty->WriteConfigFileL(fileName);
            }
            okToExit = EFalse;
            break;
        }

        case ECmdProfileListClose:
            okToExit = ETrue;
            break;
        
        default:
            User::Invariant();
    }

    CleanupStack::PopAndDestroy(2); //fileNameBuf, nameBuf
    return okToExit;
}


_LIT(KBadChars, "<>:\"/|*?\\");

// Make a profile name a legal filename
void CProfileListDialog::MakeNameLegal(TDes &aName) {
    TInt len = aName.Length();
    TBuf<10> bad;
    bad = KBadChars;
    for ( TInt i = 0; i < len; i++ ) {
        if ( bad.Locate(aName[i]) != KErrNotFound ) {
            aName[i] = ' ';
        }
    }
    aName.Trim();
    if ( aName.Length() == 0 ) {
        aName = KBlankProfileName;
    }
}


// Make profile name unique. Must already be a legal filename
void CProfileListDialog::MakeNameUniqueL(TDes &aName) {
    HBufC *fileNameBuf = HBufC::NewLC(KMaxFileName);
    TPtr fileName = fileNameBuf->Des();
    HBufC *newNameBuf = HBufC::NewLC(KMaxFileName);
    TPtr newName = newNameBuf->Des();

    newName = aName;
        
    TBool done = EFalse;
    TInt num = 2;
    while ( !done ) {
        fileName = iProfileDirectory;
        fileName.Append(newName);
        TEntry dummy;
        TInt err = CEikonEnv::Static()->FsSession().Entry(fileName, dummy);
        if ( err == KErrNotFound ) {
            // Have a unique name
            aName = newName;
            break;
        }
        User::LeaveIfError(err);

        // Start adding numbers to the end of the name
        if ( aName.Length() > (KMaxFileName + 14) ) {
            aName = aName.Left(KMaxFileName + 14);
        }
        newName.Format(KUniqueProfileFormat, &aName, num);
        num++;
    }

    CleanupStack::PopAndDestroy(2); //fileNameBuf, newNameBuf;
}
