#ifndef __FONTTEST_H__
#define __FONTTEST_H__

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <eikon.hrh>

#include <fonttest.rsg>
#include "fonttest.hrh"


class CTestAppView;


// Application
class CTestApplication : public CEikApplication {
    
private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
};


// Document
class CTestDocument : public CEikDocument {
    
public:
    //static CTestDocument *NewL(CEikApplication &anApp);
    CTestDocument(CEikApplication &anApp);
    //void ConstructL();
private: 
    CEikAppUi* CreateAppUiL();
};


// AppUI
class CTestAppUi : public CEikAppUi {
    
public:
    void ConstructL();
    CTestAppUi();
    ~CTestAppUi();

private:
    void HandleCommandL(TInt aCommand);

private:
    CTestAppView *iAppView;
};


// View
class CTestAppView : public CCoeControl {
    
public:
    CTestAppView();
    ~CTestAppView();
    void ConstructL(const TRect& aRect);

private:
    void Draw(const TRect & /*aRect*/) const;
};


#endif

