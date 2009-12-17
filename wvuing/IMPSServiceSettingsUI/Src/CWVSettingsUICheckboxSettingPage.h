/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Checkbox setting page
*
*/

#ifndef __CWVSETTINGSUICHECKBOXSETTINGPAGE_H
#define __CWVSETTINGSUICHECKBOXSETTINGPAGE_H

//  INCLUDES
#include <AknCheckBoxSettingPage.h>
#include <AknSettingPage.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Checkbox setting page
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( CWVSettingsUICheckboxSettingPage ) :
        public CAknCheckBoxSettingPage, MAknSettingPageObserver
    {
public: // New functions
    /**
    * Two-phased constructor.
    * @param aResourceId the resource of the setting page
    * @param aSelectionItemList the selection item list of the setting page,
    *        ownership is transferred
    */
    static CWVSettingsUICheckboxSettingPage* NewL( TInt aResourceId,
    CSelectionItemList* aSelectionItemList );

protected: // constructor and destructor

    /**
    * C++ default constructor.
    * @param aResourceId the resource of the setting page
    * @param aSelectionItemList the selection item list of the setting page
    */
    CWVSettingsUICheckboxSettingPage( TInt aResourceId,
                                      CSelectionItemList* aSelectionItemList );

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUICheckboxSettingPage();

private: //Functions from base classes

    /*
    * From MAknSettingPageObserver
    * @see MAknSettingPageObserver
    */
    void HandleSettingPageEventL( CAknSettingPage* aSettingPage,
                                  TAknSettingPageEvent aEventType );

    /**
     * From CAknSettingPage.
     * @see CAknSettingPage
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );

    /**
     * From CAknSettingPage.
     * @see CAknSettingPage
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

private: // new functions
    /**
    * Sets the Cba to only "cancel" if no items are selected
    * @since 2.6
    */
    void SetCbaL();

private:    // Data
    // selection item list, does NOT own
    CSelectionItemList* iSelectionItemList;

    };

#endif      // __CWVSETTINGSUICHECKBOXSETTINGPAGE_H

// End of File
