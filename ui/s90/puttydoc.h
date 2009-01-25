/*    puttydoc.h
 *
 * Putty UI Document class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYDOC_H__
#define __PUTTYDOC_H__

#include <eikapp.h>
#include <eikdoc.h>
#include <eikappui.h>


/**
 * PuTTY UI application document class. Does very little, just creates the
 * application UI.
 */
class CPuttyDocument : public CEikDocument {
    
public:
    CPuttyDocument(CEikApplication &anApp);

private: 
    CEikAppUi* CreateAppUiL();
};


#endif
