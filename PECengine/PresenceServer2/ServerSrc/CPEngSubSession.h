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
* Description:  Presence Server sub-session handler implementation
*				 All clients sub-session related request are handler here
*
*/



#ifndef __CPENGSUBSESSION_H__
#define __CPENGSUBSESSION_H__

//  INCLUDES
#include	<e32base.h>
#include	<badesca.h>

#include	"MPEngSubSession.h"

#include	"TPEngServerMessages.h"

// FORWARD DECLARATIONS
class	MPEngServer;
class	RPEngMessage;
class	CPEngCSPSessManager;

// CLASS DECLARATION
/**
 *  Presence Server sub-session handler implementation
 *  All clients sub-session related request are handler here,
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngSubSession ) :
        public CObject,
        public MPEngSubSession
    {
public:  // Constructors and destructor

    /**
     *  Two-phased constructor.
     *  @param aServer server instance
     *  @param aMessage create sub-session message
     *  @param aSessionId Id of the main session
     *  @return sub-session instance
     */
    static CPEngSubSession* NewL( MPEngServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId );

    static CPEngSubSession* NewLC( MPEngServer& aServer,
                                   const RPEngMessage& aMessage,
                                   TInt32 aSessionId );

    /**
    * Destructor.
    */
    virtual ~CPEngSubSession();

public: // New functions of the MPengSubSession
    /**
     *	Store request response
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    void StoreRequestResponse( HBufC8* aResponse );

    /**
     *  Application Id of the sub-session
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    TDesC& AppId();

    /**
     *  Handler successful application Id login
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    void HandleLogInL();

    /**
     *  Handler successful application Id logOut
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    void HandleLogOut();

    /**
     *  Disconnect Application Id
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    void DisconnectAppId( const TDesC& aAppId );

    /**
     *  Handle CSP closing
     *  @see <MPengSubSession.h>
     *	@since 3.0
     */
    void CSPSessionClosed();

public: // Function for handling of the clients requests

    /**
      * Dispatch message from the Sub Session
      *
      *	@since 3.0
      *	@param aMessage message with the request details
      *  @param aRequest request enumeration to handle
      *	@return ETrue if message should be completed
      */
    TBool DispatchMessageL( const RPEngMessage& aMessage,
                            TInt aRequest );

    /**
      * Set Sub-session handle Id for the Session
      *
      *	@since 3.0
      *	@param aHandle Sub-session handle
      */
    void SetSubSesionHandle( TInt aHandle );


private:  // Functions from base classes

    /*
     *	Panic client
     *
     *	@since 3.0
     *	@param	aMessage clients message
     *	@param	aPanic panic reason
     */
    void PanicClient( const RPEngMessage& aMessage,
                      const TInt aPanic ) const;

    /**
     *  Fetch result of last asynchronous request
     *
     *  @since 3.0
     *  @param aMessage with request details
     */
    void FetchRequestresultL( const RPEngMessage& aMessage );

private:

    /**
    * C++ default constructor.
    */
    CPEngSubSession( TInt32 aSessionId  );

    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL( MPEngServer& aServer,
                     const RPEngMessage& aMessage );

private:    // Data

    /// OWN: Sub-session type
    TInt								iType;

    /// OWN: Sub Session Handle
    TInt								iHandle;

    /// OWN: Session Id of this sub-session
    TInt32								iSessionId;

    /// REF: CSP Session holder
    CPEngCSPSessManager*				iSessionManager;

    /// OWN: Application Id of the sub-session
    HBufC16*							iAppId;

    /// OWN: Application Id was attached to Session Holder
    TBool								iAppIdAttached;

    /// OWN: Sub-session last async request result
    HBufC8*								iRequestResult;

    };

#endif      // __CPENGSUBSESSION_H__

// End of File
