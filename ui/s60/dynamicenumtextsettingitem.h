/*    dynamicenumtextsettingitem.h
 *
 * Dynamic enumerated text setting item
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef DYNAMICENUMTEXTSETTINGITEM_H
#define DYNAMICENUMTEXTSETTINGITEM_H

#include <aknsettingitemlist.h>

class CDesC16Array;


/**
 * Dynamic enumerated text setting item. The item is otherwise similar to
 * CAknEnumeratedTextPopupSettingItem, but the list of text items to choose
 * from is set at runtime instead of being read from a resource. S60 really
 * should come with one to begin with...
 */
class CDynamicEnumTextSettingItem : public CAknEnumeratedTextPopupSettingItem {
public:
    /** 
     * Constructor.
     * 
     * @param aResourceId Resource ID for the setting item
     * @param aTexts Texts to be used. The same texts are used for both regular
     *               and popped up text lists. The ownership is not transferred
     *               to the setting item.
     * @param aValue Reference to the selection value
     */
    CDynamicEnumTextSettingItem(TInt aResourceId, const CDesCArray &aTexts,
                                TInt &aValue);

    /**
     * Destructor.
     */
    ~CDynamicEnumTextSettingItem();

protected:
    void CompleteConstructionL();

private:
    const CDesCArray &iTexts;
};


#endif
