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
* Description:  Presence Server session handler implementation
*				 All clients request are handler here,
*  				 or forwarded to the sub-session
*
*/


#ifndef __CPENGSESSION_H__
#define __CPENGSESSION_H__

//  INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class	MPEngServer;
class	RPEngMessage;
class	CPEngSubSession;

// CLASS DECLARATION
/**
 *  Presence Server session handler implementation
 *  All clients request are handler here,
 *  or forwarded to the sub-session
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS(   CPEngSession ):
        public CSession2
    {
public:		// Constructors and destructors

    /**
     * Two-phased constructor.
     * @param aServer server instance
     * @return instance of the session
     */
    static CPEngSession* NewL( MPEngServer& aServer );

    /**
     * Destructor.
     */
    virtual ~CPEngSession();

public:		// Methods derived from CSession

    /**
     *  Service clients request
     *  @see <CSession2.h>
     *	@since 3.0
     *  @param aMessage message with request details
     */
    void ServiceL( const RMessage2 &aMessage );

    /**
     *  Handles the service error
     *	@since 3.0
     *  @see <CSession2.h>
     *  @param aMessage message with request details
     *  @param aError error
     */
    void ServiceError( const RMessage2& aMessage, TInt aError );

private:	// New methods

    /**
     *  Dispatch clients message
     *
     *	@since 3.0
     *  @param aMessage clients message
     */
    TBool DispatchMessageL( const RPEngMessage& aMessage );

    /**
     *	Create new sub-session
     *
     *	@since 3.0
     *	@param aMessage clients message
     */
    void CreateNewSubSessionL( const RPEngMessage& aMessage );

    /**
     *	Remove sub-session
     *	@since 3.0
     *	@param	aHandle handle of the subsession
     */
    void RemoveSubSessionL( TUint  aHandle );

    /**
     *	Find Subsession
     *	@since 3.0
     *	@param	aHandle handle of the subsession to find
     *  @return sub-session instance
     */
    CPEngSubSession& FindSubSessionL( TUint  aHandle ) const;


    /*
     *	Panic client
     *	@since 3.0
     *  @param	aMessage clients message
     *  @param	aPanic panic reason
     */
    void PanicClient( const RPEngMessage& aMessage,
                      const TInt aPanic ) const;


protected:	// Constructors and destructors

    /**
      * C++ default constructor.
      */
    CPEngSession( MPEngServer& aServer );// Default constructor, protected to allow derivation

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:	// Data

    /// OWN: Collection of the Sub Sessions
    CObjectIx*							iSubSessions;

    /// REF: server clas
    MPEngServer&						iMainServer;
    };

#endif      //  __CPEngSession_H__
