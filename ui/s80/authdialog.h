/** @file authdialog.h
 *
 * Authentication dialogs
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __AUTHDIALOG_H__
#define __AUTHDIALOG_H__

#include <e32std.h>
#include <eikdialg.h>
#include <eikcmobs.h>


/**
 * Authentication dialog. Prompts the user to enter authentication data such
 * as the username or password.
 */
class CAuthenticationDialog : public CEikDialog {

public:
    /** 
     * Runs the authentication dialog using the correct resource.
     *
     * @param aPrompt Dialog prompt
     * @param aTarget Target descriptor for user entry
     * @param aSecret ETrue if the data should be entered with a secret editor
     *                (e.g. a password)
     *
     * @return ETrue if the user pressed OK
     */
    static TBool DoPromptL(const TDesC &aPrompt, TDes &aTarget, TBool aSecret);
    
private:
    CAuthenticationDialog(const TDesC &aPrompt, TDes &aTarget, TBool aSecret);
    
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);

private:
    const TDesC &iPrompt;
    TDes &iTarget;
    TBool iSecret;
};


#endif
