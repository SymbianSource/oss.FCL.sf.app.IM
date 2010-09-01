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
* Description:  ECOM plugin .dll for handling IMPS related WAP push messages.
*
*/



#ifndef CIMPSPUSHHANDLER_H
#define CIMPSPUSHHANDLER_H

//  INCLUDES
#include <CPushHandlerBase.h>
#include "ImpsWatchCli.h"
#ifdef _DEBUG
#include <flogger.h>
#endif

//MACROS
#ifdef _DEBUG
#define _IMPS_LOGGING_ON_
#endif


// CONSTANTS
#ifdef _IMPS_LOGGING_ON_
_LIT( KDestructor,           "In destructor" );
_LIT( KHandleMessageAsync,   "In asynchronous HandleRequestL()" );
_LIT( KHandleMessageSync,    "In synchronous HandleRequestL()" );
_LIT( KCancelHandleMessage,  "In CancelHandleMessage()" );
_LIT( KDoCancel,             "In DoCancel()" );
_LIT( KRun,                  "In RunL()" );
_LIT( KRunError,             "In RunError()" );
_LIT( KTransferMessage,      "In TransferMessage()" );
_LIT( KNewL,                 "In NewL()" );
_LIT( KMessageWasCorrupt,    "Body was corrupt" );
#endif
_LIT( KNotSupported, "This method is not supported!" );

// FORWARD DECLARTIONS
#ifdef _DOUNITTEST
class CPushHandlerPluginTest;
#endif

// CLASS DECLARATION

#ifdef _IMPS_LOGGING_ON_
class CWatcherLogger : public CBase
    {
    public:
        static void Log( TRefByValue<const TDesC> aFmt, ... );
        virtual ~CWatcherLogger();
    };
#endif

/**
*  ECOM listener plugin for WV engine.
*  Plugin is instantiated by the wap listener in the system watcher.
*  It delivers pushed delivery reports and message notifications to WV Engine.
*
*  @lib impspushhandler.lib
*  @since 2.0
*/
class CImpsPushHandler : public CPushHandlerBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        * @since S60 2.0
        */
        ~CImpsPushHandler( );

    public: // New functions

        /**
        * Default 1st phase factory method.
        * @since 2.0
        * @return created instance of the CImpsPushHandler class
        */
        static CImpsPushHandler* NewL( );

    public: // Functions from base classes

        /**
        * Method for handling a received message asynchronously.
        * This not used in CL push
        * @since 2.0
        * @param aPushMsg object representing the received push message
        * @param aStatus Status of the asynchronous call
        */
        void HandleMessageL( CPushMessage* aPushMsg, TRequestStatus& aStatus );

        /**
        * Method for handling a received message synchronously.
        * @since 2.0
        * @param aPushMsg object representing the received push message
        */
        void HandleMessageL( CPushMessage* aPushMsg );

        /**
        * Cancels the requested asynchronous message handling.
        * This not used in CL push
        * @since 2.0
        */
        void CancelHandleMessage( );


    protected:  // Functions from base classes

        /**
        * Performs operations needed for cancelling the message handling.
        * Called by the Active Scheduler after user calling Cancel().
        * @since 2.0
        */
        void DoCancel( );

        /**
        * Performs the actual tasks related to message handling.
        * Called by the Active Scheduler.
        * @since 2.0
        */
        void RunL( );

        /**
        * Performs error handling tasks in case RunL() left.
        * Called by the Active Scheduler.
        * Currently does nothing.
        * @since 2.0
        * @param aError specifies the error code related to RunL()'s leave.
        * @return error value after error handling.
        */
        TInt RunError( TInt aError );

    private:    // New functions

        /**
        * Default constructor.
        * @since 2.0
        */
        CImpsPushHandler( );

        /**
        * 2nd phase constructor.
        * @since 2.0
        */
        void ConstructL( );

        /**
        * Performs basic sanity checks for the received message.
        * Only the body is checked since header is not of interest to us.
        * @since 2.0
        * @return KErrNone if ok,KErrCorrupt if the message has invalid format
        */
        TInt PerformCheck( );

        /**
        * Transfers a push message
        * @since 2.0
        */
        void TransferMessage( );

    private:    // Functions from base classes
        /**
        * Reserved for future expansion.
        * @since 2.0
        */
        void CPushHandlerBase_Reserved1( );

        /**
        * Reserved for future expansion.
        * @since 2.0
        */
        void CPushHandlerBase_Reserved2( );

    private:    // Data

        CPushMessage*    iPushMsg;     // Pushed message
        RImpsWatchClient iWatchClient; // Client to the WV Engine

#ifdef _DOUNITTEST
        friend CPushHandlerPluginTest;
#endif
    };

#endif // CIMPSPUSHHANDLER_H

// End of File
