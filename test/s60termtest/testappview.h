/*    testappview.h
 *
 * Test View class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TESTAPPVIEW_H__
#define __TESTAPPVIEW_H__

#include <coecntrl.h>

class CTestAppUi;
class MTerminalObserver;
class CTerminalControlS2Font;
class CS2Font;


/**
 * Test application view class.
 */
class CTestAppView : public CCoeControl {
    
public:
    static CTestAppView *NewL(CTestAppUi &aAppUi, MTerminalObserver &aObserver,
                              const TRect &aRect);
    
    ~CTestAppView();

    CTerminalControl *Terminal();

public: //CCoeControl
    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    void Draw(const TRect &aRect) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);
    
private:
    CTestAppView(CTestAppUi &aAppUi);
    void ConstructL(MTerminalObserver &aObserver, const TRect& aRect);
    void GetTerminalRect(TRect &aRect);    
    
    CTestAppUi &iAppUi;
    CTerminalControlS2Font *iTerminal;
    TRect iTermRect;
    CS2Font *iFont;
};


#endif
