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
* Description:  Container of the one outgoing transaction handler
*				  It process one handler until it is completed
* 				  canceled or some error ocures.
* 				  Its completion is signaled to the observer
*
*/



#ifndef __CPENGOUTGOINGTRANSCONTAINER_H__
#define __CPENGOUTGOINGTRANSCONTAINER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPEngHandlerSendData;
class MPEngPureDataHandler;
class MPEngOutgoingTransactionHandler;


// CLASS DECLARATION
/**
 *  Container of the one outgoing transaction handler
 *  It process one handler until it is completed
 *  canceled or some error ocures.
 *  Its completion is signaled to the observer
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngOutGoingTransContainer ) :
        public CActive
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @since 3.0
     * @param aRequestHandler request handler,to signal completion
     * @param aPureDataHandler pure data handler
     * @param aTransactionHandlersArray transaction array
     *			to store new transaction
     * @return transaction container instance
     */
    static CPEngOutGoingTransContainer* NewL(
        MPEngHandlerSendData& aRequestHandler,
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngOutgoingTransactionHandler>&
        aTransactionHandlersArray );

    static CPEngOutGoingTransContainer* NewLC(
        MPEngHandlerSendData& aRequestHandler,
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngOutgoingTransactionHandler>&
        aTransactionHandlersArray );

    /**
     * Destructor.
     */
    virtual ~CPEngOutGoingTransContainer();

public: // New functions

    /**
     *  Start Handeling of the transaction
     *
     *  @since 3.0
     *  @param	transaction handler, ownership is taked by the function
     *		it is safe to call it when transaction handler is not in
     *		the clean up stack
     */
    void StartTransactionL(
        MPEngOutgoingTransactionHandler* aOutgoingTransaction );

public:		// Function from the CActive

    /**
     * CActive RunL()
     * @since 3.0
     */
    void RunL();

    /**
     * CActive RunError()
     * @since 3.0
     * @param aError The leave code
     * @return KErrNone
     */
    TInt RunError( TInt aError );

    /**
     * Implements cancellation of an outstanding request.
     * This function is called as part of the active object’s Cancel()
     * @since 3.0
     */
    void DoCancel();

private:

    /**
     *  C++ default constructor.
     */
    CPEngOutGoingTransContainer(
        MPEngHandlerSendData& aRequestHandler,
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngOutgoingTransactionHandler>&
        aTransactionHandlersArray );

    /**
     *  By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data


    /// OWN: Pure data handler
    MPEngPureDataHandler&								iPureDataHandler;

    /// OWN: Outgoing transaction ID
    TInt												iTransactionId;

    /// REF: Request Handler
    MPEngHandlerSendData&								iRequestHandler;

    /// OWN: Outgoing transaction handler
    MPEngOutgoingTransactionHandler*					iTransactionHandler;

    ///  OWN: This array is used when Transaction needs to multiply
    ///       Elements REFerenced
    RPointerArray<MPEngOutgoingTransactionHandler>&	iNewTransactionHandlers;

    };

#endif      // __CPENGOUTGOINGTRANSCONTAINER_H__

// End of File
