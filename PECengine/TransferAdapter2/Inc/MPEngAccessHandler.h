/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class handles the access of one network session
*
*/

#ifndef __MPENGACCESSHANDLER_H
#define __MPENGACCESSHANDLER_H


//  INCLUDES
#include <E32Std.h>

// DATA TYPES



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
class MPEngAccessHandler
    {

    public: //

        /**
         *  Close reference to the Access handler
         *  Access halder is deleted if it is no more needed.
         *  If Access handler is deleted and it had active connection
         *  then connection is closed as well.
         *
         *  @since 3.0
         */
        virtual void Close() = 0;


        /**
         * Open reference to the access Handler
         *
         * @since 3.0
         */
        virtual void Open() = 0 ;

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
                             HBufC8* aLoginBuffer ) = 0;

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
         *  Returns data which was used for log in.
         *  if Access handler is not connected to any presence
         *  server, log in buffer is empty.
         *
         *  @since 3.0
         *
         *  @return log in data buffer reference
         */
        virtual const TDesC8& LogInData( ) const = 0;

        /**
         *  Cancels active asynchronous operation
         *  There can be only one active asynchronous
         *  operation at the time ( LogInL, LogOutL)
         *
         *  @since 3.0
         */
        virtual void Cancel() = 0;

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


        /**
         *  Assert log in data of opened session with the passed data
         *  Function leaves with wrong password code if password does not match
         *
         *  @since 3.0
         *
         *  @param aLogInData login data
         */
        virtual void AssertLoginDataL( TDesC8& aLogInData ) = 0;


    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MPEngAccessHandler() {};
    };


#endif      //  __MPEngAccessHandler_H

//  End of File



