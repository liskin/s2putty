/*    dynamicenumtextsettingitem.cpp
 *
 * Dynamic enumerated text setting item
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#include <badesca.h>
#include "dynamicenumtextsettingitem.h"


// Constructor
CDynamicEnumTextSettingItem::CDynamicEnumTextSettingItem(
    TInt aResourceId, const CDesCArray &aTexts, TInt &aValue)
    : CAknEnumeratedTextPopupSettingItem(aResourceId, aValue),
      iTexts(aTexts) {
}


// Destructor
CDynamicEnumTextSettingItem::~CDynamicEnumTextSettingItem() {
}


// CAknSettingItem::CompleteConstructionL()
void CDynamicEnumTextSettingItem::CompleteConstructionL() {
    
    // Let the base class create the arrays and do other initialization
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();

    CArrayPtr<CAknEnumeratedText> *enumArray = EnumeratedTextArray();
    CArrayPtr<HBufC> *popUpArray = PoppedUpTextArray();

    for ( TInt i = 0; i < iTexts.Count(); i++ ) {
        // Enumerated text
        HBufC *text = iTexts[i].AllocLC();
        CAknEnumeratedText *enumText = new CAknEnumeratedText(i, text);
        CleanupStack::Pop(); //text
        CleanupStack::PushL(enumText);
        enumArray->AppendL(enumText);
        CleanupStack::Pop(); //enumText

        // Popped up text
        HBufC *popUpText = iTexts[i].AllocLC();
        popUpArray->AppendL(popUpText);
        CleanupStack::Pop(); //popUpText
    }
}
