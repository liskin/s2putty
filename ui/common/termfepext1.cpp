/*    termfepext1.cpp
 *
 * FEP extension class for S60 terminal control
 *
 * Copyright 2005 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include "termfepext1.h"
#include <aknedsts.h>
#include <coemop.h>


CTermFepExt1 *CTermFepExt1::NewL(MObjectProvider &aObjectProvider) {
    CTermFepExt1 *self = new (ELeave) CTermFepExt1(aObjectProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


CTermFepExt1::CTermFepExt1(MObjectProvider &aObjectProvider)
    : iObjectProvider(aObjectProvider) {
}


CTermFepExt1::~CTermFepExt1() {
    delete iState;
}


void CTermFepExt1::ConstructL() {

    // Need an initial state object of type CAknEdwinState
    CAknEdwinState *state = new (ELeave) CAknEdwinState();
    state->SetObjectProvider(&iObjectProvider);
    state->SetFlags(EAknEditorFlagNoLRNavigation | EAknEditorFlagNoT9);
    state->SetDefaultCase(EAknEditorLowerCase);
    // FIXME: EAknEditorTextCase would work on S60 v3 but not earlier.
    // It's not very useful without T9 though.
    state->SetPermittedCases(EAknEditorLowerCase | EAknEditorUpperCase);
    iState = state;
}


MCoeFepAwareTextEditor_Extension1::CState *CTermFepExt1::State(
    TUid /*aTypeSafetyUid*/) {

    // Can't really compare UIDs since we need to be able to return the initial
    // CAknEdwinState object when asked (or otherwise the FEP will crash) and
    // we don't have a valid UID for it. Let's just hope everybody always
    // uses CAknEdwinState objects and nothing else...
    return iState;
}


void CTermFepExt1::SetStateTransferingOwnershipL(CState *aState,
                                                 TUid aTypeSafetyUid) {
    delete iState;
    iState = aState;
    iUid = aTypeSafetyUid;
}


void CTermFepExt1::StartFepInlineEditL(
    TBool &aSetToTrue,
    const TCursorSelection & /*aCursorSelection*/,
    const TDesC & /*aInitialInlineText*/,
    TInt /*aPositionOfInsertionPointInInlineText*/,
    TBool /*aCursorVisibility*/,
    const MFormCustomDraw * /*aCustomDraw*/,
    MFepInlineTextFormatRetriever & /*aInlineTextFormatRetriever*/,
    MFepPointerEventHandlerDuringInlineEdit & /*aPointerEventHandlerDuringInlineEdit*/) {
    
    aSetToTrue = ETrue;
}


void CTermFepExt1::SetCursorType(TBool& aSetToTrue,
                                 const TTextCursor & /*aTextCursor*/) {
    aSetToTrue = ETrue;
}
