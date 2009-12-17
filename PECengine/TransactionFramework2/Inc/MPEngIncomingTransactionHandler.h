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
* Description:  Incoming transaction handling interface.
*                Defines a protocol between Presence Engine server
*                and concrete transaction handlers to process
*                incoming XML from network.
*
*/

#ifndef __MPENGINCOMINGTRANSACTIONHANDLER_H__
#define __MPENGINCOMINGTRANSACTIONHANDLER_H__


//  INCLUDES
#include <e32std.h>




// CLASS DECLARATION
/**
 * Incoming transaction handling interface.
 *
 * Defines a protocol between Presence Engine server
 * and concrete transaction handlers to process
 * incoming XML from network.
 *
 * Methods listed here are in their "calling order".
 *
 * @since 2.1
 */
class MPEngIncomingTransactionHandler
    {
    public: // New functions

        /**
         *  Process the incoming request.
         *
         *  This method is called to process incoming XML request
         *  from network server.
         *  Request can be processed either in synchronous way
         *  there passed TRequestStatus is not touched by the handler
         *  or when state is changed to pending, and handler will not be
         *  deleted till its completion.
         *
         *
         *  @since 3.0
         *  @param aRequest buffer containing incoming XML request.
         *  @param aStatus status where transaction signals completion
         */
        virtual void ProcessRequestL( const TDesC8& aRequest,
                                      TRequestStatus& aStatus ) = 0;

        /**
         *  Cancels asynchronous processing of the request
         *
         *  When processing of the request is not asynchronous
         *  this function does nothing
         *
         *  @since 3.0
         *  @param
         *  @return
         */
        virtual void CancelProcessing( void ) = 0;

    public:  //Destructor
        /**
         * Virtual inline destructor.
         *
         * Concrete transaction handlers can be
         * destroyed using this interface.
         */
        virtual ~MPEngIncomingTransactionHandler() {};

    };
#endif      //  __MPENGINCOMINGTRANSACTIONHANDLER_H__


//  End of File

