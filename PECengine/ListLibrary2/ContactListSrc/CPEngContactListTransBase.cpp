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
* Description:  Base class for contact list create and synchronize handlers
*
*/


// INCLUDE FILES
#include "CPEngContactListTransBase.h"
#include "CPEngContactListSettings.h"
#include "CPEngContactListModBase.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListTransBase::CPEngContactListTransBase()
// -----------------------------------------------------------------------------
//
CPEngContactListTransBase::CPEngContactListTransBase(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iContactList( aContactList ),
        iSessionSlotId( aSessionSlotId ),
        iManager( aManager ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )
    {
    iContactList.Open();                                // CSI: 65 #
    iContactList.Lock( EStorageLockLevelHigh );
    iContactList.BufferServerSideNotifications();
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::ConstructL()
    {
    iTransactionStatus = CPEngTransactionStatus::NewL();
    }


// Destructor
CPEngContactListTransBase::~CPEngContactListTransBase()
    {
    delete iTransactionStatus;

    iContactList.ReleaseServerSideBuffering();
    iContactList.Unlock();
    iContactList.Close();
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::RequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    switch ( iContactListSyncState )
        {
        case EPEngCntListCreate:
            DoGetXMLCreateCntListL( *xmlSerializer );
            break;

        case EPEngCntListFetch:
            DoGetXMLFetchCntListL( *xmlSerializer );
            break;

        case EPEngSubscribeCntList:
            GetXMLSubscribeListL( *xmlSerializer );
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }


    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }

// -----------------------------------------------------------------------------
// CPEngContactListTransBase::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::OutgoingProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::ProcessResponseL( const TDesC8& aResponse,
                                                  TRequestStatus& /* aStatus */ )
    {
    MPEngXMLParser* XMLparser = CreateXMLParserLC();

    switch ( iContactListSyncState )
        {
        case EPEngCntListCreate:
            {
            DoParseCreateCntListResponseL( aResponse, *XMLparser );
            break;
            }


        case EPEngCntListFetch:
            {
            DoParseFetchCntListResponseL( aResponse, *XMLparser );
            break;
            }


        case EPEngSubscribeCntList:
            {
            ParseSubscribeListResponseL( aResponse, *XMLparser );
            break;
            }


        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // XMLparser
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aHandlers */ )
    {
    }

// -----------------------------------------------------------------------------
// CPEngContactListTransBase::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListTransBase::TransactionCompleted()
    {
    return ( iContactListSyncState == EPEngTransactionCompleted );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngContactListTransBase::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::ReleaseHandler()
    {
    iContactListSyncState = EPEngTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::CompleteCntListTransactionL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::CompleteCntListTransactionL()
    {
    iContactListSyncState = EPEngTransactionCompleted;
    iContactList.Settings().SetPropertyL( KPEngHasBeenSynchronized,
                                          KPEngCntLstPropertyNativeCached,
                                          ETrue );
    iContactList.Settings().SetPropertyL( KPEngListExistsOnServer,
                                          KPEngCntLstPropertyNativeCached,
                                          ETrue );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::GetXMLSubscribeList()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::GetXMLSubscribeListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <SubscribePresence-Request>
    aXmlSerializer.StartTagL( KSubscribePresence );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer,
                                              iContactList.Settings() );


    // append set of attributes which should be subscribed
    NListLibTools::AppendAttributesForSubscriptionXmlL(
        iContactList.Settings().SubscriptionAttributes(),
        aXmlSerializer,
        iSessionSlotId,
        iCSPVersion );


    // if server supports 1.2 CSP, use autosubscribe
    if ( iCSPVersion == EWVCspV12 )
        {
        // <AutoSubscribe>T</AutoSubscribe>
        aXmlSerializer.StartTagL( KAutoSubscribe ).
        NarrowTextL( KXMLValueTrue ).
        EndTagL( KAutoSubscribe );
        }


    // </SubscribePresence-Request>
    aXmlSerializer.EndTagL( KSubscribePresence );


    // register contacts to the Attribute library
    TPEngContactlistDesViewWraper contacts( iContactList );
    NListLibTools::ActivatePresenceAttributesL( contacts, iSessionSlotId );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransBase::ParseSubscribeListResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransBase::ParseSubscribeListResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aXMLparser )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransBase::ParseSubscribeListResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    // try if there were some errors
    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();
    CleanupClosePushL( *transactionStatus );
    TInt err ( aXMLparser.ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.StorageId(),
                                                   *transactionStatus ) );
    switch ( err )
        {
        case KPEngNwErrPartiallySuccessful:
            {
            // mark for remove bad contacts,
            // transaction will also unregister their attribute models
            NListLibTools::MarkForRemoveBadContactsL( *transactionStatus, iContactList );
            }

        // fall through
        case KErrNone:
            {
            CompleteCntListTransactionL();
            transactionStatus->SetStatus( KErrNone );
            break;
            }


        case KPEngNwErrUnknownUser:
            {
            // unregister attribute models
            TPEngContactlistDesViewWraper contacts( iContactList );
            NListLibTools::DeactivatePresenceAttributesL( contacts, iSessionSlotId );

            // all contact Ids are wrong, remove them
            iContactList.ResetLocalView();
            CompleteCntListTransactionL();
            break;
            }


        default:
            {
            CompleteCntListTransactionL();
            }
        }

    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop(); // transactionStatus

    PENG_DP( D_PENG_LIT( "CPEngContactListTransBase::ParseSubscribeListResponseL() done [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }

//  End of File

