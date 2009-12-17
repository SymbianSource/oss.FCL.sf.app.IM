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
* Description:  Request Handler to update data to the network server
*
*/



#ifndef __CPENGHANDLERSENDDATA_H__
#define __CPENGHANDLERSENDDATA_H__

//  INCLUDES
#include <e32base.h>

#include "MPEngRequestHandler.h"
#include "MPEngHandlerSendData.h"

//	Own RMessage class wraper
#include "RPEngMessage.h"


// FORWARD DECLARATIONS
class	MPEngRequestHandlerObserver;
class	MPEngPureDataHandler;
class	CPEngOutGoingTransContainer;
class	MPEngOutgoingTransactionHandler;
class	CPEngTransactionStatus;
class	MPEngSubSession;

// CLASS DECLARATION
/**
 *  Request Handler to update data to the network server
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngHandlerSendData ):
        public CBase,
        public MPEngRequestHandler,
        public MPEngHandlerSendData
    {
public:  // Constructors and destructor

    /**
     *	Two-phased constructor. which creates Send Data handler from
     *	Transaction handlers.
     *	Transaction handlers from the passed pointer array are removed
     *	and its owner ship is taked by the created class.
     *	Due to the nature of the construction, it can happen that only
     *	part of the transaction handler is removed from the array,
     *	before construction fails,however remained elements in the
     *	array have now relation to failed object, and removed ones
     *	have been destroied among failed contruction of
     *	the CPEngHandlerSendData object. This constructor can be called
     *	without valid client message. In that case, when handler
     *	is completed there is no client notify about it.
     *
     *  @since 3.0
     *  @param aRequestObserver	request handler observer
     *  @param aPureDataHandler	Pure data handler,
     *  @param aServSubSession sub session
     *  @param aTransactions outgoing transactions
     *  @param aMessage message to complete request to
     *  @param aSessionId session id
     *  @param aSubSessionId subsession id
     *  @return pointer to the constructed class
     */
    static CPEngHandlerSendData* NewLC(
        MPEngRequestHandlerObserver&  aRequestObserver,
        MPEngPureDataHandler& aPureDataHandler,
        MPEngSubSession& aServSubSession,
        const RPEngMessage& aMessage,
        RPointerArray<MPEngOutgoingTransactionHandler>& aTransactions,
        TInt32 aSessionId,
        TInt32 aSubSessionId );


    /**
     * Destructor.
     */
    ~CPEngHandlerSendData();

public: // Function from MPEngRequestHandler

    /**
     * Start Handling of the request
     * @see <MPEngRequestHandler.h>
     */
    void StartHandlerL();

    /**
     * Return Session Id
     * @see <MPEngRequestHandler.h>
     */
    TInt32 SessionId() const ;

    /**
     * Return Sub-Session Id
     * @see <MPEngRequestHandler.h>
     */
    TInt SubSessionId() const ;

    /**
     * Return Request function
     * @see <MPEngRequestHandler.h>
     */
    TInt RequestFunction() const;

    /**
     * Cancel Request
     * @see <MPEngRequestHandler.h>
     */
    void CancelRequestD();

    /**
     * Set message handler
     * @see <MPEngRequestHandler.h>
     */
    void SetMessage( const RPEngMessage& aMessage );


public: // Function from MPEngHandlerSendData

    /**
     *  Create new transaction containers and start their processing
     *	@see <MPEngHandlerSendData.h>
     *  @since 3.0
     */
    void CreateNewContainersRunThemL( ) ;

    /**
     *  Complete passed transaction container
     *	@see <MPEngHandlerSendData.h>
     *  @since 3.0
     */
    void CompleteTransContainer(
        CPEngOutGoingTransContainer* aContainer,
        CPEngTransactionStatus* aStatus,
        TInt aErrCode );
    /**
     *  Checks if this is last running transaction container
     *	@see <MPEngHandlerSendData.h>
     *  @since 3.0
     */
    TBool LastRunningTransaction( );

private:  // Functions from base classes

    /**
     *	Complete request handlers
     *	and delete Request Handler Class
     *	@since 3.0
     */
    void CompleteRequestD();

private:

    /**
     *	C++ default constructor.
     */
    CPEngHandlerSendData(
        MPEngRequestHandlerObserver& aRequestHandlerObserver,
        MPEngPureDataHandler& aPureDataHandler,
        MPEngSubSession& aServSubSession,
        const RPEngMessage& aMessage,
        TInt32 aSessionId,
        TInt32 aSubSessionId );


    /**
     *	Symbian 2nd phase constructor
     */
    void ConstructL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aTransactions );

private:    // Data

    /// REF: Request Handler Observer
    MPEngRequestHandlerObserver&					iRequestHandlerObserver;

    /// REF: Pure data handler
    MPEngPureDataHandler&							iPureDataHandler;

    /// REF: Server Sub-session
    MPEngSubSession&								iServSubSession;

    /// OWN: Request RPEngMessage
    RPEngMessage									iMessage;

    /// OWN: Session Id from where request was issued
    TInt32											iSessionId;

    /// OWN: SubSssion Id from where request was issued
    TInt											iSubSessionId;

    /// OWN: Request Function
    TInt32											iFunction;

    /// OWN: All running Outgoing Transaction Handler containers
    ///      Elements OWNed
    RPointerArray<CPEngOutGoingTransContainer>		iTrasactionContainersArray;

    /// OWN: Not activated Outgoing Transaction handlers
    ///      Elements OWNed
    RPointerArray<MPEngOutgoingTransactionHandler> iOutgoingTransactionHandlers;

    /// OWN: Transaction status
    CPEngTransactionStatus*							iTransactionStatus;
    };

#endif      // __CPENGHANDLERSENDDATA_H__

// End of File
