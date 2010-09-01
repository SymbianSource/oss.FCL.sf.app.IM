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
* Description:  Handles incoming invitations
*
*/


// INCLUDE FILES
#include "PrivateEngineDefinitions.h"
#include "PublicEngineDefinitions.h"
#include "CCAInviteManager.h"                   // Own header
#include "MCAInviteObserver.h"                  // Observer interface
#include "MCASettings.h"                        // Internal settings
#include "ChatDebugPrint.h"                     // For debug macros
#include "CCARequestMapper.h"
#include "CCAInviteTracker.h"
#include "CCAStoragemanagerFactory.h"
#include "CCAInvitation.h"

#include "MCAStoredContacts.h"
#include "MCAStoredGroup.h"
#include "MCAStoredGroups.h"
#include "MCAGroupManagerInterface.h"
#include "MCAExtendedStoredGroup.h"
#include "MCAGroupOperations.h"

#include "MCAImpsFactory.h"

#include "ChatDebugAssert.h"

#include <e32base.h>

// CONSTANTS
const TInt KMaxIDNumberLength = 5; //also used in ChatDefinitions.
//both must be synchronous

_LIT( KInviteFormatString, "%d" );

// ================= MEMBER FUNCTIONS =======================

// Symbian OS default constructor can leave.
void CCAInviteManager::ConstructL()
    {
    iRejectReason = HBufC::NewL( KMaxRejectReasonLength );
    iImpsFundAPI = iImpsFactory->CreateFundClientL();
    }

// Two-phased constructor.
CCAInviteManager* CCAInviteManager::NewL(
    MCAImpsFactory* aIMPSFactory,
    MCASettings& aSettingsAPI,
    CCARequestMapper& aRequestMapper,
    MCAGroupManagerInterface& aGroupManager )
    {
    CCAInviteManager* self = new ( ELeave ) CCAInviteManager(
        aIMPSFactory,
        aSettingsAPI,
        aRequestMapper,
        aGroupManager );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCAInviteManager::~CCAInviteManager()
    {
    iInviteTrackerQueue.ResetAndDestroy();
    iInviteTrackerQueue.Close();

    iInviteObservers.Close();
    delete iLastInviteId;
    delete iRejectReason;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAInviteManager::CCAInviteManager(
    MCAImpsFactory* aIMPSFactory,
    MCASettings& aSettingsAPI,
    CCARequestMapper& aRequestMapper,
    MCAGroupManagerInterface& aGroupManager ) :
        iRequestMapper( &aRequestMapper ),
        iSettingsAPI( aSettingsAPI ),
        iImpsFactory( aIMPSFactory ),
        iGroupManager( &aGroupManager )
    {
    }

// ---------------------------------------------------------
// CCAInviteManager::InviteExpired
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::InviteExpiredL( const MCAInvitation* aInvitation )
    {
    CHAT_DP_TXT( "CCAInviteManager::InviteExpiredL" );

    // signal observers
    TInt count( iInviteObservers.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iInviteObservers[ i ] )
            {
            // it's a non-NULL
            iInviteObservers[ i ]->HandleInvitationEventL(
                MCAInviteObserver::EInviteExpired, aInvitation );
            }
        }

    RemoveInviteL( aInvitation );
    }


// ---------------------------------------------------------
// CCAInviteManager::AddInviteObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInviteManager::AddInviteObserver( MCAInviteObserver* aObserver )
    {
    return iInviteObservers.Append( aObserver );
    }


// ---------------------------------------------------------
// CCAInviteManager::RemoveInviteObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::RemoveInviteObserver( MCAInviteObserver* aObserver )
    {
    TInt pos( iInviteObservers.Find( aObserver ) );
    if ( pos != KErrNotFound )
        {
        iInviteObservers.Remove( pos );
        iInviteObservers.Compress();
        }
    }


// ---------------------------------------------------------
// CCAInviteManager::SendInvitationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::SendInvitationL(
    const CCAInvitationRequest& aInvitation )
    {
    CHAT_DP( D_CHAT_LIT( "Sending invite to group %S" ), &aInvitation.GroupID() );

    PrepareGroupForInviteeL( aInvitation );

    HBufC* loggedUser = iSettingsAPI.ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( loggedUser );

    HBufC* idTemplate = loggedUser->ReAllocL( loggedUser->Length() +
                                              KMaxIDNumberLength );
    CleanupStack::Pop( loggedUser );

    TPtr idPtr( idTemplate->Des() );
    idPtr.Append( KInviteFormatString );

    CleanupStack::PushL( idTemplate );

    iLastInviteId = HBufC::NewL( idTemplate->Length() );
    TPtr generatedIDPtr( iLastInviteId->Des() );
    generatedIDPtr.Format( *idTemplate, iInviteIDOrdinal++ );

    iCurrentInviteRequest = &aInvitation;

    CHAT_DP( D_CHAT_LIT( "Sending invite: id=%S" ), iLastInviteId );

    iCurrentOpId = iImpsFundAPI->InviteGroupRequestL(
                       *iLastInviteId,             // Invite ID
                       &aInvitation.Invitees(),    // Invited user IDs
                       NULL,                       // Screen names
                       NULL,                       // Group names
                       aInvitation.GroupID(),      // Group
                       KNullDesC,                  // Own screenname
                       KNullDesC,                  // Owns groupname
                       aInvitation.Message(),      // Invite message
                       aInvitation.Timeout() );    // Validity period

    CCARequest* request = iRequestMapper->CreateRequestL( iCurrentOpId );   // CSI: 35 # Ownership is not transferred to caller.

    delete iLastInviteId;
    iLastInviteId = NULL;
    iCurrentInviteRequest = NULL;

    iRequestMapper->RemoveRequestAndLeaveIfErrorL( request );

    CleanupStack::PopAndDestroy( idTemplate );
    }


