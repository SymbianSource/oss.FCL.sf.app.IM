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
* Description:  Network operations for chat group handling.
 *
*/

// INCLUDE FILES
#include "ChatDebugPrint.h"

#include "CCAGroupManager.h"

#include "CCAStorageManagerFactory.h"
#include "MCASettings.h"
#include "MCASearchData.h"
#include "MCASearchInterface.h"
#include "MCAStoredGroups.h"
#include "MCAStoredGroup.h"
#include "MCAExtendedStoredGroup.h"
#include "MCAGroupOperations.h"
#include "CCAGroupWrapper.h"
#include "PrivateEngineDefinitions.h"
#include "TStorageManagerGlobals.h"
#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "MCAChatInterface.h"
#include "MCAMessagesWriteInterface.h"
#include "MCAMessageUtils.h"
#include "MCAMessageCreator.h"
#include "ImpsCSPAllErrors.h"
#include "MCAImpsFactory.h"

#include "CAUtils.h"
#include "camessageutil.h"

#include <e32keys.h>
#include <ImpsFundamental.h>
#include <ImpsGroupProps.h>

// for resources
#include <caengineNG.rsg>
#include <stringloader.h>

// CONSTANTS
const TInt KCCAGMGroupWrappersGranularity = 5;
const TInt KSearchPairsSmallGranularity = 1;
const TInt KMaxUserSearchLimit = 100;

// indexes to additional data array inside request
const TInt KReqCreateIndexGroupId    = 0;
const TInt KReqCreateIndexScreenName = 2;
const TInt KReqCreateIndexWelcomeMsg = 3;
const TInt KReqCreateIndexGroupTopic = 4;

const TInt KReqJoinIndexGroupId    = 0;
const TInt KReqJoinIndexScreenName = 1;

// how many times should we try to generate unique group id
const TInt KMaxGroupIds = 100;

// maximum groups in search result. If search results
// exceeds this then "too many search results" is shown
const TInt KMaxGroupSearchLimit = 50;


// ================= MEMBER FUNCTIONS =======================

// Symbian OS default constructor can leave.
void CCAGroupManager::ConstructL()
    {
    iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
    }

