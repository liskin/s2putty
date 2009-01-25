/*
 * console.c: various interactive-prompt routines shared between
 * the console PuTTY tools
 *
 * Hacked version for Symbian SSH test
 *
 */

#include <e32std.h>
#include <e32cons.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern "C" {
#include "putty.h"
#include "storage.h"
#include "ssh.h"
}

#include "sshtestconsole.h"
#include "charutil.h"



int console_batch_mode = FALSE;


_LIT(absentmsg_batch,
     "The server's host key is not cached in the registry. You\n\
have no guarantee that the server is the computer you\n\
think it is.\n\
The server's key fingerprint is:\n\
%S\n\
Connection abandoned.\n");

// _LIT(absentmsg,
//      "The server's host key is not cached in the registry. You\n\
// have no guarantee that the server is the computer you\n\
// think it is.\n\
// The server's key fingerprint is:\n\
// %S\n\
// If you trust this host, enter \"y\" to add the key to\n\
// PuTTY's cache and carry on connecting.\n\
// If you want to carry on connecting just once, without\n\
// adding the key to the cache, enter \"n\".\n\
// If you do not trust this host, press Return to abandon the\n\
// connection.\n\
// Store key in cache? (y/n) ");

_LIT(absentmsg,
     "Key not in registry. Fingerprint: \n%S\nStore (y/n) or quit (Enter)?\n");

_LIT(wrongmsg_batch,
     "WARNING - POTENTIAL SECURITY BREACH!\n\
The server's host key does not match the one PuTTY has\n\
cached in the registry. This means that either the\n\
server administrator has changed the host key, or you\n\
have actually connected to another computer pretending\n\
to be the server.\n\
The new key fingerprint is:\n\
%S\n\
Connection abandoned.\n");

_LIT(wrongmsg,
     "WARNING - POTENTIAL SECURITY BREACH!\n\
The server's host key does not match the one PuTTY has\n\
cached in the registry. This means that either the\n\
server administrator has changed the host key, or you\n\
have actually connected to another computer pretending\n\
to be the server.\n\
The new key fingerprint is:\n\
%S\n\
If you were expecting this change and trust the new key,\n\
enter \"y\" to update PuTTY's cache and continue connecting.\n\
If you want to carry on connecting but without updating\n\
the cache, enter \"n\".\n\
If you want to abandon the connection completely, press\n\
Return to cancel. Pressing Return is the ONLY guaranteed\n\
safe choice.\n\
Update cached key? (y/n, Return cancels connection) ");

_LIT(abadoned,
     "Connection abadoned.\n");
     

void verify_ssh_host_key(char *host, int port, char *keytype,
			 char *keystr, char *fingerprint)
{
    int ret;

    /*
     * Verify the key against the registry.
     */
    ret = verify_host_key(host, port, keytype, keystr);

    if (ret == 0)		       /* success - key matched OK */
	return;

    TPtr *fpDes = CreateDes(fingerprint);
    
    if (ret == 2) {		       /* key was different */
	if (console_batch_mode) {
            console->Printf(wrongmsg_batch, fpDes);
	    exit(1);
	}
	console->Printf(wrongmsg, fpDes);
    }
    if (ret == 1) {		       /* key was absent */
	if (console_batch_mode) {
            console->Printf(absentmsg_batch, fpDes);
	    exit(1);
	}
	console->Printf(absentmsg, fpDes);
    }
    DeleteDes(fpDes);

    TBuf<32> line;
    ReadConsoleLine(line);

    if ( line.Length() > 0 ) {
	if (line[0] == 'y' || line[0] == 'Y')
	    store_host_key(host, port, keytype, keystr);
    } else {
        console->Printf(abadoned);
	exit(0); // FIXME
    }
}



_LIT(askcipher_msg,
     "The first %Scipher supported by the server is\n\
%S, which is below the configured warning threshold.\n\
Continue with connection? (y/n) ");
_LIT(askcipher_msg_batch,
     "The first %Scipher supported by the server is\n\
%S, which is below the configured warning threshold.\n\
Connection abandoned.\n");
_LIT(bothways, "");
_LIT(client_to_server,
     "client-to-server ");
_LIT(server_to_client,
     "server-to-client ");
     

/*
 * Ask whether the selected cipher is acceptable (since it was
 * below the configured 'warn' threshold).
 * cs: 0 = both ways, 1 = client->server, 2 = server->client
 */
void askcipher(char *ciphername, int cs)
{
    TPtr *cipherDes = CreateDes(ciphername);

    if (console_batch_mode) {
        console->Printf(askcipher_msg_batch,
                        (cs == 0) ? &bothways :
                        (cs == 1) ? &client_to_server : &server_to_client,
                        cipherDes);
	exit(1);
    }

    console->Printf(askcipher_msg,
                    (cs == 0) ? &bothways :
                    (cs == 1) ? &client_to_server : &server_to_client,
                    cipherDes);
    
    DeleteDes(cipherDes);

    TBuf<32> line;
    ReadConsoleLine(line);

    if ( (line.Length() > 0) && ((line[0] == 'y' || line[0] == 'Y')) ) {
	return;
    } else {
        console->Printf(abadoned);
	exit(0); //FIXME
    }
}


_LIT(appendmsg,
     "The session log file \"%S\" already exists.\n\
You can overwrite it with a new session log,\n\
append your session log to the end of it,\n\
or disable session logging for this session.\n\
Enter \"y\" to wipe the file, \"n\" to append to it,\n\
or just press Return to disable logging.\n\
Wipe the log file? (y/n, Return cancels logging) ");

