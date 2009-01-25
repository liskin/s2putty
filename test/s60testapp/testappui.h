/*    testappui.h
 *
 * Test Application UI class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TESTAPPUI_H__
#define __TESTAPPUI_H__

#include <aknappui.h>

class CTestAppView;


/**
 * Test Application UI class.
 */
class CTestAppUi: public CAknAppUi {
    
public:
    void ConstructL();
    CTestAppUi();
    ~CTestAppUi();
    void HandleCommandL(TInt aCommand);

private:
    CTestAppView *iAppView;
};


#endif
