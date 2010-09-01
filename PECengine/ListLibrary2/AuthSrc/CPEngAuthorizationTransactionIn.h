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
* Description:  Authorization in transaction handler.
*
*/

#ifndef __CPENGAUTHORIZATIONTRANSACTIONIN_H__
#define __CPENGAUTHORIZATIONTRANSACTIONIN_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngIncomingTransactionHandler.h"

// CPS Version
#include    "PEngWVCspVersion.h"


// FORWARD DECLARATION
class MPEngAuthorizationEngine;
class MPEngPresenceAttrManager;
class CPEngAuthorizationResponse;
class CPEngAuthorizationRequest;
class MPEngXMLParser;

// CLASS DECLARATION
/**
 * Authorization in transaction handler.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationTransactionIn : public CBase,
            public MPEngIncomingTransactionHandler
    {
    public: // Enumerations

        /**
         * In authorization sub types.
         */
        enum TPEngAuthorizationTransType
            {
            EPEngAuthorizationRequest   = 1,
            EPEngAuthorizationResponse  = 2
            };


    public:

        /**
         * Two-phased constructor.
         */
        static CPEngAuthorizationTransactionIn* NewL(
            MPEngAuthorizationEngine& aAuthEngine,
            MPEngPresenceAttrManager& aAttrManager,
            MPEngXMLParser& aXMLParser,
            const TDesC8& aParseBlock,
            TPEngAuthorizationTransType aType );

        /**
         *  Two-phased constructor.
         */
        static CPEngAuthorizationTransactionIn* NewLC(
            MPEngAuthorizationEngine& aAuthEngine,
            MPEngPresenceAttrManager& aAttrManager,
            MPEngXMLParser& aXMLParser,
            const TDesC8& aParseBlock,
            TPEngAuthorizationTransType aType );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationTransactionIn();


    public: // From MPEngIncomingTransactionHandler

        /**
         * Process the incoming request.
         * @see <MPEngIncomingTransactionHandler.h>
         */
        void ProcessRequestL( const TDesC8& aRequest,
                              TRequestStatus& aStatus );

        /**
         * Cancels asynchronous processing of the request
         * @see <MPEngIncomingTransactionHandler.h>
         */
        void CancelProcessing();

    private: // new helpers

        /**
         * Parse User id from the message
         *
         * @since 3.0
         * @return user id buffer
         */
        HBufC* ParseUserIdLC();


        /**
         * Parse Attribute list
         *
         * @since 3.0
         * @param aAuthItem authorization item to add attributes
         * @param aAddFunc function called on item to add attributes
         */
        template <class AuthItem>
        void ParseAttrListL( AuthItem& aAuthItem, void ( AuthItem::* aAddFunc )( TUint32 ) );


        /**
         * Parse authorization status
         *
         * @since 3.0
         * @param aAuthStatus authorization status
         */
        void ParseAuthorizationStatusL( CPEngAuthorizationResponse& aAuthStatus );


    private: // Constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationTransactionIn(
            MPEngAuthorizationEngine& aAuthEngine,
            MPEngPresenceAttrManager& aAttrManager,
            MPEngXMLParser& aXMLParser,
            const TDesC8& aParseBlock,
            TPEngAuthorizationTransType aType );


        /**
         * Symbian constructor.
         */
        void ConstructL();


    private:    // Data

        /// REF: Authorization engine
        MPEngAuthorizationEngine&               iAuthEngine;

        /// REF: Attribute manager
        MPEngPresenceAttrManager&               iAttributeManager;

        /// REF: XML Parser
        MPEngXMLParser&                         iXMLParser;

        /// REF: Block to parse
        TPtrC8                                  iParseBlock;

        /// OWN: Type of response to parse
        TPEngAuthorizationTransType             iResponseType;

    };

#endif      //  __CPENGAUTHORIZATIONTRANSACTIONIN_H__




//  End of File

