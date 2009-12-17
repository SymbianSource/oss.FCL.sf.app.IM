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
* Description:  Request handler observer abstract interface
				 Completion of all asynchronous request completion is
*				 signaled here
*
*/



#ifndef __MPENGREQUESTHANDLEROBSERVER_H__
#define __MPENGREQUESTHANDLEROBSERVER_H__

//  INCLUDES
#include <e32def.h>


// FORWARD DECLARATIONS
class MPEngRequestHandler;

// CLASS DECLARATION
/**
 *  Request handler observer abstract interface
 *  Completion of all asynchronous request completion is
 *  signaled here
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
class MPEngRequestHandlerObserver
    {
    public: // New functions of RequestHandlerObserver

        /**
          *  Remove calling request handler from array of pending requests
          *
          *	This is called when request handler is completed at its
          *  destruction
          *
          *  @since 3.0
          *  @param aRequestHandler completed request handler to be removed
          */
        virtual void CompleteRequestHandler(
            MPEngRequestHandler* aRequestHandler ) = 0;

        /**
         *  Set CSP holder to the Open state
         *
         *	All resources needed for the Open state of the session
         *	are loaded
         *  No notification is sent!!
         *
         *  @since 3.0
         */
        virtual void SetCSPSessionOpenedL() = 0;

    protected:  //Destructor

        /**
          * Virtual inline destructor.
          *
          * Concrete Request Handler can be
          * destroyed using this interface.
          */
        virtual ~MPEngRequestHandlerObserver() {};

    };

#endif      // __MPENGREQUESTHANDLEROBSERVER_H__

// End of File
