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
* Description:  Handler to synchronize contact list
*
*/

// INCLUDE FILES
#include "CPEngContactListTransSync.h"

#include "CPEngContactListModItemContainer.h"
#include "CPEngContactListSettings.h"
#include "CPEngContactListModBase.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"

#include "CPEngContactListMngTransProperties.h"
#include "CPEngContactListMngTransAddId.h"
#include "CPEngContactListMngTransRemoveId.h"

#include "PresenceDebugPrint.h"


//Default granurality for list of additional handlers
const TInt KHandlerListGranurality = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListTransSync::CPEngContactListTransSync()
// -----------------------------------------------------------------------------
//
CPEngContactListTransSync::CPEngContactListTransSync(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : CPEngContactListTransBase( aContactList,
                                     aSessionSlotId,
                                     aManager,
                                     aCSPVersion,
                                     aOperationId ),
        iAdditionalHandlers( KHandlerListGranurality )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransSync::CPEngContactListTransSync() [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }

// -----------------------------------------------------------------------------
// CPEngContactListTransSync::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::ConstructL( )
    {
    CPEngContactListTransBase::ConstructL();
    iContactListSyncState = EPEngCntListFetch;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListTransSync* CPEngContactListTransSync::NewLC(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListTransSync* self = new ( ELeave ) CPEngContactListTransSync(
        aContactList,
        aSessionSlotId,
        aManager,
        aCSPVersion,
        aOperationId );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CPEngContactListTransSync::~CPEngContactListTransSync()
    {
    iAdditionalHandlers.ResetAndDestroy();
    }


// =============================================================================
// =============== From MPEngOutgoingTransactionHandler ========================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionsArray )
    {
    for ( TInt x( iAdditionalHandlers.Count() - 1 ) ; x >= 0 ; --x )
        {
        aTransactionsArray.AppendL( iAdditionalHandlers[ x ] );
        iAdditionalHandlers.Remove( x );
        }
    }



// =============================================================================
// =============== From CPEngSyncContactList ===================================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::DoGetXMLCreateCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::DoGetXMLCreateCntListL(
    MPEngXMLSerializer& /*aXmlSerializer*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::DoParseCreateCntListResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::DoParseCreateCntListResponseL(
    const TDesC8& /*aResponse*/,
    MPEngXMLParser& /*aXMLparser*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::DoGetXMLFetchCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::DoGetXMLFetchCntListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <ListManage-Request>
    aXmlSerializer.StartTagL( KListMnageRequest );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    // <ReceiveList>T</ReceiveList>
    NListLibTools::ApppendReceiveListTagL( aXmlSerializer, iCSPVersion, ETrue );

    // </ListManage-Request>
    aXmlSerializer.EndTagL( KListMnageRequest );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::DoParseFetchCntListResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::DoParseFetchCntListResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aXMLparser )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransSync::DoParseFetchCntListResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );


    RReffArray<CPEngContactListModItemContainer> newContacts;
    CleanupClosePushL( newContacts );

    if ( aXMLparser.DecodeL( aResponse, KCodeXMLTag, EFalse ) )
        {
        if ( 0 == aXMLparser.ResultL().CompareF( K200Code ) )
            {
            MPEngXMLParser* secondXMLparser = CreateXMLParserLC();

            // Nick name update flag
            TBool updateNickNames( !( iContactList.Settings().Property( KPEngCntLstSyncMaster,
                                                                        KPEngCntLstPropertyNativePermanent ) ) );

            // parse nick list of the contact list
            NListLibTools::ParseContactListNickListL( aResponse,
                                                      newContacts,
                                                      iContactList,
                                                      aXMLparser,
                                                      *secondXMLparser,
                                                      updateNickNames );

            ParsePropertiesL( aResponse,
                              aXMLparser,
                              *secondXMLparser );

            CleanupStack::PopAndDestroy( ); // secondXMLparser
            }
        else
            {
            iTransactionStatus->AddDetailedResultL( iOperationId,
                                                    KPEngNwErrContactListDoesNotExist,
                                                    NULL,
                                                    NULL,
                                                    &( iContactList.StorageId() ),
                                                    NULL );

            iTransactionStatus->SetStatus( KPEngNwErrContactListDoesNotExist );

            iContactList.Settings().DeletePropertyL( KPEngListExistsOnServer,
                                                     KPEngCntLstPropertyNativeCached );
            iContactListSyncState = EPEngTransactionCompleted;

            CleanupStack::PopAndDestroy(); // newContacts

            PENG_DP( D_PENG_LIT( "CPEngContactListTransSync::DoParseFetchCntListResponseL() [%S] done with error" ),
                     &( iContactList.ListProperties().Name() ) );
            return;
            }
        }
    else
        {
        // No code
        iTransactionStatus->AddDetailedResultL( iOperationId,
                                                KPEngNwErrContactListDoesNotExist,
                                                NULL,
                                                NULL,
                                                &( iContactList.StorageId() ),
                                                NULL );

        iTransactionStatus->SetStatus( KPEngNwErrContactListDoesNotExist );
        iContactListSyncState = EPEngTransactionCompleted;
        CleanupStack::PopAndDestroy(); // newContacts
        PENG_DP( D_PENG_LIT( "CPEngContactListTransSync::DoParseFetchCntListResponseL() [%S] no response" ),
                 &( iContactList.ListProperties().Name() ) );
        return;
        }


    // Figure what is new and what has happened
    if ( iContactList.Settings().Property( KPEngCntLstSyncMaster,
                                           KPEngCntLstPropertyNativePermanent ) )
        {
        iContactList.AdoptNetworkViewL( newContacts );
        CreateMasterCntListUpdateHandlersL();
        }
    else
        {
        iContactList.AdoptNewContactsL( newContacts );
        }
    CleanupStack::PopAndDestroy(); //  newContacts


    // resolve new step for the handler
    if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                           KPEngCntLstPropertyNativePermanent ) )
        {
        iContactListSyncState = EPEngSubscribeCntList;
        }
    else
        {
        CompleteCntListTransactionL();
        }

    PENG_DP( D_PENG_LIT( "CPEngContactListTransSync::DoParseFetchCntListResponseL() done [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::ParsePropertiesL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::ParsePropertiesL( const TDesC8& aResponse,
                                                  MPEngXMLParser& aXMLparser1,
                                                  MPEngXMLParser& aXMLparser2 )
    {
    if ( NListLibTools::ParseContactListPropertiesL( aResponse,
                                                     iContactList.Settings(),
                                                     aXMLparser1,
                                                     aXMLparser2 ) )
        {
        MPEngOutgoingTransactionHandler* newHandler =
            CPEngContactListMngTransProperties::NewLC( iContactList,
                                                       iManager,
                                                       iCSPVersion,
                                                       iOperationId );

        iAdditionalHandlers.AppendL( newHandler );
        CleanupStack::Pop(); // newHandler
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransSync::CreateMasterCntListUpdateHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransSync::CreateMasterCntListUpdateHandlersL()
    {
    RPointerArray<CPEngContactListModItemContainer> nickList;
    CleanupClosePushL( nickList );

    iContactList.FillAddNickListL( nickList );
    if ( 0 != nickList.Count() )
        {
        MPEngOutgoingTransactionHandler* newHandler =
            CPEngContactListMngTransAddId::NewLC( iContactList,
                                                  iSessionSlotId,
                                                  iCSPVersion,
                                                  iOperationId );
        iAdditionalHandlers.AppendL( newHandler );
        CleanupStack::Pop(); // newHandler
        }


    nickList.Reset();
    iContactList.FillRemoveNickListL( nickList );
    if ( 0 != nickList.Count() )
        {
        MPEngOutgoingTransactionHandler* newHandler =
            CPEngContactListMngTransRemoveId::NewLC( iContactList,
                                                     iSessionSlotId,
                                                     iCSPVersion,
                                                     iOperationId );

        iAdditionalHandlers.AppendL( newHandler );
        CleanupStack::Pop(); // newHandler
        }


    CleanupStack::PopAndDestroy(); // nickList
    }


//  End of File





















