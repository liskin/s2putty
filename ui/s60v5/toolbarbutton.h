/*    customtoolbar.h
 *
 * Button class for custom toolbar
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#ifndef CUSTOMTOOLBARBUTTON_H
#define CUSTOMTOOLBARBUTTON_H

#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include "puttyui.hrh"

struct toolbarButtonData {
    TPuttyToolbarCommands iAction;
    TBuf<6> iUpLabel;
    TBuf<6> iDownLabel;
    TBuf<25> iSettingsLabel;
    TBool iCustom;
    TBool iCtrlModifier;
    TBool iAltModifier;
    TBool iSendEscFirst;
    TBuf<40> iSendkeys;
    TFileName iButtonUpIconFile;
    TFileName iButtonDownIconFile;
    TInt  iCustomCmdId;
};

/*
static toolbarButtonData KDefaultToolbarButtons [ ] = {
        { 0, _L("Tab"), _L("Tab"), _L("Tab"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0  },
        { 1, _L("Alt"), _L("Alt"), _L("Alt+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },      
        { 2, _L("Unlock"), _L("Lock"), _L("Ctrl+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 }
};
*/

static toolbarButtonData KDefaultToolbarButtons [ ] = {
        { EPuttyToolbarTab, _L("Tab"),  _L("Tab"), _L("Tab"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0  },
        { EPuttyToolbarAltP, _L("Alt"), _L("Alt"), _L("Alt+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },      
        { EPuttyToolbarCtrlP, _L("Ctrl"), _L("Ctrl"), _L("Ctrl+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L("") },
        { EPuttyToolbarLock, _L("Unlock"), _L("Lock"), _L("Toggle toolbar lock"), EFalse, EFalse, EFalse, EFalse, _L("UnLock"), _L(""), _L(""), 0 },
        { EPuttyToolbarSelect, _L("Select"), _L("Select"), _L("Select"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarCopy, _L("Copy"), _L("Copy"), _L("Copy"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPaste, _L("Paste"), _L("Paste"), _L("Paste"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPipe, _L("Pipe"), _L("Pipe"), _L("Pipe |"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowUp, _L("Up"), _L("Up"), _L("Arrow up"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowDown, _L("Down"), _L("Down"), _L("Arrow down"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowLeft, _L("Left"), _L("Left"), _L("Arrow left"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowRight, _L("Right"), _L("Right"), _L("Arrow right"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEsc, _L("Esc"), _L("Esc"),  _L("Esc"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPageUp, _L("PUp"), _L("PUp"), _L("Page up"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPageDown, _L("PDown"), _L("PDown"), _L("Page down"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarHome, _L("Home"), _L("Home"), _L("Home"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEnd, _L("End"), _L("End"), _L("End"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarDelete, _L("Delete"), _L("Delete"), _L("Delete"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarInsert, _L("Insert"), _L("Insert"), _L("Insert"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEnter, _L("Enter"), _L("Enter"), _L("Enter"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 } ,
        { EPuttyToolbarNoTool, _L("TheEnd"), _L(""), _L(""), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 }
};



class CCustomToolbarButton : public CBase {
    public:
        ~CCustomToolbarButton();        
        static CCustomToolbarButton* NewLC (TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel);
        static CCustomToolbarButton* NewL (TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel);

        static CCustomToolbarButton* NewLC (TRect aItemRect, toolbarButtonData aButtonData);
        static CCustomToolbarButton* NewL (TRect aItemRect, toolbarButtonData aButtonData);
        
        
        void GenerateIconL();
        void GenerateIconUpL(TInt fontSize, TInt backgroundTransparency, TInt TextTransparency);       
        void GenerateIconDownL(TInt fontSize, TInt backgroundTransparency, TInt TextTransparency);       
        void SetButtonDown(TBool aDown) { iDown = aDown; };
        TBool GetButtonDown() {return iDown; };
        TBool GetButtonSelectable() {return iSelectable; };
        void SetButtonSelectable(TBool aSelectable) { iSelectable = aSelectable; };
        void SetRect(TRect aButtonRect) { iItem = aButtonRect; };
        TRect GetRect() { return iItem; };
        toolbarButtonData GetButtonData() { return iButtonData; };
        TInt GetFontSize() {return iFontSize; };
        CFbsBitmap* GetButtonDownBitmap() { return iButtonDownBitmap; };
        CFbsBitmap* GetButtonDownBitmapMask() { return iButtonDownBitmapMask; };
        CFbsBitmap* GetButtonUpBitmap() { return iButtonUpBitmap; };
        CFbsBitmap* GetButtonUpBitmapMask() { return iButtonUpBitmapMask; };
        
        
        
    private:
        CCustomToolbarButton();
        void ConstructL(TRect aItemRect, TPuttyToolbarCommands aAction, TDes aLabel);
        void ConstructL(TRect aItemRect, toolbarButtonData aButtonData);
        void ClearIcons();
        CFbsBitmap* CreateBitmapL( TDes& aText , TSize aTargetSize, TInt *aFontSize, TBool aMask, TInt Transparency, TInt TextTransparency );

        CFbsBitmap*             iButtonDownBitmap;
        CFbsBitmap*             iButtonDownBitmapMask;
        CFbsBitmap*             iButtonUpBitmap;
        CFbsBitmap*             iButtonUpBitmapMask;
        TRect                   iItem;
        TBool                   iDown;
        TBool                   iSelectable;
        TInt                    iCustomCmdId;
        TInt                    iFontSize;
        
        toolbarButtonData       iButtonData;
        
        TBool                   iIconsAvailable;
};

#endif
