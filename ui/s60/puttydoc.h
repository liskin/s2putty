/*    puttydoc.h
 *
 * Putty UI Document class
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYDOC_H__
#define __PUTTYDOC_H__

#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>


/**
 * PuTTY UI application document class. Does very little, just creates the
 * application UI.
 */
class CPuttyDocument : public CAknDocument {
    
public:
    CPuttyDocument(CAknApplication &anApp);

private: 
    CEikAppUi* CreateAppUiL();
};


#endif
