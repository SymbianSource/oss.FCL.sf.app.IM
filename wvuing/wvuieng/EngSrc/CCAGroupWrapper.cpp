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
* Description:  Network operations for one chat group handling
*
*/



// INCLUDE FILES
#include "MCAImpsGroupClient.h"
#include "MCASettings.h"
#include "MCAExtendedStoredGroup.h"
#include "MCAGroupEventObserver.h"

#include "ImpsCSPAllErrors.h"
#include "CCAGroupWrapper.h"
#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "ChatDebugPrint.h"

#include <ImpsErrors.h>
#include <coemain.h>     // For CCoeEnv

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAGroupWrapper::CCAGroupWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAGroupWrapper::CCAGroupWrapper( MCASettings& aSettingsAPI,
                                  MCAExtendedStoredGroup& aGroup,
                                  CCARequestMapper& aRequestMapper,
                                  MCAImpsGroupClient* aClient  )
        : iSettingsAPI( aSettingsAPI ),
        iGroup( aGroup ),
        iConeEnv( CCoeEnv::Static() ),
        iRequestMapper( aRequestMapper ),
        iImpsGroupClient( aClient ),
        iOperationError( KErrNone )
    {
    }

// -----------------------------------------------------------------------------
// CCAGroupWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAGroupWrapper* CCAGroupWrapper::NewL( MCASettings& aSettingsAPI,
                                        MCAExtendedStoredGroup& aGroup,
                                        CCARequestMapper& aRequestMapper,
                                        MCAImpsGroupClient* aClient )
    {
    CCAGroupWrapper* self = new( ELeave ) CCAGroupWrapper( aSettingsAPI, aGroup,
                                                           aRequestMapper, aClient );

    return self;
    }


