/** @file connectiondialog.h
 *
 * Connection open dialog
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __CONNECTIONDIALOG_H__
#define __CONNECTIONDIALOG_H__

#include <e32std.h>
#include <eikdialg.h>


/**
 * Connection open dialog. Prompts the user to enter the information
 * needed to set up the connection. Uses the resource structure
 * R_CONNECTION_DIALOG.
 */
class CConnectionDialog : public CEikDialog {
    
public:
    /** 
     * Constructor.
     * @param aHostName Target descriptor for the host name entered by
     * the user.
     */
    CConnectionDialog(TDes &aHostName);
    
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aKeycode);

private:
    TDes *iHostName;
};


#endif
