/*    customtoolbar.cpp
 *
 * Putty custom toolbar class for touch UI
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#include <e32std.h>
#include <e32cmn.h>
#include <eikenv.h>
#include <eikspane.h>
#include <putty.rsg>
#include <putty.mbg>
#include <akniconutils.h>
#include <akndef.h>
#include <aknscreenmode.h>
#include <fbs.h>
#include "customtoolbar.h"
#include "terminalview.h"

_LIT(KSvgFile, "\\resource\\apps\\putty.mif");

CCustomToolBar::CCustomToolBar (CTerminalContainer* aParent) : iParent(aParent) {
    // No implementation required
}

CCustomToolBar::~CCustomToolBar ( ) {
    UnloadButtonsL(); //Delete button icons
}

void CCustomToolBar::UnloadButtonsL() {
    delete iTab.iButtonDownBitmap;
    delete iTab.iButtonDownBitmapMask;
    delete iTab.iButtonUpBitmap;
    delete iTab.iButtonUpBitmapMask;
    
    delete iAltPlus.iButtonDownBitmap;
    delete iAltPlus.iButtonDownBitmapMask;
    delete iAltPlus.iButtonUpBitmap;
    delete iAltPlus.iButtonUpBitmapMask;

    delete iCtrlPlus.iButtonDownBitmap;
    delete iCtrlPlus.iButtonDownBitmapMask;
    delete iCtrlPlus.iButtonUpBitmap;
    delete iCtrlPlus.iButtonUpBitmapMask;

    delete iTBLock.iButtonDownBitmap;
    delete iTBLock.iButtonDownBitmapMask;
    delete iTBLock.iButtonUpBitmap;
    delete iTBLock.iButtonUpBitmapMask;

    delete iSelect.iButtonDownBitmap;
    delete iSelect.iButtonDownBitmapMask;
    delete iSelect.iButtonUpBitmap;
    delete iSelect.iButtonUpBitmapMask;

    delete iCopy.iButtonDownBitmap;
    delete iCopy.iButtonDownBitmapMask;
    delete iCopy.iButtonUpBitmap;
    delete iCopy.iButtonUpBitmapMask;

    delete iPaste.iButtonDownBitmap;
    delete iPaste.iButtonDownBitmapMask;
    delete iPaste.iButtonUpBitmap;
    delete iPaste.iButtonUpBitmapMask;

    delete iPipe.iButtonDownBitmap;
    delete iPipe.iButtonDownBitmapMask;
    delete iPipe.iButtonUpBitmap;
    delete iPipe.iButtonUpBitmapMask;

    delete iUp.iButtonDownBitmap;
    delete iUp.iButtonDownBitmapMask;
    delete iUp.iButtonUpBitmap;
    delete iUp.iButtonUpBitmapMask;

    delete iDown.iButtonDownBitmap;
    delete iDown.iButtonDownBitmapMask;
    delete iDown.iButtonUpBitmap;
    delete iDown.iButtonUpBitmapMask;

    delete iLeft.iButtonDownBitmap;
    delete iLeft.iButtonDownBitmapMask;
    delete iLeft.iButtonUpBitmap;
    delete iLeft.iButtonUpBitmapMask;    

    delete iRight.iButtonDownBitmap;
    delete iRight.iButtonDownBitmapMask;
    delete iRight.iButtonUpBitmap;
    delete iRight.iButtonUpBitmapMask;    

    delete iEsc.iButtonDownBitmap;
    delete iEsc.iButtonDownBitmapMask;
    delete iEsc.iButtonUpBitmap;
    delete iEsc.iButtonUpBitmapMask;    

    delete iPageUp.iButtonDownBitmap;
    delete iPageUp.iButtonDownBitmapMask;
    delete iPageUp.iButtonUpBitmap;
    delete iPageUp.iButtonUpBitmapMask;  
    
    delete iPageDown.iButtonDownBitmap;
    delete iPageDown.iButtonDownBitmapMask;
    delete iPageDown.iButtonUpBitmap;
    delete iPageDown.iButtonUpBitmapMask;
    
    delete iHome.iButtonDownBitmap;
    delete iHome.iButtonDownBitmapMask;
    delete iHome.iButtonUpBitmap;
    delete iHome.iButtonUpBitmapMask;
    
    delete iEnd.iButtonDownBitmap;
    delete iEnd.iButtonDownBitmapMask;
    delete iEnd.iButtonUpBitmap;
    delete iEnd.iButtonUpBitmapMask;
    
    delete iDelete.iButtonDownBitmap;
    delete iDelete.iButtonDownBitmapMask;
    delete iDelete.iButtonUpBitmap;
    delete iDelete.iButtonUpBitmapMask;
    
    delete iInsert.iButtonDownBitmap;
    delete iInsert.iButtonDownBitmapMask;
    delete iInsert.iButtonUpBitmap;
    delete iInsert.iButtonUpBitmapMask;    

    delete iEnter.iButtonDownBitmap;
    delete iEnter.iButtonDownBitmapMask;
    delete iEnter.iButtonUpBitmap;
    delete iEnter.iButtonUpBitmapMask;
}

CCustomToolBar* CCustomToolBar::NewLC (CTerminalContainer* aParent, TRect aRect, TTouchSettings *aTouchSettings) {
    CCustomToolBar* self = new (ELeave)CCustomToolBar(aParent);
    CleanupStack::PushL (self );
    self->ConstructL (aParent,aRect,aTouchSettings);
    return self;
}

CCustomToolBar* CCustomToolBar::NewL (CTerminalContainer* aParent, TRect aRect, TTouchSettings *aTouchSettings) {
    CCustomToolBar* self=CCustomToolBar::NewLC (aParent,aRect,aTouchSettings);
    CleanupStack::Pop ( ); // self;
    return self;
}

void CCustomToolBar::ConstructL (CTerminalContainer* aParent, TRect /*aRect*/, TTouchSettings *aTouchSettings) {
    iTouchSettings = aTouchSettings;
    //UpdateButtonsSizeAndCount(iTouchSettings->GetTbButtonWidth(),iTouchSettings->GetTbButtonHeigth(),iTouchSettings->GetTbButtonCount());
    LoadButtonsL(); //Create button lists
    iLocked = ETrue; // locks toolbar to it's place
    if (iTouchSettings->GetShowToolbar() == 1) {
        iHidden = EFalse; // Toolbar is shown on startup
    } else {
        iHidden = ETrue; // Toolbar is hidden on startup
    }

    iTouchFeedBack = MTouchFeedback::Instance();
    iTouchFeedBack->SetFeedbackEnabledForThisApp(ETrue);
    iToolbarOrientation = ELandscape;
    SetContainerWindowL(static_cast<CCoeControl&>(*aParent));
}

