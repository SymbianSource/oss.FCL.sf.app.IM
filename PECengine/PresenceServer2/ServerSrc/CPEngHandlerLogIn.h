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
* Description:  Log in Handler
* 				 Handles Log in for opening of the network session
*
*/



#ifndef __CPENGHANDLERLOGIN_H__
#define __CPENGHANDLERLOGIN_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngRequestHandler.h"

//	Own RMessage class wraper
#include "RPEngMessage.h"

// FORWARD DECLARATIONS
class MPEngRequestHandlerObserver;
class MPEngAccessHandler;
class MPEngSubSession;

// CLASS DECLARATION

/**
 *  Log in Handler
 *  Handles Log in for opening of the network session
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngHandlerLogIn ) :
        public CActive,
        public MPEngRequestHandler
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @since 3.0
     * @param aObserver Request handler observer
     * @param aAccessHandler access handler
     * @param aSubSession sub session
     * @param aMessage message to complete request to
     * @param aSessionId session id
     * @param aSubSessionId subsession id
     * @return Log in handler
     */
    static CPEngHandlerLogIn* NewL(
        MPEngRequestHandlerObserver& aObserver,
        MPEngAccessHandler& aAccessHandler,
        MPEngSubSession& aSubSession,
        const RPEngMessage& aMessage,
        TInt32 aSessionId,
        TInt32 aSubSessionId );

    static CPEngHandlerLogIn* NewLC(
        MPEngRequestHandlerObserver& aObserver,
        MPEngAccessHandler& aAccessHandler,
        MPEngSubSession& aSubSession,
        const RPEngMessage& aMessage,
        TInt32 aSessionId,
        TInt32 aSubSessionId );

    /**
     * Destructor.
     */
    virtual ~CPEngHandlerLogIn();

public: // New Functions

    /**
     * Start Handling of the request
     *
     * @since 3.1
     * @param aMessage message of the request
     */
    void StartHandlerL( const RPEngMessage& aMessage );


public: // Function from MPEngRequestHandler

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

public:		// Function from the CActive
    /**
     * CActive RunL()
     * @since 3.0
     */
    void RunL();

    /**
     * CActive RunError()
     * @since 3.0
     */
    TInt RunError( TInt aError );

    /**
     * Implements cancellation of an outstanding request.
     * @since 3.0
     */
    void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    CPEngHandlerLogIn(
        MPEngRequestHandlerObserver& aObserver,
        MPEngAccessHandler& aAccessHandler,
        MPEngSubSession& aSubSession,
        const RPEngMessage& aMessage,
        TInt32 aSessionId,
        TInt32 aSubSessionId );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    /// OWN: Request RPEngMessage
    RPEngMessage						iMessage;

    /// OWN: Server Session Id from where request came
    TInt32								iSessionId;

    /// OWN: Server Sub session Id from where request came
    TInt								iSubSessionId;

    /// OWN: Request Function
    TInt								iFunction;

    /// REF: Access handler
    MPEngAccessHandler&					iAccessHandler;

    /// REF: Request Handler Observer
    MPEngRequestHandlerObserver&		iObserver;

    /// REF: Sub-session from which request came
    MPEngSubSession&					iSubSession;

    };

#endif      // __CPENGHANDLERLOGIN_H__

// End of File
