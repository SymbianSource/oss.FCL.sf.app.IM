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
* Description:  Contact list add contact handler.
*
*/


// INCLUDE FILES
#include "CPEngContactListMngTransAddId.h"

#include "CPEngContactListModChangeMonitor.h"
#include "CPEngContactListSettings.h"
#include "CPEngTransactionStatus.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "PEngWVPresenceErrors2.h"

#include "PresenceDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::CPEngContactListMngTransAddId()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransAddId::CPEngContactListMngTransAddId(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : CPEngContactListMngTransBase( aContactList,
                                        aSessionSlotId,
                                        aCSPVersion,
                                        aOperationId )

    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransAddId::CPEngContactListMngTransAddId() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    iUpdateState = EPEngListContentUpdate;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::ConstructL()
    {
    CPEngContactListMngTransBase::ConstructL( );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransAddId* CPEngContactListMngTransAddId::NewLC(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListMngTransAddId* self =
        new ( ELeave ) CPEngContactListMngTransAddId( aContactList,
                                                      aSessionSlotId,
                                                      aCSPVersion,
                                                      aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }


// Destructor
CPEngContactListMngTransAddId::~CPEngContactListMngTransAddId()
    {
    }


// =============================================================================
// =============== Functions from CPEngContactListMngTransBase =================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoAppendListNickUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoAppendListNickUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <AddNickList>
    aXmlSerializer.StartTagL( KAddNickList );

    // fill add nick array
    RPointerArray<CPEngContactListModItemContainer> addNickList;
    DoFillNickListLC( addNickList );

    TInt count( addNickList.Count() );
    for ( TInt x( 0 ); x < count ; ++x )
        {
        // <NickName>
        // <Name>
        aXmlSerializer.StartTagL( KNickName
                                ).StartTagL( KName );
        // Nick name value
        // </Name>
        aXmlSerializer.UnicodeTextL( addNickList[ x ]->NickName()
                                   ).EndTagL( KName );

        // <UserID>
        // contact Id value
        // </UserID>
        // </NickName>
        aXmlSerializer.StartTagL( KUserIDXMLTag
                                ).WvAddressL( addNickList[ x ]->Id()
                                            ).EndTagL( KUserIDXMLTag
                                                     ).EndTagL( KNickName );
        }
    CleanupStack::PopAndDestroy(); // addNickList
    // </AddNickList>
    aXmlSerializer.EndTagL( KAddNickList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoGetXMLSubscriptionUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoGetXMLSubscriptionUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // update subscription for new contacts,since server does not
    // act dynamically and subscription will be updated at next log In

    // <SubscribePresence-Request>
    aXmlSerializer.StartTagL( KSubscribePresence );

    const MDesCArray& addedNicks = iContactList.ChangeMonitor().AddedContactIds();

    // add contacts who needs to be subscribed
    TInt listCount( addedNicks.MdcaCount() );
    for ( TInt x( 0 ) ; x < listCount ; x++ )
        {
        // <User>
        // <UserID>
        aXmlSerializer.StartTagL( KUser
                                ).StartTagL( KUserIDXMLTag );

        // subscribed contact
        aXmlSerializer.WvAddressL( addedNicks.MdcaPoint( x ) );

        // </UserID>
        // </User>
        aXmlSerializer.EndTagL( KUserIDXMLTag
                              ).EndTagL( KUser );
        }

    // append set of presence attributes to be subscribed
    NListLibTools::AppendAttributesForSubscriptionXmlL(
        iContactList.Settings().SubscriptionAttributes(),
        aXmlSerializer,
        iSessionSlotId,
        iCSPVersion );

    // </SubscribePresence-Request>
    aXmlSerializer.EndTagL( KSubscribePresence );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoParseSubscriptionUpdateResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoParseSubscriptionUpdateResponseL(
    const TDesC8& aResponse )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransAddId::ParseSubscriptionUpdateResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    // new added contact are in the monitor
    CPEngContactListModChangeMonitor& changeMonitor = iContactList.ChangeMonitor();

    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();
    CleanupClosePushL( *transactionStatus );

    MPEngXMLParser* xMLparser = CreateXMLParserLC();
    TInt err ( xMLparser->ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.StorageId(),
                                                   *transactionStatus ) );

    CleanupStack::PopAndDestroy(); // xMLparser

    switch ( err )
        {
        case KErrNone:
            {
            // transaction was successful
            DoUpdateTransactionState();
            break;
            }


        case KPEngNwErrPartiallySuccessful:
            {
            // partial success
            // check just wrong contact WV IDs
            TInt count ( transactionStatus->DetailedResultCount() );
            for ( TInt i ( 0 ) ; i < count ; i++ )
                {
                const MPEngDetailedResultEntry2& detailedResult =
                    transactionStatus->DetailedResult( i );

                // what ever error, rollback that WV ID
                if ( !iRollBackIDsArray ) // create rollback array if needed
                    {
                    iRollBackIDsArray = new ( ELeave ) CDesCArrayFlat( count );
                    }

                // if contact Id was not subscribed, then remove it
                iUpdateState = EPEngRollBack; // rollback will be needed
                TPtrC badContactId;
                if ( KErrNone == detailedResult.GetDetailedDesc( badContactId, EPEngDTPresenceID ) )
                    {
                    // mark bad contact Id for rollback
                    iRollBackIDsArray->AppendL( badContactId );
                    changeMonitor.RemoveAddedContactId( badContactId );
                    iContactList.RemoveContactL( badContactId );
                    iUpdateState = EPEngRollBack;
                    }
                }
            break;
            }


        case KPEngNwErrUnknownUser:
        default: // any other error means something wrong, need of rollback
            {
            // all contacts are bad, mark contacts for removing
            TInt idCount ( changeMonitor.CountAddedContactIds() );
            if ( !idCount )
                {
                // nothing to roll back, completed
                iUpdateState = EPEngTransactionCompleted;
                break;
                }


            // create rollback array if needed
            if ( !iRollBackIDsArray )
                {
                iRollBackIDsArray = new ( ELeave ) CDesCArrayFlat( idCount );
                }


            for ( TInt ii ( 0 ) ; ii < idCount ; ii++ )
                {
                // store wrong contact ids to the container
                TPtrC badContactId ( changeMonitor.AddedContactIds().MdcaPoint( ii ) );
                iTransactionStatus->AddDetailedResultL( iOperationId,
                                                        KPEngNwErrUnknownUser,
                                                        NULL,
                                                        &badContactId,
                                                        &( iContactList.StorageId() ),
                                                        NULL );

                // mark bad contact Id for rollback
                iRollBackIDsArray->AppendL( badContactId );
                changeMonitor.RemoveAddedContactId( badContactId );
                iContactList.RemoveContactL( badContactId );
                }
            iUpdateState = EPEngRollBack;
            }
        }


    // import transaction status, takes ownership
    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop(); // transactionStatus

    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransAddId::ParseSubscriptionUpdateResponseL() done [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoUpdateTransactionState()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoUpdateTransactionState()
    {
    switch ( iUpdateState )
        {
        case EPEngListContentUpdate:
            {
            // in CSP version 1.2 auto subscribe is enabled
            if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                                   KPEngCntLstPropertyNativePermanent )
                 && iNewIdsCount
                 && ( iCSPVersion == EWVCspV11 ) )
                {
                iUpdateState = EPEngSubscriptionUpdate;
                }
            else
                {
                iUpdateState = EPEngActivateAttributes;
                }
            break;
            }


        // Subscription and rollback both are treated same
        case EPEngSubscriptionUpdate:
        case EPEngRollBack:
            {
            iUpdateState = EPEngTransactionCompleted;
            break;
            }

        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoFillNickListLC()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoFillNickListLC(
    RPointerArray<CPEngContactListModItemContainer>& aNickList )
    {
    CleanupClosePushL( aNickList );
    iContactList.FillAddNickListL( aNickList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransAddId::DoCompleteContactListUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransAddId::DoCompleteContactListUpdateL()
    {
    iContactList.CommitAddContactsL();
    }

//  End of File
