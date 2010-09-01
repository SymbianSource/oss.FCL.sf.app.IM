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
* Description:  Sub-session class of the Storage Server
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include	"CPEngStorageSubSession.h"
#include	"PEngStorageServerCommon.h"

#include	"CPEngStorageFolder.h"
#include	"MPEngStorageServer.h"
#include	"RPEngMessage.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStorageSubSession::CPEngStorageSubSession
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngStorageSubSession::CPEngStorageSubSession( TInt32 aSessionId )
        : iBufferedNotif( 10 ), // 10 as value to grow array
        iSessionId( aSessionId )
    {
    }


// -----------------------------------------------------------------------------
// CPEngStorageSubSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngStorageSubSession::ConstructL(
    MPEngStorageServer& aServer,
    const RPEngMessage& aMessage )
    {
    iStorageFolder = aServer.StorageFolderL( aMessage );
    }


// -----------------------------------------------------------------------------
// CPEngStorageSubSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageSubSession* CPEngStorageSubSession::NewL(
    MPEngStorageServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngStorageSubSession* self = NewLC( aServer, aMessage, aSessionId );

    CleanupStack::Pop(); // self

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngStorageSubSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngStorageSubSession* CPEngStorageSubSession::NewLC(
    MPEngStorageServer& aServer,
    const RPEngMessage& aMessage,
    TInt32 aSessionId )
    {
    CPEngStorageSubSession* self = new( ELeave ) CPEngStorageSubSession( aSessionId );

    CleanupClosePushL( *self );
    self->ConstructL( aServer, aMessage );
    return self;
    }


// Destructor
CPEngStorageSubSession::~CPEngStorageSubSession()
    {
    iBufferedNotif.Reset();
    // decrease count of the storage Folder
    if ( iStorageFolder )
        {
        iStorageFolder->CancelSubSessionRequests( iSessionId, iHandle );
        iStorageFolder->Close();
        }
    }



