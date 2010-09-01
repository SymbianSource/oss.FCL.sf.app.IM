/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network operations for chat access handling.
*
*/


#ifndef CCAACCESSMANAGER_H
#define CCAACCESSMANAGER_H

// INCLUDES
#include "MCAImpsAccessClient.h"
#include "MCAAccessInterface.h"
#include "PrivateEngineDefinitions.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CCARequestMapper;

// CLASS DECLARATION

/**
 *  Network operations for chat group handling.
 *
 *  @lib CAEngine.lib
 *  @since 3.0
 */
class CCAAccessManager : public CBase,
            public MImpsAccessHandler2,
            public MImpsErrorHandler2,
            public MCAAccessInterface
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAAccessManager* NewL( CCARequestMapper& aRequestMapper );

        /**
         * Destructor.
         */
        virtual ~CCAAccessManager();

    protected: // From MImpsErrorHandler

        /**
         *	From MImpsErrorHandler
         *  @see MImpsErrorHandler::HandleErrorL
         */
        void HandleErrorL(
            TInt aStatus,
            TInt aOpId,
            const TDesC* aDescription,
            const CImpsDetailed* aDetailedRes,
            TImpsCspIdentifier& aCspId );


    private: // From MCAAccessInterface

        /**
         * @see MCAAccessInterface
         */
        void LoginL(   TImpsCspIdentifier& aCspId,
                       const TDesC& aPsw,
                       const TDesC& aClientId,
                       TUint32 aAP,
                       const TDesC* aKey1 = NULL,
                       const TDesC* aKey2 = NULL );

        /**
         * @see MCAAccessInterface
         */
        void LogoutL();

    private: // From MImpsAccessHandler2

        /**
         * @see MImpsAccessHandler2:
         */
        void HandleLoginL( TInt aId,
                           TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsAccessHandler2:
         */
        void HandleLogoutL( TInt aId,
                            TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsAccessHandler2:
         */
        void HandleLoginCancelL( TInt aCancelledOpId,
                                 TImpsCspIdentifier& aCspId );


        /**
         * @see MImpsAccessHandler2:
         */
        void HandleNbrSessionsL( TInt aId,
                                 TInt aNbr,
                                 TImpsCspIdentifier& aCspId );

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * C++ default constructor.
         * @see CCAGroupManager::NewL.
         */
        CCAAccessManager( CCARequestMapper& aRequestMapper );

        /**
         * Execute requests
         */
        TInt ExecuteRequestL( TInt aOperationId,
                              TOperationRequests aRequestType,
                              MDesCArray* aAdditionalData = NULL );

    private: // Data

        /// Access client for access management.
        MCAImpsAccessClient* iImpsAccessClient; // does not own

        /// Does not own.
        CCARequestMapper& iRequestMapper;
    };

#endif      // CCAGROUPMANAGER_H

// End of File