void CCustomToolBar::Draw( const TRect& /*aRect*/ ) const {

    if (iHidden) {
      return; // Lets exit if toolbar is hidden
    }
        
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    TBool invertMask = EFalse;
   
    // Background of the toolbar -----------------------------------
    TRect toolbarRect = TRect(TPoint(0,0),Size());

    // All buttons---------------------------------------------------
    TRect buttonRect = TRect(TPoint(0,0),TSize(iToolbarItemWidth,iToolbarItemHeight));
    
    for ( int i = 0; i < iToolbarItemCount; i++) {
        if ( iButtons[i]->iDown ) {
            gc.BitBltMasked(iButtons[i]->iItem.iTl, iButtons[i]->iButtonDownBitmap, buttonRect, iButtons[i]->iButtonDownBitmapMask, invertMask);
        } else {
            gc.BitBltMasked(iButtons[i]->iItem.iTl, iButtons[i]->iButtonUpBitmap, buttonRect, iButtons[i]->iButtonUpBitmapMask, invertMask);
        }
    }
    
}

void CCustomToolBar::SizeChanged() {
    // Calculate new item positions
    TRect item = Rect();
        
    // When terminal is in portrait mode
   
   if ( GetOrientation() == EPortrait ) {
        if ( iToolbarOrientation == EPortrait ) {       
            // Toolbar is in portrait mode
           if ( TestForSecondRow( ETestOnly ) ) {
               ToolbarPortrait2rows();
           } else {               
               ToolbarPortrait1rows();
           }
           
        } else {
           //Toolbar is in landscape mode
           if ( TestForSecondRow( ETestOnly ) ) {
               ToolbarLandscape2rows();
           } else {
               ToolbarLandscape1rows(); 
           }
        }
    } else {
        // When terminal is in landscape mode
        if ( iToolbarOrientation == EPortrait ) {
            // Toolbar is in portrait mode
            if ( TestForSecondRow( ETestOnly ) ) {
                ToolbarPortrait2rows();
            } else {
                ToolbarPortrait1rows();
            }
            
        } else {
            // Toolbar is in landscape mode
            if ( TestForSecondRow( ETestOnly ) ) {
                ToolbarLandscape2rows();
            } else {
                ToolbarLandscape1rows();
            }
        }
    }
}

void CCustomToolBar::Move(const TPoint& aPoint) {
    // User is moving/dragging this toolbar
    TPoint offset;
    offset.iX =  aPoint.iX - iMovePoint.iX;
    offset.iY = aPoint.iY - iMovePoint.iY;
    
    TRect rect = Rect();
    rect.Move(offset);
    
    if (rect.iTl.iX >= KToolbarGap && rect.iTl.iY >= KToolbarGap && rect.iBr.iY <= iParent->Size().iHeight - KToolbarGap && rect.iBr.iX <= iParent->Size().iWidth - KToolbarGap) {
        // Move
        SetRect(rect);
    } else {
        if (rect.iTl.iY < KToolbarGap) {
            SetTop();
        } else if (rect.iTl.iX < KToolbarGap) {
            SetLeft();
        }
    }
   
}

TBool CCustomToolBar::IsDragDone(const TPointerEvent& aPointerEvent) {
    // We accept dragging when user has done at least KDraggingAccepted pixel drag
    TBool ret = EFalse;
    
    TInt x =  Abs(aPointerEvent.iPosition.iX - iDragStartPoint.iX);
    TInt y = Abs(aPointerEvent.iPosition.iY - iDragStartPoint.iY);

    if (x > KDraggingAccepted || y > KDraggingAccepted) {
        ret  = ETrue;
    }
    
    return ret;
}

