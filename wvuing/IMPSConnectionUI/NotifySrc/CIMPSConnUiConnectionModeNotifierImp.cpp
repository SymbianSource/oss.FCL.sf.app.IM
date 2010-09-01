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
* Description:  Connection mode notifier implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <mimpsconnuiconnectionmodeobserverng.h>
#include "CIMPSConnUiConnectionModeNotifierImp.h"
#include "CnUiPanics.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::NewL()
// -----------------------------------------------------------------------------
//
CIMPSConnUiConnectionModeNotifierImp* CIMPSConnUiConnectionModeNotifierImp::NewL(
    CIMPSConnUiConnectionModeNotifier& aInterface,
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionModeNotifierImp* self =
        new ( ELeave ) CIMPSConnUiConnectionModeNotifierImp( aInterface,
                                                             aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }


// Destructor
CIMPSConnUiConnectionModeNotifierImp::~CIMPSConnUiConnectionModeNotifierImp()
    {
    Dying();
    delete iConnModeHandler;
    iObserverArray.Close();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnUiConnectionModeNotifierImp::CIMPSConnUiConnectionModeNotifierImp(
    CIMPSConnUiConnectionModeNotifier& aInterface,
    TIMPSConnectionClient aClient )
        : iInterface( aInterface ),
        iClient( aClient ),
        iStarted( EFalse ),
        iDying( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::ConstructL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::ConstructL()
    {
    iConnModeHandler = CreateConnModeHandlerL();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::StartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::StartL()
    {
    //make here sanity checks to encapsulate
    //notifier behaviour
    if ( iDying )
        {
        //if dying, the notifier restart is silently ignored
        //notifier is evidently going down anyway
        return;
        }

    if ( iStarted )
        {
        User::Leave( KErrInUse );
        }

    if ( iObserverArray.Count() == 0 )
        {
        User::Leave( KErrNotReady );
        }

    TRAPD( err, DoStartL() );
    if ( err != KErrNone )
        {
        DoStop();
        User::Leave( err );
        }

    iStarted = ETrue;
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::Stop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::Stop()
    {
    //notify observers from cancel
    StopAndNotifyObserversFromError( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::AddObserverL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::AddObserverL(
    MIMPSConnUiConnectionModeObserver* aObserver )
    {
    __ASSERT_ALWAYS( aObserver,
                     CnUiPanicOrLeaveL( EIMPSConn_NULLPtr,
                                        KErrArgument ) );

    iObserverArray.AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CIMPSConnUiConnectionModeNotifierImp::RemoveObserver(
    MIMPSConnUiConnectionModeObserver* aObserver )
    {
    return iObserverArray.RemoveObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::ConnectionModeByCategoryL()
// -----------------------------------------------------------------------------
//
TIMPSConnectionModeEvent CIMPSConnUiConnectionModeNotifierImp::ConnectionModeByCategoryL(
    TIMPSConnectionModeEvent aModeEventCateqory )
    {
    return iConnModeHandler->ConnectionModeL( iClient, aModeEventCateqory );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::StopAndNotifyObserversFromError()
// from MCnUiConnModeObserver
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::HandleConnModeChange(
    TIMPSConnectionClient /*aRequestedClient*/,
    TIMPSConnectionModeEvent aConnMode )
    {
    NotifyObserversFromEvent( aConnMode );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::DoStartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::DoStartL()
    {
    iConnModeHandler->NotifyConnectionModeChangesL( this, iClient );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::DoStop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::DoStop()
    {
    iConnModeHandler->CancelConnectionModeNotify();
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::Dying()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::Dying()
    {
    iDying = ETrue;
    if ( iStarted )
        {
        iStarted = EFalse;
        DoStop();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::NotifyObserversFromEvent()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::NotifyObserversFromEvent(
    TIMPSConnectionModeEvent aEvent )
    {
    //if not running, don't notify the clients
    if ( !iStarted )
        {
        return;
        }

    iObserverArray.NotifyObservers( *this, aEvent );
    }



// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::StopAndNotifyObserversFromError()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::StopAndNotifyObserversFromError( TInt aError )
    {
    //if not running, don't notify the clients
    if ( !iStarted )
        {
        return;
        }

    //Error propagating from the underlying implementation
    //causes the notifier to stop. However, flag & actual
    //stopping must be done before notifying the clients
    //since some client may wan't to restart this notifier
    //in the notification callback...

    //Also the state must be set to "not started" before
    //actual stopping  since the stopping of some event
    //sources cause further KErrCancel events
    //(==>those will be filtered on !started check on above.)
    iStarted = EFalse;
    DoStop();

    //do the notify
    iObserverArray.NotifyObserversFromError( *this, aError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::MediateNotifyL()
// From MGenObserverNotifyMediator
// Forwards handling to pre-registered mediator function.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::MediateNotifyL(
    MIMPSConnUiConnectionModeObserver& aObserverToNotify,
    TIMPSConnectionModeEvent& aNotifyData )
    {
    aObserverToNotify.HandleConnectionModeEventL( &iInterface, aNotifyData );
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::MediateNotifyError()
// From MGenObserverNotifyMediator
// Forwards any error propagated from MediateNotifyL() to
// observer handle error.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::MediateNotifyError(
    MIMPSConnUiConnectionModeObserver& aObserverToNotify,
    TInt aLeaveError )
    {
    aObserverToNotify.HandleConnectionModeEventNotifyError( &iInterface, aLeaveError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionModeNotifierImp::MediateError()
// From MGenObserverNotifyMediator
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionModeNotifierImp::MediateError(
    MIMPSConnUiConnectionModeObserver& aObserverToNotify,
    TInt aError )
    {
    aObserverToNotify.HandleConnectionModeEventNotifyError( &iInterface, aError );
    }


//  End of File

