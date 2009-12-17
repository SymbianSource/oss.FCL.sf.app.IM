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
* Description:  Setting item array creator
*
*/



#ifndef CWVSETTINGSUISETTINGITEMCREATOR_H
#define CWVSETTINGSUISETTINGITEMCREATOR_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CAknSettingItem;

// CLASS DECLARATION

/**
*  Setting item list creator
*  Creates a setting item list.
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUISettingItemCreator ) : public CBase
    {
public:

    /**
    * Creates a setting item list.
    * @param aCASettings if true we show also CA setting item
    * @param aServerInUse the server in use
    * @param aPecLoginType PEC login type
    * @param aImLoginType IM login type
    * @param aShowPEC should we show PEC login type or not
    * @return setting item array
    */
    static CAknSettingItemArray* CreateSettingItemListL( TBool aCASettings,
    TDes& aServerInUse,
    TInt& aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
    TDes& aMemorySelectionType,
#endif
    TDes& aConnectionDaysIM,
    TDes& aConnectionHoursIM );


    /**
    * Used to ask if scheduling is supported
    * @return ETrue if scheduling is supported
    */
    static TBool SchedulingSupported();

private: // Constructor and destructor
    // prohibit construction and destruction

    /**
    * C++ default constructor.
    */
    CWVSettingsUISettingItemCreator();

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUISettingItemCreator();

private:
    /**
    * Adds setting item to the setting item array
    * @since 2.1
    * @param aIndex index of the setting item
    * @param aResourceId resource ID for the setting item
    * @param aSettingPageResourceId setting page resource id
    * @param aAssosiatedResource assosiated resource id
    * @param aServerInUse server in use
    * @param aPecLoginType PEC login type
    * @param aImLoginType IM login type
    * @return setting item
    */
    static CAknSettingItem* CreateSettingItemLC( TInt aIndex,
                                                 TInt aResourceId,
                                                 TInt aSettingPageResourceId,
                                                 TInt aAssosiatedResource,
                                                 TDes& aServerInUse,
                                                 TInt& aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                                 TDes& aMemorySelectionType,
#endif
                                                 TDes& aConnectionDaysIM,
                                                 TDes& aConnectionHoursIM );

private:    // Data

    static TInt IntResourceValueL( TInt aResourceId );

private:    // Data

    };

#endif      // CWVSETTINGSUISETTINGITEMCREATOR_H

// End of File
