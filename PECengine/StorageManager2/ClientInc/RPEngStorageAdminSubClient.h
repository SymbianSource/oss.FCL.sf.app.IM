/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  it is used to update session slot state and signal new
*				 session slot events
*
*/


#ifndef __RPENGSTORAGEADMINSUBCLIENT_H__
#define __RPENGSTORAGEADMINSUBCLIENT_H__

//  INCLUDES
#include <e32def.h>
#include "PEngStorageGlobals.h"

// FORWARD DECLARATIONS
class RPEngStorageAdminClient;

/**
 *  Storage Admin sub-client
 *  it is used to update session slot state and
 *  signal new session slot events
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */

// CLASS DECLARATION
class  RPEngStorageAdminSubClient: public RSubSessionBase

    {
    public:		// Constructors and destructors

        /**
         * C++ default constructor.
         */
        IMPORT_C RPEngStorageAdminSubClient( );

        /**
         * Destructor.
         */
        IMPORT_C ~RPEngStorageAdminSubClient( void );

    public: 	// new methods

        /**
         *  Connects to the presence server
         *
         *	@since 3.0
         *	@param aAdminClient main admin client
         *	@param aSessionName packed session slot Id
         *	@return KErrNone or any of system wide error codes
         */
        IMPORT_C TInt Connect(
            RPEngStorageAdminClient& aAdminClient,
            const TDesC8& aSesssionName );

        /**
         *	Close connection to the presence server
         *	When this sub-session is closed it is assumed that
         *  State of the connection is closed
         *
         *	@since 3.0
         */
        IMPORT_C void Close();

    public:		// new Storage management Functions

        /**
         *	Update Presence State of defined connection.
         *
         *	@since 3.0
         *  @param aState state of the Session Slot, in case
         *				aEven is not reconstructed, this value
         *				is used instead to determine state of the
         *				session
         *	@param aEvent new event of session slot from the Presence Server
         *	@return KErrNone if notification went OK,
         *					otherwise system wide error code
         */
        IMPORT_C TInt NotifyEvent( const TDesC8& aEvent );

        /**
         *  Notify error from to the listeners
         *
         *  @since 3.0
         *  @param aError code to be notified to listeners
         */
        IMPORT_C TInt NotifyError( TInt aError );

        /**
         *	Check if Application Id is registered for given session slot
         *
         *  @since 3.0
         *	@param aAppId application Id
         *  @return KErrNone if registered, KErrNotFound if not
         *			system wide error code if something other goes wrong
         */
        IMPORT_C TInt CheckAppIdRegistration( const TDesC& aAppId );

    private:	// Data

        /// Flag if sub Session is connected to the Server
        TBool										iConnected;


    };

#endif      //  __RPENGSTORAGEADMINSUBCLIENT_H__





















