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
* Description:  Request handler abstract interface
*				 Defines basic functionality must be implemented
*  				 by request handler
*
*/



#ifndef __MPENGREQUESTHANDLER_H__
#define __MPENGREQUESTHANDLER_H__

//  INCLUDES

#include <e32def.h>

// FORWARD DECLARATION
class RPEngMessage;

/**
 *  Request handler abstract interface
 *  Defines basic functionality must be implemented
 *  by request handler
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
class MPEngRequestHandler
    {
    public: // New functions

        /**
         *	Return Session Id
         *	Returns Session Id from which request was issued
         *	This is used to track from which connected client request came
         *
         *	@since 3.0
         *	@return session ID
         */
        virtual TInt32 SessionId( ) const = 0;

        /**
         *	Return Sub Session Id
         *	Returns Sub Session Id from which request was issued
         *	This is used to track from which connect client request came
         *
         *	@since 3.0
         *	@return sub-session Id
         */
        virtual TInt SubSessionId( ) const = 0;

        /**
         *	Return Request function
         *	returns Request function of the request handler
         *
         *	@since 3.0
         *	@return request function
         */
        virtual TInt RequestFunction( ) const = 0;

        /**
         *	Cancel Request
         *
         *	Canceles asynchronous Request processing, request is
         *	completed to the client side and request handler
         *	is deleted
         *
         *
         *	@since 3.0
         */
        virtual void CancelRequestD( ) = 0;

        /**
         * Set message handler
         *
         * @since 3.0
         */
        virtual void SetMessage( const RPEngMessage& aMessage ) = 0;

    public:  //Destructor

        /**
          * Virtual inline destructor.
          *
          * Concrete Request Handler can be
          * destroyed using this interface.
          */
        virtual ~MPEngRequestHandler() {};

    };

#endif      // __MPENGREQUESTHANDLER_H__

// End of File
