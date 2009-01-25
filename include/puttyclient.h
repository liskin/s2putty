/** @file puttyclient.h
 *
 * PuTTY client interface, implemented by the UI application
 *
 * Copyright 2002,2005 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYCLIENT_H__
#define __PUTTYCLIENT_H__

#include <e32std.h>
#include <gdi.h>

/**
 * The PuTTY client interface class. The client interface is
 * implemented by the user-interface application, and used by the
 * engine to send information back to the client.
 */
class MPuttyClient {

public:
    // Constants

    /** Host key prompt responses */
    enum THostKeyResponse {
        EAbadonConnection,  /**< Abadon the connection and do not accept the key */
        EAcceptTemporarily, /**< Accept the key for this session */
        EAcceptAndStore     /**< Accept the key and store it for future use */
    };

    
    // Methods

    /** 
     * Draws text on the terminal window. The coordinates are
     * zero-based character coordinates inside the terminal.
     * 
     * @param aX Text start X-coordinate
     * @param aY Text start Y-coordinate
     * @param aText The text to draw
     * @param aBold Bold attribute
     * @param aUnderline Underline attribute
     * @param aForeground Foreground color to use
     * @param aBackground Background color to use
     */
    virtual void DrawText(TInt aX, TInt aY, const TDesC &aText, TBool aBold,
                          TBool aUnderline, TRgb aForeground,
                          TRgb aBackground) = 0;

    /** 
     * Sets the cursor position in the terminal window. The coordinates are
     * zero-based character coordinates inside the terminal, the same as
     * those used in DrawText().
     * 
     * @param aX New cursor X-coordinate
     * @param aY New cursor Y-coordinate
     */
    virtual void SetCursor(TInt aX, TInt aY) = 0;

    /** 
     * Reports a connection error to the user. After displaying the
     * message, the engine must be re-initialized
     * 
     * @param aMessage The error message
     */
    virtual void ConnectionError(const TDesC &aMessage) = 0;

    /** 
     * Reports a fatal error to the user. After showing the message,
     * the implementation must exit the program.
     * 
     * @param aMessage The error message
     */
    virtual void FatalError(const TDesC &aMessage) = 0;

    /** 
     * Reports that the connection has been closed normally.
     */
    virtual void ConnectionClosed() = 0;

    /** 
     * Prompts the user to accept or reject an unknown host key. The
     * method must display the key fingerprint to the user, and ask
     * whether it should be accepted once, accepted permanently or
     * rejected completely.
     * 
     * @param aFingerprint The key fingerprint to display
     * @return User selection
     * @see THostKeyResponse
     */
    virtual THostKeyResponse UnknownHostKey(const TDesC &aFingerprint) = 0;

    /** 
     * Prompts the user to accept or reject a changed host key. The
     * method must display the key fingerprint to the user, and ask
     * whether it should be accepted once, accepted permanently or
     * rejected completely.
     * 
     * @param aFingerprint The key fingerprint to display
     * @return User selection
     * @see THostKeyResponse
     */
    virtual THostKeyResponse DifferentHostKey(const TDesC &aFingerprint) = 0;

    /** 
     * Prompts the user to accept or reject a cipher below the warning
     * threshold.
     * 
     * @param aCipherName The cipher name
     * @param aDirection The cipher usage description
     * 
     * @return ETrue if the cipher is accepted
     * @see TCipherDirection
     */
    virtual TBool AcceptCipher(const TDesC &aCipherName,
                               const TDesC &aCipherUsage) = 0;

    /** 
     * Prompts the user to enter a username, password, or RSA key
     * passphrase. Used for SSH authention.
     * 
     * @param aPrompt Dialog prompt
     * @param aTarget Target descriptor for user input
     * @param aSecret ETrue if the value is a secret, e.g. a password
     * 
     * @return ETrue if the user entered valid input
     */
    virtual TBool AuthenticationPrompt(const TDesC &aPrompt, TDes &aTarget,
                                       TBool aSecret) = 0;
};


#endif
