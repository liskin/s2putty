/*    dialler.cpp
 *
 * A dial-up connection setup class
 *
 * Copyright 2002-2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include "dialler.h"

_LIT(KAssertPanic, "dialler.cpp");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


// FIXME: Need an implementation for S80 2.0 using RConnection

CDialler *CDialler::NewL(MDialObserver *aObserver) {
    
    CDialler *self = new (ELeave) CDialler(aObserver);;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


CDialler::CDialler(MDialObserver *aObserver) {
    
    iObserver = aObserver;
}


CDialler::~CDialler() {
}


void CDialler::ConstructL() {
}


void CDialler::DialL() {
    
    iObserver->DialCompleted(KErrNone);
}


void CDialler::CancelDial() {
}
