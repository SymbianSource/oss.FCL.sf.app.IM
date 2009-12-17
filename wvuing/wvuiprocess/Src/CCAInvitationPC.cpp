/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Singleton base class
*
*/



// INCLUDE FILES
#include    "CCAInvitationPC.h"
#include    "MCAInvitationObserverPC.h"

#include 	"MCAMessage.h"
#include 	"CCAStorageManagerFactory.h"
#include    "MCAStoredContact.h"
#include    "MCAStoredContacts.h"
#include 	"MCAInvite.h"

#include 	"MCAMainViewArrayPC.h"
#include 	"MCAInvitation.h"

#include 	"ChatDebugAssert.h"
#include 	"ChatDebugPrint.h"


#include    "MCAChatInterface.h"
#include 	"MCAProcessManager.h"
#include 	"CCAProcessManagerFactory.h"
#include    "CAUtils.h"
#include 	"CCAEngine.h"
#include    "CCALoginPC.h"
#include    <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAInvitationPC::CCAInvitationPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInvitationPC::CCAInvitationPC( CCAEngine& aEngine, MCAChatInterface& aChatInterface )
        : iEngine( aEngine ),
        iChatInterface( aChatInterface ),
        iInviteInterface( *iEngine.GetInvitationInterface() )
    {
    }


