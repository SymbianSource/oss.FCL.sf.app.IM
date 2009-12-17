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
* Description: Proxy class for Im app launcher.
*
*/



// INCLUDE FILES
#include    <E32Math.h>
#include    "impsapplauncherproxy.h"
#include    "impsutils.h"

// CONSTANTS

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::CImpsAppLauncherProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImpsAppLauncherProxy::CImpsAppLauncherProxy( MImpsAppLaunchHandler* aLaunchObserver )
        :   CActive( EPriorityStandard ),
        iLaunchObserver( aLaunchObserver ),
        iRequestList( _FOFF( CLaunchRequest, iLink ) )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImpsAppLauncherProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImpsAppLauncherProxy* CImpsAppLauncherProxy::NewL(
    MImpsAppLaunchHandler* aLaunchObserver )
    {
    CImpsAppLauncherProxy* self = new( ELeave ) CImpsAppLauncherProxy(
        aLaunchObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CImpsAppLauncherProxy::~CImpsAppLauncherProxy()
    {
    // Free the request queue
    TDblQueIter<CLaunchRequest> iter( iRequestList );
    iter.SetToFirst();
    while ( iter )
        {
        CLaunchRequest* request = iter;
        iter++;
        request->Destroy();
        }

    Cancel();
    delete iCurrentRequest; // just in case
    }


// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::LaunchApplication
// The request function. Launches the application which is registered with
// the given ApplicationID.
// -----------------------------------------------------------------------------
//
TInt CImpsAppLauncherProxy::LaunchApplicationL( const TDesC& aApplicationId,
                                                const TDesC& aSAP,
                                                const TDesC& aUserID )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: LaunchApplication (appId: %S, SAP: %S, user: %S)" ), &aApplicationId, &aSAP, &aUserID );
#endif

    CLaunchRequest* launchRequest = CLaunchRequest::NewL( aApplicationId, aSAP, aUserID );


    if ( IsActive() )
        {
        if ( !iCurrentRequest )
            {
            delete launchRequest;
            User::Leave( KErrCorrupt );
            }

        if ( !( *iCurrentRequest == *launchRequest ) )
            {
            // Different request
            // Queue this aApplicationId it for later handling
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: Active with different request " ) );
#endif
            QueueRequest( *launchRequest );
            }
        else
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: Active with same request " ) );
#endif

            }
        }
    else // not active
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: Not active. Starting." ) );
#endif
        // Assign a new request to current
        delete iCurrentRequest;
        iCurrentRequest = NULL;
        iCurrentRequest = launchRequest;

        // return ImLauncher process starting error code
        TInt err = DoStartProcess( *iCurrentRequest );
        if ( err )
            {
            delete iCurrentRequest;
            iCurrentRequest = NULL;
            }

        return err;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::RunL
// Called when the launcher process exited
// -----------------------------------------------------------------------------
//
void CImpsAppLauncherProxy::RunL()
    {
    // Here comes the control when the ImLauncher.exe (or dll) finished
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: RunL %d" ), iStatus.Int() );
#endif

    // Notify the observer if any
    if ( iLaunchObserver != NULL )
        {
        iLaunchObserver->HandleAppLaunch( iCurrentRequest->ApplicationId(), iStatus.Int() );
        }

    // Delete the current Client ID
    delete iCurrentRequest;
    iCurrentRequest = NULL;

    ProcessNextRequestL();
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::DoCancel
// -----------------------------------------------------------------------------
//
void CImpsAppLauncherProxy::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::RunError
// Called when the RunL leaved
// -----------------------------------------------------------------------------
//
TInt CImpsAppLauncherProxy::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::DoStartProcess
// Starts the ImLauncher process
// -----------------------------------------------------------------------------
//
TInt CImpsAppLauncherProxy::DoStartProcess( CLaunchRequest& aRequest )
    {
    TInt result;

    RProcess launcher;
    const TDesC& appId = aRequest.ApplicationId();
    const TDesC& sap = aRequest.Sap();
    const TDesC& userId = aRequest.UserId();
    TImpsLaunchParams params( appId, sap, userId );
    _LIT( KLauncherFilenameExe, "ImLauncher.exe" );
    TPtrC paramsDes = params.Get();
    result = launcher.Create( KLauncherFilenameExe, paramsDes );
    if ( result != KErrNone )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: Create process failed: %d " ), result );
#endif
        return result;
        }

    // When the launcher exits we'll get the exit code in iStatus
    iStatus = KRequestPending;
    SetActive();
    launcher.Logon( iStatus );

    launcher.Resume();
    launcher.Close();

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsAppLauncherProxy: Create process success" ) );
#endif
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::QueueRequest
// Adds the request to the queue. No duplicates are queued.
// -----------------------------------------------------------------------------
//
void CImpsAppLauncherProxy::QueueRequest( CLaunchRequest& aRequest )
    {
    // Check for duplicate requests
    if ( !IsDuplicate( aRequest ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Queued" ) );
#endif
        iRequestList.AddLast( aRequest );
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Request already exists" ) );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::ProcessNextRequest
// Gets the next request from the queue and processes it.
// -----------------------------------------------------------------------------
//
void CImpsAppLauncherProxy::ProcessNextRequestL()
    {
    // Check for queued
    TDblQueIter<CLaunchRequest> iter ( iRequestList );
    iter.SetToFirst();
    if ( iter )
        {
        // There is request to handle
        CLaunchRequest* request = iter;
        delete iCurrentRequest;
        iCurrentRequest = NULL;
        iCurrentRequest = request;
        request->iLink.Deque(); // remove it from the queue

#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "CImpsAppLauncherProxy: Processing the next request: \"%S\"" ),
            &( iCurrentRequest->ApplicationId() ) );
#endif

        TInt err( DoStartProcess( *iCurrentRequest ) );
        if ( ( err != KErrNone ) )
            {
            // if starting failed notify the observer
            if ( iLaunchObserver != NULL )
                {
                iLaunchObserver->HandleAppLaunch( iCurrentRequest->ApplicationId(), err );
                }
            }

        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "CImpsAppLauncherProxy: No more requests to process" ) );
#endif

        }
    }


// -----------------------------------------------------------------------------
// CImpsAppLauncherProxy::IsDuplicate
// Checks if the given ApplicationID is already in the queue.
// -----------------------------------------------------------------------------
//
TBool CImpsAppLauncherProxy::IsDuplicate( CLaunchRequest& aRequest )
    {
    TDblQueIter<CLaunchRequest> iter ( iRequestList );
    iter.SetToFirst();
    while ( iter )
        {
        CLaunchRequest* request = iter++;
        if ( aRequest == *request )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CLaunchRequest::CLaunchRequest
// -----------------------------------------------------------------------------

CLaunchRequest::CLaunchRequest( )
    {
    }


CLaunchRequest* CLaunchRequest::NewL( const TDesC& aCID,
                                      const TDesC& aSAP,
                                      const TDesC& aUserId )
    {
    CLaunchRequest* self = new( ELeave ) CLaunchRequest();

    CleanupStack::PushL( self );
    self->ConstructL( aCID, aSAP, aUserId );
    CleanupStack::Pop();

    return self;

    }

void CLaunchRequest::ConstructL( const TDesC& aCID,
                                 const TDesC& aSAP,
                                 const TDesC& aUserId )
    {
    iCID = aCID.AllocL();
    iSAP = aSAP.AllocL();
    iUserId = aUserId.AllocL();
    }

CLaunchRequest::~CLaunchRequest()
    {
    delete iCID;
    delete iSAP;
    delete iUserId;
    } //lint !e1540 iFields freed in Destroy

void CLaunchRequest::Destroy()
    {
    iLink.Deque();
    delete this;
    }


const TDesC& CLaunchRequest::ApplicationId()
    {
    return *iCID;
    }

const TDesC& CLaunchRequest::Sap()
    {
    return *iSAP;
    }

const TDesC& CLaunchRequest::UserId()
    {
    return *iUserId;
    }

bool CLaunchRequest::operator==( CLaunchRequest& rhs )
    {
    if ( this->ApplicationId().Compare( rhs.ApplicationId() ) ||
         this->Sap().Compare( rhs.Sap() ) ||
         this->UserId().Compare( rhs.UserId() ) )
        {
        return true;
        }
    else
        {
        return false;
        }
    }

//  End of File
