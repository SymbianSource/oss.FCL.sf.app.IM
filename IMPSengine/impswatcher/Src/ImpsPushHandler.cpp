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


// INCLUDE FILES
#include <e32std.h>
#include <CPushHandlerBase.h>
#include <ecom/ImplementationProxy.h>
#include <PluginKiller.h>
#include <PushMessage.h>
#include <PushLog.h>
#include "ImpsPushHandler.h"

// CONSTANTS

_LIT8( KWVID, "WVCI*" );

// ECOM framework related
const TImplementationProxy ImplementationTable[] = 
    {
#ifdef __EABI__
    IMPLEMENTATION_PROXY_ENTRY(0x101F4696, CImpsPushHandler::NewL)
#else
    {{0x101F4696}, CImpsPushHandler::NewL}
#endif
    };

// ============================= LOCAL FUNCTIONS ===============================
#ifdef _IMPS_LOGGING_ON_
const TInt KLogBufferLength = 256;
_LIT(KLogDir, "impswatcher");
_LIT(KLogFile, "impspushhandlerplugin.txt");

void CWatcherLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImpsPushHandler::New
// 1st phase constructor
// Returns: <new CImpsPushHandler>:
// -----------------------------------------------------------------------------
//
CImpsPushHandler* CImpsPushHandler::NewL( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KNewL );
#endif
    CImpsPushHandler* self = new( ELeave ) CImpsPushHandler( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::CImpsPushHandler
// -----------------------------------------------------------------------------
//
CImpsPushHandler::CImpsPushHandler( )
    :CPushHandlerBase()
    {
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::ConstructL
// Adds the AO to the Active Scheduler.
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::ConstructL( )
    {
    // Add plugin to AS
#ifndef _DOUNITTEST
    CActiveScheduler::Add( this );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::~CImpsPushHandler
// -----------------------------------------------------------------------------
//
CImpsPushHandler::~CImpsPushHandler( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KDestructor );
#endif

    // AO's destructor always calls Cancel()
    Cancel( );

    delete iPushMsg;

    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::HandleMessageL
// Starts the message handling procedure asynchronously.
// This is not used in CL messages
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::HandleMessageL(
    CPushMessage* aPushMsg,
    TRequestStatus& aStatus )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KHandleMessageAsync );
#endif

    // We take ownership of this object and delete it ourself
    iPushMsg = aPushMsg;
    SetConfirmationStatus( aStatus );
    SignalConfirmationStatus( KErrNotSupported );
    iPluginKiller->KillPushPlugin( );
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::HandleMessageL
// Synchronous version of the HandleMessageL(). See asynchronous version's 
//  description.
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::HandleMessageL( CPushMessage* aPushMsg )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KHandleMessageSync );
#endif

    // We take ownership of this object and delete it ourself
    iPushMsg = aPushMsg;  

    // Do sanity checks
    if( PerformCheck( ) == KErrNone )
        {
        TransferMessage( );
        }

    // Finish up
    iPluginKiller->KillPushPlugin( );
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::CancelHandleMessage
// Cancels the pending asynchronous HandleMessageL.
// This is not used in CL messages
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::CancelHandleMessage( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KCancelHandleMessage );
#endif
    Cancel( );
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::DoCancel
// Cancels the operation.
// This is not used in CL messages
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::DoCancel( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KDoCancel );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::RunL
// Basically all the work is done from/through this method.
// This is not used in CL messages
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::RunL( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KRun );
#endif

    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::RunError
// Called by ActiveScheduler in case RunL leaves. Currently does nothing.
// Returns: <KErrNone>: 
// This is not used in CL messages
// -----------------------------------------------------------------------------
//
TInt CImpsPushHandler::RunError(TInt /*aError*/)
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KRunError );
#endif
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::CPushHandlerBase_Reserved1
// Reserved for future expansion.
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::CPushHandlerBase_Reserved1( )
    {
#ifndef _DOUNITTEST
    User::Panic( KNotSupported, KErrNotSupported );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::CPushHandlerBase_Reserved2
// Reserved for future expansion.
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::CPushHandlerBase_Reserved2( )
    {
#ifndef _DOUNITTEST
    User::Panic( KNotSupported, KErrNotSupported );
#endif
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::PerformCheck
// Returns: <errorcode>: KErrCorrupt if the message has invalid format
// -----------------------------------------------------------------------------
//
TInt CImpsPushHandler::PerformCheck( )
    {
    TInt error = KErrNone;

    TPtrC8 messageBodyPtr;
    TBool bodyPresent = EFalse;
    bodyPresent = iPushMsg->GetMessageBody( messageBodyPtr );
    // Check that the message begins with WVCI keyword
    if ( ( !bodyPresent ) || ( messageBodyPtr.Match( KWVID ) == KErrNotFound ) )
        {
#ifdef _IMPS_LOGGING_ON_
        CWatcherLogger::Log( KMessageWasCorrupt );
#endif
        error = KErrCorrupt;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CImpsPushHandler::TransferMessage
// -----------------------------------------------------------------------------
//
void CImpsPushHandler::TransferMessage( )
    {
#ifdef _IMPS_LOGGING_ON_
    CWatcherLogger::Log( KTransferMessage );
#endif
    TInt serverRunning = iWatchClient.Register( );
    if( serverRunning == KErrNone )
        {
        TPtrC8 messageBodyPtr;
        TBool bodyPresent = iPushMsg->GetMessageBody( messageBodyPtr );
        if( bodyPresent )
            {
            iWatchClient.SendCIR( messageBodyPtr );
            }
        iWatchClient.UnRegister( );
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns: TImplementationProxy*: Implementation table to the ECOM framework 
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }


//  End of File

