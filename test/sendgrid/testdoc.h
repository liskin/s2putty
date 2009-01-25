/*    testdoc.h
 *
 * Test Document class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TESTDOC_H__
#define __TESTDOC_H__

#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>


/**
 * Test application document class. Does very little, just creates the
 * application UI.
 */
class CTestDocument : public CAknDocument {
    
public:
    CTestDocument(CAknApplication &aApp);

private: 
    CEikAppUi* CreateAppUiL();
};


#endif
