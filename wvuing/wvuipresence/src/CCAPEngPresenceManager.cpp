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
* Description:  Manager for presence operations (fetcher/notifier/publisher).
*
*/



// INCLUDE FILES
#include "CCAPEngPresenceManager.h"
#include "CAPresenceDefinitions.h"
#include "CAPresenceUtils.h"
#include "CCAPEngListManager.h"

// Contact handling
#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"

#include "impsbuilddefinitions.h"

#include "MCAContactList.h"

#include "MCAStoredContact.h"

#include "ChatDebugPrint.h"

#include "MCAPresenceObserver.h"
#include "MCAWatcherObserver.h"
#include "MCASettings.h"

#include "CCAPresenceErrors.h"
#include "CAPresenceConst.h"
#include "MCAPresence.h"
#include "MCAContactLists.h"
#include "TCAWrappers.h"

#include "MCAReactiveAuthObserver.h"
#include "SServerPrefers.h"
#include "TDecodeAttrParams.h"

#include "ImpsCSPAllErrors.h"

#include <WVUIPresenceVariationNG.rsg>

#include <PEngWVPresenceAttributes2.h>
#include <PEngWVPresenceErrors2.h>
#include <CPEngPresenceNotifier2.h>
#include <CPEngAttributeTransaction2.h>
#include <CPEngAttributeStore2.h>
#include <CPEngTrackedPresenceIDs2.h>
#include <CPEngTrackedPresenceID2.h>
#include <CIMPSSAPSettingsStore.h>
#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlotID2.h>

#include <MPEngPresenceAttrModel2.h>
#include <MPEngTransactionStatus2.h>

