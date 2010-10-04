/*    terminalcontrol.h
 *
 * Terminal UI control base class
 *
 * Copyright 2002,2004-2005 Petteri Kangaslampi
 * Portions copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TERMINALCONTROL_H__
#define __TERMINALCONTROL_H__

#include <e32std.h>
#include <coecntrl.h>
#include <fepbase.h>


/**
 * Terminal observer class. Each terminal control client must implement
 * the observer interface.
 */
class MTerminalObserver {

public:
    /** 
     * Terminal size has been changed.
     * 
     * @param aWidth New terminal width in characters
     * @param aHeight New terminal height in characters
     */
    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight) = 0;

    /** 
     * A key has been pressed
     * 
     * @param aCode Key code
     * @param aModifiers Key modifiers
     */
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers) = 0;
};


class CFont;
class CTermFepExt1;


/**
 * A terminal UI base class class. Manages a terminal control on the screen,
 * taking care of drawing the text and delivering keypresses to the user.
 * The terminal user must implement the MTerminalObserver interface.
 * Concrete terminal control classes are derived from this, and only one
 * concrete terminal control implementation is included in each variant.
 */
class CTerminalControl : public CCoeControl, public MCoeFepAwareTextEditor {

public:    
    /** 
     * Destructor.
     */
    virtual ~CTerminalControl();

    /** 
     * Draws text on the terminal window. The coordinates are zero-based
     * character coordinates inside the terminal. There is no clipping --
     * the terminal control assumes that the user takes care of this.
     *
     * Cursors can be drawn as text with a different background color.
     * 
     * @param aX Text X-coordinate
     * @param aY Text Y-coordinate
     * @param aText The text to draw
     * @param aBold ETrue if text should be bold (not supported currently)
     * @param aUnderline ETrue if text should be underlined.
     * @param aForeground Text foreground color
     * @param aBackground Text background color. The text is drawn inside a
     *                    solid rectangle colored with this color.
     */
    virtual void DrawText(TInt aX, TInt aY, const TDesC &aText, TBool aBold,
                          TBool aUnderline, TRgb aForeground,
                          TRgb aBackground);
    
    /** 
     * Sets the cursor in the terminal window to a new position. The
     * coordinates are zero-based character coordinates inside the terminal.
     * To hide the cursor, move it to (-1,-1) or other coordinates outside of
     * the terminal.
     * 
     * @param aX New cursor X-coordinate
     * @param aY New cursor Y-coordinate
     */
    virtual void SetCursor(TInt aX, TInt aY);

    /** 
     * Sets the modifiers to add to the next key event.
     * 
     * @param aModifiers Modifiers to add, set to zero to use only
     *                   event default modifiers.
     */
    void SetNextKeyModifiers(TUint aModifiers);

    /** 
     * Sets the terminal control to be grayed out or normal. The terminal
     * control can be grayed out when there is no connection, and it cannot
     * be redrawn properly.
     * 
     * @param aGrayed True if the terminal should be grayed out.
     */
    void SetGrayed(TBool aGrayed);

    /** 
     * Enables or disables selection mode for copy/paste. When in
     * selection mode, cursor keys control the selection cursor and are not
     * transmitted to the observer.
     * 
     * @param iSelectMode ETrue to enable mark mode, EFalse to disable.
     */
    void SetSelectMode(TBool iSelectMode);

    /** 
     * Sets the selection mark to the current selection cursor
     * position. The selection end will automatically be the current
     * selection cursor position and the terminal contents between
     * those two characters are the current selection.
     */
    void SetMark();

    /** 
     * Removes the mark and deactivates the current selection.
     */
    void RemoveMark();

    /** 
     * Makes a copy of currently selected text in mark mode.
     * This method can only be called while in mark mode.
     * 
     * @return Selected text in a new HBufC. The object is pushed to the
     *         cleanup stack and ownership is transferred to the client.
     */
    const HBufC *CopySelectionLC();    

    /** \fn void SetRect(const TRect aRect);
     *
     * Sets the terminal control size and position on screen. The actual size
     * used is rounded down from the argument to the nearest size in full
     * characters.
     *
     * @param aRect New terminal window size and position.
     */

    /** 
     * Sets the default colors used to clear the terminal and clears the
     * terminal.
     * 
     * @param aForeground Default foreground color
     * @param aBackground Default background color
     */
    virtual void SetDefaultColors(TRgb aForeground, TRgb aBackground);

    // CCoeControl methods
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                        TEventCode aType);
    virtual void SizeChanged();
    virtual TCoeInputCapabilities InputCapabilities() const;
#ifdef PUTTY_S90
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

#ifdef PUTTY_S60TOUCH
    void StartVKB();
    void ClearVKBBuffer();
    //Need this for toolbar up / down buttons since those are not real keyup & keydown events and emulating those is quite messy.
    void SetLastKeyArrow() { iLastKeyArrow = ETrue; };
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleTextChange();
    void SetPointerSelect(TBool aEnabled);
    TBool DoWeHaveSelection() { return iHaveSelection; };
    void SetAltModifier(TBool aAltMod); // Default state off
    void SetCtrlModifier(TBool aCtrlMod); // Default state off
#ifdef PUTTY_SYM3_TEST50
    void CreateHttpListL();
    const CDesCArray &HttpList();
