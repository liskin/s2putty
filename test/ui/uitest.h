#ifndef __UITEST_H__
#define __UITEST_H__

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <eikon.hrh>

#include "terminalcontrol.h"

#include <uitest.rsg>
#include "uitest.hrh"


class CTestAppView;


// Application
class CTestApplication : public CEikApplication {
    
private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
};


// Document
class CTestDocument : public CEikDocument {
    
public:
    //static CTestDocument *NewL(CEikApplication &anApp);
    CTestDocument(CEikApplication &anApp);
    //void ConstructL();
private: 
    CEikAppUi* CreateAppUiL();
};


// AppUI
class CTestAppUi : public CEikAppUi, public MTerminalObserver {
    
public:
    void ConstructL();
    CTestAppUi();
    ~CTestAppUi();

    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers);
    virtual void RePaintWindow();

    
private:
    void HandleCommandL(TInt aCommand);
    void DrawLine(TInt aLine);

private:
    CTestAppView *iAppView;

    TUint16 *iTermBuf;
    TInt iTermWidth, iTermHeight;
    TInt iWriteLine;
    TInt64 iSeed;
};


// View
class CTestAppView : public CCoeControl {
    
public:
    CTestAppView(MTerminalObserver *aTerminalObserver);
    ~CTestAppView();
    void ConstructL(const TRect& aRect);

    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);

    CTerminalControl *Terminal();

private:
    void Draw(const TRect &aRect) const;

    MTerminalObserver *iTerminalObserver;
    CTerminalControl *iTerminal;
};


#endif