#include <CPEngReactAuthStore.h>
#include <CPEngReactAuthTransaction.h>
#include <CPEngReactAuthNotifier.h>
#include <CPEngReactAuthTransaction.h>
#include <MPEngReactAuthObserver.h>
#include <MPEngReactAuthTransactionObserver.h>
#include <MPEngAuthorizationRequest.h>
#include <MPEngAuthorizationRespond.h>
#include <MPEngAuthorizationStatus.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::CCAPEngPresenceManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAPEngPresenceManager::CCAPEngPresenceManager()
        : iAuthMode( KUndefined ),
        iNetworkState( KUndefined ),
        iPEngAPIInitialized( EFalse ),
        iObserverQueued( EFalse ),
        iCachedStatus( EFalse ),
        iAttributeProcessing( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::ConstructL( MCASettings* aApplicationSettings )
    {
    // Set settings API
    SetSettingsAPIL( aApplicationSettings );

    // Assign attributes that we are handling
    ResetAttributesL( EFalse );

    // Create detailed error container
    iErrors = CCAPresenceErrors::NewL();

    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAPEngPresenceManager* CCAPEngPresenceManager::NewL(
    MCASettings* aApplicationSettings )
    {
    CCAPEngPresenceManager* self = new( ELeave ) CCAPEngPresenceManager;

    CleanupStack::PushL( self );
    self->ConstructL( aApplicationSettings );
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAPEngPresenceManager::~CCAPEngPresenceManager()
    {
    delete iPEngAttributeTransaction;
    delete iListManager;
    delete iPEngAttributeStore;
    delete iErrors;
    if ( iOwnPresenceNotifier )
        {
        iOwnPresenceNotifier->RemoveObserver( *this );
        }
    delete iOwnPresenceNotifier;
    delete iCachedStatusText;

    iAttributes.Close();

    if ( iRANotifier )
        {
        iRANotifier->RemoveObserver( *this );
        iRANotifier->Stop();
        }

    delete iRAStore;
    delete iRANotifier;
    delete iRATransaction;

    delete iSessionSlotID;

    iOwnStates.ResetAndDestroy();
    iPresenceStates.ResetAndDestroy();
    iFetchObjects.ResetAndDestroy();

    TInt count( iAttrArrays.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        iAttrArrays[ a ].iArray.ResetAndDestroy();
        }
    iAttrArrays.Close();

    delete iIdle;
    }


// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AddWatcherL
// Sets watcher flag for given contact
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::AddWatcherL( const TDesC& aWVId )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::AddWatcherL" );
    CCAStorageManagerFactory::ContactListInterfaceL()->SetWatched( aWVId, ETrue );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::GetOnlineFriendsL
// Fetches list of online-friends
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::GetOnlineFriendsL( CDesCArray &aOnlineList,
                                                TBool aFetchFromNetwork )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::GetOnlineFriendsL()...starts" );

    CPtrCArray* friends = CAPresenceUtils::GenerateFriendsArrayLC(
                              CCAStorageManagerFactory::ContactListInterfaceL() );

    // Reset given array
    aOnlineList.Reset();

    // update our lists
    if ( aFetchFromNetwork )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::GetOnlineFriendsL starting \
                      network fetch" );
        FetchAttributesL( *friends, &aOnlineList, NULL, NULL, EFalse );
        }

    // Populate aOnlineList with online friends
    TInt friendCount( friends->Count() );
    MCAStoredContact* contact = NULL;

    for ( TInt i( 0 ); i < friendCount; ++i )
        {
        contact = CCAStorageManagerFactory::ContactListInterfaceL()->
                  FindAnyContact( ( *friends )[i] );
        if ( contact )
            {
            TStorageManagerGlobals::TPresenceStatus status =
                contact->OnlineStatus();
            if ( status == TStorageManagerGlobals::EOnline ||
                 status == TStorageManagerGlobals::EAway ||
                 status == TStorageManagerGlobals::EBusy )
                {
                aOnlineList.AppendL( contact->UserId() );
                }
            }
        }

    CleanupStack::PopAndDestroy( friends );

    CHAT_DP_TXT( "CCAPEngPresenceManager::GetOnlineFriendsL...over" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::GetOnlineUsersL
// Fetches list of online users from given user-list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::GetOnlineUsersL( const CDesCArray &aUserList,
                                              CDesCArray *aOnlineList,
                                              CDesCArray *aOffLineList,
                                              TBool aUpdateStorage /*= EFalse*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::GetOnlineUsersL, \
                          aUserList.Count() = %d, &aOnlineList = %d, \
                          &aOffLineList = %d" ),
             aUserList.Count(), aOnlineList, aOffLineList );

    FetchAttributesL( aUserList, aOnlineList, aOffLineList, NULL,
                      aUpdateStorage );

    CHAT_DP_TXT( "CCAPEngPresenceManager::GetOnlineUsersL done" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::PresenceObserver
// Returns presence-observer pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAPresenceObserver* CCAPEngPresenceManager::PresenceObserver() const
    {
    return iPresenceObserver;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::RefreshFriendsL
// Refreshes the presence status of friends according to refresh-flags
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::RefreshFriendsL()
    {
    // Create needed MDesCArray-based array for presence operations
    CPtrCArray* contactsArray = CAPresenceUtils::GenerateFriendsArrayLC(
                                    CCAStorageManagerFactory::ContactListInterfaceL()/*, EFalse*/ );
    FetchAttributesL( *contactsArray, NULL, NULL, NULL, ETrue );
    CleanupStack::PopAndDestroy( contactsArray );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SubscribeFriendsL
// Subscribes friends based on update-flag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SubscribeFriendsL( const TSubscribeMode aMode )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::SubscribeFriendsL, mode = %d" ),
             aMode );

    if ( !iPEngAPIInitialized )
        {
        return;
        }
    iListManager->SubscribeListsL( aMode == MCAPresence::ESubscribe );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ContactLists
// Returns contact-list-handling interface
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAContactLists* CCAPEngPresenceManager::ContactLists()
    {
    return iListManager;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::LastOperationResult
// Returns last operation's error results
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CCAPresenceErrors& CCAPEngPresenceManager::LastOperationResult() const
    {
    return *iErrors;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SettingsAPI
// Return settings-API
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCASettings* CCAPEngPresenceManager::SettingsAPI() const
    {
    return iApplicationSettings;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SetSettingsAPIL
// Sets settings API
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SetSettingsAPIL(
    MCASettings* aApplicationSettings )
    {
    // CCAPEngPresenceManager need settings API to function correctly
    if ( !iApplicationSettings && !aApplicationSettings )
        {
        User::Leave( ECANoSettingsAPI );
        }

    // If settings API handle provided, then update current handle
    if ( aApplicationSettings )
        {
        iApplicationSettings = aApplicationSettings;
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::RemoveWatcherL
// Removes watcher from given user
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::RemoveWatcherL( const TDesC &aWVId )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::RemoveWatcherL" );
    CCAStorageManagerFactory::ContactListInterfaceL()->
    SetWatched( aWVId, EFalse );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SetPresenceObserver
// Sets presence observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SetPresenceObserver(
    MCAPresenceObserver* aObserver )
    {
    iPresenceObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SetWatcherObserver
// Sets watcher observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SetWatcherObserver( MCAWatcherObserver* aObserver )
    {
    iWatcherObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleNetworkStateL
// To be called when network state changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleNetworkStateL( TNetworkStatus aState,
                                                  const SServerPrefers& aServerPreferences,
                                                  CPEngNWSessionSlotID2* aSessionSlotID )
    {
    CHAT_DP_FUNC_ENTER( "CCAPEngPresenceManager::HandleNetworkState" );

    // read own states from resource
    CAPresenceUtils::ReadStatesFromResourceL(
        RSC_CHAT_VARIATION_OWN_PRESENCE_STATES, iOwnStates );

    // read incoming presence states from resource
    CAPresenceUtils::ReadStatesFromResourceL(
        RSC_CHAT_VARIATION_PRESENCE_STATES, iPresenceStates );

    iServerPrefers = aServerPreferences;
    iAliasUsed = aServerPreferences.iAliasUsed;

    if ( aState != EUpdateBrand )
        {
        // notification of branding must not keep the state
        // changed, but should be returned to the existing state
        iNetworkState = aState;
        }

    if ( aState == ELoggedIn )
        {
        // initialize own presence observer, if one is queued
        InitializeOwnPresenceObserverL();
        // check if we have cached values
        if ( iCachedAppSettingsAuthValue )
            {
            SetPresenceAuthorizationL( iCachedAppSettingsAuthValue );
            iCachedAppSettingsAuthValue = 0;
            }
        if ( iCachedStatus )
            {
            ChangeStatusL( iCachedPresenceStatus, *iCachedStatusText );
            iCachedStatus = EFalse;
            delete iCachedStatusText;
            iCachedStatusText = NULL;
            }
        else if ( iCachedStatusText )
            {
            ChangeStatusMessageL( *iCachedStatusText );
            delete iCachedStatusText;
            iCachedStatusText = NULL;
            }
        }

    // update own state and notify observers
    NotifyOwnPresenceObserverL();

    if ( aState == EUpdateBrand )
        {
        iRAUsed = aServerPreferences.iReactiveAuthorization;
        iAliasUsed = aServerPreferences.iAliasUsed;
        return;
        }

    if ( iListManager )
        {
        iListManager->SetLoggedIn( iNetworkState == ELoggedIn, aSessionSlotID );
        }

    if ( !IsLoggedIn() )
        {
        // logging out
        CancelPendingRequests();

        // stop the RA notifier
        if ( iRANotifier )
            {
            iRANotifier->Stop();
            }
        }

    CHAT_DP_FUNC_DONE( "CCAPEngPresenceManager::HandleNetworkState" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::InitializePEngAPIL
// Handles changes in setting values
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::InitializePEngAPIL()
    {
    // we can straight delete the old one, since
    // if creating does not work, then we cannot do anything anyway
    delete iListManager;
    iListManager = NULL;
    // Create list manager
    iListManager = CCAPEngListManager::NewL(
                       *iApplicationSettings,
                       iAttributes,
                       *this,             // presence observer
                       iSessionSlotID,
                       *this );           // presence updater

    delete iPEngAttributeTransaction;
    iPEngAttributeTransaction = NULL;
    // Create publisher
    iPEngAttributeTransaction =
        CPEngAttributeTransaction2::NewL( *iSessionSlotID );

    delete iPEngAttributeStore;
    iPEngAttributeStore = NULL;
    // Create attribute store
    iPEngAttributeStore = CPEngAttributeStore2::NewL( *iSessionSlotID );

    // reactive authorization stuffs
    delete iRAStore;
    iRAStore = NULL;
    iRAStore = CPEngReactAuthStore::NewL( *iSessionSlotID );

    delete iRANotifier;
    iRANotifier = NULL;
    iRANotifier = CPEngReactAuthNotifier::NewL( *iSessionSlotID );
    iRANotifier->AddObserver( *this );

    delete iRATransaction;
    iRATransaction = NULL;
    iRATransaction = CPEngReactAuthTransaction::NewL( *iSessionSlotID );

    iPEngAPIInitialized = ETrue;

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    contacts->OwnStatus().SetUserIdL( iSessionSlotID->UserId() );
    contacts->OwnStatus().SetAliasL( KNullDesC );
    iListManager->SetLoggedIn( ETrue, iSessionSlotID );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleSettingsChangeL
// Handles changes in setting values
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    if ( ! IsLoggedIn() )
        {
        // we might not have a server, unless we're logged in, so we can't do
        // any presence stuff
        return;
        }

    // Presence update mode change
    if ( aChangedSettingEnum == MCASettings::EAutomaticPresenceUpdate )
        {
        if ( iApplicationSettings->Value(
                 MCASettings::EAutomaticPresenceUpdate ) )
            {
            SubscribeFriendsL( MCAPresence::ESubscribe );
            }
        else
            {
            SubscribeFriendsL( MCAPresence::EUnSubscribe );
            }
        }

    if ( aChangedSettingEnum == MCASettings::EOwnAlias )
        {
        // this branch will not get done unless alias usage has been enabled
        // through resource variation.
        UpdateAliasAttributeL();
        }

    // Presence authorization mode change
    if ( aChangedSettingEnum == MCASettings::EAuthorizeIMPresence )
        {
        SetPresenceAuthorizationL( iApplicationSettings->Value(
                                       MCASettings::EAuthorizeIMPresence ) );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleAttributeTransactionError
// Handler for fetcher errors
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleAttributeTransactionError(
    TInt aError,
    CPEngAttributeTransaction2& /*aTransaction*/,
    TInt aTransactionOperation )
    {
    if ( aTransactionOperation == EPEngTransOpOwnAttributePublish )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::\
                              HandlePresencePublishError, error %d" ), aError );
        iRequestQueue.ResponseReceived( TCARequestQueue::EPublisherWait );
        }
    else if ( ( aTransactionOperation == EPEngTransOpAttributeFetchToCache ) ||
              ( aTransactionOperation == EPEngTransOpAttributeFetchToObjects ) )
        {
        CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::HandleAttributeFetchError, \
                              error = %d" ), aError );
        iRequestQueue.ResponseReceived( TCARequestQueue::EFetcherWait );

        // delete previous results, so that we don't accidentally use them
        if ( aTransactionOperation == EPEngTransOpAttributeFetchToObjects )
            {
            iFetchObjects.ResetAndDestroy();
            }
        }

    iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aError );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleAttributeTransactionCompleteL
// Handler for fetcher complete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleAttributeTransactionCompleteL(
    MPEngTransactionStatus2& aStatus,
    CPEngAttributeTransaction2& aTransaction,
    TInt aTransactionOperation )
    {
    if ( aTransactionOperation == EPEngTransOpOwnAttributePublish )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::HandlePresencePublishCompletedL" );
        NotifyOwnPresenceObserverL();
        iRequestQueue.ResponseReceived( TCARequestQueue::EPublisherWait );
        }
    else if ( aTransactionOperation == EPEngTransOpAttributeFetchToCache )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::HandleAttributeFetchCompletedL" );
        iRequestQueue.ResponseReceived( TCARequestQueue::EFetcherWait );
        }
    else if ( aTransactionOperation == EPEngTransOpAttributeFetchToObjects )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::HandleAttributeFetchCompletedL" );
        iFetchObjects.ResetAndDestroy();
        aTransaction.GetFetchedAttributes( iFetchObjects );
        iRequestQueue.ResponseReceived( TCARequestQueue::EFetcherWait );
        }
    iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aStatus.Status() );
    CAPresenceUtils::HandleTransactionStatusL( aStatus, *iErrors );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandlePresenceChangeL
// Handler for attribute change event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandlePresenceChangeL(
    CPEngPresenceNotifier2& /*aNotifier*/,
    CPEngTrackedPresenceIDs2& aChangedPresenceIDs )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::HandlePresenceChangeL" );

    DecodeNotifierDataL( &aChangedPresenceIDs, NULL );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandlePresenceError
// Handler for attribute notifier errors
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandlePresenceError( TInt aError,
                                                  CPEngPresenceNotifier2& /* aNotifier */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::HandlePresenceNotifyError: %d" ),
             aError );

    iOperationError = CAPresenceUtils::MapErrorPECtoCSP( aError );
    }


// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::DecodeFetcherDataL
// Decodes fetcher data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::DecodeFetcherDataL(
    const MDesCArray& aUserList,
    CDesCArray *aStateOnline,
    CDesCArray *aStateOffline,
    CDesCArray *aStateUnknown,
    TBool aUpdateStorage )
    {
    MCAStoredContacts* contacts = aUpdateStorage ?
                                  CCAStorageManagerFactory::ContactListInterfaceL() : NULL;

    User::LeaveIfError( iAttrArrays.Append( TDecodeAttrParams() ) );
    // newly created array entry must be filled completely, so no leaving allowed!
    TInt pos( iAttrArrays.Count() - 1 );
    TDecodeAttrParams& decodeParams = iAttrArrays[ pos ];
    decodeParams.iPresenceStates = &iPresenceStates;
    decodeParams.iContactStorage = contacts;
    decodeParams.iStateOnline = aStateOnline;
    decodeParams.iStateOffline = aStateOffline;
    decodeParams.iStateUnknown = aStateUnknown;
    decodeParams.iEntryIndex = 0;
    decodeParams.iServerPrefers = iServerPrefers;
    decodeParams.iWait = TCARequestQueue::EDecodeAttrWait;
    decodeParams.iWatcherObserver = NULL;
    TRAPD( err, PopulateAttrModelsL( decodeParams.iArray, &aUserList ) );
    if ( err != KErrNone )
        {
        // array entry could not be filled properly => remove it
        decodeParams.iArray.ResetAndDestroy();
        iAttrArrays.Remove( pos );
        // now it's safe to leave
        User::Leave( err );
        }

    iOperationError = KErrNone;

    if ( !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack( DecodeAttrModels, this ) );
        }

    iRequestQueue.WaitForResponseL( TCARequestQueue::EDecodeAttrWait );

    User::LeaveIfError( iOperationError );
    }


TInt CCAPEngPresenceManager::DecodeAttrModels( TAny *aInstance )
    {
    return static_cast<CCAPEngPresenceManager*>( aInstance )->
           DoDecodeAttrModels();
    }

TInt CCAPEngPresenceManager::DoDecodeAttrModels()
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::DoDecodeAttrModels" );
    TBool retval( EFalse );

    if ( !iAttrArrays.Count() ) // No modelarray to decode.
        {
        return EFalse;
        }

    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::DoDecodeAttrModels, \
                          Count of attr packets %d" ), iAttrArrays.Count() );

    TDecodeAttrParams& decodeParams = iAttrArrays[ 0 ];

    TRAP( iOperationError, retval = CAPresenceUtils::DecodeAttrModelsL(
                                        decodeParams ) );

    if ( iOperationError != KErrNone || !retval )
        {
        // Ready.
        TCARequestQueue::TWaitCategory wait = decodeParams.iWait;
        decodeParams.iArray.ResetAndDestroy();
        iAttrArrays.Remove( 0 );
        if ( wait != TCARequestQueue::ENoWaitNeeded )
            {
            iRequestQueue.ResponseReceived( wait );
            }

        if ( iOperationError != KErrNone )
            {
            CActiveScheduler::Current()->Error( iOperationError );
            }
        }

    return TBool( iAttrArrays.Count() );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::DecodeNotifierDataL
// Decodes notifier data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::DecodeNotifierDataL(
    CPEngTrackedPresenceIDs2* aChangedPresenceIDs,
    const MDesCArray* aUserList )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::DecodeNotifierDataL - Enter" );

    if ( !aChangedPresenceIDs && aUserList )
        {
        return;
        }

    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    User::LeaveIfError( iAttrArrays.Append( TDecodeAttrParams() ) );
    // newly created array entry must be filled completely, so no leaving allowed!
    TInt pos( iAttrArrays.Count() - 1 );
    TDecodeAttrParams& decodeParams = iAttrArrays[ pos ];
    decodeParams.iPresenceStates = &iPresenceStates;
    decodeParams.iContactStorage = contacts;
    decodeParams.iStateOnline = NULL;
    decodeParams.iStateOffline = NULL;
    decodeParams.iStateUnknown = NULL;
    decodeParams.iEntryIndex = 0;
    decodeParams.iServerPrefers = iServerPrefers;
    decodeParams.iWait = TCARequestQueue::ENoWaitNeeded;
    decodeParams.iWatcherObserver = iWatcherObserver;
    TRAPD( err, PopulateAttrModelsL( decodeParams.iArray, aUserList, aChangedPresenceIDs ) );
    if ( err != KErrNone )
        {
        // array entry could not be filled properly => remove it
        decodeParams.iArray.ResetAndDestroy();
        iAttrArrays.Remove( pos );
        // now it's safe to leave
        User::Leave( err );
        }

    if ( iAttributeProcessing && !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack( DecodeAttrModels, this ) );
        }

    CHAT_DP_TXT( "CCAPEngPresenceManager::DecodeNotifierDataL - Done" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::IsLoggedIn
// Returns the network state of module
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngPresenceManager::IsLoggedIn() const
    {
    return iNetworkState == ELoggedIn ;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::PopulateAttrModelsL
// Populates attribute-model-array with given data. If aNotifierData is given,
// then uses notifier data, otherwise uses fetcher data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::PopulateAttrModelsL(
    RPointerArray<MPEngPresenceAttrModel2>& aArray,
    const MDesCArray* aUserList,
    CPEngTrackedPresenceIDs2* aChangedPresenceIDs /*=NULL*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::PopulateAttrModelsL, \
                          &aChangedPresenceIDs=%d" ), aChangedPresenceIDs );

    if ( !iPEngAPIInitialized )
        {
        CHAT_DP_TXT( "iPEngAPIInitialized not initialized" );
        return;
        }

    TLinearOrder<MPEngPresenceAttrModel2> rule(
        CAPresenceUtils::CompareAttrModelArray );

    if ( aChangedPresenceIDs ) // Notifier
        {
        // wrap changed ids and update models for those users
        TTrackedPresenceIds updatedIds;
        updatedIds.InitializeLC( *aChangedPresenceIDs );
        ExtractAttributeModelsForUsersL( updatedIds, aArray );
        CleanupStack::PopAndDestroy(); // updatedIds
        }
    else // Fetcher
        {
        // we are using fetcher which has fetched the attributes to cache.
        // so we must use the cache to decode the stuff
        ExtractAttributeModelsForUsersL( *aUserList, aArray );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ExtractAttributeModelL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::ExtractAttributeModelL( const TDesC& aId,
                                                     RPointerArray<MPEngPresenceAttrModel2>& aArray, TUint32 aAttribute )
    {
    MPEngPresenceAttrModel2* attributeModel = NULL;
    TInt error( KErrNone );
    if ( aId.Length() == 0 )
        {
        // no id
        error = iPEngAttributeStore->GetOwnAttribute( aAttribute,
                                                      attributeModel );
        }
    else
        {
        // have id
        error = iPEngAttributeStore->GetCachedAttribute( aId, aAttribute,
                                                         attributeModel );
        }

    if ( error == KErrNone )
        {
        CleanupClosePushL( *attributeModel );
        aArray.AppendL( attributeModel );   // ownership transfers
        CleanupStack::Pop(); // attributeModel
        }
    else if ( ( error != KErrNotSupported ) && ( error != KErrNotFound ) )
        {
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SetPresenceAuthorizationL
// Sets presence authorization mode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SetPresenceAuthorizationL(
    TInt aAppSettingsAuthValue )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::SetPresenceAuthorizationL, \
                         aAppSettingsAuthValue = %d" ), aAppSettingsAuthValue );

    if ( !iPEngAPIInitialized )
        {
        iCachedAppSettingsAuthValue = aAppSettingsAuthValue;
        return;
        }

    if ( iRAUsed )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::SetPresenceAuthorizationL - \
                      Reactive authorization in use, ignoring call" );
        return;
        }

    CHAT_DP_TXT( "CCAPEngPresenceManager::SetPresenceAuthorizationL - \
                  Reactive authorization not in use" );

    switch ( aAppSettingsAuthValue )
        {
        case MCASettings::EAll:
            {
            CHAT_DP_TXT( "Own presence publish: to all" );
            iListManager->SetAuthorizationToAllL();
            break;
            }
        case MCASettings::EFriends:
            {
            CHAT_DP_TXT( "Own presence publish: to friends" );
            iListManager->SetAuthorizationToFriendsL();
            break;
            }
        case MCASettings::ENobody:
            {
            CHAT_DP_TXT( "Own presence publish: to nobody" );
            iListManager->SetAuthorizationToNoneL();
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
// CCAPEngPresenceManager::FetchAttributesL
// Fetches attributes for given users
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::FetchAttributesL(  const MDesCArray& aUsers,
                                                CDesCArray *aStateOnline,
                                                CDesCArray *aStateOffline,
                                                CDesCArray *aStateUnknown,
                                                TBool aUpdateStorage  )
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::FetchAttributesL" );

    if ( IsLoggedIn() && aUsers.MdcaCount() > 0 )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::FetchAttributesL, \
                      fetching attributes" );
#ifdef _DEBUG
        TInt count( aUsers.MdcaCount() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            TPtrC data( aUsers.MdcaPoint( i ) );
            CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::FetchAttributesL, \
                                  user %d: %S" ), i, &data );
            }
#endif
        iOperationError = KErrNone;

        // Fetching of attributes may return error
        // Leave to avoid hanging in WaitResponseL
        User::LeaveIfError( iPEngAttributeTransaction->
                            FetchAttributesToCache( aUsers,
                                                    iAttributes.Array(),
                                                    *this ) );

        iRequestQueue.WaitForResponseL( TCARequestQueue::EFetcherWait );
        CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::FetchAttributesL, \
                              attribute fetch done (%d)" ), iOperationError );

        // Decode retrieved data if we still have memory left
        if ( iOperationError != KErrNoMemory && iOperationError != KErrDiskFull )
            {
            DecodeFetcherDataL( aUsers, aStateOnline, aStateOffline,
                                aStateUnknown, aUpdateStorage );
            }

        User::LeaveIfError( iOperationError );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::UpdateAliasAttributeL
// Update alias attribute to the network.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::UpdateAliasAttributeL()
    {
    CHAT_DP_TXT( "CCAPEngPresenceManager::UpdateAliasAttributeL" );

    if ( iNetworkState != ELoggedIn && iNetworkState != ELoggingOut )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::UpdateAliasAttributeL: No active \
                      network state -> ignore!" );
        return;
        }

    // check if alias is enabled at all
    if ( ! iAliasUsed )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::UpdateAliasAttributeL: Alias \
                      support disabled" );
        return;
        }

    HBufC* alias = iApplicationSettings->OwnAliasL();
    if ( !alias )
        {
        CHAT_DP_TXT( "CCAPEngPresenceManager::UpdateAliasAttributeL: Alias \
                      is not defined" );
        return;
        }

    CleanupStack::PushL( alias );
    MPEngPresenceAttrModel2* aliasAttr = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetAndLockOwnAttribute(
                            KUidPrAttrAlias, aliasAttr ) );
    CleanupClosePushL( *aliasAttr );
    aliasAttr->SetDataDesC16L( *alias, EPEngAlias );

    CHAT_DP_TXT( "CCAPEngPresenceManager::UpdateAliasAttributeL, publishing \
                  attribute" );
    iOperationError = KErrNone;
    TInt err = iPEngAttributeTransaction->PublishAndUnLockOwnAttribute(
                   aliasAttr, *this );
    if ( err )
        {
        CleanupStack::PopAndDestroy(); // aliasAttr
        }
    else
        {
        CleanupStack::Pop( ); // aliasAttr
        }
    CleanupStack::PopAndDestroy( alias );

    iRequestQueue.WaitForResponseL( TCARequestQueue::EPublisherWait );

    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::UpdateAliasAttributeL, \
                          attribute publishing done (%d)" ), iOperationError );

    if ( iOperationError == ECSPInvalidPresenceValue )
        {
        User::Leave( iOperationError );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SetReactiveAuthObserver
// Reactive authorization observer for internal events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SetReactiveAuthObserver(
    MCAReactiveAuthObserver* aRAObserver )
    {
    iRAObserver = aRAObserver;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::CancelPendingRequests
// Fetches attributes for given users
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::CancelPendingRequests()
    {
    CHAT_DP_FUNC_ENTER( "CancelPendingRequests" );

    if ( iPEngAttributeTransaction )
        {
        iPEngAttributeTransaction->CancelFetchAttributes();
        iPEngAttributeTransaction->CancelPublishOwnAttributes();
        }

    if ( iListManager )
        {
        iListManager->CancelPendingRequests();
        }

    CHAT_DP_FUNC_DONE( "CancelPendingRequests" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::SynchronizePresenceSettingsL
// Make sure server state is in sync with our presence settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SynchronizePresenceSettingsL(
    const SServerPrefers& /* aServerPreferences */ )
    {
    // authorization
    SetPresenceAuthorizationL( iApplicationSettings->Value(
                                   MCASettings::EAuthorizeIMPresence ) );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AliasL
// Fetch alias for given userid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAPEngPresenceManager::AliasL( const TDesC& aUserId )
    {
    if ( aUserId.Length() == 0 )
        {
        // can't find alias in this case...
        User::Leave( KErrNotFound );
        return NULL; // some compilers demand this
        }
    HBufC* aliasName = NULL;

    RArray<TUint32> attribute;
    CleanupClosePushL( attribute );
    attribute.AppendL( KUidPrAttrAlias );

    // get alias from network
    iOperationError = KErrNone;

    iPEngAttributeTransaction->FetchAttributesToObjects( aUserId,
                                                         attribute.Array(),
                                                         *this );
    iRequestQueue.WaitForResponseL( TCARequestQueue::EFetcherWait );

    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::AliasL, attribute fetch \
                          done (%d)" ), iOperationError );
    CleanupStack::PopAndDestroy(); // attribute.Close()

    if ( !iOperationError )
        {
        // Fetched attributes are in iFetchObjects
        const MPEngPresenceAttrModel2* aliasAttr =
            CAPresenceUtils::FindAttr( KUidPrAttrAlias, iFetchObjects );

        if ( aliasAttr )
            {
            // got an alias from the network
            TPtrC text = aliasAttr->DataDesC16( EPEngAlias );
            aliasName = text.AllocL();
            }
        else
            {
            // didn't get an alias, so use an empty alias
            aliasName = KNullDesC().AllocL();
            }
        }
    else
        {
        // got some problem
        User::LeaveIfError( iOperationError );
        return NULL; // some compilers demand this
        }

    // release fetched attributes
    iFetchObjects.ResetAndDestroy();

    // ownership transfers
    return aliasName;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AliasL
// Fetch alias for given userid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAPEngPresenceManager::AliasL( MCAStoredContact* aContact )
    {
    if ( !aContact )
        {
        return NULL;
        }

    HBufC* aliasName = AliasL( aContact->UserId() );
    CleanupStack::PushL( aliasName );

    if ( KErrNone != aContact->Alias().Compare( *aliasName ) )
        {
        CCAStorageManagerFactory::ContactListInterfaceL()->ResortContactInAll( aContact );
        aContact->SetAliasL( *aliasName );
        aContact->SignalChanges();
        }

    CleanupStack::Pop( aliasName );

    // ownership transfers
    return aliasName;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ChangeStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngPresenceManager::ChangeStatusL( TPresenceStatus aStatus,
                                            const TDesC& aStatusMessage )
    {
    CHAT_DP_FUNC_ENTER( "ChangeStatusL" );

    if ( !iPEngAPIInitialized )
        {
        // cache the status
        iCachedPresenceStatus = aStatus;
        HBufC* tempStatusText = aStatusMessage.AllocL();
        delete iCachedStatusText;
        iCachedStatusText = tempStatusText;
        iCachedStatus = ETrue;
        return KErrNone;
        }

    RPointerArray<MPEngPresenceAttrModel2> models;
    CleanupStack::PushL( TCleanupItem( CAPresenceUtils::DestroyCloseModelArray, &models ) );

    CCAState* state = CAPresenceUtils::FindStateL( aStatus, iOwnStates );

    if ( state )
        {
        TInt attrCount( state->iAttributes.Count() );
        for ( TInt i( 0 ); i < attrCount; ++i )
            {
            TCAAttribute& attr = state->iAttributes[i];
            MPEngPresenceAttrModel2* pengAttr = NULL;
            User::LeaveIfError( iPEngAttributeStore->GetAndLockOwnAttribute(
                                    attr.iAttribute, pengAttr ) );
            CleanupClosePushL( *pengAttr );

            if ( attr.iAttribute == KUidPrAttrStatusText )
                {
                // special case, set the status text
                pengAttr->SetDataDesC16L( aStatusMessage, EPEngStatusText );
                }
            else
                {
                // otherwise set the value from list
                pengAttr->SetDataIntL( attr.iData, attr.iField, attr.iGroup );
                }

            // set qualifier and append to list
            pengAttr->SetQualifier( attr.iQualifier );
            models.AppendL( pengAttr );
            CleanupStack::Pop(); // pengAttr
            }
        }

#ifdef _DEBUG
    else
        {
        CHAT_DP_TXT( "!!UI offered presence state that was not defined in resource!!" );
        }
#endif // _DEBUG        

    iOperationError = KErrNone;
    if ( models.Count() > 0 )
        {
        // we have some attributes, publish them
        CHAT_DP_TXT( "CCAPEngPresenceManager::ChangeStatusL, publishing attributes" );
        User::LeaveIfError( iPEngAttributeTransaction->PublishAndUnLockOwnAttributes(
                                models,
                                *this ) );
        CleanupStack::Pop(); // models

        iRequestQueue.WaitForResponseL( TCARequestQueue::EPublisherWait );
        CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::ChangeStatusL, attribute \
                              publishing done (%d)" ), iOperationError );
        }
    else
        {
        CleanupStack::PopAndDestroy(); // models
        }

    CHAT_DP_FUNC_DONE( "ChangeStatusL" );
    return iOperationError;
    }


// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ChangeStatusMessageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngPresenceManager::ChangeStatusMessageL( const TDesC& aStatusMessage )
    {
    CHAT_DP_FUNC_ENTER( "ChangeStatusMessageL" );

    if ( !iPEngAPIInitialized )
        {
        // cache the status text
        HBufC* tempText = aStatusMessage.AllocL();
        delete iCachedStatusText;
        iCachedStatusText = tempText;
        return KErrNone;
        }

    MPEngPresenceAttrModel2* statusMessage = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetAndLockOwnAttribute(
                            KUidPrAttrStatusText, statusMessage ) );
    CleanupStack::PushL( statusMessage );

    statusMessage->SetDataDesC16L( aStatusMessage, EPEngStatusText );
    statusMessage->SetQualifier( ETrue );
    CHAT_DP_TXT( "CCAPEngPresenceManager::ChangeStatusMessageL, publishing \
                  attributes" );
    iOperationError = KErrNone;
    iPEngAttributeTransaction->PublishAndUnLockOwnAttribute( statusMessage,
                                                             *this );
    CleanupStack::Pop( ); // statusMessage
    iRequestQueue.WaitForResponseL( TCARequestQueue::EPublisherWait );
    CHAT_DP( D_CHAT_LIT( "CCAPEngPresenceManager::ChangeStatusMessageL, \
                          attribute publishing done (%d)" ), iOperationError );

    CHAT_DP_FUNC_DONE( "ChangeStatusMessageL" );
    return iOperationError;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::StatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAPresence::TPresenceStatus CCAPEngPresenceManager::StatusL()
    {
    CHAT_DP_FUNC_ENTER( "StatusL" );
    MCAPresence::TPresenceStatus currentStatus( MCAPresence::EOffline );

    if ( !iPEngAPIInitialized )
        {
        // if the api is not initialized -> we are offline
        return currentStatus;
        }

    RPointerArray<MPEngPresenceAttrModel2> attributes;
    CAPresenceUtils::PushModelArrayL( attributes, ETrue );

    MPEngPresenceAttrModel2* userAvailability = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetOwnAttribute(
                            KUidPrAttrUserAvailability, userAvailability ) );
    CleanupClosePushL( *userAvailability );
    attributes.AppendL( userAvailability );
    CleanupStack::Pop(); // userAvailability

    MPEngPresenceAttrModel2* commCap = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetOwnAttribute(
                            KUidPrAttrCommCap, commCap ) );
    CleanupClosePushL( *commCap );
    attributes.AppendL( commCap );
    CleanupStack::Pop(); // commCap

    MPEngPresenceAttrModel2* onlineStatus = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetOwnAttribute(
                            KUidPrAttrOnlineStatus, onlineStatus ) );
    CleanupClosePushL( *onlineStatus );
    attributes.AppendL( onlineStatus );
    CleanupStack::Pop(); // onlineStatus

    MPEngPresenceAttrModel2* clientInfo = NULL;
    User::LeaveIfError( iPEngAttributeStore->GetOwnAttribute(
                            KUidPrAttrClientInfo, clientInfo ) );
    CleanupClosePushL( *clientInfo );
    attributes.AppendL( clientInfo );
    CleanupStack::Pop(); // clientInfo

    currentStatus =
        CAPresenceUtils::DecodeOnlineState(
            iPresenceStates, attributes );

    CleanupStack::PopAndDestroy(); // attributes

    CHAT_DP_FUNC_DONE( "StatusL" );
    return currentStatus;
    }


// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AddOwnPresenceStatusObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::AddOwnPresenceStatusObserverL(
    MCAPresenceObserver* aObserver )
    {
    iOwnStatusObserver = aObserver;
    if ( !iSessionSlotID )
        {
        // we are not yet online so we queue this observer
        iObserverQueued = ETrue;
        return;
        }
    InitializeOwnPresenceObserverL();
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::InitializeOwnPresenceObserverL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::InitializeOwnPresenceObserverL()
    {
    if ( !iObserverQueued )
        {
        // no queued observer, no need to do anything
        return;
        }
    // we don't have to use a temp variable since this
    // notifier has to be always created and if creating fails we cannot
    // use the old notifier

    if ( iOwnPresenceNotifier )
        {
        iOwnPresenceNotifier->RemoveObserver( *this );
        }

    delete iOwnPresenceNotifier;
    iOwnPresenceNotifier = NULL;

    iOwnPresenceNotifier = CPEngPresenceNotifier2::NewL( *iSessionSlotID );
    iOwnPresenceNotifier->AddObserver( *this );

    CArrayFixFlat<TUint32>* observedAttributes =
        new CArrayFixFlat<TUint32>( iAttributes.Count() ); // N observed attributes
    CleanupStack::PushL( observedAttributes );

    // just add all attributes to be observed
    for ( TInt i = iAttributes.Count() - 1; i >= 0; i-- )
        {
        observedAttributes->AppendL( iAttributes[ i ] );
        }
    iOwnPresenceNotifier->Start( KPEngUserOwnPresenceId(),
                                 observedAttributes->Array() );

    CleanupStack::PopAndDestroy( observedAttributes );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::NotifyOwnPresenceObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::NotifyOwnPresenceObserverL()
    {
    TPresenceStatus status = StatusL();
    TStorageManagerGlobals::TPresenceStatus storageStatus;
    switch ( status )
        {
        case MCAPresence::EOnline:
            {
            storageStatus = TStorageManagerGlobals::EOnline;
            break;
            }
        case MCAPresence::EAway:
            {
            storageStatus = TStorageManagerGlobals::EAway;
            break;
            }
        case MCAPresence::EBusy:
            {
            storageStatus = TStorageManagerGlobals::EBusy;
            break;
            }
        case MCAPresence::EOffline: // Flowthrough
        default:
            {
            // If we're online, offline means invisible
            storageStatus = IsLoggedIn() ?
                            TStorageManagerGlobals::EInvisible :
                            TStorageManagerGlobals::EOffline;
            break;
            }
        }

    if ( iPEngAttributeStore )
        {
        // Get own status message
        MPEngPresenceAttrModel2* statusMessage = NULL;

        TInt err = iPEngAttributeStore->GetOwnAttribute(
                       KUidPrAttrStatusText, statusMessage );
        CleanupClosePushL( *statusMessage );

        TPtrC16 statusText( KNullDesC );
        if ( err == KErrNone && statusMessage )
            {
            statusText.Set( statusMessage->DataDesC16( EPEngStatusText ) );
            }

        // Update own status
        MCAStoredContact& ownStatus =
            CCAStorageManagerFactory::ContactListInterfaceL()->OwnStatus();

        ownStatus.SetOnlineStatus( storageStatus );
        ownStatus.SetStatusTextL( statusText );
        CleanupStack::PopAndDestroy( statusMessage ); // now we can delete this

        ownStatus.SignalChanges();
        }

    if ( iOwnStatusObserver )
        {
        // notify observer with current status
        iOwnStatusObserver->HandleOwnStatusChange( status );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::NotifyOwnPresenceObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::InitializePresenceAPIL(
    CPEngNWSessionSlotID2& aSessionSlotID )
    {
    delete iSessionSlotID;
    iSessionSlotID = NULL;
    iSessionSlotID = aSessionSlotID.CloneL();

    InitializePEngAPIL();
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AddAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::AddAttributeL( TInt aAttribute )
    {
    // add attribute to the end of the attribute list
    iAttributes.Append( InternalToPECAttrL(
                            ( MCAPresence::TDynamicAttributes ) aAttribute ) );

    // restart the notifier, if any
    iListManager->ReplaceAttributesL( iAttributes );

    // next time something is done, the attributes should be updated
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::RemoveAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::RemoveAttributeL( TInt aAttribute )
    {
    // remove attribute from the attribute list
    TUint32 pecAttr =
        InternalToPECAttrL( ( MCAPresence::TDynamicAttributes ) aAttribute );
    for ( TInt i = iAttributes.Count() - 1; i >= 0; i-- )
        {
        if ( iAttributes[ i ] == pecAttr )
            {
            iAttributes.Remove( i );
            }
        }

    // restart the notifier, if any
    iAttributes.Compress();
    iListManager->ReplaceAttributesL( iAttributes );

    // next time something is done, the attributes should be updated
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ResetAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::ResetAttributesL(
    TBool aNotifyListManager /* = ETrue */ )
    {
    // always supported list of "base attributes",
    // dynamically branded attributes are not to be added here
    iAttributes.Reset();
    User::LeaveIfError( iAttributes.Append( KUidPrAttrOnlineStatus ) );
    User::LeaveIfError( iAttributes.Append( KUidPrAttrCommCap ) );
    User::LeaveIfError( iAttributes.Append( KUidPrAttrUserAvailability ) );
    User::LeaveIfError( iAttributes.Append( KUidPrAttrStatusText ) );
    User::LeaveIfError( iAttributes.Append( KUidPrAttrClientInfo ) );

    if ( aNotifyListManager )
        {
        iListManager->ReplaceAttributesL( iAttributes );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::InternalToPECAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CCAPEngPresenceManager::InternalToPECAttrL(
    TDynamicAttributes aAttribute )
    {
    switch ( aAttribute )
        {
        case EAliasAttr :
            {
            return KUidPrAttrAlias;
            }
        default :
            {
            User::Leave( KErrNotFound );
            break;
            }
        }
    // this is never reached
    return 0;
    }

// reactive authorization things
//
// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandlePendingAuthorizationReqL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandlePendingAuthorizationReqL(
    CPEngReactAuthNotifier& /* aNotifier */,
    TArray< MPEngAuthorizationRequest* >& aPendingAuthReqs )
    {
    CHAT_DP_FUNC_ENTER( "HandlePendingAuthorizationReqL" );

    if ( aPendingAuthReqs.Count() )
        {
        iRAObserver->HandleRAPendingReqL();
        }

    CHAT_DP_FUNC_DONE( "HandlePendingAuthorizationReqL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandlerAuthorizationStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandlerAuthorizationStatusL(
    CPEngReactAuthNotifier& /* aNotifier */,
    TArray< const MPEngAuthorizationStatus* > /*aAuthStatuses*/ )
    {
    CHAT_DP_FUNC_ENTER( "HandlerAuthorizationStatusL" );

    // will be done only once, since the observer will
    // ask for all of the requests once it's called.
    iRAObserver->HandleRAStatusL();

    CHAT_DP_FUNC_DONE( "HandlerAuthorizationStatusL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleRAError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleReactAuthError(
    TInt aError,
    CPEngReactAuthNotifier& /* aNotifier */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleReactAuthError" );

    iRAObserver->HandleRAError( aError );

    CHAT_DP_FUNC_DONE( "HandleReactAuthError" );
    }

// reactive authorization transactions

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleReactAuthTransactionCompleteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleReactAuthTransactionCompleteL(
    MPEngTransactionStatus2& /* aStatus */,
    CPEngReactAuthTransaction& /* aTransaction */,
    TInt /* aTransactionOperation */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleReactAuthTransactionCompleteL" );

    iRequestQueue.ResponseReceived( TCARequestQueue::EReactiveAuthWait );

    CHAT_DP_FUNC_DONE( "HandleReactAuthTransactionCompleteL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::HandleReactAuthTransactionError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::HandleReactAuthTransactionError(
    TInt /* aError */,
    CPEngReactAuthTransaction& /* aTransaction */,
    TInt /* aTransactionOperation */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleReactAuthTransactionError" );

    iRequestQueue.ResponseReceived( TCARequestQueue::EReactiveAuthWait );

    CHAT_DP_FUNC_DONE( "HandleReactAuthTransactionError" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::UpdatePresenceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::UpdatePresenceL( const TDesC& aUserId )
    {
    TOneDesWrapper wvId( aUserId );
    DecodeFetcherDataL( wvId, NULL, NULL, NULL, ETrue );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ExtractAttributeModelsForUserL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::ExtractAttributeModelsForUserL(
    const TDesC& aUserId,
    RPointerArray<MPEngPresenceAttrModel2>& aAttrModels )
    {
    // the base attribute set
    ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrOnlineStatus );

    ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrCommCap );

    ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrUserAvailability );

    ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrStatusText );

    ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrClientInfo );

    // add more dynamically branded attributes here
    if ( iAliasUsed )
        {
        ExtractAttributeModelL( aUserId, aAttrModels, KUidPrAttrAlias );
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::PresenceStates
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const RPointerArray<CCAState>& CCAPEngPresenceManager::PresenceStates()
    {
    return iPresenceStates;
    }

void CCAPEngPresenceManager::SetAttributeProcessing( TBool aEnabled )
    {
    iAttributeProcessing = aEnabled;
    if ( iAttributeProcessing )
        {
        if ( iAttrArrays.Count()  && !iIdle->IsActive() )
            {
            // we have something to process
            iIdle->Start( TCallBack( DecodeAttrModels, this ) );
            }
        }
    else
        {
        if ( iIdle->IsActive() )
            {
            // we had something ongoing, so pause it
            iIdle->Cancel();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::AllowReactiveAuthRequestL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::SendReactiveAuthResponseL( TInt aId,
                                                        TBool aResponse )
    {
    CHAT_DP_FUNC_ENTER( "SendReactiveAuthResponseL" );

    MPEngAuthorizationRequest& authReq =
        iRAStore->PendingAuthorizationReq( aId );

    // lock the response so nobody else will mess with it
    MPEngAuthorizationRespond* response = NULL;
    iRAStore->GetAndLockAuthorizationRespond( response );
    CleanupClosePushL( *response );

    if ( aResponse )
        {
        // accepted
        TArray<TUint32> attr = iAttributes.Array();
        TInt ignore;
        TRAP( ignore, authReq.SetAuthorizedAttributesL( attr ) );
        }
    else
        {
        // rejected
        TInt ignore;
        TRAP( ignore, authReq.DenyAuthorizationL() );
        }

    // now we can publish it. PEC engine knows which one we just changed
    if ( KErrNone !=
         iRATransaction->PublishReactAuthResponds( response, *this ) )
        {
        // got error, the lock stays with us
        CleanupStack::PopAndDestroy(); // response
        }
    else
        {
        iRequestQueue.WaitForResponseL( TCARequestQueue::EReactiveAuthWait );
        CleanupStack::Pop(); // response
        // all done, unlocking happens by the store as it took ownership
        }

    CHAT_DP_FUNC_DONE( "SendReactiveAuthResponseL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::GetNextPendingRARequestL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngPresenceManager::GetNextPendingRARequestL( TInt& aId,
                                                        TDes& aUserId,
                                                        TInt& aLastPos  )
    {
    CHAT_DP_FUNC_ENTER( "GetNextPendingRARequestL" );
    TInt count = iRAStore->PendingAuthorizationReqCount();
    for ( TInt i( 0 ) ; i < count ; ++i )
        {
        MPEngAuthorizationRequest& authReq =
            iRAStore->PendingAuthorizationReq( i );

        if ( MPEngAuthorizationRequest::EPEngAuthPending ==
             authReq.AuthorizationState() )
            {
            // found a pending one

            // caller is interested in these
            aId = i;
            aUserId.Copy( authReq.UserId() );
            aLastPos = i + 1;
            CHAT_DP_FUNC_DONE( "GetNextPendingRARequestL" );
            return ETrue;
            }
        }
    return EFalse;
    /*

        if ( ( count > 0 ) && ( aLastPos < ( count-1 ) ) )
            {
            // take the first pending one from the queue for processing,
            // find out which one it is.
            TBool getOut = EFalse;

            // for ( TInt i = 0; ! getOut && ( i < count ); i++ )
            // queue even after it's responded!
            for ( i = aLastPos; ! getOut && ( i < count ); i++ )
                {
                MPEngAuthorizationRequest& authReq =
                            iRAStore->PendingAuthorizationReq( i );

                if ( MPEngAuthorizationRequest::EPEngAuthPending ==
                                            authReq.AuthorizationState() )
                    {
                    // found a pending one

                    // caller is interested in these
                    aId = i;
                    aUserId.Copy( authReq.UserId() );
                    aLastPos = i;
                    CHAT_DP_FUNC_DONE("GetNextPendingRARequestL");
                    return ETrue;
                    }
                }
            aLastPos = i;
            retVal = EFalse;
            }
        else
            {
            // nothing in queue
            retVal = EFalse;
            }

        CHAT_DP_FUNC_DONE("GetNextPendingRARequestL");
        return retVal;
    */
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::GetNextRAStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAPEngPresenceManager::GetNextRAStatusL( TInt& aId, TBool& aStatus,
                                                TDes& aUserId, TInt& aLastPos )
    {
    CHAT_DP_FUNC_ENTER( "GetNextRAStatusL" );

    TBool retVal = EFalse;

    TInt count = iRAStore->RespondedAuthorizationCount();

    if ( ( count > 0 ) && ( aLastPos < ( count - 1 ) ) )
        {
        // take the next from the queue
        aLastPos++;

        const MPEngAuthorizationStatus& authStatus =
            iRAStore->RespondedAuthorization( aLastPos );

        // caller is interested in these
        aId = aLastPos;
        aStatus = authStatus.AuthorizationStatus() ==
                  MPEngAuthorizationStatus::EPEngAuthAccepted;
        aUserId.Copy( authStatus.UserId() );

        // stuff left in queue?
        retVal = aLastPos < ( iRAStore->RespondedAuthorizationCount() - 1 )
                 ? ETrue : EFalse;
        }
    else
        {
        // nothing in queue
        retVal = EFalse;
        }

    CHAT_DP_FUNC_DONE( "GetNextRAStatusL" );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::PendingRARequestCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngPresenceManager::PendingRARequestCount()
    {
    CHAT_DP_FUNC_ENTER( "PendingRARequestCount" );
    TInt count = iRAStore->PendingAuthorizationReqCount();
    CHAT_DP_FUNC_DONE( "PendingRARequestCount" );
    return count;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::RAStatusCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAPEngPresenceManager::RAStatusCount()
    {
    CHAT_DP_FUNC_ENTER( "RAStatusCount" );
    TInt count = iRAStore->RespondedAuthorizationCount();
    CHAT_DP_FUNC_DONE( "RAStatusCount" );
    return count;
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::PostLoginRAFlushL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::PostLoginRAFlushL()
    {
    CHAT_DP_FUNC_ENTER( "PostLoginRAFlushL" );

    // handle any reactive authorization events which came during login,
    // when IM Presence was not listening yet

    // start the RA notifier
    if ( !iRANotifier->IsActive() )
        {
        iRANotifier->Start();
        }

    if ( iRAStore->PendingAuthorizationReqCount() )
        {
        // pending authorization requests
        iRAObserver->HandleRAPendingReqL();
        }

    if ( iRAStore->RespondedAuthorizationCount() )
        {
        // incoming status stuff
        iRAObserver->HandleRAStatusL();
        }

    CHAT_DP_FUNC_DONE( "PostLoginRAFlushL" );
    }

// -----------------------------------------------------------------------------
// CCAPEngPresenceManager::ExtractAttributeModelsForUsersL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAPEngPresenceManager::ExtractAttributeModelsForUsersL(
    const MDesCArray& aUserList,
    RPointerArray<MPEngPresenceAttrModel2>& aAttrModels )
    {
    CHAT_DP_FUNC_ENTER( "ExtractAttributeModelsForUsersL" );

    TInt count = aUserList.MdcaCount();
    for ( TInt i = 0 ; i < count; ++i )
        {
        // the base attribute set
        ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                KUidPrAttrOnlineStatus );
        ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                KUidPrAttrCommCap );
        ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                KUidPrAttrUserAvailability );
        ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                KUidPrAttrStatusText );
        ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                KUidPrAttrClientInfo );

        // add more dynamically branded attributes here
        if ( iAliasUsed )
            {
            ExtractAttributeModelL( aUserList.MdcaPoint( i ), aAttrModels,
                                    KUidPrAttrAlias );
            }
        }

    CHAT_DP_FUNC_DONE( "ExtractAttributeModelsForUsersL" );
    }

//  End of File
