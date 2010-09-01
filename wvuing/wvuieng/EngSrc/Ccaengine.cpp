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
* Description:  This interface is used by UI to access network services i.e.
 *                sending messages and joining to groups e.g. This class is
 *                also used to get different interfaces to the services
 *                offered by the CCAEngine.dll.
 *
*/


// INCLUDE FILES
#include "ChatDebugPrint.h"

#include "CCAEngine.h"

#include "CCAChatContainer.h"
#include "CCAMessageHandler.h"
#include "CCAMessageCreator.h"

#include "CCAContactListModel.h"
#include "CCASearchManager.h"
#include "MCASearchObserverInterface.h"
#include "MCASearchData.h"
#include "CCAGroupManager.h"
#include "MCAGroupOperations.h"
#include "CCABlockingManager.h"
#include "CCAInviteManager.h"
#include "CCAPartialSuccessData.h"
#include "CCASettingsManager.h"
#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "MCAPresence.h"
#include "PublicEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "MCAStoredGroup.h"
#include "MCAStoredGroups.h"
#include "MCAExtendedStoredGroup.h"
#include "CCAStorageManagerFactory.h"
#include "CCAStorageObserver.h"

#include "CAPresenceManager.h"
#include "MCAPresence.h"

#include "CCAAdapterDll.h"
#include "MCAImpsFactory.h"
#include "MCAImpsGroupClient.h"
#include "MCAImpsImClient.h"
#include "MCAImpsFundClient.h"
#include "MCAError.h"
#include "MCAStoredContacts.h"

#include "CALoggerManagerFactory.h"
#include "TCALoggerMessageFactory.h"
#include "CCALoggerHeadersInterface.h"
#include "CCAAccessManager.h"
#include "CCAImageLoader.h"
#include "MCAContentProcessor.h"

#include "SServerPrefers.h"

#include "ImpsCSPAllErrors.h"
#include <e32base.h>

// granularity used for small arrays, which usually
// contains only one elements
const TInt KSearchPairsSmallGranularity = 1;

