/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute list transaction manager
*
*/

// INCLUDE FILES
#include "CPEngAttributeListTransactionManager.h"
#include "PEngListLibraryFactory.h"
#include "MPEngListLibFactory.h"
#include "PEngPresenceEngineConsts2.h"
#include "CPEngContactListTransactionManager.h"
#include "CPEngContactListSettings.h"
#include "CPEngAttributeListItem.h"
#include "CPEngAttributeListTransCreateAttrList.h"
#include "CPEngAttributeListTransDeleteAttrList.h"
#include "MPEngStorageManager.h"
#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrManager.h"
#include "PEngMessagePacker.h"
#include "PresenceDebugPrint.h"
#include <e32std.h>


// MACROS
#define CONTINUE_IF( a )\
    if ( a )\
        {\
        continue;\
        }


//Default granurality for ID lists
const TInt KAttributeListIdGranurality = 3;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CPEngAttributeListTransactionManager()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransactionManager::CPEngAttributeListTransactionManager(
    MPEngListLibFactory& aFactory,
    TPEngWVCspVersion& aCSPVersion )
        : CPEngAttributeListManager( aFactory ),
        iCSPVersion( aCSPVersion )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::ConstructL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngAttributeListManager::ConstructL( aSessionSlot );
    iContactListTransManager = static_cast <CPEngContactListTransactionManager*>
                               ( PEngListLibraryFactory::ContactListTransactionManagerLC( aSessionSlot ) );

    CleanupStack::Pop();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransactionManager* CPEngAttributeListTransactionManager::NewL(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot,
    TPEngWVCspVersion& aCSPVersion )
    {
    CPEngAttributeListTransactionManager* self = NewLC( aFactory,
                                                        aSessionSlot,
                                                        aCSPVersion );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransactionManager* CPEngAttributeListTransactionManager::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot,
    TPEngWVCspVersion& aCSPVersion )
    {
    CPEngAttributeListTransactionManager* self =
        new( ELeave ) CPEngAttributeListTransactionManager( aFactory,
                                                            aCSPVersion );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionSlot );

    return self;
    }


//   Destructor
CPEngAttributeListTransactionManager::~CPEngAttributeListTransactionManager()
    {
    if ( iContactListTransManager )
        {
        iContactListTransManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveAttrListTransManager( this );
        delete this;
        }
    }



