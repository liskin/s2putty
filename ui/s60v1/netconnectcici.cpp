/*    netconnectcici.cpp
 *
 * A concrete network connection setup class using CIntConnectionInitiator
 *
 * Copyright 2006 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <intconninit.h>
#include "netconnectcici.h"

_LIT(KPanic, "nccici");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KPanic, __LINE__))

const TInt KMaxAccessDeniedRetryCount = 3;


// Factory
CNetConnect *CNetConnect::NewL(MNetConnectObserver &aObserver) {
    CNetConnectCICI *self =
        new (ELeave) CNetConnectCICI(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
CNetConnectCICI::CNetConnectCICI(MNetConnectObserver &aObserver) :
    iObserver(aObserver) {
    CActiveScheduler::Add(this);
}


// Second-phase constructor
void CNetConnectCICI::ConstructL() {
    
    // On a 6600, CIntConnectionInitiator::NewL() will fail with
    // KErrNotFound once after an access point has been deleted and a
    // new one created, at least if the access point was the only one
    // in use. After a single network connection has been established
    // automatically, future connection attempts will work fine. To
    // work around this, we'll simply return immediately from
    // connection attempts if this breakage happends.
    //
    // On Series 60 2.0, RConnection is the preferred way to set up
    // connections, but it's possible that somebody will try to run the
    // S60v1 build on v2 devices since they should be compatible, so we'll
    // keep the workaround.
    TRAPD(err, iIntConnInit = CIntConnectionInitiator::NewL());
    if ( err == KErrNotFound ) {
        iIntConnInitBroken = ETrue;
    } else {
        User::LeaveIfError(err);
        User::LeaveIfError(iNif.Open());
        iNifOpen = ETrue;
    }
}


// Destructor
CNetConnectCICI::~CNetConnectCICI() {
    Cancel();
    if ( iNifTimersDisabled ) {
        iNif.DisableTimers(EFalse);
    }
    if ( iNifOpen ) {
        iNif.Close();
    }    
    delete iIntConnInit;
}


// Connect
void CNetConnectCICI::Connect(RSocketServ & /*aSocketServ*/) {

    assert(iState == EStateNone);
    
    // Work around a broken CIntConnectionInitiator
    if ( iIntConnInitBroken ) {
        iObserver.NetConnectComplete(KErrNone);
        iState = EStateOldConnection;
        return;
    }

    // Disable connection inactivity timers    
    // Note that we don't check for the return code. That's because the call
    // fails with KErrNotReady on a 6600, but it still seems to work. We can't
    // defer the call to after the connection has been set up, since in that
    // case the connection will get closed automatically before the SSH socket
    // gets opened. Sigh.
    iNif.DisableTimers(ETrue);
    iNifTimersDisabled = ETrue;
            
    iAccessDeniedRetryCount = 0;
    DoConnect();
    iState = EStateConnecting;
}


// Really connect, used for retries too
void CNetConnectCICI::DoConnect() {
    
    // Build connection preferences
    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
    pref.iRanking = 1;
    pref.iDirection = ECommDbConnectionDirectionOutgoing;
    pref.iDialogPref = ECommDbDialogPrefPrompt;
    pref.iBearer.iBearerSet = ECommDbBearerUnknown;
    pref.iBearer.iIapId = 0; // undefined IAP

    // Start connection
    // Note that at CIntConnectionInitiator in least Series 60 v1.2 won't set
    // the status object to KRequestPending by itself, so we'll need to do that
    // before calling ConnectL(), otherwise we'll eat events from other active
    // objects. v2.0 does work without this, but it doesn't hurt...
    iStatus = KRequestPending;

    // Grr, why can this leave when it's an asynchronous method?
    TRAPD(err, iIntConnInit->ConnectL(pref, iStatus));
    if ( err != KErrNone ) {
        iStatus = err;
        iObserver.NetConnectComplete(err);
        return;
    }
    SetActive();
}


// Cancel connection
void CNetConnectCICI::CancelConnect() {
    if ( iState != EStateConnecting ) {
        return;
    }
    Cancel();
    if ( iNifTimersDisabled ) {
        iNif.DisableTimers(EFalse);
        iNifTimersDisabled = EFalse;
    }
}


// Active object RunL
void CNetConnectCICI::RunL() {

    TInt err = iStatus.Int();

    switch ( err ) {
        case KConnectionCreated:
        case KConnectionExists:
        case KConnectionPref1Created:
        case KConnectionPref1Exists:
        case KConnectionPref2Created:
        case KConnectionPref2Exists:
            iState = EStateConnected;
            err = KErrNone;
            break;

        case KErrAccessDenied: {
            // According to the Forum Nokia IAPConnect example, changing the
            // IAP may fail with KErrAccessDenied on the first try. To work
            // around this, we'll retry a couple of times.
            iState = EStateNone;
            iAccessDeniedRetryCount++;
            if ( iAccessDeniedRetryCount < KMaxAccessDeniedRetryCount ) {
                DoConnect();
                return;
            }
            break;
        }

        default: {
            // According to the IAPConnect example,
            // CIntConnectionInitiator needs to be destroyed and recreated if
            // errors occur and the user might want to retry
            iState = EStateNone;
            iIntConnInit->Cancel();
            delete iIntConnInit;
            TRAPD(err2, iIntConnInit = CIntConnectionInitiator::NewL());
            if ( err2 != KErrNone ) {
                User::Panic(KPanic, err2);
            }
            break;
        }
    }

    iObserver.NetConnectComplete(err);
}


// Active object cancel
void CNetConnectCICI::DoCancel() {
    assert(iState == EStateConnecting);
    // Not sure if this is really correct, but this is what the IAPConnect
    // example does...
    iIntConnInit->Cancel();
    iState = EStateNone;
}