// maximum groups in search result. If search results
// exceeds this then "too many search results" is shown
const TInt KMaxGroupSearchLimit = 50;

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
EXPORT_C CCAEngine* CCAEngine::NewL( TUid aAppUid )
    {
    CCAEngine* self = new ( ELeave ) CCAEngine( aAppUid );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CCAEngine::~CCAEngine()
// Note the destruction order of objects.
// ---------------------------------------------------------
//
EXPORT_C CCAEngine::~CCAEngine()
    {
    CHAT_DP_FUNC_ENTER( "~CCAEngine" );

    if ( iImpsImServer )
        {
        iImpsImServer->Unregister();
        }

    if ( iImpsGroupServer )
        {
        iImpsGroupServer->Unregister();
        }

    if ( iImpsFundAPI )
        {
        iImpsFundAPI->Unregister();
        }

    // Must be deleted before settings manager
    delete iContactListModel;

    delete iAccessManager;
    delete iGroupManager;
    delete iSearchManager;
    delete iBlockingManager;
    delete iInviteManager;
    delete iSettingsManager;
    delete iRequestMapper;

    delete iMessageHandler;
    delete iMessageCreator;
    delete iChatContainer;

    delete iImageLoader;

    delete iPartialSuccessData;

    // synch stuff. should be already deallocated but let's have a
    // failsafe delete here
    delete iLoggedUserId;

    // not reset done, because contains iRequest and don't want delete twice
    delete iPairs;
    delete iRequest;
    delete iStorageObserver;

    // Release singleton instances because we are sure,
    // that we do not need them anymore.
    TRAPD( ignore, CALoggerManagerFactory::ReleaseInstanceL() );
    TRAP( ignore, CCAStorageManagerFactory::ReleaseL() );
    TRAP( ignore, ReleaseAdapterL() );
    CAPresenceManager::Release();

    delete iLoggerMessageFactory;
    delete iLoggerHeadersInterface;

    CHAT_DP_FUNC_DONE( "~CCAEngine" );
    }


// ---------------------------------------------------------
// CCAEngine::GetInternalSettingsInterface()
// ---------------------------------------------------------
//
EXPORT_C void CCAEngine::ResetLoggers()
    {
#ifdef RD_MULTIPLE_DRIVE
    // Release singleton instances because we are sure,
    // that we do not need them anymore.
    TRAPD( ignore, CALoggerManagerFactory::ReleaseInstanceL() ;

           CHAT_DP( D_CHAT_LIT( " ignore %d" ), ignore );
           //Set message factory to logger    // now create these interfaces again...for newly changed drive....

           CALoggerManagerFactory::ReadInstanceL( iLoggerMessageFactory , EFalse ); );
#endif
    }



// ---------------------------------------------------------
// CCAEngine::GetInternalSettingsInterface()
// ---------------------------------------------------------
//
EXPORT_C MCASettings* CCAEngine::GetInternalSettingsInterface() const
    {
    return iSettingsManager;
    }

// ---------------------------------------------------------
// CCAEngine::GetSearchInterface()
// ---------------------------------------------------------
//
EXPORT_C MCASearchInterface* CCAEngine::GetSearchInterface() const
    {
    return iSearchManager;
    }

// ---------------------------------------------------------
// CCAEngine::GetInviteInterface()
// ---------------------------------------------------------
//
EXPORT_C MCAInvite* CCAEngine::GetInvitationInterface() const
    {
    return iInviteManager;
    }

// ---------------------------------------------------------
// CCAEngine::GetBlockingInterface()
// ---------------------------------------------------------
//
EXPORT_C MCABlocking* CCAEngine::GetBlockingInterface() const
    {
    return iBlockingManager;
    }

// ---------------------------------------------------------
// CCAEngine::GetFundClientInterface()
// ---------------------------------------------------------
//
EXPORT_C MCAImpsFundClient* CCAEngine::GetFundClientInterface() const
    {
    return iImpsFundAPI;
    }

// ---------------------------------------------------------
// CCAEngine::GetAccessInterface()
// ---------------------------------------------------------
//
EXPORT_C MCAAccessInterface* CCAEngine::GetAccessInterface() const
    {
    return iAccessManager;
    }

// ---------------------------------------------------------
// CCAEngine::ContactListModel()
// ---------------------------------------------------------
//
EXPORT_C MCAContactListModel& CCAEngine::ContactListModel() const
    {
    return *iContactListModel;
    }

// ---------------------------------------------------------
// CCAEngine::ChatInterface()
// ---------------------------------------------------------
//
EXPORT_C MCAChatInterface& CCAEngine::ChatInterface() const
    {
    return *iChatContainer;
    }

// ---------------------------------------------------------
// CCAEngine::Messageutils()
// ---------------------------------------------------------
//
EXPORT_C const MCAMessageUtils& CCAEngine::MessageUtils() const
    {
    return *this;
    }

// ---------------------------------------------------------
// CCAEngine::GetGroupInterface()
// ---------------------------------------------------------
//
EXPORT_C MCAGroupManagerInterface* CCAEngine::GetGroupInterface() const
    {
    return iGroupManager;
    }

// ---------------------------------------------------------
// CCAEngine::BackgroundInterface()
// ---------------------------------------------------------
//
EXPORT_C MCABackgroundInterface* CCAEngine::BackgroundInterface() const
    {
    return iRequestMapper;
    }

// ---------------------------------------------------------
// CCAEngine::HandleNetworkStateChangeL
// ---------------------------------------------------------
//
EXPORT_C void CCAEngine::HandleNetworkStateChangeL(
    TNetworkState aState,
    TPEngWVCspServicesTree2* aServicesSupported,
    const SServerPrefers& aServerPrefers,
    CPEngNWSessionSlotID2* aSessionSlotID /* = NULL */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleNetworkStateChangeL" );
    CHAT_DP( D_CHAT_LIT( " * old state %d, new state %d" ), iState, aState );

    if ( ! iFullyConstructed )
        {
        // user cancelled login before it had even started,
        // engine is constructed half-way. so get out.
        CHAT_DP_TXT( "  Engine not fully constructed, get out." );
        return;
        }

    if ( iState == aState )
        {
        // If new state is the same as previous, then do nothing
        CHAT_DP_TXT( "CCAEngine::HandleNetworkStateChangeL with same state as \
                previously! -> ignoring..." );
        return;
        }

    if ( aState == ELoggedIn )
        {
        // logged in, reconstruct contact list model for new server (change sorting order)
        MCAContactListModel* model = iContactListModel;
        model->SetSort();
        }

    TNetworkState oldState = iState;

    if ( aState != EServerPrefers )
        {
        iState = aState;
        }

    if ( aServicesSupported && iSearchManager )
        {
        TBool searchSupported =
            aServicesSupported->iFundamentalFeat.FunctionSupported(
                KPEngFFSearchFunction );
        iSearchManager->SetSearchSupported( searchSupported );
        }

    iMessageHandler->SetLocalEchoInGroup( aServerPrefers.iLocalEchoInGroup );

    if ( aState != ESyncPresence && aState != EServerPrefers )
        {
        // ESyncPresence is a special case for presence manager only

        // notify RequestMapper
        if ( iRequestMapper )
            {
            iRequestMapper->HandleNetworkStateChange( aState );
            }

        // notify BlockingManager
        if ( iBlockingManager )
            {
            iBlockingManager->HandleNetworkStateChange(
                aState, aServerPrefers.iUseGrant );
            }

        // notify GroupManager
        if ( iGroupManager )
            {
            iGroupManager->HandleNetworkStateChangeL( aState );
            }

        if ( aState == ELoggedIn )
            {
            SynchronizeGroupsL( aServicesSupported );
            }
        }

    // presence manager
    if ( iPresenceManager )
        {
        switch ( aState )
            {
            case ELoggedIn:
                {
                iPresenceManager->HandleNetworkStateL( MCAPresence::ELoggedIn,
                                                       aServerPrefers,
                                                       aSessionSlotID );
                break;
                }

            case ELoggingOut:
                {
                iPresenceManager->HandleNetworkStateL( MCAPresence::ELoggingOut,
                                                       aServerPrefers,
                                                       aSessionSlotID );
                break;
                }

            case ELoggedOut:
                {
                iPresenceManager->HandleNetworkStateL( MCAPresence::ELoggedOut,
                                                       aServerPrefers,
                                                       aSessionSlotID );
                break;
                }
            case EServerPrefers:
                {
                iPresenceManager->HandleNetworkStateL( MCAPresence::EUpdateBrand,
                                                       aServerPrefers,
                                                       aSessionSlotID );
                return;
                }
            case ESyncPresence:
                {
                CHAT_DP_TXT( "**ESyncPresence event**" );
                if ( oldState != ELoggedIn )
                    {
                    CHAT_DP_TXT( "**Synchronizing presence settings**" );
                    // do not synchronize anything if we're logging out
                    iPresenceManager->SynchronizePresenceSettingsL(
                        aServerPrefers );
                    }
                }

            default:
                {
                // Presence module does not support -> ignore
                break;
                }
            }
        }

    CHAT_DP_FUNC_DONE( "HandleNetworkStateChangeL" );
    }

// ---------------------------------------------------------
// CCAEngine::SynchronizeGroupsL()
// ---------------------------------------------------------
//
void CCAEngine::SynchronizeGroupsL(
    TPEngWVCspServicesTree2* aServicesSupported )
    {
    CHAT_DP_FUNC_DP( "StartSyn", "Logging in" );

    TBool searchSupported = EFalse;

    // state changed to logged in
    if ( aServicesSupported )
        {
        searchSupported =
            aServicesSupported->iFundamentalFeat.FunctionSupported(
                KPEngFFSearchFunction );
        iSearchManager->SetSearchSupported( searchSupported );
        CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleNetworkStateChangeL \
                    - Search supported: %d" ), searchSupported );
        }
    else
        {
        CHAT_DP_FUNC_DP( "HandleNetworkStateChangeL",
                         "Services are not supported" );
        }

    // check if the groups are supported at all

    TBool groupsSupported = EFalse;

    if ( aServicesSupported )
        {
        // find out if the group stuff is supported
        groupsSupported = aServicesSupported->iGroupFeat.FeatureSupported();
        }

    if ( groupsSupported )
        {
        CHAT_DP_FUNC_DP( "SynchronizeGroupsL", "Groups are supported" );

        // groups are supported. search only if we can...
        if ( searchSupported )
            {
            // but make sure the search is not started yet
            if ( iRequestMapper->BackgroundTaskStatus(
                     MCABackgroundInterface::EGroupFetch ) &
                 ( MCABackgroundInterface::EUnknown |
                   MCABackgroundInterface::ECompleted |
                   MCABackgroundInterface::ECancelled |
                   MCABackgroundInterface::EFailed
                 ) )
                {
                CHAT_DP_FUNC_DP( "HandleNetworkStateChangeL",
                                 "Registering group list fetch" );
                iRequestMapper->RegisterBackgroundTask( this,
                                                        MCABackgroundInterface::EGroupFetch );
                }
            }
        else
            {
            CHAT_DP_FUNC_DP( "SynchronizeGroupsL", "Search is not supported" );
            // Search is not supported, so show all the groups the user has.
            // This is the only thing the IM Application can do about this
            // at the moment.
            iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
            iStoredGroups->ShowAllGroups();
            }
        }
    else
        {
        CHAT_DP_FUNC_DP( "SynchronizeGroupsL",
                         "Groups are not supported: hiding groups, \
                          not searching" );

        // groups are not supported
        // hide all our groups from storage
        iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
        iStoredGroups->HideAllGroups( ETrue );  // hides all groups
        }
    }

// ---------------------------------------------------------
// CCAEngine::PartialSuccessData()
// ---------------------------------------------------------
//
EXPORT_C const RPointerArray<MCAError>* CCAEngine::PartialSuccessData() const
    {
    return iPartialSuccessData->PartialSuccessData();
    }

// ---------------------------------------------------------
// CCAEngine::HandleErrorL()
// ---------------------------------------------------------
//
void CCAEngine::HandleErrorL(
    TInt           aStatus,
    TInt           aOpId,
    const TDesC*   aDescription,
    const CImpsDetailed* aDetailedRes,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleErrorL, opid %d, status %d" ),
             aOpId, aStatus );

    if ( aDescription )
        {
        TPtrC descr( *aDescription );
        CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleErrorL, description: %S" ),
                 &descr );
        }

    // In case of partial success store the data if it exists
    if ( ( aStatus == ECSPPartiallySuccessful ) && aDetailedRes )
        {
        CHAT_DP_TXT( "CCAEngine::HandleErrorL, \
                      partially successful situation.." );
        // must be trapped although this is an L-method because the request must
        // be handled in every case
        TRAPD( err,
               iPartialSuccessData->SetPartialSuccessDataL( *aDetailedRes ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }

    // in debug builds, print out the partial success data
#ifdef _DEBUG
    const RPointerArray<MCAError>* myerrors =
        iPartialSuccessData->PartialSuccessData();
    if ( myerrors )
        {
        TInt limit( ( *myerrors ).Count() );
        for ( TInt i( 0 ); i < limit; i++ )
            {
            CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleErrorL PSD code %d" ),
                     ( *myerrors )[i]->Code() );
            }
        }
#endif

    // redirect the error code to appropriate manager
    TPtrC recipientPtr( KNullDesC );

    TInt error( KErrNotFound );

    if ( aOpId != 0 )
        {
        // This is not a push message, so try to find recipient
        recipientPtr.Set( iChatContainer->MessageRecipient( aOpId, error ) );
        }

    if ( ( error == KErrNotFound ) || ( recipientPtr.Length() == 0 ) )
        {
        // In this case the recipient doesn't exist which means that
        // this is not a sent message error
        // => redirect to requestmapper

        CHAT_DP_TXT( "CCAEngine::HandleErrorL, redirecting to RequestMapper" );
        iRequestMapper->HandleRequest( aOpId, aStatus );
        if ( aOpId == 0 )
            {//server pushed event
            iGroupManager->LastImpsError( aStatus );
            }
        }
    else
        {
        // We found a recipient for the message so this is a sent message error
        // => redirect to chatdatamanager

        const RPointerArray<MCAError>* partialData =
            iPartialSuccessData->PartialSuccessData();

        // If partially success data has error code ECSPRecipientBlockedSender
        // we can show the message as it was sent succesfully.
        TBool blockedSender( EFalse );
        if ( partialData )
            {
            if ( partialData->Count() == 1 ) // only one error code
                {
                if ( ( *partialData )[0]->Code() == ECSPRecipientBlockedSender )
                    {
                    blockedSender = ETrue;
                    }
                }
            }

        if ( aStatus == ECSPPartiallySuccessful && blockedSender )
            {
            // In this case show that message sending was successful
            CHAT_DP_TXT( "CCAEngine::HandleErrorL, ECSPRecipientBlockedSender, \
                    redirecting to ChatDataManager" );
            iMessageHandler->HandleMessageSentL( KErrNone, aOpId, ETrue );
            }
        else
            {
            // Message sending was not successful,
            // "IM.Recipient.Blocked.Messages" functionality
            CHAT_DP_TXT( "CCAEngine::HandleErrorL, other error, redirecting to \
                    ChatDataManager" );
            CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleErrorL, indicating error, \
                        recipient: %S" ), &recipientPtr );
            iMessageHandler->HandleMessageSentL( aStatus, aOpId, EFalse );
            }
        }
    }