// -----------------------------------------------------------------------------
// CCAInvitationPC::CCAInvitationPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInvitationPC* CCAInvitationPC::NewL( CCAEngine& aEngine, MCAChatInterface& aChatInterface )
    {
    CCAInvitationPC* self = new ( ELeave ) CCAInvitationPC( aEngine, aChatInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC::~CCAInvitationPC
// Destructor
// -----------------------------------------------------------------------------
//
CCAInvitationPC::~CCAInvitationPC()
    {

    iInvitationArray.Reset();
    iInvitationArray.Close();
    iEngine.GetInvitationInterface()->RemoveInviteObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::ConstructL( )
    {
    // codescanner warning can be ignored
    iInviteInterface.PopulateInviteList( iInvitationArray );
    iCurrentActiveInvitation = 0;
    iEngine.GetInvitationInterface()->AddInviteObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC::RegisterObserver
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::RegisterObserver( MCAInvitationObserverPC* aObserver )
    {
    ASSERT( aObserver );

    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC::UnRegisterObserver
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::UnRegisterObserver()
    {

    iObserver = NULL;
    }



// ---------------------------------------------------------
// CCAInvitationPC::InvitationUserIDL
// ---------------------------------------------------------
//
const TDesC& CCAInvitationPC::InvitationUserIDL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return KNullDesC();
        }

    MCAInvitation* inv = iInvitationArray[ aIndex ];
    return inv->UserID();
    }

// ---------------------------------------------------------
// CCAInvitationPC::InvUserIdentificationL
// ---------------------------------------------------------
//
TPtrC CCAInvitationPC::InvUserIdentificationL( TInt aIndex )
    {

    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return KNullDesC();
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    return contacts->Identification( inv->UserID() );
    }

// ---------------------------------------------------------
// CCAInvitationPC::ReceivedAt
// ---------------------------------------------------------
//
TTime CCAInvitationPC::ReceivedAt( TInt aIndex )
    {
    TTime receviedat( 0 );
    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return receviedat;
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    receviedat = inv->ReceivedAt();
    return receviedat;
    }

// ---------------------------------------------------------
// CCAInvitationPC::Message
// ---------------------------------------------------------
//
const TDesC& CCAInvitationPC::Message( TInt aIndex )
    {

    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return KNullDesC;
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    return inv->Message();
    }

// ---------------------------------------------------------
// CCAInvitationPC::ReceivedAt
// ---------------------------------------------------------
//
const TDesC& CCAInvitationPC::GroupName( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return KNullDesC;
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    return inv->GroupId();
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC: SetActiveInvitation
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::SetActiveInvitation( const TInt aIndex )
    {
    iCurrentActiveInvitation = aIndex;

    }

// ---------------------------------------------------------
// CCAPrivateChatListContainer::HandleInvitation()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationPC::HandleInvitationEventL(
    TInviteEventType aEvent, const MCAInvitation* aInvitation  )
    {
    TInt count = iInvitationArray.Count();
    switch ( aEvent )
        {
        case ENewInvite:
            {
            iInvitationArray.Append( aInvitation );

            if ( iObserver )
                {
                iObserver->HandleInvitationEventL( TEnumsPC::ENewInvite, count );
                }
            break;
            }
        case EInviteCancel: // Flowthrough
        case EInviteExpired:
            {
            TInt inviteIndex = FindIndexForInvitation( aInvitation );
            if ( inviteIndex != KErrNotFound )
                {
                Delete( inviteIndex );
                }
            break;
            }

        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAInvitationPC::HandleInvitationResponse()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationPC::HandleInvitationResponse(
    TBool /*aAcceptance*/,
    const TDesC& /*aUserId*/,
    const TDesC& /*aGroupName*/,
    const TDesC& /*aResponse*/ )
    {
    // nothing to do here since CCAppUi shows the dialog
    }


// ---------------------------------------------------------
// CCAInvitationPC::GetActiveInvitationIndex()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationPC::GetActiveInvitationIndex()
    {
    return iCurrentActiveInvitation;
    }

// ---------------------------------------------------------
// CCAInvitationPC::IsInvitationUnread()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAInvitationPC::IsInvitationUnread( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return KErrGeneral;
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    return inv->IsRead();
    }

// ---------------------------------------------------------
// CCAInvitationPC::SetInvitationAsReadL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationPC::SetInvitationAsReadL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return;
        }
    MCAInvitation* inv = iInvitationArray[ aIndex ];
    inv->SetInvitationAsReadL();
    }

// ---------------------------------------------------------
// CCAInvitationPC::GetInvitationIndex()
// Resolves the tabindex to invitationarrayindex.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationPC::GetInvitationIndex( TInt aTabIndex )
    {
    TInt count = InvitationCount();
    TInt invitationIndex = ( count - 1 ) - aTabIndex;
    return invitationIndex;
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC: InvitationCount
// -----------------------------------------------------------------------------
//
TInt CCAInvitationPC::InvitationCount( )
    {
    return iInvitationArray.Count();
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC: InvitationCount
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::DeleteInvitationL( TInt aInvitationIndex )
    {
    if ( aInvitationIndex < 0 )   // negative indices shouldn't exist
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aInvitationIndex );
        return;
        }

    MCAInvitation* inv = iInvitationArray[  aInvitationIndex ];
    MCAMainViewArrayPC* mainViewArrayPC = CCAProcessManagerFactory::InstanceL()->
                                          GetProcessManager()->GetArrayInterface();

    mainViewArrayPC->FindInvitationAndDelete( inv->InviteID() );

    iInviteInterface.RemoveInvitationL( inv );
    iInvitationArray.Remove( aInvitationIndex );

    }

// -----------------------------------------------------------------------------
// CCAInvitationPC: RejectInvitationL()
// -----------------------------------------------------------------------------
//
void CCAInvitationPC::ReplyInvitationL( const TInt aInvitationIndex,
                                        const TBool aInviteAccepted,
                                        const TDesC& aResponse,
                                        const TDesC& aScreenName )
    {
    if ( aInvitationIndex < 0 || aInvitationIndex >= iInvitationArray.Count() )
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aInvitationIndex );
        return;
        }
    MCAInvitation* inv = iInvitationArray[  aInvitationIndex ];
    MCAMainViewArrayPC* mainViewArrayPC = CCAProcessManagerFactory::InstanceL()->
                                          GetProcessManager()->GetArrayInterface();
    mainViewArrayPC->FindInvitationAndDelete( inv->InviteID() );

    iInviteInterface.ReplyInvitationL( inv, aInviteAccepted, aResponse, aScreenName );
    iInvitationArray.Remove( aInvitationIndex );

    }

