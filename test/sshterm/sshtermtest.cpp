#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

extern "C" {

#define PUTTY_DO_GLOBALS
#include "putty.h"

}

#include "epocnet.h"
#include "epocmemory.h"
#include "epocnoise.h"
#include "charutil.h"

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

// int term_ldisc(int /*mode*/)
// {
//     return FALSE;
// }

extern "C" void ldisc_update(int /*echo*/, int /*edit*/)
{
}

void logevent(char * /*string*/) {
}

void log_packet(int /*direction*/, int /*type*/, char * /*texttype*/,
                void * /*data*/, int /*len*/) {
}

void logtraffic(unsigned char /*c*/, int /*logmode*/)
{
}


// int from_backend(int /*is_stderr*/, char *data, int len)
// {
//     #define BUFSIZE 32
//     TBuf<BUFSIZE> buf;
//     char *c = data;
    
//     while ( len > 0 ) {
//         TInt doNow = BUFSIZE;
//         if ( doNow > len ) {
//             doNow = len;
//         }
//         len -= doNow;
//         buf.SetLength(doNow);
//         int p = 0;
//         while ( doNow-- ) {
//             buf[p++] = *c++;
//         }
//         console->Write(buf);
//     }

//     return 0;
// }


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



// Callbacks from terminal.c

Context get_ctx(void)
{
    return (void*) 42;
}

void free_ctx(Context /*ctx*/)
{
}


/*
 * Move the system caret. (We maintain one, even though it's
 * invisible, for the benefit of blind people: apparently some
 * helper software tracks the system caret, so we should arrange to
 * have one.)
 */
void sys_cursor(int /*x*/, int /*y*/)
{
}


void set_sbar(int /*total*/, int /*start*/, int /*page*/)
{
}


void palette_set(int /*n*/, int /*r*/, int /*g*/, int /*b*/)
{
}

void palette_reset(void)
{
}

char *get_window_title(int icon)
{
    return icon ? "PuttY.Icon" : "PuTTY";
}


/*
 * Report the window's position, for terminal reports.
 */
void get_window_pos(int *x, int *y)
{
    *x = 0;
    *y = 0;
}

/*
 * Report the window's pixel size, for terminal reports.
 */
void get_window_pixels(int *x, int *y)
{
    *x = 640;
    *y = 200;
}

/*
 * Report whether the window is iconic, for terminal reports.
 */
int is_iconic(void)
{
    return 0;
}


/*
 * Maximise or restore the window in response to a server-side
 * request.
 */
void set_zoomed(int /*zoomed*/)
{
}

/*
 * Refresh the window in response to a server-side request.
 */
void refresh_window(void)
{
}


/*
 * Move the window to the top or bottom of the z-order in response
 * to a server-side request.
 */
void set_zorder(int /*top*/)
{
}

/*
 * Move the window in response to a server-side request.
 */
void move_window(int /*x*/, int /*y*/)
{
}


/*
 * Minimise or restore the window in response to a server-side
 * request.
 */
void set_iconic(int /*iconic*/)
{
}

void request_resize(int /*w*/, int /*h*/)
{
}

/*
 * Beep.
 */
void beep(int /*mode*/)
{
}

/*
 * set or clear the "raw mouse message" mode
 */
void set_raw_mouse_mode(int /*activate*/)
{
}

void set_title(char * /*title*/)
{
}

void set_icon(char * /*title*/)
{
}



void do_cursor(Context /*ctx*/, int x, int y, char * /*text*/, int /*len*/,
	       unsigned long /*attr*/, int /*lattr*/) {
    console->SetPos(x, y);
}



/*
 * Draw a line of text in the window, at given character
 * coordinates, in given attributes.
 *
 * We are allowed to fiddle with the contents of `text'.
 */
void do_text(Context /*ctx*/, int x, int y, char *text, int len,
	     unsigned long attr, int /*lattr*/)
{

    // Make sure we have a resonable character set
    assert((attr & CSET_MASK) != ATTR_OEMCP);
    assert((attr & CSET_MASK) != ATTR_ACP);
    assert(!DIRECT_CHAR(attr));
    assert(!DIRECT_FONT(attr));

    // Convert to proper wide chars (why aren't they so already? grr)
    static TUint16 *wbuf = NULL;
    static int wlen = 0;
    int i;
    if ( wlen < len ) {
        sfree(wbuf);
        wlen = len;
        wbuf = (TUint16*) smalloc(wlen * sizeof(TUint16));
    }
    for (i = 0; i < len; i++)
        wbuf[i] = (TUint16) ((attr & CSET_MASK) + (text[i] & CHAR_MASK));

    // Draw
    int oldX = console->WhereX();
    int oldY = console->WhereY();
    console->SetPos(x, y);
    TPtrC16 ptr(wbuf, len);
    console->Write(ptr);
    console->SetPos(oldX, oldY);
}



/*
 * Writes stuff to clipboard
 */
void write_clip(wchar_t * /*data*/, int /*len*/, int /*must_deselect*/)
{
}

void get_clip(wchar_t ** /*p*/, int * /*len*/)
{
}


/*
 * Translate a raw mouse button designation (LEFT, MIDDLE, RIGHT)
 * into a cooked one (SELECT, EXTEND, PASTE).
 */
Mouse_Button translate_button(Mouse_Button /*button*/)
{
    return (Mouse_Button) 0;
}


/* This function gets the actual width of a character in the normal font.
 */
int CharWidth(Context /*ctx*/, int /*uc*/) {

    return 1;
}



int UpdateTerminal(TAny * /*anAny*/) {
    term_out();
    term_update();
    return 0;
}



static void doTestL() {

    console->Printf(_L("PuTTY SSH + terminal test, "));
    {
        TBuf<32> date, time;
        StringToDes(__DATE__, date);
        StringToDes(__TIME__, time);
        console->Printf(_L("%S %S\n"), &date, &time);
    }

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

    // Hack some defaults
    strcpy(cfg.line_codepage, "ISO-8859-15");
    cfg.vtmode = VT_UNICODE;
    cfg.sshprot = 2;

    init_ucs_tables();

    {
        console->Printf(_L("Where Do You Want To Go Today?\n"));
        TBuf<256> buf;
        ReadConsoleLine(buf, ETrue);
        char *p = cfg.host;
        for ( int i = 0; i < buf.Length(); i++ ) {
            *p++ = (char) buf[i];
        }
        *p = 0;
    }

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

        console->Printf(_L("Connecting..."));
	error = back->init(cfg.host, cfg.port, &realhost, nodelay);
        console->Printf(_L(" connected\n"));
	if (error) {
	    fprintf(stderr, "Unable to open connection:\n%s", error);
	    exit(1);
	}
	sfree(realhost);
    }
    connopen = 1;

    term_init();
    TSize size = console->ScreenSize();
    term_size(size.iHeight, size.iWidth, cfg.savelines);

    InitConsoleReaderL();

    // yuck
    CPeriodic *termOutPeriodic = CPeriodic::NewL(EPriorityNormal);
    CleanupStack::PushL(termOutPeriodic);
    termOutPeriodic->Start(10000, 10000, TCallBack(UpdateTerminal, NULL));

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

    CleanupStack::PopAndDestroy(); // termOutPeriodic

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
