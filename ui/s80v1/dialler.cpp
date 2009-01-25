/*    dialler.cpp
 *
 * A dial-up connection setup class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include "dialler.h"

_LIT(KAssertPanic, "dialler.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


CDialler *CDialler::NewL(MDialObserver *aObserver) {
    
    CDialler *self = new (ELeave) CDialler(aObserver);;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


CDialler::CDialler(MDialObserver *aObserver)
    : CActive(EPriorityNormal) {
    
    iObserver = aObserver;
}


CDialler::~CDialler() {

    if ( iDialing ) {
        CancelDial();
    }

    Cancel();
    
    if ( iNetDialOpen ) {
        iNetDial.Close();
    }
}


void CDialler::ConstructL() {
    User::LeaveIfError(iNetDial.Open());
    iNetDialOpen = ETrue;
    CActiveScheduler::Add(this);
}


void CDialler::DialL() {
    
    // Check if a network connection is already open. If yes, notify observer
    // and return
    TBool netActive;
    TInt err = iNetDial.NetworkActive(netActive);
    if ( (err != KErrNone) && (err != KErrNotReady) ) {
        User::Leave(err);
    }
    if ( (err == KErrNone) && netActive ) {
        iObserver->DialCompleted(KErrNone);
        return;
    }

    // Start dialing
    iNetDial.StartDialOut(iStatus);
    iDialing = ETrue;
    SetActive();
}


void CDialler::CancelDial() {

    if ( !iDialing ) {
        return;
    }
    
    iNetDial.CancelDialOutErrorNotification(iStatus);
}


void CDialler::DoCancel() {
    assert(iDialing);
    iNetDial.CancelDialOutErrorNotification(iStatus);
}


void CDialler::RunL() {
    iObserver->DialCompleted(iStatus.Int());
}
