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
* Description:  Update handler to remove contacts from contact list
*
*/

// INCLUDE FILES
#include "CPEngContactListMngTransRemoveId.h"
#include "CPEngContactListModItemContainer.h"
#include "CPEngContactListModChangeMonitor.h"
#include "CPEngContactListSettings.h"
#include "MPEngXMLParser.h"
#include "MPEngXMLSerializer.h"
#include "PEngXMLTags.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"

#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrTransactionEnv.h"

#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::CPEngContactListMngTransRemoveId()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransRemoveId::CPEngContactListMngTransRemoveId(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlot,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : CPEngContactListMngTransBase( aContactList,
                                        aSessionSlot,
                                        aCSPVersion,
                                        aOperationId )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransRemoveId::CPEngContactListMngTransRemoveId() [%S]" ),
             &( iContactList.ListProperties().Name() ) );


    // if server supports CSP 1.2 => no need to unsubscribe, it is done automaticaly
    if ( iContactList.Settings().Property( KPEngCntLstSubscribe ,
                                           KPEngCntLstPropertyNativePermanent ) &&
         ( iCSPVersion == EWVCspV11 ) )
        {
        iUpdateState = EPEngSubscriptionUpdate;
        }

    else
        {
        iUpdateState = EPEngListContentUpdate;
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::ConstructL()
    {
    CPEngContactListMngTransBase::ConstructL( );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransRemoveId* CPEngContactListMngTransRemoveId::NewLC(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlot,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    // first push aContact to cleanUpStack
    CPEngContactListMngTransRemoveId* self = new ( ELeave )
    CPEngContactListMngTransRemoveId(
        aContactList,
        aSessionSlot,
        aCSPVersion,
        aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngContactListMngTransRemoveId::~CPEngContactListMngTransRemoveId()
    {
    }



// =============================================================================
// =============== Functions from CPEngContactListMngTransBase =================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoAppendListNickUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoAppendListNickUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <RemoveNickList>
    aXmlSerializer.StartTagL( KRemoveNickList );

    RPointerArray<CPEngContactListModItemContainer> removeNickList;
    DoFillNickListLC( removeNickList  );

    // insert contact Id to remove
    TInt count( removeNickList.Count() );

    for ( TInt x( 0 ); x < count ; x++ )
        {
        // <UserID>
        // contact Id value
        // </UserID>
        aXmlSerializer.StartTagL( KUserIDXMLTag
                                ).WvAddressL( removeNickList[ x ]->Id()
                                            ).EndTagL( KUserIDXMLTag );
        }
    CleanupStack::PopAndDestroy(); // removeNickList
    // </RemoveNickList>
    aXmlSerializer.EndTagL( KRemoveNickList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoGetXMLSubscriptionUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoGetXMLSubscriptionUpdateL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // remove subscription from removed contacts,since server does not
    // act dynamically and subscription will be removed  at next log In

    // <UnsubscribePresence-Request>
    aXmlSerializer.StartTagL( KUnsubscribePresence );

    RPointerArray<CPEngContactListModItemContainer> removeNickList;
    DoFillNickListLC( removeNickList );

    // add those fellas we do not need anymore te be subscribed
    TInt count( removeNickList.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        // <User>
        // <UserID>
        aXmlSerializer.StartTagL( KUser
                                ).StartTagL( KUserIDXMLTag );

        // now fella we wanna unsubscribe
        aXmlSerializer.WvAddressL( removeNickList[ x ]->Id() );

        // </UserID>
        // </User>
        aXmlSerializer.EndTagL( KUserIDXMLTag
                              ).EndTagL( KUser );
        }

    CleanupStack::PopAndDestroy(); // removeNickList
    // </UnsubscribePresence-Request>
    aXmlSerializer.EndTagL( KUnsubscribePresence );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoParseSubscriptionUpdateResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoParseSubscriptionUpdateResponseL(
    const TDesC8& aResponse )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransRemoveId::DoParseSubscriptionUpdateResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    MPEngXMLParser* XMLparser = CreateXMLParserLC();
    // try if there were some errors
    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();
    CleanupClosePushL( *transactionStatus );
    TInt err ( XMLparser->ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.StorageId(),
                                                   *transactionStatus ) );

    // clean all attribute models for all contacts we are trying to delete
    RPointerArray<CPEngContactListModItemContainer> removeNickList;
    DoFillNickListLC( removeNickList );

    MPEngPresenceAttrTransactionEnv* attributeManager =
        PEngAttrLibFactory::AttributeTransEnvInstanceLC( iSessionSlotId );

    TInt count ( removeNickList.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        attributeManager->RemoveSubscriptionL( removeNickList[ x ]->Id() );
        }
    CleanupStack::PopAndDestroy( 2 ); // attributeManager, removeNickList


    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop(); // transactionStatus
    CleanupStack::PopAndDestroy( ); // XMLparser


    // was it one the errors that we can continue with request?
    if ( ( err == KErrNone )
         || ( err == KPEngNwErrPartiallySuccessful )
         || ( err == KPEngNwErrUnknownUser )
         || ( err == KPEngNwErrServiceUnavailable ) )
        {
        // Continue with next step
        DoUpdateTransactionState();
        }
    else
        {
        // Error was something bigger, terminate transaction
        iUpdateState = EPEngTransactionCompleted;
        }


    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransRemoveId::DoParseSubscriptionUpdateResponseL() done [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoUpdateTransactionState()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoUpdateTransactionState()
    {
    switch ( iUpdateState )
        {
        case EPEngListContentUpdate:
            {
            if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                                   KPEngCntLstPropertyNativePermanent )
                 && ( iCSPVersion == EWVCspV12 ) )
                {
                iUpdateState = EPEngDeActivateAttributes;
                }
            else
                {
                iUpdateState = EPEngTransactionCompleted;
                }
            break;
            }


        case EPEngSubscriptionUpdate:
            {
            iUpdateState = EPEngListContentUpdate;
            break;
            }


        case EPEngRollBack:
            {
            iUpdateState = EPEngTransactionCompleted;
            }


        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoFillNickListLC()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoFillNickListLC(
    RPointerArray<CPEngContactListModItemContainer>& aNickList )
    {
    CleanupClosePushL( aNickList );
    iContactList.FillRemoveNickListL( aNickList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransRemoveId::DoCompleteContactListUpdateL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransRemoveId::DoCompleteContactListUpdateL()
    {
    iContactList.CommitRemoveContactsL();
    }


//  End of File