// Destructor
CCAGroupWrapper::~CCAGroupWrapper()
    {
    // signal observer to keep UI state in sync
    if ( iObserver )
        {
        iObserver->HandleGroupLeft( KErrNone );
        }

    delete iCommonProperties;
    delete iPrivateProperties;

    if ( iOldTopic )
        {
        delete iOldTopic;
        iOldTopic = NULL;
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::Group
// ---------------------------------------------------------
//
MCAStoredGroup& CCAGroupWrapper::Group()
    {
    return iGroup;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::RegisterEventObserverL
// ---------------------------------------------------------
//
void CCAGroupWrapper::RegisterEventObserver( MCAGroupEventObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::LocalPropertiesL
// ---------------------------------------------------------
//
void CCAGroupWrapper::LocalProperties( CImpsCommonGroupProps*& aCommonProps,
                                       CImpsPrivateGroupProps*& aPrivateProps )
    {
    aCommonProps = iCommonProperties;
    aPrivateProps = iPrivateProperties;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::SetLocalPropertiesL
// ---------------------------------------------------------
//
void CCAGroupWrapper::SetLocalProperties( CImpsCommonGroupProps* aCommonProps,
                                          CImpsPrivateGroupProps* aPrivateProps )
    {
    // since we own these, we must get rid of these
    delete iCommonProperties;
    iCommonProperties = aCommonProps;

    delete iPrivateProperties;
    iPrivateProperties = aPrivateProps;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::Requet
// ---------------------------------------------------------
//
CCARequest* CCAGroupWrapper::Request()
    {
    return iRequest;
    }

// ---------------------------------------------------------
// CCAGroupManager::LastImpsError
// ---------------------------------------------------------
//
void CCAGroupWrapper::LastImpsError( TInt aError )
    {
    iLastImpsError = aError;
    }

// ---------------------------------------------------------
// CCAGroupManager::SetTopicShown
// ---------------------------------------------------------
//
void CCAGroupWrapper::SetTopicShown( TBool aIsShown )
    {
    iTopicShown = aIsShown;
    }

// ---------------------------------------------------------
// CCAGroupManager::IsTopicShown
// ---------------------------------------------------------
//
TBool CCAGroupWrapper::IsTopicShown() const
    {
    return iTopicShown;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::OperationId
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::OperationId()
    {
    return iOperationId;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::JoinL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::JoinL( const TDesC& aScreenName, TBool aUsers,
                             TBool aIsFavourite )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::JoinGroupL to group %S" ),
             &iGroup.GroupId() );

    // JOIN PHASE//////////////////////////////////////////////////////////////
    iOperationId = iImpsGroupClient->JoinGroupL( iGroup.GroupId(), aScreenName,
                                                 aUsers, ETrue );


    // We need additional data e.g. group ID and the screen name
    // Granularity of two because there will 2 items
    CDesCArrayFlat* dataArray = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( dataArray );
    dataArray->AppendL( iGroup.GroupId() );
    dataArray->AppendL( aScreenName );

    if ( aIsFavourite )
        {
        iOperationError = ExecuteRequestL( iOperationId, EJoinGroup,
                                           dataArray );
        }
    else
        {
        iOperationError = ExecuteRequestL( iOperationId,
                                           EJoinToNonFavouriteGroup,
                                           dataArray );
        }

    CleanupStack::PopAndDestroy( dataArray );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::CreateGroupL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::CreateGroupL( const TDesC& aScreenName, TBool aJoinGroup )
    {
    CHAT_DP_TXT( "CCAGroupWrapper::CreateGroupL...starts" );

    if ( !iCommonProperties )
        {
        User::Leave( KErrNotReady );
        }

    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( dataArray );

    iOperationId = iImpsGroupClient->CreateGroupL( iGroup.GroupId(),
                                                   iCommonProperties,
                                                   aScreenName,
                                                   aJoinGroup,
                                                   ETrue );

    dataArray->AppendL( iGroup.GroupId() );
    dataArray->AppendL( iCommonProperties->GroupName() );

    if ( aJoinGroup )
        {
        dataArray->AppendL( aScreenName );
        dataArray->AppendL( iCommonProperties->Welcome() );
        dataArray->AppendL( iCommonProperties->Topic() );
        iOperationError = ExecuteRequestL( iOperationId,
                                           ECreateAndJoinGroup,
                                           dataArray );
        }
    else
        {
        iOperationError = ExecuteRequestL( iOperationId,
                                           ECreateGroup,
                                           dataArray );
        }

    CleanupStack::PopAndDestroy( dataArray );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::LeaveL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::LeaveL()
    {
    CHAT_DP_TXT( "CCAGroupWrapper::LeaveGroupL...starts" );

    iOperationId = iImpsGroupClient->LeaveGroupL( iGroup.GroupId() );

    // In client initiated Leave we must store the group id because it
    // doesn't exist in the callback. Adding only 1 --> granularity 1
    CDesCArrayFlat* dataArray = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( dataArray );
    dataArray->AppendL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId, ELeaveGroup, dataArray );

    CleanupStack::PopAndDestroy( dataArray );

    CHAT_DP_TXT( "CCAGroupWrapper::LeaveGroupL...over" );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::DeleteFromNetworkL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::DeleteFromNetworkL()
    {
    CHAT_DP_TXT( "CCAGroupWrapper::DeleteGroupL...starts" );

    iOperationId = iImpsGroupClient->DeleteGroupL( iGroup.GroupId() );

    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( dataArray );
    dataArray->AppendL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId, EDeleteGroup, dataArray );

    CleanupStack::PopAndDestroy( dataArray );

    CHAT_DP_TXT( "CCAGroupWrapper::DeleteGroupL...over" );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::UpdatePropertiesL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::UpdatePropertiesL( CImpsCommonGroupProps* aCommonProps,
                                         CImpsPrivateGroupProps* aPrivateProps )
    {
    CHAT_DP_TXT( "CCAGroupWrapper::UpdatePropertiesL...starts" );

    iOperationId = iImpsGroupClient->SetGroupPropertiesL( iGroup.GroupId(),
                                                          aCommonProps,
                                                          aPrivateProps );

    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( dataArray );
    dataArray->AppendL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId,
                                       ESetGroupProperties, dataArray );

    CleanupStack::PopAndDestroy( dataArray );

    if ( iOperationError == KErrNone || iOperationError == ECSPSuccessful )
        {
        if ( aCommonProps )
            {
            if ( aCommonProps->Topic().Length() > 0 &&
                 iOldTopic != NULL &&
                 0 != aCommonProps->Topic().CompareC( *iOldTopic ) )
                {
                // Set new topic not shown, this happens when user
                // changes topic
                iTopicShown = EFalse;
                }
            }

        // updating was successful, update the local properties
        CopyPropertiesL( aCommonProps, iCommonProperties,
                         aPrivateProps, iPrivateProperties );
        }

    if ( iOldTopic )
        {
        delete iOldTopic;
        iOldTopic = NULL;
        }
    iOldTopic = iCommonProperties->Topic().AllocL();

    CHAT_DP_TXT( "CCAGroupWrapper::UpdatePropertiesL...over" );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::AddMembersL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::AddMembersL( const CDesCArray& aUserList,
                                   const CDesCArray& aScreenNameList )
    {
    CHAT_DP_TXT( "CCAGroupWrapper::AddMembersL...starts" );

    if ( ( aUserList.MdcaCount() == 0 ) &&
         ( aScreenNameList.MdcaCount() == 0 ) )
        {
        // Both lists are empty, it's time to leave
        User::Leave( KErrArgument );
        }

    iOperationId = iImpsGroupClient->AddMembersL( iGroup.GroupId(),
                                                  aUserList, aScreenNameList );

    // We need additional data for group ID
    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( dataArray );
    dataArray->AppendL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId, EAddMembers, dataArray );

    CleanupStack::PopAndDestroy( dataArray );

    CHAT_DP_TXT( "CCAGroupWrapper::AddMembersL...over" );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::RemoveMembersL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::RemoveMembersL( const CDesCArray& aUserList )
    {
    CHAT_DP_TXT( "CCAGroupWrapper::RemoveMembersL" );

    iOperationId =
        iImpsGroupClient->RemoveMembersL( iGroup.GroupId(), aUserList );

    iOperationError = ExecuteRequestL( iOperationId, ERemoveMembers );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::ModifyMembersL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::ModifyMembersL( const CDesCArray* aAdminList,
                                      const CDesCArray* aModerList,
                                      const CDesCArray* aOrdinaryList )
    {
    CHAT_DP_TXT( "CCAGroupWrapper::ModifyMembersAccessL()...starts" );

    iOperationId = iImpsGroupClient->ModifyMembersAccessL(
                       iGroup.GroupId(),
                       aAdminList,
                       aModerList,
                       aOrdinaryList );

    iOperationError = ExecuteRequestL( iOperationId, EModifyMembersAccess );

    CHAT_DP_TXT( "CCAGroupWrapper::ModifyMembersAccessL()...over" );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::GetPropertiesL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::GetPropertiesL( CImpsCommonGroupProps*& aCommonProps,
                                      CImpsPrivateGroupProps*& aPrivateProps )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::PropertiesL to group %S" ),
             &iGroup.GroupId() );

    if ( ( iCommonProperties ) || ( iPrivateProperties ) )
        {
        aCommonProps = iCommonProperties;
        aPrivateProps = iPrivateProperties;
        return KErrNone;
        }

    iOperationId = iImpsGroupClient->GroupPropertiesL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId, EGroupProperties );

    aCommonProps = iCommonProperties;
    aPrivateProps = iPrivateProperties;
// if new operation id is created , then set it to zero.
    iOperationId = KErrNone;
    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::GetMembersL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::GetMembersL( CDesCArray& aUserList,
                                   CDesCArray& aScreenNames,
                                   CDesCArray& aModers,
                                   CDesCArray& aAdmins )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::MembersL to group %S" ),
             &iGroup.GroupId() );

    iMembers = &aUserList;
    iScreenNames = &aScreenNames;
    iModers = &aModers;
    iAdmins = &aAdmins;

    iOperationId = iImpsGroupClient->GroupMembersL( iGroup.GroupId() );

    iOperationError = ExecuteRequestL( iOperationId, EGroupMembers );

    iMembers = NULL;
    iScreenNames = NULL;
    iModers = NULL;
    iAdmins = NULL;

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::GetRejectListL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::GetRejectListL( CDesCArray& aRejectList )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::RejectListL to group %S" ),
             &iGroup.GroupId() );
    iRejectedList = &aRejectList;

    iOperationId = iImpsGroupClient->SetRejectListL( iGroup.GroupId(),
                                                     NULL, NULL );

    iOperationError = ExecuteRequestL( iOperationId, ESetRejectList );

    iRejectedList = NULL;

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::SetRejectListL
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::SetRejectListL( const CDesCArray& aRejectedList,
                                      const CDesCArray& aEnabledList )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::SetRejectListL to group %S" ),
             &iGroup.GroupId() );

    iOperationId = iImpsGroupClient->SetRejectListL( iGroup.GroupId(),
                                                     &aRejectedList, &aEnabledList );

    iOperationError = ExecuteRequestL( iOperationId, ESetRejectList );

    return iOperationError;
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleCompleteL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleCompleteL( TInt aOpId,
                                       TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleCompleteL OpId = %d" ), aOpId );
    // If code added before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    if ( aOpId != 0 )
        {
        iRequestMapper.HandleRequest( aOpId, KErrNone );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleJoinL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleJoinL( TInt aOpId,
                                   const MDesCArray& /* aUserList */,
                                   const MDesCArray& aScreenNames,
                                   const TDesC& /* aWelcomeText */,
                                   TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleJoinL OpId = %d" ), aOpId );
    if ( aOpId != 0 )
        {
        CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::JoinGroupL" ) );

        TInt status( KErrNone );

        TRAPD( err, iGroup.AddParticipantL( aScreenNames ) );

        if ( err )
            {
            status = err;
            }

        iRequestMapper.HandleRequest( aOpId, status );
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleGroupMembersL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleGroupMembersL( TInt aOpId,
                                           const MDesCArray& aUserList,
                                           const MDesCArray& aScreenNames,
                                           const MDesCArray& aModers,
                                           const MDesCArray& aAdmins,
                                           TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleGroupMembersL OpId = %d" ),
             aOpId );

    if ( aOpId != 0 )
        {
        TInt status( KErrNone );
        TRAPD( err,
               CopyArrayL( aUserList, iMembers );
               CopyArrayL( aScreenNames, iScreenNames );
               CopyArrayL( aModers, iModers );
               CopyArrayL( aAdmins, iAdmins );
             )
        if ( err )
            {
            status = err;
            }

        iRequestMapper.HandleRequest( aOpId, status );
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleGroupPropertiesL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleGroupPropertiesL( TInt aOpId,
                                              const TDesC& /*aGroupId*/,
                                              const CImpsCommonGroupProps& aGroupProps,
                                              const CImpsPrivateGroupProps& aOwnProps,
                                              TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleGroupPropertiesL OpId = %d" ),
             aOpId );
    // If code added before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    if ( aOpId != 0 )
        {
        iRequestMapper.HandleRequest( aOpId, KErrNone );
        }

    // Copy relevant data to properties which are
    // stored in wrapper. This can be done always, as the properties contain
    // all fields in every case.
    CopyPropertiesL( &aGroupProps, iCommonProperties, &aOwnProps,
                     iPrivateProperties );

    // push event from the network
    // signal observer
    if ( iObserver )
        {
        iObserver->HandleGroupPropertiesChanged();
        }

    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleRejectListL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleRejectListL( TInt aOpId,
                                         const MDesCArray& aUserList,
                                         TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleRejectListL OpId = %d" ),
             aOpId );

    if ( aOpId != 0 )
        {
        CHAT_DP( D_CHAT_LIT( " --RejectList count %d -- " ),
                 aUserList.MdcaCount() );
        TInt status( KErrNone );
        TRAPD( err, CopyArrayL( aUserList, iRejectedList ) );
        if ( err )
            {
            status = err;
            }
        iRequestMapper.HandleRequest( aOpId, status );
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleSubscriptionL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleSubscriptionL( TInt aOpId, TBool /*aIsSubscribed*/,
                                           TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleSubscriptionL OpId = %d" ),
             aOpId );
    // If code added before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    if ( aOpId != 0 )
        {
        iRequestMapper.HandleRequest( aOpId, KErrNone );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleNewUsersL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleNewUsersL( const TDesC& /* aGroupId */,
                                       const MDesCArray& /* aUserList */,
                                       const MDesCArray& aScreenNames,
                                       TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleNewUsersL" ) );
    // always push event, so no need to stop request
    iGroup.AddParticipantL( aScreenNames );
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleLeftUsersL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleLeftUsersL( const TDesC& /* aGroupId */,
                                        const MDesCArray& aUserList,
                                        const MDesCArray& aScreenNames,
                                        TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleLeftUsersL" ) );

    // always push event, so no need to stop request

    CHAT_DP( D_CHAT_LIT( "aUserListCount: %d aScreenNamesCount %d" ),
             aUserList.MdcaCount(), aScreenNames.MdcaCount() );

    iGroup.RemoveParticipantL( aScreenNames );
    }

// ---------------------------------------------------------
// CCAGroupWrapper::HandleLeaveL
// ---------------------------------------------------------
//
void CCAGroupWrapper::HandleLeaveL( TInt aOpId, const TDesC& /* aGroupId */,
                                    const TDesC& /*aDescription*/,
                                    TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::HandleLeaveL OpId = %d" ), aOpId );

    // If code added before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    if ( aOpId != 0 )
        {
        iRequestMapper.HandleRequest( aOpId, KErrNone );
        }
    else
        {
        // push event from the network
        // signal observer
        if ( iObserver )
            {
            iObserver->HandleGroupLeft( iLastImpsError );
            //consume error
            iLastImpsError = KErrNone;
            }
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::CopyArrayL
// ---------------------------------------------------------
//
void CCAGroupWrapper::CopyArrayL( const MDesCArray& aSource, CDesCArray* aDest )
    {
    if ( !aDest )
        {
        return;
        }

    aDest->Reset();
    TInt count( aSource.MdcaCount() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        TPtrC src( aSource.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyArrayL [%d] %S" ), i, &src );
        aDest->AppendL( src );
        }
    }

// ---------------------------------------------------------
// CCAGroupWrapper::ExecuteRequest
// ---------------------------------------------------------
//
TInt CCAGroupWrapper::ExecuteRequestL( TInt aOperationId,
                                       TOperationRequests aRequestType,
                                       MDesCArray* aAdditionalData )
    {
    // creating request
    iRequest = iRequestMapper.CreateRequestL( aOperationId, EFalse );
    CCARequestMapper& requestMapper = iRequestMapper;
    CCARequest* request = iRequest;

    // setting type
    iRequest->SetRequestType( aRequestType );

    // setting additional data if exists
    if ( aAdditionalData )
        {
        iRequest->SetAdditionalData( *aAdditionalData );
        }

    // Now when the request type and additional data is set we can
    // activate the synchronous wait
    iRequest->StartWait();

    TInt retVal( request->ErrorCode() );
    requestMapper.RemoveRequest( request );
    iRequest = NULL;
    request = NULL;

    return retVal;
    }


// ---------------------------------------------------------
// CCAGroupWrapper::CopyPropertiesL
// ---------------------------------------------------------
//
void CCAGroupWrapper::CopyPropertiesL(
    const CImpsCommonGroupProps* aFromCommonProps,
    CImpsCommonGroupProps* aToCommonProps,
    const CImpsPrivateGroupProps* aFromPrivateProps,
    CImpsPrivateGroupProps* aToPrivateProps
)
    {
    CHAT_DP_FUNC_ENTER( "CopyPropertiesL" );

    if ( aToCommonProps && aFromCommonProps )
        {
        // copy all relevant fields, ignoring empty fields
        if ( aFromCommonProps->GroupName().Length() > 0 )
            {
            aToCommonProps->SetGroupNameL( aFromCommonProps->GroupName() );
            }

        if ( aFromCommonProps->Welcome().Length() > 0 )
            {
            aToCommonProps->SetWelcomeL( aFromCommonProps->Welcome() );
            }

        if ( aFromCommonProps->Topic().Length() > 0 )
            {
            aToCommonProps->SetTopicL( aFromCommonProps->Topic() );
            }

        if ( aFromCommonProps->IsPrivateAllowed() != EImpsPropUndef )
            {
            CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyPropertiesL - \
                                  Whispering: %d" ),
                     aFromCommonProps->IsPrivateAllowed() );
            aToCommonProps->SetPrivateAllowed(
                aFromCommonProps->IsPrivateAllowed() );
            }

        if ( aFromCommonProps->IsSearchable() != EImpsPropUndef )
            {
            CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyPropertiesL - \
                                  Searchable: %d" ),
                     aFromCommonProps->IsSearchable() );
            aToCommonProps->SetSearchable( aFromCommonProps->IsSearchable() );
            }

        if ( aFromCommonProps->MaxNbrOfUsers() >= 0 )
            {
            CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyPropertiesL - \
                                  Max users: %d" ),
                     aFromCommonProps->MaxNbrOfUsers() );
            aToCommonProps->SetMaxNbrOfUsers(
                aFromCommonProps->MaxNbrOfUsers() );
            }

        if ( aFromCommonProps->IsOpen() != EImpsPropUndef )
            {
            CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyPropertiesL - Open: %d" ),
                     aFromCommonProps->IsOpen() );
            aToCommonProps->SetOpen( aFromCommonProps->IsOpen() );
            }
        }

    if ( aToPrivateProps && aToPrivateProps )
        {
        if ( aFromPrivateProps->IsPrivateAllowed() != EImpsPropUndef )
            {
            CHAT_DP( D_CHAT_LIT( "CCAGroupWrapper::CopyPropertiesL - (Private) \
                                  Allowed: %d" ),
                     aFromPrivateProps->IsPrivateAllowed() );
            aToPrivateProps->SetPrivateAllowed(
                aFromPrivateProps->IsPrivateAllowed() );
            }
        }

    CHAT_DP_FUNC_DONE( "CopyPropertiesL" );
    }


//  End of File
