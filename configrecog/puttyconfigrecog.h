/*    puttyconfigrecog.h
 *
 * PuTTY configuration file recognizer
 *
 * Copyright 2002,2003 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYCONFIGRECOG_H__
#define __PUTTYCONFIGRECOG_H__


#include <apmrec.h>

class CConfigRecognizer : public CApaDataRecognizerType {
public:
    CConfigRecognizer();
    TUint PreferredBufSize();
    TDataType SupportedDataTypeL(TInt aIndex) const;
    virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
};



#endif
