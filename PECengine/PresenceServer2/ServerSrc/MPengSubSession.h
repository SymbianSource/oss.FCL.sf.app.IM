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
* Description:  Sub Session abstract Interface
*  				 used by CSP holder to inform Sub Session about closing
*
*/



#ifndef __MPENGSUBSESSION_H__
#define __MPENGSUBSESSION_H__

//  INCLUDES
#include <e32def.h>


// FORWARD DECLARATIONS
class	CPengSessionSlotId;

// CLASS DECLARATION
/**
 *  Sub Session abstract Interface
 *  used by CSP holder to inform Sub Session about closing
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
class MPEngSubSession
    {
    public:
        /**
         *	Store request response
         *
         *	@since 3.0
         *	@param aResponse asynchronous request response
         */
        virtual void StoreRequestResponse( HBufC8* aResponse ) = 0;


        /**
         *  Application Id of the sub-session
         *
         *  @since 3.0
         *  @return application Id of the sub-session
         */
        virtual TDesC& AppId() = 0;

        /**
         *  Handler successful application Id login
         *
         *  @since 3.0
         */
        virtual void HandleLogInL() = 0;

        /**
         *  Handler successful application Id logOut
         *
         *  @since 3.0
         */
        virtual void HandleLogOut() = 0;

        /**
         *  Disconnect Application Id
         *
         *  @since 3.0
         *  @param aAppId application Id
         */
        virtual void DisconnectAppId( const TDesC& aAppId ) = 0;


        /**
         *  CSP session closed
         *
         *  @since 3.0
         */
        virtual void CSPSessionClosed() = 0;

    protected:  //Destructor

        /**
         * Virtual inline destructor.
         *
         * Cannot be used to delete
         */
        virtual ~MPEngSubSession() {};

    };

#endif      // __MPENGSUBSESSION_H__

// End of File
