/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manager for presence-list handling
*               (Contact-list updater/ Attribute-list publisher ).
*
*/



// INCLUDE FILES
#include "CCAPEngListManager.h"
#include "ChatDebugPrint.h"
#include "CAPresenceDefinitions.h"
#include "MCASettings.h"
#include "CAPresenceUtils.h"
#include "CCAPresenceErrors.h"
#include "CAPresenceConst.h"
#include "CCAPEngListObserver.h"
#include "ImpsErrors.h"
#include "MCAPresenceError.h"

#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"
#include "MCAStoredContact.h"
#include "MCAContactList.h"
#include "MCAPresenceUpdater.h"

#include "CAUtils.h"
#include "impsbuilddefinitions.h"
#include "ChatDebugAssert.h"

#include <MPEngAttributeList2.h>
#include <MPEngContactList2.h>
#include <PEngWVPresenceAttributes2.h>
#include <CPEngContactListTransaction2.h>
#include <CPEngContactListStore2.h>
#include <CPEngAttributeListStore2.h>
#include <CPEngAttributeListTransaction2.h>
#include <CPEngPresenceNotifier2.h>
#include <MPEngTransactionStatus2.h>
#include <MPEngPresenceObserver2.h>
#include <MPEngContactListProperties.h>
#include <PEngPresenceEngineConsts2.h>
#include <TPEngContactListBaseSettings.h>
#include "MCAListInitObserver.h"
#include <impspresenceconnectionuiconstsng.h>
#include <CPEngNWSessionSlot2.h>
#include <CPEngNWSessionSlotID2.h>

// =============================== CONSTANTS ===================================

