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
* Description:  Transaction factory interface.
*
*/

#ifndef __MPENGTRANSACTIONFACTORY_H__
#define __MPENGTRANSACTIONFACTORY_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPEngCSPInfo;
class MPEngOutgoingTransactionHandler;
class MPEngIncomingTransactionHandler;



// CONSTANTS


// CLASS DECLARATION
/**
 * Abstract interface for transaction factories.
 *
 * Transaction factories are used to create concrete
 * transaction handlers. Presence engine subcomponents
 * like ContactListLibrary and AttributeLibrary
 * implement concrete transaction factories and
 * concrete transaction handlers.
 *
 * @since 2.1
 */
class MPEngTransactionFactory
    {
    public: // New functions

        /**
         *	Clean up stack support for the factory
         *
         *	Transaction factory will be treated as reference counted
         *	singleton.
         *	@since 3.0
         */
        virtual void Close( void ) = 0;


        /**
         *	Factory method to create handler for incoming data
         *
         *	When incoming message from the Network server arrives, it is
         *	offered to all transaction factories over this interface to
         *	process the data.
         *	If data requires processing, it is done by the Transaction
         *	handler returned by this function call.
         *	If there is no need to process data by the factory, NULL
         *	pointer is returned instead.
         *
         *	Incoming data is passed to the factory as plain XML as it was
         *	retrieved from the network.
         *	Created Incoming request handler will be then run by
         *	the presence Server to process it.
         *
         *  @since 3.0
         *  @param	aIncomingRequest XML data as it was retrieved from the
         *				network server.
         *  @return NULL for no processing, or instance of the
         *				incoming transaction handler
         */
        virtual MPEngIncomingTransactionHandler* IncomingTransactionHandlerL(
            const TDesC8& aIncomingRequest ) = 0;



        /**
         *	Factory method to create outgoing handler(s)
         *
         *	This method is called to retrieve handlers for outgoing
         *	operation.
         *	First parameter defines transaction operation from which
         *	factory shall understand if request concerns it.
         *	There is optional parameter which caries data needed
         *	for the operation procession. How ever it can be NULL
         *	if transaction does not require it.
         *	Once transaction handler is created and appended into the
         *	passed array, owner ship of the handler is taken by
         *	the caller of the function.
         *
         *  @since 3.0
         *  @param	aOperation enumeration of the operation
         *  @param	aData optional data needed for operation
         *  @param	aHandlers array where newly created handlers
         *				are stored
         */
        virtual void OutgoingTransactionHandlerL(
            TInt aOperation,
            const TDesC& aData,
            RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers ) = 0;



    protected:  //Destructor

        /**
         * Virtual inline destructor.
         *
         * Concrete factories can't be destroyed
         * using this interface.
         *
         * (Transaction Factories must be implemented as ref counted
         *	singletons.)
         */
        virtual ~MPEngTransactionFactory() {};
    };

#endif      //  __MPENGTRANSACTIONFACTORY_H__

//  End of File