#endif
#endif
    // MCoeFepAwareTextEditor methods
    void StartFepInlineEditL(
        const TDesC &aInitialInlineText,
        TInt aPositionOfInsertionPointInInlineText,
        TBool aCursorVisibility,
        const MFormCustomDraw *aCustomDraw,
        MFepInlineTextFormatRetriever &aInlineTextFormatRetriever,
        MFepPointerEventHandlerDuringInlineEdit &aPointerEventHandlerDuringInlineEdit);    
    void UpdateFepInlineTextL(const TDesC& aNewInlineText,
                              TInt aPositionOfInsertionPointInInlineText);    
    void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
    void CommitFepInlineEditL(CCoeEnv& aConeEnvironment);
    void CancelFepInlineEdit();
    TInt DocumentLengthForFep() const;
    TInt DocumentMaximumLengthForFep() const;
    void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
    void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
    void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition,
                                TInt aLengthToRetrieve) const;
    void GetFormatForFep(TCharFormat& aFormat, TInt aDocumentPosition) const;
    void GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine,
                                     TInt& aHeight, TInt& aAscent,
                                     TInt aDocumentPosition) const;
#ifdef PUTTY_S60
    MCoeFepAwareTextEditor_Extension1* Extension1(TBool &aSetToTrue);
#endif

private: // MCoeFepAwareTextEditor
    void DoCommitFepInlineEditL();

protected:
    /** 
     * Terminal control constructor.
     *
     * @param aObserver The observer object to use
     */
    CTerminalControl(MTerminalObserver &aObserver);
    
    /** 
     * Second-phase constructor. Must be called before the control is used.
     * 
     * @param aRect Initial terminal control rectangle on screen. Use SetRect()
     *              to change the terminal size.
     * @param aContainerWindow The window that contains this control.
     */
    void ConstructL(const TRect &aRect, RWindow &aContainerWindow);

    /** 
     * Get the final colors for a character cell, taking selection and cursor
     * into account. Note that the selection cursor doesn't affect the colors,
     * but is typically drawn as a rectangle within the character cell.
     * 
     * @param aX Character cell X-coordinate
     * @param aY Character cell Y-coordinate
     * @param aForeground Final foreground color
     * @param aBackground Final background color
     */
    virtual void GetFinalColors(TInt aX, TInt aY, TRgb &aForeground,
                                TRgb &aBackground) const;

    /** 
     * Gets the final character for a character cell, taking FEP editor into
     * account.
     * 
     * @param aX Character cell X-coordinate
     * @param aY Character cell Y-coordinate
     * 
     * @return Final character for the cell.
     */
    virtual TText FinalChar(TInt aX, TInt aY) const;


protected:

    /**
     * Attributes for a single character on the terminal.
     */
    struct TTerminalAttribute {
        TRgb iFgColor;
        TRgb iBgColor;
        TBool iBold;
        TBool iUnderline;
    };


    // Internal methods    
    virtual void Resize();
    virtual void Clear();
    virtual void AllocateBuffersL();
    virtual void GetSelectionInScanOrder(TInt &aStartX, TInt &aStartY,
                                         TInt &aEndX, TInt &aEndY) const;
    virtual void UpdateFepEditDisplay();

    virtual void UpdateDisplay(TInt aX, TInt aY, TInt aLength) = 0;

    
    TInt iFontWidth, iFontHeight; // font char dimensions in pixels
    MTerminalObserver &iObserver;
    TBool iGrayed;
    TText *iChars;
    TTerminalAttribute *iAttributes;
    TInt iCharWidth, iCharHeight;
    TInt iCursorX, iCursorY;
    TBool iSelectMode;
    TInt iSelectX, iSelectY; // selection cursor position
    TInt iMarkX, iMarkY; // mark position;
    TBool iHaveSelection;
    TUint iNextKeyModifiers;
    TBool iCtrlDown;

    TBool iFepEditActive;
    TBuf<64> iFepEditBuf;
    TInt iFepCursorPos;
    TBool iFepCursorVisible;
#ifdef PUTTY_S60
    CTermFepExt1 *iFepExt1;
#endif
    TInt iFepEditOrigX, iFepEditOrigY; // FEP edit reference pos
    TInt iFepEditX, iFepEditY; // Current FEP edit pos, -1 if invalid
    TInt iFepEditDisplayLen;
    TRgb iDefaultFg, iDefaultBg;
#ifdef PUTTY_S60TOUCH
    void ConvertToCharPos(TPoint& aPoint);
    void DetermineCursorMove(TPoint& aPointOld, TPoint& aPointNew);
    void MoveCursorToPoint(TPoint& aPointStart, TPoint& aPointEnd);
    TPoint iLastPoint;
    TPoint iOrig;
    TBool iPointerSelectEnabled;
    
    TBool iLastKeyArrow;
    
    //We need to allways check if we have anough space left in the buffer if not we need to expand the buffer.
    void CheckDisplayBufferSizeL(const TDesC &aText);
    void CheckDisplayBufferSizeL(); //Tests if we have space for one more character.
    HBufC* iDisplayBuf;
    TInt iDisplayCursorPos;

    void TestForModifiers(const TKeyEvent aKeyEvent);
    void TestForModifiers();
    TBool iCtrlModifier;
    TBool iAltModifier;
    
#ifdef PUTTY_SYM3_TEST50
    CDesCArray *iHttpList;
#endif
    
#endif
};


#endif