const TInt KMaxIdattempts = 50; // max amount of attempts to find unused id


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPEngListManager::CCAPEngListManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPEngListManager::CCAPEngListManager( RArray<TUint32>& aHandledAttributes,
                                        MPEngPresenceObserver2& aNotifierObserver,
                                        MCASettings& aApplicationSettings,
                                        MCAPresenceUpdater& aUpdater )
        : iOperationError( KErrNone ),
        iIsLoggedIn( EFalse ),
        iAttributes( aHandledAttributes ),
        iNotifierObserver( aNotifierObserver ),
        iApplicationSettings( &aApplicationSettings ),
        iPresenceUpdater( aUpdater ),
        iMainViewProcessObserver( NULL ),
        iContactFetchingComplete( EFalse )
    {
    CHAT_DP_TXT( "CCAPEngListManager::CCAPEngListManager()" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::ConstructL( CPEngNWSessionSlotID2* aSessionSlotID )
    {
    CHAT_DP_TXT( "CCAPEngListManager::ConstructL()" );

    iPEngContactListUpdater =
        CPEngContactListTransaction2::NewL( *aSessionSlotID );

    iPEngAttributeListPublisher =
        CPEngAttributeListTransaction2::NewL( *aSessionSlotID );

    iPEngContactListStore = CPEngContactListStore2::NewL( *aSessionSlotID );
    iPEngAttributeListStore = CPEngAttributeListStore2::NewL( *aSessionSlotID );

    iPEngAttributeNotifier = CPEngPresenceNotifier2::NewL( *aSessionSlotID );
    User::LeaveIfError( iPEngAttributeNotifier->AddObserver( *this ) );

    iErrors = CCAPresenceErrors::NewL();

    iListObserver = CCAPEngListObserver::NewL( aSessionSlotID,
                                               *iPEngAttributeNotifier, iAttributes, iPresenceUpdater );

    iAttribListSupported = AttribListsSupportedL( *aSessionSlotID );

#ifndef IMPS_CONTACT_FETCH_BACKGROUND
    // Start listening for presence events.
    RestartNotifierL();
#endif //IMPS_CONTACT_FETCH_BACKGROUND
    iObserver = NULL;
    iListInitInProcees = ETrue;
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPEngListManager* CCAPEngListManager::NewL(
    MCASettings& aApplicationSettings,
    RArray<TUint32>& aHandledAttributes,
    MPEngPresenceObserver2& aNotifierObserver,
    CPEngNWSessionSlotID2* aSessionSlotID,
    MCAPresenceUpdater& aUpdater )
    {
    CHAT_DP_TXT( "CCAPEngListManager::NewL()" );

    CCAPEngListManager* self = new( ELeave ) CCAPEngListManager(
        aHandledAttributes,
        aNotifierObserver,
        aApplicationSettings,
        aUpdater );

    CleanupStack::PushL( self );
    self->ConstructL( aSessionSlotID );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCAPEngListManager::~CCAPEngListManager()
    {
    CHAT_DP_TXT( "CCAPEngListManager::~CCAPEngListManager()" );
    ReleaseListManager( EFalse );
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::ReleaseListManager
//
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::ReleaseListManager(
    TBool /*aReleaseContactListModels*/ )
    {
    CHAT_DP_TXT( "CCAPEngListManager::~CCAPEngListManager()" );

    delete iPEngAttributeListPublisher;
    iPEngAttributeListPublisher = NULL;
    delete iPEngContactListUpdater;
    iPEngContactListUpdater = NULL;
    delete iPEngContactListStore;
    iPEngContactListStore = NULL;
    delete iPEngAttributeListStore;
    iPEngAttributeListStore = NULL;
    delete iPEngAttributeNotifier;
    iPEngAttributeNotifier = NULL;
    delete iErrors;
    iErrors = NULL;

    delete iListObserver;
    iListObserver = NULL;

    delete iIdle;
    iIdle = NULL;

    delete iFetchIdle;
    iFetchIdle = NULL;

    delete iListIds;
    iListIds = NULL;

    iAppendContactsArray.Close();

    TInt count = iUpdateListQueue.Count();
    iUpdateListQueue.ResetAndDestroy();

    iContactFetchStep = MCAContactLists::EBaseSynchronise;

    // If this assert fails then all changes were not updated to network!
    // Check the implemententation of DeleteServerContactL
    __CHAT_ASSERT_DEBUG( count == 0 )
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SetLoggedIn
// Sets network state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SetLoggedIn( TBool aIsLoggedIn,
                                      CPEngNWSessionSlotID2* /*aSessionSlotID*/ )
    {
    if ( iIsLoggedIn && !aIsLoggedIn ) // Logout after login
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::SetLoggedIn: \
                             Release old lists." ) );
        // Release contact storage
        CancelPendingRequests();
        ReleaseListManager( ETrue );
        }

    iIsLoggedIn = aIsLoggedIn;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::IsLoggedIn
// Returns network state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngListManager::IsLoggedIn() const
    {
    return iIsLoggedIn;
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::FetchContacts
// Launch next step of fetchcontacts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::FetchContacts( TAny *aInstance )
    {
    // called by CIdle to fetch the contacts on background

    CCAPEngListManager* instance = static_cast<CCAPEngListManager*>( aInstance );
    TInt launchAgain = EFalse;
    TRAPD( error, launchAgain = instance->FetchStateMachineL() );
    if ( error != KErrNone )
        {
        switch ( instance->iContactFetchStep )
            {
            case EPublishAttributeLists:
                {
                // creating the list of lists failed, sync can't continue
                CActiveScheduler::Current()->Error( error );
                instance->iContactFetchStep = ESynchronizeDone;
                break;
                }
            default:
                {
                MCAStoredContacts* storage = NULL;
                TRAP_IGNORE( storage = CCAStorageManagerFactory::ContactListInterfaceL() );
                if ( storage )
                    {
                    // list fetching failed, try next list
                    if ( storage->SetNextForSynchroniseProcess() != KErrNotFound )
                        {
                        return ETrue;
                        }
                    }
                // next list can't be tried, sync can't continue
                if ( error == KErrNoMemory )
                    {
                    CActiveScheduler::Current()->Error( error );
                    }
                instance->iContactFetchStep = ESynchronizeDone;
                break;
                }
            }
        }

    return launchAgain;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::FetchStateMachineL
// Launch next step of FetchStateMachineL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::FetchStateMachineL()
    {
    TBool launchAgain = EFalse;

    MCAStoredContacts* storage = CCAStorageManagerFactory::ContactListInterfaceL();
    switch ( iContactFetchStep )
        {
        case EBaseSynchronise: // -> ContactListTransactionComplete / Error
            {
            // Can leave cause first time is called from UI.
            // Base synchronise for getting lists.
            TBool autoUpdate =
                iApplicationSettings->Value( MCASettings::EAutomaticPresenceUpdate );

            TBool needToUnSubscribe = EFalse;
            //Mode is manual or automatic there is no need of sending unsubscribe message..

            User::LeaveIfError(
                iPEngContactListUpdater->BaseSynchronizeContactLists( *this, needToUnSubscribe ) );

            iContactFetchStep = EPublishAttributeLists;
            break;
            }
        case EPublishAttributeLists:
            {
            iContactFetchStep = EInitializeLists;
            launchAgain = ETrue;
            UpdatePresenceAuthorizeL();
            break;
            }
        case EInitializeLists:
            {
            TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );
            TInt count( contactLists.MdcaCount() );
            if ( count == 0 ) // no action needed cause no lists.
                {
                iContactFetchStep = ESynchronizeDone;
                storage->ResetContactLists();   // Used to send notification to UI

                iListInitInProcees = EFalse;
                if ( iObserver )
                    {
                    iObserver->HandleListInitChange( !iListInitInProcees );
                    }

                return KErrNone;
                }
            // Append lists to storage.
            for ( TInt a( 0 ); a < count; ++a )
                {
                TPtrC listId( contactLists.MdcaPoint( a ) );
                TPtrC displayName( CAUtils::DisplayId( listId, ETrue ) );
                MCAContactList* newCaList =
                    storage->AddContactListIdL( listId, displayName );
                }
            // notify the View to show waiting nore for synch
            if ( storage->InitSynchroniseProcess() != KErrNotFound )
                {
                iContactFetchStep = ESynchronizeList;
                launchAgain = ETrue;
                }
            else
                {
                iContactFetchStep = ESynchronizeDone;
                }

            // Reset notifier.
            iListObserver->ClearListNotifier();
            break;
            }
        case ERestartSynchronize:
            {
            if ( storage->InitSynchroniseProcess() > KErrNotFound )
                {
                iContactFetchStep = ESynchronizeList;
                launchAgain = ETrue;
                }
            else
                {
                iContactFetchStep = ESynchronizeDone;
                }

            // Reset notifier.
            iListObserver->ClearListNotifier();
            break;
            }
        case ESynchronizeList: //-> ContactListTransactionCompleteL / Error
            {
            MCAContactList& list = storage->ListInSyncProcessL();
            iPEngContactListUpdater->SynchronizeContactList( list.ListId(), *this );
            iContactFetchStep = ESubscribeList;
            // init completed
            break;
            }
        case ESubscribeList: // SubscribeListL / HandleContactListTransactionCompleteL / Error
            {
            MCAContactList& list = storage->ListInSyncProcessL();
            TBool autoUpdate( iApplicationSettings->Value( MCASettings::EAutomaticPresenceUpdate ) );
            TPtrC listId( list.ListId() );
            TRAPD( error,
                   MPEngContactList2& cntList = iPEngContactListStore->ContactListL( listId );
                   // subscribe the list
                   iContactFetchStep = EFetchContacts;
                   SubscribeListL( cntList, listId, autoUpdate, ETrue );
                 );

            if ( error != KErrNone )
                {
                // sync failed, try next list
                list.SetSynchronised( MCAContactList::ESynchroniseFailed );
                User::Leave( error );
                }
            CDesCArray* contactlist = new( ELeave ) CDesCArrayFlat( 1 );
            CleanupStack::PushL( contactlist );
            contactlist->AppendL( listId );
            RestartNotifierL( contactlist );
            CleanupStack::PopAndDestroy( contactlist );
            iListInitInProcees = EFalse;
            if ( iObserver )
                {
                iObserver->HandleListInitChange( !iListInitInProcees );
                }

            break;
            }
        case EFetchContacts: // DoAppendContacts
            {
            MCAContactList& list = storage->ListInSyncProcessL();
            TPtrC listId( list.ListId() );

            // Make sure we have list in storage
            TAppendContactsParams appendParams = {
                NULL,
                NULL,
                NULL,
                storage,
                0,
                &list
                };
            TRAPD( error,
                   MPEngContactList2& cntList = iPEngContactListStore->ContactListL( listId );
                   appendParams.iContactList = &cntList;
                   appendParams.iEntryIndex = ( cntList.Count() - 1 );
                   User::LeaveIfError( iAppendContactsArray.Append( appendParams ) );

                   if ( !iIdle )
            {
            iIdle = CIdle::NewL( CActive::EPriorityIdle );
                }
            if ( !iIdle->IsActive() )
            {
            iIdle->Start( TCallBack( AppendContacts, this ) );
                }

            iContactFetchStep = EFinalizeList;
                 );
            if ( error != KErrNone )
                {
                // sync failed, try next list
                list.SetSynchronised( MCAContactList::ESynchroniseFailed );
                User::Leave( error );
                }
            break;
            }
        case EFinalizeList:
            {
            MCAContactList& list = storage->ListInSyncProcessL();
            TPtrC listId( list.ListId() );

            TRAPD( error,
                   MPEngContactList2& cntList = iPEngContactListStore->ContactListL( listId );
                   TPtrC displayName;
                   TInt success = ( cntList.ListProperties().DisplayName( displayName ) == KErrNone );
                   if ( success )
            {
            list.SetDisplayNameL( displayName );
                }
            list.SetSynchronised();

            // update list notifier
            iListObserver->AddL( listId );
                 );
            if ( error != KErrNone )
                {
                list.SetSynchronised( MCAContactList::ESynchroniseFailed );
                User::LeaveIfError( error );
                }

            // Keep lists sorted
            storage->SortContactLists();
            if ( storage->SetNextForSynchroniseProcess() > KErrNotFound )
                {
                // Then next one
                iContactFetchStep = ESynchronizeList;
                launchAgain = ETrue;
                }
            else
                {
                // All synchronised
                // and subscribed to network
                iContactFetchStep = EPublishListsDone;
                launchAgain = ETrue;
                }
            break;
            }
        case EPublishListsDone:
            {
            iContactFetchStep = ESynchronizeDone;
            RestartNotifierL();
            launchAgain = iStartFetchAgain;
            iStartFetchAgain = EFalse;

            iContactFetchingComplete = ETrue;
            if ( iMainViewProcessObserver )
                {
                iMainViewProcessObserver->HandleListInitChange( ETrue );
                }
            break;
            }
        default: //Should not come here
            {
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoFetchContactsL : Done" ) );
    return launchAgain;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::FetchContactsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
TInt CCAPEngListManager::FetchContactsL( TBool aBaseSyncDone /*=EFalse*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoFetchContactsL : Enter" ) );

    // called from UI to start and restart the fetching of contact lists
    switch ( iContactFetchStep )
        {
        case EBaseSynchronise:
            {
            iBaseSyncDone = aBaseSyncDone;
            if ( iBaseSyncDone )
                {
                iContactFetchStep = EInitializeLists;
                }

            // starting the sync for the first time
            TBool ret = FetchStateMachineL();
            if ( ret )
                {
                StartFetchIdle();
                }
            return ret;
            }
        case ESynchronizeDone:
            {
            // sync already done once, restart it
            iContactFetchStep = ERestartSynchronize;
            StartFetchIdle();
            break;
            }
        default:
            {
            // sync ongoing, needs to be restarted after it has finished the first round
            iStartFetchAgain = ETrue;
            break;
            }
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoFetchContactsL : Done" ) );
    return KErrNone;
    }

#else // IMPS_CONTACT_FETCH_BACKGROUND
TInt CCAPEngListManager::FetchContactsL( TBool /*aBaseSyncDone*/ /*=EFalse*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoFetchContactsL : Enter" ) );

    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );
    TInt count( contactLists.MdcaCount() );
    if ( count == 0 )
        {
        return KErrNone;
        }
    // update list notifier
    iListObserver->SetListIds( contactLists );

    MCAStoredContacts* storage =
        CCAStorageManagerFactory::ContactListInterfaceL();
    iOperationError = KErrNone;

    TBool autoUpdate( iApplicationSettings->Value(
                          MCASettings::EAutomaticPresenceUpdate ) );

    RPointerArray<MPEngContactList2> listModels;
    CleanupClosePushL( listModels );
    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        MPEngContactList2& cntList = iPEngContactListStore->ContactListL( contactLists.MdcaPoint( x ) );
        listModels.AppendL( &cntList );
        }

    for ( TInt i( 0 ); i < count && iOperationError == KErrNone; ++i )
        {
        MPEngContactList2& list = *( listModels[ i ] );
        TPtrC listId( list.ListProperties().Name() );

        // subscribe the list
        SubscribeListL( list, listId, autoUpdate, EFalse );

        // Make sure we have list in storage
        TPtrC displayName;
        TInt success = ( list.ListProperties().DisplayName( displayName )
                         == KErrNone );

        // do not notify yet
        MCAContactList* newCaList = storage->AddContactListIdL( listId,
                                                                success ? displayName :
                                                                CAUtils::DisplayId( listId, ETrue ),
                                                                EFalse );

        // anb fetch contacts
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::FetchContactsL, Get \
                              contact-list(%S) from store" ), &listId );

        TAppendContactsParams appendParams = {
            &list,
            NULL,
            NULL,
            storage,
            // we go from the end this time
            ( list.Count() - 1 ),
            newCaList
            };

        iAppendContactsParams = appendParams;

        // Idle loop for storing contacts to local storage
        CIdle* idle = CIdle::NewL( CActive::EPriorityIdle );
        CleanupStack::PushL( idle );

        idle->Start( TCallBack( AppendContacts, this ) );
        iRequestQueue.WaitForResponseL( TCARequestQueue::EAppendContactsWait );

        // notify about new contact list, now when all is added
        storage->SendContactListUpdatedEvent( *newCaList );

        CleanupStack::PopAndDestroy( idle );
        }

    // and subscribe to network
    iPEngContactListUpdater->PublishContactLists( contactLists, *this );
    iRequestQueue.WaitForResponseL( TCARequestQueue::EContactListUpdater );

    CleanupStack::PopAndDestroy(); // contactLists
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoFetchContactsL : Exit" ) );
    return iOperationError;
    }

#endif //IMPS_CONTACT_FETCH_BACKGROUND    

// -----------------------------------------------------------------------------
// CCAPEngListManager::SubscribeListsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SubscribeListsL( TBool aSubscribe )
    {
    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );
    TInt count( contactLists.MdcaCount() );
    if ( count == 0 )
        {
        return;
        }

    for ( TInt i( 0 ); i < count && iOperationError == KErrNone; ++i )
        {
        TPtrC listId( contactLists.MdcaPoint( i ) );
        MPEngContactList2& list = iPEngContactListStore->ContactListL( listId );

        // subscribe the list
        // EFalse as fourth parameter to avoid updating to
        // network at this point
        SubscribeListL( list, listId, aSubscribe, EFalse );
        }

    // Update all lists to network at once
    User::LeaveIfError( iPEngContactListUpdater->PublishContactLists(
                            contactLists, *this ) );
    // Wait for request to complete
    iRequestQueue.WaitForResponseL(
        TCARequestQueue::EContactListUpdater );

    if ( aSubscribe )
        {
        RestartNotifierL( &contactLists );
        }
    else
        {
        if ( iPEngAttributeNotifier->IsActive() )
            {
            iPEngAttributeNotifier->Stop();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::IsContactFetchingCompleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCAPEngListManager::IsContactFetchingCompleted()
    {
    return iContactFetchingComplete;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AddMainViewProcessObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCAPEngListManager::AddMainViewProcessObserver( MCAListInitObserver* aObserver )
    {
    if ( !iMainViewProcessObserver )
        {
        iMainViewProcessObserver = aObserver;
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::RemoveMainViewProcessObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCAPEngListManager::RemoveMainViewProcessObserver()
    {
    iMainViewProcessObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::FetchServerContactsL
// Fetches given server-side contact-list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::FetchServerContactsL(  const TDesC& aContactListId,
                                                CPtrCArray& aWVIds )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::FetchServerContactsL, Get \
                          contact-list(%S) from store" ), &aContactListId );

    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    // Fill arrays
    TInt count( list.Count() );
    MPEngContactItem* contact = NULL;
    for ( TInt i( 0 ); i < count; ++i )
        {
        contact = &list.ContactItem( i );
        aWVIds.AppendL( contact->Id() );
        }
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::AppendContacts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::AppendContacts( TAny *aInstance )
    {
    return static_cast<CCAPEngListManager*>( aInstance )->
           DoAppendContacts();
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::StartFetchIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::StartFetchIdle()
    {
    TRAPD( err,
           if ( !iFetchIdle )
    {
    // if the idle does not exist, create it
    iFetchIdle = CIdle::NewL( CActive::EPriorityIdle );
        }
    if ( !iFetchIdle->IsActive() )
    {
    // and if it isn't started, do it
    iFetchIdle->Start( TCallBack( FetchContacts, this ) );
        }
         );
    return err;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::DoAppendContacts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::DoAppendContacts()
    {
    CHAT_DP_TXT( "CCAPEngListManager::DoAppendContacts" );
    TBool retval( EFalse );
    MCAStoredContacts* contactInterface = NULL;

#ifndef IMPS_CONTACT_FETCH_BACKGROUND

    TRAP( iOperationError, contactInterface =
              CCAStorageManagerFactory::ContactListInterfaceL();
          contactInterface->SetContactListLock( ETrue );
          retval = CAPresenceUtils::AppendContactsL(
                       iAppendContactsParams.iContactList,
                       iAppendContactsParams.iWVIds,
                       iAppendContactsParams.iNicknames,
                       iAppendContactsParams.iContactStorage,
                       iAppendContactsParams.iEntryIndex,
                       *( iAppendContactsParams.iCaContactList ),
                       iPresenceUpdater ) );

    if ( contactInterface )
        {
        contactInterface->SetContactListLock( EFalse );
        }

    if ( iOperationError != KErrNone || !retval )
        {
        iRequestQueue.ResponseReceived( TCARequestQueue::EAppendContactsWait );
        return EFalse;
        }
    return retval;
#else
    if ( !iAppendContactsArray.Count() ) // No appendcontacts array to process.
        {
        if ( iContactFetchStep == EFinalizeList )
            {
            StartFetchIdle();
            }
        return EFalse;
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DoAppendContacts, \
                      Count of append packets %d" ), iAppendContactsArray.Count() );

    TAppendContactsParams& appendContacts = iAppendContactsArray[ 0 ];

    TRAP( iOperationError, contactInterface =
              CCAStorageManagerFactory::ContactListInterfaceL();
          contactInterface->SetContactListLock( ETrue );
          retval = CAPresenceUtils::AppendContactsL(
                       appendContacts.iContactList,
                       appendContacts.iWVIds,
                       appendContacts.iNicknames,
                       appendContacts.iContactStorage,
                       appendContacts.iEntryIndex,
                       *( appendContacts.iCaContactList ),
                       iPresenceUpdater ) );

    if ( iOperationError != KErrNone || !retval )
        {
        if ( contactInterface )
            {
            contactInterface->SetContactListLock( EFalse );
            }
        iAppendContactsArray.Remove( 0 );
        }
    TBool retVal = TBool( iAppendContactsArray.Count() );
    if ( !retVal && iContactFetchStep == EFinalizeList )
        {
        StartFetchIdle();
        }
    return retVal;
#endif // IMPS_CONTACT_FETCH_BACKGROUND   
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SubscribeListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SubscribeListL( MPEngContactList2& aList,
                                         const TDesC& aListId, TBool aSubscribe,
                                         TBool aUpdate /* = ETrue */ )
    {
    MPEngContactListProperties& contactListProperties = aList.ListProperties();
    // setting subscription attributes also sets up the subscription
    // property
    TBool update( EFalse );
    TBool bgFetch( iContactFetchStep == EFetchContacts );
    if ( aSubscribe )
        {
        update = contactListProperties.SetSubscriptionAttributesL(
                     iAttributes.Array() );
        CHAT_DP( D_CHAT_LIT( "Subscribe ready" ) );
        }
    else
        {
        // set the subscription property
        update = contactListProperties.DeActivateSubscriptionL();
        CHAT_DP( D_CHAT_LIT( "Unsubscribe ready" ) );
        }

    if ( update && aUpdate )
        {
        UpdateToNetworkL( EContactListUpdate, aListId
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                          , !bgFetch // don't wait for response when bg fetch is ongoing
#endif //IMPS_CONTACT_FETCH_BACKGROUND     
                        );
        }
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    else if ( bgFetch )
        {
        // step to next state if no updating needed
        FetchStateMachineL();
        }
#endif //IMPS_CONTACT_FETCH_BACKGROUND     

    CHAT_DP( D_CHAT_LIT( "UpdateL called" ) );
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::DeleteServerContactL
// Deletes one contact from server side contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::DeleteServerContactL(
    const TDesC& aContactListId,
    const TDesC& aWVId,
    const TBool aUpdateToServer /*ETrue*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteServerContactL, Get \
                          contact-list(%S) from store" ), &aContactListId );
    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteServerContactL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    TRAPD( err, list.RemoveContactL( aWVId ) );
    CAPresenceUtils::HandleListErrorsL( err );

    // update will destroy wv id descriptor, need to store it localy
    HBufC* userId = aWVId.AllocLC();
    // Update changes to network or append to queue
    if ( aUpdateToServer )
        {
        UpdateToNetworkL( EContactListUpdate, aContactListId );
        }
    else
        {
        HBufC* listId = aContactListId.AllocLC();
        iUpdateListQueue.AppendL( listId );
        CleanupStack::Pop( listId );
        }

    TContactListsWrapper contactLists = FetchServerContactListsL();
    TInt count( contactLists.MdcaCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        TPtrC listId( contactLists.MdcaPoint( x ) );
        if ( KErrNone == listId.CompareF( aContactListId ) )
            {
            // same list as we remove it from, no need to check
            continue;
            }
        // is
        MPEngContactList2& list =
            iPEngContactListStore->ContactListL( listId );
        if ( KErrNone == list.FindContact( *userId ) )
            {
            // wv id still exists in another contact list
            CleanupStack::PopAndDestroy();
            return;
            }
        }
    // it was not dounf anywhere anymore, remove it from notifier
    RemoveFromNotifierL( *userId );
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::DeleteServerContactL
// Deletes several contacts from server side contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::DeleteServerContactL(
    const TDesC& aContactListId,
    const MDesCArray& aWVIds,
    const TBool aUpdateToServer /*ETrue*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteServerContactL (several), \
                          Get contact-list(%S) from store" ), &aContactListId );
    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteServerContactL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    TInt count( aWVIds.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TRAPD( err, list.RemoveContactL( aWVIds.MdcaPoint( i ) ) );
        CAPresenceUtils::HandleListErrorsL( err );
        }

    iListObserver->SkipStorageRemoval();

    // Update changes to network or append to queue
    TInt err;
    if ( aUpdateToServer )
        {
        TRAP( err, UpdateToNetworkL( EContactListUpdate, aContactListId ) );
        }
    else
        {
        HBufC* listId = aContactListId.AllocLC();
        iUpdateListQueue.AppendL( listId );
        CleanupStack::Pop( listId );
        }

    // restart notifier after the list is really updated
    RestartNotifierL();

    // and propagate the error
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::CommitDeleteServerContactL
// Updates deleted contacts to server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::CommitDeleteServerContactL()
    {
    // Check connection
    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CommitDeleteServerContactL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    if ( iUpdateListQueue.Count() == 0 )
        {
        // nothing to update
        return;
        }

    // construct MDesCArray from RPointerArray
    TInt count = iUpdateListQueue.Count();
    CDesCArray* lists = new ( ELeave ) CDesCArrayFlat( count );
    CleanupStack::PushL( lists );
    for ( TInt i = 0; i < count; ++i )
        {
        TInt pos;
        if ( lists->Find( *iUpdateListQueue[i], pos ) != 0 )
            {
            // not already in array
            lists->AppendL( *iUpdateListQueue[i] );
            }
        }

    // update to network
    User::LeaveIfError( iPEngContactListUpdater->PublishContactLists(
                            *lists, *this ) );
    iRequestQueue.WaitForResponseL( TCARequestQueue::EContactListUpdater );

    // clean queue
    CleanupStack::PopAndDestroy();
    iUpdateListQueue.ResetAndDestroy();

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CommitDeleteServerContactL, \
                          contact-list update done (%d)" ), iOperationError );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AddServerContactL
// Adds one contact to server side contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAPEngListManager::AddServerContactL( const TDesC& aContactListId,
                                                         const TDesC& aWVId,
                                                         const TDesC& aNickname,
                                                         TBool /*aCanRemove  = ETrue */,
                                                         const TDesC* aOldWVId /* = NULL */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL, Get \
                          contact-list(%S) from store" ), &aContactListId );

    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    TPEngContactListBaseSettings baseSettings;
    baseSettings.iContactListType = EPEngCachedContactList;
    baseSettings.iContactListNameAutoUpdate = ETrue;

    // try to create the contact list
    // we don't need to change any properties here, so ignore the returned value
    TRAPD( err, iPEngContactListStore->CreateContactListL( aContactListId,
                                                           baseSettings ) );

    // if it's already there we can just start using it
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    // get the list
    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    HBufC* oldNick = NULL;
    if ( aOldWVId /*&& ( ! aCanRemove )*/ )
        {
        // contact has been changed, so remove it first
        // as some servers will not accept same nicknames (yes nicknames) in the
        // list at the same time
        if ( CAUtils::NeutralCompare( aWVId, *aOldWVId ) != 0 )
            {

            TInt index( list.FindContact( *aOldWVId ) );

            if ( index != KErrNotFound )
                {
                MPEngContactItem& contactItem = list.ContactItem( index );
                oldNick = contactItem.NickName().AllocLC();

                list.RemoveContactL( *aOldWVId );

                CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL, \
                                      Remove old contact..." ) );
                // remove from IM storage too
                MCAStoredContacts* storage =
                    CCAStorageManagerFactory::ContactListInterfaceL();
                storage->RemoveContact( aContactListId, *aOldWVId );

                TRAPD( removeErr,
                       UpdateToNetworkL( EContactListUpdate, aContactListId ) );
                if ( removeErr == KErrCancel )
                    {
                    // In this case the network connection was closed and
                    // the request was cancelled
                    CHAT_DP(
                        D_CHAT_LIT( "CCAPEngListManager::AddServerContactL \
                                 leaving KErrCancel" ) );
                    User::Leave( err );
                    }
                }
            }
        }

    // add the new one
    TRAP( err, list.AddContactL( aWVId, aNickname ) );
    CAPresenceUtils::HandleListErrorsL( err );

    err = KErrNone;
    // Update changes to network
    CHAT_DP( D_CHAT_LIT(
                 "CCAPEngListManager::AddServerContactL, Add new contact..." ) );
    TRAP( err, UpdateToNetworkL( EContactListUpdate, aContactListId ) );

    if ( err == KErrCancel || ( err == Imps_ERROR_BASE - 601 ) )
        {
        // In this case the network connection was closed and the request
        // was cancelled
        CHAT_DP( D_CHAT_LIT(
                     "CCAPEngListManager::AddServerContactL leaving KErrCancel" ) );
        User::Leave( err );
        }

    MCAStoredContacts* storage =
        CCAStorageManagerFactory::ContactListInterfaceL();
    MCAStoredContact* contact = NULL;
    if ( err != KErrNone )
        {
        // failed
        CHAT_DP( D_CHAT_LIT( " **AddServerContactL failed with %d" ), err );

        // If partially successfull.
        // Take last detailed error as right error code.
        if ( err == Imps_ERROR_BASE - 201 )
            {
            const RPointerArray< MCAPresenceError >& errorArray =
                iErrors->DetailedErrors();
            if ( errorArray.Count() > 0 )
                {
                err = errorArray[ 0 ]->ErrorCode();
                }
            }

        TRAPD( listErr, list.RemoveContactL( aWVId ) );
        CAPresenceUtils::HandleListErrorsL( listErr );

        // remove from IM storage too
        storage->RemoveContact( aContactListId, aWVId );

        if ( aOldWVId && oldNick )
            {
            TRAPD( error, list.AddContactL( *aOldWVId, *oldNick ) );
            CAPresenceUtils::HandleListErrorsL( error );
            }

        // Update changes to network
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL \
            Trying to restore previous state" ) );
        TRAPD( restoreErr,
               UpdateToNetworkL( EContactListUpdate, aContactListId ) );
        if ( restoreErr == KErrCancel )
            {
            // In this case the network connection was closed and the
            // request was cancelled
            CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL \
                ***Could not restore old contact!" ) );
            User::Leave( restoreErr );
            }
        }
    else
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL success" ) );
        // successful editing (update storage)
        MCAContactList* contactList = NULL;
        contact = storage->FindContact( aContactListId, aWVId, contactList );
        if ( contact )
            {
            TInt index( list.FindContact( aWVId ) );
            if ( index != KErrNotFound )
                {
                // found it, update nick
                MPEngContactItem& contactItem = list.ContactItem( index );
                contact->SetNicknameL( contactItem.NickName() );
                // Nick name was updated, resort the contacts
                contactList->ResortContact( contact );
                contact->SignalChanges();
                }
            }
        }

    AddToNotifierL( aWVId );
    if ( oldNick )
        {
        CleanupStack::PopAndDestroy( oldNick );
        }

    CHAT_DP( D_CHAT_LIT( " **AddServerContactL leaving with %d" ), err );
    User::LeaveIfError( err );
    return contact;
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::AddServerContactL
// Adds several contacts to server side contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::AddServerContactL( const TDesC& aContactListId,
                                            const MDesCArray& aWVIds,
                                            const MDesCArray& aNicknames )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL (several), \
                          Get contact-list(%S) from store" ), &aContactListId );

    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AddServerContactL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    TPEngContactListBaseSettings baseSettings;
    baseSettings.iContactListType = EPEngCachedContactList;
    baseSettings.iContactListNameAutoUpdate = ETrue;

    // try to create the contact list
    // we don't need to change any properties here, so ignore the returned value
    TRAPD( err, iPEngContactListStore->CreateContactListL( aContactListId,
                                                           baseSettings ) );

    // if it's already there we can just start using it
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    TInt count( aWVIds.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TRAPD( err, list.AddContactL( aWVIds.MdcaPoint( i ),
                                      aNicknames.MdcaPoint( i ) ) );
        CAPresenceUtils::HandleListErrorsL( err );
        }

    iListObserver->SkipStorageRemoval();

    RestartNotifierL();

    err = KErrNone;
    // Update changes to network
    TRAP( err, UpdateToNetworkL( EContactListUpdate, aContactListId ) );
    if ( err != KErrNone )
        {
        MCAStoredContacts* storage =
            CCAStorageManagerFactory::ContactListInterfaceL();

        for ( TInt i( 0 ); i < count; ++i )
            {
            list.RemoveContactL( aWVIds.MdcaPoint( i ) );
            // remove from IM storage too
            storage->RemoveContact( aContactListId, aWVIds.MdcaPoint( i ) );
            }
        User::Leave( err );
        }
    }


// -----------------------------------------------------------------------------
// CCAPEngListManager::UpdateContactNicknameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::UpdateContactNicknameL(  const TDesC& aContactListId,
                                                  const TDesC& aWVId,
                                                  const TDesC& aNickname )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateContactNicknameL, Get \
                          contact-list(%S) from store" ), &aContactListId );

    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateContactNicknameL, \
            Not logged in --> KErrDisconnected" ) );
        User::Leave( KErrDisconnected );
        }

    TPEngContactListBaseSettings baseSettings;
    baseSettings.iContactListType = EPEngCachedContactList;
    baseSettings.iContactListNameAutoUpdate = ETrue;

    // try to create the contact list
    // we don't need to change any properties here, so ignore the returned value
    TRAPD( err, iPEngContactListStore->CreateContactListL(
               aContactListId, baseSettings ) );

    // if it's already there we can just start using it
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    // get the list
    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    TInt index = list.FindContact( aWVId );
    User::LeaveIfError( index );

    MPEngContactItem& item = list.ContactItem( index );
    item.UpdateNickNameL( aNickname );

    // Update changes to network
    UpdateToNetworkL( EContactListUpdate, aContactListId );

    index = list.FindContact( aWVId );
    User::LeaveIfError( index );
    MPEngContactItem& item2 = list.ContactItem( index );

    MCAStoredContacts* storage =
        CCAStorageManagerFactory::ContactListInterfaceL();

    // successful editing (update storage)
    MCAStoredContact* contact = storage->FindContact( aContactListId, aWVId );
    if ( contact )
        {
        contact->SetNicknameL( item2.NickName() );
        contact->SignalChanges();
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::UpdateServerContactsL
// Updates given server-side contact-list with given userids/nicknames
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::UpdateServerContactsL(	const TDesC& aContactListId,
                                                const MDesCArray& aWVIds,
                                                const MDesCArray& aNicknames )
    {
    CHAT_DP_TXT( "CCAPEngListManager::UpdateServerContactsL()" );

    if ( aWVIds.MdcaCount() != aNicknames.MdcaCount() )
        {
        User::Leave( ECAArraysNotSameSize );
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateServerContactsL, Get \
	                      contact-list(%S) from store" ), &aContactListId );

    MPEngContactList2& list =
        iPEngContactListStore->ContactListL( aContactListId );

    // Clean the local list
    CHAT_DP_TXT( "CCAPEngListManager::UpdateServerContactsL, \
	              remove all old IDs" );
    list.RemoveAllContactsL();

    // Add what was requested
    TInt err( KErrNone );
    TInt count( aWVIds.MdcaCount() );
    for ( TInt counter( 0 ); counter < count; ++counter )
        {
#ifdef _DEBUG
        TPtrC data1( aWVIds.MdcaPoint( counter ) );
        TPtrC data2( aNicknames.MdcaPoint( counter ) );
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateServerContactsL, \
                              adding %d: %S (%S)" ), counter, &data1, &data2 );
#endif
        TRAP( err, list.AddContactL( aWVIds.MdcaPoint( counter ),
                                     aNicknames.MdcaPoint( counter ) ) );
        CAPresenceUtils::HandleListErrorsL( err );
        }

    // restart notifier
    RestartNotifierL();

    // Update changes to network
    UpdateToNetworkL( EContactListUpdate, aContactListId );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::FetchServerContactListsL
// Fetches list of server-side contact-lists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TContactListsWrapper CCAPEngListManager::FetchServerContactListsL()
    {
    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );
    return contactLists;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::ReleaseContactListsL
// Releases local resources held by contact-list, except aExcludedContactList
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::ReleaseContactListsL(
    const TDesC& aExcludedContactList )
    {
    CHAT_DP_TXT( "CCAPEngListManager::ReleaseContactListsL()" );

    const MDesCArray& unknownLists =
        iPEngContactListStore->UnknownServerContactListsL();
    TInt count( unknownLists.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( unknownLists.MdcaPoint( i ) != aExcludedContactList )
            {
            iPEngContactListStore->ReleaseUnknownContactListFromStore(
                unknownLists.MdcaPoint( i ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::DeleteContactListL
// Not supported by 2.1
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::DeleteContactListL( const TDesC& aContactListID
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                                             , TBool aFailedList /* = EFalse */
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                                           )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL \
	    enter with id  %S" ), &aContactListID );

    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL, \
            Not logged in --> KErrDisconnected" ) );
        return KErrDisconnected;
        }

    TRAPD( err,
           // unsubscribe
           MPEngContactList2& list =
               iPEngContactListStore->ContactListL( aContactListID );
           SubscribeListL( list, aContactListID, EFalse, EFalse );
         );

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL \
        trapped block 1 done err = %d" ), err );

    TRAPD( err2,
           // delete
           iPEngContactListStore->ReleaseContactList( aContactListID );
           iPEngContactListStore->DeleteContactListL( aContactListID );
         );

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL \
        trapped block 2 done err2 = %d" ), err2 );

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    // When deleting failed list, ignore certain errors in unsubscribing if
    // actual deleting went ok
    if ( aFailedList )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL \
	                          aFailedList = ETrue" ) );
        if ( err2 == KErrNone && ( err == KErrNotFound || err == KErrNotReady ) )
            {
            err = err2;
            }
        }
    else
        {
        // Take care of both error codes
        err += err2;
        }
#else
    // Take care of both error codes
    err += err2;
#endif // IMPS_CONTACT_FETCH_BACKGROUND

    if ( err == KErrNone )
        {
        iOperationError = KErrNone;

        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL \
	                          synchronizing.." ) );
        iPEngContactListUpdater->PublishContactList( aContactListID, *this );
        iRequestQueue.WaitForResponseL( TCARequestQueue::EContactListUpdater );
        err = iOperationError;

        if ( err == KErrNone )
            {
            // update list notifier
            iListObserver->Remove( aContactListID );

            // and attribute notifier
            RemoveFromNotifierL( aContactListID );

            // deletion was successful, remove from storage
            MCAStoredContacts* storage =
                CCAStorageManagerFactory::ContactListInterfaceL();
            storage->RemoveContactList( aContactListID );
            }
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DeleteContactListL done with \
                          error code %d" ), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SetDisplayNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::SetDisplayNameL(
    const TDesC& aContactListID,
    const TDesC& aDisplayName )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::SetDisplayNameL setting \
                          properties.." ) );

    MPEngContactListProperties* prop = NULL;
    TRAPD( err, prop = &iPEngContactListStore->ContactListPropertiesL(
                           aContactListID ) );
    if ( err == KErrNone && prop )
        {
        prop->SetDisplayNameL( aDisplayName );

        // update to network
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::SetDisplayNameL \
                              synchronizing.." ) );
        iOperationError = KErrNone;
        iPEngContactListUpdater->PublishContactList( aContactListID, *this );
        iRequestQueue.WaitForResponseL( TCARequestQueue::EContactListUpdater );
        err = iOperationError;

        if ( err == KErrNone )
            {
            MCAStoredContacts* storage =
                CCAStorageManagerFactory::ContactListInterfaceL();
            MCAContactList* list = storage->FindContactList( aContactListID );
            if ( list )
                {
                list->SetDisplayNameL( aDisplayName );
                }
            }
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::SetDisplayNameL done with error \
                          code %d" ), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::CreateContactListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngListManager::CreateContactListL( const TDesC& aDisplayName )
    {

    if ( !IsLoggedIn() )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CreateContactListL, \
            Not logged in --> KErrDisconnected" ) );
        return KErrDisconnected;
        }

    TPEngContactListBaseSettings baseSettings;
    baseSettings.iContactListType = EPEngCachedContactList;
    baseSettings.iContactListNameAutoUpdate = ETrue;

    // generate id from displayname
    HBufC* id = CAUtils::GenerateIdLC( aDisplayName );
    TPtrC listId( id->Des() );

    MPEngContactListProperties* prop = NULL;
    TInt error = KErrAlreadyExists;
    TInt attempt = 1;

    // loop until we find unused id
    while ( error == KErrAlreadyExists && attempt < KMaxIdattempts )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CreateContactListL %S" ), &listId );

        // CleanupStack is not needed because ownership is not transferred
        TRAP( error, prop = &iPEngContactListStore->CreateContactListL( // CSI: 35 # See comment above
                                listId, baseSettings ) );

        if ( error == KErrAlreadyExists )
            {
            // we need to generate another id
            CleanupStack::PopAndDestroy( id );
            id = NULL;
            id = CAUtils::GenerateIdLC( aDisplayName, attempt );
            listId.Set( id->Des() );
            ++attempt;
            }
        }

    if ( error == KErrNone && prop )
        {
        // PEC might have changed the id.
        HBufC* realId = prop->Name().AllocL();
        CleanupStack::PushL( realId );
        listId.Set( realId->Des() );

        iOperationError = KErrNone;

        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CreateContactListL setting \
                              properties.." ) );
        prop->SetDisplayNameL( aDisplayName );

        // must exist on server
        prop->SetPropertyL( KPEngCntLstMustExistOnServer,
                            KPEngCntLstPropertyNativePermanent, ETrue );

        // update to network
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CreateContactListL \
                              synchronizing.." ) );
        iPEngContactListUpdater->SynchronizeContactList( listId, *this );
        iRequestQueue.WaitForResponseL( TCARequestQueue::EContactListUpdater );
        error = iOperationError;

        if ( error == KErrNone )
            {
            MCAStoredContacts* storage =
                CCAStorageManagerFactory::ContactListInterfaceL();
            MCAContactList* newList = storage->AddContactListIdL( listId, aDisplayName );

            if ( newList )
                {
                newList->SetSynchronised();
                storage->SendContactListUpdatedEvent( *newList );
                }

            // subscribed or not?
            TBool autoUpdate( iApplicationSettings->Value(
                                  MCASettings::EAutomaticPresenceUpdate ) );
            MPEngContactList2& list = iPEngContactListStore->ContactListL( listId );

            // update list notifier
            iListObserver->AddL( listId );

            // subscribe the list
            SubscribeListL( list, listId, autoUpdate, EFalse );

            // and attribute notifier
            AddToNotifierL( listId );
            }

        CleanupStack::PopAndDestroy( realId );
        }
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::CreateContactListL done with \
                          error code %d" ), error );
    CleanupStack::PopAndDestroy( id );
    return error;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::LastOperationResult
// Returns the error results of last operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const MCAPresenceErrors& CCAPEngListManager::LastOperationResult() const
    {
    return *iErrors;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::IdentificationL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CCAPEngListManager::IdentificationL( const TDesC& aUserId )
    {
    TContactListsWrapper contactListIds( iPEngContactListStore->AllContactListsL() );
    TInt count( contactListIds.MdcaCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        MPEngContactList2& list = iPEngContactListStore->ContactListL(
                                      contactListIds.MdcaPoint( a ) );
        TInt index = list.FindContact( aUserId );
        if ( index != KErrNotFound )
            {
            MPEngContactItem& contact = list.ContactItem( index );
            if ( contact.NickName().Length() != 0 )
                {
                return contact.NickName();
                }
            }
        }

    return CAUtils::DisplayId( aUserId );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::IsSyncDone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngListManager::IsSyncDone()
    {
    return ( iContactFetchStep == ESynchronizeDone );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleAttributeChangeL
// Handler for attribute change event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandlePresenceChangeL(
    CPEngPresenceNotifier2& aNotifier,
    CPEngTrackedPresenceIDs2& aChangedPresenceIDs )
    {
    CHAT_DP_TXT( "CCAPEngListManager::HandlePresenceChangeL" );
    iNotifierObserver.HandlePresenceChangeL( aNotifier, aChangedPresenceIDs );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleAttributeNotifyError
// Handler for attribute notifier errors
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandlePresenceError( TInt aError,
                                              CPEngPresenceNotifier2& aNotifier )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::HandlePresenceNotifyError: %d" ),
             aError );
    iNotifierObserver.HandlePresenceError( aError, aNotifier );

    if ( ( aError != KErrNoMemory ) && ( aError != KErrCancel )
         && ( aError != KErrServerTerminated ) )
        {
        TRAPD( err, RestartNotifierL() );
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::HandleAttributeNotifyError, \
                              RestartNotifierL returned %d" ), err );

        if ( err != KErrNone ) // Signal user about error
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SetAuthorizationToAllL
// Sets presence authorization mode to all
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SetAuthorizationToAllL()
    {
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToAllL" );
    TInt err( KErrNone );

    // Add CommCap/IM and OnlineStatus to default attribute list
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToAllL, Associate \
	              CommCap&OnlineStatus to default attribute list" );
    MPEngAttributeList2* list = AttributeListLC( iPEngAttributeListStore );
    TRAP( err, list->AddPresenceAttributeL( KUidPrAttrCommCap ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, list->AddPresenceAttributeL( KUidPrAttrOnlineStatus ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, list->AddPresenceAttributeL( KUidPrAttrUserAvailability ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, list->AddPresenceAttributeL( KUidPrAttrClientInfo ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, AddDynamicAttributesToListL( list ) );
    CAPresenceUtils::HandleListErrorsL( err );

    iPEngAttributeListStore->SetAsDefaultAttributeListL( *list );
    CleanupStack::PopAndDestroy( list );

    // remove possible authorizations from all contact lists
    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );

    // Deattach attribute-lists from all our contact-lists
    DetachAttributeListL( contactLists );

    // Update changes to network
    UpdateToNetworkL( EAttributeListUpdate );
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToAllL done" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AddDynamicAttributesToListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::AddDynamicAttributesToListL(
    MPEngAttributeList2* aList )
    {
    // skip KUidPrAttrCommCap, KUidPrAttrOnlineStatus, KUidPrAttrClientInfo
    // KUidPrAttrUserAvailability add all other attributes to the given list
    for ( TInt i = iAttributes.Count() - 1; i >= 0; i-- )
        {
        if (       ( iAttributes[ i ] != KUidPrAttrCommCap )
                   && ( iAttributes[ i ] != KUidPrAttrOnlineStatus )
                   && ( iAttributes[ i ] != KUidPrAttrUserAvailability )
                   && ( iAttributes[ i ] != KUidPrAttrClientInfo ) )
            {
            aList->AddPresenceAttributeL( iAttributes[ i ] );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SetAuthorizationToNoneL
// Sets presence authorization mode to none
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SetAuthorizationToNoneL()
    {
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToNoneL" );

    // Remove CommCap/IM from default attribute list
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToNoneL, remove CommCap \
	              from default attribute-list" );
    MPEngAttributeList2* list = AttributeListLC( iPEngAttributeListStore );
    list->RemoveAllAttributes();
    iPEngAttributeListStore->SetAsDefaultAttributeListL( *list );
    CleanupStack::PopAndDestroy( list );

    // remove possible authorizations from all contact lists
    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );

    // Deattach attribute-lists from all our contact-lists
    DetachAttributeListL( contactLists );

    // Update changes to network
    UpdateToNetworkL( EAttributeListUpdate );
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToNoneL done" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::SetAuthorizationToFriendsL
// Sets presence authorization mode to friends
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::SetAuthorizationToFriendsL()
    {
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToFriendsL" );

    // Remove CommCap/IM from default attribute list
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToFriendsL, \
	              Remove CommCap/IM from default attribute list" );
    MPEngAttributeList2* defaultAttrList =
        AttributeListLC( iPEngAttributeListStore );
    defaultAttrList->RemoveAllAttributes();
    iPEngAttributeListStore->SetAsDefaultAttributeListL( *defaultAttrList );
    CleanupStack::PopAndDestroy( defaultAttrList );

    // Create attribute-list with CommCap-attribute
    MPEngAttributeList2* attrList =
        iPEngAttributeListStore->CreateEmptyAttributeListL();
    CleanupClosePushL( *attrList );
    TRAPD( err, attrList->AddPresenceAttributeL( KUidPrAttrCommCap ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, attrList->AddPresenceAttributeL( KUidPrAttrOnlineStatus ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, attrList->AddPresenceAttributeL( KUidPrAttrUserAvailability ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, attrList->AddPresenceAttributeL( KUidPrAttrClientInfo ) );
    CAPresenceUtils::HandleListErrorsL( err );
    TRAP( err, AddDynamicAttributesToListL( attrList ) );
    CAPresenceUtils::HandleListErrorsL( err );

    // Fetch all contacts from all lists we have
    TContactListsWrapper contactLists( iPEngContactListStore->AllContactListsL() );

    // Attach created attribute-list to all contact-lists we have
    AttachAttributeListL( contactLists, *attrList );

    CleanupStack::PopAndDestroy( attrList ); // attrList

    // Update changes to network
    UpdateToNetworkL( EAttributeListUpdate );
    CHAT_DP_TXT( "CCAPEngListManager::SetAuthorizationToFriendsL done" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::RestartNotifierL
// Restarts the presence attribute notifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::RestartNotifierL(
    MDesCArray* aListIds /* = NULL */ )
    {
    CHAT_DP_TXT( "CCAPEngListManager::RestartNotifierL Enter" );
    if ( iApplicationSettings->Value( MCASettings::EAutomaticPresenceUpdate ) )
        {

        MDesCArray* listIds;
        TContactListsWrapper contactLists;
        if ( ! aListIds )
            {
            // use all lists
            contactLists = TContactListsWrapper( iPEngContactListStore->AllContactListsL() );
            listIds = &contactLists;
            }
        else
            {
            listIds = aListIds;
            }

        TMultiPecListWrapper allContacts;
        allContacts.InitializeLC( *listIds, *iPEngContactListStore );

        CHAT_DP_TXT( "CCAPEngListManager::RestartNotifierL Wrapper done" );

        if ( iPEngAttributeNotifier->IsActive() )
            {
            // run update, since it is already running
            CHAT_DP_TXT( "CCAPEngListManager::RestartNotifierL Updating Notifier" );
            User::LeaveIfError( iPEngAttributeNotifier->Update( allContacts ) );
            }
        else
            {
            CHAT_DP_TXT( "CCAPEngListManager::RestartNotifierL Starting notifier" );
            User::LeaveIfError( iPEngAttributeNotifier->Start( allContacts,
                                                               iAttributes.Array() ) );
            }

        CleanupStack::PopAndDestroy(); // allContacts
        }
    CHAT_DP_TXT( "CCAPEngListManager::RestartNotifierL Exit" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::RemoveFromNotifierL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::RemoveFromNotifierL( const TDesC& aId )
    {
    CHAT_DP_TXT( "CCAPEngListManager::RemoveFromNotifierL" );
    if ( iApplicationSettings->Value( MCASettings::EAutomaticPresenceUpdate )
         &&
         iPEngAttributeNotifier->IsActive()
       )
        {
        // remove id
        TInt err( iPEngAttributeNotifier->Remove( aId ) );
        err = err == KErrNotFound ? KErrNone : err;
        User::LeaveIfError( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AddToNotifierL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::AddToNotifierL( const TDesC& aId )
    {
    CHAT_DP_TXT( "CCAPEngListManager::AddToNotifierL" );
    if ( iApplicationSettings->Value( MCASettings::EAutomaticPresenceUpdate ) )
        {
        if ( iPEngAttributeNotifier->IsActive() )
            {
            TInt err( iPEngAttributeNotifier->Add( aId,
                                                   iAttributes.Array() ) );
            err = err == KErrAlreadyExists ? KErrNone : err;
            User::LeaveIfError( err );
            }
        else
            {
            User::LeaveIfError( iPEngAttributeNotifier->Start( aId,
                                                               iAttributes.Array() ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AttributeListLC
// Gets given attribute list. If listID is empty, then return default list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CCAPEngListManager::AttributeListLC(
    CPEngAttributeListStore2* aListStore )
    {
    CHAT_DP_TXT( "CCAPEngListManager::AttributeListLC" );

    MPEngAttributeList2* list = NULL;
    TInt err;
    TRAP( err, list = aListStore->GetDefaultAttributeListL(
                          EPEngNetworkAttributeLists ) );

    // If network list is not available, then create it.
    if ( !list )
        {
        list = aListStore->CreateEmptyAttributeListL();
        }

    CleanupClosePushL( *list );
    return list;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleAttributeListPublishError
// Handler for attribute-list publisher error event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandleAttributeListTransactionError( TInt aError,
                                                              CPEngAttributeListTransaction2& /*aTransaction*/,
                                                              TInt /*aTransactionOperation*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::HandleAttributeListPublisherError,\
                          error %d" ), aError );

    iRequestQueue.ResponseReceived( TCARequestQueue::EAttributeListUpdater );
    iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aError );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleAttributeListPublishCompletedL
// Handler for attribute-list publisher complete event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandleAttributeListTransactionCompleteL(
    MPEngTransactionStatus2& aStatus,
    CPEngAttributeListTransaction2& /*aTransaction*/,
    TInt /*aTransactionOperation*/ )
    {
    CHAT_DP_TXT( "CCAPEngListManager::HandleAttributeListPublisherCompletedL" );

    iRequestQueue.ResponseReceived( TCARequestQueue::EAttributeListUpdater );
    if ( aStatus.Status() )
        {
        iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aStatus.Status() );
        CAPresenceUtils::HandleTransactionStatusL( aStatus, *iErrors );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleContactListUpdateError
// Handler for contact-list publisher error event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandleContactListTransactionError(
    TInt aError,
    CPEngContactListTransaction2& /*aTransaction*/,
    TInt aTransactionOperation )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::HandleContactListTransactionError,\
                          error %d" ), aError );

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    if ( ( aTransactionOperation == EPEngTransOpCntListSyncList &&
           iContactFetchStep == ESubscribeList ) ||
         ( aTransactionOperation == EPEngTransOpCntListUpdateList &&
           iContactFetchStep == EFetchContacts ) ||
         ( aTransactionOperation == EPEngTransOpCntListBaseSync &&
           iContactFetchStep == EPublishAttributeLists ) ||
         ( aTransactionOperation == EPEngTransOpCntListUpdateList &&
           iContactFetchStep == EPublishListsDone ) )
        {
        StartFetchIdle();
        }
#endif //IMPS_CONTACT_FETCH_BACKGROUND

    iRequestQueue.ResponseReceived( TCARequestQueue::EContactListUpdater );
    iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aError );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::HandleContactListsUpdatedL
// Handler for contact-list publisher complete event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::HandleContactListTransactionCompleteL(
    MPEngTransactionStatus2& aStatus,
    CPEngContactListTransaction2& /*aTransaction*/,
    TInt aTransactionOperation )
    {
    CHAT_DP_TXT( "CCAPEngListManager::HandleContactListTransactionCompleteL" );

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    if ( ( aTransactionOperation == EPEngTransOpCntListSyncList &&
           iContactFetchStep == ESubscribeList ) ||
         ( aTransactionOperation == EPEngTransOpCntListUpdateList &&
           iContactFetchStep == EFetchContacts ) ||
         ( aTransactionOperation == EPEngTransOpCntListBaseSync &&
           iContactFetchStep == EPublishAttributeLists ) ||
         ( aTransactionOperation == EPEngTransOpCntListUpdateList &&
           iContactFetchStep == EPublishListsDone ) )
        {
        StartFetchIdle();
        }
#endif //IMPS_CONTACT_FETCH_BACKGROUND	

    iRequestQueue.ResponseReceived( TCARequestQueue::EContactListUpdater );
    if ( aStatus.Status() )
        {
        iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aStatus.Status() );
        CAPresenceUtils::HandleTransactionStatusL( aStatus, *iErrors );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::UpdateToNetworkL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::UpdateToNetworkL( TNetworkOperation aOperation,
                                           const TDesC& aListID, TBool aWait /*= ETrue*/ )
    {
    if ( !IsLoggedIn() )
        {
        CHAT_DP_TXT( "CCAPEngListManager::UpdateToNetworkL, system is not \
                      logged to service, operation request ignored!" );
        return;
        }

    iOperationError = KErrNone;

    switch ( aOperation )
        {
        case EContactListUpdate:
            {
            CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateToNetworkL, \
                              contact-list update (%S)" ), &aListID );

            if ( aListID == KNullDesC )
                {
                User::Leave( KErrArgument );
                }

            User::LeaveIfError( iPEngContactListUpdater->PublishContactList(
                                    aListID, *this ) );

            if ( aWait )
                {
                iRequestQueue.WaitForResponseL(
                    TCARequestQueue::EContactListUpdater );
                }

            CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateToNetworkL, \
                              contact-list update done (%d)" ),
                     iOperationError );
            break;
            }
        case EAttributeListUpdate:
            {
            if ( iAttribListSupported )
                {
                CHAT_DP_TXT( "CCAPEngListManager::UpdateToNetworkL, \
               attribute-list update" );
                User::LeaveIfError(
                    iPEngAttributeListPublisher->PublishAttributeLists( *this ) );

                if ( aWait )
                    {
                    iRequestQueue.WaitForResponseL(
                        TCARequestQueue::EAttributeListUpdater );
                    }

                CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::UpdateToNetworkL, \
                               attribute-list update done (%d)" ),
                         iOperationError );
                }
            else
                {
                CHAT_DP_TXT( "CCAPEngListManager::UpdateToNetworkL, \
            attribute lists - NOT SUPPORTED" );
                }
            break;
            }
        default:
            {
            CHAT_DP_TXT( "CCAPEngListManager::UpdateToNetworkL, unknown \
                      operation requested!" );
            User::Leave( KErrNotSupported );
            break;
            }
        }

    User::LeaveIfError( iOperationError );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AttachAttributeListL
// Cancels pending requests to network side.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::AttachAttributeListL( MDesCArray& aContactLists,
                                               MPEngAttributeList2& aAttributeList )
    {
    CHAT_DP_TXT( "CCAPEngListManager::AttachAttributeListL" );

    TInt count( aContactLists.MdcaCount() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC listId( aContactLists.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::AttachAttributeListL, \
                              attaching attribute-list to %S" ), &listId );

        // Attach created attribute-list to our given contact-list
        TRAPD( err, iPEngAttributeListStore->
               AttachAttributeListToContactListL( listId, aAttributeList ); );
        CAPresenceUtils::HandleListErrorsL( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::DetachAttributeListL
// Cancels pending requests to network side.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::DetachAttributeListL(  MDesCArray& aContactLists )
    {
    CHAT_DP_TXT( "CCAPEngListManager::DetachAttributeListL" );

    TInt count( aContactLists.MdcaCount() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC listId( aContactLists.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::DetachAttributeListL, \
                              detaching attribute-list from %S" ), &listId );

        TRAPD( err, iPEngAttributeListStore->
               DeleteAttributeListFromContactListL( listId ) );
        CAPresenceUtils::HandleListErrorsL( err );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::CancelPendingRequests
// Cancels pending requests to network side.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::CancelPendingRequests( )
    {
    CHAT_DP_FUNC_ENTER( "CancelPendingRequests" );

    if ( iPEngContactListUpdater )
        {
        iPEngContactListUpdater->CancelContactListPubSync();
        }
    if ( iPEngAttributeListPublisher && iAttribListSupported )
        {
        iPEngAttributeListPublisher->CancelPublishAttributeLists();
        }

    CHAT_DP_FUNC_DONE( "CancelPendingRequests" );
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::ReplaceAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::ReplaceAttributesL( RArray<TUint32>& aAttributes )
    {
    iAttributes = aAttributes;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::ServiceTreeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPEngWVCspServicesTree2 CCAPEngListManager::ServiceTreeL( CPEngNWSessionSlotID2& aSessionSlotID )
    {
    CPEngNWSessionSlot2* slot = CPEngNWSessionSlot2::NewLC( aSessionSlotID );
    TPEngWVCspServicesTree2 cspTree;
    TPckg<TPEngWVCspServicesTree2> cspTreePtr( cspTree );
    TInt err = slot->GetOption( KPEngWVCspServicesTree2,
                                KPEngNWSessionSlotTransportQuery,
                                cspTreePtr );

    CleanupStack::PopAndDestroy( slot );
    return cspTree;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::AttribListsSupported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngListManager::AttribListsSupportedL( CPEngNWSessionSlotID2& aSessionSlotID )
    {
    TPEngWVCspServicesTree2 cspTree = ServiceTreeL( aSessionSlotID );
    TBool attribListsSupported = cspTree.iPresenceFeat.FeatureSupported();
    attribListsSupported &= cspTree.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCALI );
    attribListsSupported &= cspTree.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDALI );
    attribListsSupported &= cspTree.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGALS );
    return attribListsSupported;
    }

// -----------------------------------------------------------------------------
// CCAPEngListManager::UpdatePresenceAuthorizeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::UpdatePresenceAuthorizeL()
    {
    if ( !iAttribListSupported )
        {
        // nothing to do if not supported
        return;
        }

    TInt authValue =
        iApplicationSettings->Value( MCASettings::EAuthorizeIMPresence );
    switch ( authValue )
        {
        case MCASettings::EAll:
            {
            CHAT_DP_TXT( "Own presence publish: to all" );
            SetAuthorizationToAllL();
            break;
            }
        case MCASettings::EFriends:
            {
            CHAT_DP_TXT( "Own presence publish: to friends" );
            SetAuthorizationToFriendsL();
            break;
            }
        case MCASettings::ENobody:
            {
            CHAT_DP_TXT( "Own presence publish: to nobody" );
            SetAuthorizationToNoneL();
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }
// -----------------------------------------------------------------------------
// CCAPEngListManager::AddObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCAPEngListManager::AddObserver( MCAListInitObserver* aObserver )
    {
    iObserver = aObserver;
    }
// -----------------------------------------------------------------------------
// CCAPEngListManager::RemoveObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngListManager::RemoveObserver()
    {
    iObserver = NULL;
    }
// -----------------------------------------------------------------------------
// CCAPEngListManager::IsListInitialization
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngListManager::IsListInitializationCompleted()
    {
    return iListInitInProcees;
    }

// End of file
