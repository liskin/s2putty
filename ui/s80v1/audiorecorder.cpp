/*    audiorecorder.cpp
 *
 * A simple audio recorder class used for capturing audio for a random number
 * generator seed.
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include "audiorecorder.h"

_LIT(KAssertPanic, "audiorecorder");
#define assert(x) __ASSERT_ALWAYS(x, User::Panic(KAssertPanic, __LINE__))


CAudioRecorder *CAudioRecorder::NewL(MRecorderObserver *aObserver) {
    
    CAudioRecorder *self = new (ELeave) CAudioRecorder(aObserver);;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}



CAudioRecorder::CAudioRecorder(MRecorderObserver *aObserver) {
    iObserver = aObserver;
}



CAudioRecorder::~CAudioRecorder() {
    if ( iState != EStateNone ) {
        CancelRecord();
    }
    delete iRecorder;
    delete iClipLocation;
    delete iAudioType;
}



void CAudioRecorder::ConstructL() {
}



void CAudioRecorder::RecordL(TDes8 &aTarget) {

    assert(iState == EStateNone);

    // Create a new recorder utility
    delete iRecorder;
    iRecorder = NULL;
    iRecorder = CMdaAudioRecorderUtility::NewL(*this);

    // Create a new clip location object that points to the target descriptore
    delete iClipLocation;
    iClipLocation = NULL;
    iClipLocation = new TMdaDesClipLocation(aTarget);

    // Create a new audio type object that describes the format we want
    delete iAudioType;
    iAudioType = NULL;
    iAudioType = new (ELeave) CMdaAudioType;
    iAudioType->iFormat = new (ELeave) TMdaRawAudioClipFormat;
    iAudioType->iCodec = new (ELeave) TMdaSL16RawAudioCodec;
    iAudioType->iSettings = new (ELeave) TMdaAudioDataSettings;
    iAudioType->iSettings->iSampleRate = 8000;
    iAudioType->iSettings->iChannels = 1;

    // Open the recorder object with the settings.
    iRecorder->OpenL(iClipLocation, iAudioType->iFormat, iAudioType->iCodec,
                     iAudioType->iSettings);
    iState = EStateOpening;
}


void CAudioRecorder::CancelRecord() {

    if ( iState == EStateRecording ) {
        iRecorder->Stop();
        iRecorder->Close();
        iState = EStateOpen;
    }

    if ( (iState == EStateOpen) || (iState == EStateOpening) ) {
        delete iRecorder;
        iRecorder = NULL;
        delete iClipLocation;
        iClipLocation = NULL;
        delete iAudioType;
        iAudioType = NULL;
        iState = EStateNone;
    }
}


// MMdaObjectStateChangeObserver::MoscoStateChangeEvent methods
void CAudioRecorder::MoscoStateChangeEvent(CBase* /*aObject*/,
                                           TInt /*aPreviousState*/,
                                           TInt aCurrentState,
                                           TInt aErrorCode) {

    assert(iState != EStateNone);

    // If the recorder utility has been opened successfully, we'll start
    // recording
    if ( iState == EStateOpening ) {
        if ( aErrorCode != KErrNone ) {
            iObserver->RecordCompleted(aErrorCode);
            return;
        }        
        if ( aCurrentState != EMdaControllerStatePrepared ) {
            iObserver->RecordCompleted(KErrGeneral);
        }
        
        iState = EStateOpen;
        
 	iRecorder->SetGain(iRecorder->MaxGain());
        TRAPD(error, iRecorder->RecordL());
        if ( error != KErrNone ) {
            iObserver->RecordCompleted(error);
            return;
        }

        iState = EStateRecording;
        return;
    }

    // If recording has finished, report that and stop
    if ( iState == EStateRecording ) {
        if ( (aErrorCode == KErrNone) &&
             (aCurrentState == EMdaControllerStatePlaying) ) {
            return;
        }

        if ( ((aErrorCode == KErrOverflow) || (aErrorCode == KErrNone)) &&
             (aCurrentState == EMdaControllerStatePrepared) ) {
            CancelRecord();
            iState = EStateNone;
            iObserver->RecordCompleted(KErrNone);
            return;
        }
    }
    
    // Report errors back to the observer
    if ( aErrorCode != KErrNone ) {
        iObserver->RecordCompleted(aErrorCode);
        return;
    }

}
