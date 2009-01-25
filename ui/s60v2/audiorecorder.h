/*    audiorecorder.h
 *
 * A simple audio recorder class used for capturing audio for a random number
 * generator seed.
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __AUDIORECORDER_H__
#define __AUDIORECORDER_H__

#include <mdaaudiosampleeditor.h>


/**
 * An observer class for CAudioRecorder. The observer gets notified when the
 * recording has completed.
 */
class MRecorderObserver {
public:
    /** 
     * Notifies the observer that recording has completed.
     * 
     * @param anError The error code, KErrNone if recording was successfull.
     */
    virtual void RecordCompleted(TInt anError) = 0;
};


/**
 * A simple audio recorder class, used for capturing audio for random number
 * generator initialization. Records audio from the default input with maximum
 * gain, in 8kHz 16bit mono format.
 */
class CAudioRecorder : public CBase, public MMdaObjectStateChangeObserver {
public:
    /** 
     * Constructs a new CAudioRecorder object.
     * 
     * @param aObserver The observer to use
     * 
     * @return A new CAudioRecorder object
     */
    static CAudioRecorder *NewL(MRecorderObserver *aObserver);

    /** 
     * Destructor.
     */
    ~CAudioRecorder();

    /** 
     * Starts recording audio. Recording will proceed until has finished, an
     * error has occurred, or recording has been cancelled using
     * CancelRecord().
     * 
     * @param aTarget Target descriptor for the audio. The descriptor must
     *        remain valid as long as recording is in progress.
     */
    void RecordL(TDes8 &aTarget);

    /** 
     * Cancels recording.
     */
    void CancelRecord();

    // MMdaObjectStateChangeObserver methods
    void MoscoStateChangeEvent(CBase *aObject, TInt aPreviousState,
                               TInt aCurrentState, TInt aErrorCode);

private:
    CAudioRecorder(MRecorderObserver *aObserver);
    void ConstructL();

    MRecorderObserver *iObserver;

    enum {
        EStateNone = 0,
        EStateOpening,
        EStateOpen,
        EStateRecording
    } iState;

    CMdaAudioRecorderUtility *iRecorder;
    TMdaClipFormat *iClipFormat;
    TMdaRawAudioCodec *iCodec;
    TMdaAudioDataSettings *iSettings;    
    TMdaDesClipLocation *iClipLocation;    
};


#endif
