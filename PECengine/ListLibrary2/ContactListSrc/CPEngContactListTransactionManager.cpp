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
* Description:  Contact list transaction manager
*
*/


// INCLUDE FILES
#include "CPEngContactListTransactionManager.h"

#include "PEngListLibraryFactory.h"
#include "MPEngListLibFactory.h"
#include "PEngPresenceEngineConsts2.h"

#include "CPEngContactListModBase.h"
#include "CPEngContactListSettings.h"
#include "CPEngContactListModChangeMonitor.h"

#include "CPEngContactListMngTransAddId.h"
#include "CPEngContactListMngTransRemoveId.h"
#include "CPEngContactListMngTransProperties.h"
#include "CPEngContactListMngTransSubcription.h"
#include "CPEngContactListDeleteTrans.h"
#include "CPEngGetWatchersTransaction.h"
#include "CPEngStartSyncTransaction.h"

#include "CPEngContactListTransCreate.h"
#include "CPEngContactListTransSync.h"

#include "PEngContactIdsTools.h"
#include "PEngListLibTools.h"

#include "MPEngStorageManager.h"
#include "PEngMessagePacker.h"

#include "CPEngSessionSlotId.h"

#include "PresenceDebugPrint.h"
#include <e32std.h>


// CONSTANTS
_LIT( KPEngUnsubscribe, "UNSUB" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CPEngContactListTransactionManager()
// -----------------------------------------------------------------------------
//
CPEngContactListTransactionManager::CPEngContactListTransactionManager(
    MPEngListLibFactory& aFactory,
    TPEngWVCspVersion& aCSPVersion )
        : CPEngContactListManagerBase( aFactory ),
        iCSPVersion( aCSPVersion )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::ConstructL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    CPEngContactListManagerBase::ConstructL( aSessionSlot );
    InitializePlainUserId();
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::NewL()
// -----------------------------------------------------------------------------
//
CPEngContactListTransactionManager* CPEngContactListTransactionManager::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot,
    TPEngWVCspVersion& aCSPVersion )
    {
    CPEngContactListTransactionManager* self =
        new ( ELeave ) CPEngContactListTransactionManager( aFactory,
                                                           aCSPVersion );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionSlot );

    return self;
    }


// Destructor
CPEngContactListTransactionManager::~CPEngContactListTransactionManager()
    {
    }



// =============================================================================
// ===============Function from MPEngTransactionFactory ======================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::Release()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveCntListTransManager( this );
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetIncomingNwTransactionHandlersL()
// -----------------------------------------------------------------------------
//
MPEngIncomingTransactionHandler* CPEngContactListTransactionManager::IncomingTransactionHandlerL(
    const TDesC8& /* aIncomingRequest */ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetOutgoingNwTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::OutgoingTransactionHandlerL(
    TInt aOperation,
    const TDesC& aData,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransactionManager::OutgoingTransactionHandlerL() [%d]" ),
             aOperation );

    switch ( aOperation )
        {
        case EPEngTransOpCntListUpdateList:
        case EPEngTransOpCntListSyncList:
            {
            if ( !iEnviromentSynchronized )
                {
                User::Leave( KErrNotReady );
                }

            //Create handlers
            GetOutGoingTransactionHandlersL( aData,
                                             aHandlers,
                                             aOperation );
            break;
            }


        case EPEngTransOpCntListBaseSync:
            {
            if ( iEnviromentSynchronized )
                {
                // already done, no need for it
                return;
                }

            TBool needToUnsubscribe( EFalse );
            if ( aData.Compare( KPEngUnsubscribe ) == 0 )
                {
                needToUnsubscribe = ETrue;
                }

            CPEngStartSyncTransaction* startSync =
                CPEngStartSyncTransaction::NewLC( *this,
                                                  iCSPVersion,
                                                  aOperation,
                                                  needToUnsubscribe );

            aHandlers.AppendL( startSync );
            CleanupStack::Pop(); //newHandler

            break;
            }

        default:
            {
            }
        }
    }


