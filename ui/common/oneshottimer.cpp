/*    oneshottimer.cpp
 *
 * A one-shot timer class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "oneshottimer.h"

_LIT(KPanic, "COneShotTimer");
_LIT(KOneShotTimer, "oneshottimer.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KOneShotTimer, __LINE__))


// Factory method
COneShotTimer *COneShotTimer::NewL(TCallBack aCallBack, TInt aPriority) {
    COneShotTimer *self = new (ELeave) COneShotTimer(aCallBack, aPriority);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Constructor
COneShotTimer::COneShotTimer(TCallBack aCallBack, TInt aPriority)
    : CActive(aPriority),
      iCallBack(aCallBack) {
    CActiveScheduler::Add(this);
}


// Second-phase constructor
void COneShotTimer::ConstructL() {
    User::LeaveIfError(iTimer.CreateLocal());
    iTimerOpen = ETrue;
}


// Destructor
COneShotTimer::~COneShotTimer() {
    Cancel();
    if ( iTimerOpen ) {
        iTimer.Close();
    }
}


// Start the timer
void COneShotTimer::After(TTimeIntervalMicroSeconds32 aTime) {
    if ( iTimerRequestActive ) {
        Cancel();
    }
    iTimer.After(iStatus, aTime);
    SetActive();
    iTimerRequestActive = ETrue;
}


// Cancel a timer request
void COneShotTimer::CancelTimer() {
    if ( !iTimerRequestActive ) {
        return;
    }
    Cancel();
}


// CActive::DoCancel
void COneShotTimer::DoCancel() {
    assert(iTimerRequestActive);
    iTimer.Cancel();
    iTimerRequestActive = EFalse;
}


// CActive::RunL
void COneShotTimer::RunL() {
    
    TInt error = iStatus.Int();
    
    if ( error != KErrNone ) {
        User::Panic(KPanic, error);
    }
    assert(iTimerRequestActive);
    iTimerRequestActive = EFalse;
    iCallBack.CallBack();
}
