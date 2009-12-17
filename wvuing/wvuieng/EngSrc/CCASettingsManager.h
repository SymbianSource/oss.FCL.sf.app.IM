/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Settings manager
*
*/


#ifndef CCASETTINGSMANAGER_H
#define CCASETTINGSMANAGER_H

// INCLUDES
#include <e32base.h>
#include "MCASettings.h"

// FORWARD DECLARATIONS
class MCASettingsObserver;
class MCASDCRVariator;
class CIMPSSAPSettingsStore;

// CLASS DECLARATION

/**
*  Handles all application settings-related operations.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCASettingsManager : public CBase, public MCASettings
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aIdentifier UID of application
        */
        static CCASettingsManager* NewL( TUid aIdentifier );

        /**
        * Destructor.
        */
        virtual ~CCASettingsManager();

    public: // From MCASettings

        /**
        * From MCASettings. @see MCASettings
        * @since 1.2
        * @param aObserver Settings observer pointer
        */
        void AddObserverL( const MCASettingsObserver* aObserver );

        /**
        * From MCASettings. @see MCASettings
        * @since 1.2
        * @param aObserver Settings observer pointer
        */
        void RemoveObserver( const MCASettingsObserver* aObserver );

        /**
        * From MCASettings. @see MCASettings
        * @since 1.2
        * @param aSetting Settings enumeration that value is returned
        * @return Value of setting
        */
        TBool Value( TCASettingFlags aSetting,
                     CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * @since 1.2
        * @param aSetting Settings enumeration that value is returned
        * @return Value of setting
        */
        TInt Value( TCASettingIntegers aSetting,
                    CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * Transfers ownership!!!
        * @since 1.2
        * @param aSetting Settings enumeration that value is returned
        * @return Value of setting
        */
        HBufC* ValueL( TCASettingStrings aSetting,
                       CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * @param aSetting Settings enumeration that value is set
        * @param aValue Value of setting
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors. NULL if rollback is wanted in every
        *                        error situation.
        */
        void SetValueL( TCASettingFlags aSetting, TBool aValue,
                        RArray<TInt>* aIgnoreRollback = NULL,
                        CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * @param aSetting Settings enumeration that value is set
        * @param aValue Value of setting
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors. NULL if rollback is wanted in every
        *                        error situation
        */
        void SetValueL( TCASettingIntegers aSetting, TInt aValue,
                        RArray<TInt>* aIgnoreRollback = NULL,
                        CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * @param aSetting Settings enumeration that value is set
        * @param aValue Value of setting
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors. NULL if rollback is wanted in every
        *                        error situation
        */
        void SetValueL( TCASettingStrings aSetting, const TDesC& aValue,
                        RArray<TInt>* aIgnoreRollback = NULL,
                        CIMPSSAPSettings* aSap = NULL );

        /**
        * From MCASettings. @see MCASettings
        * @since 1.2
        */
        void FlushData();

        /**
        * From MCASettings. @see MCASettings
        * @since 3.1
        */
        void SetDefaultSapL( CIMPSSAPSettings* aSap );

        /**
        * From MCASettings. @see MCASettings
        * @since 3.1
        */
        HBufC* OwnAliasL();

        /**
        * From MCASettings. @see MCASettings
        * @since 3.1
        */
        void SetOwnAliasL( const TDesC& aAlias );

    private:

        /**
        * By default Symbian OS constructor is private.
        * @param aIdentifier Identifier of setting storage used
        */
        void ConstructL( TUid aIdentifier );

        /**
        * C++ default constructor.
        */
        CCASettingsManager();

        /**
        * Notifies observers about changed setting
        * @param aEnum Setting that was changed
        */
        void NotifyObserversL( TInt aEnum );

        /**
        * Returns default value for given setting if such exists
        * @since 1.2
        * @param aEnum Setting enumeration
        * @return Default value for that setting
        */
        TInt DefaultIntegerValue( TInt aEnum ) const;

        /**
        * Returns default value for given setting if such exists
        * @since 1.2
        * @param aEnum Setting enumeration
        * @return Default value for that setting
        */
        TBool DefaultBooleanValue( TInt aEnum ) const;

        /**
        * Helper method for setting boolean and integer values
        * @since 2.1
        * @param aSetting Settings enumeration that value is set
        * @param aValue Value of setting
        * @param oldValue old value of setting which is set if
        *        observers notifying leaves
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors. NULL if rollback is wanted in every
        *                        error situation
        */
        void SetIntValueL( TInt aSetting, TInt aValue, TInt oldValue,
                           RArray<TInt>* aIgnoreRollback = NULL,
                           CIMPSSAPSettings* aSap = NULL );

        /**
        * Converts the settings integer to sap opaque ids
        * @param aEnum Setting that was changed
        */
        const TDesC& ConvertSettingInteger( TInt aEnum ) const;

        /**
        * Get correct SAP. If aSap is NULL and RD_SETTINGS_FACELIFT
        * is defined will return default SAP.
        * !!NOTE!! If returns NULL, then there is nothing in
        * CleanupStack.
        * @param aSap server
        */
        CIMPSSAPSettings* GetSAPLC( CIMPSSAPSettings* aSap );

        /**
        * Store to SAP or cenrep
        * @param aSap server (can be NULL)
        * @param aSetting the setting which we are saving
        * @param aValue the value of the setting
        */
        void StoreL( CIMPSSAPSettings* aSap,
                     TInt aSetting,
                     TInt aValue );

        /**
        * Store to SAP or cenrep
        * @param aSap server (can be NULL)
        * @param aSetting the setting which we are saving
        * @param aValue the value of the setting
        */
        void StoreL( CIMPSSAPSettings* aSap,
                     TInt aSetting,
                     const TDesC& aValue );

        /**
         * Finds alias for given aUserID inside aAliasTable.
         * @param aAliasTable string containeing all aliases for different ids
         * @param aUserId Userid to look for
         * @param aIndex Position inside aAliasTable, where the wanted alias starts.
         *               Or KErrNotFound
         * @param aLength aLength length of alias
         */
        void LocateAliasL( const TDesC& aAliasTable, const TDesC& aUserId,
                           TInt& aIndex, TInt& aLength );

    private: // Data

        // Array of observers
        RPointerArray<MCASettingsObserver> iObserverList;

        MCASDCRVariator* iSDCRVariator;

        CIMPSSAPSettingsStore* iSapSettingsStore;
        CIMPSSAPSettings* iDefaultSap;
        TBool iSettingsFaceLift;
    };

#endif      // CCASETTINGSMANAGER_H

// End of File