// Two-phased constructor.
CCAGroupManager* CCAGroupManager::NewL(
    MCASearchInterface& aSearchAPI,
    MCASettings& aSettingsAPI,
    CCARequestMapper& aRequestMapper,
    MCAImpsFactory* aIMPSFactory,
    MCAChatInterface& aChatInterface,
    MCAMessageUtils& aMessageUtils )
    {
    CCAGroupManager* self = new ( ELeave ) CCAGroupManager( aSearchAPI,
                                                            aSettingsAPI, aRequestMapper, aIMPSFactory,
                                                            aChatInterface, aMessageUtils );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCAGroupManager::~CCAGroupManager()
    {
    iGroupWrappers.ResetAndDestroy();
    delete iProperties;
    delete iPrivProperties;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAGroupManager::CCAGroupManager(
    MCASearchInterface& aSearchAPI,
    MCASettings& aSettingsAPI,
    CCARequestMapper& aRequestMapper,
    MCAImpsFactory* aIMPSFactory,
    MCAChatInterface& aChatInterface,
    MCAMessageUtils& aMessageUtils   )
        : iRequestMapper( aRequestMapper ),
        iSettingsAPI( aSettingsAPI ),
        iSearchAPI( aSearchAPI ),
        iImpsFactory( aIMPSFactory ),
        iChatInterface( aChatInterface ),
        iMessageUtils( aMessageUtils ),
        iGroupWrappers( KCCAGMGroupWrappersGranularity )
    {
    }

// ---------------------------------------------------------
// CCAGroupManager::PopulateCreatedGroupsL
// ---------------------------------------------------------
//
TInt CCAGroupManager::PopulateCreatedGroupsL( CDesCArray& aGroupList ) const
    {
    CHAT_DP_FUNC_ENTER( "PopulateCreatedGroupsL" );

    // Reset the given array to be sure that it will be valid
    aGroupList.Reset();

    HBufC* loggedUserId = iSettingsAPI.ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( loggedUserId );

    CSearchPairs* pairs =
        new ( ELeave ) CSearchPairs( KSearchPairsSmallGranularity );
    CleanupStack::PushL( pairs );

    CImpsSearchRequest* request = CImpsSearchRequest::NewL();
    CleanupStack::PushL( request );

    request->SetRequestL( EImpsGroupUserIDOwner, *loggedUserId );
    pairs->AppendL( request );

    // If there is error in search, return so we don't send stop error request
    TInt searchError( KErrNone );
    searchError = iSearchAPI.StartSearchL( *pairs, KMaxUserSearchLimit, NULL );
    if ( searchError != KErrNone )
        {
        CHAT_DP( D_CHAT_LIT( "CCAGroupManager::PopulateCreatedGroupsL - error \
                              from StartSearchL %d" ), searchError );
        // request, pairs, loggedUserId
        CleanupStack::PopAndDestroy( 3, loggedUserId );
        return searchError;
        }

    TInt resultCount( iSearchAPI.SearchDataInterface()->SearchDataCount() );

    // just add all found groups to the group list
    for ( TInt counter( 0 ); counter < resultCount; counter++ )
        {
        TPtrC pGroupId(
            iSearchAPI.SearchDataInterface()->SearchData( counter ) );
        aGroupList.AppendL( pGroupId );
        }

    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::PopulateCreatedGroupsL - found %d \
                          groups from joined user" ), aGroupList.Count() );

    CleanupStack::PopAndDestroy( 3, loggedUserId );

    CHAT_DP_FUNC_DONE( "PopulateCreatedGroupsL" );

    return KErrNone;
    }

// ---------------------------------------------------------
// CCAGroupManager::JoinedGroups
// ---------------------------------------------------------
//
TInt CCAGroupManager::JoinedGroups()
    {
    return iNumJoinedGroups;
    }


// we just wrap the Handle*L routines by passing them
// on to the correct group wrapper

// ---------------------------------------------------------
// CCAGroupManager::HandleErrorL
// ---------------------------------------------------------
//
void CCAGroupManager::HandleErrorL(
    TInt aStatus,
    TInt aOpId,
    const TDesC* /* aDescription */,
    const CImpsDetailed* /* aDetailedRes */,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    CHAT_DP_FUNC_ENTER( "HandleErrorL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleErrorL - opid=%d, status=%d" ),
             aOpId, aStatus );

    CHAT_DP_FUNC_DONE( "HandleErrorL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleCompleteL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleCompleteL(
    TInt aOpId,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleCompleteL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleCompleteL - opid=%d" ), aOpId );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aOpId );

    if ( ! wrapper )
        {
        // no such wrapper
        CHAT_DP_FUNC_DP( "HandleCompleteL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CCARequest* request = wrapper->Request();
    __ASSERT_DEBUG( request, User::Panic( KCAEnginePanicCategory,
                                          KErrNotFound ) );
    if ( !request )
        {
        User::Leave( KErrNotFound );
        }
    const MDesCArray* dataArray = request->AdditionalData();

    switch ( request->RequestType() )
        {
        case ECreateAndJoinGroup :
            {
            __ASSERT_DEBUG( dataArray, User::Panic( KCAEnginePanicCategory,
                                                    KErrNotFound ) );
            if ( !dataArray )
                {
                // can't use User::LeaveIfNull for const pointers
                User::Leave( KErrGeneral );
                }

            CHAT_DP_FUNC_DP( "HandleCompleteL", "Creating chat data for group" );

            MCAMessagesWriteInterface& groupChat =
                iChatInterface.MessageWriteInterfaceL( KNullDesC, KNullDesC,
                                                       dataArray->MdcaPoint( KReqCreateIndexGroupId ),
                                                       MCAMessagesReadInterface::EGroupContainer );

            groupChat.SetScreenNameL( dataArray->MdcaPoint(
                                          KReqCreateIndexScreenName ) );

            // Write welcome message to chat
            TPtrC welcome = dataArray->MdcaPoint( KReqCreateIndexWelcomeMsg );
            TPtrC topic = dataArray->MdcaPoint( KReqCreateIndexGroupTopic );

            CHAT_DP_FUNC_DP( "HandleCompleteL", "Showing welcome message" );
            MCAStoredGroup& group = wrapper->Group();
            TPtrC groupIdentifier =
                group.GroupName().Length() > 0 ? group.GroupName() :
                CAUtils::DisplayId( group.GroupId() );

            WriteWelcomeMessageL( welcome, groupIdentifier, topic, groupChat );

            // note, it suffices to increase the number of created groups
            // when CreateGroupL is called
            break;
            }

        default:
            {
            break;
            }

        }

    wrapper->HandleCompleteL( aOpId, aCspId );

    CHAT_DP_FUNC_DONE( "HandleCompleteL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleJoinL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleJoinL(
    TInt aOpId,
    const MDesCArray& aUserList,
    const MDesCArray& aScreenNames,
    const TDesC& aWelcomeText,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleJoinL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleJoinL - opid=%d" ), aOpId );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aOpId );

    if ( ! wrapper )
        {
        // no wrapper
        CHAT_DP_FUNC_DP( "HandleJoinL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CCARequest* request = wrapper->Request();
    __ASSERT_DEBUG( request, User::Panic( KCAEnginePanicCategory,
                                          KErrNotFound ) );
    if ( !request )
        {
        User::Leave( KErrNotFound );
        }
    const MDesCArray* dataArray = request->AdditionalData();

    __ASSERT_DEBUG( dataArray, User::Panic( KCAEnginePanicCategory,
                                            KErrNotFound ) );
    if ( !dataArray )
        {
        // can't use User::LeaveIfError for const pointers
        User::Leave( KErrGeneral );
        }

    CHAT_DP_FUNC_DP( "HandleJoinL", "Creating chat data for group" );

    MCAMessagesWriteInterface& groupChat =
        iChatInterface.MessageWriteInterfaceL( KNullDesC, KNullDesC,
                                               dataArray->MdcaPoint( KReqJoinIndexGroupId ),
                                               MCAMessagesReadInterface::EGroupContainer );
    groupChat.SetScreenNameL( dataArray->MdcaPoint( KReqJoinIndexScreenName ) );


    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privProps = NULL;
    // ownership is preserved in wrapper
    wrapper->LocalProperties( commonProps, privProps );

    TPtrC topic( KNullDesC );
    if ( commonProps )
        {
        topic.Set( commonProps->Topic() );
        }

    // Show welcome message
    MCAStoredGroup& group = wrapper->Group();
    TPtrC groupIdentifier =
        group.GroupName().Length() > 0 ? group.GroupName() :
        CAUtils::DisplayId( group.GroupId() );
    TRAPD( err, WriteWelcomeMessageL( aWelcomeText, groupIdentifier, topic, groupChat ) );
    if ( err == KErrNoMemory )
        {
        // Handle no memory error here
        request->SetErrorCode( err );
        request->StopWaitIfNeeded();
        return;
        }
    else
        {
        User::LeaveIfError( err );
        }

    // Topic was shown in WriteWelcomeMessageL
    if ( topic.Length() > 0 )
        {
        wrapper->SetTopicShown( ETrue );
        }

    // make sure it's marked as joined in the UI
    MCAExtendedStoredGroup& grp =
        static_cast<MCAExtendedStoredGroup&>( wrapper->Group() );
    if ( !grp.IsJoined() )
        {
        grp.SetJoined( ETrue );
        iNumJoinedGroups++;
        }

    CHAT_DP_FUNC_DP( "HandleJoinL", "Passing on to wrapper" );
    wrapper->HandleJoinL( aOpId, aUserList, aScreenNames,
                          aWelcomeText, aCspId );

    // show the joined-messages
    TInt numNames = aScreenNames.MdcaCount();
    for ( TInt index = 0; index < numNames; index++ )
        {
        HBufC* newUser = aScreenNames.MdcaPoint( index ).AllocLC();

        HBufC* finalNewUserText =
            StringLoader::LoadLC( R_SYSTEM_MESSAGE_JOIN,
                                  *newUser );

        CHAT_DP_FUNC_DP( "HandleJoinL", "Initiating new users message" );

        MCAMessage* newUserMsg =
            iMessageUtils.MessageCreator().CreateSystemMessageL(
                MCAMessage::ESystemMessageNewUsers,
                *finalNewUserText );

        // Append message
        TRAPD( err, CAMessageUtil::AppendMessageWithDateStampL(
                   *newUserMsg,
                   groupChat,
                   iMessageUtils.MessageCreator() ) );

        if ( err == KErrNoMemory )
            {
            // Handle no memory error here
            CleanupStack::PopAndDestroy( 2, newUser ); // finalNewUserText, newUser
            request->SetErrorCode( err );
            request->StopWaitIfNeeded();
            return;
            }
        else
            {
            User::LeaveIfError( err );
            }

        CleanupStack::PopAndDestroy( 2, newUser ); // finalNewUserText, newUser
        }

    CHAT_DP_FUNC_DONE( "HandleJoinL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleGroupMembersL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleGroupMembersL(
    TInt aOpId,
    const MDesCArray& aUserList,
    const MDesCArray& aScreenNames,
    const MDesCArray& aModers,
    const MDesCArray& aAdmins,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleGroupMembersL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleGroupMembersL - opid=%d" ),
             aOpId );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aOpId );
    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleGroupMembersL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CHAT_DP_FUNC_DP( "HandleGroupMembersL", "Passing on to wrapper" );
    wrapper->HandleGroupMembersL( aOpId, aUserList, aScreenNames, aModers,
                                  aAdmins, aCspId );

    CHAT_DP_FUNC_DONE( "HandleGroupMembersL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleGroupPropertiesL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleGroupPropertiesL(
    TInt aOpId,
    const TDesC& aGroupId,
    const CImpsCommonGroupProps& aGroupProps,
    const CImpsPrivateGroupProps& aOwnProps,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleGroupPropertiesL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleGroupPropertiesL - opid=%d" ),
             aOpId );

    CCAGroupWrapper* wrapper = NULL;
    TPtrC groupId( aGroupId );

    if ( aOpId != 0 )
        {
        // aGroupId is empty
        wrapper = FindGroupWrapper( aOpId );

        if ( !wrapper )
            {
            CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "No wrapper, leaving" );
            User::Leave( KErrNotFound );
            }

        groupId.Set( wrapper->Group().GroupId() );
        }
    else
        {
        // aGroupId is valid
        wrapper = FindGroupWrapper( aGroupId );
        }

    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CImpsCommonGroupProps* common = NULL;
    CImpsPrivateGroupProps* priv = NULL;

    // does the wrapper have the properties already?
    wrapper->LocalProperties( common, priv );

    if ( ( !common ) && ( !priv ) )
        {
        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "No existing properties" );

        // no existing properties, so create new ones
        common = CImpsCommonGroupProps::NewL();
        CleanupStack::PushL( common );

        priv = CImpsPrivateGroupProps::NewL();
        CleanupStack::Pop( common );

        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "Created new properties" );

        // transfer ownership
        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "Setting the properties" );
        wrapper->SetLocalProperties( common, priv );
        }

    // set the admin status
    CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "Setting administrator status" );
    MCAExtendedStoredGroup& group =
        static_cast<MCAExtendedStoredGroup&>( wrapper->Group() );

    if ( aOwnProps.Privileges() == EImpsAdmin )
        {
        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL",
                         "User is administrator in group" );
        group.SetAdmin( ETrue );
        }
    else if ( aOwnProps.Privileges() != EImpsUserUndef )
        {
        CHAT_DP_FUNC_DP( "HandleGroupPropertiesL",
                         "User is not administrator in group" );
        group.SetAdmin( EFalse );
        }

    if ( aGroupProps.GroupName().Length() > 0 )
        {
        if ( aGroupProps.GroupName().CompareC( group.GroupName() ) != 0 )
            {
            // group name has changed, so update it and signal UI
            group.SetGroupNameL( aGroupProps.GroupName() );
            iStoredGroups->SignalGroupChangedL( group.GroupId() );
            }
        }
    // if the group is stored in persistent storage, save changes to disk also
    if ( group.StorageType() == TStorageManagerGlobals::EStoragePersistent )
        {
        group.SaveChangesL();
        }


    // see if anything changed
    MCAMessagesWriteInterface& groupChat =
        iChatInterface.MessageWriteInterfaceL( KNullDesC, KNullDesC, groupId );

    // Ignore empty topics. We must only react to push messages
    // React also when topic was not shown in HandleJoinL while joining
    if ( ( ( aOpId == 0 ) && ( aGroupProps.Topic().Length() > 0 ) )
         || ( !( wrapper->IsTopicShown() ) && ( aOpId == iPropsOpIdWhileJoining ) ) )
        {
        // there is a topic, but has it changed?

        if ( ( ( 0 != aGroupProps.Topic().CompareC( common->Topic() ) ) ||
               !( wrapper->IsTopicShown() ) // Topic changed by user, compare gives 0
             )
             && aOpId == 0 )
            {
            // Check the group topic again, if length > 0, then display the topic
            if ( aGroupProps.Topic().Length() > 0 )
                {
                // it has changed
                CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "Topic has changed" );

                // Topic has changed so initiate system message
                HBufC* newTopic = aGroupProps.Topic().AllocLC();

                HBufC* finalNewTopic =
                    StringLoader::LoadLC( R_SYSTEM_MESSAGE_TOPIC,
                                          *newTopic );

                MCAMessage* topicChgMsg =
                    iMessageUtils.MessageCreator().CreateSystemMessageL(
                        MCAMessage::ESystemMessageTopicChanged,
                        *finalNewTopic );

                // Append message
                CAMessageUtil::AppendMessageWithDateStampL(
                    *topicChgMsg,
                    groupChat,
                    iMessageUtils.MessageCreator() );

                wrapper->SetTopicShown( ETrue );

                CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleGroupPropertiesL - \
                                  topic is %S" ), finalNewTopic );

                CleanupStack::PopAndDestroy( 2, newTopic );//finalNewTopic, newTopic
                }
            }
        else if ( aGroupProps.Topic().Length() > 0
                  && aOpId == iPropsOpIdWhileJoining )
            {
            // Show topic of group when joining
            iPropsOpIdWhileJoining = KErrNotFound;
            HBufC* topicText =
                StringLoader::LoadLC( R_SYSTEM_MESSAGE_TOPIC_TEXT,
                                      aGroupProps.Topic() );
            groupChat.AppendL(
                iMessageUtils.MessageCreator().CreateSystemMessageL(
                    MCAMessage::ESystemMessageWelcome, *topicText ) );

            wrapper->SetTopicShown( ETrue );
            CleanupStack::PopAndDestroy( topicText );
            }
        }

    // Private (whispering) state change. We must only react to push messages
    if ( ( aOpId == 0 ) && ( aGroupProps.IsPrivateAllowed() == EImpsPropYes ) )
        {
        // The state is active, but has it changed?
        if ( aGroupProps.IsPrivateAllowed() != common->IsPrivateAllowed() )
            {
            // private flag has changed
            CHAT_DP_FUNC_DP( "HandleGroupPropertiesL",
                             "Private messaging turn on" );
            HBufC* privatMessaging =
                StringLoader::LoadLC( R_SYSTEM_MESSAGE_PRIVAT_ON );

            MCAMessage* privateChgMsg =
                iMessageUtils.MessageCreator().CreateSystemMessageL(
                    MCAMessage::ESystemMessagePrivatChanged,
                    *privatMessaging );

            // Append message
            CAMessageUtil::AppendMessageWithDateStampL(
                *privateChgMsg,
                groupChat,
                iMessageUtils.MessageCreator() );

            CleanupStack::PopAndDestroy( privatMessaging );
            }
        }
    else
        {
        // we must only react to push messages
        if ( ( aOpId == 0 ) &&
             ( aGroupProps.IsPrivateAllowed() != EImpsPropUndef ) )
            {
            // the flag is in some non-empty, valid, non-Yes state
            // (currently only one such possibility)

            if ( aGroupProps.IsPrivateAllowed() != common->IsPrivateAllowed() )
                {
                CHAT_DP_FUNC_DP( "HandleGroupPropertiesL",
                                 "Private messaging turn off" );

                HBufC* privatMessaging = StringLoader::LoadLC(
                                             R_SYSTEM_MESSAGE_PRIVAT_OFF );

                MCAMessage* privateChgMsg =
                    iMessageUtils.MessageCreator().CreateSystemMessageL(
                        MCAMessage::ESystemMessagePrivatChanged,
                        *privatMessaging );

                // Append message
                CAMessageUtil::AppendMessageWithDateStampL(
                    *privateChgMsg,
                    groupChat,
                    iMessageUtils.MessageCreator() );

                CleanupStack::PopAndDestroy( privatMessaging );
                }
            }
        }

    CHAT_DP_FUNC_DP( "HandleGroupPropertiesL", "Passing on to wrapper" );
    wrapper->HandleGroupPropertiesL( aOpId, groupId, aGroupProps,
                                     aOwnProps, aCspId );

    CHAT_DP_FUNC_DONE( "HandleGroupPropertiesL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleRejectListL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleRejectListL(
    TInt aOpId,
    const MDesCArray& aUserList,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleRejectListL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleRejectListL - opid=%d" ),
             aOpId );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aOpId );
    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleRejectListL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CHAT_DP_FUNC_DP( "HandleRejectListL", "Passing on to wrapper" );
    wrapper->HandleRejectListL( aOpId, aUserList, aCspId );

    CHAT_DP_FUNC_DONE( "HandleRejectListL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleSubscriptionL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleSubscriptionL(
    TInt aOpId,
    TBool aIsSubscribed,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleSubscriptionL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleSubscriptionL - opid=%d" ),
             aOpId );

    // can't be a push message
    CCAGroupWrapper* wrapper = FindGroupWrapper( aOpId );
    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleSubscriptionL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CHAT_DP_FUNC_DP( "HandleSubscriptionL", "Passing on to wrapper" );
    wrapper->HandleSubscriptionL( aOpId, aIsSubscribed, aCspId );

    CHAT_DP_FUNC_DONE( "HandleSubscriptionL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleNewUsersL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleNewUsersL(
    const TDesC& aGroupId,
    const MDesCArray& aUserList,
    const MDesCArray& aScreenNames,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleNewUsersL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleNewUsersL " ) );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );

    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleNewUsersL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    // print the user and screen names list
#ifdef _DEBUG
    TInt i( 0 );
    TInt count( aUserList.MdcaCount() );
    CHAT_DP_FUNC_DP( "HandleNewUsersL", "User list:" );
    for ( i = 0; i < count; i++ )
        {
        TPtrC p = TPtrC( aUserList.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "   %S" ), &p );
        }

    count = aScreenNames.MdcaCount();
    CHAT_DP_FUNC_DP( "HandleNewUsersL", "Screen names list:" );
    for ( i = 0; i < count; i++ )
        {
        TPtrC p = TPtrC( aScreenNames.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "   %S" ), &p );
        }