void CCustomToolBar::HandlePointerEventL(const TPointerEvent& aPointerEvent) {

    if (iHidden) {
       return; // Lets exit if hidden
    }
        
    if (aPointerEvent.iType == TPointerEvent::EButton1Down) {
        iDrag = EFalse;
        iMovePoint = aPointerEvent.iPosition;
        iDragStartPoint = aPointerEvent.iPosition;
        
        //Draw button down if not selectable tool. Do we need this after code clean up?
        for ( int i = 0; i < iToolbarItemCount; i++) {
            if ( iButtons[i]->iItem.Contains(aPointerEvent.iPosition) ) {
                if ( iButtons[i]->iSelectable ) {
                    iTool = iButtons[i]->iAction;
                    iButtons[i]->iDown = ETrue; // Set button down when clicked down.
                    iLastButtonDown = iButtons[i]->iItem;
                }
                iTouchFeedBack->InstantFeedback(ETouchFeedbackBasic); // touch feedback to all buttons down
            }
        }
        DrawDeferred();
    } else if (aPointerEvent.iType == TPointerEvent::EDrag) {
        if (!iLocked) {
            // Do we accept dragging?
            if (!iDrag && IsDragDone(aPointerEvent)) {
                iDrag = ETrue;
                
                if ( iTool == EPuttyToolbarSelect ) {
                    for (int i = 0 ; i < iToolbarItemCount ; i++) {
                        if ( iButtons[i]->iAction == EPuttyToolbarSelect) {
                            iButtons[i]->iDown = EFalse;
                            break;
                        }
                    }
                    iParent->HandleCustomToolbar(EPuttyToolbarSelect); //inform parent that we are moving the toolbar and canceling select
                }
                iTool = EPuttyToolbarNoTool;
                DrawDeferred();
            }

            // Dragging accepted in IsDragDone()
            if (iDrag) {
                TRect acceptedArea = Rect();
                // Grow a little area of where dragging is accepted
                acceptedArea.Grow(KDraggingAcceptGrow,KDraggingAcceptGrow);
                if (acceptedArea.Contains(aPointerEvent.iPosition)) {
                    Move(aPointerEvent.iPosition);
                    iMovePoint = aPointerEvent.iPosition;
                }
            }
        } else {
            //Drag is detected even after 1 pixel move so lets see if the user stays inside the button field
            
            if (!iDrag && IsDragDone(aPointerEvent) && !iLastButtonDown.Contains(aPointerEvent.iPosition) ) {
                if (iTool != EPuttyToolbarSelect) {    
                    iTool = EPuttyToolbarNoTool;
                    for (int i = 0 ; i < iToolbarItemCount; i++) {
                        iButtons[i]->iDown = EFalse; // put all buttons up
                    }
                }
            }
        }
    } else if (aPointerEvent.iType == TPointerEvent::EButton1Up) {
        if (!iDrag) {
            // Handle tool selection
            for (TInt i = 0; i < iToolbarItemCount; i++) {
                if ( iButtons[i]->iItem.Contains(aPointerEvent.iPosition) ) {
                    HandleButtonSelection(i);
                    break;
                }
            }
        }
        iDrag = EFalse;
    }
}

void CCustomToolBar::DeActiveTool(TPuttyToolbarCommands aTool) {
    if ( aTool == EPuttyToolbarNoTool ) {
        for ( int i = 0 ; i < iToolbarItemCount; i++) {
            if ( iButtons[i]->iAction == iTool && iButtons[i]->iAction != EPuttyToolbarLock) {
                iButtons[i]->iDown = EFalse;
            }
        }
        iTool = aTool;
        return;
    } else {
        for ( int i = 0; i < iToolbarItemCount; i++) {
            if ( iButtons[i]->iAction == aTool && iButtons[i]->iAction != EPuttyToolbarLock ) {
                iButtons[i]->iDown = EFalse;
                if ( iTool == aTool ) {
                    iTool = EPuttyToolbarNoTool;
                }
                break;
            }
        }
    
    }
    //iTool = aTool;
}

void CCustomToolBar::ActivateTool(TPuttyToolbarCommands aTool) {
     for ( int i = 0; i < iToolbarItemCount; i++) {
            if ( iButtons[i]->iAction == aTool && iButtons[i]->iAction != EPuttyToolbarLock ) {
                iButtons[i]->iDown = ETrue;
                iTool = aTool;
                break;
            }
        }    
}

//Handles real button press
void CCustomToolBar::HandleButtonSelection(TInt aButtonPos) {
    
    switch (iButtons[aButtonPos]->iAction) {
        case EPuttyToolbarTab:
            iParent->HandleCustomToolbar(EPuttyToolbarTab);
            iTool = EPuttyToolbarNoTool;
            iButtons[aButtonPos]->iDown = EFalse;
            break;
        case EPuttyToolbarAltP:
            iTool = EPuttyToolbarAltP;
            iButtons[aButtonPos]->iDown = ETrue;
            iParent->HandleCustomToolbar(EPuttyToolbarAltP);
            break;
        case EPuttyToolbarCtrlP:
            iTool = EPuttyToolbarCtrlP;
            iButtons[aButtonPos]->iDown = ETrue;
            iParent->HandleCustomToolbar(EPuttyToolbarCtrlP);
            break;
        case EPuttyToolbarLock:
            if (iLocked) {
              iLocked = EFalse;
              iButtons[aButtonPos]->iDown = ETrue;
            } else {
              iLocked = ETrue;
              iButtons[aButtonPos]->iDown = EFalse;
            }
            break;
        case EPuttyToolbarSelect:
            iTool = EPuttyToolbarSelect;
            iButtons[aButtonPos]->iDown = ETrue;
            iParent->HandleCustomToolbar(EPuttyToolbarSelect);
            break;
        default:
        /*
        case EPuttyToolbarPipe:
        case EPuttyToolbarArrowUp:
        case EPuttyToolbarArrowDown:
        case EPuttyToolbarArrowLeft:
        case EPuttyToolbarArrowRight:
        case EPuttyToolbarEsc:
        case EPuttyToolbarPageUp:
        case EPuttyToolbarPageDown:
        case EPuttyToolbarHome:
        case EPuttyToolbarEnd:
        case EPuttyToolbarDelete:
        case EPuttyToolbarInsert:
        case EPuttyToolbarEnter:
        */
            iParent->HandleCustomToolbar(iButtons[aButtonPos]->iAction);
            iTool = EPuttyToolbarNoTool;
            iButtons[aButtonPos]->iDown = EFalse;
            break;
    }
}

