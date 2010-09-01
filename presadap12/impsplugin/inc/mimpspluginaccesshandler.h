/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This class is an access handler of one network session.
*
*/

#ifndef __MIMPSPLUGINACCESSHANDLER_H__
#define __MIMPSPLUGINACCESSHANDLER_H__


//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class MPEngSessionStatusObserver;
class TPEngWVCspServicesTree2;


// CLASS DECLARATION

/**
 *  Access handler of one network session
 *
 *  It is used to open/close network session
 *  and register its observers
 *
 *  @since 3.0
 */
class MImpsPluginAccessHandler
    {

    public: //

        /**
         *  Log in to the presence server
         *  Log in is done with the parameters passed in the
         *  aLoginBuffer.
         *
         *  @since 3.0
         *
         *  @param aStatus TRequestStatus where completion of the
         *          log in is signaled
         *  @param aLoginBuffer login buffer containing
         *          data which is used to log in to the
         *          presence server.
         */
        virtual void LogInL( TRequestStatus& aStatus,
                             TDesC &aPassword,
                             TDesC &aClientId,
                             TDesC &aServiceAddress,
                             TInt32 aSapId ) = 0;
        /**
         *  Log out from the presence server
         *
         *  @since 3.0
         *
         *  @param aStatus TRequestStatus where completion of the
         *           log out is signaled.
         *  @return
         */
        virtual void LogOutL( TRequestStatus& aStatus ) = 0;


        /**
         *  Cancels active asynchronous operation
         *  There can be only one active asynchronous
         *  operation at the time ( LogInL, LogOutL)
         *
         *  @since 3.0
         */
        virtual void Cancel( TInt aReason ) = 0;

        /**
         *  Register session status observer
         *
         *  @since 3.0
         *
         *  @param aObserver session status observer
         */
        virtual void RegisterSessionObserverL(
            MPEngSessionStatusObserver& aObserver ) = 0;

        /**
         *  UnRegister session status observer
         *
         *  @since 3.0
         *  @param aObserver session status observer
         */
        virtual void UnregisterSessionObserver(
            MPEngSessionStatusObserver& aObserver ) = 0;


        /**
         *  Get session service capabilities
         *
         *  @since 3.0
         *
         *  @return session service capabilities
         */
        virtual TPEngWVCspServicesTree2& ServiceCapabilities() = 0;

    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MImpsPluginAccessHandler() {};
    };


#endif      //  __MIMPSACCESSHANDLER_H

//  End of File