// ---------------------------------------------------------
// CCAEngine::HandleSettingsChangeL()
// ---------------------------------------------------------
//
void CCAEngine::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    CHAT_DP_TXT( "CCAEngine::HandleSettingsChange" );

    if ( iPresenceManager )
        {
        iPresenceManager->HandleSettingsChangeL( aChangedSettingEnum );
        }
    }

// Symbian OS default constructor can leave.
void CCAEngine::ConstructL()
    {
    // Construct request mapper
    iRequestMapper = CCARequestMapper::NewL();

    // Setting engine to listen errors in group fetching in background task
    iRequestMapper->RegisterBackgroundObserver(
        this,
        MCABackgroundInterface::EGroupFetch,
        MCABackgroundInterface::EFailed );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCARequestMapper" );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCASettingsManager" );

    // Create manager for internal settings handling
    iSettingsManager = CCASettingsManager::NewL( iAppUid );
    iSettingsManager->AddObserverL( this );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating iChatContainer" );

    iContactListModel = CCAContactListModel::NewL( *iSettingsManager );
    CHAT_DP( D_CHAT_LIT( "iContactListModel constructed..." ) );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCAPresenceManager" );
    // it's a singleton and always present
    iPresenceManager = CAPresenceManager::InstanceL( iSettingsManager );

    CHAT_DP( D_CHAT_LIT( "iPresenceManager constructed..." ) );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCAMessageCreator" );

    iMessageCreator = CCAMessageCreator::NewL();

    CHAT_DP( D_CHAT_LIT( "CCAMessageCreator constructed..." ) );

    // Construct the partial success data holder also
    iPartialSuccessData = CCAPartialSuccessData::NewL();

    //This T-class is heap-allocated only because we do not want to
    //include T-class header in header file of CCAEngine.
    iLoggerMessageFactory = new ( ELeave ) TCALoggerMessageFactory;

    //Set message factory to logger
    CALoggerManagerFactory::ReadInstanceL( iLoggerMessageFactory );

    CHAT_DP_TXT( "CCAEngine::ConstructL, done." );
    iGroupSynchronizing = EFalse;
    }

