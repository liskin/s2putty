/*    terminalview.cpp
 *
 * Putty terminal view
 *
 * Copyright 2007,2009 Petteri Kangaslampi
 * Portions copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#include <aknviewappui.h>
#include <putty.rsg>
#include <akntitle.h>
#include <aknnotedialog.h>
#include <aknquerydialog.h>
#include <bautils.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <aknlists.h>
#include <baclipb.h>
#include "terminalview.h"
#ifdef PUTTY_S60TOUCH
    #include "../s60v5/terminalcontainer.h"
#else
    #include "terminalcontainer.h"
#endif
#ifdef PUTTY_SYM3
    #include <txtclipboard.h>
#endif
#include "puttyappui.h"
#include "puttyengine.h"
#include "puttyuids.hrh"
#include "puttyui.hrh"
#include "stringutils.h"
#include "palettes.h"

_LIT(KAssertPanic, "termview");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))

_LIT(KFatalErrorPanic, "Fatal Error");
_LIT(KDefaultFont, "fixed6x13");
_LIT(KFontExtension, ".s2f");
const TInt KFatalErrorExit = -1;
const TInt KConnectionErrorExit = -2;
static const TInt KFullScreenWidth = 0xf5;


// Factory
CTerminalView *CTerminalView::NewL() {
    CTerminalView *self = new (ELeave) CTerminalView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
CTerminalView::CTerminalView() {
}


// Second-phase constructor
void CTerminalView::ConstructL() {
    BaseConstructL(R_PUTTY_TERMINAL_VIEW);
#ifdef PUTTY_S60TOUCH
    iReleaseAltAfterKey = EFalse;
    iReleaseCtrlAfterKey = EFalse;
#endif

}


// Destructor
CTerminalView::~CTerminalView() {
    if ( iSendGrid ) {
        AppUi()->RemoveFromStack(iSendGrid);
        delete iSendGrid;
    }
    delete iPutty;
    delete iContainer;
    delete iConnectIdle;
    delete iNetConnect;
    delete iConnectionError;
}


// CAknView::Id()
TUid CTerminalView::Id() const {
    return TUid::Uid(KUidPuttyTerminalViewDefine);
}


// CAknView::HandleCommandL
void CTerminalView::HandleCommandL(TInt aCommand) {

    // Map from commands to simple keypresses
    static struct {
        TInt iCommand;
        TKeyCode iKeyCode;
        TUint iModifiers;
    } KCommandKeyMap[] = {
        { EPuttyCmdSendEsc, (TKeyCode) 0x1b, 0 },
        { EPuttyCmdSendPipe,  (TKeyCode) '|', 0 },
        { EPuttyCmdSendBackquote,  (TKeyCode) '`', 0 },
        { EPuttyCmdSendCR,  EKeyEnter, 0 },
        { EPuttyCmdSendSpace,  (TKeyCode) ' ', 0 },
        { EPuttyCmdSendTab, (TKeyCode) '\t', 0 },
        { EPuttyCmdSendPageUp, EKeyPageUp, 0 },
        { EPuttyCmdSendPageDown, EKeyPageDown, 0 },
        { EPuttyCmdSendHome, EKeyHome, 0 },
        { EPuttyCmdSendEnd, EKeyEnd, 0 },
        { EPuttyCmdSendInsert, EKeyInsert, 0 },
        { EPuttyCmdSendDelete, EKeyDelete, 0 },
        { EPuttyCmdSendAlt1, (TKeyCode) '1', EModifierAlt },
        { EPuttyCmdSendAlt2, (TKeyCode) '2', EModifierAlt },
        { EPuttyCmdSendAlt3, (TKeyCode) '3', EModifierAlt },
        { EPuttyCmdSendAlt4, (TKeyCode) '4', EModifierAlt },
        { EPuttyCmdSendAlt5, (TKeyCode) '5', EModifierAlt },
        { EPuttyCmdSendAlt6, (TKeyCode) '6', EModifierAlt },
        { EPuttyCmdSendAlt7, (TKeyCode) '7', EModifierAlt },
        { EPuttyCmdSendAlt8, (TKeyCode) '8', EModifierAlt },
        { EPuttyCmdSendAlt9, (TKeyCode) '9', EModifierAlt },
        { EPuttyCmdSendAlt0, (TKeyCode) '0', EModifierAlt },
        { EPuttyCmdSendCtrlBrkt, (TKeyCode) 0x1d, 0 },
        { EPuttyCmdSendCtrlC, (TKeyCode) 0x03, 0, },
        { EPuttyCmdSendCtrlD, (TKeyCode) 0x04, 0 },
        { EPuttyCmdSendCtrlZ, (TKeyCode) 26,0 },
        { EPuttyCmdSendF1, EKeyF1, 0 },
        { EPuttyCmdSendF2, EKeyF2, 0 },
        { EPuttyCmdSendF3, EKeyF3, 0 },
        { EPuttyCmdSendF4, EKeyF4, 0 },
        { EPuttyCmdSendF5, EKeyF5, 0 },
        { EPuttyCmdSendF6, EKeyF6, 0 },
        { EPuttyCmdSendF7, EKeyF7, 0 },
        { EPuttyCmdSendF8, EKeyF8, 0 },
        { EPuttyCmdSendF9, EKeyF9, 0 },
        { EPuttyCmdSendF10, EKeyF10, 0},        
        { 0, (TKeyCode) 0, 0 } // must be the last item
    };
   

    // Don't handle any commands if we're waiting for the connect idle to
    // kick in. Getting here during that period is just about impossible
    // anyway.
    if ( iConnectIdle ) {
        return;
    }

    // The Send (green) key repeats the last command, but only those with
    // ID below EPuttyCmdNotRepeated
    if ( aCommand == EPuttyCmdRepeatLast ) {
        aCommand = iLastCommand;
    } else if ( aCommand < EPuttyCmdNotRepeated ) {
        iLastCommand = aCommand;
    }
    
    switch ( aCommand ) {

        case EPuttyCmdSend:
            if ( (iState == EStateConnected) && (!iSendGrid) ) {
                // Show the send grid, centered at the lower part of the window
                TSize size;
                CSendGrid::GetRecommendedSize(size);
                TRect cr = ClientRect();
                if ( size.iWidth > cr.Width() ) {
                    size.iWidth = cr.Width();
                }
                if ( size.iHeight > cr.Height() ) {
                    size.iHeight = cr.Height();
                }
                TPoint tl = cr.iTl + TSize((cr.Width() - size.iWidth) / 2,
                                           (3*(cr.Height() - size.iHeight)) / 4);
                TPoint br = cr.iBr - TSize((cr.Width() - size.iWidth) / 2,
                                           (cr.Height() - size.iHeight) / 4);
                iSendGrid = CSendGrid::NewL(TRect(tl, br), R_PUTTY_SEND_GRID,
                                            *this);
                AppUi()->AddToStackL(iSendGrid);                
            }
            break;

        case EPuttyCmdDisconnect:
            if ( CAknQueryDialog::NewL()->ExecuteLD(
                    R_PUTTY_CONFIRM_DISCONNECT_QUERY) ) {
                DoDisconnectL();
            }
            break;

        case EPuttyCmdSendLine: {
            if ( iState == EStateConnected ) {
                if ( QuerySendTextL(R_PUTTY_STR_LINE, ETrue, 0) ) {
                    iPutty->SendKeypress(EKeyEnter, 0);
                }
            }
            break;
        }

        case EPuttyCmdSendText: {
            if ( iState == EStateConnected ) {
                QuerySendTextL(R_PUTTY_STR_TEXT, ETrue, 0);
            }
            break;
        }
        
        case EPuttyCmdSendAltKeys: {
            if ( iState == EStateConnected ) {
                QuerySendTextL(R_PUTTY_STR_ALT_KEYS, ETrue, EModifierAlt);
            }
            break;
        }
        
        case EPuttyCmdSendCtrlKeys: {
            if ( iState == EStateConnected ) {
                QuerySendTextL(R_PUTTY_STR_CTRL_KEYS, ETrue, EModifierCtrl);
            }
            break;
        }

        case EPuttyCmdSendCtrlAD: {
            if ( iState == EStateConnected ) {
                iPutty->SendKeypress((TKeyCode)0x01, 0); // Ctrl-A
                iPutty->SendKeypress((TKeyCode)'d', 0);
            }
            break;
        }
#ifdef PUTTY_S60TOUCH
        case EPuttyCmdSendCtrlAN: {
            if ( iState == EStateConnected ) {
                iPutty->SendKeypress((TKeyCode)0x01, 0); // Ctrl-A
                iPutty->SendKeypress((TKeyCode)'n', 0);
            }
            break;
        }
        case EPuttyCmdSendCtrlAP: {
            if ( iState == EStateConnected ) {
                iPutty->SendKeypress((TKeyCode)0x01, 0); // Ctrl-A
                iPutty->SendKeypress((TKeyCode)'p', 0);
            }
            break;
        }
        case EPuttyCmdSendAltALeft: {
            if ( iState == EStateConnected ) {
                iPutty->SendKeypress(EKeyLeftArrow, EModifierAlt); // Alt-ArrowLeft
            }
            break;
        }
        case EPuttyCmdSendAltARight: {
            if ( iState == EStateConnected ) {
                iPutty->SendKeypress(EKeyRightArrow, EModifierAlt); // Alt-ArrowRight
            }
            break;
        }
        case EPuttyCmdToolbarSettings: {
            SetToolbarButtonL();
            break;
        }
        case EPuttyCmdGeneralToolbar: {
            SetGeneralToolbarSettingsL();
            break;        
        }
        case EPuttyCmdTouchSettings: {
            SetTouchSettingsL();
            break;
        }
#endif
        case EPuttyCmdFullScreen: {
            SetFullScreenL(iFullScreen ? (TBool)EFalse : (TBool)ETrue);
            break;
        }

        case EPuttyCmdSetFont: {
            SetFontL();
            break;
        }

        case EPuttyCmdSetPalette: {
            SetPaletteL();
            break;
        }

        case EPuttyCmdSelect:
            if ( iState == EStateConnected ) {
                // activate selection (if not active)
                if ( !iSelection ) {
#ifdef PUTTY_S60TOUCH
                    iContainer->Select(ETrue);
#endif
                    iSelection = ETrue;
                    iContainer->Terminal().SetSelectMode(ETrue);
                    iMark = EFalse;
                    CEikButtonGroupContainer::Current()->
                        SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_CANCEL);
                    CEikButtonGroupContainer::Current()->DrawDeferred();
                } else {
                    // selection already active -- stop
#ifdef PUTTY_S60TOUCH
                    iContainer->Select(EFalse);
#endif
                    iSelection = EFalse;
                    iMark = EFalse;
                    iContainer->Terminal().RemoveMark();
                    iContainer->Terminal().SetSelectMode(EFalse);
                    CEikButtonGroupContainer::Current()->
                        SetCommandSetL(R_PUTTY_TERMINAL_SOFTKEYS);
                    CEikButtonGroupContainer::Current()->DrawDeferred();
                }
            }
            break;

        case EPuttyCmdMark:
            if ( (iState == EStateConnected) && iSelection ) {
                iContainer->Terminal().SetMark();
                iMark = ETrue;
            }
            break;

        case EPuttyCmdCopy:
#ifdef PUTTY_S60TOUCH
            if ( (iState == EStateConnected) ) {
            if ( iContainer->Terminal().DoWeHaveSelection() ) {          
#else
            if ( (iState == EStateConnected) && iMark ) {
#endif
                // get selection
                const HBufC *text = iContainer->Terminal().CopySelectionLC();
                if ( text->Length() > 0 ) {
                    // put selection on clipboard
                    CClipboard* cb = CClipboard::NewForWritingLC(
                        CCoeEnv::Static()->FsSession());
                    cb->StreamDictionary().At(KClipboardUidTypePlainText);    			 
                    CPlainText *plainText = CPlainText::NewL();
                    CleanupStack::PushL(plainText);    			 
                    plainText->InsertL(0, *text);    				    			 
                    plainText->CopyToStoreL(cb->Store(),
                                            cb->StreamDictionary(),
                                            0,
                                            plainText->DocumentLength());    			 
                    CleanupStack::PopAndDestroy(); // plainText
                    cb->CommitL();
                    CleanupStack::PopAndDestroy(); // cb
                }
                CleanupStack::PopAndDestroy(); // text
            }
            // terminate selection mode
#ifdef PUTTY_S60TOUCH
            } //!iContainer->Terminal().DoWeHaveSelection() ends
            iContainer->Select(EFalse);
#endif
            iSelection = EFalse;
            iMark = EFalse;
            iContainer->Terminal().RemoveMark();
            iContainer->Terminal().SetSelectMode(EFalse);
            CEikButtonGroupContainer::Current()->
                SetCommandSetL(R_PUTTY_TERMINAL_SOFTKEYS);
            CEikButtonGroupContainer::Current()->DrawDeferred();
            break;
        	
        case EPuttyCmdPaste:
            if ( iState == EStateConnected ) {
                // get text from clipboard
                CPlainText *plainText = CPlainText::NewL();
                CleanupStack::PushL(plainText);
                
            	CClipboard* cb = CClipboard::NewForReadingL(
                    CCoeEnv::Static()->FsSession());
            	CleanupStack::PushL(cb);
            	cb->StreamDictionary().At(KClipboardUidTypePlainText);

            	plainText->PasteFromStoreL(cb->Store(), cb->StreamDictionary(),
                                           0);

            	// paste text (if any) to terminal
            	if ( plainText->DocumentLength() > 0 ) {
                    HBufC *buf = HBufC::NewLC(plainText->DocumentLength());
                    TPtr ptr = buf->Des();
                    plainText->Extract(ptr, 0, plainText->DocumentLength());

                    int i = 0;
                    int len = ptr.Length();
                    while ( i < len ) {
                        TKeyCode key = (TKeyCode)ptr[i++];
                        if ( (key == (TKeyCode)CEditableText::ELineBreak) ||
                             (key == (TKeyCode)CEditableText::EParagraphDelimiter)) {
                            iPutty->SendKeypress(EKeyEnter, 0);
                        } else {
                            iPutty->SendKeypress(key, 0);
                        }
                    }
                    CleanupStack::PopAndDestroy(); // buf                    
            	}
            	CleanupStack::PopAndDestroy(2); // cb, plainText
            }
            break;

        case EAknSoftkeyCancel:
            // Cancel selection
            if ( iSelection ) {
#ifdef PUTTY_S60TOUCH
                iContainer->Select(EFalse);
#endif
                iSelection = EFalse;
                iMark = EFalse;
                iContainer->Terminal().RemoveMark();
                iContainer->Terminal().SetSelectMode(EFalse);
                CEikButtonGroupContainer::Current()->
                    SetCommandSetL(R_PUTTY_TERMINAL_SOFTKEYS);
                CEikButtonGroupContainer::Current()->DrawDeferred();
            }
            break;
        
        default:
            // Handle standard "Send Key X" commands
            if ( iState == EStateConnected ) {
                TInt i = 0;
                while ( KCommandKeyMap[i].iCommand != 0 ) {
                    if ( KCommandKeyMap[i].iCommand == aCommand ) {
                        iPutty->SendKeypress(KCommandKeyMap[i].iKeyCode,
                                             KCommandKeyMap[i].iModifiers);
                        return;
                    }
                    i++;
                }
            }

            // Forward unknown commands to the app ui
            AppUi()->HandleCommandL(aCommand);
    }
}


// CAknView::DoActivateL
void CTerminalView::DoActivateL(const TVwsViewId &aPrevViewId,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8 & /*aCustomMessage*/) {

    CEikonEnv *eikenv = CEikonEnv::Static();
    assert(!iContainer);
    assert(!iPutty);
    iState = EStateNone;

    // Build the engine instance to use
    iPutty = CPuttyEngine::NewL(this,
                                ((CPuttyAppUi*)AppUi())->DataDirectory());
    iPutty->ReadConfigFileL(((CPuttyAppUi*)AppUi())->TerminalProfileFile());
    Config *cfg = iPutty->GetConfig();

    // Determine the font to use to begin with
    iFontDirectory = ((CPuttyAppUi*)AppUi())->FontDirectory();
    HBufC *font = StringToBufLC(cfg->font.name);
    iFontFile = iFontDirectory;
    iFontFile.Append(*font);
    iFontFile.Append(KFontExtension);
    CleanupStack::PopAndDestroy(); //font
    if ( (font->Length() == 0) ||
         (!BaflUtils::FileExists(eikenv->FsSession(), iFontFile)) ) {
        // The specified font doesn't exist, use the default.
        // The default font must exist or we're screwed...
        iFontFile = iFontDirectory;
        iFontFile.Append(KDefaultFont);
        iFontFile.Append(KFontExtension);
        if ( !BaflUtils::FileExists(eikenv->FsSession(), iFontFile) ) {
            // Should never happen, but give a reasonable error msg just in
            // case
            FatalError(R_PUTTY_STR_ERROR_DEFAULT_FONT_NOT_FOUND);
        }
    }

    // Check if we should be full-screen
    iFullScreen = (cfg->width == KFullScreenWidth);

    if ( !iContainer ) {
        TRect rect;
        
        if ( iFullScreen ) {
            rect = CEikonEnv::Static()->EikAppUi()->ApplicationRect();
        } else {
            rect = ClientRect();
        }
        iContainer = CTerminalContainer::NewL(rect, this, this, iFontFile);
        iContainer->SetMopParent(this);
        iContainer->SetDefaultColors(TRgb(cfg->colours[0][0],
                                          cfg->colours[0][1],
                                          cfg->colours[0][2]),
                                     TRgb(cfg->colours[2][0],
                                          cfg->colours[2][1],
                                          cfg->colours[2][2]));
        iContainer->ActivateL();
    }

    // Activate the UI control
    AppUi()->AddToStackL(iContainer);
    
    // Set title
    HBufC *title = eikenv->AllocReadResourceL(R_PUTTY_STR_TERMINAL_TITLE);
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
        (StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
    titlePane->SetText(title); //takes ownership

    // Prompt for the host to connect to if not already set
    if ( cfg->host[0] != 0 ) {
        // Host name already set -- just connect.
        // Use a CIdle to kickstart connection setup. We could handle the
        // connection from here just fine, but CAknWaitDialog seems to have
        // a race condition, and it disappears immediately if created from
        // here...
        iConnectIdle = CIdle::NewL(CActive::EPriorityIdle);
        iConnectIdle->Start(TCallBack(ConnectIdleCallback, this));
    } else {
        HBufC *buf = HBufC::NewLC(sizeof(cfg->host));
        TPtr ptr = buf->Des();
        StringToDes(cfg->host, ptr);
        CAknTextQueryDialog *dlg = new (ELeave) CAknTextQueryDialog(ptr);
        dlg->SetPromptL(*(eikenv->AllocReadResourceLC(R_PUTTY_STR_HOST_PROMPT)));
        dlg->PrepareLC(R_PUTTY_STRING_QUERY_DIALOG);
        dlg->SetMaxLength(sizeof(cfg->host));
        if ( dlg->RunLD() ) {
            // Got a hostname -- store and connect
            DesToString(ptr, cfg->host, sizeof(cfg->host));
            iConnectIdle = CIdle::NewL(CActive::EPriorityIdle);
            iConnectIdle->Start(TCallBack(ConnectIdleCallback, this));
        } else {
            // Cancelled -- go back to the profile list view
            DoDisconnectL();
        }
        CleanupStack::PopAndDestroy(2); // prompt, buf
    }
    
    iLastCommand = EPuttyCmdSendEsc;
}


// CAknView::DoDeactivate()
void CTerminalView::DoDeactivate() {

    if ( iSendGrid ) {
        AppUi()->RemoveFromStack(iSendGrid);
        delete iSendGrid;
        iSendGrid = NULL;
    }
    if ( iContainer ) {
        AppUi()->RemoveFromStack(iContainer);
    }
    delete iContainer;
    iContainer = NULL;
    delete iPutty;
    iPutty = NULL;
}


// CAknView::DynInitMenuPaneL
void CTerminalView::DynInitMenuPaneL(TInt aResourceId,
                                     CEikMenuPane *aMenuPane) {
    if ( aResourceId == R_PUTTY_EDIT_MENU ) {
        aMenuPane->SetItemDimmed(EPuttyCmdMark, !iSelection);
        aMenuPane->SetItemDimmed(EPuttyCmdCopy, !(iSelection && iMark));
    }
}


// Disconnect and return to the profile list view
void CTerminalView::DoDisconnectL() {
    // Delete our PuTTY engine instance, since currently multiple
    // simultaneous instances won't work
    delete iPutty;
    iPutty = NULL;
    delete iNetConnect;
    iNetConnect = NULL;
    AppUi()->ActivateLocalViewL(TUid::Uid(KUidPuttyProfileListViewDefine));
}


// Start the connection
void CTerminalView::DoConnectL() {

    delete iConnectIdle;
    iConnectIdle = NULL;
    
    iState = EStateNetConnecting;
    delete iNetConnect;
    iNetConnect = NULL;
    iNetConnect = CNetConnect::NewL(*this);
    iNetConnect->SetPromptAP(iPutty->GetConfig()->accesspoint);
    iNetConnect->Connect();

    // Show wait dialog
    iNetConnectWaitDialog = new (ELeave) CAknWaitDialog(
        (CEikDialog**) &iNetConnectWaitDialog, ETrue);
    iNetConnectWaitDialog->SetCallback(this);
    iNetConnectWaitDialog->ExecuteLD(R_PUTTY_NET_CONNECT_WAIT_DIALOG);
}

void CTerminalView::DoConnect() {
    TRAPD(error, DoConnectL());
    if ( error != KErrNone ) {
        FatalError(*StringLoader::LoadLC(R_PUTTY_STR_FATAL_ERROR_WITH_CODE,
                                         error));
        CleanupStack::PopAndDestroy();
    }
}

// CIdle callback for connection setup
TInt CTerminalView::ConnectIdleCallback(void *aAny) {
    ((CTerminalView*)aAny)->DoConnect();
    return 0;
}


// MProgressDialogCallback::DialogDismissedL()
void CTerminalView::DialogDismissedL(TInt /*aButtonId*/) {
    // Network connection wait dialog dismissed -- cancel connection and
    // return to the profile list view
    if ( iState == EStateNetConnecting ) {
        iNetConnect->CancelConnect();
        iState = EStateNone;
        DoDisconnectL();
    }
}


// MNetConnectObserver::NetConnectComplete()
void CTerminalView::NetConnectComplete(TInt aError, RSocketServ &aSocketServ,
                                       RConnection &aConnection) {

    CEikonEnv *eikenv = CEikonEnv::Static();
    
    // Dismiss the wait dialog
    iState = EStateNetConnected;
    iNetConnectWaitDialog->ProcessFinishedL();
    
    if ( aError != KErrNone ) {
        // An error occurred -- display an error message and return to the
        // profile list view
        if ( aError != KErrCancel ) { // Silently exit if cancelled
            HBufC *err = HBufC::NewLC(128);
            TPtr errp = err->Des();            
            eikenv->GetErrorText(errp, aError);
            HBufC *msg = StringLoader::LoadLC(R_PUTTY_STR_NET_CONNECT_FAILED,
                                              errp, aError, eikenv);
            
            CAknNoteDialog *dlg = new (ELeave) CAknNoteDialog();
            dlg->SetTextL(*msg);
            dlg->ExecuteDlgLD(R_PUTTY_INFO_MESSAGE_DIALOG);

            CleanupStack::PopAndDestroy(2); // err, msg
        }
        
        iState = EStateNone;
        DoDisconnectL();
        return;
    }

    delete iConnectionError;
    iConnectionError = NULL;
    
    // Show a "Connecting to host" note before proceeding. This slows the connection
    // process slightly, but the dialog has to be a modal one to make it
    // visible at all, otherwise it wouldn't get shown before the PuTTY
    // engine connection process starts. Since the engine connection is
    // synchronous and can take quite a while, it's useful to give feedback
    // to the user at this point.
    CAknInformationNote *note = new (ELeave) CAknInformationNote(ETrue);
    note->SetTone(CAknInformationNote::ENoTone);
    note->SetTimeout(CAknInformationNote::EShortTimeout);
    note->ExecuteLD(*(StringLoader::LoadLC(R_PUTTY_STR_CONNECTING_TO_HOST,
                                           eikenv)));
    //We Need to destroy that note msg loaded with LoadLC
    CleanupStack::PopAndDestroy();
    
    TInt err = iPutty->Connect(aSocketServ, aConnection);
    if ( err != KErrNone ) {
        // Error -- show error message and return to the profile list view
        TBuf<128> msg;
        iPutty->GetErrorMessage(msg);
        CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog();
        dlg->SetTextL(msg);
        dlg->ExecuteDlgLD(R_PUTTY_INFO_MESSAGE_DIALOG);
        iState = EStateNone;
        DoDisconnectL();
        return;
    }
    
    iState = EStateConnected;
    
    HandleStatusPaneSizeChange(); //Redraw things some reason sometimes screen stays blank
}


// CAknView::HandleStatusPaneSizeChange()
void CTerminalView::HandleStatusPaneSizeChange() {
    // The send grid doesn't handle size changes well -- just delete it
    if ( iSendGrid ) {
        AppUi()->RemoveFromStack(iSendGrid);
        delete iSendGrid;
        iSendGrid = NULL;
    }
    if ( iContainer && (!iPutty->GetConfig()->resize_action) ) {
        if ( iFullScreen ) {
            iContainer->SetRect(
                CEikonEnv::Static()->EikAppUi()->ApplicationRect());
        } else {
            iContainer->SetRect(ClientRect());
        }
    }
}


// Fatal error, with message from a resource
void CTerminalView::FatalError(TInt aResourceId) {
    CAknNoteDialog* dlg = new(ELeave) CAknNoteDialog();
    HBufC *text = (CCoeEnv::Static())->AllocReadResourceLC(aResourceId);
    dlg->SetTextL(*text);
    dlg->ExecuteDlgLD(R_PUTTY_INFO_MESSAGE_DLG);
    CleanupStack::PopAndDestroy(); //text
    User::Exit(KFatalErrorExit);
}


// Prompt the user for a text string and send it
TBool CTerminalView::QuerySendTextL(TInt aPrompt, TBool aPermitPredictive,
                                     TUint aModifiers) {
    HBufC *buf = HBufC::NewLC(256);
    TPtr ptr = buf->Des();
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(ptr);
    dlg->SetPredictiveTextInputPermitted(aPermitPredictive);
    dlg->SetPromptL(*(StringLoader::LoadLC(aPrompt)));

    TInt ret = EFalse;
    if ( dlg->ExecuteLD(R_PUTTY_SEND_TEXT_DIALOG) ) {
        int i = 0;
        int len = ptr.Length();
        while ( i < len ) {
            iPutty->SendKeypress((TKeyCode)ptr[i++], aModifiers);
        }
        ret = ETrue;
    }
    CleanupStack::PopAndDestroy(2); // buf, prompt
    return ret;
}


// Full or regular screen display
void CTerminalView::SetFullScreenL(TBool aFullScreen) {
    iFullScreen = aFullScreen;
    if ( iFullScreen ) {
        iContainer->SetRect(
            CEikonEnv::Static()->EikAppUi()->ApplicationRect());
    } else {
        iContainer->SetRect(ClientRect());
    }
}

#ifdef PUTTY_S60TOUCH
//Shows popuplist with strings loaded from resources
TInt CTerminalView::PopUpListViewL(TInt aResourceId, TInt aSelectedItem) {
    TInt ret = 0;
    // We'll show a popup list of fonts and let the user select one
    CAknSinglePopupMenuStyleListBox *box = new (ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(box);

    CAknPopupList *popup = CAknPopupList::NewL(box,
                                           R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                           AknPopupLayouts::EMenuWindow);    
    CleanupStack::PushL(popup);
    box->ConstructL(popup, 0);
    box->CreateScrollBarFrameL(ETrue);
    box->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,
                                               CEikScrollBarFrame::EAuto);

    // Add fonts to the listbox
    //CDesCArrayFlat* items = iCoeEnv->ReadDesCArrayResourceL(R_PUTTY_PROFILEEDIT_TBBUTTON_POPPED_UP_TEXT_ARRAY);
    CDesCArrayFlat* items = iCoeEnv->ReadDesCArrayResourceL(aResourceId);
    CleanupStack::PushL( items );

    box->Model()->SetItemTextArray((MDesC16Array*)items);
    //box->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    CleanupStack::Pop(); // items

    box->SetCurrentItemIndex(aSelectedItem);

    // Run selection
    TInt ok = popup->ExecuteLD();
    CleanupStack::Pop(); // popup
    if ( ok ) {
        ret = box->CurrentItemIndex();
    } else {
        ret = -1;
    }
    CleanupStack::PopAndDestroy(); // box
    return ret;
}

TInt CTerminalView::CheckGestureMap (TInt aCmd, TBool aItemToCmd) {
     static struct {
        TInt iCommand;
     } KItemMap [] = {
        { EPuttyCmdNone },
        { EPuttyCmdStartVKB },
        { EPuttyCmdOpenPopUpMenu },
        { EPuttyCmdFullScreen },
        { EPuttyCmdToggleToolbar },
        { EPuttyCmdSendTab },
        { EPuttyCmdSend },
        { EPuttyCmdSendLine },
        { EPuttyCmdSendCtrlAN },
        { EPuttyCmdSendCtrlAP },
        { EPuttyCmdSendAltALeft },
        { EPuttyCmdSendAltARight },
        { EPuttyCmdSendPageUp },
        { EPuttyCmdSendPageDown },
        { EPuttyCmdSendHome },
        { EPuttyCmdSendEnd },
        { EPuttyCmdSendDelete },
        { EPuttyCmdSendInsert },
        { EPuttyCmdSendCR },
        { EPuttyCmdSendCtrlP },
        { EPuttyCmdSendAltP },
        { EPuttyCmdSendEsc }
    };    
//Update also gesture action count!!!
     const TInt iGestureCount = 22; 
     if ( aItemToCmd ) {
         if ( aCmd < iGestureCount && aCmd > -1 ) {
             return KItemMap[aCmd].iCommand;
         } else {
             return -1;
         }
     } 
     
    for(int i = 0; i < iGestureCount; i++) {
        if ( KItemMap[i].iCommand == aCmd ) {
            return i;        
        }
    }
    return -1;
}

// Show touch settings
void CTerminalView::SetTouchSettingsL() {
    //R_PUTTY_PROFILEEDIT_GESTURE_POPPED_UP_TEXT_ARRAY
    TTouchSettings *iTouchSettings = iContainer->GetTouchSettings();
    TInt itemToEdit = PopUpListViewL(R_PUTTY_PROFILEEDIT_TOUCH_SETTINGLIST_TEXT,0);
    TInt itemCommand = -1;
/*      
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdNone; text = "None"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdStartVKB; text = "Open virtual keyboard"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdOpenPopUpMenu; text = "Open popup menu"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdFullScreen; text = "Toggle full screen"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdToggleToolbar; text = "Toggle toolbar"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendTab; text = "Send tab key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSend; text = "Open send grid"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendLine; text = "Open send line dialog"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendCtrlAN; text = "Send ctrl+an"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendCtrlAP; text = "Send ctrl+ap"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendAltALeft; text = "Send alt+left arrow"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendAltARight; text = "Send alt+right arrow"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendPageUp; text = "Send page up key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendPageDown; text = "Send page down key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendHome; text = "Send home key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendEnd; text = "Send end key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendDelete; text = "Send delete key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendInsert; text = "Send insert key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendCR; text = "Send insert key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendCtrlP; text = "Add ctrl modifier to next key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendAltP; text = "Add alt modifier to next key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendAltP; text = "Add alt modifier to next key"; },
        AVKON_ENUMERATED_TEXT { value = EPuttyCmdSendEsc; text = "Send esc key"; }
*/    
    
    if (itemToEdit >= 0) {
        TInt selectedItem = 0;
        switch ( itemToEdit ) {
            case 0:
                selectedItem = iTouchSettings->GetSingleTap();
                break;
            case 1:
                selectedItem = iTouchSettings->GetDoubleTap();
                break;
            case 2:
                selectedItem = iTouchSettings->GetLongTap();
                break;
            case 3:
                selectedItem = iTouchSettings->GetSwipeUp();
                break;
            case 4:
                selectedItem = iTouchSettings->GetSwipeDown();
                break;
            case 5:
                selectedItem = iTouchSettings->GetSwipeLeft();
                break;
            case 6:
                selectedItem = iTouchSettings->GetSwipeRight();
                break;
        }
        selectedItem = CheckGestureMap(selectedItem, EFalse);
        itemCommand = PopUpListViewL(R_PUTTY_PROFILEEDIT_GESTURE_POPPED_UP_TEXT_ARRAY, selectedItem);        
        itemCommand = CheckGestureMap(itemCommand, ETrue);
        if ( itemCommand != -1 ) {
            switch ( itemToEdit ) {
                case 0:                   
                    iTouchSettings->SetSingleTap(itemCommand);
                    break;
                case 1:                   
                    iTouchSettings->SetDoubleTap(itemCommand);
                    break;
                case 2:                   
                    iTouchSettings->SetLongTap(itemCommand);
                    break;
                case 3:                   
                    iTouchSettings->SetSwipeUp(itemCommand);
                    break;
                case 4:                   
                    iTouchSettings->SetSwipeDown(itemCommand);
                    break;
                case 5:                   
                    iTouchSettings->SetSwipeLeft(itemCommand);
                    break;
                case 6:                   
                    iTouchSettings->SetSwipeRight(itemCommand);
                    break;
            }
        }

        iTouchSettings->WriteSettingFileL();      
    } // end if
}

// Show general toolbar settings
void CTerminalView::SetGeneralToolbarSettingsL() {
    //R_PUTTY_PROFILEEDIT_GESTURE_POPPED_UP_TEXT_ARRAY
    TTouchSettings *iTouchSettings = iContainer->GetTouchSettings();
    TInt itemToEdit = PopUpListViewL(R_PUTTY_PROFILEEDIT_GENERAL_TOOLBAR_SETTINGLIST_TEXT,0);
    TInt itemCommand = -1;
    
    if (itemToEdit >= 0) {
        TInt selectedItem = 0;
        switch ( itemToEdit ) {
            case 0:
                selectedItem = iTouchSettings->GetShowToolbar();
                itemCommand = PopUpListViewL(R_PUTTY_PROFILEEDIT_TBSHOWSTARTUP_POPPED_UP_TEXT_ARRAY, selectedItem);
                if ( itemCommand != -1 ) {
                    iTouchSettings->SetShowToolbar(itemCommand);
                }
                break;
            case 1: //Set button count
                {                
                selectedItem = iTouchSettings->GetTbButtonCount();

                CAknNumberQueryDialog* dlg =
                    CAknNumberQueryDialog::NewL( selectedItem );
                dlg->SetPromptL(_L("Enter toolbar button count:"));
                if ( dlg->ExecuteLD( R_PUTTY_TBCOUNT_QUERY ) ) {
                  
                    if ( selectedItem < 1 ) {
                        selectedItem = 1;
                    } else if ( selectedItem > 8 ) {
                        selectedItem = 8;
                    }
                    iTouchSettings->SetTbButtonCount(selectedItem);
                    iTouchSettings->WriteSettingFileL();
                    iContainer->UpdateAfterSettingsChangeL();
                } else {
                  //user canceled
                }
                break;
                }
            case 2: //Set Width
            case 3: //Set Heigth                
                {
                TInt width = iTouchSettings->GetTbButtonWidth();
                TInt heigth = iTouchSettings->GetTbButtonHeigth();
                CAknMultiLineDataQueryDialog* dlg =
                    CAknMultiLineDataQueryDialog::NewL( width,  heigth );
                if ( dlg->ExecuteLD( R_PUTTY_TBWIDHTHEIGTH_QUERY ) ) {
                    //If changed remember to change from putty.rss also
                    if ( width < 25 ) {
                        width = 25;
                    } else if (width > 150 ) {
                        width = 150;
                    }
                    if ( heigth < 25 ) {
                        heigth = 25;
                    } else if (heigth > 150 ) {
                        heigth = 150;
                    }
                    iTouchSettings->SetTbButtonWidth(width);
                    iTouchSettings->SetTbButtonHeigth(heigth);
                    iTouchSettings->WriteSettingFileL();
                    iContainer->UpdateAfterSettingsChangeL();
                } else {
                  //user canceled
                }
                break;
                }
            case 4: //Set button up transparency                
                {
                TInt bg = iTouchSettings->GetButtonUpBGTransparency();
                TInt text = iTouchSettings->GetButtonUpTextTransparency();
                CAknMultiLineDataQueryDialog* dlg =
                    CAknMultiLineDataQueryDialog::NewL( bg,  text );
                if ( dlg->ExecuteLD( R_PUTTY_TBUPTRANSPARENCY_QUERY ) ) {
                    //If changed remember to change from putty.rss also                 
                    if ( bg < 0 ) {
                        bg = 0;
                    } else if (bg > 255 ) {
                        bg = 255;
                    }
                    if ( text < 0 ) {
                        text = 0;
                    } else if (text > 255 ) {
                        text = 255;
                    }
                    iTouchSettings->SetButtonUpBGTransparency(bg);
                    iTouchSettings->SetButtonUpTextTransparency(text);
                    iTouchSettings->WriteSettingFileL();
                    iContainer->UpdateAfterSettingsChangeL();
                } else {
                  //user canceled
                }
                break;
                }
            case 5: //Set button down transparency                
                {
                TInt bg = iTouchSettings->GetButtonDownBGTransparency();
                TInt text = iTouchSettings->GetButtonDownTextTransparency();
                CAknMultiLineDataQueryDialog* dlg =
                    CAknMultiLineDataQueryDialog::NewL( bg,  text );
                if ( dlg->ExecuteLD( R_PUTTY_TBUPTRANSPARENCY_QUERY ) ) {
                    //If changed remember to change from putty.rss also
                    if ( bg < 0 ) {
                        bg = 0;
                    } else if (bg > 255 ) {
                        bg = 255;
                    }
                    if ( text < 0 ) {
                        text = 0;
                    } else if (text > 255 ) {
                        text = 255;
                    }
                    iTouchSettings->SetButtonDownBGTransparency(bg);
                    iTouchSettings->SetButtonDownTextTransparency(text);
                    iTouchSettings->WriteSettingFileL();
                    iContainer->UpdateAfterSettingsChangeL();
                } else {
                  //user canceled
                }
                break;
                }
            case 6: //Set button font size
                {
                TInt fontSize = iTouchSettings->GetButtonFontSize();

                CAknNumberQueryDialog* dlg =
                    CAknNumberQueryDialog::NewL( fontSize );
                dlg->SetPromptL(_L("Enter button font size:"));
                if ( dlg->ExecuteLD( R_PUTTY_TBFONTSIZE_QUERY ) ){
                  
                    if ( fontSize > 2000 ) {
                        fontSize = 2000;
                    }
                    iTouchSettings->SetButtonFontSize(fontSize);
                    iTouchSettings->WriteSettingFileL();
                    iContainer->UpdateAfterSettingsChangeL();
                } else {
                  //user canceled
                }
                break;
                }

        }
        
              
    } // end if
}

// Show toolbar settings
void CTerminalView::SetToolbarButtonL() {
    //TInt buttonToEdit = PopUpListViewL(R_PUTTY_PROFILEEDIT_TBBUTTON_POPPED_UP_TEXT_ARRAY, 0);
    TTouchSettings *iTouchSettings = iContainer->GetTouchSettings();
    TInt buttonToEdit = PopUpListViewL(R_PUTTY_PROFILEEDIT_TBBUTTONS_TEXT_ARRAY, 0);
    if (buttonToEdit >= 0) {
        TInt selectedCommand = 0;
        switch (buttonToEdit) {
            case 0:                   
                selectedCommand = iTouchSettings->GetTbButton1();
            break;
            case 1:                   
                selectedCommand = iTouchSettings->GetTbButton2();
            break;
            case 2:                   
                selectedCommand = iTouchSettings->GetTbButton3();
            break;
            case 3:                   
                selectedCommand = iTouchSettings->GetTbButton4();
            break;
            case 4:                   
                selectedCommand = iTouchSettings->GetTbButton5();
            break;
            case 5:                   
                selectedCommand = iTouchSettings->GetTbButton6();
            break;
            case 6:                   
                selectedCommand = iTouchSettings->GetTbButton7();
            break;
            case 7:                   
                selectedCommand = iTouchSettings->GetTbButton8();
            break;        
        }
        TInt buttonCommand = PopUpListViewL(R_PUTTY_PROFILEEDIT_TBBUTTON_POPPED_UP_TEXT_ARRAY, selectedCommand);
        if ( buttonCommand != -1 ) {
            //Test if button is allready defined in some of the buttons if not update it.
            if ( iTouchSettings->TestIfToolBarActionAllreadySet(buttonCommand) ) {
                //Show note
                if ( CAknQueryDialog::NewL()->ExecuteLD( R_PUTTY_TBBUTTON_EXISTS_QUERY_DIALOG) ) {
                  // do swap else return
                    if ( iTouchSettings->SwapButtons(buttonToEdit, buttonCommand) ) {
                        iTouchSettings->WriteSettingFileL();
                        iContainer->SwapButtons(buttonToEdit, buttonCommand);
                        //iContainer->UpdateAfterSettingsChangeL();
                    }                
                }
                return;
            }
            
            switch ( buttonToEdit ) {
                case 0:                   
                    iTouchSettings->SetTbButton1(buttonCommand);
                break;
                case 1:                   
                    iTouchSettings->SetTbButton2(buttonCommand);
                break;
                case 2:                   
                    iTouchSettings->SetTbButton3(buttonCommand);
                break;
                case 3:                   
                    iTouchSettings->SetTbButton4(buttonCommand);
                break;
                case 4:                   
                    iTouchSettings->SetTbButton5(buttonCommand);
                break;
                case 5:                   
                    iTouchSettings->SetTbButton6(buttonCommand);
                break;
                case 6:                   
                    iTouchSettings->SetTbButton7(buttonCommand);
                break;
                case 7:                   
                    iTouchSettings->SetTbButton8(buttonCommand);
                break;
            }
        }
        iTouchSettings->WriteSettingFileL();
        iContainer->UpdateAfterSettingsChangeL();
    }
}
#endif

// Change font
void CTerminalView::SetFontL() {

    // We'll show a popup list of fonts and let the user select one

    Config *cfg = iPutty->GetConfig();
    
    // Create a listbox and a popup to contain it
    CAknSinglePopupMenuStyleListBox *box =
        new (ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(box);
    CAknPopupList *popup = CAknPopupList::NewL(box,
                                               R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                               AknPopupLayouts::EMenuWindow);    
    CleanupStack::PushL(popup);
    box->ConstructL(popup, 0);
    box->CreateScrollBarFrameL(ETrue);
    box->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,
                                                   CEikScrollBarFrame::EAuto);

    // Add fonts to the listbox
    const CDesCArray &fonts = ((CPuttyAppUi*)AppUi())->Fonts();
    box->Model()->SetItemTextArray((MDesC16Array*)&fonts);
    box->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Find current font from the list if possible and set it as the default
    HBufC *cur = StringToBufLC(cfg->font.name);
    if ( cur->Length() == 0 ) {
        CleanupStack::PopAndDestroy();
        cur = TPtrC(KDefaultFont).AllocLC();
    }
    TInt font;
    if ( fonts.Find(*cur, font) != 0 ) {
        font = 0;
    }
    CleanupStack::PopAndDestroy(); // cur
    box->SetCurrentItemIndex(font);

    // Run selection
    TInt ok = popup->ExecuteLD();
    CleanupStack::Pop(); // popup
    if ( ok ) {
        iFontFile = iFontDirectory;
        iFontFile.Append(fonts[box->CurrentItemIndex()]);
        iFontFile.Append(KFontExtension);
        iContainer->SetFontL(iFontFile);                
        DesToString(fonts[box->CurrentItemIndex()], cfg->font.name,
                    sizeof(cfg->font.name));
    }
    CleanupStack::PopAndDestroy(); // box
}


// Change palette
void CTerminalView::SetPaletteL() {

    // We'll show a popup list of palettes and let the user select one

    Config *cfg = iPutty->GetConfig();
    
    CPalettes *palettes = CPalettes::NewL(R_PUTTY_PALETTE_NAMES,
                                          R_PUTTY_PALETTES);
    CleanupStack::PushL(palettes);

    // Create a listbox and a popup to contain it
    CAknSinglePopupMenuStyleListBox *box =
        new (ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(box);
    CAknPopupList *popup = CAknPopupList::NewL(box,
                                               R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                               AknPopupLayouts::EMenuWindow);    
    CleanupStack::PushL(popup);
    box->ConstructL(popup, 0);
    box->CreateScrollBarFrameL(ETrue);
    box->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,
                                                   CEikScrollBarFrame::EAuto);

    // Add palettes to the listbox
    const CDesCArray &names = palettes->PaletteNames();
    box->Model()->SetItemTextArray((MDesC16Array*)&names);
    box->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    box->SetCurrentItemIndex(palettes->IdentifyPalette(
                                 (const unsigned char*) cfg->colours));

    // Run selection
    TInt ok = popup->ExecuteLD();
    CleanupStack::Pop(); // popup
    if ( ok ) {
        // Set the palette to the engine and use it for container default
        // colors to make the background match
        palettes->GetPalette(box->CurrentItemIndex(),
                             (unsigned char*) cfg->colours);
        iContainer->SetDefaultColors(TRgb(cfg->colours[0][0],
                                          cfg->colours[0][1],
                                          cfg->colours[0][2]),
                                     TRgb(cfg->colours[2][0],
                                          cfg->colours[2][1],
                                          cfg->colours[2][2]));
        iPutty->ResetPalette();
        iPutty->RePaintWindow();
        iContainer->DrawDeferred();
    }
    CleanupStack::PopAndDestroy(2); // palettes, box
}


// Handle Enter (center of joystick) key press
TBool CTerminalView::HandleEnterL() {

#ifdef PUTTY_S60TOUCH
    if ( iSelection && iContainer->Terminal().DoWeHaveSelection() ) {
#else
    if ( iSelection && iMark ) {
#endif    
    
        // Have a selection -- copy it
        HandleCommandL(EPuttyCmdCopy);
        return ETrue;
    } else if ( iSelection ) {
        // In selection mode but no selection yet -- drop a mark
        HandleCommandL(EPuttyCmdMark);
        return ETrue;
    }
    return EFalse;
}


// MPuttyClient::FatalError
void CTerminalView::FatalError(const TDesC &aMessage) {
    CAknNoteDialog* dlg = new(ELeave) CAknNoteDialog();
    dlg->SetTextL(aMessage);
    dlg->ExecuteDlgLD(R_PUTTY_INFO_MESSAGE_DLG);
    User::Exit(KFatalErrorExit);
}


// MPuttyClient::DrawText()
void CTerminalView::DrawText(TInt aX, TInt aY, const TDesC &aText, TBool aBold,
                             TBool aUnderline, TRgb aForeground,
                             TRgb aBackground) {
    iContainer->Terminal().DrawText(aX, aY, aText, aBold, aUnderline,
                                    aForeground, aBackground);
#ifdef PUTTY_S60TOUCH
    iContainer->DrawDeferred(); // lets redraw things so toolbar can be shown.
#endif
}


// MPuttyClient::SetCursor()
void CTerminalView::SetCursor(TInt aX, TInt aY) {
    iContainer->Terminal().SetCursor(aX, aY);
}


// MPuttyClient::ConnectionError()
void CTerminalView::ConnectionError(const TDesC &aMessage) {

    TRAPD(error, ConnectionErrorL(aMessage));
    if ( error != KErrNone ) {
        User::Panic(KFatalErrorPanic, error);
    }
}

void CTerminalView::ConnectionErrorL(const TDesC &aMessage) {

    // Store the connection error and display it when the connection is safely
    // closed. Displaying it now with a waiting note would result in a nested
    // active scheduler causing problems to the engine, while non-waiting notes
    // would disappear immediately when the view changes.
    iConnectionError = aMessage.AllocL();
}


// MPuttyClient::ConnectionClosed()
void CTerminalView::ConnectionClosed() {
    // If we have a connection error waiting, show it as a waiting dialog box,
    // giving the user time to read it. Otherwise just display a
    // "Connection closed" note.
    if ( iConnectionError ) {
        CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog();
        dlg->SetTextL(*iConnectionError);
        dlg->ExecuteDlgLD(R_PUTTY_INFO_MESSAGE_DIALOG);
        delete iConnectionError;
        iConnectionError = NULL;
    } else {
        CAknInformationNote* dlg = new (ELeave) CAknInformationNote(ETrue);
        dlg->SetTone(CAknInformationNote::ENoTone);
        dlg->SetTimeout(CAknInformationNote::EShortTimeout);
        dlg->ExecuteLD(*StringLoader::LoadLC(R_PUTTY_STR_CONNECTION_CLOSED));
        CleanupStack::PopAndDestroy(); // message
    }
    iState = EStateNone;
    DoDisconnectL();
}


// MPuttyClient::UnknownHostKey()
MPuttyClient::THostKeyResponse CTerminalView::UnknownHostKey(
    const TDesC &aFingerprint) {
    
    MPuttyClient::THostKeyResponse resp = EAbadonConnection;
    TRAPD(error, resp = HostKeyDialogL(aFingerprint,
                                       R_PUTTY_STR_UNKNOWN_HOST_KEY_DLG_FMT));
    if ( error != KErrNone ) {
        User::Panic(KFatalErrorPanic, error);
    }
    return resp;
}


// MPuttyClient::DifferentHostKey()
MPuttyClient::THostKeyResponse CTerminalView::DifferentHostKey(
    const TDesC &aFingerprint) {

    MPuttyClient::THostKeyResponse resp = EAbadonConnection;
    TRAPD(error, resp = HostKeyDialogL(
              aFingerprint, R_PUTTY_STR_DIFFERENT_HOST_KEY_DLG_FMT));
    if ( error != KErrNone ) {
        User::Panic(KFatalErrorPanic, error);
    }
    return resp;
}


MPuttyClient::THostKeyResponse CTerminalView::HostKeyDialogL(
    const TDesC &aFingerprint, TInt aDialogFormatRes) {

    CEikonEnv *env = CEikonEnv::Static();

    HBufC *fmt = env->AllocReadResourceLC(aDialogFormatRes);
    HBufC *contents = HBufC::NewLC(fmt->Length() + aFingerprint.Length());
    contents->Des().Format(*fmt, &aFingerprint);

    MPuttyClient::THostKeyResponse resp = EAbadonConnection;

    CAknQueryDialog *dlg = CAknQueryDialog::NewL();
    dlg->SetPromptL(*contents);
    if ( dlg->ExecuteLD(R_PUTTY_HOSTKEY_QUERY_DIALOG) ) {
        resp = EAcceptAndStore;
    } else {
        resp = EAbadonConnection;
    }
    
    CleanupStack::PopAndDestroy(2); // contents, fmt

    return resp;
}


// MPuttyClient::AcceptCipher()
TBool CTerminalView::AcceptCipher(const TDesC &aCipherName,
                                  const TDesC &aCipherType) {
    TBool resp = EFalse;
    TRAPD(error, resp = AcceptCipherL(aCipherName, aCipherType));
    if ( error != KErrNone ) {
        User::Panic(KFatalErrorPanic, error);
    }
    return resp;
}


TBool CTerminalView::AcceptCipherL(const TDesC &aCipherName,
                                  const TDesC &aCipherType) {
    
    CEikonEnv *env = CEikonEnv::Static();

    HBufC *fmt = env->AllocReadResourceLC(R_PUTTY_STR_ACCEPT_CIPHER_DLG_FMT);

    HBufC *contents = HBufC::NewLC(fmt->Length() +
                                   aCipherName.Length() +
                                   aCipherType.Length());
    contents->Des().Format(*fmt, &aCipherType, &aCipherName);

    CAknQueryDialog *dlg = CAknQueryDialog::NewL();
    dlg->SetPromptL(*contents);
    TBool res = dlg->ExecuteLD(R_PUTTY_CIPHER_QUERY_DIALOG);

    CleanupStack::PopAndDestroy(2); // contents, fmt
    return res;
}


// MPuttyClient::AuthenticationPrompt()
TBool CTerminalView::AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                                          TBool aSecret) {
    TBool ret = EFalse;
    TRAPD(error, ret = AuthenticationPromptL(aPrompt, aTarget, aSecret));
    if ( error != KErrNone ) {
        User::Panic(KFatalErrorPanic, error);
    }
    return ret;
}
    
TBool CTerminalView::AuthenticationPromptL(const TDesC &aPrompt, TDes &aTarget,
                                           TBool aSecret) {

    CAknTextQueryDialog *dlg = CAknTextQueryDialog::NewL(aTarget);
    
    CleanupStack::PushL(dlg);
    dlg->SetPromptL(aPrompt);
    CleanupStack::Pop();
    
    if ( aSecret ) {
        return dlg->ExecuteLD(R_PUTTY_AUTH_SECRET_DIALOG);
    }
    return dlg->ExecuteLD(R_PUTTY_AUTH_NOT_SECRET_DIALOG);
}

// MTerminalObserver::TerminalSizeChanged()
void CTerminalView::TerminalSizeChanged(TInt aWidth, TInt aHeight) {
    assert((aWidth > 1) && (aHeight > 1));
    if ( iPutty ) {
        iPutty->SetTerminalSize(aWidth, aHeight);
    }
}


// MTerminalObserver::KeyPressed();
void CTerminalView::KeyPressed(TKeyCode aCode, TUint aModifiers) {
    if ( iPutty ) {
#ifdef PUTTY_S60TOUCH
        if ( iReleaseAltAfterKey ) {
            iReleaseAltAfterKey = EFalse;
            iContainer->ReleaseAlt();
        }
            
        if ( iReleaseCtrlAfterKey ) {
            iReleaseCtrlAfterKey = EFalse;
            iContainer->ReleaseCtrl();
        }                      

        iPutty->SendKeypress(aCode, aModifiers);
#else
        iPutty->SendKeypress(aCode, aModifiers);
#endif
    }
}


// MSendGridObserver::MsgoCommand()
void CTerminalView::MsgoCommandL(TInt aCommand) {
    HandleCommandL(aCommand);
    if ( iSendGrid ) {
        AppUi()->RemoveFromStack(iSendGrid);
        delete iSendGrid;
        iSendGrid = NULL;
    }
}


// MSendGridObserver::MsgoTerminated()
void CTerminalView::MsgoTerminated() {
    if ( iSendGrid ) {
        AppUi()->RemoveFromStack(iSendGrid);
        delete iSendGrid;
        iSendGrid = NULL;
    }
}

#ifdef PUTTY_S60TOUCH
void CTerminalView::SendKeypress(TKeyCode aCode, TUint aModifiers) {
  iPutty->SendKeypress(aCode,aModifiers);
}

void CTerminalView::SetReleaseAltAfterKeyPress(TBool aValue) {
    iReleaseAltAfterKey = aValue;
}

void CTerminalView::SetReleaseCtrlAfterKeyPress(TBool aValue) {
    iReleaseCtrlAfterKey = aValue;
}

TInt CTerminalView::MouseMode() {
    TTouchSettings *iTouchSettings = iContainer->GetTouchSettings();
    return iTouchSettings->GetAllowMouseGrab() && iPutty->MouseMode();
}

void CTerminalView::MouseClick(TInt modifiers, TInt row, TInt col) {
    iPutty->MouseClick(modifiers, row, col);
}

#endif

