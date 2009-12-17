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
* Description:  Client filtered event notifier implementation
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CPEngNWSessionSlotNotifier2.h>
#include <CPEngNWSessionSlotEvent2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <CPEngNWSessionSlotID2.h>
#include <PEngPresenceEngineConsts2.h>

#include "CIMPSConnUiClntFilteredPresEventNotifierImp.h"
#include "CnUiEventMapper.h"
#include "IMPSCommonUiDebugPrint.h"
#include "impspresenceconnectionuiconstsng.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CIMPSConnUiClntFilteredPresEventNotifierImp*
CIMPSConnUiClntFilteredPresEventNotifierImp::NewL(
    CIMPSConnUiPresEventNotifier& aInterface,
    TIMPSConnectionClient aClient )
    {
    CIMPSConnUiClntFilteredPresEventNotifierImp* self =
        new ( ELeave ) CIMPSConnUiClntFilteredPresEventNotifierImp( aInterface, aClient );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CIMPSConnUiClntFilteredPresEventNotifierImp::~CIMPSConnUiClntFilteredPresEventNotifierImp()
    {
    //inform the base class that notifier is dying
    //==>stops further notifying
    Dying();
    delete iClientStatusHandler;
    if ( iPecEventNotifier )
        {
        iPecEventNotifier->RemoveObserver( *this );
        }
    delete iPecEventNotifier;
    delete iSessionSlotID;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnUiClntFilteredPresEventNotifierImp::CIMPSConnUiClntFilteredPresEventNotifierImp(
    CIMPSConnUiPresEventNotifier& aInterface,
    TIMPSConnectionClient aClient )
        : CIMPSConnUiPresEventNotifierImpBase( aInterface ),
        iClientToFilter( aClient )
    {
    }


// Symbian OS default constructor can leave.
void CIMPSConnUiClntFilteredPresEventNotifierImp::ConstructL()
    {
    iPecEventNotifier = CPEngNWSessionSlotNotifier2::NewL();
    iPecEventNotifier->AddObserver( *this );

    iClientStatusHandler = CreateClientStatusHandlerLC();
    CleanupStack::Pop( ); // iClientStatusHandler

    iClientLoggedIn = IsLoggedInL( iClientToFilter );

    }

// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::StartL()
// -----------------------------------------------------------------------------

TBool CIMPSConnUiClntFilteredPresEventNotifierImp::IsLoggedInL( TIMPSConnectionClient aClient )
    {
    CPEngNWSessionSlotID2* pattern = CPEngNWSessionSlotID2::NewLC();

    pattern->SetServiceAddressMatchAnyL();
    pattern->SetUserIdMatchAnyL();
    if ( aClient == EIMPSConnClientPEC )
        {
        pattern->SetAppIdL( KPEngAppIdPEC() );
        }
    else
        {
        pattern->SetAppIdL( KPEngAppIdIM() );
        }

    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();
    RPointerArray< CPEngNWSessionSlotID2 > array;
    User::LeaveIfError( slotManager->GetNWSessionSlots( array,
                                                        *pattern,
                                                        EPEngNWPresenceSessionOpen ) );
    CleanupStack::PopAndDestroy( 2, pattern ); // pattern, slotManager

    TBool returnValue( EFalse );
    if ( array.Count() > 0 )
        {
        array.ResetAndDestroy();
        // this client has an open network session
        returnValue = ETrue;
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::StartL()
// -----------------------------------------------------------------------------

void CIMPSConnUiClntFilteredPresEventNotifierImp::DoStartL(
    const TArray<TPEngNWSessionSlotEvent>& /* aEvents */ )
    {
    CPEngNWSessionSlotID2* pattern = CPEngNWSessionSlotID2::NewLC();

    pattern->SetServiceAddressMatchAnyL();
    pattern->SetUserIdMatchAnyL();
    if ( iClientToFilter == EIMPSConnClientPEC )
        {
        pattern->SetAppIdL( KPEngAppIdPEC() );
        }
    else
        {
        pattern->SetAppIdL( KPEngAppIdIM() );
        }

    //issue first request for PEC events
    if ( !iPecEventNotifier->IsActive() )
        {
        iPecEventNotifier->Start( *pattern );
        }

    CleanupStack::PopAndDestroy( pattern );

    //and then for the Connection UI client status flags
    iClientStatusHandler->NotifyClientLoginStatusChangesL( this,
                                                           iClientToFilter );

    //and initial state
    iClientLoggedIn = IsLoggedInL( iClientToFilter );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::DoStop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiClntFilteredPresEventNotifierImp::DoStop()
    {
    if ( iPecEventNotifier )
        {
        iPecEventNotifier->Stop();
        }

    if ( iClientStatusHandler )
        {
        iClientStatusHandler->CancelClientStatusNotify();
        }
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::AppServiceStatusL()
// -----------------------------------------------------------------------------
//
const CPEngNWSessionSlotID2*
CIMPSConnUiClntFilteredPresEventNotifierImp::AppServiceStatusL(
    TIMPSConnectionClient aClient )
    {
    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    // create a match pattern to match all of this client's slots
    CPEngNWSessionSlotID2* matchPattern = CPEngNWSessionSlotID2::NewLC();
    matchPattern->SetServiceAddressMatchAnyL();
    matchPattern->SetUserIdMatchAnyL();

    if ( aClient == EIMPSConnClientIM )
        {
        matchPattern->SetAppIdL( KPEngAppIdIM() );
        }
    else
        {
        matchPattern->SetAppIdL( KPEngAppIdPEC() );
        }

    RPointerArray< CPEngNWSessionSlotID2 > slotIDs;
    // get only active session slot id's
    User::LeaveIfError( slotManager->GetNWSessionSlots(
                            slotIDs,
                            *matchPattern,
                            EPEngNWPresenceSessionOpen ) );

    CleanupStack::PopAndDestroy( 2, slotManager ); // matchPattern, slotManager
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &slotIDs ) );
    CPEngNWSessionSlotID2* tempSlotID = NULL;
    if ( slotIDs.Count() == 0 )
        {
        // no slots, just return NULL
        CleanupStack::PopAndDestroy();
        return tempSlotID;
        }
    // we take the first one, since at this point there can be only one
    // active connection
    tempSlotID = slotIDs[0]->CloneL();
    delete iSessionSlotID;
    iSessionSlotID = tempSlotID;

    CleanupStack::PopAndDestroy(); // slotIDs
    return iSessionSlotID;
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::HandleNWSessionSlotChangeL()
// From PEC Engine MPEngEventNotifierObserver
// -----------------------------------------------------------------------------
//

void CIMPSConnUiClntFilteredPresEventNotifierImp::HandleNWSessionSlotChangeL(
    CPEngNWSessionSlotNotifier2& /*aNotifier*/,
    CPEngNWSessionSlotEvent2& aEvent )
    {
    TIMPSPresenceServiceEvent appEvent;
    CnUiEventMapper::ConvertToAppEvent( aEvent.Event(), appEvent );

    if ( iClientLoggedIn )
        {
        //Client is logged in
        //==>deliver all events to observers
        if ( appEvent == EIMPSPresenceServiceOffline )
            {
            //==>PEC engine wen't to offline ==>
            //the current client isn't either logged in anymore
            iClientLoggedIn = EFalse;
            }

        TIMPSConnectionClient client;
        const CPEngNWSessionSlotID2* slotID = &aEvent.NWSessionSlotID();

        if ( 0 == slotID->AppId().Compare( KPEngAppIdIM() ) )
            {
            client = EIMPSConnClientIM;
            }
        else
            {
            client = EIMPSConnClientPEC;
            }

        NotifyObserversFromEvent( *slotID, client, appEvent );
        }
    }



// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::HandleNWSessionSlotError()
// From PEC Engine MPEngEventNotifierObserver
// -----------------------------------------------------------------------------
//
void CIMPSConnUiClntFilteredPresEventNotifierImp::HandleNWSessionSlotError(
    TInt aError,
    CPEngNWSessionSlotNotifier2& /* aNotifier */ )
    {
    //here comes only errors from
    //starting or stopping the CPEngEventNotifier
    StopAndNotifyObserversFromError( aError );
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::HandleClientStatusChange()
// From PEC Engine MCnUiClientStatusObserver
// -----------------------------------------------------------------------------
//
void CIMPSConnUiClntFilteredPresEventNotifierImp::HandleClientLoginStatusChange(
    TIMPSConnectionClient aRequestedClient,
    TBool aClientNowLoggedIn )
    {
    // Removed filtering of events in here, which is the easiest way for
    // IM to detect that login is really complete. Previously IsLoggedIn method
    // was the only way, but it was possbile that the contact lists were not
    // fully synchronized --> IM could not launch properly nor no one notified
    // it about the synchronization of lists.
    // Removed filtering: if( iClientLoggedIn != aClientNowLoggedIn )

    //the filtered clients login status has changed
    //==>notify the observers from event
    iClientLoggedIn = aClientNowLoggedIn;

    if ( iClientLoggedIn )
        {
        // ignore the error, there is not really much we can
        // do here if an error occurs
        TInt ignore;
        TRAP( ignore, DoHandleClientLoginStatusChangeL( aRequestedClient,
                                                        aClientNowLoggedIn ) );
        }
    else
        {
        //client has logged out
        NotifyObserversFromEvent( *iSessionSlotID,
                                  aRequestedClient,
                                  EIMPSPresenceServiceOffline );
        }
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::DoHandleClientLoginStatusChangeL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiClntFilteredPresEventNotifierImp::DoHandleClientLoginStatusChangeL(
    TIMPSConnectionClient aRequestedClient,
    TBool /* aClientNowLoggedIn */ )
    {
    //client has logged in
    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    CPEngNWSessionSlotID2* matchPattern = CPEngNWSessionSlotID2::NewLC();
    matchPattern->SetServiceAddressMatchAnyL();
    matchPattern->SetUserIdMatchAnyL();

    if ( aRequestedClient == EIMPSConnClientIM )
        {
        matchPattern->SetAppIdL( KPEngAppIdIM() );
        }
    else
        {
        matchPattern->SetAppIdL( KPEngAppIdPEC() );
        }

    RPointerArray< CPEngNWSessionSlotID2 > slotIDs;
    TInt error( slotManager->GetNWSessionSlots(
                    slotIDs,
                    *matchPattern,
                    EPEngNWPresenceSessionOpen ) );
    if ( error != KErrNone )
        {
        slotIDs.ResetAndDestroy();
        User::Leave( error );
        }
    CleanupStack::PopAndDestroy( 2, slotManager ); // matchPattern, slotManager
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &slotIDs ) );

    CPEngNWSessionSlotID2* tempSlotID = NULL;
    if ( slotIDs.Count() == 0 )
        {
        // this should actually not happen here, but if it happens
        // it means we are logged out
        NotifyObserversFromEvent( *iSessionSlotID,
                                  aRequestedClient,
                                  EIMPSPresenceServiceOffline );
        }
    else
        {
        tempSlotID = slotIDs[0]->CloneL();
        delete iSessionSlotID;
        iSessionSlotID = tempSlotID;

        NotifyObserversFromEvent( *iSessionSlotID,
                                  aRequestedClient,
                                  EIMPSPresenceServiceOnline );
        }
    CleanupStack::PopAndDestroy(); //slotIDs
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiClntFilteredPresEventNotifierImp::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CIMPSConnUiClntFilteredPresEventNotifierImp::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }

//  End of File