// =============================================================================
// =============== Functions from base class ===================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngStorageSubSession::DisplayName()
// -----------------------------------------------------------------------------
//
TBool CPEngStorageSubSession::DispatchMessageL(
    const RPEngMessage& aMessage,
    TInt aRequest )
    {
    TBool completeMessage( ETrue );
    switch ( aRequest )
        {
            /*********************************************************************/
            /*  Asynchronous Requests Management                                 */
            /*********************************************************************/

            // Cancel Asynchronous request Request
        case EFolderSubSessCancelRequest:
            {
            iStorageFolder->CancelRequest( aMessage,
                                           iSessionId,
                                           iHandle );
            break;
            }

        // Reload Asynchronous Scout on the Server side
        case EFolderSubSessReloadAsynchronousScout:
            {
            iStorageFolder->ReloadAsyncScoutL( aMessage,
                                               iSessionId,
                                               iHandle );
            completeMessage = EFalse;
            break;
            }

        /*********************************************************************/
        /*  Store Entry management                                           */
        /*********************************************************************/

        // Write store entry
        case EFolderSubSessWriteEntry:
            {
            const CPEngDataEntry* entry = iStorageFolder->WriteStoreEntryL(
                                              aMessage,
                                              !iBufferingActiveCounter,
                                              iSessionId,
                                              iHandle );
            if ( entry )
                {
                TInt err( iBufferedNotif.InsertInAddressOrder( entry ) );
                err = err == KErrAlreadyExists ? KErrNone : err;
                User::LeaveIfError( err );
                }
            break;
            }

        // Read store Entry
        case EFolderSubSessReadEntry:
            {
            iStorageFolder->ReadStoreEntryL( aMessage );
            break;
            }

        // Get Entry size
        case EFolderSubSessGetEntryLength:
            {
            iStorageFolder->GetStoreEntryLengthL( aMessage );
            break;
            }

        // Remove Store entry from the storage
        case EFolderSubSessRemoveEntry:
            {
            iStorageFolder->RemoveStoreEntryL( aMessage );
            break;
            }

        // Lock Store Entry
        case EFolderSubSessLockEntry:
            {
            iStorageFolder->LockStoreEntryL( aMessage,
                                             iSessionId,
                                             iHandle );
            completeMessage = EFalse;
            break;
            }

        // Unlock the Store entry
        case EFolderSubSessUnlockEntry:
            {
            // check if it is needed to buffer notification
            const CPEngDataEntry* entry =
                iStorageFolder->UnlockStoreEntryL(
                    aMessage,
                    !iBufferingActiveCounter,
                    iSessionId,
                    iHandle );
            if ( entry )
                {
                TInt err( iBufferedNotif.InsertInAddressOrder( entry ) );
                err = err == KErrAlreadyExists ? KErrNone : err;
                User::LeaveIfError( err );
                }
            completeMessage = EFalse;
            break;
            }

        // Is Store entry locked
        case EFolderSubSessIsEntryLocked:
            {
            iStorageFolder->IsStoreEntryLockedL( aMessage );
            completeMessage = EFalse;
            break;
            }

        // Suspend notification to the client
        case EFolderSubSessSuspendNotification:
            {
            iBufferingActiveCounter += aMessage.Int0();
            aMessage.Complete( iBufferingActiveCounter );
            completeMessage = EFalse;
            break;
            }

        // Resume notification to the client
        case EFolderSubSessResumeNotification:
            {
            ResumeSIDsNotification( aMessage.Int0() );
            aMessage.Complete( iBufferingActiveCounter );
            completeMessage = EFalse;
            break;
            }

        /*********************************************************************/
        /*   Storage ID changes notify management                            */
        /*********************************************************************/

        // Synchronous Request to register for notification
        case EFolderSubSessListenSIDsChanges:
            {
            iStorageFolder->ListenSIDsChangesL( aMessage,
                                                iSessionId,
                                                iHandle );
            break;
            }

        // Synchronous Request to update set of SIDs client
        case EFolderSubSessUpdateListenSIDsScout:
            {
            iStorageFolder->UpdateListenSIDsScoutL( aMessage,
                                                    iSessionId,
                                                    iHandle );
            break;
            }

        // Fetch indexes of changed SIds
        case EFolderSubSessFetchChangedSIDsIndexes:
            {
            iStorageFolder->FetchChangedIdsIndexesL( aMessage,
                                                     iSessionId,
                                                     iHandle );
            break;
            }

        // Synchronous Request to force notify Store Entry change
        case EFolderSubSessNotifyChangedSIDs:
            {
            // check if it is needed to buffer notification
            const CPEngDataEntry* entry =
                iStorageFolder->NotifyChangedStorageIdL(
                    aMessage,
                    !iBufferingActiveCounter );
            if ( entry )
                {
                TInt err( iBufferedNotif.InsertInAddressOrder( entry ) );
                err = err == KErrAlreadyExists ? KErrNone : err;
                User::LeaveIfError( err );
                }

            break;
            }

        default:
            {
            PanicClient( aMessage, ESSBadRequest );
            break;
            }
        }
    // check if message can be completed
    completeMessage &= aMessage.MessageValid();
    return completeMessage;
    }

// -----------------------------------------------------------------------------
// CPEngStorageSubSession::SetSubSesionHandle()
// -----------------------------------------------------------------------------
//
void CPEngStorageSubSession::SetSubSesionHandle(
    TInt aHandle )
    {
    iHandle = aHandle;
    }


// =============================================================================
// =============== Private Functions from base class ===========================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngStorageSubSession::PanicClient()
// Panic client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageSubSession::PanicClient(
    const RPEngMessage& aMessage,
    const TInt aPanic ) const
    {
    aMessage.Panic( KStorageSessionName, aPanic );
    }


// -----------------------------------------------------------------------------
// CPEngStorageSubSession::ResumeSIDsNotificationL()
// Resume Notification of the SIDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngStorageSubSession::ResumeSIDsNotification(
    TInt aCount )
    {
    iBufferingActiveCounter -= aCount;
    if ( iBufferingActiveCounter <= KErrNone )
        {
        // notify buffered SIDs
        iBufferingActiveCounter = KErrNone;
        iStorageFolder->NotifySIDChanges( iBufferedNotif );
        iBufferedNotif.Reset();
        }
    }

//  End of File
