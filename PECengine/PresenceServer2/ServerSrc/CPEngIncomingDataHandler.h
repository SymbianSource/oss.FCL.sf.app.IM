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
* Description:  Incoming data handler
*                it handles incoming data from the network server
*
*/



#ifndef __CPENGINCOMINGDATAHANDLER_H__
#define __CPENGINCOMINGDATAHANDLER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPEngPureDataHandler;
class MPEngIncomingTransactionHandler;
class MPEngTransactionFactory;
class MPEngRequestHandlerObserver;

// CLASS DECLARATION
/**
 *  Incoming data handler
 *  it handles incoming data from the network server
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngIncomingDataHandler ) :
        public CActive
    {
public:  // Constructors and destructor

    /**
     *  Two-phased constructor.
     *  @since 3.0
     *  @param aPureDataHandler pure data handler
     *  @param aTransactionFactories transaction factories od CPS session
     *  @param aRequestHandlerObserver request handler observer
     */
    static CPEngIncomingDataHandler* NewL(
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngTransactionFactory>&
        aTransactionFactories,
        MPEngRequestHandlerObserver& aRequestHandlerObserver );

    static CPEngIncomingDataHandler* NewLC(
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngTransactionFactory>&
        aTransactionFactories,
        MPEngRequestHandlerObserver& aRequestHandlerObserver );

    /**
     *  Destructor.
     */
    virtual ~CPEngIncomingDataHandler();

public: // New functions

    /**
     *  Start listener
     *
     *  @since 3.0
     */
    void StartListeningL();

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

private:  // New functions
    /**
     * Handle new incoming data from the VW server
     * @since 3.0
     */
    void HandleIncomingDataL();


private:

    /**
     * C++ default constructor.
     */
    CPEngIncomingDataHandler(
        MPEngPureDataHandler& aPureDataHandler,
        RPointerArray<MPEngTransactionFactory>&
        aTransactionFactories,
        MPEngRequestHandlerObserver& aRequestHandlerObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data


    /// REF: Pure data handler
    MPEngPureDataHandler&						iPureDataHandler;

    /// REF: Transaction factories
    RPointerArray<MPEngTransactionFactory>&		iTransactionFactories;

    /// REF: Request handler observer
    MPEngRequestHandlerObserver&				iRequestHandlerObserver;

    /// OWN: Incoming transaction handler
    MPEngIncomingTransactionHandler*			iTransactionHandler;
    };

#endif      // __CPENGINCOMINGDATAHANDLER_H__

// End of File
