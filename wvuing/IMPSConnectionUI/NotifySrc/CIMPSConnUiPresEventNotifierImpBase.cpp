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
* Description:  Event notifier implementation base
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <mimpsconnuipresenceeventobserverng.h>
#include <CPEngNWSessionSlotID2.h>
#include "CIMPSConnUiPresEventNotifierImpBase.h"
#include "CnUiEventMapper.h"
#include "CnUiPanics.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Destructor
CIMPSConnUiPresEventNotifierImpBase::~CIMPSConnUiPresEventNotifierImpBase()
    {
    iObserverArray.Close();
    delete iSessionSlotID;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnUiPresEventNotifierImpBase::CIMPSConnUiPresEventNotifierImpBase(
    CIMPSConnUiPresEventNotifier& aInterface )
        : iInterface( aInterface ),
        iStarted( EFalse ),
        iDying( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::StartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::StartL()
    {
    //make here sanity checks to encapsulate
    //real notifiers behaviour
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

    RArray< TPEngNWSessionSlotEvent > pecEvents;
    CleanupClosePushL( pecEvents );
    CnUiEventMapper::AddAllKnownPecEventsL( pecEvents );
    TRAPD( err, DoStartL( pecEvents.Array() ) );
    if ( err != KErrNone )
        {
        DoStop();
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy(); //pecEvents
    iStarted = ETrue;
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::Stop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::Stop()
    {
    StopAndNotifyObserversFromError( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::Dying()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::Dying()
    {
    iDying = ETrue;
    if ( iStarted )
        {
        iStarted = EFalse;
        DoStop();
        }
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::AddObserverL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::AddObserverL(
    MIMPSConnUiPresenceEventObserver* aObserver )
    {
    __ASSERT_ALWAYS( aObserver,
                     CnUiPanicOrLeaveL( EIMPSConn_NULLPtr,
                                        KErrArgument ) );

    iObserverArray.AddObserverL( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CIMPSConnUiPresEventNotifierImpBase::RemoveObserver(
    MIMPSConnUiPresenceEventObserver* aObserver )
    {
    return iObserverArray.RemoveObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::NotifyObserversFromEvent()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::NotifyObserversFromEvent(
    const CPEngNWSessionSlotID2& aSessionSlotID,
    TIMPSConnectionClient aClient,
    TIMPSPresenceServiceEvent aEvent )
    {
    //if not running, don't notify the clients
    if ( !iStarted )
        {
        return;
        }

    CPEngNWSessionSlotID2* tempSlot = NULL;
    if ( &aSessionSlotID )
        {
        TRAPD( err, tempSlot = aSessionSlotID.CloneL() );
        if ( err )
            {
            return;
            }
        delete iSessionSlotID;
        iSessionSlotID = tempSlot;
        iClient = aClient;
        }

    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSConnUiPresEventNotifierImpBase::NotifyObserversFromEvent() [%d]" ), aEvent );
    iObserverArray.NotifyObservers( *this, aEvent );
    }



// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::StopAndNotifyObserversFromError()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::StopAndNotifyObserversFromError( TInt aError )
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
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSConnUiPresEventNotifierImpBase::StopAndNotifyObserversFromError() [%d]" ), aError );
    iObserverArray.NotifyObserversFromError( *this, aError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::MediateNotifyL()
// From MGenObserverNotifyMediator
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::MediateNotifyL(
    MIMPSConnUiPresenceEventObserver& aObserverToNotify,
    TIMPSPresenceServiceEvent& aNotifyData )

    {
    aObserverToNotify.HandlePresenceEventL( &iInterface, *iSessionSlotID, iClient, aNotifyData );
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::MediateNotifyError()
// From MGenObserverNotifyMediator
// Forwards any error propagated from MediateNotifyL() to
// observer handle error.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::MediateNotifyError(
    MIMPSConnUiPresenceEventObserver& aObserverToNotify,
    TInt aLeaveError )
    {
    aObserverToNotify.HandlePresenceEventNotifyError( &iInterface, aLeaveError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImpBase::MediateError()
// From MGenObserverNotifyMediator
// Forwards error to observer.
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImpBase::MediateError(
    MIMPSConnUiPresenceEventObserver& aObserverToNotify,
    TInt aError )
    {
    aObserverToNotify.HandlePresenceEventNotifyError( &iInterface, aError );
    }

//  End of File

