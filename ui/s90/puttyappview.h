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
#include "terminalcontrols2font.h"


class CPuttyAppUi;
class CS2Font;



/**
 * PuTTY UI application view class. Owns the terminal control.
 */
class CPuttyAppView : public CCoeControl {
    
public:
    CPuttyAppView(MTerminalObserver *aTerminalObserver, CPuttyAppUi *aAppUi);
    ~CPuttyAppView();
    void ConstructL(const TDesC &aFontDir, const TRect& aRect);

    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);
    virtual TCoeInputCapabilities InputCapabilities() const;

    CTerminalControl *Terminal();

    void SetFontL(TBool aLargeFont);
    void SetFullScreenL(TBool aFullScreen);

private:
    void Draw(const TRect &aRect) const;
    virtual void SizeChanged();

    void ResizeTerminal();

    MTerminalObserver *iTerminalObserver;
    CTerminalControlS2Font *iTerminal;
    HBufC *iFontDir;
    HBufC *iFontFile;
    CS2Font *iFont;
    TRect iTermRect;
    TBool iLargeFont;
    TBool iFullScreen;
    TBool iChrDown;
    CPuttyAppUi *iAppUi;
    TInt iFontWidth, iFontHeight;
    TBool iCtrlToggle;
};

#endif
