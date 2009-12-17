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
* Description:  Main view array interface to get  the array items to be displayed
				 via the listbox for the Single View
*
*/


//platform includes
#include <aknview.h>
#include <eiklbo.h>
#include <badesca.h>
#include <bldvariant.hrh>
#include <e32cmn.h>
#include <e32def.h>
#include <aknenv.h>
#include <StringLoader.h>
#include <barsread.h>
#include <chatNG.rsg>
#include <collate.h>


#include "ImpsCSPAllErrors.h"
#include "chatdefinitions.h"
#include "ChatDebugPrint.h"
#include "CCAArrayItemPC.h"
#include "MCAStoredContacts.h"
#include "MCAContactLists.h"
#include "MCAContactList.h"
#include "ChatDebugAssert.h"
#include "MCAStoredContactsObserver.h"
#include "CCAStorageManagerFactory.h"
#include "MCASettingsObserver.h"
#include "CAPresenceManager.h"
#include "MCAChatInterface.h"
#include "IMUtils.h"
#include "CCAMainViewArrayPC.h"
#include "MCAMessageContainerInfo.h"

#include <cimpspresenceconnectionuing.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>

#include "CCAPCUtils.h"
#include "PublicEngineDefinitions.h"
#include "CCAEngine.h"
#include "MCAInvite.h"
#include "MCAInvitation.h"
#include "ChatDebugPrint.h"
#include "CCAServerContactsArrayPC.h"

#include "MCAStoredContact.h"
#include "MCAStoredGroups.h"
#include "MCAStoredGroup.h"
#include "MCASingleListViewCB.h"
#include "CAUtils.h"
#include "MCAMessage.h"
#include "CCAGroupListArrayPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include "MCABlocking.h"

const TInt KGranularity = 2;    //array granularity

_LIT( KContactOnlineIconA,      "12\t" );
_LIT( KEmptyIconB,              "11\t" );
_LIT( KContactWatcherIcon,      "\t2" );
#ifdef RD_MESSAGE_INDICATION_NEW
_LIT( KContactMsgPendingIcon,   "3\t" );
#else
_LIT( KContactMsgPendingIcon,   "\t3" );
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::CCAMainViewArrayPC
// -----------------------------------------------------------------------------

CCAMainViewArrayPC::CCAMainViewArrayPC( CCAEngine& aEngine, MCAContactListModel& aContactListModel,
                                        MCAChatInterface& aChatInterface )
        : iEngine( aEngine ),
        iContactListModel( aContactListModel ),
        iChatInterface( aChatInterface ),
        iInviteInterface( *iEngine.GetInvitationInterface() ),
        iOwnDataIndex( KOwnDataIndex ),	//	starting index of Owndata, it's 0 as per ui spec
        iOpenChatsIndex( KOpenChatsIndex ),//	starting index of OpenChats, it's 1 as per ui spec
        iContactListIndex( KOwnDataIndex ),	//	starting index of all the contactlists......
        iOpenChatListItemExpanded( EFalse ),// initially the convesation in collapsed state..
        iSelectionMode( /*EFalse*/TEnumsPC::ESingleListViewSelect ),
        iExpandAll( EFalse ),
        iSettings( aEngine.GetInternalSettingsInterface() )
