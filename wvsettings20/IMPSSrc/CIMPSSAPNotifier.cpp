/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SAP settings central repository event notifier.
*
*/


// INCLUDE FILES

#include 	<e32std.h>
#include	<e32svr.h>
#include	<centralrepository.h>
#include	"CIMPSSAPNotifier.h"
#include	"mimpssapobserver.h"

// CONSTANTS

// These consts define cenrep Ids which when changed cause notification event.
// For now it is the whole SAP Settings Id range for all access groups.
const TUint32 KNotifyMask = 0x00000000;
const TUint32 KNotifyIds = 0x00000000;

const TUint32 KGroupMask = 0xFC000000; // SAP base id mask: 6 first bits
const TUint32 KPECGroup = 0xC0000000; // do not modify
const TUint32 KIMGroup = 0xC4000000; // do not modify

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::CIMPSSAPNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPNotifier::CIMPSSAPNotifier( TInt aPriority ) : CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSSAPNotifier::ConstructL( TUid aRepositoryUid )
    {
    iRepository = CRepository::NewL( aRepositoryUid );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPNotifier* CIMPSSAPNotifier::NewL( TInt aPriority, TUid aRepositoryUid )
    {
    CIMPSSAPNotifier* self = new( ELeave ) CIMPSSAPNotifier( aPriority );

    CleanupStack::PushL( self );
    self->ConstructL( aRepositoryUid );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CIMPSSAPNotifier::~CIMPSSAPNotifier()
    {
    Cancel();
    iObservers.Close();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::AddObserverL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPNotifier::AddObserverL( MIMPSSAPObserver* aObserver,
                                     TIMPSAccessGroup aGroup )
    {
    if ( !aObserver )
        {
        User::Leave( KErrArgument );
        }

    TIMPSObserverTypePair obs( aObserver, aGroup );

    iObservers.AppendL( obs );
    if ( iObservers.Count() == 1 )
        {
        RequestNotificationL(); // request only once
        }
    }


// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::RemoveObserver()
// -----------------------------------------------------------------------------
//
void CIMPSSAPNotifier::RemoveObserver( MIMPSSAPObserver* aObserver )
    {
    if ( !aObserver )
        {
        return;
        }
    TInt count( iObservers.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iObservers[ i ].Observer() == aObserver )
            {
            iObservers.Remove( i );
            }
        }

    if ( iObservers.Count() == 0 )
        {
        //cancel any outstanding notification request
        Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::RunL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPNotifier::RunL()
    {
    if ( iStatus != KErrCancel )
        {
        NotifyObservers();
        RequestNotificationL();
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::RunError()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPNotifier::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::DoCancel()
// -----------------------------------------------------------------------------
//
void CIMPSSAPNotifier::DoCancel()
    {
    iRepository->NotifyCancelAll();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::RequestNotificationL()
// -----------------------------------------------------------------------------
//

void CIMPSSAPNotifier::RequestNotificationL()
    {
    User::LeaveIfError( iRepository->NotifyRequest( KNotifyIds, KNotifyMask,
                                                    iStatus ) );
    SetActive(); // we have outstanding request now
    }


// -----------------------------------------------------------------------------
// CIMPSSAPNotifier::NotifyObservers()
// -----------------------------------------------------------------------------
//

void CIMPSSAPNotifier::NotifyObservers()
    {
    TInt count( iObservers.Count() );
    TUint32 completionValue( iStatus.Int() );
    completionValue &= KGroupMask;

    for ( TInt ii( count ); --ii >= 0; )
        {
        if ( completionValue == KPECGroup )
            {
            if ( iObservers[ ii ].Group() == EIMPSPECAccessGroup )
                {
                iObservers[ ii ].Observer()->HandleSAPEvent(
                    MIMPSSAPObserver::ESAPSettingChanged );
                }
            }

        else if ( completionValue == KIMGroup )
            {
            if ( iObservers[ ii ].Group() == EIMPSIMAccessGroup )
                {
                iObservers[ ii ].Observer()->HandleSAPEvent(
                    MIMPSSAPObserver::ESAPSettingChanged );
                }
            }
        else
            {
            // no notifications for unknown access groups
            }

        }
    }



//  End of File
