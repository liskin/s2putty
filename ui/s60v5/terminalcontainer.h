/*    terminalcontainer.h
 *
 * Putty UI container class for the terminal view
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002,2004,2007,2008,2009 Petteri Kangaslampi
 * Copyright 2009 Risto Avila
 * See license.txt for full copyright and license information.
*/

#ifndef TERMINALCONTAINER_H
#define TERMINALCONTAINER_H

#include <coecntrl.h>
#include <aknlongtapdetector.h>
#include <aknstyluspopupmenu.h>
#include <akntoolbarobserver.h>
#include <touchfeedback.h>
#include "terminalcontrols2font.h"
#include "oneshottimer.h"
#include "touchuisettings.h"

// Forward declarations
class CTerminalView;
class CS2Font;
class COneShotTimer;
class CCustomToolBar;

const TPoint KPopupPoint( 50, 50 );

enum TSwipeGesture {
    ENoSwipe = -1,
    ESwipeLeft = 0,
    ESwipeRight = 1,
    ESwipeUp = 2,
    ESwipeDown = 3
};

enum TTouchActions {
   EPuttyTouchTap = 0,
   EPuttyTouchDoubleTap,
   EPuttyTouchLongTap,
   EPuttyTouchSwipeUp,
   EPuttyTouchSwipeDown,
   EPuttyTouchSwipeLeft,
   EPuttyTouchSwipeRight
};

/**
 * PuTTY UI view class for the terminal view. Owns the terminal
 * control. The terminal needs a separate container control since the terminal
 * control size is rounded to the closes integer number of characters and may
 * thus not fill the whole client rectangle.
 */
class CTerminalContainer : public CCoeControl,
                           public MAknLongTapDetectorCallBack,
                           public MEikMenuObserver {
    
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
    
    /* Touch input */
    void HandlePointerEventL( const TPointerEvent& aEvent );
    void CreatePopupMenuFromResourceL( const TPoint &aPosition = KPopupPoint );
    void HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation );
    void ProcessCommandL(TInt aCommandId);
    void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/) {
    }

protected:
    //CallBack for timeout after single tap. if timer is expired tap was signle tap and SingleTap() is called.
    static TInt DoubleTapCallBack(TAny *aPtr); 
    //SingleTap() processes the last tap event that was ignored after time was started
    void SingleTap();


public: // From CCoeControl
    virtual TInt CountComponentControls() const;
    virtual CCoeControl *ComponentControl(TInt aIndex) const;
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);
    virtual void Draw(const TRect &aRect) const;
    virtual void SizeChanged();
    virtual TCoeInputCapabilities InputCapabilities() const;
    
    //Implemented command handling from custom toolbar
    void HandleCustomToolbar(TInt acommand);
    TTouchSettings* GetTouchSettings() { return &iTouchSettings; };
    void UpdateAfterSettingsChange();
    void Select(TBool aValue); //Enable / disable select
    void ReleaseAlt();
    void ReleaseCtrl();
    void SwapButtons(TInt aButton, TInt aCmd);
    
private:
    // Constructor
    CTerminalContainer(CTerminalView *aView);

    // Second-phase Constructor
    void ConstructL(const TRect &aRect, MTerminalObserver *aTerminalObserver,
                    const TDesC &aFontFile);
    
    // Calculate a new terminal size with the current configuration
    // (font, window etc)
    void GetTerminalRect(TRect &aRect);
    
    TSwipeGesture IsSwipeGesture();
    TSwipeGesture TestHorizontalSwipe();
    TSwipeGesture TestVerticalSwipe();
    void HandleTouchAction(TInt aCommand, const TPoint& aPenEventScreenLocation); //Handles touch actions
    
    CTerminalControlS2Font *iTerminal;
    TRect iTermRect;
    TBool iLargeFont;
    TBool iBackSpace;
    TBool iFullScreen;
    CS2Font *iFont;
    CTerminalView *iView;
    TRgb iDefaultFg, iDefaultBg;
    
    /* Touch ui */
    CAknLongTapDetector* iLongTapDetector;
    COneShotTimer *iTimer;
    TBool iLongTapCallbackReceived;
    TBool iSingleTapTimerExpired;
    TTime iLastTapStart;
    TPointerEvent iLastPointerEvent;
    RArray<TPoint> iDragPoints;
    
    //PopUpMenu
    CAknStylusPopUpMenu* iPopup;
    TBool iSelectionStart;
    TBool iLastEventDrag;
    TBool iSelectEnabled;
    TBool iIgnoreTbClick;
       
    TBool iCtrlModifier;
    TBool iAltModifier;
    
    TBool iGestureCtrlMod;
    TBool iGestureAltMod;
     
    TTouchSettings iTouchSettings;
    MTouchFeedback* iTouchFeedBack;
    CCustomToolBar* iCustomToolbarControl;

    
};

#endif
