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
* Description:  Class takes care of current server selection logic
*
*/



#ifndef CWVSETTINGSUISERVERSELECTOR_H
#define CWVSETTINGSUISERVERSELECTOR_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class CIMPSSAPSettingsList;
class CWVSettingsUIDialogImp;

// CLASS DECLARATION

/**
*  Server selection popup query
*  Executes Server selection popup query, and updates model with selected
*  current server
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUIServerSelector ) : public CBase
    {
public: // New methods

    /**
    * Executes a selection query
    * @since 2.1
    * @param aSAPSettingsStore Pointer to settings model
    * @param aShowDefaultSAP whether to show the default sap or not
    * @param aForceSelection Does system force user to select something
    * example in case of deletion
    */
    static void ExecuteSelectionQueryLD( CIMPSSAPSettingsStore& aSAPSettingsStore,
    CWVSettingsUIDialogImp& aDialog,
    TBool aShowDefaultSAP = ETrue,
    TBool aForceSelection = EFalse );

private: //New functions

    /**
    * Implementation of selection query
    * @since 2.1
    */
    void DoSelectionL();

    /**
    * Reads server data to internal array
    * Ownership is transferred!
    * @since 2.1
    * @param aCurrentServerIndex Index of an item that was currently
    * selected
    * @return array of servers. NOTE can return NULL!
    */
    CDesCArrayFlat* GenerateServerListL( TInt& aCurrentServerIndex );

    /**
    * Find SAP internal ID according to selected index
    * @since 2.1
    * @param aIndex Index to be searched
    * @return SAP ID
    */
    TUint32 TranslateSelectedIndexL( TInt aIndex );

private: // Constructors and destructor
    /**
    * C++ default constructor.
    * @param aSAPSettingsStore Pointer to settings model
    * @param aExcludedServer Optional server to be excluded from list, for
    * @param aForceSelection Does system force user to select something
    */
    CWVSettingsUIServerSelector( CIMPSSAPSettingsStore& aSAPSettingsStore,
                                 TBool aShowDefaultSAP,
                                 TBool aForceSelection,
                                 CWVSettingsUIDialogImp& aDialog );

    /**
    * Destructor
    */
    virtual ~CWVSettingsUIServerSelector();

private: // Data
    // Doesn't own. pointer to SAP settings store
    CIMPSSAPSettingsStore& iSAPSettingsStore;
    // Owns. pointer to SAP settings list
    CIMPSSAPSettingsList* iSAPSettingsList;
    // Owns. Pointer to server list
    CDesCArray* iServerList;
    // Is selection forced one or not
    TBool iForceSelection;
    // Should default sap be displayed in list or not
    TBool iShowDefaultSAP;

    // the base dialog of service settings view
    CWVSettingsUIDialogImp& iDialog;
    };

#endif      // CWVSETTINGSUISERVERSELECTOR_H

// End of File