EXPORT_C TBool CCAEngine::ReadyForShutdown()
    {
    if ( iRequestMapper )
        {
        if ( iRequestMapper->WaitCount() )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

EXPORT_C void CCAEngine::CancelAllRequests()
    {
    if ( iRequestMapper )
        {
        iRequestMapper->CancelAllRequests();
        }

    if ( iPresenceManager )
        {
        iPresenceManager->CancelPendingRequests();
        }
    }

EXPORT_C MCALoggerHeadersInterface* CCAEngine::GetLoggerHeadersInterface()
    {
    return iLoggerHeadersInterface;
    }

// stop synchronizing the groups
void CCAEngine::StopSynchronizeGroupsL()
    {
    CHAT_DP_FUNC_ENTER( "StopSynchronizeGroupsL" );

    if ( ! iGroupSynchronizing )
        {
        // don't bother, group synchronizing already stopped
        // or not yet started
        return;
        }

    // Make sure that our state will be right one.
    // Sync should be over after this method.

    CHAT_DP_FUNC_DP( "StopSynchronizeGroupsL", "Ending search" );
    TInt status = KErrNone;
    TRAPD( err, status = iSearchManager->EndSearchL() );

    iGroupSynchronizing = EFalse;

    User::LeaveIfError( err );

    // Ignore the cancel error code. We don't want to show
    // error note when exiting the application while search was
    // active
    if ( status != EOperationCancelled )
        {
        User::LeaveIfError( status );
        }

    CHAT_DP_FUNC_DP( "StopSynchronizeGroupsL", "Deallocating stuff" );

    // make sure we're done
    if ( iLoggedUserId )
        {
        delete iLoggedUserId;
        iLoggedUserId = NULL;
        }

    if ( iPairs )
        {
        // not reset done, because contains iRequest
        // and don't want delete twice
        delete iPairs;
        iPairs = NULL;
        }

    if ( iRequest )
        {
        delete iRequest;
        iRequest = NULL;
        }

    iSearchResults = NULL;

    CHAT_DP_FUNC_DONE( "StopSynchronizeGroupsL" );
    }

// synchronize the groups in storage with those in the server
void CCAEngine::StartSynchronizeGroupsL()
    {
    CHAT_DP_FUNC_ENTER( "StartSynchronizeGroupsL" );

    // make sure we're stopped
    StopSynchronizeGroupsL();

    if ( ! iStoredGroups )
        {
        // lazy initialization
        iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
        }

    // make all groups invisible
    iStoredGroups->HideAllGroups();

    delete iLoggedUserId;
    iLoggedUserId = NULL;
    iLoggedUserId = iSettingsManager->ValueL( MCASettings::EOwnWVUserID );

    CSearchPairs* tempPairs =
        new ( ELeave ) CSearchPairs( KSearchPairsSmallGranularity );
    CleanupStack::PushL( tempPairs );
    CImpsSearchRequest* tempRequest = CImpsSearchRequest::NewL();
    CleanupStack::PushL( tempRequest );
    tempRequest->SetRequestL( EImpsGroupUserIDOwner, *iLoggedUserId );
    tempPairs->AppendL( tempRequest );
    CleanupStack::Pop( tempRequest );
    CleanupStack::Pop( tempPairs );
    delete iPairs;
    delete iRequest;
    iPairs = tempPairs;
    iRequest = tempRequest;

    // If there is error in search, return so we don't send stop error request
    TInt searchError( KErrNone );
    TInt err( KErrNone );
    TRAP( err, searchError =
              iSearchManager->StartSearchL( *iPairs, KMaxGroupSearchLimit, this ) );

    iGroupSynchronizing = ETrue;

    // it's synchronous
    if ( ( err != KErrNone ) || ( searchError != KErrNone ) )
        {
        // we must cleanup so we can't just leave
        CHAT_DP( D_CHAT_LIT( "CCAEngine::StartSynchronizeGroupsL - error from \
                              StartSearchL %d, err %d" ), searchError, err );
        StopSynchronizeGroupsL();

        // leave now, when it's all clean and tidy
        // Leave also with cancel --> but leave quietly
        if ( KErrNone == searchError || EOperationCancelled == searchError )
            {
            User::Leave( err );
            }
        else
            {
            User::Leave( searchError );
            }
        }

    DoHandleSearchFinishedL();
    StopSynchronizeGroupsL();

    CHAT_DP_FUNC_DONE( "StartSynchronizeGroupsL" );
    }

// from MCASearchObserverInterface
void CCAEngine::HandleSearchError( TInt /* aErrorCode */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleSearchError" );

    // something bad happened
    TRAPD( err, StopSynchronizeGroupsL() );
    if ( err != KErrNone )
        {
        CHAT_DP( D_CHAT_LIT( "CCAEngine::HandleSearchError - Unexpected, \
                              left with %d" ), err );
        CActiveScheduler::Current()->Error( err );
        }

    CHAT_DP_FUNC_DONE( "HandleSearchError" );
    }

// the actual leaving handler for finished searches
void CCAEngine::DoHandleSearchFinishedL()
    {
    CHAT_DP_FUNC_ENTER( "DoHandleSearchFinishedL" );

    TInt resultCount(
        iSearchManager->SearchDataInterface()->SearchDataCount() );

    CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - found %d \
                          groups from joined user" ), resultCount );
    CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - From index \
                          %d to %d" ), iSearchResults, resultCount - 1 );
    TInt status = KErrNone;
    for ( TInt counter( iSearchResults ); counter < resultCount; counter++ )
        {
        // handle requests, but don't handle the groups which were already there
        if ( status == EOperationCancelled )
            {
            CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - \
                                  Operation Cancelled" ) );
            break;
            }

        // see if this group is already in storage
        TPtrC groupId( iSearchManager->SearchDataInterface()->SearchData(
                           counter ) );
        MCAStoredGroup* group = iStoredGroups->FindGroup( groupId );

        if ( !group )
            {
            CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - Group\
                                  %S is not in storage, adding" ), &groupId );

            // not found
            // so create group to storage with valid parameters:
            // must have groupid (before group operations), groupname, owngroup

            MCAExtendedStoredGroup* exGrp = iStoredGroups->CreateGroupL();  // CSI: 35 # Ownership is not transferred to caller.
            exGrp->SetGroupIdL( groupId );
            // make it invisible
            exGrp->SetVisible( EFalse );
            MCAGroupOperations* grOp =
                iGroupManager->GroupOperationsL( groupId );

            CImpsCommonGroupProps* commonProps = NULL;
            CImpsPrivateGroupProps* privProps = NULL;

            // fetch properties from network
            CHAT_DP_FUNC_DP( "DoHandleSearchFinishedL", "Getting properties" );
            status = grOp->GetPropertiesL( commonProps, privProps ) ;
            if ( status == EOperationCancelled )
                {
                CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - \
	                                  Operation Cancelled" ) );
                break;
                }
            if ( commonProps )
                {
                // group name has to be known
                TPtrC name( commonProps->GroupName() );
                CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - \
                                      Group has name '%S'" ), &name );
                if ( name.Length() > 0 )
                    {
                    exGrp->SetGroupNameL( name );
                    }
                else
                    {
                    exGrp->SetGroupNameL( KNullDesC );
                    }

                // our very own group
                exGrp->SetOwnGroup( ETrue );

                // make it visible
                exGrp->SetVisible( ETrue );

                // save the group and signal the UI the group changed
                iStoredGroups->SaveGroupL( groupId );
                }
            else
                {
                CHAT_DP_FUNC_DP( "DoHandleSearchFinishedL",
                                 "No common properties" );
                // if the name can't be fetched,
                // name the group with group's wvid
                exGrp->SetGroupNameL( groupId );
                }

            }
        else
            {
            CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - \
                Group %S is already in storage, making it visible" ), &groupId );
            ( static_cast<MCAExtendedStoredGroup *>( group ) )->SetVisible( ETrue );
            iStoredGroups->SignalGroupAddedL( groupId );
            }
        }

    // add only number of new ones
    iSearchResults += ( resultCount - iSearchResults );
    CHAT_DP( D_CHAT_LIT( "CCAEngine::DoHandleSearchFinishedL - Searching, index \
                          is now %d" ), iSearchResults );

    CHAT_DP_FUNC_DONE( "DoHandleSearchFinishedL" );
    }

// from MCASearchObserverInterface
void CCAEngine::HandleSearchFinished( )
    {
    CHAT_DP_FUNC_ENTER( "HandleSearchFinished" );

    CHAT_DP_FUNC_DONE( "HandleSearchFinished" );
    }

// from MCABackgroundTask
void CCAEngine::HandleBackgroundTaskL( TInt aSubTask )
    {
    CHAT_DP_FUNC_ENTER( "HandleBackgroundTaskL" );
    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::HandleBackgroundTaskL Subtask \
	                      is: %d" ), aSubTask );

    StartSynchronizeGroupsL();

    // imps errors are ignored at the moment as we don't have any
    // notes specified for them

    CHAT_DP_FUNC_DONE( "HandleBackgroundTaskL" );
    }

// from MCABackgroundObserver
void CCAEngine::HandleBackgroundEvent(
    MCABackgroundInterface::TCABackgroundTasks aEventSource,
    MCABackgroundInterface::TCABackgroundStatus aStatus,
    TInt /*aSubTaskNumber*/,
    TInt aLeaveCode )
    {
    // Currently we are interested only group fetching problems
    if ( ( aEventSource == MCABackgroundInterface::EGroupFetch ) &&
         ( aStatus == MCABackgroundInterface::EFailed ) )
        {
        if ( ( aLeaveCode > Imps_ERROR_BASE ) && ( aLeaveCode < KErrNone ) )
            {
            // propagate system errors to current active scheduler,
            // it should show a note
            CActiveScheduler::Current()->Error( aLeaveCode );
            }
        // imps errors are ignored at the moment as we don't have any
        // notes specified for them
        }
    }


// ---------------------------------------------------------
// CCAEngine::MessageCreator()
// ---------------------------------------------------------
//
MCAMessageCreator& CCAEngine::MessageCreator() const
    {
    return *iMessageCreator;
    }

// ---------------------------------------------------------
// CCAEngine::MessageErrorInformer()
// ---------------------------------------------------------
//
MCAMessageErrorInformer& CCAEngine::MessageErrorInformer() const
    {
    return *iMessageHandler;
    }

// ---------------------------------------------------------
// CCAEngine::ImageScaler()
// ---------------------------------------------------------
//
MCAContentProcessor& CCAEngine::ImageScaler() const
    {
    return *iImageLoader;
    }

// ---------------------------------------------------------
// CCAEngine::MemoryHandler
// ---------------------------------------------------------
//
MCABufferMemoryHandler& CCAEngine::MemoryHandler() const
    {
    return *iChatContainer;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAEngine::CCAEngine( TUid aAppUid )
        :   iAppUid( aAppUid ),
        iState( ELoggedOut )
    {
    }

// ---------------------------------------------------------
// CCAEngine::FinalizeEngineConstructionL()
// ---------------------------------------------------------
//
EXPORT_C void CCAEngine::FinalizeEngineConstructionL()
    {
    CHAT_DP_FUNC_ENTER( "FinalizeEngineConstructionL" );

    if ( iFullyConstructed )
        {
        // nothing to do
        CHAT_DP_TXT( "  Already fully constructed, all done" );
        CHAT_DP_FUNC_DONE( "FinalizeEngineConstructionL" );
        return;
        }

    MCAImpsFactory* impsFactory = CreateImpsFactoryL();

    CHAT_DP_TXT( "CCAEngine::ConstructL, CCAAccessManager::NewL" );
    iAccessManager = CCAAccessManager::NewL( *iRequestMapper );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCASearchManager for searching \
            and synchronizing groups" );
    iSearchManager = CCASearchManager::NewL( impsFactory, iRequestMapper );

    iChatContainer = CCAChatContainer::NewL( *iSettingsManager );
    CHAT_DP( D_CHAT_LIT( "iChatContainer constructed..." ) );

    iImageLoader = CCAImageLoader::NewL( *iChatContainer, *impsFactory );
    CHAT_DP( D_CHAT_LIT( "iImageLoader constructed..." ) );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCAGroupManager" );
    iGroupManager = CCAGroupManager::NewL( *iSearchManager,
                                           *iSettingsManager, *iRequestMapper, impsFactory,
                                           *iChatContainer, *this );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCABlockingManager" );

    iBlockingManager = CCABlockingManager::NewL(
                           impsFactory,
                           iRequestMapper,
                           iSettingsManager );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCAMessageHandler" );

    iMessageHandler = CCAMessageHandler::NewL( *iChatContainer, *this,
                                               impsFactory );

    CHAT_DP( D_CHAT_LIT( "CCAMessageHandler constructed..." ) );

    iSettingsManager->AddObserverL( iBlockingManager );

    CHAT_DP_TXT( "CCAEngine::ConstructL, creating CCAInviteManager" );

    iInviteManager = CCAInviteManager::NewL( impsFactory,
                                             *iSettingsManager,
                                             *iRequestMapper,
                                             *iGroupManager );

    CHAT_DP_TXT( "CCAEngine::ConstructL, registering APIs" );

    // Register observers and error handlers
    CHAT_DP_TXT( "CCAEngine::ConstructL, CreateFundClientL" );
    iImpsFundAPI = impsFactory->CreateFundClientL();
    iImpsFundAPI->RegisterL( iSearchManager, iInviteManager );
    iImpsFundAPI->RegisterErrorObserverL( *this );

    CHAT_DP_TXT( "CCAEngine::ConstructL, CreateImClientL" );
    iImpsImServer = impsFactory->CreateImClientL();
    iImpsImServer->RegisterL( iMessageHandler, iBlockingManager, iAppUid,
                              CActive::EPriorityIdle );
    iImpsImServer->RegisterErrorObserverL( *this );

    CHAT_DP_TXT( "CCAEngine::ConstructL, CreateGroupClientL" );
    iImpsGroupServer = impsFactory->CreateGroupClientL();

    iImpsGroupServer->RegisterL( iGroupManager, iAppUid,
                                 CActive::EPriorityIdle );
    iImpsGroupServer->RegisterErrorObserverL( *this );

    iStorageObserver = CCAStorageObserver::NewL(
                           *iSettingsManager,
                           *CCAStorageManagerFactory::ContactListInterfaceL(),
                           *iPresenceManager,
                           *iBlockingManager );

    //Create interface for access to history headers
    iLoggerHeadersInterface =
        CCALoggerHeadersInterface::NewL( *iChatContainer, iSettingsManager );

    // whip up the IMPS server
    impsFactory->ConnectL();

    iFullyConstructed = ETrue;

    CHAT_DP_FUNC_DONE( "FinalizeEngineConstructionL" );
    }

// ---------------------------------------------------------
// CCAEngine::IsEngineFinalized()
// ---------------------------------------------------------
//
EXPORT_C TBool CCAEngine::IsFinalized()
    {
    return iFullyConstructed;
    }

// ---------------------------------------------------------
// CCAEngine::IsBackgroundTaskPending()
// ---------------------------------------------------------

EXPORT_C TBool CCAEngine::IsBackgroundTaskPending()
    {
    return iRequestMapper->IsBackgroundTaskPending();
    }

// ---------------------------------------------------------
// CCAEngine::RegisterBackGroundTaskObserver()
// ---------------------------------------------------------
//
EXPORT_C void CCAEngine::RegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver )
    {
    iRequestMapper->RegisterBackGroundTaskObserver( aObserver );
    }

// ---------------------------------------------------------
// CCAEngine::UnRegisterBackGroundTaskObserver()
// ---------------------------------------------------------
//
EXPORT_C void CCAEngine::UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver )
    {
    iRequestMapper->UnRegisterBackGroundTaskObserver( aObserver );
    }



//  End of File
