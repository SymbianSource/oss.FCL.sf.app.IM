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
* Description:  Presence Server abstract interface
*
*/



#ifndef MPENGSERVER_H
#define MPENGSERVER_H

//  INCLUDES

#include <e32def.h>
#include <bamdesca.h>
#include "TPEngServerMessages.h"

// FORWARD DECLARATIONS
class CPEngCSPSessManager;
class CPEngSessionSlotId;
class CPEngSubSession;

// CLASS DECLARATION

/**
 *  Presence Server abstract interface
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
class MPEngServer
    {
    public: // New functions used by the server session
        /**
         *  Inform server about created session,
         *  Access count is increased
         *
         *  @since 3.0
         */
        virtual void SessionCreated() = 0;

        /**
         *  Inform server about closing of the session
         *  Access count is decreased
         *
         *  @since 3.0
         */
        virtual void SessionDied() = 0;


        /**
         *  Add subsession to the container
         *
         *  @since 3.0
         *  @param aSubSession sub session instance
         */
        virtual void AddSubSessionL( CPEngSubSession& aSubSession ) = 0;

        /**
         *  Remove subsession from the container
         *
         *  @since 3.0
         *  @param aSubSession subsession instance
         */
        virtual void RemoveSubSessionL( CPEngSubSession& aSubSession ) = 0;

    public: // New functions used by the server Sub-Session

        /**
         *  Session Manager
         *	If session manager is not active it is created
         *
         *  @since 3.0
         *  @param aSessionId session slot Id
         *  @return session Manager
         */
        virtual CPEngCSPSessManager* CSPSessionManagerLC(
            CPEngSessionSlotId& aSessionId ) = 0;

    public:	// New functions used by the Session Manager

        /**
         *  Remove Session Manager from the server
         *
         *  @since 3.0
         *  @param aManager session manager to be removed
         */
        virtual void RemoveCSPSessManager(
            CPEngCSPSessManager* aManager ) = 0;


        /**
         *  Session is opened
         *  Informs server that session slot is opened
         *  Plugin server is informed about change is needed
         *  @since 3.0
         */
        virtual void SessionOpened() = 0;

        /**
         *  Session is closed
         *  Informs server that session slot is closed
         *  Plugin server is informed about change is needed
         *  @since 3.0
         */
        virtual void SessionClosed() = 0;

    protected:  //Destructor

        /**
          * Virtual inline destructor.
          *
          * Server cannot be destroyed through this interface
          * destroyed using this interface.
          */
        virtual ~MPEngServer() {};

    };

#endif      // MPENGSERVER_H

// End of File
