/*    customtoolbar.h
 *
 * Putty custom toolbar class for touch UI
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#ifndef CUSTOMTOOLBAR_H
#define CUSTOMTOOLBAR_H

#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include "terminalview.h"
#include "terminalcontainer.h"
#include <touchfeedback.h>
#include <touchlogicalfeedback.h>
#include <akniconutils.h>
#include <e32cmn.h>
#include <touchfeedback.h>
#include "touchuisettings.h"
#include "puttyui.hrh"

#include "toolbarbutton.h"

// Forward declarations
class CTerminalView;
class CTerminalContainer;
class CPuttyAppUi;
class TSize;

// How far stylus can go from toolbar while dragging that it is accepted 
const TInt KDraggingAcceptGrow = 40;

// Gap between buttons in the toolbar
const TInt KToolbarItemGap = 5;

// How close from screen borders toolbar can be dragged after it locks into screen borders
const TInt KToolbarGap = 3; 

// How many buttons we can have
const TInt KToolbarMaxItemCount = 8;

// How much toolbar must move after it is regonized as dragging. This value
// helps selecting to button from toolbar because toolbar can move a little while selecting/tapping.
const TInt KDraggingAccepted = 8;

//This is workaround for orientation detection
const TInt KPortraitHeight = 640;

//This is count of available buttons
const TInt KToolbarButtonCount = 20;

enum TOrientation {
  EPortrait = 0,
  ELandscape,
  ETestOnly
};

struct tbButton {
        CFbsBitmap*             iButtonDownBitmap;
        CFbsBitmap*             iButtonDownBitmapMask;
        CFbsBitmap*             iButtonUpBitmap;
        CFbsBitmap*             iButtonUpBitmapMask;
        TRect                   iItem;
        TBool                   iDown;
        TBool                   iSelectable;
        TPuttyToolbarCommands   iAction;
        TInt                    iCustomCmdId;
};

/*
struct toolbarButtonData {
    TInt command;
    TDesC Label;
    TDesC SettingsLabel;
    TBool isCustom;
    TBool isCtrlModifier;
    TBool isAltModifier;
    TBool sendEscFirst;
    TDesC sendkeys;
    TDesC buttonUpIconFile;
    TDesC buttonDownIconFile;
    TInt  customCmdId;
};

static toolbarButtonData KDefaultToolbarButtons [ ] = {
        { EPuttyToolbarTab, _L("Tab"), _L("Tab"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0  },
        { EPuttyToolbarAltP, _L("Alt"), _L("Alt+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },      
        { EPuttyToolbarCtrlP, _L("Ctrl"), _L("Ctrl+..."), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L("") },
        { EPuttyToolbarLock, _L("Lock"), _L("Toggle toolbar lock"), EFalse, EFalse, EFalse, EFalse, _L("UnLock"), _L(""), _L(""), 0 },
        { EPuttyToolbarSelect, _L("Select"), _L("Select"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarCopy, _L("Copy"), _L("Copy"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPaste, _L("Paste"), _L("Paste"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPipe, _L("Pipe"), _L("Pipe |"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowUp, _L("Up"), _L("Arrow up"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowDown, _L("Down"), _L("Arrow down"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowLeft, _L("Left"), _L("Arrow left"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarArrowRight, _L("Right"), _L("Arrow right"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEsc, _L("Esc"), _L("Esc"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPageUp, _L("Page Up"), _L("Page up"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarPageDown, _L("Page Down"), _L("Page down"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarHome, _L("Home"), _L("Home"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEnd, _L("End"), _L("End"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarDelete, _L("Delete"), _L("Delete"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarInsert, _L("Insert"), _L("Insert"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 },
        { EPuttyToolbarEnter, _L("Enter"), _L("Enter"), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 } ,
        { EPuttyToolbarNoTool, _L("End of button list"), _L(""), EFalse, EFalse, EFalse, EFalse, _L(""), _L(""), _L(""), 0 }
};
*/


