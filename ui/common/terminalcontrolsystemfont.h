/*    terminalsystemfont.h
 *
 * A terminal UI control using a system font
 *
 * Copyright 2002,2004-2006 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TERMINALCONTROLSYSTEMFONT_H__
#define __TERMINALCONTROLSYSTEMFONT_H__

#include "terminalcontrol.h"


/**
 * Concrete terminal control using a system font.
 */
class CTerminalControlSystemFont : public CTerminalControl {

public:    
    /** 
     * Factory method.
     * 
     * @param aObserver The observer object to use
     * @param aRect Initial terminal control rectangle on screen. Use SetRect()
     *              to change the terminal size.
     * @param aContainerWindow The window that contains this control.
     * 
     * @return A new terminal control object
     */
    static CTerminalControlSystemFont *NewL(MTerminalObserver &aObserver,
                                            const TRect &aRect,
                                            RWindow &aContainerWindow);
    /** 
     * Changes the terminal font.
     * Note! The client MUST call SetRect() immediately afterwards with a
     * new size (FIXME?).
     * 
     * @param aLargeFont ETrue to use the large terminal font
     */
    virtual void SetFontL(TBool aLargeFont);

    /** 
     * Destructor.
     */
    virtual ~CTerminalControlSystemFont();

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
    CTerminalControlSystemFont(MTerminalObserver &aObserver);
    
    /** 
     * Second-phase constructor.
     * 
     * @param aRect 
     */
    void ConstructL(const TRect &aRect, RWindow &aContainerWindow);

    // CTerminalControl methods
    virtual void UpdateDisplay(TInt aX, TInt aY, TInt aLength);
    virtual void AllocateBuffersL();    

    void DoDraw(CGraphicsContext &aGc, TInt aX, TInt aY,
                const TDesC &aText, TBool aBold, TBool aUnderline,
                TRgb aForeground, TRgb aBackground) const;
    void UpdateWithGc(CGraphicsContext &aGc, TInt aX, TInt aY,
                      TInt aLength) const;
    
    CFont *iFont;
    HBufC *iRowTextBuf;
};


#endif