#endif

    // get the list of participants
    MCAStoredGroup& group = wrapper->Group();

    // this granularity is as good as any
    CDesCArrayFlat* participants =
        new ( ELeave ) CDesCArrayFlat( KJoinedMembersArrayGranularity );
    CleanupStack::PushL( participants );
    group.GetParticipantsL( *participants );

    TInt numNames( aScreenNames.MdcaCount() );

    // list has names
    for ( TInt index = 0; index < numNames; index++ )
        {
        // don't show joined-message for persons already joined to the group
        TInt posIgnored = 0;
        if ( participants->Find( aScreenNames.MdcaPoint( index ),
                                 posIgnored, ECmpFolded ) )
            {
            // a new user, show the message
            HBufC* newUser = aScreenNames.MdcaPoint( index ).AllocLC();

            HBufC* finalNewUserText =
                StringLoader::LoadLC( R_SYSTEM_MESSAGE_JOIN,
                                      *newUser );

            CHAT_DP_FUNC_DP( "HandleNewUsersL",
                             "Initiating new users message" );

            MCAMessagesWriteInterface& groupChat =
                iChatInterface.MessageWriteInterfaceL( KNullDesC,
                                                       KNullDesC, aGroupId );

            MCAMessage* newUserMsg =
                iMessageUtils.MessageCreator().CreateSystemMessageL(
                    MCAMessage::ESystemMessageNewUsers,
                    *finalNewUserText );

            // Append message
            CAMessageUtil::AppendMessageWithDateStampL(
                *newUserMsg,
                groupChat,
                iMessageUtils.MessageCreator() );

            // finalNewUserText, newUser
            CleanupStack::PopAndDestroy( 2, newUser );
            }
        }

    CleanupStack::PopAndDestroy( participants );

    CHAT_DP_FUNC_DP( "HandleNewUsersL", "Passing on to wrapper" );
    wrapper->HandleNewUsersL( aGroupId, aUserList, aScreenNames, aCspId );

    CHAT_DP_FUNC_DONE( "HandleNewUsersL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleLeftUsersL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleLeftUsersL(
    const TDesC& aGroupId,
    const MDesCArray& aUserList,
    const MDesCArray& aScreenNames,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleLeftUsersL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleLeftUsersL" ) );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );

    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleLeftUsersL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    CHAT_DP_FUNC_DP( "HandleLeftUsersL", "Passing on to wrapper" );
    wrapper->HandleLeftUsersL( aGroupId, aUserList, aScreenNames, aCspId );

    TInt numNames( aScreenNames.MdcaCount() );
    // list has names
    for ( TInt index = 0; index < numNames; index++ )
        {
        HBufC* leftUser = aScreenNames.MdcaPoint( index ).AllocLC();

        HBufC* finalLeftUserText =
            StringLoader::LoadLC( R_SYSTEM_MESSAGE_LEAVE,
                                  *leftUser );

        CHAT_DP_FUNC_DP( "HandleLeftUsersL", "Initiating left users message" );

        MCAMessagesWriteInterface& groupChat =
            iChatInterface.MessageWriteInterfaceL( KNullDesC,
                                                   KNullDesC, aGroupId );

        MCAMessage* userLeftMsg =
            iMessageUtils.MessageCreator().CreateSystemMessageL(
                MCAMessage::ESystemMessageLeftUsers, *finalLeftUserText );

        // Append message
        CAMessageUtil::AppendMessageWithDateStampL(
            *userLeftMsg,
            groupChat,
            iMessageUtils.MessageCreator() );

        CleanupStack::PopAndDestroy( 2, leftUser ); // finalLeftUserText, leftUser
        }

    CHAT_DP_FUNC_DONE( "HandleLeftUsersL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::HandleLeaveL()
// ---------------------------------------------------------
//
void CCAGroupManager::HandleLeaveL(
    TInt aOpId,
    const TDesC& aGroupId,
    const TDesC& aDescription,
    TImpsCspIdentifier& aCspId )
    {
    CHAT_DP_FUNC_ENTER( "HandleLeaveL" );
    CHAT_DP( D_CHAT_LIT( "CCAGroupManager::HandleLeaveL - opid=%d" ), aOpId );

    CCAGroupWrapper* wrapper = NULL;
    if ( aOpId != 0 )
        {
        // operation id is valid
        wrapper = FindGroupWrapper( aOpId );
        }
    else
        {
        // group id is valid
        wrapper = FindGroupWrapper( aGroupId );
        }

    if ( ! wrapper )
        {
        CHAT_DP_FUNC_DP( "HandleLeaveL", "No wrapper, leaving" );
        User::Leave( KErrNotFound );
        }

    // handle the leave in wrapper
    MCAExtendedStoredGroup& gr =
        static_cast<MCAExtendedStoredGroup&>( wrapper->Group() );

    // Work with copy of group id, as wrapper gets deleted soon.
    // Also, group id is not valid in push.
    HBufC* grId = gr.GroupId().AllocLC();

    CHAT_DP_FUNC_DP( "HandleLeaveL", "Passing on to wrapper" );

    wrapper->LastImpsError( iLastImpsError );
    //reset/consume last error
    iLastImpsError = KErrNone;

    wrapper->HandleLeaveL( aOpId, *grId, aDescription, aCspId );

    if ( aOpId == 0 )
        {
        CHAT_DP_FUNC_DP( "HandleLeaveL",
                         "Push message, cleaning up in HandleLeaveL" );
        // push messages we cleanup here,
        // pull messages in the LeaveGroupL which originated
        // this call chain
        CleanupLeaveGroupL( *grId );
        }

    CleanupStack::PopAndDestroy( grId );
    CHAT_DP_FUNC_DONE( "HandleLeaveL" );
    }

// set group as un-joined, and delete wrapper, group and associated
// group chat data
void CCAGroupManager::CleanupLeaveGroupL( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "CleanupLeaveGroupL" );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );
    if ( !wrapper )
        {
        CHAT_DP_FUNC_DP( "CleanupLeaveGroupL", "no wrapper, nothing to do!" );
        return;
        }
    MCAExtendedStoredGroup& gr =
        static_cast<MCAExtendedStoredGroup&>( wrapper->Group() );

    CHAT_DP_FUNC_DP( "CleanupLeaveGroupL", "Leaving group" );

    if ( gr.IsJoined() )
        {
        gr.SetJoined( EFalse );
        iNumJoinedGroups--;
        }

    if ( ( !gr.IsOwnGroup() ) &&
         ( gr.StorageType() != TStorageManagerGlobals::EStoragePersistent ) )
        {
        // not own, not stored and not joined (after CleanupLeaveGroupL)
        // aWrappers shouldn't be on the list, so delete also from storage
        // (not from server)
        CHAT_DP_FUNC_DP( "CleanupLeaveGroupL", "Deleting aWrapper and group" );
        // delete returns error code of network delete operation
        // in case of group leave it is not import if group deleting fails
        // from server, return code ignored
        DeleteGroupL( aGroupId, EFalse );
        }
    else
        {
        // delete all server-related things:
        // members, participants, admin status, whispering, etc.
        gr.ResetMembersLocally();
        gr.ResetParticipantsL();

        gr.SetAdmin( EFalse );

        // delete wrapper
        DeleteGroupWrapper( aGroupId );
        RemoveChatDataL( aGroupId );
        }

    CHAT_DP_FUNC_DONE( "CleanupLeaveGroupL" );
    }



