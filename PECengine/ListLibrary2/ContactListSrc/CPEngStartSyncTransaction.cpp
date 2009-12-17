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
* Description:  Transction handler to do basic synchronization
*
*/


// INCLUDE FILES
#include "CPEngStartSyncTransaction.h"

#include "MPEngContactListTransactionManager.h"
#include "CPEngContactListSettings.h"
#include "CPEngContactListModBase.h"

#include "MPEngTransactionFactory.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "PEngContactIdsTools.h"
#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


//Default granurality for list of lists
const TInt KListOfListGranurality = 5;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::CPEngStartSyncTransaction()
// -----------------------------------------------------------------------------
//
CPEngStartSyncTransaction::CPEngStartSyncTransaction(
    MPEngContactListTransactionManager& aCntLstSettingsManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId,
    TBool aNeedToUnsubscribe )
        : iContactListManager( aCntLstSettingsManager ),
        iSynchronizationState( EPEngGetListOfLists ),
        iServerCntLists( KListOfListGranurality ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId ),
        iUnsubscribe( aNeedToUnsubscribe )
    {
    PENG_DP( D_PENG_LIT( "CPEngStartSyncTransaction::CPEngStartSyncTransaction()" ) );
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::NewLC()
// -----------------------------------------------------------------------------
//
CPEngStartSyncTransaction* CPEngStartSyncTransaction::NewLC(
    MPEngContactListTransactionManager& aCntLstSettingsManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId,
    TBool aNeedToUnsubscribe )
    {
    CPEngStartSyncTransaction* self = new ( ELeave ) CPEngStartSyncTransaction(
        aCntLstSettingsManager,
        aCSPVersion,
        aOperationId,
        aNeedToUnsubscribe );

    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngStartSyncTransaction::~CPEngStartSyncTransaction()
    {
    PENG_DP( D_PENG_LIT( "CPEngStartSyncTransaction::~CPEngStartSyncTransaction()" ) );
    delete iTransactionStatus;
    iServerCntLists.Reset();
    delete iDefaultCntList;
    }


// ==============================================================================
// ============= Functions from MPEngOutgoingTransactionHandler =================
// ==============================================================================

// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::RequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    switch ( iSynchronizationState )
        {
        case EPEngGetListOfLists:
            {
            GetXMLGetListOfContactListsL( *xmlSerializer );
            break;
            }

        case EPEngUnSubscribe:
            {
            GetXMLUnsubscribeAllL( *xmlSerializer );
            break;
            }

        default:
            {
            User::Leave( KErrCorrupt );
            break;
            }
        }

    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::ProcessResponseL( const TDesC8& aResponse,
                                                  TRequestStatus& /* aStatus */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngStartSyncTransaction::ProcessResponseL()" ) );

    // transaction to server was successful
    MPEngXMLParser* XMLparser = CreateXMLParserLC();

    switch ( iSynchronizationState )
        {
        case EPEngGetListOfLists:
            {
            ParseGetListOfListsResponseL( aResponse, XMLparser );
            break;
            }

        case EPEngUnSubscribe:
            {
            ParseUnsubscreAllResponseL( aResponse, XMLparser );
            break;
            }

        default:
            {
            User::Leave( KErrCorrupt );
            }
        }

    CleanupStack::PopAndDestroy(); // XMLparser
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /*aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngStartSyncTransaction::TransactionCompleted()
    {
    return ( iSynchronizationState == EPEngTransactionCompleted );
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngStartSyncTransaction::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::ReleaseHandler()
    {
    iSynchronizationState = EPEngTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::GetXMLContactListUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::GetXMLGetListOfContactListsL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // get lists of the contact lists of the server
    aXmlSerializer.StartTagL( KGetListOfList ).EndTagL( KGetListOfList );
    }

// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::GetXMLContactListUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::GetXMLUnsubscribeAllL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <UnsubscribePresence-Request>
    aXmlSerializer.StartTagL( KUnsubscribePresence );

    // add all contact lists which are on the server
    TInt count( iServerCntLists.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        // <ContactList>
        // append name of the contact list
        // </ContactList>
        aXmlSerializer.StartTagL( KContactList
                                ).WvAddressL( iServerCntLists[ x ]
                                            ).EndTagL( KContactList );
        }

    //  </UnsubscribePresence-Request>
    aXmlSerializer.EndTagL( KUnsubscribePresence );
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ParseGetListOfListsResponseL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::ParseGetListOfListsResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser* aXMLparser )
    {
    iServerCntLists.Reset();

    // initialize to EFalse, we don't necessarily need the result
    TBool result( EFalse );
    aXMLparser->ParseResultL( aResponse,
                              iOperationId,
                              *iTransactionStatus,
                              result );

    //if it was found anyway
    if ( result )
        {
        iSynchronizationState = EPEngTransactionCompleted;
        return;
        }

    // Is there default contact list
    if ( aXMLparser->DecodeL( aResponse, KDefaultContactList, EFalse ) )
        {
        iDefaultCntList = aXMLparser->ResultAsUnicodeTextL();
        iDefaultCntList->Des().Fold();
        iServerCntLists.AppendL( *iDefaultCntList );
        }
    else
        {
        iDefaultCntList = KNullDesC().AllocL();
        }


    const TDesC& domain = iContactListManager.UserDomain();
    if ( aXMLparser->DecodeL( aResponse, KContactList, EFalse ) )
        {
        TInt countResults( aXMLparser->Count() );
        for ( TInt i( 0 ) ; i < countResults; ++i )
            {
            // convert to 16 unicode
            HBufC16* listName = aXMLparser->ResultAsUnicodeTextL( i );
            CleanupStack::PushL( listName );
            listName->Des().Fold();
            NContactIdsTools::AddContactIdToArrayL( iServerCntLists,
                                                    *listName,
                                                    domain );
            CleanupStack::PopAndDestroy();
            }
        }

    if ( iUnsubscribe )
        {
        iSynchronizationState = EPEngUnSubscribe;
        }
    else
        {
        iSynchronizationState = EPEngTransactionCompleted;
        iContactListManager.CompleteEnviromentSyncL( iServerCntLists,
                                                     KNullDesC,
                                                     EFalse );
        }
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::ParseUnsubscreAllResponseL()
// -----------------------------------------------------------------------------
//
void CPEngStartSyncTransaction::ParseUnsubscreAllResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser* aXMLparser )
    {
    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();
    CleanupClosePushL( *transactionStatus );
    TInt err ( aXMLparser->ParseResultL( aResponse,
                                         iOperationId,
                                         *transactionStatus ) );

    switch ( err )
        {
        case KErrNone:
            {
            break;
            }


        case KPEngNwErrPartiallySuccessful:
            {
            // partially success
            // remove not existing contact lists, though, should not happen
            TInt count ( transactionStatus->DetailedResultCount() );
            for ( TInt x ( 0 ) ; x < count ; x++ )
                {
                const MPEngDetailedResultEntry2& detailedResult =
                    transactionStatus->DetailedResult( x );

                // ignore if there are some errors, except wrong contact list
                if ( detailedResult.Error() == KPEngNwErrContactListDoesNotExist )
                    {
                    // not existing contact list, remove it from list
                    // of server lists
                    TPtrC wrongContactLst;
                    detailedResult.GetDetailedDesc( wrongContactLst,
                                                    EPEngDTContactListID );
                    TInt pos( KErrNotFound );
                    if ( KErrNone == iServerCntLists.Find( wrongContactLst, pos ) )
                        {
                        iServerCntLists.Delete( pos );
                        }
                    }
                }

            break;
            }


        case KPEngNwErrContactListDoesNotExist:
            {
            iServerCntLists.Reset();
            delete iDefaultCntList;
            iDefaultCntList = NULL;
            iDefaultCntList = KNullDesC().AllocL();
            break;
            }

        default:
            {
            break;
            }
        }

    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop( ); // transactionStatus

    iContactListManager.CompleteEnviromentSyncL( iServerCntLists,
                                                 *iDefaultCntList,
                                                 ETrue );

    iSynchronizationState = EPEngTransactionCompleted;
    }

//  End of File
