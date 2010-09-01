/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Imps connection manager.
*
*
*/




// INCLUDE FILES
#include    <e32std.h>
#include    "ImpsSendReceive.h"
#include    "impsactiveconnmonitor.h"
#include    "impsutils.h"
#include    "impserrors.h"


// CONSTANTS

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// LOCAL CONSTANTS AND MACROS

// Wait time 8 seconds before IAP re-opened
const TInt KImpsWaitIAPOpen = 8000000;



// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

//**********************************
// CImpsConnManager
//**********************************
CImpsConnManager::CImpsConnManager(
    CImpsSendReceive2& aSender,
    TInt aPriority )
        : CActive( aPriority ),
        iSender( aSender ),
        iState( EImpsNoAP ),
        iCmd( EImpsTransNone ),
        iConnState( EMsgBearerActive ),
        iManager( NULL ),
        iRunL( EFalse )
    {
    ( void ) iTimer.CreateLocal();
    // Add this to the scheduler
    CActiveScheduler::Add( this );
    }

// EPOC default constructor can leave.
void CImpsConnManager::ConstructL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: CONSTRUCTL rel200540.3f cman=%d" ), ( TInt )this );
#endif
    iManager = NewMsgConnManagerL( 0 );
    }

// Two-phased constructor.
CImpsConnManager* CImpsConnManager::NewL(
    CImpsSendReceive2& aSender, TInt aPriority )
    {
    CImpsConnManager* self = new ( ELeave ) CImpsConnManager(
        aSender, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CImpsConnManager::~CImpsConnManager()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: DESTRUCTOR begins cman=%d" ), ( TInt )this );
#endif
    Cancel();
    iTimer.Close();
    if ( iManager )
        {
        iManager->Destroy();
        }

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: DESTRUCTOR ends cman=%d" ), ( TInt )this );
#endif
    }

// ---------------------------------------------------------
// CImpsConnManager::OpenAPL
// ---------------------------------------------------------
//
void CImpsConnManager::OpenAPL(
    TInt  aIAP, TBool aDelayed )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: OpenAPL iap=%d delay=%d cman=%d" ),
                            aIAP, aDelayed, ( TInt )this );
#endif
    if ( IsActive () )
        {
        // Notice: We should not be here
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: BUSY cman=%d ****" ), ( TInt )this );
#endif
        User::Leave( KErrNotReady );
        }

    iConnState = EMsgBearerActive;

    if ( ( aDelayed || iRunL ) && iCmd != EImpsTransWaitOpen )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d OpenAPL extra wait ****" ), ( TInt )this );
#endif
        iCmd = EImpsTransWaitOpen;
        iState = EImpsNoAP;
        iIAP = aIAP;
        SetActive();
        // Give time for Connection Manager
        iTimer.After( iStatus, KImpsWaitIAPOpen );
        return;
        }
    iCmd = EImpsTransNone;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls SetAccessPointIDL" ), ( TInt )this );
#endif
    iManager->SetAccessPointIDL( aIAP );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls RemoveEventSubscriber and AddEventSubscriberL" ), ( TInt )this );
#endif
    // Verify that in error cases nothing is left hanging
    iManager->RemoveEventSubscriber( this );
    iManager->AddEventSubscriberL( this );
    iStatus = KRequestPending;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls StartConnection" ), ( TInt )this );
#endif
    iManager->StartConnection( iStatus );
    iCmd = EImpsTransOpenAP;
    iState = EImpsNoAP;
    iIAP = aIAP;
    SetActive();
    }

// ---------------------------------------------------------
// CImpsConnManager::CloseAP
// ---------------------------------------------------------
//
void CImpsConnManager::CloseAP( TBool /*aTotalClose*/ )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: CloseAP state=%d %d cman=%d" ),
                            iState, IsActive(), ( TInt )this );
#endif

    if ( iState == EImpsNoAP && !IsActive() )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d CloseAP already closed" ), ( TInt )this );
