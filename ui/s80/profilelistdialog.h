/** @file profilelistdialog.h
 *
 * Profile list dialog
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PROFILELISTDIALOG_H__
#define __PROFILELISTDIALOG_H__

#include <e32std.h>
#include <eikdialg.h>

class CPuttyEngine;

/**
 * Profile list dialog.
 */
class CProfileListDialog : public CEikDialog {
    
public:
    /** 
     * Constructor.
     */
    /** 
     * Constructor
     * 
     * @param aProfileDirectory Profile directory name
     * @param aDefaultProfileFile Default profile file name, must not be in the
     *                            profile directory
     * @param aProfileFile Output: Selected profile name if ExecuteLD() returns
     *                     with ECmdProfileListConnect. Otherwise unmodified.
     * @param aPutty PuTTY engine instance
     */
    CProfileListDialog(const TDesC &aProfileDirectory,
                       const TDesC &aDefaultProfileFile,
                       TDes &aProfileFile,
                       CPuttyEngine *aPutty);

    ~CProfileListDialog();
    
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aKeycode);

private:
    void MakeNameLegal(TDes &aName);
    void MakeNameUniqueL(TDes &aName);
    
    CDesCArrayFlat *iProfileArray;
    const TDesC &iProfileDirectory;
    const TDesC &iDefaultProfileFile;
    TDes &iProfileFile;
    CPuttyEngine *iPutty;
};


#endif
