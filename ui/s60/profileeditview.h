/*    profileeditview.h
 *
 * Putty profile edit view
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITVIEW_H
#define PROFILEEDITVIEW_H

#include <aknview.h>
#include <eiklbo.h> 
#include "puttyclient.h"

// Forward declarations
class CPuttyEngine;
class CAknNavigationDecorator;


/**
 * PuTTY profile edit view. The profile edit view lets the user edit the
 * profile name and other settings.
 */
class CProfileEditView : public CAknView, public MEikListBoxObserver {

public:
    /** 
     * Factory method.
     *
     * @return A new CProfileEditView instance.
     */
    static CProfileEditView *NewL();

    /** 
     * Destructor.
     */
    ~CProfileEditView();

public: // From CAknView
    TUid Id() const;
    void HandleCommandL(TInt aCommand);
    void DoActivateL(const TVwsViewId &aPrevViewId, TUid aCustomMessageId,
                     const TDesC8 &aCustomMessage);
    void DoDeactivate();
    void HandleStatusPaneSizeChange();
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

public: // From MEikListBoxObserver
    void HandleListBoxEventL(CEikListBox *aListBox, TListBoxEvent aEventType);

private: // Constructors
    CProfileEditView();
    void ConstructL();

private:
    // Current view mode -- page list or one of the settings pages
    enum TView {
        EViewPageList,
        EViewGeneral,
        EViewSsh,
        EViewDisplay,
#ifdef PUTTY_S60TOUCH
        EViewTouch,
        EViewGeneralToolbar,
        EViewToolbar,
#endif
        EViewLogging // the order must match the listbox
    };

    // Helper active object to switch views asynchronously. This is used to
    // break the call stack when switching away from the page list view from
    // within the listbox observer callback.
    class CViewSwitcher : public CActive {
    public:
        CViewSwitcher(CProfileEditView &aView);
        ~CViewSwitcher();
        void SwitchView(TView aNewView);
        void RunL();
        void DoCancel();
    private:
        TView iNewView;
        CProfileEditView &iView;
    };
    friend class CViewSwitcher;    

private: // Helper functions
    void SetViewL(TView aView);

private:
    TView iView;
    
    CCoeControl *iControl;
    CPuttyEngine *iPutty;
    TDes *iProfileName;
    CViewSwitcher *iSwitcher;
    CAknNavigationDecorator *iDecorator;
};


#endif
