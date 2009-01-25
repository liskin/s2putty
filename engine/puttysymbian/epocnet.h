/** @file epocnet.h
 *
 * Symbian OS implementation of PuTTY networking support
 *
 * Mostly copied from the SyOSsh EPOC Release 5 port by Gabor Keresztfalvi,
 * some additional fixes for the current Symbian port by Petteri Kangaslampi
 *
 * [No copyright messages in the original, assumed to be copyrighted by
 *  Gabor Keresztfavli and available through the PuTTY license]
 *
 * Portions copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PUTTY_EPOCNET_H
#define PUTTY_EPOCNET_H

#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include <apgtask.h>
#include <es_sock.h>
#include <in_sock.h>


/**
 * A socket activity watcher mixin interface class.
 * Used by the engine to keep track of
 * the number of open sockets.
 */
class MSocketWatcher {
public:
    /** Called when a socket is opened. */
    virtual void SocketOpened() = 0;
    
    /** Called when a socket is closed. */
    virtual void SocketClosed() = 0;
};


/** 
 * Sets a socket watcher object. The watcher will be notified when a socket
 * is opened or closed.
 * 
 * @param aWatcher The watcher object. Set to NULL to remove the watcher.
 *
 * @see MSocketWatcher
 */
void sk_set_watcher(MSocketWatcher *aWatcher);


/** 
 * Sets the connection to use. Must be called after sk_init(), and
 * the RSocketServ and RConnection objects must remain valid until
 * sk_cleanup().
 *
 * @param aSocketServ The socket server session to use
 * @param aConnection The network connection to use.
 */
void sk_set_connection(RSocketServ &aSocketServ, RConnection &aConnection);


/** 
 * Gets the number of active sockets.
 * 
 * @return The number of active sockets.
 */
int sk_num_active_sockets(void);


/** 
 * Sets the logging context to use for networking log messages.
 * 
 * @param aLogCtx The log context
 */
void sk_provide_logctx(void *aLogCtx);


class CNetAS : public CActiveScheduler
{
	void Error(TInt err) const
	{ 
		_LIT(KMsgErr,"CNetAS-error");
		User::Panic(KMsgErr,err);
	};
};

class CRecver : public CActive
{
public:
	static CRecver *NewL(Socket p);
	~CRecver();
	void StartRecver();
	void Reissue();
private:
	CRecver();
	void ConstructL(Socket p);
	void RunL();		// From CActive
	void DoCancel();	// From CActive
        void DoRunL();

private:
	TBuf8<2048> readbuf;
	TSockXfrLength readlen;
	Socket parent;
};

class CSender : public CActive
{
public:
	static CSender *NewL(Socket p);
	~CSender();
	void Sendit();
private:
	CSender();
	void ConstructL(Socket p);
	void RunL();		// From CActive
	void DoCancel();	// From CActive
        void DoRunL();

private:
	TPtrC8 writebuf;
	Socket parent;
};

class CAcceptor : public CActive
{
public:
	static CAcceptor *NewL(Socket p);
	~CAcceptor();
	void StartAcceptor();
private:
	CAcceptor();
	void ConstructL(Socket p);
	void RunL();		// From CActive
	void DoCancel();	// From CActive
        void DoRunL();

private:
	Socket parent;
};

class RSocketS : public RSocket
{
public:
	TInt id, selectflags;
	TRequestStatus estat;
	CRecver *r;
	CSender *s;
	CAcceptor *a;
};

/* Some defines... */
#ifndef SOCKET
#define SOCKET RSocketS *
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET NULL
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR NULL
#endif

#define KInvalSocket 100
#define MAXSOCKETS 10

#define FD_CONNECT 1
#define FD_READ 2
#define FD_WRITE 3
#define FD_EXCEPTION 4

#endif
