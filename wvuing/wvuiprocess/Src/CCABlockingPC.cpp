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
* Description:  Process Component for Login,Logout
*
*/



// INCLUDE FILES
#include 	"CCAEngine.h"
#include 	"PublicEngineDefinitions.h"
#include 	"ImpsCSPAllErrors.h"

#include    "MCABlocking.h"
#include    "MCAStoredContact.h"
#include    "MCAStoredContacts.h"
#include    "CCAStorageManagerFactory.h"
#include	"MCABackgroundInterface.h"

#include    "MCAContactListModel.h"
#include    "MCAContactList.h"
#include    "MCAContactLists.h"

#include    "CAPresenceManager.h"

#include 	"MCABackgroundObserver.h"
#include 	"MCABlockingPC.h"
#include 	"MCAServerContactsArrayPC.h"
#include 	"CCAServerContactsArrayPC.h"

#include    "CAArrayUtils.h"
#include 	"CCABlockingPC.h"
#include 	"MCABlockingBackgroundObserverPC.h"





// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCABlockingPC::NewL
// -----------------------------------------------------------------------------
//
CCABlockingPC* CCABlockingPC::NewL( CCAEngine& aEngine )
    {

    CCABlockingPC* self = new ( ELeave ) CCABlockingPC( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCABlockingPC::~CCABlockingPC()
    {
    delete iBlockingArray;
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::CCABlockingPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCABlockingPC::CCABlockingPC( CCAEngine& aEngine )
        : iEngine( aEngine ),
        iBlockingInterface( *iEngine.GetBlockingInterface() ),
        iBackgroundInterface( *iEngine.BackgroundInterface() )

    {

    }

// -----------------------------------------------------------------------------
// CCABlockingPC::ConstructL
// -----------------------------------------------------------------------------
//

void CCABlockingPC::ConstructL()
    {
    // codescanner warning can be ignored
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetBackgroundStatus
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::GetBackgroundStatus()
    {
    return ( iBackgroundInterface.BackgroundTaskStatus(
                 MCABackgroundInterface::EBlockGrantListFetch ) );
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetBlockedListFromServerL
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::GetBlockedListFromServerL()
    {
    return iBlockingInterface.GetBlockedListFromServerL();
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetBlockedList
// -----------------------------------------------------------------------------
//
const CDesCArray*	CCABlockingPC::GetBlockedList()
    {
    const CDesCArray* blockedList = iBlockingInterface.BlockedList();
    return blockedList;
    }
// -----------------------------------------------------------------------------
// CCABlockingPC::RemoveL
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::RemoveL( const CDesCArray* aUnBlockList,
                             const CDesCArray* aGrantList  )
    {
    if ( aUnBlockList )
        {
        return iBlockingInterface.RemoveL( aUnBlockList, aGrantList );
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::InsertL
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::InsertL( const CDesCArray* aUnBlockList,
                             const CDesCArray* /*aGrantList =NULL*/ )
    {
    if ( aUnBlockList )
        {
        return iBlockingInterface.InsertL( aUnBlockList, NULL );
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetBlockedMembersL
// -----------------------------------------------------------------------------
//
void CCABlockingPC::GetBlockedMembersL( const TDesC& aIcon )
    {
    const CDesCArray* blockedMembers = iBlockingInterface.BlockedList();
    CDesCArray* blockedWVIDs = CAArrayUtils::CloneArrayLC( *blockedMembers );

    //Temporary array for the popup list
    CDesCArray* identifications = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CDesCArray* nicknames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    CleanupStack::PushL( identifications );
    CleanupStack::PushL( nicknames );


    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    //Populating temporary array
    TInt blockedMembersCount( blockedWVIDs->MdcaCount() );
    for ( TInt count( 0 ); count < blockedMembersCount; ++count )
        {
        identifications->AppendL( contacts->Identification(
                                      blockedWVIDs->MdcaPoint( count ) ) );
        MCAStoredContact* contact = contacts->FindAnyContact(
                                        blockedWVIDs->MdcaPoint( count ) );
        if ( contact )
            {
            nicknames->AppendL( contact->Nickname() );
            }
        else
            {
            nicknames->AppendL( KNullDesC );
            }
        }


    if ( iBlockingArray )
        {
        iBlockingArray->ReplaceL( nicknames, blockedWVIDs, aIcon,
                                  identifications );
        }
    else
        {
        // servercontactsarray takes the ownership
        iBlockingArray = CCAServerContactsArrayPC::NewL( nicknames,
                                                         blockedWVIDs,
                                                         aIcon,
                                                         identifications );
        }

    CleanupStack::Pop( 3, blockedWVIDs );
    // identifications, nicknames, blockedWVIDs

    }

// ---------------------------------------------------------
// CCABlockingUI::GetBlockedMembersLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAServerContactsArrayPC* CCABlockingPC::GetBlockedMembersLC(
    const TDesC& aIcon )
    {
    const CDesCArray* blockedMembers = iBlockingInterface.BlockedList();
    CDesCArray* blockedWVIDs = CAArrayUtils::CloneArrayLC( *blockedMembers );

    //Temporary array for the popup list

    CDesCArray* identifications = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CDesCArray* nicknames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    CleanupStack::PushL( identifications );
    CleanupStack::PushL( nicknames );

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    //Populating temporary array
    TInt blockedMembersCount( blockedWVIDs->MdcaCount() );
    for ( TInt count( 0 ); count < blockedMembersCount; ++count )
        {
        identifications->AppendL( contacts->Identification(
                                      blockedWVIDs->MdcaPoint( count ) ) );
        MCAStoredContact* contact = contacts->FindAnyContact(
                                        blockedWVIDs->MdcaPoint( count ) );
        if ( contact )
            {
            nicknames->AppendL( contact->Nickname() );
            }
        else
            {
            nicknames->AppendL( KNullDesC );
            }
        }

    // servercontactsarray takes the ownership
    CCAServerContactsArrayPC* pairsArray =
        CCAServerContactsArrayPC::NewL( nicknames,
                                        blockedWVIDs,
                                        aIcon, identifications );

    CleanupStack::Pop( 3, blockedWVIDs ); // identifications, nicknames, blockedWVIDs
    CleanupStack::PushL( pairsArray );
    return pairsArray;
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetBlockedMembersL
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC& CCABlockingPC::GetServerContactsArray()
    {

    return *iBlockingArray;

    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetUnBlockListL
// -----------------------------------------------------------------------------
//
CDesCArrayFlat* CCABlockingPC::GetUnBlockListL( TInt aSelectedItem,
                                                const TDesC& aFindTxt )
    {
    // Simulate the findPane functionality when getting the iSelectedItem
    // from the blockingArray structure
    CDesCArrayFlat* unBlockList =  new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    // unBlockList is those persons who are going to be unblocked
    const TCAServerContact* unblocked = iBlockingArray->WVIdL( aSelectedItem, aFindTxt );
    if ( unblocked ) //can be NULL
        {
        unBlockList->AppendL( unblocked->iWVID );
        }
    return unBlockList;

    }

// -----------------------------------------------------------------------------
// CCABlockingPC::GetWvId
// -----------------------------------------------------------------------------
//
TPtrC CCABlockingPC::GetWvIdL( const TDesC& aIcon, TInt aDeletedIndex )
    {
    CCAServerContactsArrayPC* blockedMembers = static_cast
                                               < CCAServerContactsArrayPC* >
                                               ( GetBlockedMembersLC(  aIcon ) );
    if ( ( *blockedMembers ).MdcaCount() < 0 ||
         ( aDeletedIndex < 0 || aDeletedIndex >= ( *blockedMembers ).MdcaCount() ) )
        {
        return KNullDesC();
        }
    TPtrC wvId = ( ( *blockedMembers )[ aDeletedIndex ].iWVID );
    CleanupStack::Pop( blockedMembers );
    return wvId;
    }


// -----------------------------------------------------------------------------
// CCABlockingPC::RegisterBackgroundObserver
// -----------------------------------------------------------------------------
//
void CCABlockingPC::RegisterBackgroundObserver(
    MCABlockingBackgroundObserverPC* aBackgroundObserver )
    {
    ASSERT( aBackgroundObserver );

    iBackgroundObserver = aBackgroundObserver;
    iBackgroundInterface.RegisterBackgroundObserver( this,
                                                     MCABackgroundInterface::EBlockGrantListFetch,
                                                     MCABackgroundInterface::EFailed |
                                                     MCABackgroundInterface::EUnknown |
                                                     MCABackgroundInterface::ECancelled |
                                                     MCABackgroundInterface::ECompleted );

    }

// -----------------------------------------------------------------------------
// CCABlockingPC::RegisterBackgroundObserver
// -----------------------------------------------------------------------------
//
void CCABlockingPC::UnRegisterBackgroundObserver(  )
    {
    iBackgroundObserver = NULL;
    iBackgroundInterface.UnRegisterBackgroundObserver( this );
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::HandleBackgroundEvent
// -----------------------------------------------------------------------------
//
void CCABlockingPC::HandleBackgroundEvent(
    MCABackgroundInterface::TCABackgroundTasks aEventSource,
    MCABackgroundInterface::TCABackgroundStatus aStatus,
    TInt aSubTaskNumber,
    TInt aLeaveCode
)
    {

    TEnumsPC::TCABackgroundTasks eventSource;
    TEnumsPC::TCABackgroundStatus status;

    switch ( aEventSource )
        {
        case MCABackgroundInterface::EGroupFetch:
            {
            eventSource = TEnumsPC::EGroupFetch;
            break;
            }
        case MCABackgroundInterface::EPresence:
            {
            eventSource = TEnumsPC::EPresenceTask;
            break;
            }
        case MCABackgroundInterface::EWhisperSync:
            {
            eventSource = TEnumsPC::EWhisperSync;
            break;
            }
        default:
            {
            eventSource = TEnumsPC::EBlockGrantListFetch;
            break;
            }

        }

    switch ( aStatus )
        {
        case MCABackgroundInterface::EUnknown:
            {
            status = TEnumsPC::EUnknownStatus;
            break;
            }
        case MCABackgroundInterface::EIdleStarted:
            {
            status = TEnumsPC::EIdleStarted;
            break;
            }
        case MCABackgroundInterface::EStarting:
            {
            status = TEnumsPC::EStarting;
            break;
            }
        case MCABackgroundInterface::EWaiting:
            {
            status = TEnumsPC::EWaiting;
            break;
            }
        case MCABackgroundInterface::ECompleted:
            {
            status = TEnumsPC::ECompleted;
            break;
            }
        case MCABackgroundInterface::ECancelled:
            {
            status = TEnumsPC::ECancelled;
            break;
            }
        default:
            {
            status = TEnumsPC::EFailedStatus;
            break;
            }

        }

    iBackgroundObserver->HandleBackgroundEventPC(
        eventSource,
        status,
        aSubTaskNumber,
        aLeaveCode
    );

    }


// -----------------------------------------------------------------------------
// CCABlockingPC::RegisterBackgroundObserver
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::DeleteContactL( TInt aSelectedItem, const TDesC& aFindTxt )
    {

    TInt errorCode( KErrNone );
    // Simulate the findPane functionality when getting the iSelectedItem
    // from the blockingArray structure
    const TCAServerContact* unblocked = iBlockingArray->WVIdL( aSelectedItem, aFindTxt );
    // if block is in hide mode. We have to delete those items from contact lists.
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    TInt countOfLists( contacts->ListCount() );
    for ( TInt listCount( 0 ); listCount < countOfLists && errorCode == KErrNone; ++listCount )
        {
        MCAContactList& list = contacts->ListAt( listCount );
        MCAStoredContact* contact = NULL;

        if ( unblocked ) //can be NULL
            {
            contact = contacts->FindContact( list.ListId(), unblocked->iWVID );
            }

        if ( contact )
            {
            MCAPresence* presence = CAPresenceManager::InstanceL();
            MCAContactLists* listInterface = presence->ContactLists();
            // don't update to network directly
            TRAP( errorCode, listInterface->DeleteServerContactL( list.ListId(),
                                                                  contact->UserId(), EFalse ) );
            }
        }

    // Update all changes with one operation
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();
    listInterface->CommitDeleteServerContactL();

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::RemoveIDsL
// -----------------------------------------------------------------------------
//
void CCABlockingPC::RemoveIDsL( const CDesCArray* aUserIds )
    {
    TInt userIdCount( aUserIds->MdcaCount() );

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    TInt listCount( contacts->ListCount() );

    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();

    for ( TInt listindex( 0 ); listindex < listCount; listindex++ )
        {//list iteration
        MCAContactList& myList( contacts->ListAt( listindex ) );

        for ( TInt useridindex( 0 ); useridindex < userIdCount; useridindex++ )
            {
            listInterface->DeleteServerContactL( myList.ListId(),
                                                 aUserIds->MdcaPoint( useridindex ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCABlockingPC::IsContactBlocked
// -----------------------------------------------------------------------------
//

TBool CCABlockingPC::IsContactBlocked( const TDesC& aContact,
                                       TBool aUseGrant /* EFalse*/ ) const
    {
    return iBlockingInterface.IsContactBlocked( aContact, aUseGrant );
    }


// -----------------------------------------------------------------------------
// CCABlockingPC::SetTemporaryGrantL
// -----------------------------------------------------------------------------
//
TInt CCABlockingPC::SetTemporaryGrantL( const TDesC& aContact, TBool aOn )
    {
    return iBlockingInterface.SetTemporaryGrantL( aContact, aOn );
    }

