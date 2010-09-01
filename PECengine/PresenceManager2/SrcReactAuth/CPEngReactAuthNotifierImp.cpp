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
* Description:  Notifier API to listen presence reactive authorization changes.
*
*/

// INCLUDE FILES
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngReactAuthNotifierImp.h"
#include "CPEngNWSessionSlotEventEntry.h"
#include "GenObserverNotifyMediators.h"

#include "CPEngNWSessionSlotStorageProxy.h"
#include "PEngListLibraryFactory.h"
#include "MPEngAuthorizationManager.h"
#include "TPEngGenArrayAdapter.h"
#include "MPEngAuthorizationRequest.h"
#include <CPEngReactAuthNotifier.h>
#include <MPEngReactAuthObserver.h>




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngReactAuthNotifierImp* CPEngReactAuthNotifierImp::NewL(
    CPEngReactAuthNotifier& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngReactAuthNotifierImp* self =
        new ( ELeave ) CPEngReactAuthNotifierImp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngReactAuthNotifierImp::~CPEngReactAuthNotifierImp()
    {
    iDying = ETrue;
    Stop();
    if ( iAuthManager )
        {
        iAuthManager->Close();
        }

    iObsArray.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::CPEngReactAuthNotifierImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngReactAuthNotifierImp::CPEngReactAuthNotifierImp(
    CPEngReactAuthNotifier& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iAuthManager = PEngListLibraryFactory::AuthorizationManagerL( iUsedSlot->BaseId() );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::Start()
// -----------------------------------------------------------------------------
//
TInt CPEngReactAuthNotifierImp::Start( TInt /*aOptions*/ )
    {
    //sanity checks to encapsulate notifier behaviour
    if ( iDying )
        {
        //if dying, the notifier restart is silently ignored
        //because notifier is evidently shutting down
        return KErrNone;
        }

    if ( iStarted )
        {
        return KErrInUse;
        }


    TRAPD( err,
        {
        //Register for reactive authorization events
        iAuthManager->RegisterAuthorizationObserverL( *this );
        } );

    if ( err == KErrNone )
        {
        iStarted = ETrue;
        }

    else
        {
        //Cancel the reactive authorization notification requests
        iAuthManager->UnRegisterAuthorizationObserver( *this );
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::Stop()
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::Stop()
    {
    if ( !iStarted )
        {
        return;
        }

    iAuthManager->UnRegisterAuthorizationObserver( *this );
    iStarted = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::IsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngReactAuthNotifierImp::IsActive() const
    {
    return iStarted;
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::AddObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngReactAuthNotifierImp::AddObserver(
    MPEngReactAuthObserver& aObserver )

    {
    return iObsArray.AddObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::RemoveObserver()
// -----------------------------------------------------------------------------
//
TInt CPEngReactAuthNotifierImp::RemoveObserver(
    MPEngReactAuthObserver& aObserver )

    {
    return iObsArray.RemoveObserver( &aObserver );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::HandleNewPendingAuthorization()
// From MPEngAuthorizationObserver
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::HandleNewPendingAuthorization(
    MPEngAuthorizationRequest& aNewPendingAuthorization )
    {
    TPEngGenArrayAdapter< MPEngAuthorizationRequest* > adapter( &aNewPendingAuthorization );
    TArray< MPEngAuthorizationRequest* > array = adapter.Array();

    TGenNotifyMediator2 < MPEngReactAuthObserver,
    CPEngReactAuthNotifier&,
    TArray< MPEngAuthorizationRequest* >& >
    eventMediator( &MPEngReactAuthObserver::HandlePendingAuthorizationReqL,
                   &MPEngReactAuthObserver::HandleReactAuthError,
                   iInterface,
                   array );

    iObsArray.NotifyObservers( eventMediator );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::HandlerNewAuthorizationStatus()
// From MPEngAuthorizationObserver
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::HandlerNewAuthorizationStatus(
    const MPEngAuthorizationStatus& aNewAuthorizationStatus )
    {
    TPEngGenArrayAdapter< const MPEngAuthorizationStatus* > adapter( &aNewAuthorizationStatus );
    TArray< const MPEngAuthorizationStatus* > array = adapter.Array();

    TGenNotifyMediator2 < MPEngReactAuthObserver,
    CPEngReactAuthNotifier&,
    TArray< const MPEngAuthorizationStatus* > >
    eventMediator( &MPEngReactAuthObserver::HandlerAuthorizationStatusL,
                   &MPEngReactAuthObserver::HandleReactAuthError,
                   iInterface,
                   array );

    iObsArray.NotifyObservers( eventMediator );
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::HandleAuthorizationEngineUpdate()
// From MPEngAuthorizationObserver
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::HandleAuthorizationEngineUpdate()
    {
    //Nothing to do
    }


// -----------------------------------------------------------------------------
// CPEngReactAuthNotifierImp::HandleAuthorizationEngineError()
// From MPEngAuthorizationObserver
// -----------------------------------------------------------------------------
//
void CPEngReactAuthNotifierImp::HandleAuthorizationEngineError( TInt aError )
    {
    TPEngGenArrayAdapter< MPEngAuthorizationRequest* > adapter( NULL );
    TArray< MPEngAuthorizationRequest* > array = adapter.Array();

    TGenNotifyMediator2 < MPEngReactAuthObserver,
    CPEngReactAuthNotifier&,
    TArray< MPEngAuthorizationRequest* >& >
    eventMediator( &MPEngReactAuthObserver::HandlePendingAuthorizationReqL,
                   &MPEngReactAuthObserver::HandleReactAuthError,
                   iInterface,
                   array );

    iObsArray.NotifyErrorObservers( eventMediator, aError );
    }


// End of File