// ---------------------------------------------------------
// CCAInviteManager::PopulateInviteList
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::PopulateInviteList(
    RPointerArray< MCAInvitation >& aList )
    {
    aList.Reset();
    TInt count( iInviteTrackerQueue.Count() - 1 );
    // populate the list backwards so that the most recently received
    // invitation gets the first slot in the list
    for ( ; count >= 0; count-- )
        {
        // the error codes can be ignored, because it's not so fatal
        // if one invitation is missing from the list.
        // The other methods in UI will fail in any case.
        aList.Append( iInviteTrackerQueue[ count ]->Invitation() );
        }
    }


// ---------------------------------------------------------
// CCAInviteManager::ActiveInvitations
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInviteManager::ActiveInvitations()
    {
    return iInviteTrackerQueue.Count();
    }


// ---------------------------------------------------------
// CCAInviteManager::RemoveInvitation
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::RemoveInvitationL( MCAInvitation* aInvitation )
    {
    RemoveInviteL( aInvitation );
    }


// ---------------------------------------------------------
// CCAInviteManager::RejectReasonPtr
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAInviteManager::RejectReasonPtr()
    {
    return iRejectReason;
    }

// ---------------------------------------------------------
// CCAInviteManager::UnreadInvitesCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInviteManager::UnreadInvitesCount() const
    {
    TInt inviteCount( iInviteTrackerQueue.Count() );
    TInt unreadCount( 0 );
    for ( TInt i( 0 ); i < inviteCount; i++ )
        {
        if ( !iInviteTrackerQueue[ i ]->Invitation()->IsRead() )
            {
            ++unreadCount;
            }
        }
    return unreadCount;
    }

// ---------------------------------------------------------
// CCAInviteManager::HandleGroupInviteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::HandleGroupInviteL( const TDesC& aInviteID,
                                           const TDesC& aUserID,
                                           const TDesC& aScreenName,
                                           const TDesC& aGroupName,
                                           const TDesC& aInviteReason,
                                           const TInt aValidityPeriod,
                                           TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP_TXT( "Received new invitation!" );
    CHAT_DP( D_CHAT_LIT( " * %S invites us to group: %S" ),
             &aUserID, &aGroupName );

    MCAStoredContacts* contactList = NULL;

    TRAPD( err, contactList =
               CCAStorageManagerFactory::ContactListInterfaceL() );
    if ( ( err != KErrNone ) || ( ! contactList ) )
        {
        // we can't leave, but we can return
        CHAT_DP_TXT( "CCAInviteManager::GroupInviteUserRequest - Couldn't \
                      get contactlist interface" );
        return;
        }

    // check if the invitation should be blocked
    TInt receiveInvitations( iSettingsAPI.Value(
                                 MCASettings::EReceiveInvitations ) );
    if ( receiveInvitations == MCASettings::ENobody )
        {
        // ignore invites from everyone
        CHAT_DP_TXT( " * invitations blocked from everyone" );
        return;
        }
    else if ( receiveInvitations == MCASettings::EFriends &&
              ( ! contactList->FindAnyContact( aUserID ) ) )
        {
        // invite sender not found from friends list, ignore invite
        CHAT_DP_TXT( " * invitations blocked from everyone except friends" );
        return;
        }

    // ok, the invitation should be processed
    MCAInvitation* invitation = NULL;
    err = KErrNone;
    TRAP( err, invitation = CreateInviteRequestL( aInviteID,
                                                  aUserID,
                                                  aScreenName,
                                                  aGroupName,
                                                  aInviteReason,
                                                  aValidityPeriod ) );

    if ( err != KErrNone )
        {
        return;
        }

    CHAT_DP_TXT( " * invitation created" );

    // create a invitetracker that takes care of expiring the invite
    CCAInviteTracker* trackedInvite = NULL;
    err = KErrNone;
    TRAP( err, trackedInvite = CCAInviteTracker::NewL( this, invitation ) );

    if ( err != KErrNone )
        {
        delete invitation;
        return;
        }

    // append the invite to the list
    if ( iInviteTrackerQueue.Append( trackedInvite ) != KErrNone )
        {
        delete trackedInvite;
        return;
        }

    CHAT_DP_TXT( " * signalling observers" );
    // signal observers
    TInt count( iInviteObservers.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iInviteObservers[ i ] )
            {
            // it's non-NULL
            iInviteObservers[ i ]->HandleInvitationEventL(
                MCAInviteObserver::ENewInvite, invitation );
            }
        }

    }

