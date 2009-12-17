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
* Description: Class for imps oprhan manage.
*
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <e32base.h>
#include    "ImpsOrphans.h"
#include    "impsfields.h"
#include    "impsdataaccessor.h"
#include    "impssdatautils.h"
#include    "impskey.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// constant
_LIT( KDefaultApplicationID, "Default" );
static const TInt KDefaultQueueCapacity = 10; //The capacity of the orphan queue
static const TInt KDefaultOrphanValidity = 3600;

//**********************************
// CImpsOrphans
//**********************************
CImpsOrphans::CImpsOrphans( )
    {
    }

// -----------------------------------------------------------------------------
// CImpsOrphans::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CImpsOrphans::ConstructL()
    {
    iOrphanQueue = new( ELeave ) COrphanQueue( KDefaultQueueCapacity );
    iLauncherProxy = CImpsAppLauncherProxy::NewL( this );
    }

// -----------------------------------------------------------------------------
// CImpsOrphans::NewL
//
// -----------------------------------------------------------------------------
//
CImpsOrphans* CImpsOrphans::NewL()
    {
    // Perform the construction.
    CImpsOrphans* self = new ( ELeave ) CImpsOrphans( );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CImpsOrphans::~CImpsOrphans()
    {
    delete iLauncherProxy;
    delete iOrphanQueue;
    }


// -----------------------------------------------------------------------------
// CImpsOrphans::NewOrphanL
// Creates a new oprhan message and tries to launch the receiving application
// -----------------------------------------------------------------------------
//
void CImpsOrphans::NewOrphanL( CImpsFields* aMsg, TImpsSessIdent aCspId )
    {
    // Handle only NewMessage & Invitation
    if ( ( aMsg->MessageType() != EImpsNewMessage ) &&
         ( aMsg->MessageType() != EImpsInviteUserReq ) )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsOrphans: Orphan filtered out %d" ), aMsg->MessageType() );
#endif
        // Do not delete aMsg immediately but leave. This is because of
        // CImpsCSPSession::CreateSapResponse still has references
        // to CImpsFields entity used after this method call.
        User::Leave( KErrNotSupported );
        return;
        }

    TImpsEventType eventType( ServiceType( ( TImpsMessageType )aMsg->MessageType() ) );
    COrphan* orphan = AddOrphanL( aMsg, eventType, aCspId ); // the ownership is in the iOrphanList
    // Call application launcher
    TInt err( iLauncherProxy->LaunchApplicationL( orphan->ApplicationId(), *( orphan->iSap ), *( orphan->iUserId ) ) );
    if ( err != KErrNone )
        {
        // Application starting failed
        // Orphaned message is kept in the queue until it expires or it is overwritten
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsOrphans: Starting failed %d." ), err );
#endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsOrphans::NextOrphanLC
// Transfers the next orphan message from the queue for handling
// -----------------------------------------------------------------------------
//
CImpsFields* CImpsOrphans::NextOrphanLC( const TDesC& aCID,
                                         TImpsEventType aServiceType,
                                         TImpsSessIdent aCspId )
    {
    if ( ( aServiceType == EImpsEventMessage ) ||
         ( aServiceType == EImpsEventCommon ) )
        {
        return iOrphanQueue->OrphanMessageLC( aCID, aServiceType, aCspId );
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CImpsOrphans::CheckExpiryL
// Deletes the expired orphan messages
// -----------------------------------------------------------------------------
//
void CImpsOrphans::CheckExpiryL( )
    {

    TTime myExpiry;
    myExpiry.HomeTime();

    iOrphanQueue->DiscardExpired( myExpiry );

    }

// -----------------------------------------------------------------------------
// CImpsOrphans::Stop
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImpsOrphans::Stop( )
    {
    }

// -----------------------------------------------------------------------------
// CImpsOrphans::HandleAppLaunchL
// Handles the application launch
// -----------------------------------------------------------------------------
//
void CImpsOrphans::HandleAppLaunch( const TDesC& /*aApplicationId*/, TInt aStatus )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsOrphans: HandleAppLaunchL status %d" ), aStatus );
#endif

    // removing the warning
    aStatus++;
    }
// -----------------------------------------------------------------------------
// CImpsOrphans::AddOrphanL
// Creates and adds an orphan message to queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
COrphan* CImpsOrphans::AddOrphanL( CImpsFields *aFields,
                                   TImpsEventType aServiceType,
                                   TImpsSessIdent aCspId )
    {
    // Get the ClientID, SessionID
    TPtrC sessionID = aFields->SessionIdL();
    TPtrC applicationID;

    TImpsDataAccessor myAc( aFields );
    TImpsSDataUtils::GetApplicationIDL( &myAc, applicationID );
    // Get the Validity time from message: relative time
    TTimeIntervalSeconds validity;
    GetValidityFromMessageL( myAc, validity );

    if ( applicationID.Length() == 0 )
        {
        applicationID.Set( KDefaultApplicationID );
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsOrphans: NewOrphan(appID: %S, servType: %u, SAP: %S, user: %S)" ), &applicationID, aServiceType, &aCspId.SAP(), &aCspId.UserId() );
#endif

    // Set the validity as absolute time
    TTime myTime;
    myTime.HomeTime();
    myTime += validity;

#ifndef _NO_IMPS_LOGGING_
    _LIT( KDateString5, "%-B%:0%J%:1%T%:2%S%:3%+B" );
    TBuf<30> dateString;
    myTime.FormatL( dateString, KDateString5 );
    CImpsClientLogger::Log( _L( "CImpsOrphans: NewOrphan valid till %S" ), &dateString );
#endif

    COrphan* orphan = COrphan::NewL( applicationID,
                                     sessionID,
                                     aServiceType,
                                     aCspId.SAP(),
                                     aCspId.UserId(),
                                     myTime,
                                     aFields );
    iOrphanQueue->Add( *orphan );
    return orphan;
    }


// -----------------------------------------------------------------------------
// CImpsOrphans::GetValidityFromMessage
// Retrieves the message validity from the instant message. The validity is in
// seconds. If the message did not contain the validity then the default
// validity is returned.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImpsOrphans::GetValidityFromMessageL( MImpsDataAccessor& aAc,
                                            TTimeIntervalSeconds& aValidity )
    {
    TInt validity;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyNewMessage, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyValidity, 0 ) );
    if ( aAc.RestoreIntegerL( myKey, validity ) )
        {
        aValidity = validity;
        }
    else
        {
        aValidity = KDefaultOrphanValidity;
        }
    CleanupStack::PopAndDestroy( );  //myKey
    }


TImpsEventType CImpsOrphans::ServiceType( TImpsMessageType aMessageType )
    {
    TImpsEventType event( EImpsEventNone );

    switch ( aMessageType )
        {
            // IM
        case EImpsNewMessage:
            event = EImpsEventMessage;
            break;
            // Fundamental
        case EImpsInviteUserReq:
            event = EImpsEventCommon;
            break;
        default:
            event = EImpsEventNone;
        }
    return event;
    }


//////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// COrphanQueue::COrphanQueue
// Constructor
// -----------------------------------------------------------------------------
//
COrphanQueue::COrphanQueue( TInt aCapacity ):
        iOrphanList( _FOFF( COrphan, iLink ) ),  //lint !e413
        iCapacity( aCapacity ),
        iSize( 0 )
    {
    }

// -----------------------------------------------------------------------------
// COrphanQueue::~COrphanQueue
// Destructor
// -----------------------------------------------------------------------------
//
COrphanQueue::~COrphanQueue()
    {
    TDblQueIter<COrphan> iter ( iOrphanList );
    iter.SetToFirst();

    while ( iter )
        {
        COrphan* orphan = iter;
        iter++;
        orphan->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// COrphanQueue::SetLength
// Sets the new capacity of the queue
// -----------------------------------------------------------------------------
//
void COrphanQueue::SetLength( TInt aNewCapacity )
    {
    iCapacity = aNewCapacity;
    }

// -----------------------------------------------------------------------------
// COrphanQueue::Add
// Adds a new orphan message to the queue
// -----------------------------------------------------------------------------
//
void COrphanQueue::Add( COrphan& aOrphan )
    {
    // Check the capacity
    __ASSERT_DEBUG( iCapacity, User::Panic( KImpsPanicCategory, EImpsCorrupted ) );
    if ( iSize < iCapacity )
        {
        // Simply add to the end
        iOrphanList.AddLast( aOrphan );
        ++iSize;
        }
    else
        {
        // Remove the 1st and add to the last
        COrphan* orphan = iOrphanList.First();
        orphan->Destroy();
        iOrphanList.AddLast( aOrphan );
        }
    }

// -----------------------------------------------------------------------------
// COrphanQueue::OrphanMessage
// Returns the orphan message with the given client ID and removes it from the
// queue. NULL is returned if not found.
// -----------------------------------------------------------------------------
//
CImpsFields* COrphanQueue::OrphanMessageLC( const TDesC& aApplicationId,
                                            TImpsEventType aServiceType,
                                            TImpsSessIdent aCspId )
    {
    TPtrC applicationId;
    aApplicationId.Length() ? applicationId.Set( aApplicationId ) : applicationId.Set( KDefaultApplicationID );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsOrphans: NextOrphanLC(appID: %S, servType: %u, SAP: %S, user: %S)" ), &applicationId, aServiceType, &aCspId.SAP(), &aCspId.UserId() );
#endif

    TDblQueIter<COrphan> iter ( iOrphanList );
    iter.SetToFirst();

    while ( iter )
        {
        COrphan* orphan = iter;
        iter++;
        // Find the given ClientID, Service Type, SAP and UserId
        if ( ( !applicationId.Compare( orphan->ApplicationId() ) )  &&
             ( aServiceType == orphan->iServiceType ) &&
             ( !orphan->iSap->CompareF( aCspId.SAP() ) )  &&
             ( !orphan->iUserId->CompareF( aCspId.UserId() ) ) )
            {
            // found
            // transfer the ownership to the caller
            CImpsFields* impsFields = orphan->Message();
            orphan->RemoveOwnership();
            orphan->Destroy();
            --iSize;
            CleanupStack::PushL( impsFields );
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsOrphans: NextOrphanLC(): Found! " ) );
#endif
            return impsFields;
            }
        }
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsOrphans: NextOrphanLC() Not found!" ) );
#endif

    return NULL;
    }

// -----------------------------------------------------------------------------
// COrphanQueue::DestroyOrphans
// Deletes all orphan messages which have the give ClientID
// -----------------------------------------------------------------------------
//
void COrphanQueue::DestroyOrphans( TDesC& aApplicationId )
    {
    TDblQueIter<COrphan> iter ( iOrphanList );
    iter.SetToFirst();

    while ( iter )
        {
        COrphan* orphan = iter;
        iter++;
        if ( !aApplicationId.Compare( orphan->ApplicationId() ) )
            {
            orphan->Destroy();
            --iSize;
            }
        }
    }

// -----------------------------------------------------------------------------
// COrphanQueue::DiscardExpired
// Deletes all orphan messages which are expired
// -----------------------------------------------------------------------------
//
void COrphanQueue::DiscardExpired( TTime aExpiry )
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsOrphans: Check expired" ) );
#endif

    TDblQueIter<COrphan> iter ( iOrphanList );
    iter.SetToFirst();

    while ( iter )
        {
        COrphan* orphan = iter;
        iter++;
        // Check the validity

        if ( orphan->Expiry() < aExpiry )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsOrphans: Expired discarded" ) );
#endif
            orphan->Destroy();
            --iSize;
            }
        }


    }
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// COrphan::COrphan
// -----------------------------------------------------------------------------

COrphan::COrphan(
    const TDesC& aApplicationID,
    const TDesC& aSessionId,
    TImpsEventType aServiceType,
    TTime aValidity,
    CImpsFields* aFields ) :
        iServiceType( aServiceType ),
        iFields ( aFields ),
        iExpiry( aValidity )
    {
    iCID = aApplicationID;
    iSessionId = aSessionId;
    }

// -----------------------------------------------------------------------------
// COrphan::ConstructL
// -----------------------------------------------------------------------------
void COrphan::ConstructL(
    const TDesC&    aSap,
    const TDesC&    aUserId )
    {
    iSap = aSap.AllocL();
    iUserId = aUserId.AllocL();
    }

// -----------------------------------------------------------------------------
// COrphan::NewL
// -----------------------------------------------------------------------------
COrphan* COrphan::NewL(
    const TDesC& aCID,
    const TDesC& aSessionId,
    TImpsEventType aServiceType,
    const TDesC&    aSap,
    const TDesC&    aUserId,
    TTime aValidity,
    CImpsFields* aFields )
    {
    COrphan* self = new ( ELeave ) COrphan( aCID, aSessionId, aServiceType, aValidity, aFields );

    CleanupStack::PushL( self );
    self->ConstructL( aSap, aUserId );
    CleanupStack::Pop();

    return self;

    }
// -----------------------------------------------------------------------------
// COrphan::~COrphan
// -----------------------------------------------------------------------------
COrphan::~COrphan()
    {
    delete iSap;
    delete iUserId;
    } //lint !e1540 iFields freed in Destroy

// -----------------------------------------------------------------------------
// COrphan::Destroy
// -----------------------------------------------------------------------------
void COrphan::Destroy()
    {
    iLink.Deque();
    delete iFields;
    iFields = NULL;
    delete this;
    }

// -----------------------------------------------------------------------------
// COrphan::ClientId
// -----------------------------------------------------------------------------
TDesC& COrphan::ApplicationId()
    {
    return iCID;
    }

// -----------------------------------------------------------------------------
// COrphan::Message
// -----------------------------------------------------------------------------
CImpsFields* COrphan::Message()
    {
    return iFields;
    }

// -----------------------------------------------------------------------------
// COrphan::Expiry
// -----------------------------------------------------------------------------
TTime COrphan::Expiry()
    {
    return iExpiry;
    }

// -----------------------------------------------------------------------------
// COrphan::RemoveOwnership
// -----------------------------------------------------------------------------
void COrphan::RemoveOwnership()
    {
    iFields = NULL;
    }

//  End of File
