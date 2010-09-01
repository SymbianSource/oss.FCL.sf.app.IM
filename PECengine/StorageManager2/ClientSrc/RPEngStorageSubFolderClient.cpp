/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sub-client to managed particular session slot
*
*/


// INCLUDE FILES
#include "RPEngStorageSubFolderClient.h"

#include "RPEngStorageClient.h"
#include "TPEngStorageServerMessages.h"

#include "PresenceDebugPrint.h"

// PEC Engine internal constants
#include "PEngInternalGlobalConsts.h"

// MACROS
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }

#define SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType )\
    TIpcArgs messArgs;\
    messArgs.Set( KMessageSlot0, &aStoreId );\
    messArgs.Set( KMessageSlot1, aStoreType );

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::RPEngStorageSubFolderClient
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RPEngStorageSubFolderClient::RPEngStorageSubFolderClient( )
    {
    }

// Destructor
RPEngStorageSubFolderClient::~RPEngStorageSubFolderClient()
    {
    }

// =============================================================================
// ============================ Close/Connect FUNCTIONS ========================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::Connect()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::Connect(
    RPEngStorageClient& aStorageMainClient,
    const TDesC8& aSessionName )
    {
    PENG_DP_TXT( "RPEngStorageSubFolderClient::Connect()" );

    TIpcArgs messArguments;
    messArguments.Set( KMessageSlot0, &aSessionName );

    TInt err( RSubSessionBase::CreateSubSession(
                  aStorageMainClient,
                  EMainSessCreateFolderSubSession,
                  messArguments ) );

    iConnected = ( err == KErrNone ? ETrue : EFalse );
    return err;
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::Close()
// -----------------------------------------------------------------------------
//
void RPEngStorageSubFolderClient::Close()
    {
    PENG_DP_TXT( "RPEngStorageSubFolderClient::Close()" );


    CloseSubSession( EMainSessCloseSubSession );
    iConnected = EFalse;
    }


// =============================================================================
// ======================= Storage FUNCTIONS ===================================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::Store()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::Store(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType,
    const TDesC8& aData )
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    messArgs.Set( KMessageSlot2, &aData );
    return SendReceive( EFolderSubSessWriteEntry, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::Retrieve()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::Retrieve(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType,
    TDes8& aData )
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    messArgs.Set( KMessageSlot2, &aData );
    return SendReceive( EFolderSubSessReadEntry, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::Delete()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::Delete(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType )
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    return SendReceive( EFolderSubSessRemoveEntry, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::StoreEntryLocked()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::StoreEntryLocked(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType,
    TPengStorageLockPriority aPriority ) const
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    messArgs.Set( KMessageSlot2, aPriority );
    return SendReceive( EFolderSubSessIsEntryLocked, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::LockStoreEntry()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::LockStoreEntry(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType,
    TPengStorageLockPriority aPriority,
    TBool aCreateEntry ) const
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    // priority and Create flag will be transported as one TInt value
    TInt value( aPriority );
    value |= ( aCreateEntry  ? KLockEntryCreateMask : 0 );
    messArgs.Set( KMessageSlot2,  value );

    return SendReceive( EFolderSubSessLockEntry, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::UnlockStoreEntry()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::UnlockStoreEntry(
    const TDesC& aStoreId,
    TPEngStorageType aStoreType ) const
    {
    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aStoreId, aStoreType );
    return SendReceive( EFolderSubSessUnlockEntry, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::BufferServerSideNotifications()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::BufferServerSideNotifications(
    TInt aCount )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, aCount );
    return SendReceive( EFolderSubSessSuspendNotification, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::ReleaseServerSideBuffering()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::ReleaseServerSideBuffering(
    TInt aCount )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, aCount );
    return SendReceive( EFolderSubSessResumeNotification, messArgs );
    }


// =============================================================================
// ======================= NOTIFY FUNCTIONS ====================================
// =============================================================================


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::StartListeningSIdsChanges()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::StartListeningSIdsChanges(
    const TDesC& aEngagedSIds )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aEngagedSIds );
    return SendReceive( EFolderSubSessListenSIDsChanges, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::UpdateListenSIdsChangesScout()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::UpdateListenSIdsChangesScout(
    const TDesC& aEngagedSIds )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, EFolderSubSessListenSIDsChanges );
    messArgs.Set( KMessageSlot1, &aEngagedSIds );
    return SendReceive( EFolderSubSessUpdateListenSIDsScout, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::ReloadSIdsChangesScout()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::ReloadListenSIdsChangesScout(
    TRequestStatus& aStatus )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, EFolderSubSessListenSIDsChanges );
    SendReceive( EFolderSubSessReloadAsynchronousScout, messArgs, aStatus );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::FetchChangedSIdsIndexes()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::FetchChangedSIdsIndexes(
    TDes& aChangedIds )
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, EFolderSubSessListenSIDsChanges );
    messArgs.Set( KMessageSlot1, &aChangedIds );
    return SendReceive( EFolderSubSessFetchChangedSIDsIndexes, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::StopListeningSIdsChanges()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::StopListeningSIdsChanges()
    {
    // add Start listen ID to the message container,
    // this request is sort of cancel of request
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( 0, EFolderSubSessListenSIDsChanges );
    return SendReceive( EFolderSubSessCancelRequest, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngStorageSubFolderClient::NotifyChangedSIDsBlind()
// -----------------------------------------------------------------------------
//
TInt RPEngStorageSubFolderClient::NotifyChangedSIdBlind(
    const TDesC& aPackedChangedSId,
    TPEngStorageType aStoreType )
    {
    PENG_DP( D_PENG_LIT( "RPEngStorageSubFolderClient::NotifyChangedSIdBlind: %S" ),
             &aPackedChangedSId );

    RETURN_IF_NOT_CONNECTED();

    SET_MESSAGE_ARGUMENTS( aPackedChangedSId, aStoreType );

    return SendReceive( EFolderSubSessNotifyChangedSIDs, messArgs );
    }

//  End of File