// =============================================================================
// =============== from MPEngContactListTransactionManager =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CompleteEnviromentSyncL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::CompleteEnviromentSyncL(
    CDesCArray& aExistingLists,
    const TDesC& aDefaultList,
    TBool aListsUnsubscribed )
    {
    //Generate array of plain list names
    //+1 to prevent having count 0
    const TInt existingCount = aExistingLists.Count();
    CPtrCArray* planeNames = new ( ELeave ) CPtrCArray( existingCount + 1 );
    CleanupStack::PushL( planeNames );

    for ( TInt i( 0 ) ; i < existingCount ; ++i )
        {
        TPtrC name( aExistingLists[ i ] );
        NContactListIdsTools::ExtractPureContactListName( name,
                                                          iPlainUserName );
        planeNames->AppendL( name );
        }


    //Process all known contact list settings,
    //wether thay exist on the server or not
    for ( TInt x( iContactListSettings.Count() - 1 ) ; x >= 0 ; --x )
        {
        // For future: what if is it master and default
        // Update is needed
        CPEngContactListSettings* sett = iContactListSettings[ x ];

        sett->DeletePropertyL( KPEngCntLstDefault,
                               KPEngCntLstPropertyNativePermanent );
        sett->DeletePropertyL( KPEngHasBeenSynchronized,
                               KPEngCntLstPropertyNativeCached );


        //Try locate list from plain names array
        //If list is found, remove it from plain names and parameter array
        TInt cn( planeNames->Count() - 1 );
        for ( ; cn >= 0 ; --cn )
            {
            if ( KErrNone == NContactListIdsTools::CompareContactListIds( sett->Name(),
                                                                          ( *planeNames )[ cn ] ) )
                {
                sett->SetPropertyL( KPEngListExistsOnServer,
                                    KPEngCntLstPropertyNativeCached,
                                    ETrue );
                if ( aListsUnsubscribed )
                    {
                    sett->DeletePropertyL( KPEngCntLstSubscribe,
                                           KPEngCntLstPropertyNativePermanent );

                    sett->DeletePropertyL( KPEngSubcriptionUpdate,
                                           KPEngCntLstPropertyNativeCached );
                    }
                aExistingLists.Delete( cn );
                planeNames->Delete( cn );
                break;
                }
            }


        //Process the case that list was not among server lists
        if ( cn == -1 )
            {
            //List was not found from plain names
            if ( !sett->Property( KPEngCntLstSyncMaster, KPEngCntLstPropertyNativePermanent ) &&
                 sett->Property( KPEngCntLstMustExistOnServer, KPEngCntLstPropertyNativePermanent ) )
                {
                CompleteCntListDeletionL( sett->Name() );
                }
            else
                {
                sett->DeletePropertyL( KPEngListExistsOnServer, KPEngCntLstPropertyNativeCached );
                }
            }
        }


    //Process those lists which were left over in the existing lists array
    //=> those are new ones that needs to be registered to local system
    const TInt remainingCount( aExistingLists.Count() );
    for ( TInt y( 0 ) ; y < remainingCount ; ++y )
        {
        CPEngContactListSettings* sett =
            CPEngContactListSettings::NewServerLC( ( *planeNames )[ y ],
                                                   aExistingLists[ y ],
                                                   *this );

        CPEngContactListManagerBase::InsertContactListSettingsL( *sett );
        CleanupStack::Pop( sett );

        sett->SetPropertyL( KPEngListExistsOnServer,
                            KPEngCntLstPropertyNativeCached,
                            ETrue );
        }


    //List of lists handled
    CleanupStack::PopAndDestroy(); // planeNames
    iEnviromentSynchronized = ETrue;



    // Handle the default contact list
    TPtrC defaultList( aDefaultList );
    NContactListIdsTools::ExtractPureContactListName( defaultList,
                                                      iPlainUserName );

    CPEngContactListSettings* sett = ContactListSettingsOrNull( defaultList );
    if ( sett )
        {
        sett->SetPropertyL( KPEngCntLstDefault,
                            KPEngCntLstPropertyNativePermanent,
                            ETrue );
        }
    else
        {
        //Force store of contact list manager,
        //to distribute the enviroment sync state
        StoreL();
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::UserDomain()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngContactListTransactionManager::UserDomain()
    {
    return iUserDomain;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::DefaultCntListSettingsOrNull()
// -----------------------------------------------------------------------------
//
CPEngContactListSettings* CPEngContactListTransactionManager::DefaultCntListSettingsOrNull()
    {
    return CPEngContactListManagerBase::DefaultCntListSettingsOrNull();
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CompleteCntListDeletionL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::CompleteCntListDeletionL(
    const TDesC& aContactList )
    {
    DeleteContactListSettingsL( aContactList );
    }



// =============================================================================
// =============== From main class =============================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetOutGoingTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetOutGoingTransactionHandlersL(
    const TDesC& aData,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransactionManager::GetOutGoingTransactionHandlersL()" ) );
    if ( aData.Length() == 0 )
        {
        return;
        }

    // unpack SIDs, creates sIds and its count
    CDesC16Array* sIds = PEngMessagePacker::UnpackDesArrayLC( aData );
    TInt countStoreIDs( sIds->Count() );

    // loop going through all requested Store IDs
    for ( TInt x( 0 ) ; x < countStoreIDs ; x++ )
        {
        // try to load contact list from store
        CPEngContactListModBase* volatile model = NULL;
        TRAPD( e, model = &( LoadContactListModelL( sIds->MdcaPoint( x ),
                                                    ETrue,
                                                    ETrue ) ) );

        // if list does not exists, continue for next SID
        if ( e == KErrNotFound )
            {
            continue;
            }

        User::LeaveIfError( e );
        CleanupClosePushL( *model );

        // is update already in progress?
        if ( model->UpdateInProgress() )
            {
            User::Leave( KErrInUse );
            }

        // clean change monitor
        model->ChangeMonitor().Reset();
        model->StoreEntryL();


        // is it watcher list?
        if ( KErrNone ==  sIds->MdcaPoint( x ).CompareF( KPEngWatcherList ) )
            {
            GetWatcherListHandlerL( *model,
                                    aHandlers,
                                    aOperationId );
            CleanupStack::PopAndDestroy(); // model
            continue;
            }


        // is this delete contact list request
        if ( model->Settings().Property( KPEngContactListDeleteNeeded,
                                         KPEngCntLstPropertyNativeCached ) )
            {
            GetDeleteCntListHandlerL( *model,
                                      aHandlers,
                                      aOperationId );
            CleanupStack::PopAndDestroy(); // model
            continue;
            }


        // is list synchronized
        if ( model->Settings().Synchronized() )
            {
            GetContactListsUpdateHandlersL( *model,
                                            aHandlers,
                                            aOperationId );
            }
        else
            {
            GetContactListsSyncHandlersL( *model,
                                          aHandlers,
                                          aOperationId );
            }
        CleanupStack::PopAndDestroy(); // model
        }

    CleanupStack::PopAndDestroy( sIds );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetContactListUpdateHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetContactListsUpdateHandlersL(
    CPEngContactListModBase& aContactList,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    CPEngContactListSettings& settings = aContactList.Settings();
    if ( settings.Property( KPEngListExistsOnServer,
                            KPEngCntLstPropertyNativeCached ) )
        {
        GetUpdateHandlersForExistingCntListL( aContactList,
                                              aHandlers,
                                              aOperationId );

        // check if subscription needs some update
        if ( settings.Property( KPEngSubcriptionUpdate,
                                KPEngCntLstPropertyNativeCached ) )
            {
            GetSubscriptionUpdateHandlerL( aContactList,
                                           aHandlers,
                                           aOperationId );
            }

        // is property update needed
        if ( settings.Property( KPEngPropertiesUpdateNeeded,
                                KPEngCntLstPropertyNativeCached ) )
            {
            GetUpdateCntListPropertiesHandlerL( aContactList,
                                                aHandlers,
                                                aOperationId  );
            }
        }

    else
        {
        GetUpdateHandlersForNotExistingCntListL( aContactList,
                                                 aHandlers,
                                                 aOperationId );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetContactListSyncHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetContactListsSyncHandlersL(
    CPEngContactListModBase& aContactList,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    // check if there is need to created any handler
    CPEngContactListSettings& settings = aContactList.Settings();

    if ( // list must exists on the server -> need transaction
        ( settings.Property( KPEngCntLstMustExistOnServer,
                             KPEngCntLstPropertyNativePermanent ) )
        ||
        // list exists on the server -> need of transaction
        ( settings.Property( KPEngListExistsOnServer,
                             KPEngCntLstPropertyNativeCached ) )
        ||
        // is anything in the contact list -> sync it
        ( aContactList.Count() != 0 ) )
        {
        // create sync handler
        CleanContactListSettingsForListCreationL( aContactList.Settings() );
        GetSynchronizeOneCntListHandlerL( aContactList,
                                          aHandlers,
                                          aOperationId );
        }
    else
        {
        // there was no need to create sync transaction for
        // the contact list, set it as synchronized
        settings.SetPropertyL( KPEngHasBeenSynchronized,
                               KPEngCntLstPropertyNativeCached,
                               ETrue );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetDeleteCntListHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetDeleteCntListHandlerL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    // check if contact list exists on the presence server, can be that it is just
    // lazy initialized
    if ( aModel.Settings().Property( KPEngListExistsOnServer,
                                     KPEngCntLstPropertyNativeCached ) )
        {
        // handler is needed, create it here
        CreateHandler2ParamL < CPEngContactListDeleteTrans,
        CPEngContactListModBase&,
        MPEngContactListTransactionManager& > ( aHandlers,
                                                aModel,
                                                *this,
                                                iCSPVersion,
                                                aOperationId );
        }
    else
        {
        // no need to create handler, just commit change on the store
        CompleteCntListDeletionL( aModel.Settings().Name() );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetUpdateCntListPropertiesHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetUpdateCntListPropertiesHandlerL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    CreateHandler2ParamL < CPEngContactListMngTransProperties,
    CPEngContactListModBase&,
    MPEngContactListTransactionManager& > ( aHandlers,
                                            aModel,
                                            *this,
                                            iCSPVersion,
                                            aOperationId );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetUpdateHandlersForExistingCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetUpdateHandlersForExistingCntListL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    RPointerArray<CPEngContactListModItemContainer> nickList;
    CleanupClosePushL( nickList );

    // check is there is something to update
    aModel.FillAddNickListL( nickList );
    if ( 0 != nickList.Count() )
        {
        // something to update => create update handler
        CreateHandler2ParamL < CPEngContactListMngTransAddId,
        CPEngContactListModBase&,
        CPEngSessionSlotId& > ( aHandlers,
                                aModel,
                                *iSessionId,
                                iCSPVersion,
                                aOperationId );
        }


    nickList.Reset();
    aModel.FillRemoveNickListL( nickList );
    if ( 0 != nickList.Count() )
        {
        // something to update => create update handler
        CreateHandler2ParamL < CPEngContactListMngTransRemoveId,
        CPEngContactListModBase&,
        CPEngSessionSlotId& > ( aHandlers,
                                aModel,
                                *iSessionId,
                                iCSPVersion,
                                aOperationId );
        }

    CleanupStack::PopAndDestroy(); // nickList
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetUpdateHandlersForNotExistingCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetUpdateHandlersForNotExistingCntListL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    // delete properties which are not needed
    CleanContactListSettingsForListCreationL( aModel.Settings() );

    if ( aModel.Count() != 0 )
        {
        // something to update => create update handler
        CreateHandler3ParamL < CPEngContactListTransCreate,
        CPEngContactListModBase&,
        CPEngSessionSlotId&,
        MPEngContactListTransactionManager& > ( aHandlers,
                                                aModel,
                                                *iSessionId,
                                                *this,
                                                iCSPVersion,
                                                aOperationId );
        }
    else
        {
        // Harmonize default contact list settings if list was configured as new one
        NListLibTools::HarmonizeDefaultContactListSettingsL( aModel.Settings(), *this );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetWatcherListHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetWatcherListHandlerL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    CreateHandler1ParamL < CPEngGetWatchersTransaction,
    CPEngContactListModBase& > ( aHandlers,
                                 aModel,
                                 iCSPVersion,
                                 aOperationId );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetSynchronizeOneCntListHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetSynchronizeOneCntListHandlerL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {

    if ( aModel.Settings().Property( KPEngListExistsOnServer,
                                     KPEngCntLstPropertyNativeCached ) )
        {
        CreateHandler3ParamL < CPEngContactListTransSync,
        CPEngContactListModBase&,
        CPEngSessionSlotId&,
        MPEngContactListTransactionManager& > ( aHandlers,
                                                aModel,
                                                *iSessionId,
                                                *this,
                                                iCSPVersion,
                                                aOperationId );

        }

    else
        {
        CreateHandler3ParamL < CPEngContactListTransCreate,
        CPEngContactListModBase&,
        CPEngSessionSlotId&,
        MPEngContactListTransactionManager& > ( aHandlers,
                                                aModel,
                                                *iSessionId,
                                                *this,
                                                iCSPVersion,
                                                aOperationId );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::GetSubscriptionUpdateHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::GetSubscriptionUpdateHandlerL(
    CPEngContactListModBase& aModel,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TInt aOperationId )
    {
    CreateHandler2ParamL < CPEngContactListMngTransSubcription,
    CPEngContactListModBase&,
    CPEngSessionSlotId& > ( aHandlers,
                            aModel,
                            *iSessionId,
                            iCSPVersion,
                            aOperationId );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CreateHandler1ParamL()
// -----------------------------------------------------------------------------
//
template < class THandlerClass,
class TParam1 >
void CPEngContactListTransactionManager::CreateHandler1ParamL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TParam1 aParam1,
    TPEngWVCspVersion& aParam2,
    TInt aParam3 )
    {
    // create start synchronization handler
    THandlerClass* newHandler = THandlerClass::NewLC( aParam1,
                                                      aParam2,
                                                      aParam3 );

    aHandlers.AppendL( newHandler );
    CleanupStack::Pop(); // newHandler
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CreateHandler2ParamL()
// -----------------------------------------------------------------------------
//
template < class THandlerClass,
class TParam1,
class TParam2 >
void CPEngContactListTransactionManager::CreateHandler2ParamL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TParam1 aParam1,
    TParam2 aParam2,
    TPEngWVCspVersion& aParam3,
    TInt aParam4 )
    {
    THandlerClass* newHandler = THandlerClass::NewLC( aParam1,
                                                      aParam2,
                                                      aParam3,
                                                      aParam4 );

    aHandlers.AppendL( newHandler );
    CleanupStack::Pop(); //newHandler
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CreateHandler3ParamL()
// -----------------------------------------------------------------------------
//
template < class THandlerClass,
class TParam1,
class TParam2,
class TParam3 >
void CPEngContactListTransactionManager::CreateHandler3ParamL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
    TParam1 aParam1,
    TParam2 aParam2,
    TParam3 aParam3,
    TPEngWVCspVersion& aParam4,
    TInt aParam5 )
    {
    THandlerClass* newHandler = THandlerClass::NewLC( aParam1,
                                                      aParam2,
                                                      aParam3,
                                                      aParam4,
                                                      aParam5 );

    aHandlers.AppendL( newHandler );
    CleanupStack::Pop(); // newHandler
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::InitializePlainUserId()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::InitializePlainUserId()
    {
    TChar add( '@' );
    TInt offset( iSessionId->UserId().Locate( add ) );
    // remove domain
    if ( KErrNotFound != offset )
        {
        iPlainUserName.Set( iSessionId->UserId().Left( offset ) );
        }
    else
        {
        iPlainUserName.Set( iSessionId->UserId() );
        }

    // remove wv ID begining wv:
    if ( KErrNone == iPlainUserName.Left( KPEngWVIDPrefixLength
                                        ).CompareF( KPEngWVIDPrefix ) )
        {
        iPlainUserName.Set( iPlainUserName.Mid( KPEngWVIDPrefixLength ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransactionManager::CleanContactListSettingsForListCreationL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransactionManager::CleanContactListSettingsForListCreationL(
    CPEngContactListSettings& aSettings )
    {
    // Clean contact list settings which are not needed when contact
    // list will be created
    aSettings.DeletePropertyL( KPEngSubcriptionUpdate,
                               KPEngCntLstPropertyNativeCached );
    aSettings.DeletePropertyL( KPEngPropertiesUpdateNeeded,
                               KPEngCntLstPropertyNativeCached );
    }

//  End of File





