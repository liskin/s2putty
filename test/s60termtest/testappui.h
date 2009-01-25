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
#include "terminalcontrol.h"

class CTestAppView;


/**
 * Test Application UI class.
 */
class CTestAppUi: public CAknAppUi, public MTerminalObserver {
    
public:
    void ConstructL();
    CTestAppUi();
    ~CTestAppUi();
    void HandleCommandL(TInt aCommand);
    
public: //MTerminalObserver
    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers);
    
private:
    CTestAppView *iAppView;
    CTerminalControl *iTerminal;
    TInt iTermX, iTermY;
    TInt iTermWidth, iTermHeight;
};


#endif
