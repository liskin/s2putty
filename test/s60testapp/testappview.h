/*    testappview.h
 *
 * Test View class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TESTAPPVIEW_H__
#define __TESTAPPVIEW_H__

#include <coecntrl.h>

class CTestAppUi;


/**
 * Test application view class.
 */
class CTestAppView : public CCoeControl {
    
public:
    CTestAppView(CTestAppUi &aAppUi);
    ~CTestAppView();
    void ConstructL(const TRect& aRect);

private:
    void Draw(const TRect &aRect) const;
    
    CTestAppUi &iAppUi;
};


#endif