// ---------------------------------------------------------
// CCAGroupManager::NumberOfCreatedGroupsDuringSession
// ---------------------------------------------------------
//
TInt CCAGroupManager::NumberOfCreatedGroupsDuringSession() const
    {
    return iNumCreatedGroups;
    }

// ---------------------------------------------------------
// CCAGroupManager::GroupOperations
// ---------------------------------------------------------
//
MCAGroupOperations* CCAGroupManager::GroupOperationsL( const TDesC& aId )
    {
    CHAT_DP_FUNC_ENTER( "GroupOperationsL" );

    CCAGroupWrapper* groupWrapper = FindGroupWrapper( aId );
    if ( groupWrapper )
        {
        // found an existing wrapper
        CHAT_DP_FUNC_DONE( "GroupOperationsL" );
        return groupWrapper;
        }

    CHAT_DP_FUNC_DONE( "GroupOperationsL" );

    // no existing wrapper. make a new one, store it and return it
    return CreateWrapperL( aId );
    }

// ---------------------------------------------------------
// CCAGroupManager::SetGroupEventObserver
// Call with NULL observer is safely handled at all times
// ---------------------------------------------------------
//
void CCAGroupManager::SetGroupEventObserverL( const TDesC& aId,
                                              MCAGroupEventObserver* aObserver )
    {
    CHAT_DP_FUNC_ENTER( "SetGroupEventObserverL" );

    CCAGroupWrapper* groupWrapper = FindGroupWrapper( aId );

    if ( ! groupWrapper )
        {
        // not found
        if ( aObserver )
            {
            // valid observer
            CHAT_DP_FUNC_DP( "SetGroupEventObserverL", "Creating new wrapper" );
            groupWrapper = CreateWrapperL( aId );
            }
        }

    if ( groupWrapper )
        {
        // no new wrappers are to be created for null observers - in
        // such a case there's no observer anyway!
        CHAT_DP_FUNC_DP( "SetGroupEventObserverL", "Setting event observer" );
        groupWrapper->RegisterEventObserver( aObserver );
        }

    CHAT_DP_FUNC_DONE( "SetGroupEventObserverL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::CreateGroupL
// ---------------------------------------------------------
//
HBufC* CCAGroupManager::CreateGroupL(
    CImpsCommonGroupProps* aProperties,
    CImpsPrivateGroupProps* aPrivProperties,
    const TDesC& aScreenName,
    TBool aJoinGroup,
    TBool aIsWhisperingEnabled,
    TInt& aErrorCode
)
    {
    CHAT_DP_FUNC_ENTER( "CreateGroupL" );

    if ( !aProperties || !aPrivProperties )
        {
        User::Leave( KErrArgument );
        }
    // 1. create group to storage
    // 2. invent a group name for the newly created group
    // 3. create a group wrapper
    // 4. pass this method on to the group wrapper
    // 5. return the group id


    // deleting old values, just in case that previous CreateGroup has failed
    delete iProperties;
    delete iPrivProperties;
    iProperties = aProperties;
    iPrivProperties = aPrivProperties;

    // create group
    MCAExtendedStoredGroup* exGrp = iStoredGroups->CreateGroupL();
    // Check memory level, leaves group creation with KErrDiskFull
    // now if new group cannot be added to persistent database
    TRAPD( err, iStoredGroups->CheckFreespaceLevelL( exGrp ) );
    if ( err == KErrDiskFull )
        {
        // Make sure nothing is left to storage
        iStoredGroups->DeleteGroupL( exGrp );
        User::Leave( err );
        }
    exGrp->SetAdmin( ETrue );
    exGrp->SetGroupNameL( aProperties->GroupName() );

    CHAT_DP( D_CHAT_LIT( "Group whisper, common=%d, priv=%d" ),
             aProperties->IsPrivateAllowed(),
             aPrivProperties->IsPrivateAllowed() );

    TBool allDone = EFalse;
    TInt maxTries( KMaxGroupIds );   // fail-safe to prevent infinite loop

    CCAGroupWrapper* grOp = NULL;
    while ( ( ! allDone ) && ( maxTries > 0 ) )
        {
        HBufC* groupId = NULL;
        while ( !groupId )
            {
            groupId = GenerateGroupIdLC( aProperties->GroupName() );
            if ( iStoredGroups->FindGroup( *groupId ) )
                {
                // group was found from storage, so let's
                // delete it and try next one
                CleanupStack::PopAndDestroy( groupId );
                groupId = NULL;
                }
            }

        // now we have a unique group id that is not in storage,
        exGrp->SetGroupIdL( *groupId );
        CleanupStack::PopAndDestroy( groupId );
        groupId = NULL;

        if ( !grOp )
            {
            // we don't have a wrapper yet, so let's
            // create it for the group and set the properties
            grOp = static_cast<CCAGroupWrapper*>( GroupOperationsL(
                                                      exGrp->GroupId() ) );
            // wrapper takes the ownership of properties from now on
            grOp->SetLocalProperties( aProperties, aPrivProperties );
            iProperties = NULL;
            iPrivProperties = NULL;
            }


        // finalize group creation to the network
        CHAT_DP_FUNC_DP( "CreateGroupL", "Creating group to network" );
        aErrorCode = grOp->CreateGroupL( aScreenName, aJoinGroup );

        if ( ( aErrorCode == KErrNone )
             || ( aErrorCode == ECSPPartiallySuccessful )  )
            {
            // search groups from server, in order to get the new groupid
            // begin
            HBufC* loggedUserId = iSettingsAPI.ValueL( MCASettings::EOwnWVUserID );
            CleanupStack::PushL( loggedUserId );
            
            CSearchPairs* tempPairs = 
				new ( ELeave ) CSearchPairs( KSearchPairsSmallGranularity );
            CleanupStack::PushL( tempPairs );
            CImpsSearchRequest* tempRequest = CImpsSearchRequest::NewL();
            CleanupStack::PushL( tempRequest );
            tempRequest->SetRequestL( EImpsGroupUserIDOwner, *loggedUserId );
            tempPairs->AppendL( tempRequest );

            // If there is error in search, return so we don't send stop error request
            TInt searchError( KErrNone );
            TInt err( KErrNone );
            TRAP( err, searchError = 
				iSearchAPI.StartSearchL( *tempPairs, KMaxGroupSearchLimit, NULL ) );
                
            CleanupStack::PopAndDestroy( 3, loggedUserId );// tempRequest, tempPairs, loggedUserId
            
            if ( err != KErrNone )
            	{
            	User::LeaveIfError( err );
            	}
            else if ( searchError != KErrNone )
            	{
            	User::LeaveIfError( searchError );
            	}
            
            TInt resultCount( 
            		iSearchAPI.SearchDataInterface()->SearchDataCount() );

            for ( TInt counter( 0 ); counter < resultCount; counter++ )
            	{
            	// see if this group is already in storage
            	TPtrC groupId( iSearchAPI.SearchDataInterface()->SearchData( 
            			counter ) );
            	MCAStoredGroup* group = iStoredGroups->FindGroup( groupId );

            	if ( !group )
            		{
            		exGrp->SetGroupIdL( groupId );
            		break;
            		}
            	}
            // end

            // must update to the server
            if ( aIsWhisperingEnabled )
                {
                SetPrivateAllowedL( *grOp, *aPrivProperties );
                }

            // this is our group
            exGrp->SetOwnGroup( ETrue );

            if ( !exGrp->IsJoined() && aJoinGroup )
                {
                iNumJoinedGroups++;
                }
            // set the joined status
            exGrp->SetJoined( aJoinGroup );

            // make it a favorite, automatically
            exGrp->SaveChangesL();

            // tell the UI the group has been added
            iStoredGroups->SignalGroupAddedL( exGrp->GroupId() );

            aErrorCode = KErrNone; // it's fully successful now...
            allDone = ETrue;       // get out too
            }
        else
            {
            if ( aErrorCode == ECSPGroupAlreadyExists )
                {
                // group already exists
                CHAT_DP_FUNC_DP( "CreateGroupL",
                                 "Wrapper's CreateGroupL failed, trying with another groupid" );

                maxTries--;
                }
            else
                {
                // delete the group from storage if CreateGroupL failed,
                // aErrorCode is already set for use after return
                CHAT_DP_FUNC_DP( "CreateGroupL",
                                 "Wrapper's CreateGroupL failed, aborting" );
                // delete return value is network operation error code
                // we are not interested it,
                // because group creation already failed
                DeleteGroupL( exGrp->GroupId(), EFalse );
                // group got deleted, return NULL
                return NULL;
                }
            }
        }

    // failsafe, if we reached the maximum number of tries
    if ( maxTries == 0 )
        {
        // everything is cleaned up in the loop
        return NULL;
        }

    // Add creator of group as participant to group.
    // Just because creator is participating group.
    // Server does not always inform us for this one.
    CDesCArrayFlat* participants =
        new ( ELeave ) CDesCArrayFlat( KJoinedMembersArrayGranularity );
    CleanupStack::PushL( participants );
    participants->AppendL( aScreenName );
    exGrp->AddParticipantL( *participants );
    CleanupStack::PopAndDestroy( participants );

    iNumCreatedGroups++;

    CHAT_DP_FUNC_DONE( "CreateGroupL" );

    return exGrp->GroupId().AllocL(); // ownership is transferred
    }

// ---------------------------------------------------------
// CCAGroupManager::JoinGroupL
// We must wrap JoinL to make sure the properties are
// in place. It can't be done in wrapper as it will
// mess up the message sequences.
// ---------------------------------------------------------
//
TInt CCAGroupManager::JoinGroupL( const TDesC& aGroupId,
                                  const TDesC& aScreenName, TBool aUsers,
                                  TBool aIsFavourite, TBool aIsWhisperingEnabled )
    {
    CHAT_DP_FUNC_ENTER( "JoinGroupL" );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );

    if ( ! wrapper )
        {
        // no wrapper, create one
        // exists in a internal list, so no need to use cleanupstack
        wrapper = CreateWrapperL( aGroupId );
        }

    // check if we need to fetch the properties from the network
    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privateProps = NULL;
    wrapper->LocalProperties( commonProps, privateProps );
    // ownership is kept in wrapper

    // now we can join the group
    TInt joinStatus = KErrNone;
    joinStatus = wrapper->JoinL( aScreenName, aUsers, aIsFavourite );
    if ( joinStatus == EOperationCancelled)
        {
        return joinStatus;    
        }
    TInt retStatus( KErrNone );
    if ( ( !commonProps ) || ( !privateProps ) )
        {
        // no properties, so request from network
        // Store op id of properties request
        iPropsOpIdWhileJoining = wrapper->OperationId() + 1;
        retStatus = wrapper->GetPropertiesL( commonProps, privateProps );
        if ( retStatus == EOperationCancelled)
            {
            return retStatus;
            }
        if ( retStatus != ECSPInsufficientGroupPrivileges )
            {
            if ( ( retStatus != ECSPSuccessful ) && ( retStatus != KErrNone ) )
                {
                // got some failure
                iPropsOpIdWhileJoining = KErrNotFound;  // Init to -1
                return retStatus;
                }

            // fail-safe for buggy server returning NULL to these for some reason
            if ( ( ! commonProps ) || ( ! privateProps ) )
                {
                iPropsOpIdWhileJoining = KErrNotFound;  // Init to -1
                return ECSPInsufficientGroupPrivileges;
                }
            }
        }

    // Note! ChatData is created in HandleJoinL, but HandleJoinL
    // is not called if we get 807 ECSPGroupAlreadyJoined. So we must work
    // around to recover from that.

    if ( joinStatus == ECSPGroupAlreadyJoined )
        {
        // Make sure our status is in sync with server
        // status, then we can recover from 807 ECSPGroupAlreadyJoined.
        // If we get ECSPGroupAlreadyJoined, HandleJoinL is never called,
        // but HandleErrorL (Engine) instead!

        // create the chat data, if we don't have it

        MCAMessagesWriteInterface& groupChat =
            iChatInterface.MessageWriteInterfaceL(
                KNullDesC, KNullDesC, aGroupId,
                MCAMessagesReadInterface::EGroupContainer );
        groupChat.SetScreenNameL( aScreenName );

        // make sure it's marked as joined in the UI
        MCAExtendedStoredGroup& grp =
            static_cast<MCAExtendedStoredGroup&>( wrapper->Group() );
        if ( !grp.IsJoined() )
            {
            grp.SetJoined( ETrue );
            iNumJoinedGroups++;
            }
        }

    // also update private properties to the group
    if ( ( joinStatus == ECSPGroupAlreadyJoined )
         || ( joinStatus == ECSPSuccessful )
         || ( joinStatus == KErrNone ) )
        {
        if ( aIsWhisperingEnabled && privateProps )
            {
            joinStatus = SetPrivateAllowedL( *wrapper, *privateProps );
            if ( joinStatus == EOperationCancelled )
            	{
            	return joinStatus;
            	}
            }
        joinStatus = KErrNone;
        }

    CHAT_DP_FUNC_DONE( "JoinGroupL" );

    return joinStatus;
    }

// ---------------------------------------------------------
// CCAGroupManager::LeaveJoinedGroups
// ---------------------------------------------------------
//
void CCAGroupManager::LeaveJoinedGroupsL()
    {
    TInt count = iGroupWrappers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        MCAStoredGroup& group = iGroupWrappers[ i ]->Group();
        if ( group.IsJoined() )
            {
            iGroupWrappers[ i ]->LeaveL();
            CleanupLeaveGroupL( group.GroupId() );
            }
        }
    }

