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
* Description:  Implementation for PC Group Interface
*
*/



// INCLUDE FILES
#include	"CCAInvitationRequest.h"
#include	"MCAInvite.h"
#include	"MCAPresence.h"
#include    "MCAStoredContact.h"
#include    "MCAStoredContacts.h"
#include    "CCAStorageManagerFactory.h"
#include    "CCAEngine.h"
#include    "MCAContactListModel.h"
#include    "MCAInviteConflictResolver.h"
#include    "MCAServerContactsArrayPC.h"
#include    "CCAServerContactsArrayPC.h"


#include    "CCAInviteEditorPC.h"

//const definitions



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAInviteEditorPC: Constructor
// -----------------------------------------------------------------------------
//
CCAInviteEditorPC::CCAInviteEditorPC( MCAInvite& aInviteAPI,
                                      MCAPresence& aPresenceAP )
        : iInviteAPI( aInviteAPI ),
        iPresenceAPI( aPresenceAP )
    {
    }


// -----------------------------------------------------------------------------
// CCAInviteEditorPC: NewL
// -----------------------------------------------------------------------------
//
CCAInviteEditorPC* CCAInviteEditorPC::NewL( MCAInvite& aInviteAPI,
                                            MCAPresence& aPresenceAPI )
    {
    CCAInviteEditorPC* self = new ( ELeave ) CCAInviteEditorPC( aInviteAPI,
                                                                aPresenceAPI );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAInviteEditorPC: ConstructL
// -----------------------------------------------------------------------------
//
void CCAInviteEditorPC::ConstructL(  )
    {
    // codescanner warning can be ignored
    }


// -----------------------------------------------------------------------------
// CCAInviteEditorPC: Destructor
// -----------------------------------------------------------------------------
//
CCAInviteEditorPC::~CCAInviteEditorPC()
    {

    }



// -----------------------------------------------------------------------------
// CCAInviteEditorPC: SendInvitationL
// -----------------------------------------------------------------------------
//
TInt CCAInviteEditorPC::SendInvitationL( const CDesCArray& aUserIDs,
                                         const TDesC& aGroupID,
                                         const TDesC& aMessage )
    {
    // Create invitation
    TInt retVal = KErrNone;
    CCAInvitationRequest* invitation = CCAInvitationRequest::NewL( aUserIDs,
                                                                   aGroupID,
                                                                   aMessage );

    CleanupStack::PushL( invitation );

    iInviteAPI.SendInvitationL( *invitation );

    CleanupStack::PopAndDestroy( invitation );

    return retVal;
    }

// -----------------------------------------------------------------------------
// CCAInviteEditorPC: SendInvitationL
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC*  CCAInviteEditorPC::GetOnlineFriendsLC( const TDesC& aIconTabulator )
    {
    CDesCArray* onlineList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( onlineList );


    CDesCArray* onlineIdentificationList =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( onlineIdentificationList );

    CDesCArray* onlineNicknameList =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( onlineNicknameList );

    iPresenceAPI.GetOnlineFriendsL( *onlineList );

    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    // getting nicnames for contact id's
    TInt onlineListCount( onlineList->MdcaCount() );
    for ( TInt i( 0 ); i < onlineListCount; i++ )
        {
        onlineIdentificationList->AppendL( contacts->Identification(
                                               onlineList->MdcaPoint( i ) ) );
        MCAStoredContact* contact = contacts->FindAnyContact( onlineList->MdcaPoint( i ) );
        if ( contact )
            {
            onlineNicknameList->AppendL( contact->Nickname() );
            }
        else
            {
            onlineNicknameList->AppendL( KNullDesC );
            }
        }

    // servercontactsarray takes the ownership
    CCAServerContactsArrayPC* pairsArray =
        CCAServerContactsArrayPC::NewL( onlineNicknameList,
                                        onlineList,
                                        aIconTabulator,
                                        onlineIdentificationList );
    //ownership is transfered to the caller.
    CleanupStack::Pop( onlineNicknameList );
    CleanupStack::Pop( onlineIdentificationList );
    CleanupStack::Pop( onlineList );
    CleanupStack::PushL( pairsArray );
    return pairsArray;
    }

// -----------------------------------------------------------------------------
// CCAInviteEditorPC: PopulateInviteListL
// -----------------------------------------------------------------------------
//
void CCAInviteEditorPC::PopulateInviteListL( CDesCArray& aInviteList,
                                             const MCAServerContactsArrayPC& aRawOnlineList,
                                             const CArrayFixFlat<TInt>& aSelectionArray )
    {
    TInt inviteCount( aSelectionArray.Count() );
    CCAServerContactsArrayPC& rawOnlineList = dynamic_cast<CCAServerContactsArrayPC&>(
                                                  const_cast<MCAServerContactsArrayPC&>( aRawOnlineList ) );
    for ( TInt ii( 0 ); ii < inviteCount; ++ii )
        {
        TInt index ( aSelectionArray.At( ii ) );
        if ( index >= 0 && index < rawOnlineList.MdcaCount() )
            {
            aInviteList.AppendL( rawOnlineList[ index ].iWVID );
            }
        }
    }


//End of file