void CCustomToolBar::LoadButtonsL() {
    UpdateButtonsSizeAndCount(iTouchSettings->GetTbButtonWidth(),iTouchSettings->GetTbButtonHeigth(),iTouchSettings->GetTbButtonCount());
    
    //Tab
    LoadIconL(KSvgFile, EMbmPuttyTab_button_down_48x48, iTab.iButtonDownBitmap,iTab.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyTab_button_up_48x48, iTab.iButtonUpBitmap,iTab.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iTab.iAction = EPuttyToolbarTab;
    iTab.iDown = EFalse;
    iTab.iSelectable = ETrue;
    //Alt+
    LoadIconL(KSvgFile, EMbmPuttyAltp_button_down_48x48, iAltPlus.iButtonDownBitmap, iAltPlus.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyAltp_button_up_48x48, iAltPlus.iButtonUpBitmap,iAltPlus.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iAltPlus.iAction = EPuttyToolbarAltP;
    iAltPlus.iDown = EFalse;
    iAltPlus.iSelectable = EFalse;
    //Ctrl+
    LoadIconL(KSvgFile, EMbmPuttyCtrlp_button_down_48x48, iCtrlPlus.iButtonDownBitmap,iCtrlPlus.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyCtrlp_button_up_48x48, iCtrlPlus.iButtonUpBitmap,iCtrlPlus.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iCtrlPlus.iAction = EPuttyToolbarCtrlP;
    iCtrlPlus.iDown = EFalse;
    iCtrlPlus.iSelectable = EFalse;
    //Toolbar lock
    LoadIconL(KSvgFile, EMbmPuttyLock_button_down_48x48, iTBLock.iButtonDownBitmap, iTBLock.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyLock_button_up_48x48, iTBLock.iButtonUpBitmap,iTBLock.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));        
    iTBLock.iAction = EPuttyToolbarLock;
    iTBLock.iDown = EFalse;
    iTBLock.iSelectable = EFalse;
    //Select
    LoadIconL(KSvgFile, EMbmPuttySelect_button_down_48x48, iSelect.iButtonDownBitmap,iSelect.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttySelect_button_up_48x48, iSelect.iButtonUpBitmap,iSelect.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iSelect.iAction = EPuttyToolbarSelect;
    iSelect.iDown = EFalse;
    iSelect.iSelectable = EFalse;
    //Copy
    LoadIconL(KSvgFile, EMbmPuttyCopy_button_down_48x48, iCopy.iButtonDownBitmap,iCopy.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyCopy_button_up_48x48, iCopy.iButtonUpBitmap,iCopy.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iCopy.iAction = EPuttyToolbarCopy;
    iCopy.iDown = EFalse;
    iCopy.iSelectable = ETrue;
    //Paste
    LoadIconL(KSvgFile, EMbmPuttyPaste_button_down_48x48, iPaste.iButtonDownBitmap,iPaste.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyPaste_button_up_48x48, iPaste.iButtonUpBitmap,iPaste.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iPaste.iAction = EPuttyToolbarPaste;
    iPaste.iDown = EFalse;
    iPaste.iSelectable = ETrue;
    //Pipe
    LoadIconL(KSvgFile, EMbmPuttyPipe_button_down_48x48, iPipe.iButtonDownBitmap,iPipe.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyPipe_button_up_48x48, iPipe.iButtonUpBitmap,iPipe.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));
    iPipe.iAction = EPuttyToolbarPipe;
    iPipe.iDown = EFalse;
    iPipe.iSelectable = ETrue;
    //Arrow Up
    LoadIconL(KSvgFile, EMbmPuttyUp_button_down_48x48, iUp.iButtonDownBitmap,iUp.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyUp_button_up_48x48, iUp.iButtonUpBitmap,iUp.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iUp.iAction = EPuttyToolbarArrowUp;
    iUp.iDown = EFalse;
    iUp.iSelectable = ETrue;
    //Arrow Down
    LoadIconL(KSvgFile, EMbmPuttyDown_button_down_48x48, iDown.iButtonDownBitmap, iDown.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyDown_button_up_48x48, iDown.iButtonUpBitmap,iDown.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iDown.iAction = EPuttyToolbarArrowDown;
    iDown.iDown = EFalse;
    iDown.iSelectable = ETrue;
    //Arrow Left
    LoadIconL(KSvgFile, EMbmPuttyLeft_button_down_48x48, iLeft.iButtonDownBitmap, iLeft.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyLeft_button_up_48x48, iLeft.iButtonUpBitmap,iLeft.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iLeft.iAction = EPuttyToolbarArrowLeft;
    iLeft.iDown = EFalse;
    iLeft.iSelectable = ETrue;
    //Arrow Right
    LoadIconL(KSvgFile, EMbmPuttyRight_button_down_48x48, iRight.iButtonDownBitmap, iRight.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyRight_button_up_48x48, iRight.iButtonUpBitmap,iRight.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iRight.iAction = EPuttyToolbarArrowRight;
    iRight.iDown = EFalse;
    iRight.iSelectable = ETrue;
    //Esc
    LoadIconL(KSvgFile, EMbmPuttyEsc_button_down_48x48, iEsc.iButtonDownBitmap, iEsc.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyEsc_button_up_48x48, iEsc.iButtonUpBitmap,iEsc.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iEsc.iAction = EPuttyToolbarEsc;
    iEsc.iDown = EFalse;
    iEsc.iSelectable = ETrue;
    //Page up
    LoadIconL(KSvgFile, EMbmPuttyPageup_button_down_48x48, iPageUp.iButtonDownBitmap, iPageUp.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyPageup_button_up_48x48, iPageUp.iButtonUpBitmap,iPageUp.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iPageUp.iAction = EPuttyToolbarPageUp;
    iPageUp.iDown = EFalse;
    iPageUp.iSelectable = ETrue;
    //Page down
    LoadIconL(KSvgFile, EMbmPuttyPagedown_button_down_48x48, iPageDown.iButtonDownBitmap, iPageDown.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyPagedown_button_up_48x48, iPageDown.iButtonUpBitmap,iPageDown.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iPageDown.iAction = EPuttyToolbarPageDown;
    iPageDown.iDown = EFalse;
    iPageDown.iSelectable = ETrue;
    //Home
    LoadIconL(KSvgFile, EMbmPuttyHome_button_down_48x48, iHome.iButtonDownBitmap, iHome.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyHome_button_up_48x48, iHome.iButtonUpBitmap,iHome.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iHome.iAction = EPuttyToolbarHome;
    iHome.iDown = EFalse;
    iHome.iSelectable = ETrue;
    //End
    LoadIconL(KSvgFile, EMbmPuttyEnd_button_down_48x48, iEnd.iButtonDownBitmap, iEnd.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyEnd_button_up_48x48, iEnd.iButtonUpBitmap,iEnd.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iEnd.iAction = EPuttyToolbarEnd;
    iEnd.iDown = EFalse;
    iEnd.iSelectable = ETrue;
    //Delete
    LoadIconL(KSvgFile, EMbmPuttyDelete_button_down_48x48, iDelete.iButtonDownBitmap, iDelete.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyDelete_button_up_48x48, iDelete.iButtonUpBitmap,iDelete.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iDelete.iAction = EPuttyToolbarDelete;
    iDelete.iDown = EFalse;
    iDelete.iSelectable = ETrue;
    //Insert
    LoadIconL(KSvgFile, EMbmPuttyInsert_button_down_48x48, iInsert.iButtonDownBitmap, iInsert.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyInsert_button_up_48x48, iInsert.iButtonUpBitmap,iInsert.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iInsert.iAction = EPuttyToolbarInsert;
    iInsert.iDown = EFalse;
    iInsert.iSelectable = ETrue;

    //Enter
    LoadIconL(KSvgFile, EMbmPuttyEnter_button_down_48x48, iEnter.iButtonDownBitmap, iEnter.iButtonDownBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    LoadIconL(KSvgFile, EMbmPuttyEnter_button_up_48x48, iEnter.iButtonUpBitmap,iEnter.iButtonUpBitmapMask, TSize(iToolbarItemWidth,iToolbarItemHeight));    
    iEnter.iAction = EPuttyToolbarEnter;
    iEnter.iDown = EFalse;
    iEnter.iSelectable = ETrue;
    
    //Create list of available buttons
    iButtonList[EPuttyToolbarTab] = &iTab;
    iButtonList[EPuttyToolbarAltP] = &iAltPlus;
    iButtonList[EPuttyToolbarCtrlP] = &iCtrlPlus;
    iButtonList[EPuttyToolbarLock] = &iTBLock;
    iButtonList[EPuttyToolbarSelect] = &iSelect;
    iButtonList[EPuttyToolbarCopy] = &iCopy;
    iButtonList[EPuttyToolbarPaste] = &iPaste;
    iButtonList[EPuttyToolbarPipe] = &iPipe;
    iButtonList[EPuttyToolbarArrowUp] = &iUp;
    iButtonList[EPuttyToolbarArrowDown] = &iDown;
    iButtonList[EPuttyToolbarArrowLeft] = &iLeft;
    iButtonList[EPuttyToolbarArrowRight] = &iRight;
    iButtonList[EPuttyToolbarEsc] = &iEsc;
    iButtonList[EPuttyToolbarPageUp] = &iPageUp;
    iButtonList[EPuttyToolbarPageDown] = &iPageDown;
    iButtonList[EPuttyToolbarHome] = &iHome;
    iButtonList[EPuttyToolbarEnd] = &iEnd;
    iButtonList[EPuttyToolbarDelete] = &iDelete;
    iButtonList[EPuttyToolbarInsert] = &iInsert;
    iButtonList[EPuttyToolbarEnter] = &iEnter;
    

    //Set Default buttons to toolbar
    /*
    iButtons[0] = iButtonList[0];
    iButtons[1] = iButtonList[1];
    iButtons[2] = iButtonList[2];
    iButtons[3] = iButtonList[3];
    iButtons[4] = iButtonList[4];
    iButtons[5] = iButtonList[5];
    iButtons[6] = iButtonList[6];
    iButtons[7] = iButtonList[7];
    */
    SetDefaultButtonsFromSettings(); // Set initial values
}

void CCustomToolBar::SetDefaultButtonsFromSettings() {
    iButtons[0] = iButtonList[iTouchSettings->GetTbButton1()];
    iButtons[1] = iButtonList[iTouchSettings->GetTbButton2()];
    iButtons[2] = iButtonList[iTouchSettings->GetTbButton3()];
    iButtons[3] = iButtonList[iTouchSettings->GetTbButton4()];
    iButtons[4] = iButtonList[iTouchSettings->GetTbButton5()];
    iButtons[5] = iButtonList[iTouchSettings->GetTbButton6()];
    iButtons[6] = iButtonList[iTouchSettings->GetTbButton7()];
    iButtons[7] = iButtonList[iTouchSettings->GetTbButton8()];   
    //UpdateButtonsSizeAndCount(iTouchSettings->GetTbButtonWidth(),iTouchSettings->GetTbButtonHeigth(),iTouchSettings->GetTbButtonCount());
}

void CCustomToolBar::SwapButtons(TInt aButton, TInt aCommand) {
    TRect tmpItem = iButtons[aButton]->iItem;
    tbButton *tmpButton = iButtons[aButton];
    
    for ( int i = 0 ; i < iToolbarItemCount; i++) {
        if ( iButtons[i]->iAction == aCommand) {
            //Change button places on screen
            iButtons[aButton]->iItem = iButtons[i]->iItem;
            iButtons[i]->iItem = tmpItem;
            //Change button places on list
            iButtons[aButton] = iButtons[i]; // Swaps button places
            iButtons[i] = tmpButton;
            return;
        }
    }
}

void CCustomToolBar::UpdateButtonsFromSettings() {
    //Set buttons from settings
    
    if (iButtons[0]->iAction != iButtonList[iTouchSettings->GetTbButton1()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton1()]->iItem = iButtons[0]->iItem;
        iButtons[0] = iButtonList[iTouchSettings->GetTbButton1()];
    }

    if (iButtons[1]->iAction != iButtonList[iTouchSettings->GetTbButton2()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton2()]->iItem = iButtons[1]->iItem;
        iButtons[1] = iButtonList[iTouchSettings->GetTbButton2()];
    }

    if (iButtons[2]->iAction != iButtonList[iTouchSettings->GetTbButton3()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton3()]->iItem = iButtons[2]->iItem;
        iButtons[2] = iButtonList[iTouchSettings->GetTbButton3()];
    }

    if (iButtons[3]->iAction != iButtonList[iTouchSettings->GetTbButton4()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton4()]->iItem = iButtons[3]->iItem;
        iButtons[3] = iButtonList[iTouchSettings->GetTbButton4()];
    }

    if (iButtons[4]->iAction != iButtonList[iTouchSettings->GetTbButton5()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton5()]->iItem = iButtons[4]->iItem;
        iButtons[4] = iButtonList[iTouchSettings->GetTbButton5()];
    }

    if (iButtons[5]->iAction != iButtonList[iTouchSettings->GetTbButton6()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton6()]->iItem = iButtons[5]->iItem;
        iButtons[5] = iButtonList[iTouchSettings->GetTbButton6()];
    }

    if (iButtons[6]->iAction != iButtonList[iTouchSettings->GetTbButton7()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton7()]->iItem = iButtons[6]->iItem;
        iButtons[6] = iButtonList[iTouchSettings->GetTbButton7()];
    }

    if (iButtons[7]->iAction != iButtonList[iTouchSettings->GetTbButton8()]->iAction) {
        iButtonList[iTouchSettings->GetTbButton8()]->iItem = iButtons[7]->iItem;
        iButtons[7] = iButtonList[iTouchSettings->GetTbButton8()];
    }
    
    //UpdateButtonsSizeAndCount(iTouchSettings->GetTbButtonWidth(),iTouchSettings->GetTbButtonHeigth(),iTouchSettings->GetTbButtonCount());
}

void CCustomToolBar::LoadIconL(const TDesC& aIconFile, TInt aIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, TSize aSize) {
    AknIconUtils::CreateIconL(aBitmap, aMask, aIconFile, aIndex, aIndex + 1);
    AknIconUtils::SetSize(aBitmap, aSize, EAspectRatioPreservedSlice );
    AknIconUtils::SetSize(aMask, aSize, EAspectRatioPreservedSlice );
}

void CCustomToolBar::LoadIconL(const TDesC& aIconFile, TInt aIndex, CFbsBitmap*& aBitmap, TSize aSize) {
    aBitmap = AknIconUtils::CreateIconL(aIconFile, aIndex);
    AknIconUtils::SetSize(aBitmap, aSize, EAspectRatioPreservedSlice );
}

void CCustomToolBar::SetTop() {
    // Set toolbar location in screen
    TRect rect;
    iToolbarOrientation = ELandscape;
    if ( GetOrientation() == ELandscape )
        {
            // Set toolbar to TOP in terminal landscape mode
            if ( TestForSecondRow( ELandscape ) ) {
                TInt testWidth = (iParent->Size().iWidth - iLandscapeToolbarWidth2Rows)/2;
                if ( testWidth < 0 ) testWidth = KToolbarGap;
                rect = TRect(TPoint(testWidth,KToolbarGap), TSize(iLandscapeToolbarWidth2Rows,iLandscapeToolbarHeight2Rows));                
            } else {
                TInt testWidth = (iParent->Size().iWidth - iLandscapeToolbarWidth)/2;
                if ( testWidth < 0 ) testWidth = KToolbarGap;
                rect = TRect(TPoint(testWidth,KToolbarGap), TSize(iLandscapeToolbarWidth,iLandscapeToolbarHeight));
            }        
        }
    else
        {
            // Set toolbar to TOP in terminal portrait mode
            if ( TestForSecondRow( ELandscape ) ) {
                TInt testWidth = (iParent->Size().iWidth - iLandscapeToolbarWidth2Rows)/2;
                if ( testWidth < 0 ) testWidth = KToolbarGap;
                rect = TRect(TPoint(testWidth,KToolbarGap), TSize(iLandscapeToolbarWidth2Rows,iLandscapeToolbarHeight2Rows));           
            } else {
                TInt testWidth = (iParent->Size().iWidth - iLandscapeToolbarWidth)/2;
                if ( testWidth < 0 ) testWidth = KToolbarGap;
                rect = TRect(TPoint(testWidth,KToolbarGap), TSize(iLandscapeToolbarWidth,iLandscapeToolbarHeight));
            }        
        }
    iTouchFeedBack->InstantFeedback(ETouchFeedbackBasic); // touch feedback
    SetRect(rect);
}

void CCustomToolBar::SetLeft() {
    // Set toolbar location in screen
    TRect rect;
    iToolbarOrientation = EPortrait;
    if ( GetOrientation() == ELandscape ) {
        // Set toolbar to LEFT in terminal landscape mode
            
        if ( TestForSecondRow( EPortrait ) ) {
            //TInt count = iToolbarItemCount/2;
            TInt testHeigth = (iParent->Size().iHeight - iPortraitToolbarHeigth2Rows)/2;
            if ( testHeigth < 0 ) testHeigth = KToolbarGap;
            rect = TRect(TPoint(KToolbarGap,testHeigth), TSize(iPortraitToolbarWidth2Rows, iPortraitToolbarHeigth2Rows));
        } else {
            TInt testHeigth = (iParent->Size().iHeight - iPortraitToolbarHeigth)/2;
            if ( testHeigth < 0 ) testHeigth = KToolbarGap;
            rect = TRect(TPoint(KToolbarGap,testHeigth), TSize(iPortraitToolbarWidth,iPortraitToolbarHeigth));        
        }
    } else {
        // Set toolbar to LEFT in terminal portrait mode
        if ( TestForSecondRow( EPortrait ) ) {
            TInt testHeigth = (iParent->Size().iHeight - iPortraitToolbarHeigth2Rows)/2;
            if ( testHeigth < 0 ) testHeigth = KToolbarGap;
            rect = TRect(TPoint(KToolbarGap,testHeigth), TSize(iPortraitToolbarWidth2Rows,iPortraitToolbarHeigth2Rows));
        
        } else {
            TInt testHeigth = (iParent->Size().iHeight - iPortraitToolbarHeigth)/2;
            if ( testHeigth < 0 ) testHeigth = KToolbarGap;
            rect = TRect(TPoint(KToolbarGap,testHeigth), TSize(iPortraitToolbarWidth,iPortraitToolbarHeigth));
        } 
    }
    iTouchFeedBack->InstantFeedback(ETouchFeedbackBasic); // touch feedback
    SetRect(rect);
}

TOrientation CCustomToolBar::GetOrientation() {
    TRect test = CEikonEnv::Static()->EikAppUi()->ApplicationRect();
    //TInt widht = test.Size().iWidth;
    TInt heigth = test.Size().iHeight;
    if ( heigth == KPortraitHeight ) {
        return EPortrait;
    } else {
        return ELandscape;
    }  
}

TBool CCustomToolBar::TestForSecondRow(TOrientation aToolbarOrientation) {
    TRect item = iParent->Rect();
    
    if ( GetOrientation() == EPortrait ) {
        if ( aToolbarOrientation == ETestOnly ) {
            if ( iToolbarOrientation == EPortrait ) {  
                // Toolbar is in portrait mode
                if ( item.Height() >=  iPortraitToolbarHeigth ) {
                    //Height is enough
                    return EFalse;
                } else {
                    return ETrue;
                }
            } else {
                // Toolbar is in landscape mode
                if ( item.Width() >= iLandscapeToolbarWidth ) {
                    return EFalse;
                } else {
                    return ETrue;
                }
            }
        } else {
            if ( aToolbarOrientation == EPortrait ) {
                //We want to set toolbar to portrait mode so we must test for portrait
                if ( item.Height() >=  iPortraitToolbarHeigth ) {
                    //Height is enough
                    return EFalse;
                } else {
                    return ETrue;
                }
            } else {
                // set toolbar to landscape mode
                if ( item.Width() >= iLandscapeToolbarWidth ) {
                    return EFalse;
                } else {
                    return ETrue;
                }            
            }
        }
    } else {
        if ( aToolbarOrientation == ETestOnly ) {
            if ( iToolbarOrientation == EPortrait ) {  
                // Toolbar is in portrait mode
                if ( item.Height() >=  iPortraitToolbarHeigth ) {
                    return EFalse;
                } else {
                    return ETrue;
                }
            } else {
                // Toolbar is in landscape mode
                if ( item.Width() >= iLandscapeToolbarWidth ) {
                    return EFalse;
                } else {
                    return ETrue;
                }
            }
        } else {
            if ( aToolbarOrientation == EPortrait ) {
                //We want to set toolbar to portrait mode so we must test for portrait
                if ( item.Height() >=  iPortraitToolbarHeigth ) {
                    //heigth is enough
                    return EFalse;
                } else {
                   return ETrue;
                }           
            } else {
                //set toolbar to landscape mode
                if ( item.Width() >= iLandscapeToolbarWidth ) {
                    return EFalse;
                } else {
                    return ETrue;
                }                
            }        
        }
    }
}

void CCustomToolBar::UpdateButtonsSizeAndCount(TInt aWidth, TInt aHeigth, TInt aCount) {
    // Toolbar button width
    iToolbarItemWidth = aWidth;

    // Toolbar button height
    iToolbarItemHeight = aHeigth;

    // Amount of buttons in the toolbar
    iToolbarItemCount = aCount;

    TInt iToolbarHalfCount = iToolbarItemCount/2;
    
    //Toolbar portrait
    //width & heigth when toolbar is in one row
    iPortraitToolbarWidth = iToolbarItemWidth;
    if ( iToolbarItemCount > 2) {
        iPortraitToolbarHeigth = iToolbarItemHeight*iToolbarItemCount + ((iToolbarItemCount-2) * KToolbarItemGap);
    } else if ( iToolbarItemCount == 2 ){
        iPortraitToolbarHeigth = iToolbarItemHeight*iToolbarItemCount + KToolbarItemGap;
    } else {
        iPortraitToolbarHeigth = iToolbarItemHeight;
    }
    
    //two rows
    if ( (iToolbarItemCount % 2 ) != 0 ) {
        TInt tmpRound = iToolbarItemCount +1;
        TInt tmpRoundHalf = tmpRound/2;
        iPortraitToolbarWidth2Rows = iToolbarItemWidth*2 + KToolbarItemGap;
        if ( tmpRoundHalf > 2) {
            iPortraitToolbarHeigth2Rows = iToolbarItemHeight*tmpRoundHalf + ((tmpRoundHalf-2) * KToolbarItemGap);
        } else {
            iPortraitToolbarHeigth2Rows = iToolbarItemHeight*tmpRoundHalf;
        }        
    } else {    
        iPortraitToolbarWidth2Rows = iToolbarItemWidth*2 + KToolbarItemGap;
        if ( iToolbarHalfCount > 2) {
            iPortraitToolbarHeigth2Rows = iToolbarItemHeight*iToolbarHalfCount + ((iToolbarHalfCount-2) * KToolbarItemGap);
        } else {
            iPortraitToolbarHeigth2Rows = iToolbarItemHeight*iToolbarHalfCount;
        }
    }
    
    //Toolbar landscape
    //width & heigth when toolbar is in one row
    iLandscapeToolbarHeight = iToolbarItemHeight;
    
    if ( iToolbarItemCount > 2 ) {
        iLandscapeToolbarWidth = iToolbarItemWidth*(iToolbarItemCount) + ( (iToolbarItemCount-2) * KToolbarItemGap );
    } else if ( iToolbarItemCount == 2 ) {
        iLandscapeToolbarWidth = iToolbarItemWidth*iToolbarItemCount + KToolbarItemGap;
    } else {
        iLandscapeToolbarWidth = iToolbarItemWidth;
    }
    
    //two rows
    if ( (iToolbarItemCount % 2 ) != 0 ) {
        TInt tmpRound = iToolbarItemCount +1;
        TInt tmpRoundHalf = tmpRound/2;
        iLandscapeToolbarHeight2Rows = iToolbarItemHeight*2 + KToolbarItemGap;    
        if ( tmpRoundHalf > 2 ) {
            iLandscapeToolbarWidth2Rows = iToolbarItemWidth*tmpRoundHalf + ((tmpRoundHalf-2)*KToolbarItemGap);
        } else {
            iLandscapeToolbarWidth2Rows = iToolbarItemWidth;
        }    
    } else {
        iLandscapeToolbarHeight2Rows = iToolbarItemHeight*2 + KToolbarItemGap;    
        if ( iToolbarHalfCount > 2 ) {
            iLandscapeToolbarWidth2Rows = iToolbarItemWidth*iToolbarHalfCount + ((iToolbarHalfCount-2)*KToolbarItemGap);
        } else {
            iLandscapeToolbarWidth2Rows = iToolbarItemWidth;
        }
    }
}

void CCustomToolBar::ToolbarPortrait1rows() {
    TRect item = Rect();
    // Toolbar is in portrait mode
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);

    // Move to toolbar center
    item.Move((iPortraitToolbarWidth-iToolbarItemWidth)/2,0);
            
    item.Move(0,KToolbarItemGap);
 
    for (int i = 0; i < (iToolbarItemCount); i++) {
        iButtons[i]->iItem = item;
        item.Move(0,iToolbarItemHeight+KToolbarItemGap);
    }
}


void CCustomToolBar::ToolbarPortrait2rows() {
    TRect item = Rect();
    // Toolbar is in portrait mode
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);

    // Move to toolbar center
    item.Move((iPortraitToolbarWidth2Rows-iToolbarItemWidth-iToolbarItemWidth)/2,0);
            
    item.Move(0,KToolbarItemGap);
    
    TInt halfCount = (iToolbarItemCount/2);    
    if ( ( iToolbarItemCount % 2 ) != 0 ) {
        halfCount = halfCount+1;
    }
    
    for (int i = 0; i < halfCount; i++) {
        iButtons[i]->iItem = item;
        item.Move(0,iToolbarItemWidth+KToolbarItemGap);
    }
                        
    // New item line ------
    item = Rect();
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);
    item.Move(iToolbarItemWidth+KToolbarItemGap +((iPortraitToolbarWidth2Rows-iToolbarItemWidth-iToolbarItemWidth)/2),0);

    item.Move(0,KToolbarItemGap);
            
    for (int i = halfCount; i < (iToolbarItemCount); i++) {
        iButtons[i]->iItem = item;
        item.Move(0,iToolbarItemWidth+KToolbarItemGap);
    }
}


void CCustomToolBar::ToolbarLandscape2rows() {
    TRect item = Rect();
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);

    // Move to toolbar center
    item.Move(0,(iLandscapeToolbarHeight2Rows-iToolbarItemHeight-iToolbarItemHeight)/2);
            
    item.Move(KToolbarItemGap,0);
    
    TInt halfCount = (iToolbarItemCount/2);    
    if ( ( iToolbarItemCount % 2 ) != 0 ) {
        halfCount = halfCount+1;
    }
    
    for (int i = 0; i < halfCount; i++) {
        iButtons[i]->iItem = item;
        item.Move(iToolbarItemWidth+KToolbarItemGap,0);
    }
    
    // New item line ------
    item = Rect();
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);
    item.Move(0,iToolbarItemHeight+KToolbarItemGap +((iPortraitToolbarWidth - iToolbarItemWidth)/2));
                        
    item.Move(KToolbarItemGap,0);

    for (int i = halfCount; i < (iToolbarItemCount); i++) {
        iButtons[i]->iItem = item;
        item.Move(iToolbarItemWidth+KToolbarItemGap,0);
    }
    
}

void CCustomToolBar::ToolbarLandscape1rows() {
    TRect item = Rect();
    // Toolbar is in landscape mode
    item.SetWidth(iToolbarItemWidth);
    item.SetHeight(iToolbarItemHeight);

    // Move to toolbar center
    item.Move(0,(iLandscapeToolbarHeight-iToolbarItemHeight)/2);
            
    item.Move(KToolbarItemGap,0);

    for (int i = 0; i < (iToolbarItemCount); i++) {
        iButtons[i]->iItem = item;
        item.Move(iToolbarItemWidth+KToolbarItemGap,0);
    }
}
