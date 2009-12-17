/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SAP extension to Application settings
 *
*/



#ifndef CCAAPPSETTINGSSAPEXT_H
#define CCAAPPSETTINGSSAPEXT_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS
class CIMPSPresenceConnectionUi;
class CIMPSSAPSettings;
class CIMPSSAPSettingsStore;

// CLASS DECLARATION

/**
 *  Application settings extension to SAP settings
 *  @lib chat.lib
 *  @since Series 60 3.0
 */
class CCAAppSettingsSAPExt : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
        * @param aConnUI		   Connection UI..
         * @param aPreserveSessionIdInExit ETrue to preserve IMPSCUI Session Id when exiting,
         * needed in Always Online case
         * @param aSap If given, the settings will be saved in the sap
         * instead of cenrep.
         */
        static CCAAppSettingsSAPExt* NewL( CIMPSPresenceConnectionUi& aConnUi,
                                           TBool aPreserveSessionIdInExit = EFalse,
                                           CIMPSSAPSettings* aSap = NULL );


        /**
         * Two-phased constructor.
         * @see CCAAppSettingsSAPExt::NewL
         */
        static CCAAppSettingsSAPExt* NewLC( CIMPSPresenceConnectionUi& aConnUi,
                                            TBool aPreserveSessionIdInExit = EFalse,
                                            CIMPSSAPSettings* aSap = NULL );

        /**
         * Destructor.
         */
        virtual ~CCAAppSettingsSAPExt();

    public: // New functions

        /**
         * Sets int value to SAP settings
         * @since Series 60 3.0
         * @param aKey Key for key-value pair in SAP settings
         * @param aValue Value for key-value pair in SAP settings
         */
        void SetIntValueL( const TDesC& aKey, TInt aValue );

        /**
         * Gets int value from SAP settings
         * @since Series 60 3.0
         * @param aKey Key for key-value pair in SAP settings
         * @return Value for key
         */
        TInt IntValueL( const TDesC& aKey );


        /**
         * Sets alert tone file name to SAP settings
         * @since Series 60 3.0
         * @param aToneFileName Filename that is stored to SAP settings
         */
        void SetToneFileNameL( const TDesC& aToneFileName );

        /**
         * Gets alert tone file name from SAP settings
         * @since Series 60 3.0
         * @param aToneFileName Filename that is stored to SAP settings
         */
        void GetToneFileNameL( TDes& aToneFileName );

    private: //New functions

        /**
         * Gets editable stored SAP
         */
        CIMPSSAPSettings* StoredSAPLC();

        /**
         * Gets current SAP
         */
        CIMPSSAPSettings* CurrentSAPLC();

    private:

        /**
         * C++ default constructor.
         */
        CCAAppSettingsSAPExt( CIMPSPresenceConnectionUi& aConnUi,
                              TBool aPreserveSessionIdInExit );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL(  CIMPSSAPSettings* aSap );

    private:    // Data
        // Doesnt Own. Connection UI
        //Removed the ownership of iConnUI from this file and made it a reference
        //APPUI owns this connection UI instance and shares the same with this component.
        CIMPSPresenceConnectionUi& iConnUI;

        // Owns, sapsettingstore interface
        CIMPSSAPSettingsStore* iSapSettings;

        // does not own
        CIMPSSAPSettings* iSap;

        // ETrue if IMPSCUI Session Id should be preserved upon exit
        TBool iPreserveSessionIdInExit;
    };

#endif      // CCAAPPSETTINGSSAPEXT_H 

// End of File
