/*    terminalview.h
 *
 * Putty terminal view
 *
 * Copyright 2007,2009 Petteri Kangaslampi
 * Portions copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#ifndef TERMINALVIEW_H
#define TERMINALVIEW_H

#include <aknview.h>
#include <aknwaitdialog.h>
#include "puttyclient.h"
#include "terminalcontrol.h"
#include "netconnect.h"
#include "sendgrid.h"

// Forward declarations
class CPuttyEngine;
class CTerminalContainer;
#ifdef PUTTY_S60TOUCH
#include "touchuisettings.h"
class TTouchSettings;
#endif
/**
 * PuTTY terminal view. The terminal view is tha view that handles the actual
 * SSH connection, from network connection establishment to disconnection.
 */
class CTerminalView : public CAknView, public MPuttyClient,
                      public MTerminalObserver, public MNetConnectObserver,
                      public MProgressDialogCallback,
                      public MSendGridObserver {

public:
    /** 
     * Factory method.
     *
     * @return A new CTerminalView instance.
     */
    static CTerminalView *NewL();

    /** 
     * Destructor.
     */
    ~CTerminalView();

    /** 
     * Handles the Enter key (center of joystick), depending on terminal state.
     * Called from the terminal container.
     * 
     * @return ETrue if key handled, EFalse if the container should handle it
     */
    TBool HandleEnterL();

public: // From CAknView
    TUid Id() const;
    void HandleCommandL(TInt aCommand);
    void DoActivateL(const TVwsViewId &aPrevViewId, TUid aCustomMessageId,
                     const TDesC8 &aCustomMessage);
    void DoDeactivate();
    void HandleStatusPaneSizeChange();
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
#ifdef PUTTY_S60TOUCH
    //Forward generated keypresses to putty engine
    void SendKeypress(TKeyCode aCode, TUint aModifiers);
    void SetReleaseAltAfterKeyPress(TBool aValue); //Default state off
    void SetReleaseCtrlAfterKeyPress(TBool aValue); //Default state off
    TInt MouseMode();
    void MouseClick(TInt modifiers, TInt row, TInt col);

#endif
private: // Constructors
    CTerminalView();
    void ConstructL();

private: // From MPuttyEngine
    void DrawText(TInt aX, TInt aY, const TDesC &aText, TBool aBold,
                  TBool aUnderline, TRgb aForeground,
                  TRgb aBackground);
    void SetCursor(TInt aX, TInt aY);
    void ConnectionError(const TDesC &aMessage);
    void FatalError(const TDesC &aMessage);
    void ConnectionClosed();
    THostKeyResponse UnknownHostKey(const TDesC &aFingerprint);
    THostKeyResponse DifferentHostKey(const TDesC &aFingerprint);
    TBool AcceptCipher(const TDesC &aCipherName,
                       const TDesC &aCipherType);
    TBool AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                               TBool aSecret);

private: // From MTerminalObserver
    void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    void KeyPressed(TKeyCode aCode, TUint aModifiers);

private: // From MNetConnectObserver
    void NetConnectComplete(TInt aError, RSocketServ &aSocketServ,
                            RConnection &aConnection);

private: // From MProgressDialogCallback
    void DialogDismissedL(TInt aButtonId);

private: // From MSendGridObserver
    void MsgoCommandL(TInt aCommand);
    void MsgoTerminated();

private:
    void FatalError(TInt aResourceId);
    void DoDisconnectL();
    void DoConnectL();
    void DoConnect();
    static TInt ConnectIdleCallback(void *aAny);
    void ConnectionErrorL(const TDesC &aMessage);
    void FatalErrorL(const TDesC &aMessage);
    MPuttyClient::THostKeyResponse HostKeyDialogL(const TDesC &aFingerprint,
                                                  TInt aDialogFormatRes);
    TBool AcceptCipherL(const TDesC &aCipherName, const TDesC &aCipherType);
    TBool AuthenticationPromptL(const TDesC &aPrompt, TDes &aTarget,
                                TBool aSecret);
    TBool QuerySendTextL(TInt aPrompt, TBool aPermitPredictive,
                         TUint aModifiers);
    void SetFullScreenL(TBool aFullScreen);
    void SetFontL();
    void SetPaletteL();
#ifdef PUTTY_S60TOUCH
    TInt PopUpListViewL(TInt aResourceId, TInt aSelectedItem);
    void SetToolbarButtonL(); //Show toolbar settings
    void SetTouchSettingsL(); //Show touch settings
    void SetGeneralToolbarSettingsL(); //Show general toolbar settings
    TInt CheckGestureMap (TInt aCmd, TBool aItemToCmd);
#endif

private:
    enum {
        EStateNone,
        EStateNetConnecting,
        EStateNetConnected,
        EStateConnecting,
        EStateConnected
    } iState;
    
    CPuttyEngine *iPutty;
    CTerminalContainer *iContainer;
    TBuf<25> iDataDirectory; // "x:\private\12345678\data\"
    TBuf<23> iFontDirectory; // "x:\resource\puttyfonts\"
    TFileName iFontFile;
    CNetConnect *iNetConnect;
    CAknWaitDialog *iNetConnectWaitDialog;
    CIdle *iConnectIdle;
    HBufC *iConnectionError;
    TBool iFullScreen;
    TInt iLastCommand;
    CSendGrid *iSendGrid;
    TBool iSelection;
    TBool iMark;
#ifdef PUTTY_S60TOUCH
    TBool iReleaseAltAfterKey;
    TBool iReleaseCtrlAfterKey;
#endif    
};


#endif
