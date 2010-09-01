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
* Description:  Authorization out transaction handler.
*
*/

#ifndef __CPENGAUTHORIZATIONTRANSACTIONOUT_H__
#define __CPENGAUTHORIZATIONTRANSACTIONOUT_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"

// CLASS DECLARATION
class MPEngAuthorizationEngine;
class CPEngAuthorizationResponse;
class CPEngTransactionStatus;
class MPEngXMLParser;
class MPEngXMLSerializer;
class MPEngPresenceAttrManager;


/**
 * Authorization out transaction handler.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngAuthorizationTransactionOut : public CBase,
            public MPEngOutgoingTransactionHandler
    {
    public:

        /**
         * Two-phased constructor.
         */
        static CPEngAuthorizationTransactionOut* NewLC(
            MPEngAuthorizationEngine& aAuthEngine,
            MPEngPresenceAttrManager& aAttrManager,
            MPEngXMLParser& aXMLParser,
            TPEngWVCspVersion& aCSPVersion,
            TInt aOperationId );

        /**
         * Destructor.
         */
        virtual ~CPEngAuthorizationTransactionOut();


    public: // From MPEngOutgoingTransactionHandler

        /**
         * Synchronous method, get Outgoing transaction request
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void RequestL( TDes8& aSendBuffer );


        /**
         * Signals to the transaction handler thst it is last
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void LastRunningTransactionHandler();


        /**
         * Process the response to the request.
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void ProcessResponseL( const TDesC8& aResponse,
                               TRequestStatus& aStatus );

        /**
         * Cancels asynchronous processing of the request
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void CancelProcessing();


        /**
         * Support for simultaneous transaction handling
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void NewTransactionHandlersL(
            RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );

        /**
         * Function to signal completing of the transaction
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        TBool TransactionCompleted();


        /**
         * Gets transaction status result class
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        CPEngTransactionStatus* TransactionResult( );


        /**
         * Releases the handler
         * @see <MPEngOutgoingTransactionHandler.h>
         */
        void ReleaseHandler();


    public:  // Set authorization response

        /**
         * Set authorization response
         * Function takes ownership
         *
         * @since 3.0
         * @param aResponse authorization response
         */
        void SetAuthResponse( CPEngAuthorizationResponse* aResponse );



    private: // XML message serializers

        /**
         * Append XML request to responde to authorization
         *
         * @since 3.0
         * @param aXmlSerializer XML serializer
         */
        void AppendXMLPresenceAuthUserL( MPEngXMLSerializer& aXmlSerializer );


        /**
         * Append XML request to Cancel authorization
         *
         * @since 3.0
         * @param aXmlSerializer XML serializer
         */
        void AppendXMLCancelAuthorizationL( MPEngXMLSerializer& aXmlSerializer );


        /**
         * Append in XML message User Id
         *
         * @since 3.0
         * @param aXmlSerializer XML serializer
         */
        void AppendXMLUserIdL( MPEngXMLSerializer& aXmlSerializer );


    private: // Constructors

        /**
         * C++ constructor.
         */
        CPEngAuthorizationTransactionOut(
            MPEngAuthorizationEngine& aAuthEngine,
            MPEngPresenceAttrManager& aAttrManager,
            MPEngXMLParser& aXMLParser,
            TPEngWVCspVersion& aCSPVersion,
            TInt aOperationId );


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

        /// REF: CSP Version
        TPEngWVCspVersion&                      iCSPVersion;

        /// OWN: Transaction completed
        TBool                                   iTransCompleted;

        /// OWN: Authorization response
        CPEngAuthorizationResponse*             iResponse;

        /// OWN: Transaction status class
        CPEngTransactionStatus*                 iTransactionStatus;

        /// OWN: Operation Id
        TInt                                    iOperationId;

    };

#endif      //  __CPENGAUTHORIZATIONTRANSACTIONOUT_H__



//  End of File

