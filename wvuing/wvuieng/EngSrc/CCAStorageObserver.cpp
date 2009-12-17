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
* Description:  Observes storage events and reacts according to settings
*
*/

// INCLUDE FILES
#include "CCAStorageObserver.h"
#include "PrivateEngineDefinitions.h"
#include "PublicEngineDefinitions.h"
#include "ImpsCSPAllErrors.h"
#include "CAUtils.h"

#include "MCASettings.h"
#include "MCAStoredContacts.h"
#include "MCAStoredContact.h"
#include "MCASettings.h"
#include "MCABlocking.h"

#include    "ChatDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCAStorageObserver* CCAStorageObserver::NewL( MCASettings& aApplicationSettings,
                                              MCAStoredContacts& aContactStorage,
                                              MCAPresence& aPresenceHandler,
                                              MCABlocking& aBlockingManager )
    {
    CCAStorageObserver* self = new ( ELeave ) CCAStorageObserver(
        aApplicationSettings,
        aContactStorage,
        aPresenceHandler,
        aBlockingManager );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAStorageObserver::~CCAStorageObserver()
    {
    iContactStorage.RemoveObserver( this );
    delete iLastUserId;
    }


// ---------------------------------------------------------
// CCAStorageObserver::HandleDelete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::HandleContactDelete( const TDesC& aId )
    {
    CHAT_DP_TXT( "CCAStorageObserver::HandleDelete" );
    // find also the "soon-to-be-deleted" contacts
    IMHandleDelete( aId );
    }

// ---------------------------------------------------------
// CCAStorageObserver::HandleAddition
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::HandleAddition( MCAContactList& /*aList*/, MCAStoredContact& aContact )
    {
    CHAT_DP_TXT( "CCAStorageObserver::HandleAddition" );
    IMHandleAddition( aContact );
    }

// ---------------------------------------------------------
// CCAStorageObserver::HandleChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::HandleChange(
    MCAContactList* /* aList */,
    MCAStoredContact* /* aId */,
    TStorageManagerGlobals::TCAObserverEventType /*aEventType*/,
    TBool /*aUserIdChanged*/ )
    {
    // nothing to do
    }

// ---------------------------------------------------------
// CCAStorageObserver::HandleBackupRestoreEvent
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::HandleBackupRestoreEvent(
    const TStorageManagerGlobals::TCAObserverEventType /*aEventType*/ )
    {
    }

// Symbian OS default constructor can leave.
void CCAStorageObserver::ConstructL()
    {
    iContactStorage.AddObserverL( this );
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAStorageObserver::CCAStorageObserver( MCASettings& aApplicationSettings,
                                        MCAStoredContacts& aContactStorage,
                                        MCAPresence& aPresenceHandler,
                                        MCABlocking& aBlockingManager )
        :	iApplicationSettings( aApplicationSettings ),
        iContactStorage( aContactStorage ),
        iPresenceHandler( aPresenceHandler ),
        iBlockingManager( aBlockingManager )
    {
    }

// ---------------------------------------------------------
// CCAStorageObserver::IMHandleDelete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::IMHandleDelete( const TDesC& aUserId )
    {
    TInt err( KErrNone );

    if ( iApplicationSettings.Value( MCASettings::EReceiveIMessages )
         == MCASettings::EFriends )
        {
        TRAP( err, DoBlockingManagerDeleteL( aUserId ) );
        HandleError( err );
        }
    }

// ---------------------------------------------------------
// CCAStorageObserver::DoBlockingManagerDeleteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::DoBlockingManagerDeleteL( const TDesC& aUserId )
    {
    // granularity of one because only one item appended to array
    CDesCArrayFlat* arr = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( arr );
    arr->AppendL( aUserId );

    // remove from the grant list, according to strategy
    TInt err( iBlockingManager.RemoveL( NULL, arr ) );
    if ( err != KErrNone )
        {
        HandleError( err );
        }

    CleanupStack::PopAndDestroy( arr );
    }

// ---------------------------------------------------------
// CCAStorageObserver::IMHandleAddition
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::IMHandleAddition( MCAStoredContact& aContact )
    {
    TInt err( KErrNone );

    if ( iApplicationSettings.Value( MCASettings::EReceiveIMessages )
         == MCASettings::EFriends )
        {
        TRAP( err, DoBlockingManagerAdditionL( aContact.UserId() ) );
        HandleError( err );
        }
    DoUpdateContactStatus( aContact );
    }


// ---------------------------------------------------------
// CCAStorageObserver::DoBlockingManagerAdditionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::DoBlockingManagerAdditionL( const TDesC& aUserId )
    {
    // granularity of one because only one item appended to array
    CDesCArrayFlat* arr = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( arr );
    arr->AppendL( aUserId );

    // insert into the grant list, according to strategy
    iBlockingManager.InsertL( NULL, arr );
    CleanupStack::PopAndDestroy( arr );

    }

// ---------------------------------------------------------
// CCAStorageObserver::DoUpdateContactStatus
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::DoUpdateContactStatus( MCAStoredContact& aContact )
    {
    TPtrC wvid( aContact.UserId() );

    // update the blocking status..
    TBool blocked( EFalse );
    const CDesCArray* blockList = iBlockingManager.BlockedList();
    if ( blockList )
        {
        // do a case-insensitive search
        TInt count( blockList->Count() );
        for ( TInt i( 0 ); ( i < count ) && !blocked ; ++i )
            {
            // (*blockList)[i].CompareC( wvid, KCollationLevel, NULL )
            if ( CAUtils::NeutralCompare( ( *blockList )[i], wvid ) == 0 )
                {
                blocked = ETrue;
                }
            }
        }
    aContact.SetBlocked( blocked );
    }

// ---------------------------------------------------------
// CCAStorageObserver::HandleError
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStorageObserver::HandleError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCAStorageObserver::HandleError, aError = %d" ),
             aError );
    if ( ( aError > Imps_ERROR_BASE ) && ( aError < KErrNone ) )
        {
        // propagate system errors to current active scheduler,
        // it should show a note
        CActiveScheduler::Current()->Error( aError );
        }

    // imps errors are ignored at the moment as we don't have any
    // notes specified for them
    }


//  End of File
