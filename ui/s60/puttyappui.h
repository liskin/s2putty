/*    puttyappui.h
 *
 * Putty UI Application UI class
 *
 * Copyright 2003 Sergei Khloupnov
 * Copyright 2002,2003,2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PUTTYAPPUI_H
#define PUTTYAPPUI_H

#include <aknviewappui.h>


// Forward declarations
class CProfileListView;
class CProfileEditView;
class CTerminalView;
class CPuttyEngine;
class CAknNavigationControlContainer;


/**
 * PuTTY application UI class. This class mainly constructs views and contains
 * some minimal global logic. Most application logic is placed in views.
 */
class CPuttyAppUi : public CAknViewAppUi {

public:
    /** 
     * Second-phase constructor.
     */
    void ConstructL();

    /** 
     * Destructor.
     */
    ~CPuttyAppUi();

    /** 
     * Activates the profile list view.
     */
    void ActivateProfileListViewL();

    /** 
     * Activates the profile edit view. This is typically called from the
     * profile list view only.
     * 
     * @param aPutty The PuTTY engine instance to use. The correct settings
     *               file must have already been loaded. The profile edit view
     *               will update the settings in the engine. The reference must
     *               remain valid as long as the view is active.
     * @param aProfileName Profile name, may be modified. The reference must
     *                     remain valid as long as the view is active.
     */
    void ActivateProfileEditViewL(CPuttyEngine &aPutty, TDes &aProfileName);

    /** 
     * Gets profile edit view data. This is typically called from the profile
     * edit view upon activation, and will return the data written in using
     * ActivateProfileEditViewL().
     * 
     * @param aPutty The PuTTY engine instance to use.
     * @param aProfileName Profile name
     *
     * @see ActivateProfileEditViewL
     */
    void GetProfileEditDataL(CPuttyEngine *&aPutty, TDes *&aProfileName);

    /** 
     * Activates the terminal view, using a specified profile to connect to.
     * This is typically called from the profile list view only.
     * 
     * @param aProfileFile The profile file to use. The app UI makes a copy
     *                     of the contents, so using a temporary descriptor
     *                     is safe.
     */
    void ActivateTerminalViewL(TDesC &aProfileFile);

    /** 
     * Gets the profile file to use for initializing the engine for the
     * connection. Called from the terminal view only after being activated
     * from the profile list view.
     * 
     * @return Profile file name, including full path
     */
    const TDesC &TerminalProfileFile();

    /** 
     * Retrieves the profile directory, <drive>:\private\<SID>\profiles\
     *
     * @return Profile directory, including a trailing backslash
     */
    const TDesC &ProfileDirectory();

    /** 
     * Retrieves the data directory, <drive>:\private\<SID>\data\
     *
     * @return Data directory, including a trailing backslash
     */
    const TDesC &DataDirectory();

    /** 
     * Retrieves the font directory, <drive>:\resource\puttyfonts\
     *
     * @return Font directory, including a trailing backslash
     */
    const TDesC &FontDirectory();

    /** 
     * Retrieves the navi pane
     * 
     * @return Navigation control container
     */
    CAknNavigationControlContainer &NaviPane();

    /** 
     * Retrieves a list of available fonts
     * 
     * @return Font array
     */
    const CDesCArray &Fonts();

    
public: // from CAknAppUi
    void HandleCommandL(TInt aCommand);

private:
    CProfileListView *iProfileListView;
    CProfileEditView *iProfileEditView;
    CTerminalView *iTerminalView;

    CPuttyEngine *iProfileEditPutty;
    TDes *iProfileEditName;

    TFileName iTerminalProfileFile;

    TBuf<29> iProfileDirectory; // "x:\private\12345678\profiles\"
    TBuf<25> iDataDirectory; // "x:\private\12345678\data\"
    TBuf<23> iFontDirectory; // "x:\resource\puttyfonts\"

    CDesCArray *iFonts;
    CAknNavigationControlContainer *iNaviPane;
};


#endif
