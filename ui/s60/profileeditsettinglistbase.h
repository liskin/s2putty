/*    profileeditsettinglistbase.h
 *
 * Putty profile edit view setting list base class
 *
 * Copyright 2007 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

#ifndef PROFILEEDITSETTINGLISTBASE_H
#define PROFILEEDITSETTINGLISTBASE_H

#include <coecntrl.h>
#include <aknsettingitemlist.h> 

// Forward declarations
class CPuttyEngine;
class CProfileEditView;

/**
 * Base class for profile edit setting lists
 */
class CProfileEditSettingListBase : public CAknSettingItemList {

public:
    /** 
     * Destructor
     */
    ~CProfileEditSettingListBase();

protected: // Constructors
    CProfileEditSettingListBase(CPuttyEngine &aPutty,
                                CProfileEditView &aView);

private: // From CCoeControl
    void SizeChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    

protected:
    CPuttyEngine &iPutty;
    CProfileEditView &iView;
};


#endif
