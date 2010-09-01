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
* Description: Interface for access services
*
*/



#ifndef MCAIMPSACCESSCLIENT_H
#define MCAIMPSACCESSCLIENT_H

//  INCLUDES
#include "MCAImpsClient.h"
#include <ImpsAccessCli.h>

// CLASS DECLARATION

/**
*  Interface for access services.
*  Same interface as RImpsAccessClient
*  Comments are copied from RImpsAccessClient
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class MCAImpsAccessClient : public MCAImpsClient
    {
    public: // New functions

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener object for Access events.
        * @since 1.2
        * @param aObserver The observer.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        virtual void RegisterL(
            MImpsAccessHandler2* aObserver,
            TInt aPriority = 0 ) = 0;

        /**
        * Unregisters the listener object and disconnects from the server.
        * @since 1.2
        */
        virtual void Unregister() = 0;

        /**
         * @see RImpsAccessClient2
         */
        virtual TInt NumberOfSessionsL( TImpsCspIdentifier& aCspId ) = 0;

        // ACCESS SERVICES

        /**
         * @see RImpsAccessClient2
         */
        virtual TInt LoginL(  TImpsCspIdentifier aCspId,
                              const TDesC& aPsw,
                              const TDesC& aClientId,
                              TUint32 aAP,
                              const TDesC* aKey1 = NULL,
                              const TDesC* aKey2 = NULL ) = 0;

        /**
         * @see RImpsAccessClient2
         */
        virtual TInt LogoutL() = 0;

    protected:  // New functions

        /**
        * Destructor.
        */
        virtual ~MCAImpsAccessClient() { };
    };

#endif      // MCAIMPSACCESSCLIENT_H

// End of File
