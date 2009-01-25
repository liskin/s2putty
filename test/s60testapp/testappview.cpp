/*    testappview.cpp
 *
 * Test View class
 *
 * Copyright 2004 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <gdi.h>
#include "testappui.h"
#include "testappview.h"

CTestAppView::CTestAppView(CTestAppUi &aAppUi)
    : iAppUi(aAppUi) {
}


CTestAppView::~CTestAppView() {
}


void CTestAppView::ConstructL(const TRect &aRect) {
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
}


void CTestAppView::Draw(const TRect & /*aRect*/) const {
    CWindowGc &gc = SystemGc();
    gc.Reset();
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbWhite);    
    gc.Clear(Rect());
}
