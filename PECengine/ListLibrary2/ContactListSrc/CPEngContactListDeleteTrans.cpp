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
* Description:  Handler to delete contact list.
*
*/


// INCLUDE FILES
#include "CPEngContactListDeleteTrans.h"

#include "MPEngContactListTransactionManager.h"
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
// CPEngContactListDeleteTrans::CPEngContactListDeleteTrans()
// -----------------------------------------------------------------------------
//
CPEngContactListDeleteTrans::CPEngContactListDeleteTrans(
    CPEngContactListModBase& aContactList,
    MPEngContactListTransactionManager& aSettingsManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iContactList( aContactList ),
        iSettingsManager( aSettingsManager ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListDeleteTrans::CPEngContactListDeleteTrans() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    iContactList.Open();                // CSI: 65 #
    iContactList.Lock( EStorageLockLevelHigh );
    }

// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();

    if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                           KPEngCntLstPropertyNativePermanent ) )
        {
        iTransactionState = EPEngUnSubscribeContactList;
        }
    else
        {
        iTransactionState = EPEngDeleteContactList;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListDeleteTrans* CPEngContactListDeleteTrans::NewLC(
    CPEngContactListModBase& aContactList,
    MPEngContactListTransactionManager& aSettingsManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListDeleteTrans* self =
        new ( ELeave ) CPEngContactListDeleteTrans(
        aContactList,
        aSettingsManager,
        aCSPVersion,
        aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngContactListDeleteTrans::~CPEngContactListDeleteTrans()
    {
    iContactList.Unlock();
    iContactList.Close();
    delete iTransactionStatus;
    }


// ==============================================================================
// =============Function from MPEngOutgoingTransactionHandler =================
// ==============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::RequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    switch ( iTransactionState )
        {
        case EPEngUnSubscribeContactList:
            {
            PENG_DP( D_PENG_LIT( "CPEngContactListDeleteTrans::RequestL() unsubscribe [%S]" ),
                     &( iContactList.ListProperties().Name() ) );

            GetXMLUnsubscribeContactListL( *xmlSerializer );
            break;
            }


        case EPEngDeleteContactList:
            {
            PENG_DP( D_PENG_LIT( "CPEngContactListDeleteTrans::RequestL() delete [%S]" ),
                     &( iContactList.ListProperties().Name() ) );

            GetXMLDeleteContactListL( *xmlSerializer );
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
// CPEngContactListDeleteTrans::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::ProcessResponseL( const TDesC8& aResponse,
                                                    TRequestStatus& /* aStatus */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListDeleteTrans::ProcessResponseL() [%S] state %d" ),
             &( iContactList.ListProperties().Name() ), iTransactionState );

    MPEngXMLParser* XMLparser = CreateXMLParserLC();

    // check if we have some answer from the server
    TInt err ( XMLparser->ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.Settings().Name(),
                                                   *iTransactionStatus ) );
    // how did transaction go
    switch ( err )
        {
        case KErrNone: // transaction OK
            {
            // return, continue with transaction
            iTransactionState++; // this will go one state forward
            // if transaction is completed, do not break,
            // other switch case will do cleaning
            if ( iTransactionState != EPEngTransactionCompleted )
                {
                break;
                }
            }

        case KPEngNwErrContactListDoesNotExist:
            {
            // We wanted to delete list anyway
            // remove contact list settings from store
            iSettingsManager.CompleteCntListDeletionL( iContactList.Settings().Name() );
            iTransactionState = EPEngTransactionCompleted;
            break;
            }

        default:
            {
            // some error has occured, complete transaction and return
            iTransactionState = EPEngTransactionCompleted;
            }
        }


    CleanupStack::PopAndDestroy(); // XMLparser
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListDeleteTrans::TransactionCompleted()
    {
    return ( iTransactionState == EPEngTransactionCompleted );
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngContactListDeleteTrans::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::ReleaseHandler()
    {
    iTransactionState =  EPEngTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::GetXMLUnsubscribeContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::GetXMLUnsubscribeContactListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <UnsubscribePresence-Request>
    aXmlSerializer.StartTagL( KUnsubscribePresence );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    //  </UnsubscribePresence-Request>
    aXmlSerializer.EndTagL( KUnsubscribePresence );
    }


// -----------------------------------------------------------------------------
// CPEngContactListDeleteTrans::GetXMLDeleteContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListDeleteTrans::GetXMLDeleteContactListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <DeleteList-Request>
    aXmlSerializer.StartTagL( KDeleteList );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer, iContactList.Settings() );

    // </DeleteList-Request>
    aXmlSerializer.EndTagL( KDeleteList );
    }

//  End of File










