// ---------------------------------------------------------
// CCAGroupManager::LeaveGroupL
// We must wrap LeaveL to make sure the UI and
// IMPS engine running in different threads do not
// cause race conditions when UI calls LeaveL directly
// and IMPS engine informs via HandleLeaveL.
// ---------------------------------------------------------
//
TInt CCAGroupManager::LeaveGroupL( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "LeaveGroupL" );

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );

    if ( ! wrapper )
        {
        // not Leaving with KErrNone due to popular request
        return KErrNone;
        }

    // leave the group
    TInt retStatus = wrapper->LeaveL();

    // nuke the wrappers etc.
    CleanupLeaveGroupL( aGroupId );

    CHAT_DP_FUNC_DONE( "LeaveGroupL" );
    return retStatus;
    }

// ---------------------------------------------------------
// CCAGroupManager::GenerateGroupIdLC
// XXX cleaned up the forbidden char removal part,
// but can this be cleaned up any more?
// ---------------------------------------------------------
//
HBufC* CCAGroupManager::GenerateGroupIdLC( const TDesC& aResourcePart )
    {

    HBufC* groupId = HBufC::NewLC( KMaxGroupIDLength );
    TPtr groupIdPtr( groupId->Des() );

    // Retrieve logged user information
    HBufC* loggedUserId = iSettingsAPI.ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( loggedUserId );

    // find "@"
    TInt atIndex( loggedUserId->FindC( KAt ) );
    if ( atIndex == KErrNotFound )
        {
        atIndex = loggedUserId->Length();
        }

    // Find string between ":" and "@"
    // If there is no "wv:" start from beginning of user id
    TInt startPosition = 0;
    if ( KWV().CompareC( loggedUserId->Left( KWV().Length() ),
                         KCollationLevel, NULL ) == 0 )
        {
        startPosition = KWV().Length();
        }

    TPtrC userBaseName( loggedUserId->Mid(
                            startPosition, atIndex - startPosition ) );

    // We must get rid of the unneeded "@" characters
    TPtrC groupBaseName;
    HBufC* temp = CAUtils::GenerateIdLC( aResourcePart );
    TPtr tempPtr( temp->Des() );

    // group name empty? If it is, use userBaseName after "/" in group id
    if ( aResourcePart.Length() < 1 )
        {
        groupBaseName.Set( userBaseName );
        }
    else
        {
        // Sanity check. We removed characters, is there anything in the string?
        if ( tempPtr.Length() < 1 )
            {
            groupBaseName.Set( userBaseName );
            }
        else
            {
            groupBaseName.Set( tempPtr );
            }

        CHAT_DP( D_CHAT_LIT( "Group base name is %S" ), &groupBaseName );
        }

    // Add "wv:" at the beginning
    groupIdPtr.Copy( KWV );

    // Add "middle" part of WV user ID
    //
    // EXAMPLE 1:
    // User id = wv:jari20@domain.com
    // Group name = grouppi
    // Group id becomes wv:jari20/grouppi
    //
    // EXAMPLE 2:
    // User id = wv:jari20@domain.com
    // Group name is empty
    // Group id becomes wv:jari20/jari20

    groupIdPtr += userBaseName;

    // Add separator "/"
    groupIdPtr += KResourceSeparator;

    TInt groupInsertIndex( groupIdPtr.Length() );

    // don't add ordinal the first time
    if ( iGroupIdOrdinal > 0 )
        {
        // Format ordinal, 5 is enough
        TBuf<5> ordinal;
        ordinal.Num( iGroupIdOrdinal );
        // Add ordinal
        groupIdPtr += ordinal;
        }

    // Add domain part
    groupIdPtr += loggedUserId->Mid( atIndex );

    // Add groupId last so we can know how much it can be inserted.
    groupIdPtr.Insert( groupInsertIndex,
                       groupBaseName.Left(
                           KMaxGroupIDLength - groupIdPtr.Length() ) );

    CleanupStack::PopAndDestroy( temp );
    CleanupStack::PopAndDestroy( loggedUserId );

    // Increase ordinal
    ++iGroupIdOrdinal;

    return groupId;
    }


