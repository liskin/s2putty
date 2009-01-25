/*    netconnect.cpp
 *
 * A concrete network connection setup class using RConnection
 *
 * Copyright 2006 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <es_sock.h>
#include "netconnect.h"

_LIT(KPanic, "netconnect");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KPanic, __LINE__))


// Factory
CNetConnect *CNetConnect::NewL(MNetConnectObserver &aObserver) {
    CNetConnect *self = new (ELeave) CNetConnect(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
CNetConnect::CNetConnect(MNetConnectObserver &aObserver)
    : CActive(EPriorityStandard),
      iObserver(aObserver) {
    CActiveScheduler::Add(this);
}


// Second-phase constructor
void CNetConnect::ConstructL() {
}


// Destructor
CNetConnect::~CNetConnect() {
    if ( iRConnectionOpen ) {
        iConnection.Close();
    }
    if ( iSocketServOpen ) {
        iSocketServ.Close();
    }
}


// Connect
void CNetConnect::Connect() {

    assert(iState == EStateNone);

    if ( !iSocketServOpen ) {
        TInt err = iSocketServ.Connect();
        if ( err != KErrNone ) {
            iObserver.NetConnectComplete(err, iSocketServ, iConnection);
            return;
        }
        iSocketServOpen = ETrue;
    }
    
    // Open RConnection
    TInt err = iConnection.Open(iSocketServ);
    if ( err != KErrNone ) {
        iObserver.NetConnectComplete(err, iSocketServ, iConnection);
        return;
    }
    iRConnectionOpen = ETrue;

    // Connect to the network using default settings
    iConnection.Start(iStatus);
    iState = EStateConnecting;
    SetActive();
}


// Cancel connection
void CNetConnect::CancelConnect() {
    if ( iState != EStateConnecting ) {
        return;
    }
    Cancel();
}


// Active object RunL
void CNetConnect::RunL() {
    if ( iStatus == KErrNone ) {
        iState = EStateConnected;
    }
    iObserver.NetConnectComplete(iStatus.Int(), iSocketServ, iConnection);
}


// Active object cancel
void CNetConnect::DoCancel() {
    assert(iState == EStateConnecting);
    iConnection.Close();
    iRConnectionOpen = EFalse;
    iState = EStateNone;
}