// -----------------------------------------------------------------------------
// CCAInvitationPC: RejectReason()
// -----------------------------------------------------------------------------
//
HBufC* CCAInvitationPC::RejectReason()
    {
    return iInviteInterface.RejectReasonPtr();
    }

// -----------------------------------------------------------------------------
// CCAInvitationPC::FindIndexForInvitation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAInvitationPC::FindIndexForInvitation(
    const MCAInvitation* aInvitation ) const
    {
    return iInvitationArray.Find( aInvitation );
    }

// ---------------------------------------------------------
// CCAInvitationPC::Delete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationPC::Delete( TInt aIndex )
    {
    if ( aIndex < 0 )   // negative indices shouldn't exist
        {
        CHAT_DP( D_CHAT_LIT( "!!! check your logic, trying to delete at %d !!!" ),
                 aIndex );
        return;
        }
    else if ( aIndex < iInvitationArray.Count() )
        {
        iInvitationArray.Remove( aIndex );
        }
    }

// ---------------------------------------------------------
// CCAInvitationPC::DeleteSingleListViewInvitationL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationPC::DeleteSingleListViewInvitationL( const TDesC& aInviteID /*TInt aIndex*/ )
    {
    for ( TInt index = 0; index < iInvitationArray.Count();index++ )
        {
        MCAInvitation* inviteItem =
            dynamic_cast<MCAInvitation*>( iInvitationArray[ index ] );

        if ( CAUtils::NeutralCompare( aInviteID, inviteItem->InviteID() ) == 0 )
            {
            iInviteInterface.RemoveInvitationL( inviteItem );
            iInvitationArray.Remove( index );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAInvitationPC::FindInvitationIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationPC::FindInvitationIndex( const TDesC& aWvId, TTime aInviteTime )
    {
    for ( TInt index = 0; index < iInvitationArray.Count(); index++ )
        {
        MCAInvitation* inv = iInvitationArray[ index ];

        if ( ( CAUtils::NeutralCompare( aWvId, inv->UserID() ) == 0 ) &&
             ( inv->ReceivedAt() == aInviteTime ) )
            {
            return 	index;
            }
        }

    return -1;

    }


// ---------------------------------------------------------
// CCAInvitationPC::FindInvitationIndex
// Based on invite id. The ablove method based on user id and
// time will be removed.
// ---------------------------------------------------------
//
TInt CCAInvitationPC::FindInvitationIndex( const TDesC& aInviteId )
    {
    for ( TInt index = 0; index < iInvitationArray.Count(); index++ )
        {
        MCAInvitation* inv = iInvitationArray[ index ];

        if (  CAUtils::NeutralCompare( aInviteId, inv->InviteID() ) == 0  )
            {
            return 	index;
            }
        }

    return -1;

    }
// ---------------------------------------------------------
// CCAInvitationPC::UnreadInvitesCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationPC::UnreadInvitesCount() const
    {
    return iEngine.GetInvitationInterface()->UnreadInvitesCount();
    }

// ---------------------------------------------------------
// CCAInvitationPC::UnreadInvitesCount
// (other items were commented in a header).
// ---------------------------------------------------------

const TDesC& CCAInvitationPC::UnresdInvitationInfo()
    {
    for ( TInt index = 0; index < iInvitationArray.Count(); index++ )
        {
        MCAInvitation* inv = iInvitationArray[ index ];
        if ( ! inv->IsRead() )
            {
            return inv->InviteID();
            }
        }

    return KNullDesC();
    }

// ---------------------------------------------------------
// CCAInvitationPC::UnreadInvitesCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCAInvitationPC::GetInviteID( TInt aInvitationIndex )
    {
    for ( TInt index = 0; index < iInvitationArray.Count(); index++ )
        {
        if ( index == aInvitationIndex )
            {
            MCAInvitation* inv = iInvitationArray[ index ];
            return inv->InviteID();
            }
        }

    return KNullDesC();
    }



//End Of File