_LIT(appendmsg_batch,
     "The session log file \"%S\" already exists.\n\
Logging will not be enabled.\n");

/*
 * Ask whether to wipe a session log file before writing to it.
 * Returns 2 for wipe, 1 for append, 0 for cancel (don't log).
 */
int askappend(char *filename)
{
    TPtr *fileDes = CreateDes(filename);
    if (console_batch_mode) {
        console->Printf(appendmsg_batch, fileDes);
        DeleteDes(fileDes);
        return 0;
    }
    console->Printf(appendmsg, fileDes);
    DeleteDes(fileDes);

    TBuf<32> line;
    ReadConsoleLine(line);
    if ( (line.Length() == 0) ) {
        return 0;
    } else if ( (line[0] == 'y') || (line[0] == 'Y') ) {
        return 2;
    } else if ( (line[0] == 'n') || (line[0] == 'N') ) {
        return 1;
    } else {
        return 0;
    }
}


_LIT(obsolete_msg,
     "You are loading an SSH 2 private key which has an\n\
old version of the file format. This means your key\n\
file is not fully tamperproof. Future versions of\n\
PuTTY may stop supporting this private key format,\n\
so we recommend you convert your key to the new\n\
format.\n\
\n\
Once the key is loaded into PuTTYgen, you can perform\n\
this conversion simply by saving it again.\n");     

/*
 * Warn about the obsolescent key file format.
 */
void old_keyfile_warning(void)
{
    console->Printf(obsolete_msg);
}

#if 0
char *console_password = NULL;

int console_get_line(const char *prompt, char *str,
			    int maxlen, int is_pw)
{
    HANDLE hin, hout;
    DWORD savemode, newmode, i;

    if (is_pw && console_password) {
	static int tried_once = 0;

	if (tried_once) {
	    return 0;
	} else {
	    strncpy(str, console_password, maxlen);
	    str[maxlen - 1] = '\0';
	    tried_once = 1;
	    return 1;
	}
    }

    if (console_batch_mode) {
	if (maxlen > 0)
	    str[0] = '\0';
    } else {
	hin = GetStdHandle(STD_INPUT_HANDLE);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hin == INVALID_HANDLE_VALUE || hout == INVALID_HANDLE_VALUE) {
	    fprintf(stderr, "Cannot get standard input/output handles\n");
	    exit(1);
	}

	GetConsoleMode(hin, &savemode);
	newmode = savemode | ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT;
	if (is_pw)
	    newmode &= ~ENABLE_ECHO_INPUT;
	else
	    newmode |= ENABLE_ECHO_INPUT;
	SetConsoleMode(hin, newmode);

	WriteFile(hout, prompt, strlen(prompt), &i, NULL);
	ReadFile(hin, str, maxlen - 1, &i, NULL);

	SetConsoleMode(hin, savemode);

	if ((int) i > maxlen)
	    i = maxlen - 1;
	else
	    i = i - 2;
	str[i] = '\0';

	if (is_pw)
	    WriteFile(hout, "\r\n", 2, &i, NULL);

    }
    return 1;
}
#endif



CConsoleBase *console;

CConsoleReader *reader;


void InitConsoleL() {
    console = Console::NewL(_L("SSH test"),
                            TSize(KConsFullScreen, KConsFullScreen));
    reader = 0;
}

void FreeConsole() {
    delete console;
    console = 0;
}

void InitConsoleReaderL() {

    reader = new CConsoleReader();
    reader->ConstructL();
}


void FreeConsoleReader() {
    delete reader;
    reader = 0;
}



void ReadConsoleLine(TDes &aTarget, TBool anEcho) {

    if ( reader ) {
        reader->Cancel();
    }

    int pos = 0;
    TKeyCode key = EKeyNull;

    while ( key != EKeyEnter ) {

        key = console->Getch();
        
        if ( key == EKeyBackspace ) {
            if ( pos > 0 ) {
                pos--;
                aTarget.SetLength(pos);
                if ( anEcho ) {
                    console->SetPos(console->WhereX()-1);
                    console->Printf(_L(" "));
                    console->SetPos(console->WhereX()-1);
                }
            }
            
        } else if ( key == EKeyEnter ) {
            break;            
            
        } else {
            if ( pos < (aTarget.MaxLength()-1) ) {
                aTarget.Append((TChar)key);
                pos++;
                if ( anEcho ) {
                    console->Printf(_L("%c"), (TUint) key);
                }
            }
        }
    }    

    if ( anEcho ) {
        console->Write(_L("\n"));
    }
    
    if ( reader ) {
        reader->Activate();
    }
}



CConsoleReader::CConsoleReader() : CActive(EPriorityNormal) {
    iStatus = KErrNone;
}

CConsoleReader::~CConsoleReader() {
    Cancel();
}

void CConsoleReader::ConstructL() {
    CActiveScheduler::Add(this);
    SetActive();
    console->Read(iStatus);
}

void CConsoleReader::RunL() {
    if ( iStatus != KErrNone ) {
        User::Panic(_L("CConsoleReader"), iStatus.Int());
    }
    TKeyCode code = console->KeyCode();
    TUint modifiers = console->KeyModifiers();
    noise_ultralight((DWORD)code);
    SendKey(code, modifiers);
    SetActive();
    console->Read(iStatus);
}

void CConsoleReader::DoCancel() {
    if ( iStatus == KRequestPending ) {
        console->ReadCancel();
    }
}

void CConsoleReader::Activate() {
    SetActive();
    console->Read(iStatus);
}