// ---------------------------------------------------------
// CCAGroupManager::DeleteGroupL
// ---------------------------------------------------------
//
TInt CCAGroupManager::DeleteGroupL( const TDesC& aGroupId,
                                    TBool aDeleteFromNetwork )
    {
    CHAT_DP_FUNC_ENTER( "DeleteGroupL" );

    TBuf<KMaxGroupNameLength> groupid;
    groupid.Copy( aGroupId.Left( groupid.MaxLength() ) );

    TInt retVal( KErrNone );
    CCAGroupWrapper* wrapper = NULL;

    if ( aDeleteFromNetwork )
        {
        // delete from network

        wrapper = FindGroupWrapper( aGroupId );

        if ( ! wrapper )
            {
            // no wrapper, create one
            wrapper = CreateWrapperL( aGroupId );   // CSI: 35 # Ownership is not transferred to caller.
            }

        retVal = wrapper->DeleteFromNetworkL();
        }
    else
        {
        // deleting locally, see if we have a wrapper
        wrapper = FindGroupWrapper( aGroupId );
        }

    if ( wrapper )
        {
        // have wrapper, delete it
        DeleteGroupWrapper( aGroupId );
        }

    // delete from storage manager
    CHAT_DP_FUNC_DP( "DeleteGroupL", "Deleting from storage" );

    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( retVal == KErrNone || retVal == ECSPGroupDoesNotExist )
        {
        if ( group )
            {
            iStoredGroups->DeleteGroupL( group );
            }
        }

    // delete the chat data
    RemoveChatDataL( groupid );
    CHAT_DP_FUNC_DONE( "DeleteGroupL" );

    return retVal;
    }

