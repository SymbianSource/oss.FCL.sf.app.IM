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
* Description:  Reactive authorization transaction manager.
*
*/

#ifndef __CPENGAUTHORIZATIONTRANSFACTORY_H__
#define __CPENGAUTHORIZATIONTRANSFACTORY_H__


//  INCLUDES
#include <e32base.h>
#include "CPEngAuthorizationEngine.h"
#include "MPEngTransactionFactory.h"
#include "PEngWVCspVersion.h"


// FORWARD DECLARATIONS
class   MPEngStorageManager;
class   CPEngContactListSettings;
class   CPEngContactListModBase;
class   MPEngListLibFactory;
class   CPEngSessionSlotId;
class   MPEngXMLParser;
class   MPEngPresenceAttrManager;


// CLASS DECLARATION

/**
 * Reactive authorization transaction manager.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationTransFactory : public  CPEngAuthorizationEngine,
            public  MPEngTransactionFactory

    {
    public:  // Two-phased constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngAuthorizationTransFactory* NewLC(
            MPEngListLibFactory& aFactory,
            const CPEngSessionSlotId& aSessionId,
            TPEngWVCspVersion& aCSPVersion );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationTransFactory();


    public:  // Functions from MPEngTransactionFactory

        /**
         * Clean up stack support for the factory
         * @see <MPEngTransactionFactory.h>
         */
        void Close();


        /**
         * Factory method to create handler for incoming data
         * @see <MPEngTransactionFactory.h>
         */
        MPEngIncomingTransactionHandler* IncomingTransactionHandlerL(
            const TDesC8& aIncomingRequest );


        /**
         * Factory method to create outgoing handler(s)
         * @see <MPEngTransactionFactory.h>
         */
        void OutgoingTransactionHandlerL(
            TInt aOperation,
            const TDesC& aData,
            RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );


    private: // new private functions

        /**
         * Create Transaction handlers for authorization response
         *
         * @since 3.0
         * @param aData packed transactions
         * @param aHandlers array to store handlers
         */
        void CreateAuthResponseHandlersL(
            const TDesC& aData,
            RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers  );


    private: // constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationTransFactory( MPEngListLibFactory& aFactory,
                                        TPEngWVCspVersion& aCSPVersion );

        /**
         * Symbian constructor
         */
        void ConstructL( const CPEngSessionSlotId& aSessionId );


    private: // data

        /// REF: CSP Version
        TPEngWVCspVersion&          iCSPVersion;


        /// OWN: XML parser
        MPEngXMLParser*             iXMLParser;

        /// OWN: Attribute manager
        MPEngPresenceAttrManager*   iAttributeManager;

        /// OWN: Size for responses
        TInt                        iSize;
    };

#endif      //  __CPENGAUTHORIZATIONTRANSFACTORY_H__

//  End of File






















