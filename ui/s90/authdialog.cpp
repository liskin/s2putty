/*    authdialog.cpp
 *
 * Authentication dialogs
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikedwin.h>
#include <cknsecreteditor.h>
#include <eikcapc.h>
#include <eikbtgpc.h>
#include "puttyui.hrh"
#include <putty.rsg>
#include "authdialog.h"

_LIT(KAssertPanic, "settingsdialog.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


// Dialog interface
TBool CAuthenticationDialog::DoPromptL(const TDesC &aPrompt, TDes &aTarget,
                                       TBool aSecret) {
    CAuthenticationDialog *dlg =
        new (ELeave) CAuthenticationDialog(aPrompt, aTarget, aSecret);
    TInt res;
    if ( aSecret ) {
        res = R_AUTH_PROMPT_SECRET_DIALOG;
    } else {
        res = R_AUTH_PROMPT_PUBLIC_DIALOG;
    }

    return (TBool) dlg->ExecuteLD(res);
}


// Dialog constructor
CAuthenticationDialog::CAuthenticationDialog(const TDesC &aPrompt,
                                             TDes &aTarget,
                                             TBool aSecret)
    : iPrompt(aPrompt),
      iTarget(aTarget),
      iSecret(aSecret) {
}


// Dialog init
void CAuthenticationDialog::PreLayoutDynInitL() {

    SetTitleL(iPrompt);
    ButtonGroupContainer().SetDefaultCommand(EEikBidOk);
}


// Dialog close
TBool CAuthenticationDialog::OkToExitL(TInt /*aButtonId*/) {

    if ( iSecret ) {
        ((CCknSecretEditor*)Control(EAuthPromptEditor))->GetText(iTarget);
    } else {
        ((CEikEdwin*)Control(EAuthPromptEditor))->GetText(iTarget);
    }
    
    return ETrue;
}
