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
* Description:  Event notifier implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CPEngNWSessionSlotNotifier2.h>
#include <CPEngNWSessionSlotEvent2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <CPEngNWSessionSlotID2.h>
#include "CIMPSConnUiPresEventNotifierImp.h"
#include "CnUiEventMapper.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CIMPSConnUiPresEventNotifierImp* CIMPSConnUiPresEventNotifierImp::NewL(
    CIMPSConnUiPresEventNotifier& aInterface )
    {
    CIMPSConnUiPresEventNotifierImp* self =
        new ( ELeave ) CIMPSConnUiPresEventNotifierImp( aInterface );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self

    return self;
    }



// Destructor
CIMPSConnUiPresEventNotifierImp::~CIMPSConnUiPresEventNotifierImp()
    {
    //inform the base class that notifier is dying
    //==>stops further notifying
    Dying();
    if ( iEventNotifier )
        {
        iEventNotifier->RemoveObserver( *this );
        }
    delete iEventNotifier;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnUiPresEventNotifierImp::CIMPSConnUiPresEventNotifierImp(
    CIMPSConnUiPresEventNotifier& aInterface )
        : CIMPSConnUiPresEventNotifierImpBase( aInterface )
    {
    }



// Symbian OS default constructor can leave.
void CIMPSConnUiPresEventNotifierImp::ConstructL()
    {

    iEventNotifier = CPEngNWSessionSlotNotifier2::NewL();
    iEventNotifier->AddObserver( *this );
    }



// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::DoStartL()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImp::DoStartL(
    const TArray<TPEngNWSessionSlotEvent>& aEvents )
    {
    if ( !iEventNotifier->IsActive() )
        {
        iEventNotifier->Start( aEvents );
        }
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::DoStop()
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImp::DoStop()
    {
    iEventNotifier->Stop();
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::AppServiceStatusL()
// -----------------------------------------------------------------------------
//
const CPEngNWSessionSlotID2* CIMPSConnUiPresEventNotifierImp::AppServiceStatusL(
    TIMPSConnectionClient aClient )
    {

    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

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
    User::LeaveIfError( slotManager->GetNWSessionSlots(
                            slotIDs,
                            *matchPattern,
                            EPEngNWPresenceSessionOpen ) );

    CleanupStack::PopAndDestroy( 2, slotManager );
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &slotIDs ) );
    CPEngNWSessionSlotID2* tempSlotID = NULL;
    tempSlotID = slotIDs[0]->CloneL();
    delete iSessionSlotID;
    iSessionSlotID = tempSlotID;

    CleanupStack::PopAndDestroy(); //slotIDs
    return iSessionSlotID;
    }


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::HandleNWSessionSlotChangeL()
// From PEC Engine MPEngNWSessionSlotObserver2
// -----------------------------------------------------------------------------
//

void CIMPSConnUiPresEventNotifierImp::HandleNWSessionSlotChangeL(
    CPEngNWSessionSlotNotifier2& /*aNotifier*/,
    CPEngNWSessionSlotEvent2& aEvent )
    {
    TIMPSPresenceServiceEvent appEvent;
    CnUiEventMapper::ConvertToAppEvent( aEvent.Event(), appEvent );

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


// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::HandleNWSessionSlotError()
// From PEC Engine MPEngNWSessionSlotObserver2
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImp::HandleNWSessionSlotError(
    TInt aError,
    CPEngNWSessionSlotNotifier2& /*aNotifier*/ )
    {
    StopAndNotifyObserversFromError( aError );
    }

// -----------------------------------------------------------------------------
// CIMPSConnUiPresEventNotifierImp::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CIMPSConnUiPresEventNotifierImp::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }


//  End of File

