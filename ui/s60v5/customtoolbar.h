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
};

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
        void UpdateButtonsFromSettings();
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


    private:
        TPuttyToolbarCommands   iTool;
        TRect                   iLastButtonDown;
        TOrientation            iMode;
        TBool                   iHidden;
        CTerminalContainer*     iParent;
        TPoint                  iMovePoint;
        TPoint                  iDragStartPoint;

        tbButton                *iButtons[KToolbarMaxItemCount]; //pointters to the buttons that are shown       
        tbButton                *iButtonList[KToolbarButtonCount]; //Amount of buttons that are available to add to toolbar
        
        //Buttons that are supported
        tbButton                iTab;
        tbButton                iAltPlus;
        tbButton                iCtrlPlus;
        tbButton                iTBLock;
        tbButton                iSelect;
        tbButton                iCopy;
        tbButton                iPaste;
        tbButton                iPipe;
        tbButton                iUp;
        tbButton                iDown;
        tbButton                iLeft;
        tbButton                iRight;
        tbButton                iEsc;
        tbButton                iPageUp;
        tbButton                iPageDown;
        tbButton                iHome;
        tbButton                iEnd;
        tbButton                iDelete;
        tbButton                iInsert;
        tbButton                iEnter;
        
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
};


#endif
