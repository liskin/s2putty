/*    oneshottimer.h
 *
 * A one-shot timer class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __ONESHOTTIMER_H__
#define __ONESHOTTIMER_H__

#include <e32std.h>
#include <e32base.h>


/**
 * A one-shot timer class, providing a single callback after a period
 * of time. The time countdown can be restarted at any point.
 */
class COneShotTimer : public CActive {

public:
    /** 
     * Factory method.
     * 
     * @param aCallBack Callback function for timer callbacks
     * @param aPriority Active object priority
     * 
     * @return New COneShotTimer instance
     */
    static COneShotTimer *NewL(TCallBack aCallBack,
                               TInt aPriority = EPriorityStandard);
   
    /** 
     * Destructor.
     */
    ~COneShotTimer();
    
    /** 
     * Starts the timer. The callback will be called after the given time
     * period. If a timer request is already in progress, the old request is
     * cancelled.
     * 
     * @param aTime Time interval in microseconds. Note that the final timing
     *              accuracy depends on the timer accuracy on the platform.
     */
    void After(TTimeIntervalMicroSeconds32 aTime);
    
    /** 
     * Cancels an outstanding timing request.
     */
    void CancelTimer();

    
private:
    COneShotTimer(TCallBack aCallBack, TInt aPriority);
    void ConstructL();
    void RunL();
    void DoCancel();
    
    RTimer iTimer;
    TBool iTimerOpen;
    TCallBack iCallBack;
    TBool iTimerRequestActive;
};


#endif
