/*    puttyconfigrecog.cpp
 *
 * PuTTY configuration file recognizer
 *
 * Copyright 2002,2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <apmrec.h>
#include <apmstd.h>
#include "puttyconfigrecog.h"

const TInt KUidRecognizerValue = 0x101f9077;
const TUid KUidRecognizer = { 0x101f9077 };
_LIT8(KSettingsId, "[PuTTYConfig]");
const TInt KIdLength = 13;
_LIT8(KMIMEType, "application/prs.s2.putty-config");


CConfigRecognizer::CConfigRecognizer()
    : CApaDataRecognizerType(KUidRecognizer,
                             CApaDataRecognizerType::ECertain) {
    iCountDataTypes = 1;
}


TUint CConfigRecognizer::PreferredBufSize() {
    return KIdLength;
}


TDataType CConfigRecognizer::SupportedDataTypeL(TInt aIndex) const {
    __ASSERT_DEBUG(aIndex == 0, User::Invariant());
    return TDataType(KMIMEType);
}


void CConfigRecognizer::DoRecognizeL(const TDesC & /*aName*/,
                                     const TDesC8 &aBuffer) {
    if ( aBuffer.Length() < KIdLength ) {
        return;
    }
    if ( aBuffer.Left(KIdLength).Compare(KSettingsId) ) {
        return;
    }
    iConfidence = ECertain;
    iDataType = TDataType(KMIMEType);
}


EXPORT_C CApaDataRecognizerType *CreateRecognizer() {
    return new CConfigRecognizer();
}


GLDEF_C TInt E32Dll(TDllReason /*aReason*/) {
    return KErrNone;
}