#endif
        iCmd = EImpsTransNone;
        iConnState = EMsgBearerActive;
        // Engine core waits any kind of response in shut down
        iSender.APStatusEvent( EImpsNoAP, KErrNone, ETrue );
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d CloseAP ends" ), ( TInt )this );
#endif
        return;
        }
    else if ( iState == EImpsNoAP && IsActive() )
        {
        // Waiting open timer must be cancelled
        if ( iCmd == EImpsTransWaitOpen )
            {
            // This cancels the timer
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d err: CANCEL ***" ), ( TInt )this );
#endif
            Cancel();
            iCmd = EImpsTransCloseAP;
            iIAP = 0;
            // Now do the trick to go to own RunL and call there appropriate callback.
            ActivateMe();
            return;
            }
        else if ( iCmd == EImpsTransOpenAP )
            {
            // Open APL is pending and must be cancelled
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls CancelStartL ****" ), ( TInt )this );
#endif
            TRAPD( errx, iManager->CancelStartL() );
            if ( errx )
                {
                // Normally this is not needed, but Cancel StartL may leave with
                // KErrAbort if there is no pending request.
                // VERY RARE situation.
                iCmd = EImpsTransNone;
                iSender.APStatusEvent( EImpsNoAP, KErrNone, ETrue );
                return;
                }
            // Normal case continues in RunL
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d CloseAP ends" ), ( TInt )this );
#endif
            return;
            }
        }
    else if ( IsActive() )  //
        {
        // Previous close is not completed.
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d BUSY ***" ), ( TInt )this );
#endif
        return;
        }
    iCmd = EImpsTransCloseAP;
    iIAP = 0;
    iStatus = KRequestPending;
    SetActive();
    // Notice: Rest of activities in RunL and destructor
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls RemoveEventSubscriber" ), ( TInt )this );
#endif
    iManager->RemoveEventSubscriber( this );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls StopConnection" ), ( TInt )this );
#endif
    iManager->StopConnection( iStatus );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d CloseAP ends" ), ( TInt )this );
#endif
    }


// ---------------------------------------------------------
// CImpsConnManager::HandleBearerEventL
// ---------------------------------------------------------
//
void CImpsConnManager::HandleBearerEventL( TBool aIsAuthClose, TMsgBearerEvent aBearerEvent )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(
        _L( "CImpsConnManager: HandleBearerEventL %d authClose=%d cman=%d" ), aBearerEvent, aIsAuthClose, ( TInt )this );
#endif
    iConnState = aBearerEvent;
    if ( IsActive() )
        {
        // pending open/close, so wait that instead
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log(
            _L( "CImpsConnManager: HandleBearerEventL SPECIAL cmd=%d cman=%d ***" ), iCmd, ( TInt )this );
#endif

        /* Notice:
         This is a little bit tricky situation because of Msg ConnManager DLL
         cannot quarantee the right order of events for us! Thus it is
         possible that we first get bearer-event and after that response
         for open/close connection request although the actual order
         is that open/close is first completed and then the bearer event has
         taken a place.

               /@
               \ \
             ___> \
            (__O)  \
           (____@)  \
           (____@)   \
            (__o)_    \
                  \    \
        */

        switch ( iConnState )
            {
            case EMsgBearerActive:
                iSender.APStatusEvent( EImpsOnline, KErrNone, EFalse );
                break;
            case EMsgBearerSuspended:
                iSender.APStatusEvent( EImpsOffline, KErrNone, EFalse );
                break;
            case EMsgBearerLost:
            default:
                {
                // RunL handles rest of the activities
                // when iConnState == EMsgBearerLost.
                break;
                }
            };
        }
    else // regular case
        {
        switch ( iConnState )
            {
            case EMsgBearerActive:
                iSender.APStatusEvent( EImpsOnline, KErrNone, EFalse );
                break;
            case EMsgBearerSuspended:
                iSender.APStatusEvent( EImpsOffline, KErrNone, EFalse );
                break;
            case EMsgBearerLost:
            default:
                {
                iState = EImpsNoAP;
                iSender.APStatusEvent( EImpsNoAP, KErrNone, EFalse, aIsAuthClose );
                break;
                }
            }
        };
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d HandleBearerEventL ends" ), ( TInt )this );
#endif
    }

