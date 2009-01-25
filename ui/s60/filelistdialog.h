/*    filelistdialog.h
 *
 * A file selection dialog
 *
 * Copyright 2004 Sergei Khloupnov
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef FILELISTDIALOG_H
#define FILELISTDIALOG_H

#include <aknlistquerydialog.h>
#include <badesca.h>
#include "puttyappui.h"

/**
 * Internal file dialog implementation class. Not intended for use, use
 * CFileListDialog instead.
 */
class CFileListDialogWrapper : public CAknListQueryDialog {

// Note: this class has braindamaged interface
// you specify initial path in aFile
// and receive selected file/dir name in aFile (name without path)

public:
    ~CFileListDialogWrapper();
    static CFileListDialogWrapper* NewL(TInt *aIndex, TFileName *aFile,
                                        TBool aNewFile);
    static CFileListDialogWrapper* NewLC(TInt *aIndex, TFileName *aFile,
                                         TBool aNewFile);

protected:
    CFileListDialogWrapper(TInt *aIndex, TFileName *aFile, TBool aNewFile);
    TBool OkToExitL(TInt aKeycode);
    void PreLayoutDynInitL();
    void ConstructL();

private:
    TInt *iIndex; // not used - only for CAknListQueryDialog compatibility
    TFileName iPath;
    TFileName *iResult; // where to store result when done
    CDesCArrayFlat* iDirectoryEntries;
    TBool iNewFile;
};


/**
 * A file selection dialog class. This class should be used instead of the
 * system dialogs to maintain compatibility with S60 v0.9 and Nokia 7650.
 */
class CFileListDialog {

public:
    /** 
     * Runs the file selection dialog, prompting the user to select a file.
     * 
     * @param aPath The full path to the selected file is stored in this
     *              descriptor. If the descriptor contains an initial path,
     *              it is used as the directory where browsing starts.
     * @param aCreateFile Set to ETrue to allow the user to create a new file
     *                    (i.e. select a non-existing one).
     * 
     * @return ETrue if the user selected a file, EFalse if not.
     */
    static TBool RunDlgLD(TDes &aPath, TBool aCreateFile = EFalse);
};

#endif
