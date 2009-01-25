/*    puttyapp.h
 *
 * Putty UI Application class
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __PUTTYAPP_H__
#define __PUTTYAPP_H__

#include <eikapp.h>

/**
 * PuTTY UI Application class. Does very little, just creates the document.
 */
class CPuttyApplication : public CEikApplication {

private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;    
};

#endif
