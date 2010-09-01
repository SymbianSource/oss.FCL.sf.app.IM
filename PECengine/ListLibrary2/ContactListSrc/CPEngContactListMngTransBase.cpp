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
* Description:  Base class of the contact list management handlers.
*
*/

#include "CPEngContactListMngTransBase.h"

#include "CPEngContactListModChangeMonitor.h"
#include "CPEngContactListSettings.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"
#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"

#include <e32std.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::CPEngContactListMngTransBase()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransBase::CPEngContactListMngTransBase(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iContactList( aContactList ),
        iSessionSlotId( aSessionSlotId ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )
    {
    iContactList.Open();                            // CSI: 65 #
    iContactList.Lock( EStorageLockLevelHigh );
    iContactList.BufferServerSideNotifications();
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// Destructor
CPEngContactListMngTransBase::~CPEngContactListMngTransBase()
    {
    iContactList.ReleaseServerSideBuffering();
    iContactList.Unlock();
    iContactList.Close();
    delete iTransactionStatus;
    delete iRollBackIDsArray;
    }


// ==============================================================================
// =============Function from MPEngOutgoingTransactionHandler =================
// ==============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::RequestL( TDes8& aSendBuffer )
    {
    // refresh contact from store
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    switch ( iUpdateState )
        {
        case EPEngListContentUpdate:
            {
            CPEngContactListMngTransBase::GetXMLContactListUpdateL( *xmlSerializer );
            break;
            }

        case EPEngSubscriptionUpdate:
            {
            DoGetXMLSubscriptionUpdateL( *xmlSerializer );
            break;
            }

        case EPEngRollBack:
            {
            CPEngContactListMngTransBase::GetXMLRollBackUpdateL( *xmlSerializer );
            break;
            }

        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    // is it list update response or subscription update response
    switch ( iUpdateState )
        {
        case EPEngListContentUpdate:
            {
            CPEngContactListMngTransBase::ParseListUpdateResponseL( aResponse );
            break;
            }
        case EPEngSubscriptionUpdate:
            {
            DoParseSubscriptionUpdateResponseL( aResponse );
            break;
            }
        case EPEngRollBack:
            {
            CPEngContactListMngTransBase::ParseListUpdateResponseL( aResponse );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListMngTransBase::TransactionCompleted()
    {
    return ( iUpdateState == EPEngTransactionCompleted );
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngContactListMngTransBase::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::ReleaseHandler( )
    {
    iUpdateState = EPEngTransactionCompleted;
    }



// =============================================================================
// ===============Functions of main class ======================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::GetXMLContactListUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::GetXMLContactListUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <ListManage-Request>
    aXmlSerializer.StartTagL( KListMnageRequest );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    DoAppendListNickUpdateL( aXmlSerializer );

    // <ReceiveList>T</ReceiveList>
    NListLibTools::ApppendReceiveListTagL( aXmlSerializer, iCSPVersion, ETrue );

    // </ListManage-Request>
    aXmlSerializer.EndTagL( KListMnageRequest );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::GetXMLRollBackUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::GetXMLRollBackUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <ListManage-Request>
    aXmlSerializer.StartTagL( KListMnageRequest );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    // <RemoveNickList>
    aXmlSerializer.StartTagL( KRemoveNickList );

    // insert contact Ids for rollback
    TInt count( iRollBackIDsArray->Count() );
    for ( TInt x( 0 ); x < count ; x++ )
        {
        // <UserID>
        // Contact list id value
        // </UserID>
        aXmlSerializer.StartTagL( KUserIDXMLTag
                                ).WvAddressL( iRollBackIDsArray->MdcaPoint( x )
                                            ).EndTagL( KUserIDXMLTag );
        }

    // </RemoveNickList>
    aXmlSerializer.EndTagL( KRemoveNickList );

    // <ReceiveList>T</ReceiveList>
    NListLibTools::ApppendReceiveListTagL( aXmlSerializer, iCSPVersion, ETrue );

    // </ListManage-Request>
    aXmlSerializer.EndTagL( KListMnageRequest );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransBase::ParseListUpdateResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransBase::ParseListUpdateResponseL(
    const TDesC8& aResponse )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransBase::ParseListUpdateResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    // clean attribute models if this is roll back
    if ( iUpdateState == EPEngRollBack )
        {
        NListLibTools::DeactivatePresenceAttributesL( *iRollBackIDsArray, iSessionSlotId );
        }

    // consume response
    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();
    CleanupClosePushL( *transactionStatus );

    MPEngXMLParser* xMLparser1 = CreateXMLParserLC();
    TInt err ( xMLparser1->ParseContactListResultL( aResponse,
                                                    iOperationId,
                                                    iContactList.StorageId(),
                                                    *transactionStatus ) );

    MPEngXMLParser* xMLparser2 = CreateXMLParserLC();
    switch ( err )
        {
        case KErrNone:
        case KPEngNwErrPartiallySuccessful:
            {
            // try to parse Nick list at any case, can be that Server is giving us
            // hook what is at the moment on the contact list
            // get one more parser for this part
            RReffArray<CPEngContactListModItemContainer> newContacts;
            CleanupClosePushL( newContacts );
            if ( NListLibTools::ParseContactListNickListL( aResponse,
                                                           newContacts,
                                                           iContactList,
                                                           *xMLparser1,
                                                           *xMLparser2,
                                                           ETrue,
                                                           &iNewIdsCount ) )
                {
                iContactList.AdoptNetworkViewL( newContacts );
                // remove bad contacts from the
                NListLibTools::RemoveBadContactsL( *transactionStatus, iContactList );
                }
            else
                {
                // nick list was not there, consume result manually
                NListLibTools::RemoveBadContactsL( *transactionStatus, iContactList );
                // complete contact list update
                DoCompleteContactListUpdateL();
                }

            CleanupStack::PopAndDestroy(); // newContacts
            DoUpdateTransactionState();
            break;
            }


        case KPEngNwErrUnknownUser:
        case KPEngNwErrServiceUnavailable:
        case KPEngNwErrBadParameter:
        default:
            {
            iTransactionStatus->SetStatus( transactionStatus->Status() );
            transactionStatus->Reset();

            iUpdateState =  EPEngTransactionCompleted;
            //  fill manually transaction result
            RPointerArray<CPEngContactListModItemContainer> nickList;
            DoFillNickListLC( nickList );
            // go through array from end
            TInt contactCount ( nickList.Count() - 1 );
            for (  ; contactCount >= 0 ; --contactCount )
                {
                CPEngContactListModItemContainer* entry = nickList[ contactCount ];


                // add bad contact it to the transaction status
                TPtrC badContactId ( entry->Id() );

                iTransactionStatus->AddDetailedResultL(
                    iOperationId,
                    KPEngNwErrUnknownUser,
                    NULL,
                    &badContactId,
                    &( iContactList.StorageId() ),
                    NULL );

                if ( entry->IsFreshContact() )
                    {
                    iContactList.RemoveBadContactL( badContactId );
                    }
                else
                    {
                    entry->RollBackNickname();
                    iContactList.StoreL();
                    }
                }

            CleanupStack::PopAndDestroy(); // nickList

            // try to parse response if there was contact list content
            RReffArray<CPEngContactListModItemContainer> newContacts;
            CleanupClosePushL( newContacts );
            if ( NListLibTools::ParseContactListNickListL( aResponse,
                                                           newContacts,
                                                           iContactList,
                                                           *xMLparser1,
                                                           *xMLparser2,
                                                           ETrue ) )
                {
                iContactList.AdoptNetworkViewL( newContacts );
                }

            CleanupStack::PopAndDestroy(); // newContacts
            break;
            }
        }


    // register new added contact Ids if this is subscription
    if ( ( iUpdateState == EPEngSubscriptionUpdate ) ||
         ( iUpdateState == EPEngActivateAttributes ) )
        {
        CPEngContactListModChangeMonitor& changeMonitor = iContactList.ChangeMonitor();
        NListLibTools::ActivatePresenceAttributesL( changeMonitor.AddedContactIds(),
                                                    iSessionSlotId );
        // update state if needed
        iUpdateState = ( iUpdateState == EPEngActivateAttributes ? EPEngTransactionCompleted
                         : iUpdateState );
        }


    // deactivate removed contact Ids if needed
    if ( iUpdateState == EPEngDeActivateAttributes )
        {
        CPEngContactListModChangeMonitor& changeMonitor = iContactList.ChangeMonitor();
        NListLibTools::DeactivatePresenceAttributesL( changeMonitor.RemovedContactIds(),
                                                      iSessionSlotId );
        iUpdateState = EPEngTransactionCompleted;
        }


    CleanupStack::PopAndDestroy( 2 ); // xMLparser2, xMLparser1

    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop(); // transactionStatus

    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransBase::ParseListUpdateResponseL() done [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }

//  End of File

