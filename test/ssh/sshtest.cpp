#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <stdlib.h>
#include <stdarg.h>

extern "C" {

#define PUTTY_DO_GLOBALS
#include "putty.h"

}

#include "epocnet.h"
#include "epocmemory.h"
#include "epocnoise.h"

#include "sshtestconsole.h"


struct backend_list backends[] = {
    {Config::PROT_SSH, "ssh", &ssh_backend},
    {0, NULL}
};



static char errorFmtBuf[1024];

void fatalbox(char *p, ...)
{
    va_list ap = 0;
    va_start(ap, p);
    vsprintf(errorFmtBuf, p, ap);
    va_end(ap);
    console->Printf(_L("\nFATAL ERROR: %s\nPress a key to exit\n"), errorFmtBuf);
    if ( reader ) {
        reader->Cancel();
    }
    console->Getch();
    exit(1);
}

void connection_fatal(char *p, ...)
{
    va_list ap = 0;
    va_start(ap, p);
    vsprintf(errorFmtBuf, p, ap);
    va_end(ap);
    console->Printf(_L("\nCONNECTION FATAL ERROR: %s\nPress a key to exit\n"), errorFmtBuf);
    if ( reader ) {
        reader->Cancel();
    }
    console->Getch();
    exit(1);
}

int term_ldisc(int /*mode*/)
{
    return FALSE;
}

extern "C" void ldisc_update(int /*echo*/, int /*edit*/)
{
}

void logevent(char * /*string*/) {
}

void log_packet(int /*direction*/, int /*type*/, char * /*texttype*/,
                void * /*data*/, int /*len*/) {
}


int from_backend(int /*is_stderr*/, char *data, int len)
{
    #define BUFSIZE 32
    TBuf<BUFSIZE> buf;
    char *c = data;
    
    while ( len > 0 ) {
        TInt doNow = BUFSIZE;
        if ( doNow > len ) {
            doNow = len;
        }
        len -= doNow;
        buf.SetLength(doNow);
        int p = 0;
        while ( doNow-- ) {
            buf[p++] = *c++;
        }
        console->Write(buf);
    }

    return 0;
}


char *do_select(RSocketS * /*skt*/, int /*startup*/)
{
    return NULL;
}


class CSocketWatcher : public CBase, public MSocketWatcher {
public:
    CSocketWatcher();
    ~CSocketWatcher();
    void ConstructL();
    void SocketOpened();
    void SocketClosed();

private:
    TInt numOpen;
};

CSocketWatcher::CSocketWatcher() {
    numOpen = 0;
}

CSocketWatcher::~CSocketWatcher() {
}

void CSocketWatcher::ConstructL() {
}

void CSocketWatcher::SocketOpened() {
    numOpen++;
}

void CSocketWatcher::SocketClosed() {
    numOpen--;
    if ( numOpen < 1 ) {
        CActiveScheduler::Stop();
    }
}





static void doTestL() {

    epoc_memory_init();
    epoc_noise_init();
    
    // A stripped-down version of main() from plink.c
    int connopen;
    reader = 0;

    default_protocol = Config::PROT_SSH;
    default_port = 22;
    
    flags = FLAG_INTERACTIVE;
    /*
     * Process the command line.
     */
    do_defaults(NULL, &cfg);
    default_protocol = cfg.protocol;
    default_port = cfg.port;

//    strcpy(cfg.host, "10.1.1.1");
    strcpy(cfg.host, "unix.saunalahti.fi");
    
    /*
     * Select protocol. This is farmed out into a table in a
     * separate file to enable an ssh-free variant.
     */
    {
	int i;
	back = NULL;
	for (i = 0; backends[i].backend != NULL; i++)
	    if (backends[i].protocol == cfg.protocol) {
		back = backends[i].backend;
		break;
	    }
	if (back == NULL) {
	    fprintf(stderr,
		    "Internal fault: Unsupported protocol found\n");
            exit(1);
	}
    }

    // Create a new active scheduler
    CActiveScheduler *scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    /*
     * Initialise WinSock.
     */
    sk_init();

    // Create socket watcher
    CSocketWatcher *watcher = new CSocketWatcher();
    watcher->ConstructL();
    CleanupStack::PushL(watcher);
    sk_set_watcher(watcher);

    /*
     * Start up the connection.
     */
    {
	char *error;
	char *realhost;
	/* nodelay is only useful if stdin is a character device (console) */
	int nodelay = cfg.tcp_nodelay;

        console->Printf(_L("back->init..."));
	error = back->init(cfg.host, cfg.port, &realhost, nodelay);
        console->Printf(_L("done\n"));
	if (error) {
	    fprintf(stderr, "Unable to open connection:\n%s", error);
	    exit(1);
	}
	sfree(realhost);
    }
    connopen = 1;

    InitConsoleReaderL();

    while ( connopen ) {
        CActiveScheduler::Start();

        if ( ((!connopen) || (back->socket() == NULL)) &&
             (sk_num_active_sockets() == 0) ) {
            break;
        }
        
//         if ((!connopen || back->socket() == NULL) &&
//             bufchain_size(&stdout_data) == 0 &&
//             bufchain_size(&stderr_data) == 0)
//             break;		       /* we closed the connection */
    }

    sk_set_watcher(NULL);
    CleanupStack::PopAndDestroy(); // watcher

    FreeConsoleReader();
    CleanupStack::PopAndDestroy(); // scheduler

    sk_close_networking();
    epoc_noise_free();
    epoc_memory_free();
}


static void doMainL() {
    InitConsoleL();

    __UHEAP_MARK;
    TRAPD(error, doTestL());
    if ( error ) {
        console->Printf(_L("\nLeave %d\n"), error);
        console->Getch();
    }
    CloseSTDLIB();
    __UHEAP_MARKEND;

    FreeConsole();
}


GLDEF_C TInt E32Main() {
    
    CTrapCleanup *cleanup = CTrapCleanup::New();
    TRAPD(error, doMainL());
    if ( error != KErrNone ) {
        User::Panic(_L("SSHTest"), error);
    }
    delete cleanup;
    
    return 0;
}

#if defined(__WINS__)

EXPORT_C TInt WinsMain(TAny *)
{
	E32Main();
	return(KErrNone);
}

GLDEF_C TInt E32Dll(TDllReason)
{
	return(KErrNone);
}
#endif
