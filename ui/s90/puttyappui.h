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
#include "puttyclient.h"
#include "terminalcontrol.h"
#include "dialler.h"
#include "audiorecorder.h"
extern "C" {
#include "putty.h" // struct Config
}

class CPuttyAppView;
class CPuttyEngine;
class CEikMenuPane;


/**
 * PuTTY UI Application UI class. Contains most of the UI logic, including
 * engine and terminal callbacks.
 */
class CPuttyAppUi: public CEikAppUi, public MPuttyClient,
                   public MTerminalObserver, public MDialObserver,
                   public MRecorderObserver {
    
public:
    void ConstructL();
    CPuttyAppUi();
    ~CPuttyAppUi();

    virtual TBool ProcessCommandParametersL(TApaCommand aCommand,
                                            TFileName &aDocumentName,
                                            const TDesC8 &aTail);
    virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

    // MDialObserver methods
    virtual void DialCompleted(TInt anError);

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
                               TCipherDirection aDirection);
    virtual TBool AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                                       TBool aSecret);

    // MTerminalObserver methods
    virtual void TerminalSizeChanged(TInt aWidth, TInt aHeight);
    virtual void KeyPressed(TKeyCode aCode, TUint aModifiers);
    virtual void RePaintWindow();

    void HandleCommandL(TInt aCommand);
    
private:
    void ConnectionErrorL(const TDesC &aMessage);
    void FatalErrorL(const TDesC &aMessage);
    THostKeyResponse HostKeyDialogL(const TDesC &aFingerprint,
                                    TInt aDialogTitleRes,
                                    TInt aDialogFormatRes);
    TBool AcceptCipherL(const TDesC &aCipherName,
                        TCipherDirection aDirection);
    void ReadUiSettingsL(Config *aConfig);
    void WriteUiSettingsL(Config *aConfig);
    
private:
    CPuttyAppView *iAppView;
    CPuttyEngine *iEngine;
    TInt iTermWidth, iTermHeight;
    HBufC *iFatalErrorPanic;
    TBool iLargeFont;
    TBool iFullScreen;
    CDialler *iDialler;
    CAudioRecorder *iRecorder;
    HBufC8 *iAudio;
    TPtr8 iAudioRecordDes;
    TBool iRecording;
    TFileName iDataPath;
    TFileName iInstallPath;

    enum {
        EStateNone = 0,
        EStateDialing,
        EStateConnecting,
        EStateConnected,
        EStateDisconnected
    } iState;
};


#endif