// ---------------------------------------------------------
// CCAInviteManager::ReplyInvitationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::HandleImInviteL( const TDesC& /* aInviteID */,
                                        const TDesC& /* aUserID */,
                                        const TDesC& /* aInviteReason */,
                                        const TInt /* aValidityPeriod */,
                                        TImpsCspIdentifier& /* aCspId */ )
    {
    __CHAT_ASSERT_DEBUG( EFalse );
    }

// ---------------------------------------------------------
// CCAInviteManager::ReplyInvitationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::HandleContentInviteL( const TDesC& /* aInviteID */,
                                             const TDesC& /* aUserID */,
                                             MDesCArray* /* aUrlList */,
                                             const TDesC& /* aInviteReason */,
                                             const TInt /* aValidityPeriod */,
                                             TImpsCspIdentifier& /* aCspId */ )
    {
    __CHAT_ASSERT_DEBUG( EFalse );
    }

// ---------------------------------------------------------
// CCAInviteManager::ReplyInvitationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::ReplyInvitationL( const MCAInvitation* aInvitation,
                                         const TBool aInviteAccepted,
                                         const TDesC& aResponse,
                                         const TDesC& aScreenName
                                       )
    {
    CHAT_DP_TXT( "Reply to invite" );

    CCARequest* request = iRequestMapper->CreateRequestL(
                              iImpsFundAPI->GroupInviteUserResponseL( aInvitation->InviteID(),
                                                                      aInviteAccepted,
                                                                      aResponse,
                                                                      aScreenName,
                                                                      aInvitation->GroupId() ) );

    iRequestMapper->RemoveRequest( request );

    RemoveInviteL( aInvitation );
    }


// ---------------------------------------------------------
// CCAInviteManager::HandleInviteResponseL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::HandleInviteResponseL( const TDesC& /* aInviteID */,
                                              const TBool aAcceptance,
                                              const TDesC& aUserID,
                                              const TDesC& aScreenName,
                                              const TDesC& aGroupName,
                                              const TDesC& aResponse,
                                              TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP_TXT( "Received response to our invitation" );
    if ( aAcceptance )
        {
        CHAT_DP( D_CHAT_LIT( "%S accepts our invite to %S with comment %S" ),
                 &aScreenName, &aGroupName, &aResponse );
        }
    else
        {
        CHAT_DP( D_CHAT_LIT( "%S rejects our invitation to %S with comment %S" ),
                 &aScreenName, &aGroupName, &aResponse );
        }

    // identification for user can be in aScreenName or in aUserId
    TPtrC user( aUserID );
    if ( user.Length() == 0 )
        {
        user.Set( aScreenName );
        }

    // signal the observers
    TInt count( iInviteObservers.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iInviteObservers[ i ] )
            {
            // it's non-NULL
            iInviteObservers[ i ]->HandleInvitationResponse( aAcceptance, user,
                                                             aGroupName, aResponse );
            }
        }
    }


// ---------------------------------------------------------
// CCAInviteManager::CancelGroupInviteUserRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::HandleInviteCancelL( const TDesC& aInviteID,
                                            const TDesC& /* aUserID */,
                                            const TDesC& /* aScreenName */,
                                            const TDesC& /* aGroupName */,
                                            const TDesC& /* aResponse */,
                                            TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP_TXT( "Received cancelled invitation signal" );

    MCAInvitation* invite = FindInvite( aInviteID );
    if ( invite )
        {
        // signal the observers
        TInt count( iInviteObservers.Count() );
        for ( TInt i( 0 ); i < count; i++ )
            {
            if ( iInviteObservers[ i ] )
                {
                // it's non-NULL
                iInviteObservers[ i ]->HandleInvitationEventL(
                    MCAInviteObserver::EInviteCancel, invite );
                }
            }

        // Failed remove can be safely ignored.
        RemoveInviteL( invite );
        }
    }


