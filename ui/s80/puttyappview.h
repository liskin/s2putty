/*    puttyappview.h
 *
 * Putty UI View class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYAPPVIEW_H__
#define __PUTTYAPPVIEW_H__

#include <coecntrl.h>
#include "terminalcontrolsystemfont.h"


class CPuttyAppUi;


/**
 * PuTTY UI application view class. Owns the terminal control.
 */
class CPuttyAppView : public CCoeControl {
    
public:
    CPuttyAppView(MTerminalObserver *aTerminalObserver, CPuttyAppUi *aAppUi);
    ~CPuttyAppView();
    void ConstructL(const TRect& aRect);

    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);

    CTerminalControl *Terminal();

    void SetFontL(TBool aLargeFont);
    void SetFullScreenL(TBool aFullScreen);
    void SetDefaultColors(TRgb aDefaultFg, TRgb aDefaultBg);

private:
    void Draw(const TRect &aRect) const;
    virtual void SizeChanged();
    virtual TCoeInputCapabilities InputCapabilities() const;

    void ResizeTerminal();

    MTerminalObserver *iTerminalObserver;
    CTerminalControlSystemFont *iTerminal;
    TRect iTermRect;
    TBool iLargeFont;
    TBool iFullScreen;
    TBool iChrDown;
    CPuttyAppUi *iAppUi;
    TRgb iDefaultFg, iDefaultBg;
};

#endif
