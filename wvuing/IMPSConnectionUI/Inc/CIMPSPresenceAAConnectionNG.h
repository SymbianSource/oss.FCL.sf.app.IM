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
* Description:  IMPS connection management services to AA plugins.
*
*/

#ifndef __CIMPSPRESENCEAACONNECTIONNG_H
#define __CIMPSPRESENCEAACONNECTIONNG_H

// INCLUDES
#include <E32base.h>
#include <PEngPresenceEngineConsts2.h>
#include <impspresenceconnectionuiconstsng.h>

// FORWARD DECLARATION
class CIMPSPresenceAAConnectionImp;
class CIMPSSAPSettingsStore;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * CIMPSPresenceAAConnection.
 *
 * CIMPSPresenceAAConnection provides presence
 * connection management services to
 * presence / IMPS always automatic plugins.
 *
 *
 * @lib IMPSConnectionUi
 * @since 2.1
 */
class CIMPSPresenceAAConnection : public CBase
    {
    public:  // Two-phased constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aSapStore The SAP settings store to use.
         * Given SAP settings store object lifetime must be
         * longer created CIMPSPresenceConnectionUi.
         * If no SAP settings store provided, creates own
         * SAP Settings store internally when needed.
         */
        IMPORT_C static CIMPSPresenceAAConnection* NewL(
            TIMPSConnectionClient aClient,
            CIMPSSAPSettingsStore* aSapStore = NULL );

        /**
         * Two-phased constructor.
         * Leaves CIMPSPresenceAAConnection object on the
         * CleanupStack.
         *
         * @param aSapStore The SAP settings store to use.
         * Given SAP settings store object lifetime must be
         * longer created CIMPSPresenceConnectionUi.
         * If no SAP settings store provided, creates own
         * SAP Settings store internally when needed.
         */
        IMPORT_C static CIMPSPresenceAAConnection* NewLC(
            TIMPSConnectionClient aClient,
            CIMPSSAPSettingsStore* aSapStore = NULL );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSPresenceAAConnection();


    public:  // New connection handling methods


        /**
         * Connects to remote presence server.
         *
         * Connects the given client group to remote presence
         * server. Connection is established to default SAP defined
         * in SAP Settings Store.
         *
         * Status of the connection operation is returned as
         * return value. Internal Symbian OS errors (like running out
         * of resources, or other fatal errors coming from underlying
         * Presence Engine) are handled by leaving.
         *
         * @since 2.1
         * @param aClient Which client group to login.
         * @param aShowDetailedError Should we show error note to user
         * in case an error occurs during login.
         * @return
         */
        IMPORT_C TInt LoginL( TIMPSConnectionClient aClient,
                              TBool aShowDetailedError = EFalse );


        /**
         * Disconnects from remote presence server.
         *
         * Internal Symbian OS errors (like running out of resources,
         * or other fatal errors coming from underlying Presence Engine)
         * are handled by leaving.
         *
         * @since 2.1
         * @param aClient The client using the Connection UI to logout
         * from presence service. (This client is logged out.)
         * @param aIsScheduled Tells if this is a normal scheduled
         * scheduled logout (Need to ask confirmation from user)
         * @return Currently always KErrNone.
         */
        IMPORT_C TInt LogoutL( TIMPSConnectionClient aClient,
                               TBool aIsScheduled = EFalse );


        /**
         * Gets the remote presence server connection status.
         *
         * Gets the remote presence server connection status
         * for desired client group.
         *
         * @since 2.1
         * @param aClient The client which connection status
         * to check.
         * @return ETrue if client is logged in, else EFalse.
         */
        IMPORT_C TBool LoggedInL( TIMPSConnectionClient aClient );


        /**
         * Gets the pure connection status from Presence Engine.
         *
         * @since 2.1
         * @return Connection status from Presence Engine.
         */
        IMPORT_C TPEngNWSessionSlotState PureServiceStatusL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID );


        /**
         * Gets the current default server name.
         * !!!Note!!! Ownership is transferred to the caller!
         *
         * @since 2.6
         * @return the current default server name
         */
        IMPORT_C HBufC* DefaultServerNameL( TIMPSConnectionClient aClient );

        /**
         * Gets the active network session slot ID
         * @param aClient The client whose active session slot ID to get
         * !!!Note!!! Ownership is transferred to the caller!
         * @since 3.0
         */
        IMPORT_C CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL(
            TIMPSConnectionClient aClient );

    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSPresenceAAConnection();


        /**
         * Symbian OS constructor.
         */
        void ConstructL( TIMPSConnectionClient aClient,
                         CIMPSSAPSettingsStore* aSapStore );


    private: //Data
        CIMPSPresenceAAConnectionImp*       iImp;          ///<Implementation, owned
        TAny*                               iReserved1;    ///<Reserved, owned
    };



#endif      //__CIMPSPRESENCEAACONNECTIONNG_H

//  End of File