// ---------------------------------------------------------
// CCAInviteManager::HandleCompleteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CCAInviteManager::HandleCompleteL( TInt aOperationId,
                                         TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP_TXT( "Received invitation's HandleCompleteL" );
    // If code added before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    iRequestMapper->HandleRequest( aOperationId, KErrNone );
    }

void CCAInviteManager::HandleInviteReadL() const
    {
    // signal the observers
    TInt count( iInviteObservers.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iInviteObservers[ i ] )
            {
            // it's non-NULL
            iInviteObservers[ i ]->HandleInvitationEventL(
                MCAInviteObserver::EInviteRead, NULL );
            }
        }
    }


// ---------------------------------------------------------
// CCAInviteManager::PrepareGroupForInviteeL( const CCAInvitation& aInvitation )
// Checks if group is closed. If it's closed, then check if invitee is in
// access-list. If not, then check is the logged user admin in group.
// If so, then try to add invitee to access list.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInviteManager::PrepareGroupForInviteeL(
    const CCAInvitationRequest& aInvitation )
    {
    const TDesC& groupID = aInvitation.GroupID();

    // Check if group is open or not
    if ( iGroupManager->IsGroupOpenL( groupID ) )
        {
        CHAT_DP_TXT( "PrepareGroupForInviteeL, Group is open for everyone" );
        return;
        }

    CHAT_DP_TXT( "PrepareGroupForInviteeL, Group closed, fetching access lists" );

    // Check if invitees are in access lists
    CDesCArray* failedList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( failedList );

    TBool adminStatus =
        iGroupManager->IsAllowedAccessL( groupID, aInvitation.Invitees(),
                                         *failedList );

    // Try to add not found users to access list as normal users
    if ( failedList->Count() > 0 )
        {
        // there is something in the list

        if ( !adminStatus )
            {
            User::Leave( EUserNotAdmin );
            }

        CHAT_DP_TXT( "PrepareGroupForInviteeL, Trying to add new users to \
                      access list as normal users" );

        // adds to network. we don't know screen names
        MCAGroupOperations* grOp = iGroupManager->GroupOperationsL( groupID );
        TInt err( grOp->AddMembersL( *failedList, *failedList ) );
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( failedList );
    }



// ---------------------------------------------------------
// CCAInviteManager::RemoveInviteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAInviteManager::RemoveInviteL( const MCAInvitation* aInvitation )
    {
    TInt inviteCount( iInviteTrackerQueue.Count() );
    for ( TInt i( 0 ); i < inviteCount; i++ )
        {
        if ( iInviteTrackerQueue[ i ]->Invitation() == aInvitation )
            {
            delete iInviteTrackerQueue[ i ];
            iInviteTrackerQueue.Remove( i );
            iInviteTrackerQueue.Compress();

            // Signal the observers
            TInt count( iInviteObservers.Count() );
            for ( TInt i( 0 ); i < count; i++ )
                {
                if ( iInviteObservers[ i ] )
                    {
                    // Using read event to cause unread message indicators
                    // to be updated
                    iInviteObservers[ i ]->HandleInvitationEventL(
                        MCAInviteObserver::EInviteRead, NULL );
                    }
                }

            return ETrue;
            }
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CCAInviteManager::FindInvite
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAInvitation* CCAInviteManager::FindInvite( const TDesC& aInviteId )
    {
    TInt inviteCount( iInviteTrackerQueue.Count() );
    for ( TInt i( 0 ); i < inviteCount; i++ )
        {
        if ( iInviteTrackerQueue[ i ]->Invitation()->InviteID().CompareC(
                 aInviteId, KCollationLevel, NULL ) == 0 )
            {
            return iInviteTrackerQueue[ i ]->Invitation();
            }
        }
    return NULL;
    }


// ---------------------------------------------------------
// CCAInviteManager::CreateInviteRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAInvitation* CCAInviteManager::CreateInviteRequestL( const TDesC& aInviteID,
                                                       const TDesC& aUserID,
                                                       const TDesC& aScreenName,
                                                       const TDesC& aGroupID,
                                                       const TDesC& aInviteReason,
                                                       const TInt aValidityPeriod )
    {
    TPtrC groupName( aGroupID );
    MCAStoredGroups* groups = CCAStorageManagerFactory::GroupListInterfaceL();
    MCAStoredGroup* group = groups->FindGroup( aGroupID );
    if ( group )
        {
        groupName.Set( group->GroupName() );
        }

    CCAInvitation* invitation = CCAInvitation::NewL( aInviteID,
                                                     aUserID,
                                                     aGroupID,
                                                     aScreenName,
                                                     groupName,
                                                     aInviteReason,
                                                     aValidityPeriod );

    invitation->AddReadObserver( this );

    return invitation;
    }

//  End of File
