/*    puttyappui.h
 *
 * Putty UI Application UI class
 *
 * Copyright 2002,2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYAPPUI_H__
#define __PUTTYAPPUI_H__

#include <eikappui.h>
#include <es_sock.h>
#include "puttyclient.h"
#include "terminalcontrol.h"
#include "audiorecorder.h"
#include "netconnect.h"
extern "C" {
#include "putty.h" // struct Config
}

class CPuttyAppView;
class CPuttyEngine;
class CEikMenuPane;
class CPalettes;


/**
 * PuTTY UI Application UI class. Contains most of the UI logic, including
 * engine and terminal callbacks.
 */
class CPuttyAppUi: public CEikAppUi, public MPuttyClient,
                   public MTerminalObserver, public MNetConnectObserver,
                   public MRecorderObserver {
    
public:
    void ConstructL();
    CPuttyAppUi();
    ~CPuttyAppUi();

    virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

    // MNetConnectObserver methods
    virtual void NetConnectComplete(TInt aError,
                                    RSocketServ &aSocketServ,
                                    RConnection &aConnection);

    // MRecorderObserver methods
    virtual void RecordCompleted(TInt anError);
    
    // MPuttyClient methods
    virtual void DrawText(TInt aX, TInt aY, const TDesC &aText, TBool aBold,
                          TBool aUnderline, TRgb aForeground,
                          TRgb aBackground);
    virtual void SetCursor(TInt aX, TInt aY);
    virtual void ConnectionError(const TDesC &aMessage);
    virtual void FatalError(const TDesC &aMessage);
    virtual void ConnectionClosed();
    virtual THostKeyResponse UnknownHostKey(const TDesC &aFingerprint);
    virtual THostKeyResponse DifferentHostKey(const TDesC &aFingerprint);
    virtual TBool AcceptCipher(const TDesC &aCipherName,
                               const TDesC &aCipherUsage);
    virtual TBool AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                                       TBool aSecret);

    // MTerminalObserver methods
    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers);

    void HandleCommandL(TInt aCommand);
    TBool OfferSelectKeyL();
    
private:
    void ConnectionErrorL(const TDesC &aMessage);
    void FatalErrorL(const TDesC &aMessage);
    THostKeyResponse HostKeyDialogL(const TDesC &aFingerprint,
                                    TInt aDialogTitleRes,
                                    TInt aDialogFormatRes);
    TBool AcceptCipherL(const TDesC &aCipherName,
                        const TDesC &aCipherUsage);
    void ReadUiSettingsL(Config *aConfig);
    static TInt ConnectToProfileCallback(TAny *aAny);
    void DoConnectToProfile();
    void DoConnectToProfileL();
    
private:
    CPuttyAppView *iAppView;
    CPuttyEngine *iEngine;
    TInt iTermWidth, iTermHeight;
    HBufC *iFatalErrorPanic;
    TBool iLargeFont;
    TBool iFullScreen;
    CNetConnect *iNetConnect;
    CAudioRecorder *iRecorder;
    HBufC8 *iAudio;
    TPtr8 iAudioRecordDes;
    TBool iRecording;
    TFileName iDataPath;
    TBool iSelectMode;
    TBool iHaveMark;
    CIdle *iConnectIdle;
    CPalettes *iPalettes;

    enum {
        EStateNone = 0,
        EStateNetConnecting,
        EStateConnecting,
        EStateConnected,
        EStateDisconnected
    } iState;
};


#endif
