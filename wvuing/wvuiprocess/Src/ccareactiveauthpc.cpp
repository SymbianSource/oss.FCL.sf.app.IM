/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for Reactive Auth. PC Interface
*
*/



// INCLUDE FILES
#include "ccareactiveauthpc.h"
#include "mcareactiveauthobserverpc.h"
#include "MCAPresence.h"
#include "CAPresenceManager.h"
#include "MCAStoredContacts.h"
#include "CCAStorageManagerFactory.h"





// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: Constructor
// -----------------------------------------------------------------------------
//
CCAReactiveAuthPC::CCAReactiveAuthPC( )
    {

    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: NewL
// -----------------------------------------------------------------------------
//
CCAReactiveAuthPC* CCAReactiveAuthPC::NewL()
    {
    CCAReactiveAuthPC* self = new ( ELeave ) CCAReactiveAuthPC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCALoginPC::ConstructL
// Symbian Second Phased Constructor, contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::ConstructL()
    {
    iPresence = CAPresenceManager::InstanceL();
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: Destructor
// -----------------------------------------------------------------------------
//
CCAReactiveAuthPC::~CCAReactiveAuthPC()
    {
    if ( iReactiveAuthObserverPC )
        {
        RemoveReactiveAuthObserver();
        }
    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: AddReactiveAuthObserver
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::AddReactiveAuthObserver(
    MCAReactiveAuthObserverPC*  aObserver )
    {
    iReactiveAuthObserverPC = aObserver;
    ASSERT( aObserver );
    iPresence->SetReactiveAuthObserver( this );
    }



// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: RemoveObserver
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::RemoveReactiveAuthObserver()
    {
    //Not the owner of observer. so dont delete
    iReactiveAuthObserverPC = NULL;
    if ( iPresence )
        {
        iPresence->SetReactiveAuthObserver( NULL );
        }

    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: HandleRAError
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::HandleRAError( TInt aErrorCode )
    {
    if ( iReactiveAuthObserverPC )
        {
        iReactiveAuthObserverPC->HandleRAError( aErrorCode );
        }
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: HandleRAStatusL
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::HandleRAStatusL( )
    {
    if ( iReactiveAuthObserverPC )
        {
        iReactiveAuthObserverPC->HandleRAStatusL();
        }
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: HandleRAPendingReqL
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::HandleRAPendingReqL( )
    {
    if ( iReactiveAuthObserverPC )
        {
        iReactiveAuthObserverPC->HandleRAPendingReqL( );
        }
    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: GetListOfRAStatusL
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::GetListOfRAStatusL( TDes& aUserIdPtr,
                                            CDesCArray& aUserIdsArray  )
    {
    TBool status( EFalse );
    TInt id = 0;
    TInt lastStatusPos = 0;
    TInt statusCount = iPresence->RAStatusCount();

    while ( statusCount && iPresence->GetNextRAStatusL(
                id,
                status,
                aUserIdPtr,
                lastStatusPos ) )
        {
        // keep on emptying the queue
        if ( ! status )
            {
            aUserIdsArray.AppendL( aUserIdPtr );
            }
        }
    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: FindAnyContactL
// -----------------------------------------------------------------------------
//
TBool CCAReactiveAuthPC::FindAnyContactL( const TDesC& aContactId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( contacts->FindAnyContact( aContactId ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: GetAliasL
// -----------------------------------------------------------------------------
//
HBufC* CCAReactiveAuthPC::GetAliasL( const TDesC& aUserId  )
    {
    return iPresence->AliasL( aUserId );
    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: GetNextPendingRARequestL
// -----------------------------------------------------------------------------
//
TBool CCAReactiveAuthPC::GetNextPendingRARequestL( TInt& aId,
                                                   TDes& aUserId,
                                                   TInt& aLastPos )
    {
    return iPresence->GetNextPendingRARequestL( aId,
                                                aUserId, aLastPos );
    }


// -----------------------------------------------------------------------------
// CCAReactiveAuthPC: SendReactiveAuthResponseL
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthPC::SendReactiveAuthResponseL( TInt aId,
                                                   TBool aResponse )
    {
    iPresence->SendReactiveAuthResponseL(
        aId,
        aResponse );
    }


// End of File

