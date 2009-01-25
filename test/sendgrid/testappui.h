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
#include "sendgrid.h"

class CTestAppView;


/**
 * Test Application UI class.
 */
class CTestAppUi: public CAknAppUi, public MTerminalObserver,
                  public MSendGridObserver {
    
public:
    void ConstructL();
    CTestAppUi();
    ~CTestAppUi();
    void HandleCommandL(TInt aCommand);

    void IdleL();
    static TInt IdleCallback(TAny *aAny);
    
public: //MTerminalObserver
    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers);

public: // MSendGridObserver
    void MsgoCommandL(TInt aCommand);
    void MsgoTerminated();
    
private:
    CTestAppView *iAppView;
    CTerminalControl *iTerminal;
    TInt iTermX, iTermY;
    TInt iTermWidth, iTermHeight;
    CIdle *iIdle;
    CSendGrid *iGrid;
};


#endif
