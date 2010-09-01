/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Setting item array editor
*
*/



#ifndef CWVSETTINGSUISETTINGITEMEDITOR_H
#define CWVSETTINGSUISETTINGITEMEDITOR_H

//  INCLUDES
#include "impscommonuibuilddefinitions.h"

#include <e32base.h>
#include <badesca.h>
#include <aknsettingpage.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CAknSettingItemArray;
class CIMPSSAPSettingsStore;
class CSelectionItemList;
class CIMPSPresenceConnectionUi;
class CIMPSSAPSettings;

/**
*  Setting item list creator
*  Creates a setting item list.
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUISettingItemEditor ) : public CBase, public MAknSettingPageObserver
    {
public: // Constructor and destructor
    // prohibit construction and destruction

    /**
    * C++ default constructor.
    */
    CWVSettingsUISettingItemEditor();

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUISettingItemEditor();

public: // New methods

    /**
    * Creates a setting item list.
    * @param aIsPopUp is the setting item a popup
    * @param aListBoxIndex index of the edited item
    * @param aSettingItemList setting item array
    * @param aSAPSettingsStore SAP settings store
    * @param aPECLoginType PEC login type
    * @param aPECLoginTypeEdited tells if the login type was edited
    * @param aIMLoginTypeEdited tells if the login type was edited
    * @param aSelectedDaysPEC selected days array of PEC
    * @param aConnectHoursPECInt connection hours
    * @param aPECConnHoursEdited was PEC connection hours edited
    * @param aSelectedDaysIM selected days array of IM
    * @param aConnectHoursIMInt connection hours
    * @param aIMConnHoursEdited was IM connection hours edited
    * @param aCancelled was edit cancelled
    * @param aCASettings tell if we are showing CA settings item
    * @return setting item list
    */
    void EditSettingItemL( TBool aIsPopUp,
                           TInt aListBoxIndex,
                           CAknSettingItemArray& aSettingItemList,
                           CIMPSSAPSettingsStore& aSAPSettingsStore,
                           TInt& aIMLoginType,
                           TBool& aIMLoginTypeEdited,
                           CSelectionItemList* aSelectedDaysIM,
                           TInt& aConnectHoursIMInt,
                           TBool& aIMConnHoursEdited,
                           TBool& aCancelledaCancelled,
                           TInt aSettingItem,
                           CIMPSPresenceConnectionUi* aConnectionUI = NULL,
                           TBool * aForcedLoginMade = NULL );

private: // New methods

    /**
    * Show missing information error note
    * @since 2.1
    */
    void ShowMissingInfoErrorNoteL();

    /**
    * Edits connect days setting item
    * @param aSelectionItemList a list of the days that can be selected
    * @param aCancelled was edit cancelled
    * @since 2.5
    */
    void EditConnectedDaysL( CSelectionItemList* aSelectionItemList, TBool& aCancelled );

    /**
    * Edit connect hours setting item
    * @param aConnectHoursInt tells which setting item is selected
    * @param aCancelled was edit cancelled
    * @since 2.5
    */
    void EditConnectedHoursL( TInt& aConnectHoursInt, TBool& aCancelled );

    /**
    * Edit login type setting item
    * @see EditConnectedHoursL
    * @since 2.6
    */
    void EditLoginTypeL( TBool aIsPopUp,
                         TInt aListBoxIndex,
                         CAknSettingItemArray& aSettingItemList,
                         CIMPSSAPSettingsStore& aSAPSettingsStore,
                         TInt& aIMLoginType,
                         TBool& aIMLoginTypeEdited,
                         CSelectionItemList* aSelectedDaysIM,
                         TInt& aConnectHoursIMInt,
                         TBool& aIMConnHoursEdited,
                         TBool& aCancelled,
                         TInt aSettingItem );

    /**
     * Shows necessary query and executes login
     * if user tries to activate automatic login
     * type for server he/she hasn't not logged in
     * before.
     * @since S60 v3.2
     * @param aSapSettings reference to current SAP
     * @return ETrue if user accepted quey and login succesful,
     *         EFalse if user cancelled query or login cancelled
     *         or failed.
     */
    TBool ForceLoginL( CIMPSSAPSettings& aSapSettings );

private: // methods from base classes
    /**
    * From MAknSettingPageObserver
    * @see MAknSettingPageObserver
    */
    virtual void HandleSettingPageEventL( CAknSettingPage* aSettingPage,
                                          TAknSettingPageEvent aEventType );


private:    // Data

    // Doesn't own: SAP Settings store
    CIMPSSAPSettingsStore* iSAPSettingsStore;
    // Doesn't own: Setting item array
    CAknSettingItemArray* iSettingItemArray;

    // Doesn't own: Connection UI
    CIMPSPresenceConnectionUi* iConnectionUI;

    // Will tell if forced login was completed or not
    TBool* iForcedLoginMade;
    };

#endif      // CWVSETTINGSUISETTINGITEMEDITOR_H

// End of File
