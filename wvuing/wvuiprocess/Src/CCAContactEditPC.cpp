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
* Description:  Process Component for Contact Editing
*
*/



// ============================ HEADER FILES ===============================
// INCLUDE FILES
#include "CCAContactEditPC.h"
#include "CCAStorageManagerFactory.h"
#include "MCAPresence.h"
#include "CAPresenceManager.h"
#include "MCAMainViewArrayPC.h"
#include "MCAContactLists.h"
#include "MCAPresence.h"
#include "MCAPresenceError.h"
#include "MCAPresenceErrors.h"
#include "MCAContactList.h"
#include "CCAMainViewArrayPC.h"
#include "CAUtils.h"

#include "ChatDebugAssert.h"
#include "ChatDebugPrint.h"

#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"
#include "MCAProcessManager.h"
#include "MCASettingsPC.h"


// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCAContactEditPC::NewL
// -----------------------------------------------------------------------------
//
CCAContactEditPC* CCAContactEditPC::NewL( MCASettingsPC& aCCASettingsPC )
    {
    CCAContactEditPC* self = CCAContactEditPC::NewLC( aCCASettingsPC );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::NewLC
// -----------------------------------------------------------------------------
//
CCAContactEditPC* CCAContactEditPC::NewLC( MCASettingsPC& aCCASettingsPC )
    {
    CCAContactEditPC* self = new( ELeave ) CCAContactEditPC( aCCASettingsPC );
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCAContactEditPC::~CCAContactEditPC()
    {

    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::CCAContactEditPC
//
// -----------------------------------------------------------------------------
//

CCAContactEditPC::CCAContactEditPC( MCASettingsPC& aCCASettingsPC )
        :  iContact( NULL ),
        iCCASettingsPC( aCCASettingsPC )

    {

    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetContactDetailsL
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::GetContactDetailsL(
    const TBool aAliasEnabled,
    const TBool aNicknameEnabled,
    TBool&  aOwnData,
    HBufC*& aOrigId,
    HBufC*& aAlias,
    HBufC*& aNickname,
    HBufC*& aUserId,
    HBufC*& aIdentification
)
    {

    if ( iContact )
        {
        // found an existing contact, store the user id
        aOwnData = iOwnData;
        aUserId = iContact->UserId().AllocL();

        aOrigId = iContact->UserId().AllocL();

        aIdentification = iContact->Identification().AllocL();

        if ( aAliasEnabled )
            {

            aAlias = iContact->Alias().AllocL();

            if ( ! aAlias->Length() )
                {
                delete aAlias;
                aAlias = NULL;

                if ( aOwnData )
                    {
                    // this is our own status
                    // so get the alias from engine
                    aAlias = iCCASettingsPC.GetSapSettingValuePCL(
                                 TEnumsPC::EOwnAlias, NULL );

                    iContact->SetAliasL( aAlias ? ( *aAlias ) : KNullDesC() );

                    }
                }

            }

        if ( aNicknameEnabled )
            {
            aNickname = iContact->Nickname().AllocL();
            }
        }

    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::IsWVIDAvailableAndValidL
//
// -----------------------------------------------------------------------------
//

TBool CCAContactEditPC::IsWVIDAvailableAndValidL( TBool aCreateNew,
                                                  const TDesC&  aListId,
                                                  const TDesC& aWVID, TBool& aContactInList,
                                                  TDesC*& aUserID )
    {

    TBool retValue = EFalse;
    TBool validContact = ETrue; //This is ETrue, if we are creating a new contact

    // check if there's already a contact with same wvid
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();


    MCAStoredContact* contactInList = contacts->FindContact( aListId, aWVID );

    if ( !aCreateNew )
        {
        //if contact editor in EditMode
        // validContact is EFalse if iContact not a valid contact
        validContact = contacts->IsContactValid( iContact );
        }


    if ( contactInList && validContact )
        {

        retValue = ETrue;

        if ( iContact != contactInList )
            //this is a new contact
            {
            aContactInList = ETrue;
            aUserID = const_cast <TDesC *>( &contactInList->UserId() );

            }
        }

    return retValue;

    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::FindAnyContactByNickL
//
// -----------------------------------------------------------------------------
//

TBool CCAContactEditPC::FindAnyContactByNickL( const TDesC& aNickName )
    {
    TBool retValue = EFalse;
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    MCAStoredContact* contact = contacts->FindAnyContactByNick( aNickName );

    if ( ( contact ) && ( contact != iContact ) )
        {
        retValue = ETrue;
        }
    return retValue;
    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::AddContactL
//
// -----------------------------------------------------------------------------
//

TInt CCAContactEditPC::AddContactL(
    const TDesC& aListId,
    const TDesC& aWVID, const TDesC& aNickName,
    const TDesC& aAlias )
    {
    // fetch the contacts in list
    MCAStoredContact *addedContact = NULL;
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();
    TRAPD( err, addedContact = listInterface->AddServerContactL(
                                   aListId, aWVID, aNickName ) );

    if ( addedContact )
        {
        if ( &aAlias ) //time being fix for crash
            {
            addedContact->SetAliasL( aAlias );
            }
        iContact = addedContact;
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::UpdateContactL
//
// -----------------------------------------------------------------------------
//
TInt CCAContactEditPC::UpdateContactL( const TDesC& aListId,
                                       const TDesC& aWVID,
                                       const TDesC& aNickName )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();

    TRAPD( err, listInterface->UpdateContactNicknameL( aListId, aWVID,
                                                       aNickName ) );
    return err;
    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::RemoveContactL
//
// -----------------------------------------------------------------------------
//

TInt CCAContactEditPC::RemoveContactL(
    const TDesC& aListId,
    const TDesC& aWVID )
    {
    // fetch the contacts in list
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();
    TRAPD( err, listInterface->DeleteServerContactL( aListId, aWVID ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::SetAnyContactL
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::SetAnyContactL( const TDesC& aWVID )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    MCAStoredContact* contact = contacts->FindAnyContact( aWVID );
    iContact = contact;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::SetContactL
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::SetContactL( const TDesC& aListId,
                                    const TDesC& aWVID )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    MCAStoredContact* contact = contacts->FindContact( aListId, aWVID );
    iContact = contact;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetCurrentContactNickname
//
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactEditPC::GetCurrentContactNickname() const
    {
    __CHAT_ASSERT_DEBUG( iContact );
    return iContact->Nickname();
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::GetCurrentContactUserId
//
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactEditPC::GetCurrentContactUserId() const
    {
    __CHAT_ASSERT_DEBUG( iContact );
    return iContact->UserId();
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::InitialiseL
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::InitialiseL(
    MCAMainViewArrayPC& aMainViewArrayPC,
    const TInt index, TBool& aOwnStatus )
    {

    //reset the member variables
    iOwnData = EFalse;
    iContact = NULL;

    CCAMainViewArrayPC& ccaMainViewArrayPC =
        static_cast<CCAMainViewArrayPC&>( aMainViewArrayPC );
    MCAStoredContacts* contactlist =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( index != -1 )
        {
        iContact = ccaMainViewArrayPC.GetContactForContactEditor( index ) ;

        // check for own status
        if ( iContact == &contactlist->OwnStatus() )
            {
            iOwnData = ETrue;
            aOwnStatus = ETrue;
            }

        }
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::IsContact
//
// -----------------------------------------------------------------------------
//
TBool CCAContactEditPC::IsContact() const
    {
    if ( iContact )
        {
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetAliasFromPresenceL
//
// -----------------------------------------------------------------------------
//
HBufC* CCAContactEditPC::GetAliasFromPresenceL( const TDesC& aWVID )
    {
    // add mode - contact does not exist
    // get the alias from the network,
    MCAPresence* presence = CAPresenceManager::InstanceL();

    if ( aWVID.Length() )
        {
        return presence->AliasL( aWVID );
        }
    else
        {
        // write manually, WVID does not exist, thus can't get alias
        // so use an empty alias
        return KNullDesC().AllocL();
        }
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::DeleteCurrentContactL
//
// -----------------------------------------------------------------------------
//
TInt CCAContactEditPC::DeleteCurrentContactL( const TDesC& aListId,
                                              const TDesC& aOrigId )
    {
    // can't delete self
    if ( iOwnData )
        {
        return KErrAccessDenied;
        }

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();

    // delete contact
    TRAPD( error,
           listInterface->DeleteServerContactL( aListId, aOrigId ) );

    if ( error == KErrNone )
        {
        iContact = NULL;
        }

    return error;
    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::LastContactEditOperationResultL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<MCAPresenceError>& CCAContactEditPC::LastContactEditOperationResultL( void )
    {
    // show the detailed error codes
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();
    const MCAPresenceErrors& lastResult =
        listInterface->LastOperationResult();
    return lastResult.DetailedErrors();

    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::GetListIdLC
//
// -----------------------------------------------------------------------------
//
HBufC* CCAContactEditPC::GetListIdL( const TDesC& aId, TInt /*aNoListsResource*/,
                                     TInt &aItemCount, CDesCArrayFlat*& aListIdArray,
                                     RArray< TInt > 	&aIndexArray,
                                     const TBool aMoveOperation /*= EFalse*/ )
    {

    //make sure that we push iIndexArray to the cleanupstack
    CleanupClosePushL( aIndexArray );

    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    // Get list id from listquery.
    TInt listCount = contacts->ListCount();

    // Granularity has to be > 0
    aListIdArray = new ( ELeave ) CDesCArrayFlat( Max( listCount, 1 ) );

    CleanupStack::PushL( aListIdArray ) ;

    if ( !aMoveOperation )
        {
        //In case of Add, all contact lists in which contact is not available is shown.

        for ( TInt index( 0 ); index < listCount; ++index )
            {
            MCAContactList& contactList = contacts->ListAt( index ) ;
            if ( !contacts->FindContact( contactList.ListId(), aId ) )
                {
                aListIdArray->AppendL( contactList.DisplayName() );
                aIndexArray.AppendL( index );
                }
            }
        }
    else
        {
        /* In case of Move, all contact lists, except the current list, are shown in the list
        in same order as in IM contact list.
        */
        for ( TInt index( 0 ); index < listCount; ++index )
            {
            MCAContactList& contactList = contacts->ListAt( index ) ;
            TPtrC listid = contactList.ListId();

            //check whether its the current contact list
            if ( !CAUtils::NeutralCompare(
                     aId,
                     listid, EFalse ) )
                {
                //we have found the current list - Now add all other lists other
                //then the current one.
                for ( TInt jIndex( index + 1 ); jIndex < listCount; ++jIndex )
                    {
                    MCAContactList& contactLst = contacts->ListAt( jIndex ) ;
                    aListIdArray->AppendL( contactLst.DisplayName() );
                    aIndexArray.AppendL( jIndex );
                    }
                //all contact list has been added so break.
                break;

                }
            else
                {
                //this is not the current contact list so add it to the list
                aListIdArray->AppendL( contactList.DisplayName() );
                aIndexArray.AppendL( index );
                }
            }
        }


    aItemCount = aIndexArray.Count() ;

    if ( aItemCount == 0 )
        {
        CleanupStack::PopAndDestroy( 2 ); //aIndexArray,  aListIdArray
        aListIdArray = NULL;
        return NULL;
        }
    else if ( aItemCount == 1 )
        {
        HBufC* listId = contacts->ListAt( aIndexArray[ 0 ] ).ListId().AllocL();
        CleanupStack::PopAndDestroy( 2 ); //aIndexArray,  aListIdArray
        aListIdArray = NULL;
        return listId;
        }

    CleanupStack::Pop( 2 );  //aIndexArray,  aListIdArray

    return NULL;
    }



// -----------------------------------------------------------------------------
// CCAContactEditPC::GetListIdAtIndexLC
//
// -----------------------------------------------------------------------------
//
HBufC* CCAContactEditPC::GetListIdAtIndexL( TInt aSelectedIndex ) const
    {


    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    HBufC* listId = contacts->ListAt( aSelectedIndex ).ListId().AllocL();

    return listId;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::UpdateOwnAliasL
//
// -----------------------------------------------------------------------------
//
TInt CCAContactEditPC::UpdateOwnAliasL( const TDesC & aAlias )
    {

    TRAPD( errAlias, iCCASettingsPC.SetSapSettingValuePCL( TEnumsPC::EOwnAlias,
                                                           aAlias, NULL, NULL ) );

    if ( !errAlias )
        {
        TRAPD ( err, iContact->SetAliasL( aAlias ) );
        return err;
        }

    return errAlias;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::InitialiseL
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::InitialiseL( const TDesC& aWVID,
                                    const TDesC& aListId, TBool& aOwnStatus )
    {

    //reset the member variables
    iOwnData = EFalse;
    iContact = NULL;

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    MCAStoredContact* contactInList = contacts->FindContact( aListId, aWVID );

    if ( contactInList )
        {
        iContact = contactInList;
        // check for own status
        if ( iContact == &contacts->OwnStatus() )
            {
            iOwnData = ETrue;
            aOwnStatus = ETrue;
            }
        }

    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetOnlineStatus
//
// -----------------------------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCAContactEditPC::GetOnlineStatus() const
    {

    TEnumsPC::TOnlineStatus contactStatus = TEnumsPC::EUnknown;

    switch ( iContact->OnlineStatus() )
        {
        case TStorageManagerGlobals::EUnknown :
            {
            contactStatus = TEnumsPC::EUnknown;
            break;
            }

        case TStorageManagerGlobals::EOffline :
            {
            contactStatus = TEnumsPC::EOffline;
            break;
            }

        case TStorageManagerGlobals::EOnline :
            {
            contactStatus = TEnumsPC::EOnline;
            break;
            }

        case TStorageManagerGlobals::EInvisible :
            {
            // this can only come for own status item, no need to check since
            // for other contacts this state is impossible
            contactStatus = TEnumsPC::EInvisible;
            break;
            }

        case TStorageManagerGlobals::EAway :
            {
            contactStatus = TEnumsPC::EAway;
            break;
            }

        case TStorageManagerGlobals::EBusy :
            {
            contactStatus = TEnumsPC::EBusy;
            break;
            }
        default :
            {
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }

    return contactStatus;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetClientType
//
// -----------------------------------------------------------------------------
//
TEnumsPC::TClientType CCAContactEditPC::GetClientType() const
    {

    TEnumsPC::TClientType clientType = TEnumsPC::EUnknownClient;

    switch ( iContact->ClientType() )
        {
        case TStorageManagerGlobals::EUnknownClient :
            {
            clientType = TEnumsPC::EUnknownClient;
            break;
            }

        case TStorageManagerGlobals::EMobile :
            {
            clientType = TEnumsPC::EMobile;
            break;
            }

        case TStorageManagerGlobals::EPC :
            {
            clientType = TEnumsPC::EPC;
            break;
            }

        default :
            {
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }

    return clientType;

    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::GetStatusText
//
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactEditPC::GetStatusText() const
    {
    __CHAT_ASSERT_DEBUG( iContact );
    return iContact->StatusText();
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::GetCurrentContactIdentification
//
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactEditPC::GetCurrentContactIdentification() const
    {
    __CHAT_ASSERT_DEBUG( iContact );
    return iContact->Identification();
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::GetCurrentContactAlias
//
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactEditPC::GetCurrentContactAlias() const
    {
    __CHAT_ASSERT_DEBUG( iContact );
    return iContact->Alias();
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::ReleaseResources
//
// -----------------------------------------------------------------------------
//
void CCAContactEditPC::ReleaseResources()
    {
    //reset the member variables
    iOwnData = EFalse;
    iContact = NULL;
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::IsContactValidL
//
// -----------------------------------------------------------------------------
//
TBool CCAContactEditPC::IsContactValidL( const TDesC& aListId )
    {
    MCAStoredContacts* contactlist =
        CCAStorageManagerFactory::ContactListInterfaceL();
    MCAStoredContact* contact = NULL;

    // Check for own status
    if ( iOwnData )
        {
        contact = &contactlist->OwnStatus();
        }
    else
        {
        // Find contact
        contact = contactlist->FindContact( aListId, iContact->UserId() );
        }

    if ( contact && contact != iContact )
        {
        // new contact pointer found for the original ID's.
        // Let's use it and assume the old one is invalid.
        CHAT_DP_TXT( "CCAContactEditPC::IsContactValidL: new pointer to contact!" );
        iContact = contact;
        }
    return contact ? ETrue : EFalse;
    }


// -----------------------------------------------------------------------------
// CCAContactEditPC::IsContactValidL
//
// -----------------------------------------------------------------------------
//
TBool CCAContactEditPC::IsOwnData() const
    {
    return iOwnData;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetCollapsed
// -----------------------------------------------------------------------------
//
TBool CCAContactEditPC::IsCollapsedL( const TDesC& aListId )
    {
    // this operation should be on list only..otherwise throw exception..
    TBool isCollapsed = EFalse;
    MCAStoredContacts* contactlist =
        CCAStorageManagerFactory::ContactListInterfaceL();
    MCAContactList* contactList = contactlist->FindContactList( aListId );
    isCollapsed =  contactList->Collapsed();
    return isCollapsed;
    }

// -----------------------------------------------------------------------------
// CCAContactEditPC::DeleteContactL
// -----------------------------------------------------------------------------
//
TInt CCAContactEditPC::DeleteContactL( const TDesC& aBlockedContact )
    {

    TInt errorCode( KErrNone );

    // if block is in hide mode. We have to delete those items from contact lists.
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();


    TInt countOfLists( contacts->ListCount() );
    for ( TInt listCount( 0 ); listCount < countOfLists && errorCode == KErrNone; ++listCount )
        {
        MCAContactList& list = contacts->ListAt( listCount );
        MCAStoredContact* contact = NULL;

        contact = contacts->FindContact( list.ListId(), aBlockedContact );

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

//    End of File