// ---------------------------------------------------------
// CImpsConnManager::ManagerHandleL
// ---------------------------------------------------------
//
MMsgConnManager& CImpsConnManager::ManagerHandleL() const
    {
    if ( !iManager )
        {
        User::Leave( KErrBadHandle );
        }
    return *iManager;
    }

// ---------------------------------------------------------
// CImpsConnManager::Destroy
// ---------------------------------------------------------
//
void CImpsConnManager::Destroy()
    {
    if ( iManager )
        {
        iManager->Destroy();
        }
    iManager = NULL;
    }

// ---------------------------------------------------------
// CImpsConnManager::RunL
// ---------------------------------------------------------
//
void CImpsConnManager::RunL( )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: RunL ret=%d cmd=%d state=%d conn=%d cman=%d" ),
                            iStatus.Int(), iCmd, iState, iConnState, ( TInt )this );
#endif

    iRunL = ETrue;
    TInt ret = iStatus.Int();

    // We have delayed and given time for connection manager to work properly
    if ( iCmd == EImpsTransWaitOpen )
        {
        if ( !ret )
            {
            TInt errx = KErrNone;
            TRAP( errx, OpenAPL( iIAP, EFalse ) );
            }
        else
            {
            iState = EImpsNoAP;
            iCmd = EImpsTransNone;
            iSender.APStatusEvent( iState, ret, ETrue );
            }
        }
    else if ( iCmd == EImpsTransOpenAP )
        {
        iCmd = EImpsTransNone;
        iState = EImpsNoAP;
        if ( ret == KErrNone )
            {
            if ( iConnState != EMsgBearerLost )
                {
                // AP is opened successfully
                iState = EImpsOnline;
                }
            else // iConnState == EMsgBearerLost
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( _L( "CImpsConnManager: err: cman=%d iConnState==EMsgBearerLost ****" ), ( TInt )this );
#endif
                // Special case where order of events from ConnManager is not valid.
                ret = KImpsErrorNoIAP;
                // KImpsErrorNoIAP error code has a special error handling
                // in CSPSession class.
                }
            }
        iSender.APStatusEvent( iState, ret, ETrue );
        }
    else if ( iCmd == EImpsTransCloseAP )
        {
        iCmd = EImpsTransNone;
        iState = EImpsNoAP;
        // Notice that iSender must exist, i.e. DeleteCSP() cancels
        // everything before deleting CSP session entity referred by iSender.
        iSender.APStatusEvent( EImpsNoAP, ret, ETrue );
        }
    else if ( iCmd == EImpsTransCloseDelay )
        {
        // This is a special case where this object is used for yielding.
        // Actually it calls CloseAP
        iCmd = EImpsTransNone;
        iSender.CloseTr( );
        }
    else
        {
        iCmd = EImpsTransNone;
        }
    iRunL = EFalse;
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d RunL ends" ), ( TInt )this );
#endif
    }

// ---------------------------------------------------------
// CImpsConnManager::DoCancel
// ---------------------------------------------------------
//
void CImpsConnManager::DoCancel()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d DoCancel" ), ( TInt )this );
#endif
    TImpsTransCmd orig = iCmd;
    iCmd = EImpsTransNone;
    if ( orig == EImpsTransOpenAP )
        {
        // cancel
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d calls CancelStartL *****" ), ( TInt )this );
#endif
        TInt errx = KErrNone;
        TRAP( errx, iManager->CancelStartL() );
        }
    else if ( orig == EImpsTransCloseAP )
        {
        iState = EImpsNoAP;
        }
    // This just waits IAP connection operation to end.
    else if ( iCmd == EImpsTransWaitOpen )
        {
        iTimer.Cancel();
        }
    }

TInt CImpsConnManager::ActivateMe(  )
    {
    // This yields the control to the server thread active scheduler
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrNone );
        return KErrNone;
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d KErrNotReady ****" ), ( TInt )this );
#endif
        return KErrNotReady;
        }
    }

TInt CImpsConnManager::StartDelayed( )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsConnManager: cman=%d StartDelayed" ), ( TInt )this );
#endif
    iCmd = EImpsTransCloseDelay;
    return ActivateMe();
    }


//  End of File
