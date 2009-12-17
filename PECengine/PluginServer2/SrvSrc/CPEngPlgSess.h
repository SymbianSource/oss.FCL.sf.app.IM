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
* Description:  See class description below.
*
*/


#ifndef __CPENGPLGSESS_H__
#define __CPENGPLGSESS_H__

//  INCLUDES
#include <e32def.h>
#include "MPEngplgSrv.h"

//  CONSTANTS
_LIT( KSessionName, "PlgSrvSession" );

//  FORWARD DECLARATIONS
class CPEngPluginStatus;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    This is session class for cache server. Session receives service requests
	from client. Session is responsible to handle received requests.

-----------------------------------------------------------------------------
*/
class CPEngPlgSess : public CSession2
    {
    public:		// Constructors and destructors

        static CPEngPlgSess* NewL( MPEngPlgSrv& aServer );	// Static constructor
        virtual ~CPEngPlgSess();				// Destructor (virtual)

    protected:	// Constructors and destructors

        /**
         * Default constructor, protected to allow derivation
         *
         * @param aServer reference to the server instance
         */
        CPEngPlgSess( MPEngPlgSrv& aServer );

    private:	// New methods

        /**
         * Handles the message
         *
         * @param aMessage is received message.
         * @return TBool ETrue if message should be completed
         *               in ServiceL
         */
        TBool DispatchMessageL( const RMessage2 &aMessage );

        /*
         *	Client is panicked using this method.
         */
        void PanicClient( const RMessage2& aMessage, const TInt aPanic ) const;

    public:		// Methods derived from CSession2

        /**
         * Handles the servicing of client requests to the server.
         *
         * @param aMessage is received message.
         */
        void ServiceL( const RMessage2 &aMessage );

        /**
         * Handles service error that could happen in ServiceL.
         *
         * @param aMessage is received message.
         * @param aError is the error value.
         */
        void ServiceError( const RMessage2& aMessage, TInt aError );

    private:	// Methods not implemented

        CPEngPlgSess( const CPEngPlgSess& );		// Copy constructor
        CPEngPlgSess& operator=( const CPEngPlgSess& );	// Assigment operator

    private:	// Data

        /// not owned, pointer to server class
        MPEngPlgSrv& iPlgServer;
    };

#endif      //  __CPENGPLGSESS_H__
