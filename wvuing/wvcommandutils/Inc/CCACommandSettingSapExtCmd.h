/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for Settings Interface for getting/setting tonefilename etc
*
*/

#ifndef CCACOMMANDSETTINGASPEXTCMD_H
#define CCACOMMANDSETTINGASPEXTCMD_H



// INCLUDES
#include "MCASettingsPC.h"
#include"MCASettingSapExt.h"

// FORWARD DECLARATIONS
class CIMPSPresenceConnectionUi;
class CIMPSSAPSettingsStore;
class MCASettingsPC;
class CIMPSSAPSettings;


class CCACommandSettingSapExtCmd : public CBase, public MCASettingSapExt
    {
    public:
        /**
        * Two-phased constructor.
        * @param aSettingsPC Reference to the settings process component
        * @param aPreserveSessionIdInExit ETrue to preserve IMPSCUI Session Id when exiting,
        * needed in Always Online case
        * @param aSAPSettings If given, the settings will be saved in the sap
        * instead of cenrep.
        */
        static CCACommandSettingSapExtCmd* NewLC( MCASettingsPC* aSettingsPC,
                                                  TBool aPreserveSessionIdInExit = EFalse,
                                                  CIMPSSAPSettings* aSAPSettings = NULL );

        /**
         * Two-phased constructor.
         * @param aSettingsPC Reference to the settings process component
         * @param aPreserveSessionIdInExit ETrue to preserve IMPSCUI Session Id when exiting,
         * needed in Always Online case
         * @param aSAPSettings If given, the settings will be saved in the sap
         * instead of cenrep.
         */

        static CCACommandSettingSapExtCmd* NewL( MCASettingsPC* aSettingsPC,
                                                 TBool aPreserveSessionIdInExit = EFalse,
                                                 CIMPSSAPSettings* aSAPSettings = NULL );


        /**
         * Destructor.
         */
        virtual ~CCACommandSettingSapExtCmd( );


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

    public:
        //New Methods

        /**
        * Gets editable stored SAP
        */
        CIMPSSAPSettings* StoredSAPLC( );


        /**
        * Gets current SAP
        */
        CIMPSSAPSettings* CurrentSAPLC( ) ;

    private:

        /*
         *ConstructL
         */
        void ConstructL( CIMPSSAPSettings* aSAPSettings );



        /*
         *Default Constructor
         * @param aPreserveSessionIdInExit ETrue to preserve IMPSCUI Session Id when exiting,
         * needed in Always Online case
         * @param aSettingsPC Reference to the settings process component
         */
        CCACommandSettingSapExtCmd( TBool aPreserveSessionIdInExit,
                                    MCASettingsPC* aSettingsPC );


    private://data members

        //Does not own
        CIMPSSAPSettings* iSAPSettings;

        //Does not own
        MCASettingsPC* iSettingsPC;

        // ETrue if IMPSCUI Session Id should be preserved upon exit
        TBool iPreserveSessionIdInExit;

        // Owns, sapsettingstore interface
        CIMPSSAPSettingsStore* iSAPSettingsStore;

        // Owns. Connection UI
        CIMPSPresenceConnectionUi* iConnectionUI;
    };


#endif