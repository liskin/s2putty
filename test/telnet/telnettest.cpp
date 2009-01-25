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

static CConsoleBase *console;

class CConsoleReader : public CActive {

public:
    CConsoleReader();
    ~CConsoleReader();
    void ConstructL();
    void RunL();
    void DoCancel();
};

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
    char key = (char) code;
    back->send(&key, 1);
    SetActive();
    console->Read(iStatus);
}

void CConsoleReader::DoCancel() {
    if ( iStatus == KRequestPending ) {
        console->ReadCancel();
    }
}

static CConsoleReader *reader;


struct backend_list backends[] = {
    {Config::PROT_TELNET, "telnet", &telnet_backend},
    {0, NULL}
};


void random_save_seed(void)
{
}

void random_destroy_seed(void)
{
}

void noise_ultralight(DWORD /*data*/)
{
}

void noise_regular(void)
{
}

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





static void doTestL() {

    init_epoc_memory();
    
    // A stripped-down version of main() from plink.c
    int connopen;
    reader = 0;

    default_protocol = Config::PROT_TELNET;
    default_port = 23;
    
    flags = 0;
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

    reader = new CConsoleReader();
    CleanupStack::PushL(reader);
    reader->ConstructL();

    while ( connopen ) {
        CActiveScheduler::Start();

        if ( (!connopen) || (back->socket() == NULL) ) {
            break;
        }
        
//         if ((!connopen || back->socket() == NULL) &&
//             bufchain_size(&stdout_data) == 0 &&
//             bufchain_size(&stderr_data) == 0)
//             break;		       /* we closed the connection */
    }

    CleanupStack::PopAndDestroy(2); // reader, scheduler

    sk_close_networking();
    free_epoc_memory();
}


static void doMainL() {
    console = Console::NewL(_L("Telnet test"),
                            TSize(KConsFullScreen, KConsFullScreen));
    CleanupStack::PushL(console);
    __UHEAP_MARK;
    TRAPD(error, doTestL());
    if ( error ) {
        console->Printf(_L("\nLeave %d\n"), error);
        console->Getch();
    }
    CloseSTDLIB();
    __UHEAP_MARKEND;

    CleanupStack::PopAndDestroy(); // console
}


GLDEF_C TInt E32Main() {
    
    CTrapCleanup *cleanup = CTrapCleanup::New();
    TRAPD(error, doMainL());
    if ( error != KErrNone ) {
        User::Panic(_L("TelnetTest"), error);
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
