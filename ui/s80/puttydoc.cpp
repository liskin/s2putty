/*    puttydoc.cpp
 *
 * Putty UI Document class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "puttydoc.h"
#include "puttyappui.h"


CPuttyDocument::CPuttyDocument(CEikApplication &anApp)
    : CEikDocument(anApp) {
}

CEikAppUi *CPuttyDocument::CreateAppUiL() {
    return new (ELeave) CPuttyAppUi;
}
