#ifndef __SSHTESTCONSOLE_H__
#define __SSHTESTCONSOLE_H__

#include <e32cons.h>

extern CConsoleBase *console;

class CConsoleReader : public CActive {

public:
    CConsoleReader();
    ~CConsoleReader();
    void ConstructL();
    void RunL();
    void DoCancel();
    void Activate();
};

extern CConsoleReader *reader;


void InitConsoleL();
void FreeConsole();
void InitConsoleReaderL();
void FreeConsoleReader();

void ReadConsoleLine(TDes &aTarget, TBool anEcho = ETrue);

void SendKey(TKeyCode aCode, TUint aModifiers);


#endif