// ---------------------------------------------------------
// CCAGroupManager::IsAllowedAccessL
// check access to given group
// ---------------------------------------------------------
//
TBool CCAGroupManager::IsAllowedAccessL( const TDesC& aGroupId,
                                         const CDesCArray& aCheckUsers,
                                         CDesCArray& aNotInList )
    {
    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );
    if ( !wrapper )
        {
        // This should never happen, but just in case...
        CHAT_DP_TXT( "CCAGroupManager::IsAllowedAccessL - \
                      wrapper doesn't exist" );
        return EFalse;
        }
    MCAStoredGroup& group = wrapper->Group();

    aNotInList.Reset();

    // all users, including administrators
    CDesCArray* allGroupUsers =
        new ( ELeave ) CDesCArrayFlat( KJoinedMembersArrayGranularity );
    CleanupStack::PushL( allGroupUsers );
    group.GetMembersL( *allGroupUsers );

    TInt checkCount( aCheckUsers.Count() );
    TInt position( 0 );

    if ( ! IsGroupOpenL( aGroupId ) )
        {
        // closed group, so check access

        CHAT_DP_TXT( "IsAllowedAccessL, checking for valid access" );
        // Go through list of users to check and see if
        // they're in the list of all users
        for ( TInt i( 0 ); i < checkCount; ++i )
            {
            if ( allGroupUsers->Find( aCheckUsers[i], position, ECmpCollated ) )
                {
                aNotInList.AppendL( aCheckUsers[i] );
                }
            }
        }

    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privProps = NULL;
    GetPropertiesL( aGroupId, commonProps, privProps );

    TBool isLoggedUserAdmin = group.IsAdmin();

    CleanupStack::PopAndDestroy( allGroupUsers );

    return isLoggedUserAdmin;
    }

// check if group is open or closed
TBool CCAGroupManager::IsGroupOpenL( const TDesC& aGroupId )
    {
    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privProps = NULL;
    GetPropertiesL( aGroupId, commonProps, privProps );

    return commonProps->IsOpen();
    }

// map group id to group name
HBufC* CCAGroupManager::GroupNameL( const TDesC& aGroupId )
    {
    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privProps = NULL;
    GetPropertiesL( aGroupId, commonProps, privProps );

    HBufC* buf = NULL;
    if ( commonProps )
        {
        buf = commonProps->GroupName().AllocL();    // CSI: 35 # No leaving code after allocation, ownership transferred to caller.
        }
    else
        {
        // can't get group name from properties
        // returning empty groupname
        buf = KNullDesC().AllocL();
        }

    return buf;
    }

// handles logout event
void CCAGroupManager::HandleNetworkStateChangeL( TNetworkState  aState )
    {
    if ( aState == ELoggedOut )
        {
        RPointerArray<MCAStoredGroup> groups;
        CleanupClosePushL( groups );
        iStoredGroups->PopulateGroupsListL( groups );

        // loop through all the groups
        TInt count( groups.Count() );
        for ( TInt i( 0 ); i < count; i++ )
            {
            MCAExtendedStoredGroup* exGrp =
                static_cast<MCAExtendedStoredGroup*>( groups[ i ] );
            if ( exGrp->IsJoined() )
                {
                // the group is joined, so leave it and perform cleanup
                exGrp->SetJoined( EFalse );
                iNumJoinedGroups--;
                CleanupLeaveGroupL( exGrp->GroupId() );
                }
            }
        CleanupStack::PopAndDestroy(); // groups.Close()
        }
    }

