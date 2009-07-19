/*    terminalcontainer.h
 *
 * Putty UI container class for the terminal view
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002,2004,2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef TERMINALCONTAINER_H
#define TERMINALCONTAINER_H

#include <coecntrl.h>
#include "terminalcontrols2font.h"


// Forward declarations
class CTerminalView;
class CS2Font;


/**
 * PuTTY UI view class for the terminal view. Owns the terminal
 * control. The terminal needs a separate container control since the terminal
 * control size is rounded to the closes integer number of characters and may
 * thus not fill the whole client rectangle.
 */
class CTerminalContainer : public CCoeControl {
    
public:
    /** 
     * Factory method.
     *
     * @param aRect Initial terminal container screen rectangle
     * @param aTerminalObserver Terminal observer for the terminal
     * @param aView The view that owns this container
     * @param aFontFile The font to use
     * 
     * @return A new CTerminalContainer instance.
     */
    static CTerminalContainer *NewL(const TRect &aRect,
                                    MTerminalObserver *aTerminalObserver,
                                    CTerminalView *aView,
                                    const TDesC &aFontFile);

    /** 
     * Destructor.
     */
    ~CTerminalContainer();
    
    /** 
     * Returns a reference to the terminal control.
     * 
     * @return The terminal control in use.
     */
    CTerminalControl &Terminal();

    /** 
     * Sets the font to use.
     * 
     * @param aFontFile Font file name
     */
    void SetFontL(const TDesC &aFontFile);

    /** 
     * Sets the default colors used to clear the terminal and clears the
     * terminal.
     * 
     * @param aForeground Default foreground color
     * @param aBackground Default background color
     */
    void SetDefaultColors(TRgb aForeground, TRgb aBackground);


public: // From CCoeControl
    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);
    virtual void Draw(const TRect &aRect) const;
    virtual void SizeChanged();
    virtual TCoeInputCapabilities InputCapabilities() const;
    
private:
    // Constructor
    CTerminalContainer(CTerminalView *aView);

    // Second-phase Constructor
    void ConstructL(const TRect &aRect, MTerminalObserver *aTerminalObserver,
                    const TDesC &aFontFile);
    
    // Calculate a new terminal size with the current configuration
    // (font, window etc)
    void GetTerminalRect(TRect &aRect);

    CTerminalControlS2Font *iTerminal;
    TRect iTermRect;
    TBool iLargeFont;
    TBool iBackSpace;
    TBool iFullScreen;
    CS2Font *iFont;
    CTerminalView *iView;
    TRgb iDefaultFg, iDefaultBg;
};

#endif
