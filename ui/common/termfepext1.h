/*    termfepext1.h
 *
 * FEP extension class for S60 terminal control
 *
 * Copyright 2005 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef __TERMFEPEXT1_H__
#define __TERMFEPEXT1_H__

#include <fepbase.h>

class MObjectProvider;

/**
 * FEP extension class for S60 terminal control. All S60 FEP-aware
 * editors need to implement MCoeFepAwareTextEditor_Extension1 and
 * own a CAknEdwinState object. This isn't really documented anywhere except in
 * a single Forum Nokia post...
 */
class CTermFepExt1 : public CBase, public MCoeFepAwareTextEditor_Extension1 {
    
public:
    static CTermFepExt1 *NewL(MObjectProvider &aObjectProvider);
    ~CTermFepExt1();

    // MCoeFepAwareTextEditor_Extension1 methods
    CState* State(TUid aTypeSafetyUid);
    void SetStateTransferingOwnershipL(CState* aState, TUid aTypeSafetyUid);
    void StartFepInlineEditL(
        TBool& aSetToTrue,
        const TCursorSelection& aCursorSelection,
        const TDesC& aInitialInlineText,
        TInt aPositionOfInsertionPointInInlineText,
        TBool aCursorVisibility,
        const MFormCustomDraw* aCustomDraw,
        MFepInlineTextFormatRetriever& aInlineTextFormatRetriever,
        MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
    void SetCursorType(TBool& aSetToTrue, const TTextCursor& aTextCursor);

private:
    CTermFepExt1(MObjectProvider &aObjectProvider);
    void ConstructL();

    CState *iState;
    TUid iUid;
    MObjectProvider &iObjectProvider;
};


#endif
