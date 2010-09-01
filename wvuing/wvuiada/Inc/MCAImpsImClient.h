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
* Description:  Abstract interface for instant messaging services (RImpsIMClient)
 *
*/



#ifndef MCAIMPSIMCLIENT_H
#define MCAIMPSIMCLIENT_H

//  INCLUDES
#include "MCAImpsClient.h"
#include <ImpsImCli.h>

// CLASS DECLARATION

/**
 *  Interface for instant messaging services.
 *  Same interface as RImpsImClient.
 *  Comments are copied from RImpsImClient.
 *
 *  @lib CAAdapter.dll
 *  @since 3.0
 */
class MCAImpsImClient : public MCAImpsClient
    {
    public: // New functions

        /**
         * @see RImpsImClient2
         */
        virtual void RegisterL(
            MImpsImHandler2* aImpsObserver,
            MImpsBlockingHandler2* aBlockingHandler,
            TUid aApplicationId,
            TInt aPriority = 0 ) = 0;

        /**
         * @see RImpsImClient2
         */
        virtual void Unregister() = 0;

        // MESSAGING

        /**
         * @see RImpsImClient2
         */
        // This is an adapter to IMPS Engine API, we cannot
        // influence the const descriptor pointer parameter passing
        // in the API.
        virtual TInt SendTextMessageL(  const TDesC* aSenderSn, // CSI: 17 # See description
                                        const MDesCArray* aUserIds,
                                        const TDesC* aGroupId, // CSI: 17 # See description
                                        const MDesCArray* aScreenNames,
                                        const TDesC& aContent,
                                        TBool aDeliveryReportWanted ) = 0;

        /**
         * @see RImpsImClient2
         */
        // This is an adapter to IMPS Engine API, we cannot
        // influence the const descriptor pointer parameter passing
        // in the API.
        virtual TInt SendContentMessageL( const TDesC* aSenderSn, // CSI: 17 # See description
                                          const MDesCArray* aUserIds,
                                          const TDesC* aGroupId,  // CSI: 17 # See description
                                          const MDesCArray* aScreenNames,
                                          const TDesC& aContentType,
                                          const TDesC8& aContent,
                                          TBool aDeliveryReportWanted ) = 0;


        /**
         * @see RImpsImClient2
         */
        virtual TInt BlockEntityRequestL(
            const CDesCArray* aBlockEntity,
            const CDesCArray* aUnBlockEntity,
            TBool aBlockedListInUse,
            const CDesCArray* aGrantedEntityList,
            const CDesCArray* aUnGrantEntity,
            TBool aGrantedListInUse ) = 0;

        /**
         * @see RImpsImClient2
         */
        virtual TInt GetBlockedListRequestL() = 0;

        /**
         * Returns actual WV Engine interface class pointer which is registered.
         * @since 3.0
         * @return RImpsImClient pointer which is registered and can be used.
         *  Ownership NOT transferred.
         */
        virtual RImpsImClient2* InterfaceL() = 0;

        /**
         * Get the maximum TransactionContent size that WV engine
         * and WV server can handle, related to ParserSize.
         * Note that if service negotiation is not finished, this
         * method returns 0.
         *
         * @return size or zero
         * @since 3.0
         */
        virtual TInt MaxTransactionContentLengthL( ) = 0;

    protected:  // New functions

        /**
         * Destructor.
         */
        virtual ~MCAImpsImClient() { };
    };

#endif      // MCAIMPSIMCLIENT_H

// End of File
