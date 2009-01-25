/*    terminalcontrols2font.h
 *
 * A terminal UI control using a CS2Font bitmap font
 *
 * Copyright 2002,2004-2005 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TERMINALCONTROLS2FONT_H__
#define __TERMINALCONTROLS2FONT_H__

#include "terminalcontrol.h"


// Forward declarations
class CS2Font;
class COneShotTimer;
class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;
class RWsSession;


/**
 * Concrete terminal control using a CS2Font bitmap font.
 */
class CTerminalControlS2Font : public CTerminalControl {

public:    
    /** 
     * Factory method.
     * 
     * @param aObserver The observer object to use
     * @param aRect Initial terminal control rectangle on screen. Use SetRect()
     *              to change the terminal size.
     * @param aContainerWindow The window that contains this control.
     * @param aFont The font to use
     * 
     * @return A new terminal control object
     */
    static CTerminalControlS2Font *NewL(MTerminalObserver &aObserver,
                                        const TRect &aRect,
                                        RWindow &aContainerWindow,
                                        CS2Font &aFont);
    /** 
     * Changes the terminal font.
     * 
     * @param aFont The font to use
     */
    virtual void SetFontL(CS2Font &aFont);

    /** 
     * Destructor.
     */
    virtual ~CTerminalControlS2Font();

    /** \fn void SetRect(const TRect aRect);
     *
     * Sets the terminal control size and position on screen. The actual size
     * used is rounded down from the argument to the nearest size in full
     * characters.
     *
     * @param aRect New terminal window size and position.
     */

    // CCoeControl methods
    virtual void Draw(const TRect &aRect) const;


protected:
    /** 
     * Constructor.
     *
     * @param aObserver The observer object to use
     */
    CTerminalControlS2Font(MTerminalObserver &aObserver,
                           CS2Font &aFont);
    
    /** 
     * Second-phase constructor.
     * 
     * @param aRect 
     */
    void ConstructL(const TRect &aRect, RWindow &aContainerWindow);

    // CTerminalControl methods
    virtual void UpdateDisplay(TInt aX, TInt aY, TInt aLength);
    virtual void Clear();
    virtual void AllocateBuffersL();    

    void RenderRow(TInt aX, TInt aY, TInt aLength) const;
    void StartUpdateTimer();
    void Update();
    static TInt UpdateCallBack(TAny *aPtr);

    
    CS2Font *iFont;
    COneShotTimer *iTimer; // Timer for display refresh
    TBool iTimerRunning;
    TInt *iDirtyLeft; // Dirty area start on each line
    TInt *iDirtyRight; // Dirty area end on each line
    CFbsBitmap *iBitmap;
    CFbsBitmapDevice *iBitmapDevice;
    CFbsBitGc *iBitmapGc;
    HBufC *iRowTextBuf;
};


#endif
