/*    sendgrid.h
 *
 * S60 "Send" grid control
 *
 * Copyright 2008 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __SENDGRID_H__
#define __SENDGRID_H__

#include <e32std.h>
#include <coecntrl.h>
#include <eikcmobs.h>


#define KSendGridNumItems 12


class CAknGrid;
class CEikButtonGroupContainer;


/**
 * Send grid observer class, gets notified of grid commands and termination.
 */
class MSendGridObserver {

public:
    /** 
     * The user selected a command. This also means the grid is no longer
     * useful and can be deleted.
     * 
     * @param aCommand Command ID
     */
    virtual void MsgoCommandL(TInt aCommand) = 0;

    /** 
     * The user terminated the grid. The client can now safely delete the
     * grid control.
     */
    virtual void MsgoTerminated() = 0;
};


/**
 * S60 "Send" grid control. The send grid displays a grid control in
 * window-owning container, and prompts the user to select a command. The
 * user can use the phone keypad keys for shortcuts.
 *
 * The send grid does not handle layout changes very well, and it's usually
 * best to destroy the grid when UI layout changes.
 */
class CSendGrid : public CCoeControl, public MEikCommandObserver {

public:    
    /** 
     * Gets the recommended size in pixels for the send grid in the current UI
     * layout.
     * 
     * @param aSize Size in pixels
     */
    static void GetRecommendedSize(TSize &aSize);
    
    /** 
     * Factory method, creates a new CSendGrid instance
     * 
     * @param aRect The display rectangle for the send grid
     * @param aResourceId Resource ID for the grid resource. The resource must
     *                    be of type SENDGRID and include 12 items.
     * @param aObserver The grid observer
     * 
     * @return A new CSendGrid instance
     */
    static CSendGrid *NewL(const TRect &aRect, TInt aResourceId,
                           MSendGridObserver &aObserver);
    
    /** 
     * Destructor.
     */
    virtual ~CSendGrid();

    // CCoeControl methods
    TInt CountComponentControls() const;
    CCoeControl *ComponentControl(TInt aIndex) const;
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    void Draw(const TRect &aRect) const;

    // From MEikCommandObserver
    void ProcessCommandL(TInt aCommandId);
    
#ifdef PUTTY_SYM3
    void HandlePointerEventL( const TPointerEvent& aEvent );
#endif
    
private:
    CSendGrid(MSendGridObserver &aObserver);
    void ConstructL(const TRect &aRect, TInt aResourceId);
    void HandleSelectionL(TInt aIndex);
    void SetItemsL(TInt aResourceId);
    
    CAknGrid *iGrid;
    MSendGridObserver &iObserver;
    TInt iCommands[KSendGridNumItems];
    TInt iSubGrids[KSendGridNumItems];
    CEikButtonGroupContainer *iCba;
    CDesCArrayFlat *iItemArray;
    TBool iSetCba;
    TBool iObsCba0;
    TBool iObsCba1;
    RArray<TInt> iGridStack;
    TInt iCurrentGrid;
};


#endif
