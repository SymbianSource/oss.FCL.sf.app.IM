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
* Description:  Handler to update contact list subscription.
*
*/

// INCLUDE FILES
#include "CPEngContactListMngTransSubcription.h"

#include "MPEngContactListSettingsManager.h"
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
// CPEngContactListMngTransSubcription::CPEngContactListMngTransSubcription()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransSubcription::CPEngContactListMngTransSubcription(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iContactList ( aContactList ),
        iSessionSlotId( aSessionSlotId ),
        iTransactionCompleted( EFalse ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )

    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransSubcription::CPEngContactListMngTransSubcription() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    iContactList.Open();            // CSI: 65 #
    iContactList.Lock( EStorageLockLevelHigh );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransSubcription* CPEngContactListMngTransSubcription::NewLC(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListMngTransSubcription* self =
        new ( ELeave ) CPEngContactListMngTransSubcription( aContactList,
                                                            aSessionSlotId,
                                                            aCSPVersion,
                                                            aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngContactListMngTransSubcription::~CPEngContactListMngTransSubcription()
    {
    iContactList.Unlock();
    iContactList.Close();
    delete iTransactionStatus;
    }



// ===========================================================================
// =============Function from MPEngOutgoingTransactionHandler ================
// ===========================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::RequestL(
    TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    // check if contact list needs to be re-subscribed, or unsubscribed
    if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                           KPEngCntLstPropertyNativePermanent ) )
        {
        GetXMLSubscribeListL( *xmlSerializer );
        }
    else
        {
        GetXMLUnsubscribeListL( *xmlSerializer );
        }

    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }

// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    MPEngXMLParser* XMLparser = CreateXMLParserLC();

    // try if there were some errors
    // check what kind of the request was it
    if ( iContactList.Settings().Property( KPEngCntLstSubscribe ,
                                           KPEngCntLstPropertyNativePermanent ) )
        {
        ParseSubscribeListResponseL( aResponse, *XMLparser );
        }
    else
        {
        ParseUnsubscribeListResponseL( aResponse, *XMLparser );
        }


    CleanupStack::PopAndDestroy(); // XMLparser
    iTransactionCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListMngTransSubcription::TransactionCompleted()
    {
    return iTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngContactListMngTransSubcription::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::ReleaseHandler()
    {
    iTransactionCompleted = ETrue;
    }


// =============================================================================
// ==============NEW FUNCTIONS of the base class ===============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::GetXMLContactListUpdateL()
// Creates XML message to unsubscribe contact lists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::GetXMLUnsubscribeListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <UnsubscribePresence-Request>
    aXmlSerializer.StartTagL( KUnsubscribePresence );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    //  </UnsubscribePresence-Request>
    aXmlSerializer.EndTagL( KUnsubscribePresence );
    }

// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::GetXMLSubscribeList()
// Creates XML message to re-subscribe contact list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::GetXMLSubscribeListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <SubscribePresence-Request>
    aXmlSerializer.StartTagL( KSubscribePresence );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer,
                                              iContactList.Settings() );

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

    // init attribute models
    TPEngContactlistDesViewWraper contacts( iContactList );
    NListLibTools::ActivatePresenceAttributesL( contacts, iSessionSlotId );
    }

// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::ParseUnsubscreAllResponseL()
// Parse XML Unsubscribe contact list response
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::ParseUnsubscribeListResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aXMLparser )
    {
    // if contact list was not unsubscribed successfully,
    // just relax, it will be done by next log in anyway
    TInt err ( aXMLparser.ParseResultL( aResponse,
                                        iOperationId,
                                        *iTransactionStatus ) );
    // ignore
    switch ( err )
        {
        case KErrNone:
            // partial success is same as success in this case
        case KPEngNwErrPartiallySuccessful:
            {
            iContactList.Settings().DeletePropertyL(
                KPEngSubcriptionUpdate,
                KPEngCntLstPropertyNativeCached );
            TPEngContactlistDesViewWraper contacts( iContactList );
            NListLibTools::DeactivatePresenceAttributesL( contacts, iSessionSlotId );
            break;
            }
        default:
            {
            // client will be notified about fail by Transaction Status
            break;
            }
        }
    }
// -----------------------------------------------------------------------------
// CPEngContactListMngTransSubcription::ParseSubscribeListResponseL()
// Parse XML re-subscribe contact list response
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransSubcription::ParseSubscribeListResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aXMLparser )
    {
    TInt err ( aXMLparser.ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.StorageId(),
                                                   *iTransactionStatus ) );
    switch ( err )
        {
            //
        case KPEngNwErrPartiallySuccessful:
            {
            // only wrong contact Ids are checked here
            NListLibTools::MarkForRemoveBadContactsL( *iTransactionStatus, iContactList );
            break;
            }

        case KErrNone:
            {
            // Contact list was subscribed successfully
            iContactList.Settings().DeletePropertyL(
                KPEngSubcriptionUpdate,
                KPEngCntLstPropertyNativeCached );
            break;
            }

        case KPEngNwErrUnknownUser:
            {
            // deactivate attribute models
            TPEngContactlistDesViewWraper contacts( iContactList );
            NListLibTools::DeactivatePresenceAttributesL( contacts, iSessionSlotId );
            // all contact Ids are wrong, remove them
            iContactList.ResetLocalView();
            break;
            }
        case KPEngNwErrContactListDoesNotExist:
            {
            iContactList.Settings().DeletePropertyL(
                KPEngListExistsOnServer,
                KPEngCntLstPropertyNativeCached );
            break;
            }
        // ignore all other errors
        default:
            {
            }
        }
    }

//  End of File



















