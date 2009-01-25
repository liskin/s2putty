/*    logfile.cpp
 *
 * File logging macros. Slow but should work in any situation.
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifdef LOGFILE_ENABLED

#include <f32file.h>
#include "logfile.h"

#ifdef __WINS__
_LIT(KLogFileName, "c:\\puttylog.txt");
#else
_LIT(KLogFileName, "e:\\puttylog.txt");
#endif
_LIT(KPanic, "LogFile");
_LIT(KTimestampFormat, "%Y-%M-%D %H:%T:%S.%C - ");
_LIT(KCRLF, "\r\n");
const TInt KFormatBufferSize = 2048;


static void PanicIfError(TInt aError) {
    if ( aError != KErrNone ) {
        User::Panic(KPanic, aError);
    }
}


void LogFilePrint(TRefByValue<const TDesC> aFormat, ...) {
    
    // Connect to file server. This wastes time and resources, but ensures
    // that the debug macros don't depend on anything else being initialized.
    RFs fs;
    PanicIfError(fs.Connect());

    // Open file. Append to the end of the file if it exists, or create new
    // if it doesn't.
    RFile file;
    TInt err = file.Open(fs, KLogFileName, EFileWrite | EFileShareExclusive);
    if ( err == KErrNone ) {
        TInt pos = 0;
        PanicIfError(file.Seek(ESeekEnd, pos));
    } else if ( err == KErrNotFound ) {
        PanicIfError(file.Create(fs, KLogFileName,
                                 EFileWrite | EFileShareExclusive));
    } else {
        User::Panic(KPanic, err);
    }

    // Ugly: Buffer for the message. We don't know how much space is really
    // needed
    HBufC *buf = HBufC::New(KFormatBufferSize);
    if ( !buf ) {
        User::Panic(KPanic, 1);
    }
    TPtr ptr = buf->Des();

    // Create a timestamp and write it first on the line    
    TTime time;
    time.HomeTime();
    TRAP(err, time.FormatL(ptr, KTimestampFormat));
    PanicIfError(err);
    TPtrC8 ptr8((TUint8*)ptr.Ptr(), ptr.Size());
    PanicIfError(file.Write(ptr8));

    // Format the message, and write it
    VA_LIST args;
    VA_START(args, aFormat);
    ptr.FormatList(aFormat, args);
    VA_END(args);
    ptr8.Set((TUint8*)ptr.Ptr(), ptr.Size());
    PanicIfError(file.Write(ptr8));

    // End with a linefeed
    ptr = KCRLF;
    ptr8.Set((TUint8*)ptr.Ptr(), ptr.Size());
    PanicIfError(file.Write(ptr8));

    delete buf;
    file.Close();
    fs.Close();    
}


#endif
