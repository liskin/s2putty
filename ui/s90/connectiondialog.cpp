/*    connectiondialog.cpp
 *
 * Connection open dialog
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <eikedwin.h>
#include <eikbtgpc.h>
#include "puttyui.hrh"
#include "connectiondialog.h"


CConnectionDialog::CConnectionDialog(TDes &aHostName) {
    iHostName = &aHostName;
}


void CConnectionDialog::PreLayoutDynInitL() {
    ((CEikEdwin*)Control(EConnDlgHost))->SetTextL(iHostName);
    ButtonGroupContainer().SetDefaultCommand(EEikBidOk);
}


TBool CConnectionDialog::OkToExitL(TInt /*aKeyCode*/) {
    ((CEikEdwin*)Control(EConnDlgHost))->GetText(*iHostName);
    return ETrue;
}