// CLASS DECLARATION
class CCustomToolBar : public CCoeControl {
    public:
        ~CCustomToolBar ( );
        static CCustomToolBar* NewL (CTerminalContainer* aParent, TRect aRect, TTouchSettings *aTouchSettings);
        static CCustomToolBar* NewLC (CTerminalContainer* aParent, TRect aRect, TTouchSettings *aTouchSettings);
        void SetLeft();
        void SetTop();
        TOrientation GetOrientation();

    private:
        void Draw(const TRect& aRect) const;
        void SizeChanged();
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        void Move(const TPoint& aPoint);
        TBool IsDragDone(const TPointerEvent& aPointerEvent);
        void ShowUnpressedSizeButtonL();
        static TInt SizeButtonUp(TAny* aAny);
        
    public:
        inline TPuttyToolbarCommands ActiveTool(){return iTool;};
        void DeActiveTool(TPuttyToolbarCommands aTool); // EPuttyToolbarNoTool deactivates all
        void ActivateTool(TPuttyToolbarCommands aTool); // Activates tool
        inline void SetHidden(TBool aHidden) {iHidden = aHidden;};
        inline TBool GetHidden() { return iHidden; };
        inline RDrawableWindow* ParentDrawableWindow(){return iParent->DrawableWindow();};
        void UpdateButtonsFromSettingsL();
        void SwapButtons(TInt aButton, TInt aCommand);
        
    private:
        CCustomToolBar (CTerminalContainer* aParent);
        void ConstructL (CTerminalContainer* aParent, TRect aRect, TTouchSettings *aTouchSettings);
        void SetDefaultButtonsFromSettings();
        
        void LoadButtonsL();
        void UnloadButtonsL();
        void LoadIconL(const TDesC& aIconFile, TInt aIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, TSize aSize);
        void LoadIconL(const TDesC& aIconFile, TInt aIndex, CFbsBitmap*& aBitmap, TSize aSize);
        void HandleButtonSelection(TInt aButtonPos);
        void UpdateButtonsSizeAndCount(TInt aWidth, TInt aHeigth, TInt aCount);
        TBool TestForSecondRow(TOrientation aToolbarOrientation);
        
        void ToolbarPortrait1rows();
        void ToolbarPortrait2rows();
        void ToolbarLandscape1rows();
        void ToolbarLandscape2rows();

        TInt CountDefaultButtons();
        TInt CalculateBestFontSize();

        CCustomToolbarButton *GetButton(TInt aPlace) const { return iButtonsArray.operator [](iToolbarButtons[aPlace]); };
        toolbarButtonData GetButtonData(TInt aPlace) const { return (iButtonsArray.operator [](iToolbarButtons[aPlace]))->GetButtonData(); };
        
    private:
        TPuttyToolbarCommands   iTool;
        TRect                   iLastButtonDown;
        TOrientation            iMode;
        TBool                   iHidden;
        CTerminalContainer*     iParent;
        TPoint                  iMovePoint;
        TPoint                  iDragStartPoint;

        TBool                   iDrag;
        TBool                   iLocked;
        
        TTouchSettings          *iTouchSettings;
        
        MTouchFeedback*         iTouchFeedBack;

        // Toolbar button width
        TInt iToolbarItemWidth;

        // Toolbar button height
        TInt iToolbarItemHeight;

        // Amount of buttons in the toolbar
        TInt iToolbarItemCount;

        // Toolbar width in when toolbar is in portrait mode
        TInt iPortraitToolbarWidth;

        // Toolbar height in when toolbar is in landscape mode (1 row of buttons)
        TInt iLandscapeToolbarHeight;

        // Toolbar height in when toolbar is in landscape mode (2 rows of buttons)
        TInt iLandscapeToolbarHeight2Rows;

        // Toolbar heigth in when toolbar is in portrait mode (2 rows of buttons)
        TInt iPortraitToolbarWidth2Rows;
        
        
        //Testing
        TInt iPortraitToolbarHeigth2Rows;
        TInt iPortraitToolbarHeigth;
        TInt iLandscapeToolbarWidth;
        TInt iLandscapeToolbarWidth2Rows;
        
        TOrientation iToolbarOrientation;
        
        TInt iDefaultButtonsCount;
        RPointerArray<CCustomToolbarButton> iButtonsArray;
        TInt iToolbarButtons[KToolbarMaxItemCount];
};


#endif
