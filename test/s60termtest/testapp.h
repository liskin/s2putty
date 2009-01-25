/*    testapp.h
 *
 * Test Application class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TESTAPP_H__
#define __TESTAPP_H__

#include <aknapp.h>
#include <akndoc.h>

/**
 * Test Application class. Does very little, just creates the document.
 */
class CTestApplication : public CAknApplication {

private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;    
};

#endif