// inializing the settings
    {

    iGroupCountDown = EFalse;
    iOnlySavedGroups = 0;

    iForceExpandDone = ETrue; // set this to false initially
    iPToPChatCount = 0;
    iItemDetails.aItemID.Copy( KNullDesC );
    iItemDetails.aItemID.Copy( KNullDesC );
    iItemDetails.aItem = TEnumsPC::EInvalid;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::NewL
// -----------------------------------------------------------------------------

CCAMainViewArrayPC* CCAMainViewArrayPC::NewL( CCAEngine& aEngine,
                                              MCAContactListModel& aContactListModel,
                                              MCAChatInterface& aChatInterface )
    {
    CCAMainViewArrayPC* self = new ( ELeave ) CCAMainViewArrayPC( aEngine,
                                                                  aContactListModel,
                                                                  aChatInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ConstructL
// -----------------------------------------------------------------------------
void CCAMainViewArrayPC::ConstructL()
    {
    iMaxLengthOfIcons =
        KEmptyIconB().Length() + // longest B-column icon
        KContactOnlineIconA().Length() + // longest A-Column icon
        KContactWatcherIcon().Length() +
        KContactMsgPendingIcon().Length();
    iData = HBufC::NewL( KFriendsListMaxIdentificationLength );
    iContactInterface = CCAStorageManagerFactory::ContactListInterfaceL();
    iContactInterface->AddObserverL( this );// listen these events..
    CAPresenceManager::InstanceL()->SetWatcherObserver( this );
    iOwnDataAdded = false;

    iEngine.GetInvitationInterface()->AddInviteObserver( this );

    iSettings->AddObserverL( this );  //register with engine
    // Register this as chat observer
    iChatInterface.RegisterChatObserver( this );

    // This callback is required when ever the server is added or deleted
    // we get a callback and update the singlelist

    iSAPSettingsStore = CIMPSSAPSettingsStore::NewL();
    iSAPSettingsStore->AddObserverL( this, EIMPSIMAccessGroup );
    iGroupArray =  CCAGroupListArrayPC::NewL();
    //register for stored groups observer event
    iChatGroups = CCAStorageManagerFactory::GroupListInterfaceL();
    iChatGroups->AddObserverL( this );
    iLastloggedinUserId = NULL;
    iConnectionUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );

    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetSkipStorageEvents
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetSkipStorageEvents( TBool aSkip )
    {
    iSkipStorageEvents = aSkip;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetSkipStorageEvents
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::GetSkipStorageEvents()
    {
    return iSkipStorageEvents;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::~CCAMainViewArrayPC
// -----------------------------------------------------------------------------

CCAMainViewArrayPC::~CCAMainViewArrayPC()
    {
    delete iData;
    //ignore codescanner warning
    // because dest is getting called
    MCAPresence* presenceManager = NULL;
    TRAP_IGNORE( presenceManager = CAPresenceManager::InstanceL() );
    if ( presenceManager )
        {
        presenceManager->SetWatcherObserver( NULL );
        }
    iChatInterface.UnregisterChatObserver( this );
    iMainViewListItemArray.ResetAndDestroy();
    iMainViewListItemArray.Close();
    iOpenChatsArray.ResetAndDestroy();
    iOpenChatsArray.Close();
    iMainViewObservers.ResetAndDestroy();
    iMainViewObservers.Close();
    iInvitationsArray.Reset();
    iInvitationsArray.Close();
    if ( iContactInterface )
        {
        iContactInterface->RemoveObserver( this );
        }
    if ( iEngine.GetInvitationInterface() )
        {
        iEngine.GetInvitationInterface()->RemoveInviteObserver( this );
        }


    // Note : remove the observer before the set value since after setting the
    // value we get callback which we are not intrested as the application is exited
    // no point in calling the reset array and trying to fill the data
    if ( iSettings )
        {
        iSettings->RemoveObserver( this ); // remove from engine
        }


    //unregister for stored groups observer event
    if ( iChatGroups )
        {
        iChatGroups->RemoveObserver( this );
        }

    iSAPSettingsStore->RemoveObserver( this );
    delete iSAPSettingsStore;
    delete iGroupArray;
    delete iLastloggedinUserId;
    delete iConnectionUI;


    iRefreshConvArray.ResetAndDestroy();

    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsForceExpanded
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::IsForceExpanded() const
    {
    return iContactListModel.IsForceExpanded();
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetMsgPending
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetMsgPending( TInt aPendingMsgs, TInt aIndex )
    {

    TInt index = aIndex - iContactListIndex <= 0 ? 0 : aIndex - iContactListIndex ;

    if ( GetType( aIndex ) == TEnumsPC::EContactItem  )
        {
        MCAContactListModel::SItem item = iContactListModel.Item( index );
        MCAStoredContact* Contact = item.iContact;
        if ( Contact )
            {
            Contact->SetPendingMessages( aPendingMsgs );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetItemNameTextL
// -----------------------------------------------------------------------------

TPtrC CCAMainViewArrayPC::GetItemNameText( TInt aIndex )
    {
    TEnumsPC::TItem itemtype = GetType( aIndex );


    /* Codescanner warning is ignored, since Bound check is done
     * inside the GetType()
     * method*/
    /*aIndex >= iOpenChatsIndex*/
    switch ( itemtype )

        {
        case TEnumsPC::EOwnStatusItem:
            {
            MCAArrayItemPC *arrayItemPC = ( MCAArrayItemPC * )iMainViewListItemArray[aIndex];
            return arrayItemPC->GetItemNameText();
            }
        case TEnumsPC::EOpenChatsListItem:
        case TEnumsPC::EGroupItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
            {
            if ( aIndex <= iContactListIndex )
                {
                MCAArrayItemPC *arrayItemPC = ( MCAArrayItemPC * )iOpenChatsArray[aIndex - iOpenChatsIndex];
                return arrayItemPC->GetItemNameText();
                }
            break;
            }
        case TEnumsPC::EContactListItem:
        case TEnumsPC::EContactItem:
            {
            TPtrC itemNameText( KNullDesC );
            TRAP_IGNORE( itemNameText.Set( GetFormattedItemNameTextL( aIndex ) ) );
            return itemNameText;
            }
        case TEnumsPC::EInvalid:
        default:
            {
            return KNullDesC();
            }
        }
    return KNullDesC();
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetFormattedItemNameTextL
// -----------------------------------------------------------------------------
TPtr CCAMainViewArrayPC::GetFormattedItemNameTextL( TInt aIndex )
    {
    TEnumsPC::TItem itemType = GetType( aIndex );

    TInt index = GetItemEngineIndex( aIndex ) ;
    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAContactList* list = item.iContactList;
    TPtr buffer( iData->Des() );
    buffer.Zero();
    switch ( itemType )
        {
        case TEnumsPC::EContactListItem:
            {
            if ( list )
                {
                // C-column
                // append "number of online contacts / total number
                // of contacts" after identification
                CArrayFix<TInt>* intArray = new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
                CleanupStack::PushL( intArray );
                // Append number of online contacts
                TStorageManagerGlobals::TFilterType filterType
                = GetFilterConverterUITOEngine( GetSingleListArrayFilter() );
                intArray->AppendL( list->FilteredCount( filterType ) );
                // Append total number of contacts, depends on how blocked contacts are handled
                if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) != 2 )
                    {
                    // Blocked contacts are hidden or removed, do not include to total count.
                    // Removed is also handled here because user may log in with different clients
                    // that handle blocked contacts differently.
                    intArray->AppendL( list->FilteredCount(
                                           TStorageManagerGlobals::EFilterNonBlocked ) );
                    }
                else
                    {
                    // Blocked contacs are shown, Count() returns correct value
                    intArray->AppendL( list->Count() );
                    }
                CDesCArray* array = new( ELeave ) CDesCArrayFlat( 1 ); // one items
                CleanupStack::PushL( array );
                array->AppendL( KNullDesC );

                HBufC* textNumbers = StringLoader::LoadLC( R_QTN_CHAT_CONTACTLIST_CONTACTLIST,
                                                           *array, *intArray );
                array->Reset();

                TPtr textNumbersPtr( textNumbers->Des() );

                TPtrC identification ( list->DisplayName() );

                array->AppendL( identification.Left( buffer.MaxLength() -
                                                     iMaxLengthOfIcons - textNumbersPtr.Length() ) );

                HBufC* listString = StringLoader::LoadLC( R_QTN_CHAT_CONTACTLIST_CONTACTLIST,
                                                          *array, *intArray );
                TPtr listStringPtr( listString->Des() );

                buffer.Append( listStringPtr );
                //Code scanner warning ignored
                CleanupStack::PopAndDestroy( 4, intArray ); //array,textNumbers,listString,intArray
                }
            break;
            }
        case TEnumsPC::EOwnStatusItem:
        case TEnumsPC::EContactItem:
            {
            buffer.Append( iContactListModel.MdcaPoint( index ) );
            break;
            }
        default:
            {
            break;
            }
        }
    return buffer;
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::Filter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TEnumsPC::TFilterType CCAMainViewArrayPC::Filter()
    {
    return iFilter;
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetDefaultFilter
// -----------------------------------------------------------------------------

TEnumsPC::TFilterType CCAMainViewArrayPC::GetDefaultFilter() const
    {
    return iDefaultFilter;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetDefaultFilter
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetDefaultFilter( TEnumsPC::TFilterType aFilter )
    {
    iDefaultFilter = aFilter;
    iContactListModel.SetDefaultFilter( GetFilterConverterUITOEngine( aFilter ) );
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetDefaultFilter
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetFilter( TEnumsPC::TFilterType aFilter )
    {
    iFilter = aFilter;
    iContactListModel.SetFilter( GetFilterConverterUITOEngine( aFilter ) );
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetFilterConverterUITOEngine
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TFilterType
CCAMainViewArrayPC::GetFilterConverterUITOEngine( TEnumsPC::TFilterType aFilter )
    {

    TStorageManagerGlobals::TFilterType filter;
    switch ( aFilter )
        {

        case TEnumsPC::EFilterAll:
            {
            filter = TStorageManagerGlobals::EFilterAll;
            break;
            }
        case TEnumsPC::EFilterAndOperation:
            {
            filter = TStorageManagerGlobals::EFilterAndOperation;
            break;
            }
        case TEnumsPC::EFilterOnline:
            {
            filter = TStorageManagerGlobals::EFilterOnline;
            break;
            }
        case TEnumsPC::EFilterAway:
            {
            filter = TStorageManagerGlobals::EFilterAway;
            break;
            }
        case TEnumsPC::EFilterNonOffline:
            {
            filter = TStorageManagerGlobals::EFilterNonOffline;
            break;
            }
        case TEnumsPC::EFilterNonBlocked:
            {
            filter = TStorageManagerGlobals::EFilterNonBlocked;
            break;
            }

        case TEnumsPC::EFilterNonOfflineNonBlockedAndOperation:
            {
            filter = TStorageManagerGlobals::TFilterType
                     (
                         TStorageManagerGlobals::EFilterNonOffline |
                         TStorageManagerGlobals::EFilterNonBlocked +
                         TStorageManagerGlobals::EFilterAndOperation
                     );
            break;
            }
        case TEnumsPC::EFilterAllNonBlockedAndOperation:
            {
            filter = TStorageManagerGlobals::TFilterType
                     (
                         TStorageManagerGlobals::EFilterAll |
                         TStorageManagerGlobals::EFilterNonBlocked +
                         TStorageManagerGlobals::EFilterAndOperation
                     );
            break;
            }

        case TEnumsPC::EFilterNonOfflineNonBlockedNonOfflineOperation:
            {
            filter = TStorageManagerGlobals::TFilterType
                     (
                         ( TStorageManagerGlobals::EFilterNonOffline
                           | TStorageManagerGlobals::EFilterNonBlocked
                           + TStorageManagerGlobals::EFilterAndOperation
                         )
                         | TStorageManagerGlobals::EFilterNonOffline
                         + TStorageManagerGlobals::EFilterAndOperation
                     );

            break;
            }

        case TEnumsPC::EFilterAllNonBlockedNonOfflineOperation:
            {
            filter = TStorageManagerGlobals::TFilterType
                     (
                         (
                             TStorageManagerGlobals::EFilterAll
                             | TStorageManagerGlobals::EFilterNonBlocked
                             + TStorageManagerGlobals::EFilterAndOperation
                         )
                         | TStorageManagerGlobals::EFilterNonOffline
                         + TStorageManagerGlobals::EFilterAndOperation
                     );
            break;
            }

        default:
            {
            filter = TStorageManagerGlobals::EFilterNotDefined;
            break;
            }
        }
    return filter;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ResetDefaultFilter
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::ResetDefaultFilter()
    {
    iFilter = iDefaultFilter;
    iContactListModel.ResetDefaultFilter();
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsAllSynchronised
// -----------------------------------------------------------------------------

TEnumsPC::TSyncStatus CCAMainViewArrayPC::IsAllSynchronised()
    {

    TBool allSynchronised 	= ETrue;
    TBool failedFromAll 	= EFalse;

    allSynchronised = iContactInterface->IsAllSynchronised( failedFromAll );

    if ( allSynchronised && failedFromAll )
        return TEnumsPC::ESyncFailed;
    else if ( allSynchronised )
        return TEnumsPC::ESyncSuccess;
    else

        return TEnumsPC::ESyncNotDone;

    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetSingleListArrayFilter
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::SetSingleListArrayFilter( TEnumsPC::TFilterType aFilter )
    {
    iSingleListArrayFilter = aFilter;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetSingleListArrayFilter
// -----------------------------------------------------------------------------
//
TEnumsPC::TFilterType CCAMainViewArrayPC::GetSingleListArrayFilter()
    {
    return iSingleListArrayFilter;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::Synchronised
// -----------------------------------------------------------------------------

TEnumsPC::TSyncStatus CCAMainViewArrayPC::Synchronised( TInt aIndex )
    {

    // don't operate this function other than contactlists..
    TEnumsPC::TItem itemtype = GetType( aIndex );
    if ( itemtype != TEnumsPC::EContactListItem )
        {
        return 	TEnumsPC::ESyncUnknown; // return unknowncode...
        }
    TInt index = GetItemEngineIndex( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );

    //invalid index
    if ( item.iType == MCAContactListModel::EInvalid && !item.iContactList )
        {
        return TEnumsPC::ESyncUnknown;
        }

    TEnumsPC::TSyncStatus statustype;

    switch ( item.iContactList->Synchronised() )
        {
        case MCAContactList::ESynchroniseDone:
            {
            statustype = TEnumsPC::ESyncSuccess;
            break;
            }
        case MCAContactList::ESynchroniseFailed:
            {
            statustype = TEnumsPC::ESyncFailed;
            break;
            }
        // ESynchroniseNotDone is different from
        // ESynchroniseFailed, so check what exactly difference and
        // return the corssponding variable....
        case MCAContactList::ESynchroniseNotDone:
            {
            statustype = TEnumsPC::ESyncNotDone;
            break;
            }
        default:
            {
            statustype = TEnumsPC::ESyncUnknown;
            break;
            }
        }
    return statustype;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ClientType
// -----------------------------------------------------------------------------

TEnumsPC::TClientType CCAMainViewArrayPC::ClientType( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );
    TEnumsPC::TClientType clienttype = TEnumsPC::EUnknownClient ;

    if ( itemtype == TEnumsPC::EContactItem  )
        {
        MCAContactListModel::SItem item = iContactListModel.Item( index );
        MCAStoredContact* Contact = item.iContact;
        if ( Contact )
            {

            switch ( Contact->ClientType() )
                {
                case TStorageManagerGlobals::EMobile:
                    clienttype = TEnumsPC::EMobile;
                    break;
                case TStorageManagerGlobals::EPC:
                    clienttype = TEnumsPC::EPC;
                    break;
                default:
                    clienttype = TEnumsPC::EUnknownClient;
                    break;
                }
            }
        }
    return clienttype;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::Selected
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::Selected( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    MCAContactList* list = item.iContactList;
    if ( itemtype == TEnumsPC::EContactItem && contact  && list )
        {
        return ( contact->Selected() || list->Selected() ) ;
        }
    else if ( itemtype == TEnumsPC::EContactListItem && list )
        {
        return list->Selected();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsWatched
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::IsWatched( TInt aIndex )
    {
    // map this index to correct list and give the correct index to engine..
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( itemtype == TEnumsPC::EContactItem  && contact )
        {
        return contact->IsWatched();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::AddWatcher
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::AddWatcherL( TPtrC aContact )
    {
    CAPresenceManager::InstanceL()->AddWatcherL( aContact );
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsWatched
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::RemoveWatcherL( TPtrC aContact )
    {
    CAPresenceManager::InstanceL()->RemoveWatcherL( aContact );
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::RefreshFriendsL
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::RefreshFriendsL()
    {
    CAPresenceManager::InstanceL()->RefreshFriendsL();
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetOnlineUsersL
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::GetOnlineUsersL( const CDesCArray *aUsers,
                                          CDesCArray* aOnlineList,
                                          CDesCArray* aOfflineList,
                                          TBool aUpdateStorage )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    presence->GetOnlineUsersL( *aUsers, aOnlineList, aOfflineList, aUpdateStorage );
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetOnlineStatus
// -----------------------------------------------------------------------------

TEnumsPC::TOnlineStatus CCAMainViewArrayPC::GetOnlineStatus( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( contact && ( itemtype == TEnumsPC::EContactItem ||
                      itemtype == TEnumsPC::EOwnStatusItem ) )
        {
        return GetOnlineStatusConverter( contact->OnlineStatus() );
        }
    else
        {
        return TEnumsPC::EUnknown;
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetOnlineStatusConverter
// -----------------------------------------------------------------------------

TEnumsPC::TOnlineStatus CCAMainViewArrayPC::GetOnlineStatusConverter(
    TStorageManagerGlobals::TPresenceStatus aEngOnlineStatus )
    {
    switch ( aEngOnlineStatus )
        {
        case TStorageManagerGlobals::EOffline:
            return TEnumsPC::EOffline;
        case TStorageManagerGlobals::EOnline:
            return TEnumsPC::EOnline;
        case TStorageManagerGlobals::EInvisible:
            return TEnumsPC::EInvisible;
        case TStorageManagerGlobals::EAway:
            return TEnumsPC::EAway;
        case TStorageManagerGlobals::EBusy:
            return TEnumsPC::EBusy;
        default:
            return TEnumsPC::EUnknown;
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetBlocked
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetBlocked( TBool aBlockStatus, TInt aIndex )
    {

    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( itemtype == TEnumsPC::EContactItem  && contact )
        {
        contact->SetBlocked( aBlockStatus );
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ForceExpanded
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::ForceExpanded( TBool aAllExpanded )
    {
    iContactListModel.ForceExpanded( aAllExpanded );
    iExpandAll = aAllExpanded;
    ResetArray( EFalse );
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetOnlineStatus
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::SetOnlineStatus( TEnumsPC::TOnlineStatus aStatus, TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( itemtype == TEnumsPC::EContactItem  && contact )
        {
        TStorageManagerGlobals::TPresenceStatus status;
        switch ( aStatus )
            {
            case TEnumsPC::EOffline:
                {
                status = TStorageManagerGlobals::EOffline;
                break;
                }
            case TEnumsPC::EOnline:
                {
                status = TStorageManagerGlobals::EOnline;
                break;
                }
            case TEnumsPC::EInvisible:
                {
                status = TStorageManagerGlobals::EInvisible;
                break;
                }

            case TEnumsPC::EAway:
                {
                status = TStorageManagerGlobals::EAway;
                break;
                }
            case TEnumsPC::EBusy:
                {
                status = TStorageManagerGlobals::EBusy;
                break;
                }
            default:
                {
                status = TStorageManagerGlobals::EUnknown;
                }
            }
        contact->SetOnlineStatus( status );
        }
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::Count
// -----------------------------------------------------------------------------

TInt CCAMainViewArrayPC::Count() const
    {
    // return the total count here.

    return iMainViewListItemArray.Count() + iContactListIndex;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetTypeConverter
// Get the type from engine and return the TEnumsPC type
// -----------------------------------------------------------------------------
TEnumsPC::TItem CCAMainViewArrayPC::GetTypeConverter(
    MCAContactListModel::TItemType aItemType ) const
    {
    TEnumsPC::TItem type = TEnumsPC::EInvalid;
    switch ( aItemType )
        {
        case MCAContactListModel::EInvalid:
            {
            type = TEnumsPC::EInvalid;
            break;
            }
        case MCAContactListModel::EOwnStatus:
            {
            type = TEnumsPC::EOwnStatusItem;
            break;
            }
        case MCAContactListModel::EContactList:
            {
            type = TEnumsPC::EContactListItem;
            break;
            }
        case MCAContactListModel::EContactItem:
            {
            type = TEnumsPC::EContactItem;
            break;
            }
        case MCAContactListModel::EAll:
            {
            type = TEnumsPC::EAll;
            break;
            }
        }
    return type;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetOpenChatType
// Get the type from engine and return the type
// -----------------------------------------------------------------------------

TEnumsPC::TItem CCAMainViewArrayPC::GetOpenChatType( TInt aIndex ) const
    {
    TInt openChatArrayCount = iOpenChatsArray.Count();
    if ( SingleListViewSelectMode() &&
         ( aIndex >= iOpenChatsIndex && aIndex <= openChatArrayCount ) )
        {
        MCAArrayItemPC* item = iOpenChatsArray[ aIndex - 1 ];
        return item->Type();
        }
    else
        {
        return TEnumsPC::EInvalid;
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetType
// Get the type from engine and return the type
// -----------------------------------------------------------------------------

TEnumsPC::TItem CCAMainViewArrayPC::GetType( TInt aIndex ) const
    {
    TInt totalCount = Count();
    TInt itemArrayCount = iMainViewListItemArray.Count();
    TInt openChatArrayCount = iOpenChatsArray.Count();
    if ( totalCount <= 0 || ( aIndex < 0 || aIndex >= totalCount ) )
        {
        return TEnumsPC::EInvalid;
        }
    if ( SingleListViewSelectMode() &&
         ( aIndex >= iOpenChatsIndex && aIndex <= iContactListIndex ) )
        {
        MCAArrayItemPC* item = iOpenChatsArray[ aIndex - 1 ];
        return item->Type();
        }
    else if ( iMainViewListItemArray.Count() > 0 &&
              aIndex <= iContactListIndex + iMainViewListItemArray.Count() )
        {
        TInt index = aIndex - iContactListIndex <= 0 ? 0 : aIndex - iContactListIndex;
        MCAArrayItemPC* item = iMainViewListItemArray[ index ];
        return item->Type();
        }
    else
        {
        return TEnumsPC::EInvalid;
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SingleListViewSelectMode
// -----------------------------------------------------------------------------
//
TBool CCAMainViewArrayPC::SingleListViewSelectMode() const
    {
    if ( iSelectionMode == TEnumsPC::ESingleListViewSelect )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsCollapsed
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::IsCollapsed( TInt aIndex ) const
    {
    // this operation should be on list only..otherwise throw exception..
    TBool iscollapsed = EFalse;
    TEnumsPC::TItem itemtype = GetType( aIndex );

    TInt index = GetItemEngineIndex( aIndex );

    MCAContactListModel::SItem item;
    MCAContactList* list = NULL;

    //check the correct type and send the status
    switch ( itemtype )
        {
        case TEnumsPC::EContactListItem:
        case TEnumsPC::EContactItem:
            {
            if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
                {
                index = index  + iSelectedListIndex;
                }
            item = iContactListModel.Item( index );
            list =  item.iContactList;
            if ( list )
                {
                iscollapsed = list->Collapsed();
                }
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EGroupItem:
            {
            return !iOpenChatListItemExpanded;
            }

        default:
            {
            iscollapsed = EFalse;
            break;
            }
        }
    return iscollapsed;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetCollapsed
// -----------------------------------------------------------------------------


void CCAMainViewArrayPC::SetCollapsed( TBool aStatus , TInt aIndex )
    {
    //This operation should be on list only..otherwise throw exception..
    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item;
    switch ( itemtype )
        {
        case TEnumsPC::EContactListItem:
        case TEnumsPC::EContactItem:
            {
            TInt index = GetItemEngineIndex( aIndex );
            item = iContactListModel.Item( index );
            if ( item.iType == MCAContactListModel::EContactList ||
                 item.iType == MCAContactListModel::EContactItem )
                {
                // there's a contact list or contact in current position
                MCAContactList& list = *item.iContactList;
                list.SetCollapsed( aStatus );

                // this is actually heavy call, becoz the array has
                // to be refreshed completely. To do this in optimized way.
                ResetArray( EFalse );
                }
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EGroupItem:
            {
            // inverse the expanded state

            iOpenChatListItemExpanded = !iOpenChatListItemExpanded;
            if ( iOpenChatListItemExpanded )
                {
                iContactListIndex = iOpenChatsArray.Count();
                }
            else
                {
                iContactListIndex = iOpenChatsIndex;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsMsgPending
// -----------------------------------------------------------------------------
//
TBool CCAMainViewArrayPC::IsMsgPending( TInt aIndex )
    {

    // this operation should be on list only..otherwise throw exception..

    TInt index = GetItemEngineIndex( aIndex );
    TBool isMsgPending = EFalse;
    MCAContactListModel::SItem item;
    MCAStoredContact* contact = NULL;
    MCAContactList *list = NULL;
    //check the correct type and send the status
    switch ( GetType( aIndex ) )
        {
        case TEnumsPC::EContactItem :
            {
            item = iContactListModel.Item( index );
            contact = item.iContact;
            if ( contact )
                {
                isMsgPending = contact->PendingMessages() > 0 ? ETrue : EFalse;
                }
            break;
            }
        case TEnumsPC::EContactListItem:
            {
            item = iContactListModel.Item( index );
            list = item.iContactList;
            if ( list )
                {
                TBool status = list->PendingMessages() > 0 &&
                               list->Synchronised() == MCAContactList::ESynchroniseDone;
                isMsgPending = status ? ETrue : EFalse;
                }

            break;
            }
        case TEnumsPC::EInviteItem:
            {
            //get the pair to the invitation item here.
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAInviteItemPC* invItemPC =
                dynamic_cast<CCAInviteItemPC*>( iOpenChatsArray[index] );
            TPtrC invId = invItemPC->GetInviteID();

            isMsgPending = IsOpenChatsMsgPending( TEnumsPC::EInviteItem, invId );

            break;
            }
        case TEnumsPC::EConversationItem:
            {
            //get the pair to the conversationItem here.
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAConversationItemPC* convItemPC =
                dynamic_cast<CCAConversationItemPC*>( iOpenChatsArray[index] );
            TPtrC userId = convItemPC->UserId();

            isMsgPending = IsOpenChatsMsgPending( TEnumsPC::EConversationItem, userId );

            break;
            }
        case TEnumsPC::EGroupItem:
            {
            TArrayPairs pair;

            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAGroupItemPC* groupItem = dynamic_cast< CCAGroupItemPC* >
                                        ( iOpenChatsArray[ index ] );

            isMsgPending = IsOpenChatsMsgPending( TEnumsPC::EGroupItem, groupItem->GroupId() );

            break;
            }

        default:
            {
            break;
            }
        }

    return isMsgPending;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::IsBlocked
// -----------------------------------------------------------------------------

TBool CCAMainViewArrayPC::IsBlocked( TInt aIndex ) const
    {
    // this operation should be on list only..otherwise throw exception..

    TBool status;
    MCAContactListModel::SItem item;
    MCAStoredContact* contact = NULL;
    TInt index = GetItemEngineIndex( aIndex );
    //check the correct type and send the status
    TEnumsPC::TItem itemtype = GetType( aIndex );
    switch ( itemtype )
        {

        case TEnumsPC::EContactItem:
        case TEnumsPC::EContactListItem:
            {
            if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
                {
                index =   aIndex + iSelectedListIndex ;
                }
            item =  iContactListModel.Item( index );
            if ( item.iType == MCAContactListModel::EContactItem )
                {
                contact = item.iContact;
                status =  contact->IsBlocked();
                }
            else
                {
                status =  EFalse;
                }
            break;
            }

        case TEnumsPC::EConversationItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
              * inside the GetType()
             * method*/
            CCAConversationItemPC* conversationItem =
                dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[ index] );

            status = iEngine.GetBlockingInterface()->IsContactBlocked( conversationItem->UserId() );
            break;
            }
        case TEnumsPC::EInviteItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ index] );

            status = iEngine.GetBlockingInterface()->IsContactBlocked( inviteItem->UserId() );
            break;
            }

        default: //Could be a open chat list or open chat item
            {
            status =  EFalse;
            break;
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillArrayL
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::FillArrayL( TBool aFillOpenChatsArray )
    {

    // now fill remaining details with all contact details..
    if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
        {
        FillSelectedContactListL();
        }
    else
        {
        // first fill the owndata item..
        if ( iSelectionMode == TEnumsPC::ESingleListViewSelect )
            {
            if ( FillOwnDataL() && aFillOpenChatsArray )
                {
                FillOpenChatsL();    // then fill the conversations..or open chats....
                }
            }
        else
            {
            iContactListIndex = 0; // we have not added any thing to main array..so index is zero...
            }
        FillContactListsL();
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillContactListsL
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::FillContactListsL()
    {

    // the index is starting from 1, because we already added owndata item to the list.......
    for ( TInt iIndex = 0; iIndex < iContactListModel.Count(); iIndex++ )
        {
        FillItemL( iIndex );
        }


    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillSelectedContactListL
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::FillSelectedContactListL( )
    {

    MCAContactListModel::SItem item = iContactListModel.Item( iSelectedListIndex );
    MCAContactList* list = item.iContactList;
    if ( !list )
        {
        return;
        }
    TInt count( 0 );
    if ( !list->Collapsed() )
        {
        count = list->FilteredCount( GetFilterConverterUITOEngine( Filter() ) );
        }

    for ( TInt index = iSelectedListIndex ; index <= iSelectedListIndex + count ; index++ )
        {
        FillItemL( index );
        }

    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ResetArray
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::ResetArray( TBool aClearOpenChatsArray )
    {
    //Clear the array and then fill the array agian with fresh data
    ClearArray( aClearOpenChatsArray );
    TRAPD( err, FillArrayL( aClearOpenChatsArray ) );

    if ( err != KErrNone )
        {
        CHAT_DP( D_CHAT_LIT( "CCAMainViewArrayPC::ResetArray - Unexpected, \
                              left with %d" ), err );
        CActiveScheduler::Current()->Error( err );
        }

    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ClearArray
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::ClearArray( TBool aClearOpenChatsArray )
    {
    //clear the array
    iOwnDataAdded = false;
    iMainViewListItemArray.ResetAndDestroy();
    if ( aClearOpenChatsArray )
        {
        iOpenChatsArray.ResetAndDestroy();
        }
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillItemL
// Fill up the complete array one item at a time
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::FillItemL( TInt aIndex )
    {
    //Get the type of item here based on index......

    TPtr dataPtr( iData->Des() );
    dataPtr.Zero();


    // fetch the item and process correct type
    MCAContactListModel::SItem item = iContactListModel.Item( aIndex );
    switch ( item.iType )
        {
        case MCAContactListModel::EContactList:
            {
            // contact list
            MCAContactList* list = item.iContactList;
            __CHAT_ASSERT_DEBUG( list );

            //  add this items to contact list item -- CONTACTLIST
            //  create an object of this type and append this to the rpointerarray
            //  get the online stauts from engine-  switch( contactItem->OnlineStatus() )

            CCAContactListItemPC* contactListItem = CCAContactListItemPC::NewL();
            // to avoid the code scanner warning
            CleanupStack::PushL( contactListItem );
            //initialize the item here..
            MCAContactList::TSynchroniseState syncState = list->Synchronised();
            TStorageManagerGlobals::TFilterType filterType
            = GetFilterConverterUITOEngine( GetSingleListArrayFilter() );

            if ( syncState == MCAContactList::ESynchroniseDone )
                {
                TInt countOne = list->FilteredCount( filterType );
                TInt countTwo = 0;

                if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) != 2 )

                    {
                    // Blocked contacts are hidden or removed, do not include to total count.
                    // Removed is also handled here because user may log in with different clients
                    // that handle blocked contacts differently.
                    countTwo  = list->FilteredCount( TStorageManagerGlobals::EFilterNonBlocked );
                    }
                else
                    {
                    // Blocked contacs are shown, Count() returns correct value
                    countTwo = list->Count();
                    }



                CArrayFix<TInt>* intArray = new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
                CleanupStack::PushL( intArray );

                // Append number of online contacts
                intArray->AppendL( countOne );
                intArray->AppendL( countTwo );

                CDesCArray* array = new( ELeave ) CDesCArrayFlat( 1 ); // one items
                CleanupStack::PushL( array );
                array->AppendL( KNullDesC );

                HBufC* textNumbers = StringLoader::LoadLC( R_QTN_CHAT_CONTACTLIST_CONTACTLIST,
                                                           *array, *intArray );
                TPtr textNumbersPtr( textNumbers->Des() );

                TPtrC identification ( list->DisplayName() );

                array->Reset();

                array->AppendL( identification.Left( dataPtr.MaxLength() -
                                                     iMaxLengthOfIcons - textNumbersPtr.Length() ) );

                HBufC* listString = StringLoader::LoadLC( R_QTN_CHAT_CONTACTLIST_CONTACTLIST,
                                                          *array, *intArray );
                TPtr listStringPtr( listString->Des() );

                dataPtr.Append( listStringPtr );

                //Code scanner warning ignored
                CleanupStack::PopAndDestroy( 4, intArray ); //array,textNumbers,listString,intArray

                }

            // so append both and append the string to array...
            contactListItem->SetItemNameText( dataPtr );

            contactListItem->SetItemEngineIndex( aIndex );

            //append this to the array
            if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
                {
                TInt arrayIndex =  aIndex - iSelectedListIndex ;
                iMainViewListItemArray.Insert( contactListItem, arrayIndex );
                }
            else
                {
                iMainViewListItemArray.Insert( contactListItem,
                                               aIndex );
                }
            CleanupStack::Pop();// contactListItem
            break;
            }
        case MCAContactListModel::EContactItem:
            {
            // contact item
            // add this items to contact item -- CONTACT
            // create an object of this type and append this to the rpointerarray
            CCAContactItemPC* contactItem = CCAContactItemPC::NewL();
            MCAStoredContact* contact = item.iContact;

            // so append the name string to this...

            contactItem->SetItemNameText( contact->Identification() );
            contactItem->SetItemEngineIndex( aIndex );


            //append this to the array
            if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
                {
                TInt arrayIndex =  aIndex - iSelectedListIndex ;
                iMainViewListItemArray.Insert( contactItem, arrayIndex );
                }
            else
                {
                iMainViewListItemArray.Insert( contactItem, aIndex );
                }

            break;
            }
        default:
            {
            // the call shouldn't be here
            break;
            }
        }
    // set font if we have listbox data
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillOwnDataL
// FillOwnDataL implementation
// -----------------------------------------------------------------------------
// fill the owndata at the begining of array.....
TBool CCAMainViewArrayPC::FillOwnDataL()
    {
    TBool ownDataAdded = EFalse;
    TBool isLoggedIn( EFalse );

    // Check whether the user has logged in before, if has
    // then dont update anything just return

    TPtrC identification( KNullDesC );
    TPtrC loggedInUid( KNullDesC );


    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();


    TRAPD( err, isLoggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) )

    if ( err )
        {
        isLoggedIn = EFalse;
        //Code scanner warning ignored
        CleanupStack::PopAndDestroy();
        return ownDataAdded;
        }
    // if there are no servers then it just returns

    if ( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) == 0 )
        {
        //Code scanner warning ignored
        CleanupStack::PopAndDestroy();  // sessionSlotId
        iContactListIndex = 0;
        return ownDataAdded;
        }

    if ( !isLoggedIn )
        {
        //
        // if i logged in and logged out it should show the last logged id
        if ( iLastloggedinUserId )
            {
            identification.Set( CCAPCUtils::DisplayId( iLastloggedinUserId->Des() ) );
            }
        else
            {
            //the default sap settings have to be read when the apllication opens
            iSAPSettingsStore->GetDefaultL( sap, EIMPSIMAccessGroup );

            CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::CurrentSAPLC sap uid %d" ),
                     sap->SAPUserId() );
            loggedInUid.Set( sap->SAPUserId() );
            if ( loggedInUid.Length() == 0 )
                {
                CleanupStack::PopAndDestroy();
                iContactListIndex = 0;
                return ownDataAdded;
                }
            identification.Set( CCAPCUtils::DisplayId( loggedInUid ) );
            }
        }

    else
        {
        MCAContactListModel::SItem item = iContactListModel.Item( iOwnDataIndex );
        if ( item.iType == MCAContactListModel::EOwnStatus )
            {
            MCAStoredContact* contact = item.iContact;
            __CHAT_ASSERT_DEBUG( contact );
            identification.Set( contact->Identification() );
            if ( iLastloggedinUserId )
                {
                delete iLastloggedinUserId;
                iLastloggedinUserId = NULL;
                }
            iLastloggedinUserId = ( contact->Identification() ).AllocL();
            }
        // This is required as the engine's contactlistmodel
        // is not yet initialized immediately after login.
        // so get the identification from the connection UI.
        else
            {
            CPEngNWSessionSlotID2* sessionSlotId = NULL;
            sessionSlotId = iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotId );
            iConnectionUI->GetLoggedInSapL( *sessionSlotId, *sap );
            identification.Set( CCAPCUtils::DisplayId( sap->SAPUserId() ) );
            CleanupStack::PopAndDestroy( sessionSlotId );
            }
        }

    // add this items to contact item -- OWNDATA
    // create an object of this type and append this to the rpointerarray

    //create the object here
    CCAOwnDataItemPC* ownDataItem = CCAOwnDataItemPC::NewL();

    // so append the name string to this...
    ownDataItem->SetItemNameText( identification );


    ownDataItem->SetItemEngineIndex( iOwnDataIndex );

    //append this to the array
    iMainViewListItemArray.Insert( ownDataItem, iOwnDataIndex );
    // if this insertion is sucess then only insert open chats folder @ 2nd position..

    ownDataAdded = ETrue;

    //Code scanner warning ignored
    CleanupStack::PopAndDestroy(); //  sap

    return ownDataAdded;
    }

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
//-----------------------------------------------------------------------------
// CCAMainViewArrayPC::ChatGroupMessagesPendingCount
// This method is a tweaking of the engine code to return the correct number of
// pending count per user (ie.., all the unread msgs for a single user is equal to one
// pending count.
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMainViewArrayPC::ChatGroupMessagesPendingCount() const
    {
    TInt count = iChatInterface.GroupCount();
    TInt pendingCount = 0;

    for ( TInt i = 0; i < count; ++i )
        {
        TInt unreadCount = iChatInterface.GroupAt( i ).UnreadCount(
                               MCAMessagesReadInterface::EUnreadReceived );
        if ( unreadCount > 0 )
            {
            ++pendingCount;
            }
        }
    return pendingCount;
    }
#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::RefreshOpenChatsListItemL
// FillOwnDataL implementation
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::RefreshOpenChatsListItemL()
    {
    // iOpenChatsArray[iOpenChatsIndex -1] make sure that
    // we dont pass negative values.


    if ( iOpenChatsIndex <= 0 )
        {
        return;
        }
    CCAOpenChatsListItemPC* openchatsListItem
    = dynamic_cast<CCAOpenChatsListItemPC*> (
          iOpenChatsArray[iOpenChatsIndex -1] );

    HBufC* textOpenChats = StringLoader::LoadLC(
                               R_QTN_CHAT_CONTACTLIST_CHATS );
    TPtr openChatsPtr( textOpenChats->Des() );

    // so append the name string to this...
    openchatsListItem->SetItemNameText( openChatsPtr );

    CleanupStack::PopAndDestroy( textOpenChats ); // textOpenChats


    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FillOpenChatsL
// FillOwnDataL implementation
// -----------------------------------------------------------------------------
// fill the owndata at the begining of array.....
void CCAMainViewArrayPC::FillOpenChatsL()
    {
    //Populates the list with currently unhandled invitations
    TInt chatIndex( 0 );
    CCAOpenChatsListItemPC* openchatsListItem = CCAOpenChatsListItemPC::NewL();
    //append this to the array and increase to next item here
    iOpenChatsArray.Insert( openchatsListItem, chatIndex++ );
    openchatsListItem->SetItemEngineIndex( 0/*KInitIndex*/ );



    RefreshOpenChatsListItemL();

    TInt groupCount( 0 );
    if ( !iGroupCountDown )
        {
        groupCount = 0;
        }
    else
        {
        groupCount = iGroupArray->MdcaCount();
        }

    // if openchats is expanded from user or forced expanded.
    // if openchats is expanded from user or forced expanded...
    if ( !( iOpenChatListItemExpanded || iExpandAll ) )
        {
        iContactListIndex = 1;
        return;
        }

    for ( ; chatIndex < iChatInterface.ChatCount() ; chatIndex++ )
        {
        // check the order in ascending order or not....

        TArrayPairs pair;
        pair.iReadInterface = &iChatInterface.ChatAt( chatIndex );

        const TDesC& userId ( pair.iReadInterface->TargetId().Target() );

        TPtrC identification( iContactInterface->Identification( userId ) );
        pair.iIdentification.Copy( identification.Left(
                                       pair.iIdentification.MaxLength() ) );

        CCAConversationItemPC* conversationItem = CCAConversationItemPC::NewL( userId );

        // so append both and append the string to array...
        conversationItem->SetItemNameText( identification );

        conversationItem->SetItemEngineIndex( 0 );

        //append this to the array
        iOpenChatsArray.Insert( conversationItem, chatIndex );

        }

    iOpenChatsInActiveIndex = iOpenChatsArray.Count() - 1;



    for ( TInt index( 0 ); index < groupCount ; index++, chatIndex++ )
        {

        CCAGroupItemPC* groupItem = CCAGroupItemPC::NewL(
                                        iChatGroups->FindGroup( iGroupArray->Array()[index]->GroupId() )->GroupId() );

        groupItem->SetItemNameText( iGroupArray->MdcaPoint( index ) );
        groupItem->SetItemEngineIndex( 0 );
        iOpenChatsArray.Insert( groupItem, chatIndex );
        }
    /*iContactListIndex = iChatInterface.ChatCount() + iInvitationsArray.Count()
                        + groupCount  + 1; // OwnData,Open Chats,chat list if any*/
    iContactListIndex = iOpenChatsArray.Count();
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::HandleContactDelete
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::HandleContactDelete( const TDesC& aContactId )
    {
    CHAT_DP_TXT( "CCAMainViewArrayPC::HandleDelete" );
    if ( iSkipStorageEvents )
        {
        return;
        }
    RefreshConversationItemText( aContactId );
    TRAP_IGNORE ( RefreshInvitationItemTextL( aContactId ) );
    ResetArray( EFalse );
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleDelete( aContactId, TEnumsPC::EContactItem );
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetSkipStorageEvents
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::HandleError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCAMainViewArrayPC::HandleError, aError = %d" ), aError );
    if ( ( aError > Imps_ERROR_BASE ) && ( aError < KErrNone ) )
        {
        // propagate system errors to current active scheduler,
        // it should show a note
        CActiveScheduler::Current()->Error( aError );
        }

    // imps errors are ignored at the moment as we don't have any
    // notes specified for them
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::HandleAddition
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::HandleAddition( MCAContactList& aList, MCAStoredContact& aContact )
    {
    if ( iSkipStorageEvents )
        {
        return;
        }
    aList.SetCollapsed( EFalse );
    RefreshConversationItemText( aContact.UserId() );
    TRAP_IGNORE ( RefreshInvitationItemTextL( aContact.UserId() ) );
    TInt indexofcontact = iContactListModel.IndexOfContact( &aContact );
    ResetArray( EFalse );
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleAddition( TEnumsPC::EContactItem,
                                                   indexofcontact + iContactListIndex );
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::HandleChange
// MCAStoredContactsObserver implementation
// Codescanner warning of calling leaving functions inside non-leaving function
// HandleChange is ignored , since we are inheritting the interface MCStoredContactsObserver
// defined on the engine component
// -----------------------------------------------------------------------------

void CCAMainViewArrayPC::HandleChange( MCAContactList* aList, MCAStoredContact* /*aContact*/,
                                       TStorageManagerGlobals::TCAObserverEventType aEventType,
                                       TBool /*aUserIdChanged */ )
    {
    // we need to handle the change here..from storage manager...
    CHAT_DP_FUNC_ENTER( "HandleChange" );


    MCAContactLists* listInterface = NULL;
    TRAP_IGNORE( listInterface = CAPresenceManager::InstanceL()->ContactLists() );

    if ( listInterface )
        {
        TBool isSyncDone = listInterface->IsSyncDone();
        TBool ignoreFailed;
        MCAStoredContacts* contacts = NULL;
        TRAPD( error, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );

        if ( error != KErrNone )
            {
            return;
            }

        if ( contacts )
            {
            if ( contacts->IsAllSynchronised( ignoreFailed ) && isSyncDone && !iForceExpandDone )
                {
                NotifySyncCompleted();
                //check if there only 1 list or not.......
                if ( contacts->ListCount() == 1 )
                    {
                    // now forcefully expand....the list..
                    // get the correct list count....
                    TInt contactListIndex = KInitIndex;
                    if ( iSelectionMode == TEnumsPC::ESingleListViewSelect )
                        {
                        contactListIndex = iOwnDataIndex + 1; // it's 1 if there is only list....
                        }
                    else
                        {
                        contactListIndex = iContactListIndex; // should be zero
                        }
                    MCAContactListModel::SItem item = iContactListModel.Item( contactListIndex );

                    if ( item.iType == MCAContactListModel::EContactList )
                        {
                        MCAContactList* list = item.iContactList;
                        if ( list )
                            {

                            list->SetCollapsed( EFalse );
                            iForceExpandDone = ETrue;
                            }
                        }
                    }
                }
            }
        }

    TEnumsPC::TChange eventtype = TEnumsPC::EUnknownEvent;
    if ( iSkipStorageEvents )
        {
        return;
        }

    TInt error( KErrNone );
    switch ( aEventType )
        {
        case TStorageManagerGlobals::EStorageEventPreChange:
            {
            /*CHAT_DP_TXT("CCAMainViewArrayPC::HandleChange ignoring pre-change event");
            TEnumsPC::TItem itemType = ( aList && aContact == NULL)?
                                               TEnumsPC::EContactListItem:
                                                          TEnumsPC::EContactItem;
            TInt indexoflist
            if( TEnumsPC::EContactListItem ==  itemType )
                {
                indexoflist = iContactListModel.IndexOfList(aList,EFalse,ETrue);
                }
            else if( TEnumsPC::EContactItem ==  itemType  )
                {
                indexoflist = iContactListModel.IndexOfContact( aContact )
                }

            for (TInt index=0; index<iMainViewObservers.Count(); index++)
            	{
            	iMainViewObservers[index]->HandleChange( itemType
               	 			         iContactListIndex + indexoflist,
               	 			         TEnumsPC::EPreChange );
            	}
            */
            // ignore the pre-change events
            return;
            }
        case TStorageManagerGlobals::EStorageEventListDelete:
        case TStorageManagerGlobals::EStorageEventMultipleChanges:
            {
            // When logged out, items are removed from list. So for to be sure
            // call handleItemRemoval.
            ResetArray( EFalse );
            TInt indexoflist = iContactListModel.IndexOfList( aList, EFalse, ETrue );
            eventtype = aEventType == TStorageManagerGlobals::EStorageEventListDelete
                        ? TEnumsPC::EDelete : TEnumsPC::EMultipleChanges;
            for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
                {
                iMainViewObservers[index]->HandleChange( TEnumsPC::EContactListItem,
                                                         iContactListIndex + indexoflist, eventtype );
                }

            break;
            }
        case TStorageManagerGlobals::EStorageEventPostChange:
            {
            // there might be new or removed contacts
            ResetArray( EFalse );


            // call back the UI with HandleChange here....
            TInt indexoflist = iContactListModel.IndexOfList( aList, EFalse, ETrue );
            for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
                {
                iMainViewObservers[index]->HandleChange( TEnumsPC::EContactItem,
                                                         iContactListIndex + indexoflist, TEnumsPC::EChanged );
                }
            break;
            }
        case TStorageManagerGlobals::EStorageEventListAddition:
        case TStorageManagerGlobals::EStorageEventListChanged:
            {
            if ( aEventType == TStorageManagerGlobals::EStorageEventListAddition )
                {

                // call back the UI with HandleChange here....

                iForceExpandDone = EFalse;
                }

            ResetArray( EFalse );

            TInt indexoflist = iContactListModel.IndexOfList( aList, EFalse, ETrue );
            eventtype = aEventType == TStorageManagerGlobals::EStorageEventListAddition
                        ? TEnumsPC::EAddition : TEnumsPC::EChanged;
            for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
                {
                iMainViewObservers[index]->HandleChange( TEnumsPC::EContactListItem,
                                                         iContactListIndex + indexoflist, eventtype );
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }

    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleWatcherEvent
// @see MCAWatcherObserver
// MCAwatcherobserver events..
// ---------------------------------------------------------

void CCAMainViewArrayPC::HandleWatcherEvent( const MCAStoredContact* aContact )
    {
    if ( !aContact  || ( aContact->OnlineStatus() == TStorageManagerGlobals::EUnknown ) )
        {
        return;
        }

    // now call back UI....with onlinestatus......
    // first parameter is text and second parameter is online status...
    for ( TInt observer( 0 ) ; observer < iMainViewObservers.Count(); observer++ )
        {
        iMainViewObservers[observer]->HandleWatcherEvent( aContact->Identification(),
                                                          GetOnlineStatusConverter( aContact->OnlineStatus() ) );
        }
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleWatcherEvent
// @see MCASettingsObserver
// ---------------------------------------------------------

void CCAMainViewArrayPC::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {

    ResetArray( EFalse );
    TInt count = iMainViewObservers.Count();
    for ( TInt obs = 0; obs < count; obs++  )
        {
        TEnumsPC::TCASettingValues settings = ConvertToEnumsPC( aChangedSettingEnum );

        iMainViewObservers[obs]->HandleSettingsChangeL( settings );
        }

    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetResourceValueL
// remove this from here and place it in common utils dll.
// ---------------------------------------------------------

TInt CCAMainViewArrayPC::GetResourceValueL( TInt aResourceID )
    {
    TInt value( KErrNone );
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceID );
    value = ResourceUtils::ReadTInt32L( reader );
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); // reader

    return value;
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::AddObserverL
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------

void CCAMainViewArrayPC::AddObserverL( MCAMainViewObserverPC* aObserver )
    {

    ASSERT( aObserver );

    TInt status( iMainViewObservers.Find( aObserver ) );
    if ( status == KErrNotFound )
        {
        status = iMainViewObservers.Append( aObserver ) ;
        User::LeaveIfError( status );
        }
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::RemoveObserver
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------

void CCAMainViewArrayPC::RemoveObserver( MCAMainViewObserverPC* aObserver )
    {

    ASSERT( aObserver );

    const TInt status( iMainViewObservers.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iMainViewObservers.Remove( status );
        iMainViewObservers.Compress();
        }
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::AddObserverL
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------

void CCAMainViewArrayPC::AddObserver( MCASyncObserverPC* aObserver )
    {
    ASSERT( aObserver );
    iSyncObserver = aObserver;
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::RemoveObserver
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------

void CCAMainViewArrayPC::RemoveObserver( )
    {
    if ( iSyncObserver )
        {
        iSyncObserver = NULL;
        }
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedContactAlias
// ---------------------------------------------------------

const TDesC& CCAMainViewArrayPC::GetSelectedContactAlias( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );
    MCAContactListModel::SItem item;
    if ( itemtype == TEnumsPC::EContactItem  )
        {
        item = iContactListModel.Item( index );
        MCAStoredContact* contact = item.iContact;
        return contact->Alias();
        }
    return KNullDesC;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedContactNickname
// ---------------------------------------------------------

const TDesC& CCAMainViewArrayPC::GetSelectedContactNickname( TInt aIndex )
    {

    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item;
    if ( itemtype == TEnumsPC::EContactItem  )
        {
        item = iContactListModel.Item( index );
        MCAStoredContact* contact = item.iContact;
        return contact->Nickname();
        }
    return KNullDesC;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedContactUserId
// ---------------------------------------------------------

const TDesC& CCAMainViewArrayPC::GetSelectedContactUserId( TInt aIndex )
    {
    TEnumsPC::TItem itemtype = GetType( aIndex );
    TInt index = GetItemEngineIndex( aIndex );
    switch ( itemtype )
        {
        case TEnumsPC::EContactItem:
            {
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAStoredContact* contact = item.iContact;
            return contact->UserId();
            }

        case TEnumsPC::EConversationItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
              * inside the GetType()
             * method*/
            TInt chatIndex = aIndex - ( iOpenChatsIndex );
            CCAConversationItemPC* conversationItem =
                dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[ chatIndex ] );
            return conversationItem->UserId();
            }
        case TEnumsPC::EInviteItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            TInt chatIndex = aIndex - ( iOpenChatsIndex );
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ chatIndex ] );
            return inviteItem->UserId();
            }
        case TEnumsPC::EGroupItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            TInt chatIndex = aIndex - ( iOpenChatsIndex );
            CCAGroupItemPC* groupItem =
                dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[ chatIndex ] );
            return groupItem->GroupId();
            }
        default:
            break;
        }

    return KNullDesC;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedListId
// ---------------------------------------------------------
const TDesC& CCAMainViewArrayPC::GetSelectedListId( TInt aIndex )
    {
    TEnumsPC::TItem itemType = GetType( aIndex );

    TInt index = GetItemEngineIndex( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAContactList* list = NULL;

    switch ( itemType )
        {
        case TEnumsPC::EContactItem:
            {
            list = item.iContactList;
            break;
            }

        case TEnumsPC::EContactListItem:
            {
            list = item.iContactList;
            break;
            }
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EGroupItem:
        case TEnumsPC::EOpenChatsListItem:
            {

            break;
            }

        default:
            break;

        }

    if ( list )
        {
        return list->ListId();
        }
    return KNullDesC;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedContactIdentification
// ---------------------------------------------------------
const TDesC& CCAMainViewArrayPC::GetSelectedContactIdentification( TInt aIndex )
    {
    TEnumsPC::TItem itemtype = GetType( aIndex );

    TInt index = GetItemEngineIndex( aIndex );
    switch ( itemtype )
        {
        case TEnumsPC::EContactItem:
        case TEnumsPC::EOwnStatusItem:
            {
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAStoredContact* contact = item.iContact;
            return contact->Identification();
            }
        /*case TEnumsPC::EContactListItem:
        		{
        		MCAContactListModel::SItem item = iContactListModel.Item( index );
        		MCAContactList& list = *item.iContactList;
        		list
        		}*/
        case TEnumsPC::EConversationItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAConversationItemPC* conversationItem =
                dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[ index ] );
            return conversationItem->Identification();
            }
        case TEnumsPC::EInviteItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ index] );
            return inviteItem->Identification();
            }
        case TEnumsPC::EGroupItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAGroupItemPC* groupItem =
                dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[ index] );
            return groupItem->Identification();
            }
        default:
            break;
        }

    return KNullDesC;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::IgnoreOfflineStatusL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::IgnoreOfflineStatusL( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );
    if ( itemtype != TEnumsPC::EOwnStatusItem && itemtype != TEnumsPC::EContactItem )
        {
        return TEnumsPC::EUnknown;
        }

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( !contact )
        {
        User::Leave( KErrNotFound );
        }
    TStorageManagerGlobals::TPresenceStatus onlineStatus(
        contact->OnlineStatus() );
    TBool contactOnline( onlineStatus == TStorageManagerGlobals::EOnline ||
                         onlineStatus == TStorageManagerGlobals::EAway ||
                         onlineStatus == TStorageManagerGlobals::EBusy );

    if ( contactOnline )
        {
        return TEnumsPC::EOnline;
        }

    return TEnumsPC::EUnknown;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::MessageReadInterfaceL
//-----------------------------------------------------------
void CCAMainViewArrayPC::SetMessageReadInterfaceL( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );
    TEnumsPC::TItem itemtype = GetType( aIndex );
    if ( itemtype != TEnumsPC::EOwnStatusItem && itemtype != TEnumsPC::EContactItem )
        {
        return;
        }
    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;

    if ( !contact )
        {
        User::Leave( KErrNotFound );
        }

    TPtrC userId( contact->UserId() );

    CHAT_DP( D_CHAT_LIT( " Trying to find chat data for %S" ), &userId );

    iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC, userId );
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::ChatExistsL
//-----------------------------------------------------------
TBool CCAMainViewArrayPC::ChatExistsL( TInt aIndex )
    {
    TPtrC userId( KNullDesC() );

    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    switch ( itemtype )
        {
        case TEnumsPC::EConversationItem:
            {
            /*if (aIndex - iOpenChatsIndex >= iChatInterface.ChatCount())
            	{
            	User::Leave( KErrNotFound );
            	}*/
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAConversationItemPC* conversationItem =
                dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[ index] );
            userId.Set( conversationItem->UserId() );
            break;
            }
        case TEnumsPC::EContactItem:
            {
            MCAContactListModel::SItem item = iContactListModel.Item( index );

            MCAStoredContact* contact = item.iContact;

            if ( !contact )
                {
                User::Leave( KErrNotFound );
                }

            userId.Set( contact->UserId() );

            break;
            }
        default:
            break;
        }

    CHAT_DP( D_CHAT_LIT( " Trying to find chat data for %S" ), &userId );

    MCAMessagesReadInterface* messageReadInterface
    =  iChatInterface.ChatExistsL( KNullDesC, KNullDesC, userId );

    if ( !messageReadInterface )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::IndexOfList
//-----------------------------------------------------------

TInt CCAMainViewArrayPC::IndexOfList( TInt aIndex, TBool aIgnoreOwnItem,
                                      TBool aIgnoreEmptyLists ) const
    {
    TInt curIndex = 0;


    TEnumsPC::TItem itemtype = GetType( aIndex );

    switch ( itemtype )
        {
        case TEnumsPC::EContactItem:
            {
            TInt index( 0 );
            if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
                {
                index =   aIndex <= 0 ? 0 : ( aIndex  + iSelectedListIndex ) ;
                }
            else
                {
                index = GetItemEngineIndex( aIndex );

                }
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAContactList& list = *item.iContactList;
            curIndex = iContactListModel.IndexOfList( &list, aIgnoreOwnItem, aIgnoreEmptyLists )
                       + iContactListIndex - iSelectedListIndex; // 1 for open chats folder
            if ( curIndex < 0 || curIndex >= Count() )
                {
                return 0;
                }
            break;
            }	// return the curindex of contac list items
        case TEnumsPC::EContactListItem:
            {
            return aIndex; // this is index of contact list
            }
        case TEnumsPC::EOpenChatsListItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EGroupItem:
            {
            curIndex = iOpenChatsIndex; // this is index of open chats list
            // return the index of open chats from here..
            break;
            }

        default:
            {
            break;
            }
        }
    return curIndex;
    }
//-----------------------------------------------------------
//CCAMainViewArrayPC:: FindContactListIndexL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::FindContactListIndexL( const TDesC& aListId )
    {
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    // check the display id's of local lists
    MCAContactList* contactList = contacts->FindContactList( aListId );

    // now get the index of this list...
    return iContactListIndex + iContactListModel.IndexOfList( contactList ); // return the index of list..
    }
//-----------------------------------------------------------
//CCAMainViewArrayPC::IsSyncDoneL
//-----------------------------------------------------------

TBool CCAMainViewArrayPC::IsSyncDoneL()
    {
    MCAContactLists* listInterface =
        CAPresenceManager::InstanceL()->ContactLists();

    return listInterface->IsSyncDone();
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::CreateContactListL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::CreateContactListL( const TDesC& aDisplayName , TInt& aIndex )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();
    TInt err = contactLists->CreateContactListL( aDisplayName );

    if ( err != KErrNone )
        {
        return err;
        }
    else
        {
        MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

        contacts->SortContactLists();

        TInt listCount = contacts->ListCount();

        for ( TInt index( 0 ); index < listCount; ++index )
            {
            MCAContactList& contactList = contacts->ListAt( index ) ;
            if ( contactList.DisplayName() == aDisplayName )
                {
                aIndex = FindContactListIndexL( contactList.ListId() );
                break;
                }

            }
        }
    return err;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::ListAlreadyExistsL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::ListAlreadyExistsL( const TPtr aNamePtr )
    {
    // check the display id's of local lists
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    if ( !contacts )
        {
        return KErrUnknown;
        }
    TInt count( contacts->ListCount() );
    TInt err( KErrNone );

    // get the standard method
    TCollationMethod m = *Mem::CollationMethodByIndex( 0 );
    // dont ignore punctuation and spaces
    m.iFlags |= TCollationMethod::EIgnoreNone;

    for ( TInt i( 0 ); i < count && err == KErrNone; ++i )
        {

        TPtrC displayName = contacts->ListAt( i ).DisplayName();
        if ( displayName.CompareC( aNamePtr, KCollationLevel, &m ) == 0 )
            {
            // list with this name already exists
            err = KErrAlreadyExists;
            }
        }
    return err;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::ContactListsExistL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::ContactListsExistL()
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();

    if ( !contactLists )
        {
        return KErrGeneral;
        }
    else
        {
        return KErrNone;
        }
    }
//-----------------------------------------------------------
//CCAMainViewArrayPC::CountOfContactsInList
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::CountOfContactsInList( TInt aIndex )
    {
    TInt count = 0;
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    switch ( itemtype )
        {
        case TEnumsPC::EContactListItem:
            {

            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAContactList& list = *item.iContactList;
            count = list.Count();
            break;
            }	// return the curindex of contac list items..
        case TEnumsPC::EOpenChatsListItem:
            {
            count = iOpenChatsArray.Count() - 1;

            // return the index of open chats from here..
            break;
            }
        default:
            {
            break;
            }
        }

    return count;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::ListCountL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::ListCountL() const
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    TInt count( contacts->ListCount() );

    return count;
    }

//-----------------------------------------------------------
//MCAMainViewArrayPC::DeleteContactListL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::DeleteContactListL( TInt aIndex )
    {
    TInt err = KErrNone;

    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();
    if ( !contactLists )
        {
        return KErrGeneral;
        }

    TInt index = GetItemEngineIndex( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAContactList& list = *item.iContactList;

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
    TBool failedList = list.Synchronised() != MCAContactList::ESynchroniseDone;
#endif // IMPS_CONTACT_FETCH_BACKGROUND   

    err = contactLists->DeleteContactListL( list.ListId()
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                                            , failedList
#endif
                                          );

    //This code may not be needed if the server contact deletion
    //will do a handledelete callback. Then it will do a refresharray entirely.
    if ( err == KErrNone )
        {
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
        // need to check in  IMPS_CONTACT_FETCH_BACKGROUND disabled state
        ResetArray( EFalse );

#endif
        }
    return err;

    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::DeleteServerContactL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::DeleteServerContactL( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );
    TInt err = KErrGeneral;
    if ( itemtype == TEnumsPC::EContactListItem || itemtype == TEnumsPC::EContactItem )
        {
        MCAPresence* presence = CAPresenceManager::InstanceL();
        MCAContactLists* contactLists = presence->ContactLists();

        if ( !contactLists )
            {
            return KErrGeneral;
            }

        MCAContactListModel::SItem item = iContactListModel.Item( index );
        MCAStoredContact& contact = *item.iContact;
        MCAContactList& list = *item.iContactList;

        TRAP( err, contactLists->DeleteServerContactL( list.ListId(),
                                                       contact.UserId() ) );
        }

    return err;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::FindContactL
//-----------------------------------------------------------
TInt CCAMainViewArrayPC::FindContactL( const TDesC& aContactListId,
                                       const TDesC& aId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( contacts )
        {
        MCAStoredContact* newContact = contacts->FindContact( aContactListId, aId );
        if ( newContact )
            {
            // return the index of contact rather bool value..
            return ( iContactListIndex + iContactListModel.IndexOfContact( newContact ) );
            }

        }
    return KErrNotFound;
    }


//-----------------------------------------------------------
//CCAMainViewArrayPC::GetContactForContactEditor
//-----------------------------------------------------------
//
MCAStoredContact* CCAMainViewArrayPC::GetContactForContactEditor( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );
    if ( itemtype == TEnumsPC::EOwnStatusItem || itemtype == TEnumsPC::EContactItem )
        {

        MCAStoredContact* contact = NULL;
        MCAContactListModel::SItem item = iContactListModel.Item( index );
        if ( ( item.iType == MCAContactListModel::EContactItem ) ||
             ( item.iType == MCAContactListModel::EOwnStatus ) )
            {
            contact = item.iContact;
            }
        return contact;
        }
    return NULL;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::StatusText
//-----------------------------------------------------------
//
const TDesC& CCAMainViewArrayPC::StatusText( TInt aIndex ) const
    {
    TInt index = GetItemEngineIndex( aIndex );
    // get correct index of this..

    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact* contact = item.iContact;
    if ( contact )
        {
        return contact->StatusText();
        }
    else
        {
        return KNullDesC();
        }
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::MapContactListPositions
//-----------------------------------------------------------
//
void CCAMainViewArrayPC::MapContactListPositions( RArray<TInt>& aPos )
    {
    // add conversation also this list ..and return..
    RArray<TInt> listPos;

    iContactListModel.MapContactListPositions( listPos );
    TInt positionIndex = 0;
    // if count is more than 0..fill the maparray...
    if ( iMainViewListItemArray.Count()  + iContactListIndex > 0 )
        {
        // we've some openchats items.. so add them to the list..
        if ( iContactListIndex > 0 && SingleListViewSelectMode() )
            {
            // open chats index(value), position in array
            aPos.Insert( iOpenChatsIndex, positionIndex );
            positionIndex++;	 // increase the index, as we added the open chats folder..
            }
        TInt count = listPos.Count();

        for ( TInt posIndex = 0; posIndex < count; posIndex++, positionIndex++ )
            {
            aPos.Append( listPos[posIndex] + iContactListIndex );
            }
        }
    listPos.Close();
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::DisplayName
//-----------------------------------------------------------
//
TPtrC CCAMainViewArrayPC::DisplayName( TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );


    switch ( itemtype )
        {
        case TEnumsPC::EContactListItem:
            {
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAContactList* list = item.iContactList;
            if ( list )
                {
                return list->DisplayName();
                }
            }
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EGroupItem:
        case TEnumsPC::EOpenChatsListItem:
            {
            break;
            }
        }
    return KNullDesC();
    }


//-----------------------------------------------------------
//CCAMainViewArrayPC::SetDisplayName
//-----------------------------------------------------------
//
TInt CCAMainViewArrayPC::SetDisplayNameL( TInt aIndex, const TPtrC aItemName )
    {
    //CURRENTLY TO RENAME A LIST ITEM
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();

    TInt index = GetItemEngineIndex( aIndex );

    TEnumsPC::TItem itemtype = GetType( aIndex );

    MCAContactListModel::SItem item = iContactListModel.Item( index );

    TInt err = KErrNone;
    if ( item.iContactList )
        {
        MCAContactList& list = *item.iContactList;
        err = contactLists->SetDisplayNameL( list.ListId(), aItemName ) ;
        return err;
        }
    return KErrGeneral;
    }

//-----------------------------------------------------------
//CCAMainViewArrayPC::FetchContactsL
//-----------------------------------------------------------
//
void CCAMainViewArrayPC::FetchContactsL()
    {
    CAPresenceManager::InstanceL()->ContactLists()->FetchContactsL();
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::StatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCAMainViewArrayPC::StatusL()
    {
    MCAPresence::TPresenceStatus  presenceStatus =
        CAPresenceManager::InstanceL()->StatusL();
    return GetOwnStatusConverter( presenceStatus );
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetOwnStatusConverter
// -----------------------------------------------------------------------------

TEnumsPC::TOnlineStatus CCAMainViewArrayPC::GetOwnStatusConverter(
    MCAPresence::TPresenceStatus  aPresenceStatus )
    {
    switch ( aPresenceStatus )
        {
        case MCAPresence::EOffline:
            {
            return TEnumsPC::EOffline;
            }
        case MCAPresence::EAway:
            {
            return TEnumsPC::EAway;
            }
        case MCAPresence::EBusy:
            {
            return TEnumsPC::EBusy;
            }
        case MCAPresence::EOnline: // Flowthrough
        default:
            {
            return TEnumsPC::EOnline;
            }
        }

    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ChangeStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::ChangeStatusL( TEnumsPC::TOnlineStatus aStatus,
                                        const TDesC& aStatusMessage )
    {
    TInt error( KErrNone );

    MCAPresence* presence = CAPresenceManager::InstanceL();

    switch ( aStatus )
        {
        case TEnumsPC::EOnline:
            {
            error = presence->ChangeStatusL( MCAPresence::EOnline ,
                                             aStatusMessage );
            break;
            }
        case TEnumsPC::EAway:
            {
            error = presence->ChangeStatusL( MCAPresence::EAway ,
                                             aStatusMessage );
            break;
            }
        case TEnumsPC::EOffline:
            {
            error = presence->ChangeStatusL( MCAPresence::EOffline ,
                                             aStatusMessage );
            break;
            }
        case TEnumsPC::EBusy:
            {
            error = presence->ChangeStatusL( MCAPresence::EBusy ,
                                             aStatusMessage );
            break;
            }
        default:
            {
            error = presence->ChangeStatusL( MCAPresence::EUnknown ,
                                             aStatusMessage );
            break;
            }

        }

    return error;

    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ChangeStatusMessageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::ChangeStatusMessageL( const TDesC& aStatusMessage )

    {
    return CAPresenceManager::InstanceL()->ChangeStatusMessageL( aStatusMessage );
    }


//-----------------------------------------------------------
// CCAMainViewArrayPC::SetContactSelectionL
// used by contactselectiondialog
//-----------------------------------------------------------
//
void CCAMainViewArrayPC::SetContactSelectionL(
    TEnumsPC::TListSelectionType aSelectionMode,
    TInt aSelectedListIndex )
    {
    iSelectionMode = aSelectionMode;

    if ( iSelectionMode == TEnumsPC::ESingleListMultiSelect )
        {
        MCAStoredContacts* storedContacts =
            CCAStorageManagerFactory::ContactListInterfaceL();
        iSelectedListIndex = aSelectedListIndex <= 0 ? 0 : (
                                 aSelectedListIndex - iContactListIndex ) ;
        SetContactListIndex();
        MCAContactListModel::SItem item = iContactListModel.Item(
                                              iSelectedListIndex );

        if ( item.iType == MCAContactListModel::EContactList )
            {
            MCAContactList& list = *item.iContactList;
            if ( list.Collapsed() )
                {
                list.SetCollapsed( EFalse );
                iSetListCollapsedFlag = ETrue;
                ResetArray( EFalse );
                }
            TInt count = list.FilteredCount( GetFilterConverterUITOEngine( Filter() ) );

            for ( TInt index = iSelectedListIndex + 1 ; index <=
                  iSelectedListIndex + count ; index++ )
                {
                MCAContactListModel::SItem itemContact = iContactListModel.Item( index );
                if ( itemContact.iType == MCAContactListModel::EContactItem )
                    {
                    MCAStoredContact& contact = *itemContact.iContact;
                    contact.SetSelected( ETrue );
                    }
                }
            }

        }
    else
        {
        SetContactListIndex();
        iContactListModel.SetSelectionMode( ETrue );
        iContactListModel.SetPrimaryContactLists( EFalse );
        }

    }
//-----------------------------------------------------------
//CCAMainViewArrayPC::ResetContactSelectionL
//-----------------------------------------------------------
//
void CCAMainViewArrayPC::ResetContactSelectionL()
    {

    if ( iSetListCollapsedFlag  &&
         iSelectionMode == TEnumsPC::ESingleListMultiSelect )
        {
        SetContactListIndex();
        MCAContactListModel::SItem item =
            iContactListModel.Item( iSelectedListIndex );
        if ( item.iType == MCAContactListModel::EContactList  )
            {
            MCAContactList& list = *item.iContactList;
            list.SetCollapsed( ETrue );
            iSetListCollapsedFlag = EFalse;
            ResetArray( EFalse );
            }

        }
    iSelectedListIndex = 0;
    iSelectionMode = TEnumsPC::ESingleListViewSelect;
    SetContactListIndex();
    iContactListModel.ShowAll( EFalse );
    ClearSelectedL();
    ResetDefaultFilter();
    iContactListModel.SetSelectionMode( EFalse );
    ForceExpanded( EFalse );
    iContactListModel.SetPrimaryContactLists( ETrue );

    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ShowAll
// Sets the visibility of "all" item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::ShowAll( TBool aShow )
    {
    iShowAll = aShow;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FilteredCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::GetFilteredCount( TEnumsPC::TFilterType aFilter,
                                           TInt aIndex )
    {
    TInt count( 0 );
    TInt index = GetItemEngineIndex( aIndex ) ;

    TEnumsPC::TItem itemType = GetType( aIndex );

    switch ( itemType )
        {
        case TEnumsPC::EContactItem:
        case TEnumsPC::EContactListItem:
            {
            //  add the resloving of array index and pass it to engine
            // now get the type of index for this list...
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAContactList& list = *item.iContactList;

            // convert the filter from UI to engine and pass that filter
            count =  list.FilteredCount( GetFilterConverterUITOEngine( aFilter ) );

            break;
            }
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EGroupItem:
        case TEnumsPC::EOpenChatsListItem:
            {
            count =	iOpenChatsArray.Count() - 1;
            /*count = iChatInterface.ChatCount() + iInvitationsArray.Count()
                    + iGroupArray->MdcaCount();  */

            // return the index of open chats from here..
            break;
            }
        default:
            {
            break;
            }

        }
    return count;
    }



// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::SetSelected
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::SetSelected( TBool aSelected, TInt aIndex )
    {
    TInt index = GetItemEngineIndex( aIndex ) ;

    MCAContactListModel::SItem item = iContactListModel.Item( index );
    MCAStoredContact& contact = *item.iContact;
    contact.SetSelected( aSelected );
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetSelectedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::GetSelectedL( CDesCArray& aSelectedContact,
                                       CDesCArray* aSelectedIdentifications,
                                       TEnumsPC::TFilterType aFilter )
    {
    RPointerArray<MCAStoredContact> selectedContacts;
    CleanupClosePushL( selectedContacts );

    TStorageManagerGlobals::TFilterType filter = TStorageManagerGlobals::EFilterNotDefined;
    filter = GetFilterConverterUITOEngine( aFilter );

    MCAStoredContacts* storedContact = CCAStorageManagerFactory::ContactListInterfaceL();
    storedContact->GetSelectedL( selectedContacts, filter );

    TInt count = selectedContacts.Count();
    for ( TInt i( 0 ); i < count; ++i )
        {
        MCAStoredContact* contact = selectedContacts[i];
        aSelectedContact.AppendL( contact->UserId() );
        if ( aSelectedIdentifications )
            {
            aSelectedIdentifications->AppendL( contact->Identification() );
            }
        }
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::ClearSelectedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::ClearSelectedL( )
    {
    MCAStoredContacts* storedContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    storedContacts->ClearSelected();
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::InsertInviteItemL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::InsertInviteItemL( const MCAInvitation* aInvitation )
    {
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    const TDesC& userId = aInvitation->UserID();

    CCAInviteItemPC* inviteItem = CCAInviteItemPC::NewL( userId,
                                                         aInvitation->InviteID() );
    HBufC* inviteName = StringLoader::LoadLC(
                            R_QTN_CHAT_CONTACTLIST_INVITATION,
                            contacts->Identification( userId ) );
    inviteItem->SetItemNameText( *inviteName );

    inviteItem->SetInvitation( aInvitation );

    inviteItem->SetItemEngineIndex( 0 );
    iOpenChatsArray.Insert( inviteItem, iOpenChatsIndex );

    SetContactListIndex();
    iOpenChatsInActiveIndex++;
    CleanupStack::PopAndDestroy( inviteName );
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::InsertGroupItemL)
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::InsertGroupItemL( MCAStoredGroup* aGroup )
    {
    CCAGroupItemPC* groupItem = CCAGroupItemPC::NewL( aGroup->GroupId() );
    groupItem->SetItemNameText( aGroup->GroupName() );
    groupItem->SetItemEngineIndex( 0 );
    if ( !aGroup->IsJoined() )
        {
        iOpenChatsArray.Insert( groupItem, iOpenChatsArray.Count() );
        //Set the fresh iContactListIndex before calling InsertIntoInactiveOpenChats()
        //to sort the non joined groups. The methods inside  the sort is dependant
        //on the fresh iContactListIndex
        SetContactListIndex();
        InsertIntoInactiveOpenChats( ETrue );
        }
    else
        {
        iOpenChatsArray.Insert( groupItem, iOpenChatsIndex );
        iOpenChatsInActiveIndex++;
        SetContactListIndex();
        }
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::RefreshGroupItem()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::RefreshGroupItem( MCAStoredGroup* aGroup, TInt aFoundIndex )
    {
    /* Codescanner warning is ignored, since Bound check is done
     * inside the FindIndexOfOpenChats() method, where the foundIndex is passed*/

    CCAGroupItemPC* foundGroupItem = dynamic_cast<CCAGroupItemPC*>(
                                         iOpenChatsArray[aFoundIndex] );
    foundGroupItem->SetItemNameText( aGroup->GroupName() );
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::GetInviteItemID()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCAMainViewArrayPC::GetInviteItemID( TInt aIndex )
    {
    if ( GetType( aIndex ) != TEnumsPC::EInviteItem )
        {
        return KNullDesC;
        }
    CCAInviteItemPC* inviteItem =
        dynamic_cast< CCAInviteItemPC* > (
            iOpenChatsArray[ aIndex - iOpenChatsIndex ] );
    return inviteItem->GetInviteID();
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleInvitationEventL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleInvitationEventL(
    TInviteEventType aEvent, const MCAInvitation* aInvitation )
    {
    switch ( aEvent )
        {
        case ENewInvite:
            {
            TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();
            iItemDetails = GetArrayItemIdDetails( currentItemIndex );
            InsertInviteItemL( aInvitation );
            if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
                {
                return;
                }
            RefreshOpenChatsListItemL();
            for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
                {
                iMainViewObservers[index]->HandleChange( TEnumsPC::EOpenChatsListItem,
                                                         iOpenChatsIndex , \
                                                         TEnumsPC::EAddition );
                }

            break;
            }
        case EInviteCancel: // Flowthrough
        case EInviteExpired:
            {
            TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();
            TInt indexOfList = IndexOfList( currentItemIndex );
            TInt countOfContactsInList = CountOfContactsInList( indexOfList );

            //Find the invitation and delete.
            TInt deletedIndex = FindInvitationAndDelete( aInvitation->InviteID()  );
            if ( ( indexOfList + countOfContactsInList ) == currentItemIndex  )
                {
                --currentItemIndex;
                }
            iItemDetails = GetArrayItemIdDetails( currentItemIndex );
            //Check if singlelistview is active. If it is then give a callback.
            //else no need.
            if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
                {
                return;
                }
            RefreshOpenChatsListItemL();
            for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
                {
                iMainViewObservers[index]->HandleChange( TEnumsPC::EOpenChatsListItem,
                                                         deletedIndex, TEnumsPC::EDelete );
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
// CCAMainViewArrayPC::HandleInvitationResponse()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleInvitationResponse(
    TBool aAcceptance,
    const TDesC& aUserId,
    const TDesC& aGroupName,
    const TDesC& aResponse )
    {
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleInvitationResponse( aAcceptance,
                                                             aUserId,
                                                             aGroupName,
                                                             aResponse );
        }
    }


//-----------------------------------------------------------------------------
// CCAMainViewArrayPC::ToUpdatePresence
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAMainViewArrayPC::ToUpdatePresence( TEnumsPC::TFilterType aFilter1,
                                            TEnumsPC::TFilterType aFilter2 )
    {
    TBool updatepresence = EFalse;
    TStorageManagerGlobals::TFilterType filtertype =
        GetFilterConverterUITOEngine( aFilter2 );
    if ( TEnumsPC::EFilterNonOfflineNonBlockedAndOperation == aFilter1 )
        {
        updatepresence =  TStorageManagerGlobals::EFilterNonOffline |
                          TStorageManagerGlobals::EFilterNonBlocked +
                          TStorageManagerGlobals::EFilterAndOperation
                          & filtertype;
        }
    else if ( TEnumsPC::EFilterAllNonBlockedAndOperation == aFilter1 )
        {
        updatepresence = TStorageManagerGlobals::EFilterAll |
                         TStorageManagerGlobals::EFilterNonBlocked +
                         TStorageManagerGlobals::EFilterAndOperation
                         & filtertype;
        }
    if ( updatepresence )
        {
        return  ETrue;
        }
    else
        {
        return  EFalse;
        }
    }

//-----------------------------------------------------------------------------
// CCAMainViewArrayPC::GetItemEngineIndex
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::GetItemEngineIndex( TInt aIndex ) const
    {
    TInt itemIndex = KErrNone;
    TInt index = aIndex - iContactListIndex <= 0 ? 0 : aIndex - iContactListIndex;
    //codescanner warning ignored. as the check for aIndex is made in gettype.
    switch ( GetType( aIndex ) )
        {
        case TEnumsPC::EContactItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType
             * method*/

            CCAContactItemPC* contactItem =
                dynamic_cast< CCAContactItemPC* > ( iMainViewListItemArray[ index ] );
            itemIndex = contactItem->GetItemEngineIndex();
            break;
            }
        case TEnumsPC::EContactListItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType
             * method*/
            CCAContactListItemPC* contactListItem =
                dynamic_cast< CCAContactListItemPC* > ( iMainViewListItemArray[ index ] );
            itemIndex = contactListItem->GetItemEngineIndex();
            break;
            }
        case TEnumsPC::EOwnStatusItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType
             * method*/
            CCAOwnDataItemPC* owndataItem =
                dynamic_cast< CCAOwnDataItemPC* > ( iMainViewListItemArray[ index ] );
            itemIndex = owndataItem->GetItemEngineIndex();
            break;
            }
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EGroupItem:
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EOpenChatsListItem:
            {
            itemIndex = aIndex - iOpenChatsIndex; //openchatsarray index
            break;
            }
        default:
            {
            itemIndex = KErrNotFound;
            break;
            }
        }
    return itemIndex;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FindInvitationAndDelete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::FindInvitationAndDelete( const TDesC& aInviteID )
    {
    TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();

    TInt index = iOpenChatsIndex;
    for ( ; index < iOpenChatsArray.Count() ; index++ )
        {
        MCAArrayItemPC *arrayItemPC = ( MCAArrayItemPC * )iOpenChatsArray[index];

        if ( arrayItemPC->Type() == TEnumsPC::EInviteItem )
            {
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ index ] );

            if ( CAUtils::NeutralCompare( inviteItem->GetInviteID(), aInviteID ) == 0 )
                {
                iOpenChatsArray.Remove( index  );
                delete arrayItemPC;
                SetContactListIndex();
                iOpenChatsInActiveIndex--;
                break;
                }
            }
        }
    if ( iOpenChatsArray.Count() + 1 == currentItemIndex  )
        {
        --currentItemIndex;
        }
    iItemDetails = GetArrayItemIdDetails( currentItemIndex );
    return index;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC:::SetContactListIndex
// Based on index from single list view
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::SetContactListIndex()
    {
    if ( !SingleListViewSelectMode() )
        {
        iContactListIndex = 0;
        }
    else if ( !( iOpenChatListItemExpanded || iExpandAll ) )
        {
        iContactListIndex = 1;
        }
    else
        {
        iContactListIndex = iOpenChatsArray.Count();
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC::FindInvitationAndDelete
// Based on index from single list view
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::FindInvitationAndDelete( TInt aIndex )
    {
    TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();
    TInt index = iOpenChatsIndex;

    if ( GetType( aIndex ) == TEnumsPC::EInviteItem )
        {
        CCAInviteItemPC* inviteItem =
            dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ aIndex - iOpenChatsIndex ] );

        iOpenChatsArray.Remove( aIndex - iOpenChatsIndex );
        delete inviteItem;
        iOpenChatsInActiveIndex--;
        SetContactListIndex();
        if ( iOpenChatsArray.Count() + 1 == currentItemIndex  )
            {
            --currentItemIndex;
            }
        iItemDetails = GetArrayItemIdDetails( currentItemIndex );
        return index;
        }
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: FindAnyContactL
// -----------------------------------------------------------------------------
//
TBool CCAMainViewArrayPC::FindAnyContactL( const TDesC& aContactId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( contacts->FindAnyContact( aContactId ) )
        {
        return ETrue;
        }
    return EFalse;
    }



// ---------------------------------------------------------
// CCAMainViewArrayPC::ConvertToTCASettingStrings
// (other items were commented in a header).
// ---------------------------------------------------------
//
TEnumsPC::TCASettingValues CCAMainViewArrayPC::ConvertToEnumsPC(
    TInt aStatus )
    {
    MCASettings::TCASettingFlags engineFlags = ( MCASettings::TCASettingFlags )aStatus;

    TEnumsPC::TCASettingValues setValue = TEnumsPC::EUnknownValue;

    // Map status to setting item
    switch ( engineFlags )
        {
        case MCASettings::EDefaultScreenNameInUse :
            {
            setValue =  TEnumsPC::EDefaultScreenNameInUse;
            break;
            }
        case MCASettings::EAutomaticPresenceUpdate :
            {
            setValue =  TEnumsPC::EAutomaticPresenceUpdate;
            break;
            }

        case MCASettings::EShowOffline :
            {
            setValue =  TEnumsPC::EShowOffline;
            break;
            }
        case MCASettings::EShowTimeStamps :
            {
            setValue =  TEnumsPC::EShowTimeStamps;
            break;
            }
        default:
            {
            break;
            }
        }
    return setValue ;
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: InsertConversationItemL
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::InsertConversationItemL( const TDesC& aUserId,
                                                  const TDesC& aIdentification )
    {

    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();

    if ( contactLists && ( !contactLists->IsContactFetchingCompleted() ) )
        {
        contactLists->AddMainViewProcessObserver( this );
        iRefreshConvArray.Append( aUserId.Alloc() );
        }

    TInt index = FindIndexOfOpenChatsItem( TEnumsPC::EConversationItem, aUserId );
    if ( index != KErrNotFound )
        {
        return;
        }

    CCAConversationItemPC* conversationItem = CCAConversationItemPC::NewL( aUserId );
    // so append both and append the string to array...
    if ( aIdentification.Compare( KNullDesC ) )
        {
        //if not KNullDesC
        conversationItem->SetItemNameText( aIdentification );
        }
    else
        {
        conversationItem->SetItemNameText( iContactInterface->Identification( aUserId ) );
        }
    conversationItem->SetItemEngineIndex( 0 );
    iOpenChatsArray.Insert( conversationItem, iOpenChatsIndex );
    SetContactListIndex();
    iOpenChatsInActiveIndex++;
    RefreshOpenChatsListItemL();
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: RemoveConversationItem
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::RemoveConversationItem( const TDesC& aUserId )
    {
    TInt index = FindIndexOfOpenChatsItem( TEnumsPC::EConversationItem, aUserId );
    if ( index == KErrNotFound )
        {
        return;
        }
    //Codescanner wanring ignored sicne boundcheck is done in gettype() method
    // used in findindexofopenchats() method
    CCAConversationItemPC* convItem
    = dynamic_cast<CCAConversationItemPC*> ( iOpenChatsArray[index] );
    iOpenChatsArray.Remove( index );
    SetContactListIndex();
    delete convItem;
    iOpenChatsInActiveIndex--;

    /*since a delete and subsequent focus on the correct open chats item
    has not yet been done on the openchats array in HandleChatEvent(),
    we have to set iItemdetails correctly after closing a conversation. This
    will ensure that after the RemoveConversationItem() call in the
    conversationview, iItemDetails is pointing to the correct detail.*/

    //Subtract one, since the item is already removed from the open chats array
    //And the index of list considers the fresh openchatsarray
    TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();
    TInt indexOfList = IndexOfList( currentItemIndex - 1 );
    TInt countOfContactsInList = CountOfContactsInList( indexOfList );
    if ( indexOfList + countOfContactsInList + 1 == currentItemIndex  )
        {
        --currentItemIndex;
        }
    iItemDetails = GetArrayItemIdDetails( currentItemIndex );

    return;
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: RemoveGroupItem
// -----------------------------------------------------------------------------
//
TInt CCAMainViewArrayPC::RemoveGroupItem( const TDesC& aGroupId )
    {
    TInt openchatsindex( FindIndexOfOpenChatsItem( TEnumsPC::EGroupItem, aGroupId ) );
    if ( openchatsindex == KErrNotFound )
        {
        return KErrNotFound;
        }

    if ( openchatsindex <= iOpenChatsInActiveIndex )
        {
        iOpenChatsInActiveIndex--;
        }
    CCAGroupItemPC* groupItem = dynamic_cast <CCAGroupItemPC*>( iOpenChatsArray[openchatsindex] );
    iOpenChatsArray.Remove( openchatsindex );
    SetContactListIndex();
    delete groupItem;

    return openchatsindex;
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: ResetOpenChatsArrayL
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::ResetOpenChatsArrayL( TEnumsPC::TItem aItem )
    {
    switch ( aItem )
        {
        case TEnumsPC::EConversationItem:
            {
            //Refresh the iopenchats array
            for ( TInt index = 0; index < iChatInterface.ChatCount(); index++ )
                {
                MCAMessagesReadInterface& readInterface
                = iChatInterface.ChatAt( index );
                TInt foundIndex = FindIndexOfOpenChatsItem( TEnumsPC::EConversationItem,
                                                            readInterface.TargetId().Target() );
                const TDesC& wvId ( readInterface.TargetId().Target() );

                if ( foundIndex == KErrNotFound )
                    {
                    InsertConversationItemL( wvId, iContactInterface->Identification( wvId ) );
                    }
                }
            break;
            }
        case TEnumsPC::EGroupItem:
            {
            //Refresh the iopenchats array
            for ( TInt index = 0; index < iGroupArray->MdcaCount(); index++ )
                {
                MCAStoredGroup* group = iGroupArray->GroupAt( index );

                TInt foundIndex = FindIndexOfOpenChatsItem( TEnumsPC::EGroupItem, group->GroupId() );

                if ( foundIndex != KErrNotFound )
                    {
                    MoveGroupItem( group, foundIndex );
                    }
                else
                    {
                    InsertGroupItemL( group );
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }
// -----------------------------------------------------------------------------
// CCAMainViewArrayPC: HandleChatEvent
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::HandleChatEvent( TChatEventType aEvent,
                                          MCAMessage* aMessage )
    {
    TEnumsPC::TChange changetype = TEnumsPC::EUnknownEvent;
    TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();
    switch ( aEvent )
        {
        case MCAChatObserver::EChatListChanged:
        case MCAChatObserver::ESendListChanged:

            {
            if ( !IsCollapsed( iOpenChatsIndex ) )
                {
                if ( iPToPChatCount < iChatInterface.ChatCount() )
                    {
                    iItemDetails = GetArrayItemIdDetails( currentItemIndex );
                    changetype = TEnumsPC::EAddition;
                    iPToPChatCount = iChatInterface.ChatCount() ;
                    }
                else if ( iPToPChatCount > iChatInterface.ChatCount() )
                    {
                    //Subtract one, since the item is already removed from the open chats array
                    //And the index of list considers the fresh openchatsarray
                    TInt indexOfList = IndexOfList( currentItemIndex - 1 );
                    TInt countOfContactsInList = CountOfContactsInList(
                                                     indexOfList );
                    iPToPChatCount = iChatInterface.ChatCount() ;
                    if ( indexOfList + countOfContactsInList + 1 == currentItemIndex  )
                        {
                        --currentItemIndex;
                        }
                    iItemDetails = GetArrayItemIdDetails( currentItemIndex );
                    changetype = TEnumsPC::EDelete;
                    }
                else
                    {
                    iItemDetails = GetArrayItemIdDetails( currentItemIndex );
                    changetype = TEnumsPC::EChanged;
                    iPToPChatCount = iChatInterface.ChatCount() ;
                    }
                }
            else
                {
                iItemDetails = GetArrayItemIdDetails( currentItemIndex );
                changetype = TEnumsPC::EChanged;
                iPToPChatCount = iChatInterface.ChatCount();
                }

            if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
                {
                return;
                }
            TRAP_IGNORE( RefreshOpenChatsListItemL() );
            for ( TInt index = 0; index < iMainViewObservers.Count() ; index++ )
                {
                iMainViewObservers[index]->HandleChange(
                    TEnumsPC::EOpenChatsListItem,
                    iOpenChatsIndex, changetype );
                }
            break;
            }
        case MCAChatObserver::EUnreadCountChanged:
            {
            iItemDetails = GetArrayItemIdDetails( currentItemIndex );
            changetype = TEnumsPC::EUnReadCountChanged;
            iPToPChatCount = iChatInterface.ChatCount();

            if ( aMessage && aMessage->MessageType() == MCAMessage::EMessagePTOP
                 && aMessage->Recipient().Compare( KNullDesC ) )
                {
                TInt foundIndex = FindIndexOfOpenChatsItem(
                                      TEnumsPC::EConversationItem,
                                      aMessage->Recipient() );

                const TDesC& wvId ( aMessage->Recipient() );

                if ( foundIndex == KErrNotFound )
                    {
                    TRAP_IGNORE( InsertConversationItemL( wvId,
                                                          iContactInterface->Identification( wvId ) ) ) ;
                    changetype = TEnumsPC::EChanged;
                    }

                }

            if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
                {
                return;
                }
            TRAP_IGNORE( RefreshOpenChatsListItemL() );
            for ( TInt index = 0; index < iMainViewObservers.Count() ; index++ )
                {
                iMainViewObservers[index]->HandleChange(
                    TEnumsPC::EOpenChatsListItem,
                    iOpenChatsIndex, changetype );
                }
            break;
            }
        case MCAChatObserver::EGroupListChanged:
            {
            iItemDetails = GetArrayItemIdDetails( currentItemIndex );
            changetype = TEnumsPC::EChanged;
            iPToPChatCount = iChatInterface.ChatCount();
            iGroupArray->Sort();
            TRAP_IGNORE( ResetOpenChatsArrayL( TEnumsPC::EGroupItem ) );
            if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
                {
                return;
                }
            TRAP_IGNORE( RefreshOpenChatsListItemL() );

            for ( TInt index = 0; index < iMainViewObservers.Count() ; index++ )
                {
                iMainViewObservers[index]->HandleChange(
                    TEnumsPC::EOpenChatsListItem,
                    iOpenChatsIndex, TEnumsPC::EChanged );
                }
            break;
            }
        case MCAChatObserver::EMemoryLow:
        case MCAChatObserver::EEventNotSpecified:
        default:
            {
            //nothing to do
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMainViewArrayPC:: GetOnlineContactsL
// -----------------------------------------------------------------------------
//
void CCAMainViewArrayPC::GetOnlineContactsL( CDesCArray& aOnlineList,
                                             TBool aFetchFromNetwork )
    {
    CAPresenceManager::InstanceL()->GetOnlineFriendsL( aOnlineList, aFetchFromNetwork );
    }

// This callback is required when ever the server is added or deleted
// we get a callback and update the singlelist

// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleActiveDbEvent()
// (other items were commented in a header).
// ---------------------------------------------------------

void CCAMainViewArrayPC::HandleSAPEvent( TSAPEvent aEvent )
    {
    if ( MIMPSSAPObserver::ESAPSettingChanged == aEvent )
        {
        if ( iLastloggedinUserId )
            {
            delete iLastloggedinUserId;
            iLastloggedinUserId = NULL;
            }


        // If the iOpenChatsArray is empty, reset the array to fill it.
        if ( iOpenChatsArray.Count() < iOpenChatsIndex )
            {
            ResetArray( ETrue );
            }
        else ResetArray( EFalse );

        TInt index = 0;
        // this is always 0 for own data
        if ( iMainViewObservers.Count() )
            {
            iMainViewObservers[index]->HandleChange(
                TEnumsPC::EOwnStatusItem,
                index, TEnumsPC::EChanged );

            }
        }
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleDelete
// Group delete
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleDelete( const TDesC&  aId )
    {
    TInt currentItemIndex = iSingleViewCB->GetCurrentItemIndex();

    TInt indexOfList = IndexOfList( currentItemIndex );
    TInt countOfContactsInList = CountOfContactsInList( indexOfList );

    //find index of the list and count before deleting the item
    TInt groupIndex( iGroupArray->Find( aId ) );
    if ( groupIndex != KErrNotFound )
        {
        iGroupArray->Remove( groupIndex );
        groupIndex = RemoveGroupItem( aId );
        }
    //dont call populatelist here it wd give wrong count
    // as in leave group case that count is still not updated
    //resetarray is required as else iMainViewListItemArray would not be refreshed
    TInt cnt = iGroupArray->MdcaCount();

    if ( cnt == iOnlySavedGroups && !iGroupCountDown )
        {
        iGroupCountDown = ETrue;
        }
    else if ( !iGroupCountDown )
        {
        return;
        }

    if ( ( indexOfList + countOfContactsInList ) == currentItemIndex )
        {
        --currentItemIndex;
        }
    else if ( iChatGroups->IsGroupDeleted() &&
              TEnumsPC::EContactListItem == GetType( currentItemIndex ) )
        {

        // focus should be the chat folder then
        --currentItemIndex;
        }

    iItemDetails = GetArrayItemIdDetails( currentItemIndex );
    TEnumsPC::TChange changetype = TEnumsPC::EUnknownEvent;
    if ( !IsCollapsed( iOpenChatsIndex ) )
        {
        changetype = TEnumsPC::EDelete;
        }
    else
        {
        changetype = TEnumsPC::EChanged;
        }
    if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
        {
        return;
        }

    TRAP_IGNORE( RefreshOpenChatsListItemL() );
    //itemIndex is not being used
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleChange( TEnumsPC::EOpenChatsListItem,
                                                 groupIndex/*itemIndex*/ ,
                                                 changetype );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleAddition
// Group addition
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleAddition( const TDesC&  aId )
    {
    iItemDetails = GetArrayItemIdDetails( iSingleViewCB->GetCurrentItemIndex() );
    MCAStoredGroup* group = iChatGroups->FindGroup( aId );
    if ( group )
        {
        if ( iGroupArray->Find( aId ) != KErrNotFound )
            {
            // the group is already in the list, so let's not proceed
            return;
            }

        iGroupArray->Add( group );

        //After the item has been added to the iGroupArray only then call this.
        if ( FindIndexOfOpenChatsItem( TEnumsPC::EGroupItem, aId ) == KErrNotFound )
            {
            TRAP_IGNORE( InsertGroupItemL( group ) );
            }
        }
    TInt itemIndex = iOpenChatsArray.Count();

    TEnumsPC::TChange changetype = TEnumsPC::EUnknownEvent;
    if ( !IsCollapsed( iOpenChatsIndex ) )
        {
        changetype = TEnumsPC::EAddition;
        }
    else
        {
        changetype = TEnumsPC::EChanged;
        }
    if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
        {
        return;
        }
    TRAP_IGNORE( RefreshOpenChatsListItemL() );
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleChange( TEnumsPC::EOpenChatsListItem,
                                                 itemIndex,
                                                 changetype );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleChange
// Group Change events
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleChange( const TDesC& aId )
    {
    iItemDetails = GetArrayItemIdDetails( iSingleViewCB->GetCurrentItemIndex() );
    MCAStoredGroup* group = iChatGroups->FindGroup( aId );
    if ( group )
        {
        TInt indexOfGroup = FindIndexOfOpenChatsItem( TEnumsPC::EGroupItem, aId );
        /*if( indexOfGroup == KErrNotFound)
        	{
        	InsertGroupItemL(group);
        	}*/

        if ( indexOfGroup != KErrNotFound )
            {
            RefreshGroupItem( group, indexOfGroup );
            MoveGroupItem( group, indexOfGroup );
            }
        }

    iGroupArray->Sort();
    //check if this reset is reqd
    if ( iSelectionMode != TEnumsPC::ESingleListViewSelect )
        {
        return;
        }
    TRAP_IGNORE( RefreshOpenChatsListItemL() );
    for ( TInt index = 0; index < iMainViewObservers.Count(); index++ )
        {
        iMainViewObservers[index]->HandleChange( TEnumsPC::EOpenChatsListItem,
                                                 iOpenChatsIndex,
                                                 TEnumsPC::EChanged );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::HandleBackupRestoreEvent
// Group backup / restore events
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleBackupRestoreEvent(
    const TStorageManagerGlobals::TCAObserverEventType /*aEventType*/ )
    {

    TRAPD( restErr, PopulateGroupsListL() );

    if ( restErr )
        {
        CActiveScheduler::Current()->Error( restErr );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::IsAnyContactFound
// (other items were commented in a header).
// ---------------------------------------------------------

TBool CCAMainViewArrayPC::IsAnyContactFound( )
    {

    TInt count( Count() );
    TInt index = iContactListIndex;
    TBool isContactFound( EFalse );
    for (   ; index < count ; index++ )
        {
        TEnumsPC::TItem itemType = GetType( index );
        if ( TEnumsPC::EContactListItem == itemType )
            {
            if ( CountOfContactsInList( index )	> 0 )
                {
                isContactFound = ETrue;
                break;
                }
            }
        }
    return isContactFound;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::PopulateGroupsListL
// (other items were commented in a header).
// ---------------------------------------------------------

void CCAMainViewArrayPC::PopulateGroupsListL( )
    {
    RPointerArray<MCAStoredGroup> groupArray;
    CleanupClosePushL( groupArray );

    iChatGroups->PopulateGroupsListL( groupArray );

    TInt groupCount = groupArray.Count();
    iOnlySavedGroups = 0;
    for ( TInt index = 0; index < groupCount; index++ )
        {
        if ( !groupArray[ index ]->IsOwnGroup() )
            {
            iOnlySavedGroups++;
            }

        MCAStoredGroup* group = groupArray[ index ];

        //if it is already found then just update the read interface
        //because addition is done based on insertinorder. As a result of
        //which if the group is already in the list, the readinterface
        //data will not be changed

        TInt found = iGroupArray->Find( group->GroupId() );
        if ( found == KErrNotFound && !iChatGroups->IsGroupDeleted() )
            {
            iGroupArray->Add( group );
            InsertGroupItemL( group );
            }
        }
    RefreshOpenChatsListItemL();
    iGroupArray->Sort();
    CleanupStack::PopAndDestroy( );	//groupArray
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::MoveGroupItem
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::MoveGroupItem( MCAStoredGroup* aGroup, TInt aFoundIndex )
    {

    //Codescanner wanring ignored since boundcheck is done in gettype() method
    // in the returned foundIndex value from findindexofopenchats() method

    CCAGroupItemPC* groupItem = dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[aFoundIndex] );

    if ( !aGroup->IsJoined() && aFoundIndex <= iOpenChatsInActiveIndex )
        {
        iOpenChatsArray.Remove( aFoundIndex );
        //since the item is removed dont add plus 1 to the insertion position
        iOpenChatsArray.Insert( groupItem, iOpenChatsInActiveIndex /*+ 1*/ );
        iOpenChatsInActiveIndex--;
        InsertIntoInactiveOpenChats( EFalse );
        }

    else if ( aGroup->IsJoined() && aFoundIndex > iOpenChatsInActiveIndex )
        {
        iOpenChatsArray.Remove( aFoundIndex );
        iOpenChatsArray.Insert( groupItem, iOpenChatsIndex );
        iOpenChatsInActiveIndex++;
        }
    SetContactListIndex();
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::InsertIntoInactiveOpenChats
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::InsertIntoInactiveOpenChats( TBool aEnd )
    {

    TInt openChatsCount = iOpenChatsArray.Count();
    TInt indexInGroupArray1( 0 );
    TInt indexInGroupArray2( 0 );
    TInt position( 0 );
    if ( aEnd )//latest item added at the the end of open chats array
        {

        //To find the position of the most recently inserted non -joined
        //group item in the iGroupArray
        if ( ( openChatsCount > iOpenChatsIndex + 1 )
             && ( GetOpenChatType( openChatsCount ) == TEnumsPC::EGroupItem ) )
            {
            CCAGroupItemPC *arrayItemPC =
                dynamic_cast<CCAGroupItemPC *>( iOpenChatsArray[openChatsCount - 1] );
            indexInGroupArray1 = iGroupArray->Find( arrayItemPC->GroupId() );
            }
        else
            {
            return;
            }
        //Get the first item of the sorted inactive groups array
        if ( GetOpenChatType( openChatsCount - 1 ) == TEnumsPC::EGroupItem )
            {
            CCAGroupItemPC *arrayItemPC =
                dynamic_cast<CCAGroupItemPC *>( iOpenChatsArray[openChatsCount - 2] );
            indexInGroupArray2 = iGroupArray->Find( arrayItemPC->GroupId() );
            }
        else
            {
            return;
            }
        if ( indexInGroupArray1 > indexInGroupArray2 )
            {
            //already in sorted position. simply return
            return;
            }
        position = 	openChatsCount -
                    ( indexInGroupArray2 - indexInGroupArray1 ) - iOpenChatsIndex;
        }
    else//latest item added at the first index below iOpenChatsInActiveIndex
        {
        //To find the position of the most recently inserted non -joined
        //group item in the iGroupArray
        if ( iOpenChatsInActiveIndex + 1 == openChatsCount - 1 )
            {
            return;
            }
        if ( GetOpenChatType( iOpenChatsInActiveIndex + 2 ) == TEnumsPC::EGroupItem )
            {
            CCAGroupItemPC *arrayItemPC =
                dynamic_cast<CCAGroupItemPC *>( iOpenChatsArray[iOpenChatsInActiveIndex + 1] );
            indexInGroupArray1 = iGroupArray->Find( arrayItemPC->GroupId() );
            }
        else
            {
            return;
            }
        //Get the first item of the sorted inactive groups array
        if ( GetOpenChatType( iOpenChatsInActiveIndex + 3 ) == TEnumsPC::EGroupItem )
            {
            CCAGroupItemPC *arrayItemPC =
                dynamic_cast<CCAGroupItemPC *>( iOpenChatsArray[iOpenChatsInActiveIndex + 2] );
            indexInGroupArray2 = iGroupArray->Find( arrayItemPC->GroupId() );
            }
        else
            {
            return;
            }
        if ( indexInGroupArray1 < indexInGroupArray2 )
            {
            //already in sorted position. simply return
            return;
            }
        position = iOpenChatsInActiveIndex +
                   ( indexInGroupArray1 - indexInGroupArray2 )  + iOpenChatsIndex ;
        }


    MCAStoredGroup* group = iGroupArray->GroupAt( indexInGroupArray1 );
    TInt foundIndex = FindIndexOfOpenChatsItem( TEnumsPC::EGroupItem, group->GroupId() );
    if ( foundIndex != KErrNotFound )
        {
        CCAGroupItemPC* groupItem = dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[foundIndex] );
        iOpenChatsArray.Remove( foundIndex );
        //since the item is removed dont add plus 1 to the insertion position
        iOpenChatsArray.Insert( groupItem, position );
        SetContactListIndex();
        }
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::GetArrayItemIdDetails
// (other items were commented in a header).
// ---------------------------------------------------------
//
TCAItemDetails CCAMainViewArrayPC::GetArrayItemIdDetails( TInt aIndex )
    {
    if ( aIndex == KErrNotFound )
        {
        return iItemDetails;
        }
    TCAItemDetails itemDetails;
    TInt index = GetItemEngineIndex( aIndex );

    switch ( GetType( aIndex ) )
        {
        case TEnumsPC::EContactItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            MCAStoredContact* contact = item.iContact;
            if ( contact )
                {
                itemDetails.aItemID.Copy( contact->UserId().Left(
                                              itemDetails.aItemID.MaxLength() ) );

                itemDetails.aListID.Copy( item.iContactList->ListId().Left(
                                              itemDetails.aListID.MaxLength() ) );
                itemDetails.aItem = TEnumsPC::EContactItem;
                }
            break;
            }
        case TEnumsPC::EContactListItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            MCAContactListModel::SItem item = iContactListModel.Item( index );
            if ( item.iContactList )
                {
                itemDetails.aItemID.Copy( KNullDesC );
                itemDetails.aListID.Copy( item.iContactList->ListId().Left(
                                              itemDetails.aListID.MaxLength() ) );
                itemDetails.aItem  = TEnumsPC::EContactListItem;
                }
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            MCAArrayItemPC *arrayItemPC = ( MCAArrayItemPC * )iOpenChatsArray[index];
            itemDetails.aItemID.Copy( KNullDesC );
            itemDetails.aListID.Copy( arrayItemPC->GetItemNameText().Left(
                                          itemDetails.aListID.MaxLength() ) );
            itemDetails.aItem = TEnumsPC::EOpenChatsListItem;
            break;
            }
        case TEnumsPC::EConversationItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAConversationItemPC* conversationItem =
                dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[index] );
            itemDetails.aItemID.Copy( conversationItem->UserId().Left(
                                          itemDetails.aItemID.MaxLength() ) );
            itemDetails.aListID.Copy( KNullDesC );
            itemDetails.aItem  = TEnumsPC::EConversationItem;
            break;
            }
        case TEnumsPC::EInviteItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ index ] );
            itemDetails.aItemID.Copy( inviteItem->GetInviteID().Left(
                                          itemDetails.aItemID.MaxLength() ) );
            itemDetails.aListID.Copy( KNullDesC );
            itemDetails.aItem  = TEnumsPC::EInviteItem;
            break;
            }
        case TEnumsPC::EGroupItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAGroupItemPC* groupItem =
                dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[ index ] );
            itemDetails.aItemID.Copy( groupItem->GroupId().Left(
                                          itemDetails.aItemID.MaxLength() ) );
            itemDetails.aListID.Copy( KNullDesC );
            itemDetails.aItem = TEnumsPC::EGroupItem;
            break;
            }
        case TEnumsPC::EOwnStatusItem:
            {
            /* Codescanner warning is ignored, since Bound check is done
             * inside the GetType()
             * method*/
            CCAOwnDataItemPC* ownDataItem =
                dynamic_cast< CCAOwnDataItemPC* > ( iMainViewListItemArray[ index ] );
            itemDetails.aItemID.Copy( ownDataItem->GetItemNameText().Left(
                                          itemDetails.aItemID.MaxLength() ) );
            itemDetails.aListID.Copy( KNullDesC );
            itemDetails.aItem = TEnumsPC::EOwnStatusItem;
            break;
            }
        default:
            {
            //should never be here.
            //set the iItemDetails to null/invalid
            break;
            }
        }
    return itemDetails;
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::GetArrayItemIdIndexL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAMainViewArrayPC::GetArrayItemIdIndexL( const TCAItemDetails& aItemDetails )
    {
    TInt arrayIndex = KErrNotFound;
    switch (  aItemDetails.aItem )
        {
        case TEnumsPC::EOwnStatusItem:
            {
            arrayIndex = KOwnDataIndex;
            break;
            }
        case TEnumsPC::EContactItem:
            {
            arrayIndex = FindContactL( aItemDetails.aListID ,
                                       aItemDetails.aItemID );
            break;
            }
        case TEnumsPC::EContactListItem:
            {
            arrayIndex = FindContactListIndexL( aItemDetails.aListID );
            break;
            }
        case TEnumsPC::EConversationItem:
        case TEnumsPC::EInviteItem:
        case TEnumsPC::EGroupItem:
            {
            arrayIndex = FindIndexOfOpenChatsItem( aItemDetails.aItem, aItemDetails.aItemID );
            if ( arrayIndex != KErrNotFound )
                {
                arrayIndex = iOpenChatsIndex + arrayIndex;
                }
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
            {
            arrayIndex = iOpenChatsIndex;
            }
        default:
            {
            break;
            }
        }
    if ( KErrNotFound ==	arrayIndex )
        {
        arrayIndex = iOpenChatsIndex +
                     FindIndexOfOpenChatsItem( iItemDetails.aItem , iItemDetails.aItemID );
        }
    return arrayIndex;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::RegisterCallBack
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::RegisterCallBack( MCASingleListViewCB* aSingleViewCB )
    {

    ASSERT( aSingleViewCB );

    iSingleViewCB = aSingleViewCB;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::UnRegisterCallBack
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::UnRegisterCallBack( )
    {
    ASSERT( iSingleViewCB );
    iSingleViewCB = NULL;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::FindIndexOfOpenChatsItem
// From MCAMainViewObserverManagerPC
// ---------------------------------------------------------
//
TInt CCAMainViewArrayPC::FindIndexOfOpenChatsItem( TEnumsPC::TItem aItemType,
                                                   const TDesC& aId )
    {
    TInt itemIndex = KErrNotFound;
    switch ( aItemType )
        {
        case TEnumsPC::EConversationItem:
            {
            for ( TInt index = 1; index < iOpenChatsArray.Count(); index++ )
                {
                if ( GetOpenChatType( index + iOpenChatsIndex ) == TEnumsPC::EConversationItem )
                    {
                    CCAConversationItemPC* convItem =
                        dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[ index ] );
                    if ( CAUtils::NeutralCompare( convItem->UserId(), aId ) == 0 )
                        {
                        itemIndex = index;// + iOpenChatsIndex	;
                        break;
                        }
                    }
                }
            break;
            }
        case TEnumsPC::EInviteItem:
            {
            for ( TInt index = 1; index < iOpenChatsArray.Count(); index++ )
                {
                if ( GetOpenChatType( index + iOpenChatsIndex ) == TEnumsPC::EInviteItem )
                    {
                    CCAInviteItemPC* inviteItem =
                        dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ index ] );
                    if ( CAUtils::NeutralCompare( inviteItem->GetInviteID() , aId ) == 0  )
                        {
                        itemIndex = index;// + iOpenChatsIndex;
                        break;
                        }
                    }
                }
            break;
            }
        case TEnumsPC::EGroupItem:
            {
            for ( TInt index = 1; index < iOpenChatsArray.Count(); index++ )
                {
                if ( GetOpenChatType( index + iOpenChatsIndex ) == TEnumsPC::EGroupItem )
                    {
                    TCollationMethod collation =
                        *Mem::CollationMethodByIndex( 0 );
                    collation.iFlags |= TCollationMethod::EIgnoreNone;

                    CCAGroupItemPC* groupItem =
                        dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[ index ] );
                    if ( groupItem->GroupId().CompareC( aId, KCollationLevel, &collation ) == 0 )
                        {
                        itemIndex = index;// + iOpenChatsIndex;
                        break;
                        }
                    }
                }
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
            {
            itemIndex = iOpenChatsIndex - 1;
            break;
            }
        default:
            {
            itemIndex = KErrNotFound;
            break;
            }
        }

    return itemIndex;
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::ResetGroupCountDownOnLogout
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::ResetGroupCountDownOnLogout()
    {
    iGroupCountDown = EFalse;
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::SetcurrentItemIndex
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::SetCurrentItemIndexL( TBool aFreshLogin )
    {
    if ( iSingleViewCB )
        {
        iSingleViewCB->SetCurrentItemIndexL( aFreshLogin );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::SortContactListAfterRenameL
//
// ---------------------------------------------------------
//
TInt CCAMainViewArrayPC::SortContactListAfterRenameL( const TDesC& aListId )
    {
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    TInt newIndex = KErrNotFound;
    if ( contacts )
        {
        contacts->SortContactLists();
        newIndex = FindContactListIndexL( aListId );
        }
    return newIndex;
    }


const RPointerArray<MCAArrayItemPC>& CCAMainViewArrayPC::GetOpenChatsArray()const
    {
    return iOpenChatsArray;
    }

TInt CCAMainViewArrayPC::GetOpenChatsActiveIndex()const
    {
    return iOpenChatsInActiveIndex;

    }


TBool CCAMainViewArrayPC::IsOpenChatsMsgPending(
    const TEnumsPC::TItem aType,
    const TDesC& aId )
    {
    TInt high, mid( 0 ), low( 0 );

    TBool isMsgPending = EFalse;

    switch ( aType )
        {
        case TEnumsPC::EConversationItem:
            {
            high = iChatInterface.ChatCount() - 1;

            while ( low <= high )
                {
                mid = ( high + low ) / 2;

                MCAMessagesReadInterface& readInterface = iChatInterface.ChatAt( mid );

                if ( CAUtils::NeutralCompare( readInterface.TargetId().Target(), aId ) == 0 )
                    {
                    isMsgPending = readInterface.UnreadCount(
                                       MCAMessagesReadInterface::EUnreadReceived ) != 0;
                    break;
                    }
                else if ( CAUtils::NeutralCompare( readInterface.TargetId().Target(), aId ) < 0 )
                    {
                    low = mid + 1;
                    }
                else
                    {
                    high = mid - 1;
                    }
                }

            break;
            }
        case TEnumsPC::EGroupItem:
            {

            high = iChatInterface.GroupCount() - 1;

            // Create custom collation method to ignore punctuations
            // index 0 gets the default method
            TCollationMethod collation =
                *Mem::CollationMethodByIndex( 0 );
            collation.iFlags |= TCollationMethod::EIgnoreNone;

            while ( low <= high )
                {
                mid = ( high + low ) / 2;

                MCAMessagesReadInterface& readInterface = iChatInterface.GroupAt( mid );

                if ( readInterface.TargetId().Target().CompareC(
                         aId, KCollationLevel, &collation ) == 0 )
                    {
                    isMsgPending = readInterface.UnreadCount(
                                       MCAMessagesReadInterface::EUnreadReceived ) != 0;
                    break;
                    }
                else if ( readInterface.TargetId().Target().CompareC(
                              aId, KCollationLevel, &collation ) < 0 )
                    {
                    low = mid + 1;
                    }
                else
                    {
                    high = mid - 1;
                    }
                }
            break;
            }
        case TEnumsPC::EInviteItem:
            {
            for ( TInt indx = 1; indx < iOpenChatsArray.Count(); indx++ )
                {
                if ( GetOpenChatType( indx + iOpenChatsIndex ) == TEnumsPC::EInviteItem )
                    {
                    CCAInviteItemPC* inviteItem =
                        dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ indx ] );
                    if ( 0 == aId.Compare( inviteItem->GetInviteID() ) )
                        {
                        MCAInvitation* inv = inviteItem->GetInvitation();
                        isMsgPending = inv->IsRead();

                        // This is required 'coz teh IsRead() method returns
                        // EFalse if the invitation is not read.
                        isMsgPending = !isMsgPending;
                        }

                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }

    return isMsgPending;

    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::NotifySyncCompleted
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::NotifySyncCompleted()
    {
    if ( iSyncObserver )
        {
        iSyncObserver->NotifySyncCompleted();
        }
    }


// ---------------------------------------------------------
// CCAMainViewArrayPC::PendingMessageInfo
// ---------------------------------------------------------
//
const TDesC& CCAMainViewArrayPC::PendingMessageInfo()
    {

    MCAMessageContainerInfo* info = iChatInterface.PendingMessageInfo();

    return info->Target();

    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::ChatGroupPendingMessageInfo
// ---------------------------------------------------------
//
const TDesC& CCAMainViewArrayPC::ChatGroupPendingMessageInfo( TDesC& aGroupName )
    {

    MCAMessageContainerInfo* info = iChatInterface.ChatGroupPendingMessageInfo();

    MCAStoredGroup* group = iChatGroups->FindGroup( info->Target() );
    if ( group )
        {
        aGroupName = group->GroupName();
        }

    return info->Target();
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::AddObserver
// From MCAMainViewArrayPC
// ---------------------------------------------------------


void CCAMainViewArrayPC::RegisterObserverL( MCAListInitObserver* aObserver )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();
    if ( contactLists )
        {
        contactLists->AddObserver( aObserver );
        }
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::RemoveObserver
// From MCAMainViewArrayPC
// ---------------------------------------------------------

void CCAMainViewArrayPC::UnRegisterObserverL( )
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();
    if ( contactLists )
        {
        contactLists->RemoveObserver();
        }
    }
// ---------------------------------------------------------
// CCAMainViewArrayPC::IsListInitialization
// From MCAMainViewArrayPC
// ---------------------------------------------------------
TBool CCAMainViewArrayPC::IsListInitializationCompletedL()
    {
    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();
    if ( contactLists )
        {
        return contactLists->IsListInitializationCompleted();
        }
    else
        {
        return ETrue;
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::RefreshConversationItemText
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAMainViewArrayPC::RefreshConversationItemText( const TDesC& aContactId )
    {
    TInt index = KErrNotFound;
    index = FindIndexOfOpenChatsItem( TEnumsPC::EConversationItem, aContactId );
    if ( KErrNotFound != index )
        {
        CCAConversationItemPC* convItemPC =
            dynamic_cast<CCAConversationItemPC*>( iOpenChatsArray[index] );
        convItemPC->SetItemNameText( iContactInterface->Identification( aContactId ) );
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::RefreshInvitationItemTextL
// (other items were commented in a header).
// ---------------------------------------------------------
void CCAMainViewArrayPC::RefreshInvitationItemTextL( const TDesC& aContactId )
    {

    HBufC* inviteName = NULL;

    for ( TInt indx = 1; indx < iOpenChatsArray.Count(); indx++ )
        {
        if ( GetOpenChatType( indx + iOpenChatsIndex ) == TEnumsPC::EInviteItem )
            {
            CCAInviteItemPC* inviteItem =
                dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[ indx ] );
            if ( CAUtils::NeutralCompare( inviteItem->UserId() , aContactId ) == 0  )
                {
                //Only create once, as this is heavy call
                if ( !inviteName )
                    {
                    inviteName = StringLoader::LoadLC(
                                     R_QTN_CHAT_CONTACTLIST_INVITATION,
                                     iContactInterface->Identification( aContactId ) );

                    }
                //the below api makes a copy of the invite name
                inviteItem->SetItemNameText( *inviteName );

                }
            }
        }

    //clear the created invite name
    if ( inviteName )
        {
        CleanupStack::PopAndDestroy();
        }
    }

// ---------------------------------------------------------
// CCAMainViewArrayPC::GetIndexOfTopPendingMessage
// ---------------------------------------------------------
//
TInt CCAMainViewArrayPC::GetIndexOfTopPendingMessage()
    {
    TInt count = iOpenChatsArray.Count();

    for ( TInt index = iOpenChatsIndex ; ( index < count ) ; ++index )
        {
        if ( IsMsgPending( index ) )
            {
            return index;
            }
        }

    // Fix warning: #940-D: missing return statement at end of non-void function
    return iOpenChatsIndex;
    }

// ---------------------------------------------------------
// CCASingleListView::HandleInitChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAMainViewArrayPC::HandleListInitChange( TBool aCompleted )
    {
    TInt refreshConvCount = iRefreshConvArray.Count();
    for ( TInt index = 0; index < refreshConvCount; index++ )
        {
        HBufC* buf = iRefreshConvArray[index];
        if ( buf )
            {
            RefreshConversationItemText( *buf );
            }
        }

    iRefreshConvArray.ResetAndDestroy();


    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* contactLists = presence->ContactLists();

    if ( contactLists )
        {
        contactLists->RemoveMainViewProcessObserver();
        }
    }

// End of File