// ---------------------------------------------------------
// CCAGroupManager::LastImpsError
// ---------------------------------------------------------
//
void CCAGroupManager::LastImpsError( TInt aError )
    {
    iLastImpsError = aError;
    }

// used internally to find the proper group wrapper
CCAGroupWrapper* CCAGroupManager::FindGroupWrapper( TInt aOpId )
    {
    TInt count( iGroupWrappers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        CCAGroupWrapper* wrapper = iGroupWrappers[i];
        if ( aOpId == wrapper->OperationId() )
            {
            // found it
            return wrapper;
            }
        }

    // didn't find it
    return NULL;
    }

// used internally to find the proper group wrapper based on group id
CCAGroupWrapper* CCAGroupManager::FindGroupWrapper( const TDesC& aGroupId )
    {
    TInt count( iGroupWrappers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        CCAGroupWrapper* wrapper = iGroupWrappers[i];

        // get the group the wrapper wraps
        MCAStoredGroup& g = wrapper->Group();

        if ( 0 == CAUtils::NeutralCompare( g.GroupId(), aGroupId ) )
            {
            // found it
            return wrapper;
            }
        }

    // didn't find it
    return NULL;
    }

// used internally to delete the proper group wrapper based on group id
void CCAGroupManager::DeleteGroupWrapper( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "DeleteGroupWrapper" );

    TInt count( iGroupWrappers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        CCAGroupWrapper* wrapper = iGroupWrappers[i];

        // get the group the wrapper wraps
        MCAStoredGroup& g = wrapper->Group();

        if ( 0 == g.GroupId().CompareC( aGroupId, KCollationLevel, NULL ) )
            {
            CHAT_DP_FUNC_DP( "DeleteGroupWrapper", "Found wrapper, deleting it" );
            // found it! delete it
            iGroupWrappers.Remove( i );
            delete wrapper;
            iGroupWrappers.Compress();
            return;
            }
        }

    // didn't find it
    CHAT_DP_TXT( "CCAGroupManager::DeleteGroupWrapper tried to delete wrapper \
                  that doesn't exist!" );

    CHAT_DP_FUNC_DONE( "DeleteGroupWrapper" );
    }

// get properties for the given group
void CCAGroupManager::GetPropertiesL( const TDesC& aGroupId,
                                      CImpsCommonGroupProps*& aCommonProps,
                                      CImpsPrivateGroupProps*& aPrivProps )
    {
    CHAT_DP_FUNC_ENTER( "PropertiesL" );

    TBool tempGroup( EFalse );
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( !group )
        {
        group = iStoredGroups->CreateGroupL();  // CSI: 35 # Ownership is not transferred to caller.
        static_cast<MCAExtendedStoredGroup*>( group )->SetGroupIdL( aGroupId );
        static_cast<MCAExtendedStoredGroup*>( group )->SetVisible( EFalse );
        tempGroup = ETrue;
        }

    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );
    if ( ! wrapper )
        {
        wrapper = CreateWrapperL( aGroupId );   // CSI: 35 # Ownership is not transferred to caller.
        }

    // ownership does not get transferred
    wrapper->GetPropertiesL( aCommonProps, aPrivProps );

    if ( tempGroup )
        {
        DeleteGroupL( aGroupId, EFalse );
        }

    CHAT_DP_FUNC_DONE( "PropertiesL" );
    }

// create a group wrapper
CCAGroupWrapper* CCAGroupManager::CreateWrapperL( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "CreateWrapperL" );

    // wrapper needs a group
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( !group )
        {
        // no such group at all
        CHAT_DP_FUNC_DP( "CreateWrapperL", "No group!!" );
        User::Leave( KErrNotFound );
        }

    CHAT_DP_FUNC_DP( "CreateWrapperL", "Found group" );

    // create a new wrapper
    CHAT_DP_FUNC_DP( "CreateWrapperL", "Creating group wrapper" );

    CCAGroupWrapper* groupWrapper = CCAGroupWrapper::NewL( iSettingsAPI,
                                                           static_cast<MCAExtendedStoredGroup&>( *group ), iRequestMapper,
                                                           iImpsFactory->CreateGroupClientL() );
    CleanupStack::PushL( groupWrapper );

    // ownership transferred to array
    TInt retVal( iGroupWrappers.Append( groupWrapper ) );

    User::LeaveIfError( retVal );
    CleanupStack::Pop( groupWrapper );

    CHAT_DP_FUNC_DONE( "CreateWrapperL" );

    // that's it
    return groupWrapper;
    }

// set the private messaging to allowed
TInt CCAGroupManager::SetPrivateAllowedL( CCAGroupWrapper& aWrapper,
                                          CImpsPrivateGroupProps& aPrivProps )
    {
    aPrivProps.SetPrivateAllowed( EImpsPropYes );
    // the return value of updateproperties can be ignored because
    // there's no point showing "server error" note if the joining
    // process was otherwise successful.
    TInt operationvalue = aWrapper.UpdatePropertiesL( NULL, &aPrivProps );
    
    return operationvalue;
    }

// remove the given chat data
void CCAGroupManager::RemoveChatDataL( const TDesC& aGroupId )
    {
    CHAT_DP_FUNC_ENTER( "RemoveChatDataL" );
    iChatInterface.DeleteChatL( KNullDesC, KNullDesC, aGroupId );
    CHAT_DP_FUNC_DONE( "RemoveChatDataL" );
    }

// ---------------------------------------------------------
// CCAGroupManager::WriteWelcomeMessageL
// ---------------------------------------------------------
//
void CCAGroupManager::WriteWelcomeMessageL(
    const TDesC& aWelcomeMessage,
    const TDesC& aGroup,
    const TDesC& aTopic,
    MCAMessagesWriteInterface& aMessages )
    {
    // I can't figure out a situation in which these would require a
    // datestamp. But if that is the case at some point, use
    // CAMessageUtil::AppendMessageWithDateStampL for appending.

    // show customized welcome message
    if ( aWelcomeMessage.Length() > 0 )
        {
        aMessages.AppendL(
            iMessageUtils.MessageCreator().CreateSystemMessageL(
                MCAMessage::ESystemMessageWelcome, aWelcomeMessage ) );
        }
    // show default welcome message
    else
        {
        HBufC* welcome =
            StringLoader::LoadLC( R_SYSTEM_MESSAGE_WELCOME_TEXT, aGroup );

        aMessages.AppendL(
            iMessageUtils.MessageCreator().CreateSystemMessageL(
                MCAMessage::ESystemMessageWelcome, *welcome ) );

        CleanupStack::PopAndDestroy( welcome );
        }

    // Only show real topic
    if ( aTopic.Length() > 0 )
        {
        // load & show default topic string
        HBufC* topicText =
            StringLoader::LoadLC( R_SYSTEM_MESSAGE_TOPIC_TEXT, aTopic );
        aMessages.AppendL(
            iMessageUtils.MessageCreator().CreateSystemMessageL(
                MCAMessage::ESystemMessageWelcome, *topicText ) );
        CleanupStack::PopAndDestroy( topicText );
        }
    }
// ---------------------------------------------------------
// CCAGroupManager::CancelGroupOperationL
// ---------------------------------------------------------
//
void CCAGroupManager::CancelGroupOperationL( const TDesC& aGroupId )
    {
    CCAGroupWrapper* wrapper = FindGroupWrapper( aGroupId );
    if ( !wrapper )
        {
        return;
        }
   CCARequest* request = wrapper->Request();
       if ( request )
        {
        request->StopWaitIfNeeded(); 
        request->SetErrorCode( EOperationCancelled );   
        }   
    }
//  End of File