// =============================================================================
// ============ Functions of the MPEngTransactionFactory class ===============
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::IncomingTransactionHandlerL()
// -----------------------------------------------------------------------------
//
MPEngIncomingTransactionHandler*
CPEngAttributeListTransactionManager::IncomingTransactionHandlerL(
    const TDesC8& /* aIncomingRequest */ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::OutgoingTransactionHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::OutgoingTransactionHandlerL(
    TInt aOperation,
    const TDesC& /*aData*/,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers )
    {
    PENG_DP( D_PENG_LIT( "CPEngAttributeListTransactionManager::OutgoingTransactionHandlerL()" ) );

    switch ( aOperation )
        {
        case EPEngTransOpAttrListPublish:
            {
            // Make sure that we are handling most up-to-date data
            iStorageManager->RetrieveL( *this );

            if ( !iSynchronized )
                {
                GetSyncHandlersL( aHandlers, aOperation );
                }
            else
                {
                GetAttributeListsUpdateHandlersL( aHandlers, aOperation );
                }

            break;
            }

        default:
            {
            }
        }
    }



// =============================================================================
// ============ From MPEngAttributeListTransactionManager ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::AttributeListCreated()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::AttributeListCreated(
    const RArray<TUint32>& aAttrList )
    {
    TInt index ( FindAttributeList( aAttrList ) );
    if ( index != KErrNotFound )
        {
        iAttributeLists[ index ]->SetSynchronization( ETrue );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::AttributeEngineSynchronizedL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::AttributeEngineSynchronizedL()
    {
    iSyncHandlerCount--;
    if ( !iSyncHandlerCount )
        {
        iSynchronized = ETrue;
        StoreChangesL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitContactListL(
    const TDesC& aContactList )
    {
    CommitContactL( aContactList,
                    &CPEngAttributeListItem::CommitContactListL,
                    iDeleteAttrListContactLists );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitContactIdL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitContactIdL(
    const TDesC& aContactId )
    {
    CommitContactL( aContactId,
                    &CPEngAttributeListItem::CommitContactIdL,
                    iDeleteAttrListContactIDs );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::RollBackContactListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::RollBackContactListL(
    const TDesC& aContactList )
    {
    RollBackOneContactL( aContactList,
                         CPEngAttributeListItem::ENewContactLists,
                         iDeleteAttrListContactLists );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::RollBackContactIdL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::RollBackContactIdL(
    const TDesC& aContactId )
    {
    RollBackOneContactL( aContactId,
                         CPEngAttributeListItem::ENewContactIDs,
                         iDeleteAttrListContactIDs );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitDefaultL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitDefaultL()
    {
    // go through all Attribute lists and commit Default flag
    // if attribute list is without any contact after commiting
    // and it isn't default or to be new default,
    // this attribute list will be deleted in CompressAtrrListArray function
    for ( TInt x ( iAttributeLists.Count() - 1 ) ;  x >= 0 ; x-- )
        {
        // commit default to each
        iAttributeLists[x]->CommitDefault();
        }
    iDeleteDefaultAttrList = EFalse;

    // now compress array
    CompressArrListArray();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::RollBackDefaultL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::RollBackDefaultL()
    {
    // go through all Attribute lists and commit Default flag
    // if attribute list is without any contact after commiting and it is not default
    // or to be new default, this attribute list will be deleted in CompressAtrrListArray function
    for ( TInt x ( iAttributeLists.Count() - 1 ) ;  x >= 0 ; x-- )
        {
        // commit default to each
        iAttributeLists[x]->SetNewDefault( EFalse );
        }
    iDeleteDefaultAttrList = EFalse;

    // now compress array
    CompressArrListArray();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitDefinedAttributeList()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitDefinedContactsL(
    const CDesCArray* aContactLists,
    const CDesCArray* aContactIds )
    {
    // commit all contacts IDs and contact lists
    if ( aContactLists )
        {
        TInt count ( aContactLists->Count() );
        for ( TInt ii( 0 ) ; ii < count ; ii++ )
            {
            CommitContactListL( aContactLists->MdcaPoint( ii ) );
            }
        }


    if ( aContactIds )
        {
        TInt count ( aContactIds->Count() );
        for ( TInt i( 0 ) ; i < count ; i++ )
            {
            CommitContactIdL( aContactIds->MdcaPoint( i ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::RollBackDefinedContactsL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::RollBackDefinedContactsL(
    const CDesCArray* aContactLists,
    const CDesCArray* aContactIds )
    {
    // RollBack all contacts IDs and contact lists
    if ( aContactLists )
        {
        TInt count ( aContactLists->Count() );
        for ( TInt ii( 0 ) ; ii < count ; ii++ )
            {
            RollBackContactListL( aContactLists->MdcaPoint( ii ) );
            }
        }


    if ( aContactIds )
        {
        TInt count ( aContactIds->Count() );
        for ( TInt i( 0 ) ; i < count ; i++ )
            {
            RollBackContactIdL( aContactIds->MdcaPoint( i ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::StoreChangesToStoreL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::StoreChangesToStoreL()
    {
    User::LeaveIfError( StoreChangesL() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::GetSyncHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::GetSyncHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionHandlers,
    TInt aOperationId )
    {
    // Commit all items in the lists before they are synchronized,
    // so desired changes are synchronized right a way
    CommitAllAttributeListL();
    StoreChangesL();

    // now create SyncAttributes list handlers
    iSyncHandlerCount = 0;
    TInt count ( iAttributeLists.Count() );
    for ( TInt y( 0 ) ;  y < count ; y++ )
        {
        // create attribute list only if it needed
        if ( CheckAttributeListServerValidityCurrent( *( iAttributeLists[y] ) ) )
            {
            // ETrue => this is sync handler
            CPEngAttributeListTransCreateAttrList* newHandler =
                CPEngAttributeListTransCreateAttrList::NewLC(
                    *( iAttributeLists[y] ),
                    *this,
                    *iContactListTransManager,
                    *iAttributeManager,
                    iCSPVersion,
                    aOperationId,
                    ETrue );

            aTransactionHandlers.AppendL( newHandler );
            CleanupStack::Pop(); // newHandler
            iSyncHandlerCount++;
            }
        }

    // now check if there are some attribute lists to be deleted
    // GetDeleteAttributeListsHandlersL( aTransactionHandlers, aOperationId );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::GetAttributeListsUpdateHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::GetAttributeListsUpdateHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionHandlers,
    TInt aOperationId )
    {
    // go through all attributes and create all handlers which are necessary
    TInt count ( iAttributeLists.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        if ( CheckAttributeListCreateNewValidity( *( iAttributeLists[x] ) ) )
            {
            // there is something to update, create handler
            CPEngAttributeListTransCreateAttrList* newHandler =
                CPEngAttributeListTransCreateAttrList::NewLC(
                    *( iAttributeLists[ x ] ),
                    *this,
                    *iContactListTransManager,
                    *iAttributeManager,
                    iCSPVersion,
                    aOperationId );
            aTransactionHandlers.AppendL( newHandler );
            CleanupStack::Pop(); // newHandler
            }

        else
            {
            // Commit attribute list
            CommitAttributeListL( *( iAttributeLists[x]  ) );
            }
        }

    // now check if there are some attribute lists to be deleted
    GetDeleteAttributeListsHandlersL( aTransactionHandlers, aOperationId );
    StoreChangesL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CompressArrListArray()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::GetDeleteAttributeListsHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionHandlers,
    TInt aOperationId )
    {
    if ( CheckAttributeListDeleteValidity() )
        {
        // there is something to delete from server, create handler
        CPEngAttributeListTransDeleteAttrList* newHandler;
        newHandler = CPEngAttributeListTransDeleteAttrList::NewLC(
                         *this,
                         *iContactListTransManager,
                         iDeleteAttrListContactLists,
                         iDeleteAttrListContactIDs,
                         iDeleteDefaultAttrList,
                         iCSPVersion,
                         aOperationId );

        aTransactionHandlers.AppendL( newHandler );
        CleanupStack::Pop(); // newHandler
        }
    else
        {
        CommitDeleteAttributeListL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CompressArrListArray()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CompressArrListArray()
    {
    // go through array from the end and delete all attribute lists
    // which does not hold any contact
    for ( TInt x ( iAttributeLists.Count() - 1 ) ; x >= 0 ; x-- )
        {
        if ( 0 == iAttributeLists[ x ]->ContactsCount() )
            {
            // no more contacts, delete attribute list
            delete iAttributeLists[ x ];
            iAttributeLists.Remove( x );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::AppendDesArrayContentL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::AppendDesArrayContentL(
    const CDesC16Array& aOriginalDesArray,
    CDesC16Array& aNewDesArray )
    {
    TInt count ( aOriginalDesArray.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        aNewDesArray.AppendL( aOriginalDesArray[ x ] );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::AppendDesArrayContentL()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransactionManager::CheckAttributeListServerValidityCurrent(
    const CPEngAttributeListItem& aAttributeList )
    {
    return CheckAttributeListValidity( aAttributeList,
                                       CPEngAttributeListItem::ECurrentContactIDs,
                                       CPEngAttributeListItem::ECurrentContactLists,
                                       &CPEngAttributeListItem::CurrentlyDefault,
                                       EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CheckAttributeListCreateNewValidity()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransactionManager::CheckAttributeListCreateNewValidity(
    const CPEngAttributeListItem& aAttributeList )
    {
    return CheckAttributeListValidity( aAttributeList,
                                       CPEngAttributeListItem::ENewContactIDs,
                                       CPEngAttributeListItem::ENewContactLists,
                                       &CPEngAttributeListItem::NewDefault,
                                       EFalse );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CheckAttributeListDeleteValidity()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransactionManager::CheckAttributeListDeleteValidity()
    {
    TBool attrListValid( ETrue );
    if ( ( iDeleteAttrListContactIDs.Count() == 0 ) && !iDeleteDefaultAttrList )
        {
        attrListValid = EFalse;
        TInt cntListCount ( iDeleteAttrListContactLists.Count() );
        for ( TInt i ( 0 ) ; i < cntListCount ; i++ )
            {
            CPEngContactListSettings* cntSettings =
                iContactListTransManager->ContactListSettingsOrNull(
                    iDeleteAttrListContactLists[ i ] );

            CONTINUE_IF( !cntSettings );

            attrListValid += cntSettings->Property( KPEngListExistsOnServer,
                                                    KPEngCntLstPropertyNativeCached );

            // if list exists, break away
            if ( attrListValid )
                {
                return ETrue;
                }
            }
        }

    return attrListValid;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitAttributeList()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitAttributeListL(
    const CPEngAttributeListItem& aAttributeList )
    {
    CDesCArraySeg* newContactIDs = new ( ELeave ) CDesCArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( newContactIDs );

    AppendDesArrayContentL( aAttributeList.ArrayOfContacts(
                                CPEngAttributeListItem::ENewContactIDs ),
                            *newContactIDs );


    CDesCArraySeg* newContactLists =  new ( ELeave ) CDesCArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( newContactLists );

    AppendDesArrayContentL( aAttributeList.ArrayOfContacts(
                                CPEngAttributeListItem::ENewContactLists ),
                            *newContactLists );


    TBool newDefaulFlag( EFalse );
    if ( aAttributeList.NewDefault() )
        {
        newDefaulFlag = ETrue;
        }


    // commit all new contacts IDs and contact lists
    CommitDefinedContactsL( newContactIDs, newContactLists );
    CleanupStack::PopAndDestroy( 2 ); // newContactLists, newContactIDs
    if ( newDefaulFlag )
        {
        CommitDefaultL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitAttributeList()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitAllAttributeListL()
    {
    CDesCArraySeg* newContactIDs = new ( ELeave ) CDesCArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( newContactIDs );

    CDesCArraySeg* newContactLists =  new ( ELeave ) CDesCArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( newContactLists );
    TBool newDefaulFlag( EFalse );


    TInt count ( iAttributeLists.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        AppendDesArrayContentL( iAttributeLists[ x ]->ArrayOfContacts(
                                    CPEngAttributeListItem::ENewContactIDs ),
                                *newContactIDs );

        AppendDesArrayContentL( iAttributeLists[ x ]->ArrayOfContacts(
                                    CPEngAttributeListItem::ENewContactLists ),
                                *newContactLists );

        if ( iAttributeLists[ x ]->NewDefault() )
            {
            newDefaulFlag = ETrue;
            }
        }


    CommitDefinedContactsL( newContactLists, newContactIDs );
    CleanupStack::PopAndDestroy( 2 ); // newContactLists, newContactIDs


    if ( newDefaulFlag )
        {
        CommitDefaultL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitDeleteAttributeListL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitDeleteAttributeListL()
    {
    CDesCArraySeg* contacts = new ( ELeave ) CDesCArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( contacts );

    AppendDesArrayContentL( iDeleteAttrListContactIDs, *contacts );
    CommitDefinedContactsL( contacts, NULL );

    contacts->Reset();

    AppendDesArrayContentL( iDeleteAttrListContactLists, *contacts );
    CommitDefinedContactsL( NULL, contacts );

    CleanupStack::PopAndDestroy( ); // contacts

    if ( iDeleteDefaultAttrList )
        {
        CommitDefaultL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CommitContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::CommitContactL(
    const TDesC& aContact,
    CommitCall aCommitCall,
    CDesCArray& aDeleteArray )
    {
    // go through all Attribute lists and commit contact list to them
    // if attribute list is without any contact after commiting and it isn's
    // default or to be new default,
    // this attribute list will be deleted in  CompressAtrrListArray function
    for ( TInt x ( iAttributeLists.Count() - 1 ) ;  x >= 0 ; x-- )
        {
        ( iAttributeLists[x]->*aCommitCall )( aContact );
        }


    // now compress array
    // check if contact list was in to be deleted ones
    TInt position( 0 );
    TInt err ( aDeleteArray.FindIsq( aContact , position, ECmpFolded ) );
    if ( err == KErrNone )
        {
        aDeleteArray.Delete( position );
        }

    CompressArrListArray();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::RollBackOneContactL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransactionManager::RollBackOneContactL(
    const TDesC& aContact,
    CPEngAttributeListItem::TPEngAttrListItemArrays aNewCommitArray,
    CDesCArray& aDeleteArray )
    {
    // go through all Attribute lists and delete contact which does not exist
    // if attribute list is without any contact after commiting and
    // it is not default or to be new default, this attribute
    // list will be deleted in CompressAtrrListArray function

    // increment by one, this will remove that invalid contact also from
    // current
    CPEngAttributeListItem::TPEngAttrListItemArrays nextArray(
        static_cast<CPEngAttributeListItem::TPEngAttrListItemArrays>
        ( aNewCommitArray + 1 ) );

    for ( TInt x ( iAttributeLists.Count() - 1 ) ;  x >= 0 ; x-- )
        {
        // Delete contact from currect and also new
        iAttributeLists[x]->DeleteContactFromList( aContact, aNewCommitArray );
        iAttributeLists[x]->DeleteContactFromList( aContact, nextArray );
        }


    // check if contact list was in to be deleted ones
    TInt position( 0 );
    TInt err ( aDeleteArray.FindIsq( aContact , position, ECmpFolded ) );
    if ( err == KErrNone )
        {
        aDeleteArray.Delete( position );
        }

    // now compress array
    CompressArrListArray();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransactionManager::CheckAttributeListValidity()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransactionManager::CheckAttributeListValidity(
    const CPEngAttributeListItem& aAttributeList,
    CPEngAttributeListItem::TPEngAttrListItemArrays aIDsArray,
    CPEngAttributeListItem::TPEngAttrListItemArrays aCntListArray,
    ItemDefaultSet aDefaultSetCall,
    TBool aSynchronizeFlagIncluded )
    {
    // check is handler is needed, if any of the contact in the contact list
    // does not exist on the server an attr list
    // is not default, there is no need for transaction.
    TBool attrListValid( ETrue );
    if ( ( aAttributeList.ArrayOfContacts( aIDsArray ).Count() == 0 ) &&
         !( aAttributeList.*aDefaultSetCall )() )
        {
        attrListValid =  EFalse ;
        const CDesCArray& cntLstArray =
            aAttributeList.ArrayOfContacts( aCntListArray );
        TInt cntListCount ( cntLstArray.Count() );
        for ( TInt i ( 0 ) ; i < cntListCount ; i++ )
            {
            CPEngContactListSettings* cntSettings =
                iContactListTransManager->ContactListSettingsOrNull( cntLstArray[ i ] );

            CONTINUE_IF( ( !cntSettings
                           ||
                           !( cntSettings->Property( KPEngListExistsOnServer,
                                                     KPEngCntLstPropertyNativeCached ) )
                         ) );

            attrListValid += ( !aSynchronizeFlagIncluded || cntSettings->Synchronized() );

            // if list exists, break the loop
            if ( attrListValid )
                {
                return ETrue;
                }
            }
        }

    return attrListValid;
    }

//  End of File

