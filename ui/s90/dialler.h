/*    dialler.h
 *
 * A dial-up connection setup class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __DIALLER_H__
#define __DIALLER_H__

#include <e32base.h>


/**
 * An observer class for CDialler. The observer gets notified when the
 * connection has been set up, or there has been an error.
 */
class MDialObserver {
public:
    /** 
     * Notifies the observer that a connection setup has completed.
     * 
     * @param anError The error code, KErrNone if the connection was set up
     *                successfully.
     */
    virtual void DialCompleted(TInt anError) = 0;
};


/**
 * A dial-up connection setup class. Connects to the network asynchronously
 * and notifies an observer when the connection has been set up.
 */
class CDialler : public CBase {
    
public:
    /** 
     * Constructs a new CDialler object.
     * 
     * @param aObserver The observer to use
     * 
     * @return A new CDialler object
     */
    static CDialler *NewL(MDialObserver *aObserver);

    /** 
     * Destructor.
     */
    ~CDialler();
    
    /** 
     * Starts setting up a dialup connection. When the connection has
     * completed, or there has been an error, the observer is notified.
     */
    void DialL();

    /** 
     * Cancels the connection setup.
     */
    void CancelDial();

private:
    CDialler(MDialObserver *aObserver);
    void ConstructL();

    MDialObserver *iObserver;
};


#endif
