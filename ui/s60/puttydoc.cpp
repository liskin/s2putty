/*    puttydoc.cpp
 *
 * Putty UI Document class
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "puttydoc.h"
#include "puttyappui.h"


CPuttyDocument::CPuttyDocument(CAknApplication &anApp)
    : CAknDocument(anApp) {
}

CEikAppUi* CPuttyDocument::CreateAppUiL() {
    return new (ELeave) CPuttyAppUi;
}
