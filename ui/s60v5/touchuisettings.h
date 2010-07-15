/*    touchuisettings.h
 *
 * Putty touch ui settings file
 *
 * Copyright 2009 Risto Avila
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PUTTY_TOUCHUI_SETTINGS
#define PUTTY_TOUCHUI_SETTINGS

#include <e32base.h>
#include <f32file.h>
#include <bautils.h>
#include <eikdef.h>
#include <eikenv.h>
#include <aknview.h>
#include <aknwaitdialog.h>
#include <hal.h>

class CPuttyEngine;

class TTouchSettings {
    public:
        TTouchSettings();
        ~TTouchSettings ( ); 
    public:
        TInt GetShowToolbar() { return iShowtoolbar; };
        TInt GetAllowMouseGrab() { return iAllowMouseGrab; };
        TInt GetSingleTap() { return iSingleTap; };
        TInt GetDoubleTap() { return iDoubleTap; };
        TInt GetLongTap() { return iLongTap; };
        TInt GetSwipeLeft() { return iSwipeLeft; };
        TInt GetSwipeRight() { return iSwipeRight; };
        TInt GetSwipeUp() { return iSwipeUp; };
        TInt GetSwipeDown() { return iSwipeDown; };
        
        TInt GetTbButton1() { return itbButton1; };
        TInt GetTbButton2() { return itbButton2; };
        TInt GetTbButton3() { return itbButton3; };
        TInt GetTbButton4() { return itbButton4; };
        TInt GetTbButton5() { return itbButton5; };
        TInt GetTbButton6() { return itbButton6; };
        TInt GetTbButton7() { return itbButton7; };
        TInt GetTbButton8() { return itbButton8; };
        
        TInt GetTbButtonCount() { return itbButtonCount; };
        TInt GetTbButtonWidth() { return itbButtonWidth; };
        TInt GetTbButtonHeigth() { return itbButtonHeigth; };        
        TInt GetButtonUpBGTransparency() { return iButtonUpBackgroundTransparency; };
        TInt GetButtonUpTextTransparency() { return iButtonUpTextTransparency; };
        TInt GetButtonDownBGTransparency() { return iButtonDownBackgroundTransparency; };
        TInt GetButtonDownTextTransparency() { return iButtonDownTextTransparency; };
        TInt GetButtonFontSize() { return iButtonFontSize; };
        
        void SetShowToolbar(TInt aCmd) { iShowtoolbar = aCmd; };
        void SetAllowMouseGrab(TInt aCmd) { iAllowMouseGrab = aCmd; };
        void SetSingleTap(TInt aCmd) { iSingleTap = aCmd; };
        void SetDoubleTap(TInt aCmd) { iDoubleTap = aCmd; };
        void SetLongTap(TInt aCmd) { iLongTap = aCmd; };
        void SetSwipeLeft(TInt aCmd) { iSwipeLeft = aCmd; };
        void SetSwipeRight(TInt aCmd) { iSwipeRight = aCmd; };
        void SetSwipeUp(TInt aCmd) { iSwipeUp = aCmd; };
        void SetSwipeDown(TInt aCmd) { iSwipeDown = aCmd; };
        
        void SetTbButton1(TInt aCmd) { itbButton1 = aCmd; };
        void SetTbButton2(TInt aCmd) { itbButton2 = aCmd; };
        void SetTbButton3(TInt aCmd) { itbButton3 = aCmd; };
        void SetTbButton4(TInt aCmd) { itbButton4 = aCmd; };
        void SetTbButton5(TInt aCmd) { itbButton5 = aCmd; };
        void SetTbButton6(TInt aCmd) { itbButton6 = aCmd; };
        void SetTbButton7(TInt aCmd) { itbButton7 = aCmd; };
        void SetTbButton8(TInt aCmd) { itbButton8 = aCmd; };
        
        void SetTbButtonCount(TInt aCmd) { itbButtonCount = aCmd; };
        void SetTbButtonWidth(TInt aCmd) { itbButtonWidth = aCmd; };
        void SetTbButtonHeigth(TInt aCmd) { itbButtonHeigth = aCmd; };
        void SetButtonUpBGTransparency(TInt aCmd) { iButtonUpBackgroundTransparency = aCmd; };
        void SetButtonUpTextTransparency(TInt aCmd) { iButtonUpTextTransparency = aCmd; };
        void SetButtonDownBGTransparency(TInt aCmd) { iButtonDownBackgroundTransparency = aCmd; };
        void SetButtonDownTextTransparency(TInt aCmd) { iButtonDownTextTransparency = aCmd; };
        void SetButtonFontSize(TInt aCmd) { iButtonFontSize = aCmd; };
        
        void SetDefault(); // Set default values
        
        void ReadSettingFileL();
        void WriteSettingFileL();
        void SetDataDirectory(TBuf<25> aDataDirectory) { iDataDirectory = aDataDirectory; };
        
        TBool TestIfToolBarActionAllreadySet(TInt aCmd);
        //aButtonToSwap button number to swap with aCmdOfTheButtonToSwapWith command of the button to swap with the button number
        TBool SwapButtons(TInt aButtonToSwap, TInt aCmdOfTheButtonToSwapWith);
    private:
        TInt iShowtoolbar;
        TInt iAllowMouseGrab;
        TInt iSingleTap;
        TInt iDoubleTap;
        TInt iLongTap;
        TInt iSwipeLeft;
        TInt iSwipeRight;
        TInt iSwipeUp;
        TInt iSwipeDown;
        
        TInt itbButton1;
        TInt itbButton2;
        TInt itbButton3;
        TInt itbButton4;
        TInt itbButton5;
        TInt itbButton6;
        TInt itbButton7;
        TInt itbButton8;
               
        TInt itbButtonCount;
        TInt itbButtonWidth;
        TInt itbButtonHeigth;
        
        TInt iButtonUpBackgroundTransparency;
        TInt iButtonUpTextTransparency;
        TInt iButtonDownBackgroundTransparency;
        TInt iButtonDownTextTransparency;
        TInt iButtonFontSize;
        
        TBuf<25> iDataDirectory; // "x:\private\12345678\data\"
        TFileName iSettingFile;
        
};

#endif
