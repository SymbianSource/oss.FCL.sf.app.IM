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
* Description:  Outgoing transaction handling interface.
*                Defines a protocol between Presence Engine server
*                and concrete transaction handlers to generate outgoing
*                XML to network and process the responce.
*
*/

#ifndef __MPENGOUTGOINGTRANSACTIONHANDLER_H__
#define __MPENGOUTGOINGTRANSACTIONHANDLER_H__

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class CPEngTransactionStatus;



// CLASS DECLARATION
/**
 * Outgoing transaction handling interface.
 *
 * Defines a protocol between Presence Engine server
 * and concrete transaction handlers to generate outgoing
 * XML to network and process the response.
 *
 * Methods are listed here in their "calling order".
 *
 * @since 3.0
 */
class MPEngOutgoingTransactionHandler
    {
    public: // New functions

        /**
         *	Synchronous method, get Outgoing transaction request
         *
         *	This method is called to generate a outgoing XML request
         *	for the presence network server
         *	request is no further processed and it is send to network
         *
         *  @since 3.0
         *  @param aSendBuffer The send buffer to write XML request.
         *        Buffer has the maximum send buffer size available.
         *        Handler is responsible to divide too big requests
         *        to smaller partitions, that fit into available
         *        send buffer. In case of separation, handler must
         *        utilize incremental sending, see method
         *        OutgoingTransactionComplete().

         */
        virtual void RequestL( TDes8& aSendBuffer ) = 0;


        /**
         *	Signals to the transaction handler thst it is last
         *	active transaction handler in the scope of the client request
         *
         *	This function is called by the transaction handler runner
         *	when there is no more running transaction handler in
         *	the scope of one client request, same as this transaction
         *	handler is running.
         *	However Request handler can still multiply itself
         *	or continue running in loops as single instance.
         *
         *	@since 3.0
         */
        virtual void LastRunningTransactionHandler( ) = 0;

        /**
         *	Process the response to the request.
         *
         *	This method is called to process XML request response
         *	from network server.
         *	Request can be processed either in synchronous way
         *	there passed TRequestStatus is not touched by the handler
         *	or when state is changed to pending, and handler will not be
         *	deleted till its completion.
         *
         *
         *	@since 3.0
         *	@param aResponse buffer containing XML request response
         *	@param aStatus status where transaction signals completion
         */
        virtual void ProcessResponseL( const TDesC8& aResponse,
                                       TRequestStatus& aStatus ) = 0;

        /**
         *	Cancels asynchronous processing of the request
         *
         *	When processing of the request is not asynchronous
         *	this function does nothing
         *
         *  @since 3.0
         *  @param
         *  @return
         */
        virtual void CancelProcessing( ) = 0;

        /**
         *	Support for simultaneous transaction handling.
         *
         *	This method is called by the transaction handler
         *	runner to ask if there is need to created extra
         *	transaction handlers which will be run separately but
         *	still withing the scope of the clients request this
         *	request handler is belonging.
         *
         *  @since 3.0
         *	@param aTransactionsArray Reference to the pointer array
         *        where new transaction handlers can be stored.
         */
        virtual void NewTransactionHandlersL(
            RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers ) = 0;


        /**
         *	Function to signal completing of the transaction
         *
         *	Runner of the transaction handler is using this function
         *	to check if handler needs to send another data to the
         *	network server.
         *	It is called one processing of the request response is
         *	finished.
         *
         *  @since 3.0
         *  @param
         *  @return ETrue if transaction is completed and does not need
         *		to create any new request
         */
        virtual TBool TransactionCompleted( ) = 0;

        /**
         *	Gets transaction status result class
         *
         *	Transaction handler returns instance of the transaction
         *	status class where are details about finished transaction.
         *	This function is called once transaction handler is completed
         *	Ownership is transfered to the caller
         *
         *  @since 3.0
         *  @return instance of the transaction status with ownership
         */
        virtual CPEngTransactionStatus* TransactionResult( ) = 0;

        /**
         *	Releases handler
         *
         *	This function is called when some Symbian system error will
         *	occur, so handler has chance to do cleaning if this is
         *	needed.
         *	This function is called when for example processing of the
         *	incoming date failed and left with some error
         *	It is also called  when send data to network failed
         *	How ever, handle is not informed about particular error
         *	and its not event its concern
         *
         *  @since 3.0
         */
        virtual void ReleaseHandler( ) = 0;

    public:  //Destructor

        /**
         * Virtual inline destructor.
         *
         * Concrete transaction handlers can be
         * destroyed using this inteface.
         */
        virtual ~MPEngOutgoingTransactionHandler() {};
    };

#endif      //  __MPENGOUTGOINGTRANSACTIONHANDLER_H__

//  End of File

