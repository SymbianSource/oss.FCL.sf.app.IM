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
* Description:  Connection settings notifier implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <impspresenceconnectionuiconstsng.h>
#include <MIMPSConnUiConnectionSettingsObserver.h>
#include "CIMPSConnUiConnectionSettingsNotifierImp.h"

#include "CnUiPanics.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::NewL()
// -----------------------------------------------------------------------------
//
CIMPSConnUiConnectionSettingsNotifierImp* CIMPSConnUiConnectionSettingsNotifierImp::NewL(
    CIMPSConnUiConnectionSettingsNotifier& aInterface,
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiConnectionSettingsNotifierImp* self =
        new ( ELeave ) CIMPSConnUiConnectionSettingsNotifierImp( aInterface,
                                                                 aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }


// Destructor
CIMPSConnUiConnectionSettingsNotifierImp::~CIMPSConnUiConnectionSettingsNotifierImp()
    {
    Dying();
    iObserverArray.Close();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnUiConnectionSettingsNotifierImp::CIMPSConnUiConnectionSettingsNotifierImp(
    CIMPSConnUiConnectionSettingsNotifier& aInterface,
    TIMPSConnectionClient aClient )
        : iInterface( aInterface ),
        iClient( aClient ),
        iStarted( EFalse ),
        iDying( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::ConstructL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::StartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::StartL()
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
// CIMPSConnUiConnectionSettingsNotifierImp::Stop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::Stop()
    {
    //notify observers from cancel
    StopAndNotifyObserversFromError( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::AddObserverL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::AddObserverL(
    MIMPSConnUiConnectionSettingsObserver* aObserver )
    {
    __ASSERT_ALWAYS( aObserver,
                     CnUiPanicOrLeaveL( EIMPSConn_NULLPtr,
                                        KErrArgument ) );

    iObserverArray.AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CIMPSConnUiConnectionSettingsNotifierImp::RemoveObserver(
    MIMPSConnUiConnectionSettingsObserver* aObserver )
    {
    return iObserverArray.RemoveObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::ConnectionSettingByCategoryL()
// -----------------------------------------------------------------------------
//
TIMPSConnectionSettingsEvent
CIMPSConnUiConnectionSettingsNotifierImp::ConnectionSettingByCategoryL(
    TIMPSConnectionSettingsEvent /*aSettingsEventCateqory*/ )
    {
    return EIMPSCSEUnknown;
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::DoStartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::DoStartL()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::DoStop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::DoStop()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::Dying()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::Dying()
    {
    iDying = ETrue;
    if ( iStarted )
        {
        iStarted = EFalse;
        DoStop();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::NotifyObserversFromEvent()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::NotifyObserversFromEvent(
    TIMPSConnectionSettingsEvent aEvent )
    {
    //if not running, don't notify the clients
    if ( !iStarted )
        {
        return;
        }

    iObserverArray.NotifyObservers( *this, aEvent );
    }



// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::StopAndNotifyObserversFromError()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::StopAndNotifyObserversFromError( TInt aError )
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
// CIMPSConnUiConnectionSettingsNotifierImp::MediateNotifyL()
// From MGenObserverNotifyMediator
// Forwards handling to pre-registered mediator function.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::MediateNotifyL(
    MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
    TIMPSConnectionSettingsEvent& aNotifyData )
    {
    aObserverToNotify.HandleConnectionSettingsEventL( &iInterface, aNotifyData );
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::MediateNotifyError()
// From MGenObserverNotifyMediator
// Forwards any error propagated from MediateNotifyL() to
// observer handle error.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::MediateNotifyError(
    MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
    TInt aLeaveError )
    {
    aObserverToNotify.HandleConnectionSettingsEventNotifyError( &iInterface, aLeaveError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiConnectionSettingsNotifierImp::MediateError()
// From MGenObserverNotifyMediator
// -----------------------------------------------------------------------------
//
void CIMPSConnUiConnectionSettingsNotifierImp::MediateError(
    MIMPSConnUiConnectionSettingsObserver& aObserverToNotify,
    TInt aError )
    {
    aObserverToNotify.HandleConnectionSettingsEventNotifyError( &iInterface, aError );
    }


//  End of File

