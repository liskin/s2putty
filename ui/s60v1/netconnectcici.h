/*    netconnectcici.h
 *
 * A concrete network connection setup class using CIntConnectionInitiator
 *
 * Copyright 2006 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __NETCONNECTCICI_H__
#define __NETCONNECTCICI_H__

#include <nifman.h>
#include "netconnect.h"

class CIntConnectionInitiator;


/**
 * A concrete network connection setup class using CIntConnectionInitiator.
 * Used on S60v1 only.
 */
class CNetConnectCICI : public CNetConnect {

public:
    ~CNetConnectCICI();
    
    // CNetConnect methods
    virtual void Connect(RSocketServ &aSocketServ);
    virtual void CancelConnect();

    CNetConnectCICI(MNetConnectObserver &aObserver);
    void ConstructL();

private:
    void DoConnect();
    
    // CActive methods
    void RunL();
    void DoCancel();

    // Data
    enum {
        EStateNone = 0,
        EStateConnecting,
        EStateConnected,
        EStateOldConnection,
        EStateError
    } iState;
    MNetConnectObserver &iObserver;
    RNif iNif;
    TBool iNifOpen;
    TBool iNifTimersDisabled;
    CIntConnectionInitiator *iIntConnInit;
    TBool iIntConnInitBroken;
    TInt iAccessDeniedRetryCount;
};


#endif
