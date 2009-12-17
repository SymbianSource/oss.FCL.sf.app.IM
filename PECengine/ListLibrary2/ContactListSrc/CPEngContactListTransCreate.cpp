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
* Description:  Handler to create contact list
*
*/


// INCLUDE FILES
#include "CPEngContactListTransCreate.h"

#include "CPEngContactListModItemContainer.h"
#include "CPEngContactListSettings.h"
#include "MPEngContactListTransactionManager.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::CPEngContactListTransCreate()
// -----------------------------------------------------------------------------
//
CPEngContactListTransCreate::CPEngContactListTransCreate(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : CPEngContactListTransBase( aContactList,
                                     aSessionSlotId,
                                     aManager,
                                     aCSPVersion,
                                     aOperationId )
    {
    PENG_DP( D_PENG_LIT( "PEngContactListTransCreate::CPEngContactListTransCreate() [%S]" ),
             &( iContactList.ListProperties().Name() ) );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransCreate::ConstructL( )
    {
    CPEngContactListTransBase::ConstructL( );
    iContactListSyncState = EPEngCntListCreate;
    }


// -----------------------------------------------------------------------------
// CPEngStartSyncTransaction::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListTransCreate* CPEngContactListTransCreate::NewLC(
    CPEngContactListModBase& aContactList,
    CPEngSessionSlotId& aSessionSlotId,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListTransCreate* self =
        new ( ELeave ) CPEngContactListTransCreate( aContactList,
                                                    aSessionSlotId,
                                                    aManager,
                                                    aCSPVersion,
                                                    aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CPEngContactListTransCreate::~CPEngContactListTransCreate()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::DoGetXMLCreateCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransCreate::DoGetXMLCreateCntListL(
    MPEngXMLSerializer& aXmlSerializer )
    {
    // <CreateList-Request>
    aXmlSerializer.StartTagL( KCreateList );

    NListLibTools::AppendContactListNameXmlL( aXmlSerializer,
                                              iContactList.Settings() );

    TInt count( iContactList.Count() );
    if ( count != 0 )
        {
        // <NickList>
        aXmlSerializer.StartTagL( KNickList );

        for ( TInt x( 0 ); x < count ; x++ )
            {
            MPEngContactItem& contactItem = iContactList.ContactItem( x );

            // <NickName>
            // <Name>
            aXmlSerializer.StartTagL( KNickName
                                    ).StartTagL( KName );

            // Nick name
            // </Name>
            aXmlSerializer.UnicodeTextL( contactItem.NickName()
                                       ).EndTagL( KName );

            // <UserID>
            // contact Id
            // </UserID>
            // </NickName>
            aXmlSerializer.StartTagL( KUserIDXMLTag
                                    ).WvAddressL( contactItem.Id()
                                                ).EndTagL( KUserIDXMLTag
                                                         ).EndTagL( KNickName );
            }
        // </NickList>
        aXmlSerializer.EndTagL( KNickList );
        }

    // add properties of the contact list
    NListLibTools::AppendContactListPropertiesXmlL( aXmlSerializer,
                                                    iContactList.Settings() );

    // </CreateList-Request>
    aXmlSerializer.EndTagL( KCreateList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::DoParseCreateCntListResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransCreate::DoParseCreateCntListResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aXMLparser )
    {
    PENG_DP( D_PENG_LIT( "PEngContactListTransCreate::DoParseCreateCntListResponseL() [%S]" ),
             &( iContactList.ListProperties().Name() ) );

    CPEngTransactionStatus* transactionStatus = CPEngTransactionStatus::NewL();

    CleanupClosePushL( *transactionStatus );
    TInt err ( aXMLparser.ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   iContactList.StorageId(),
                                                   *transactionStatus ) );

    switch ( err )
        {
            // parse KErrNone and partial success in same way
        case KErrNone:
        case KPEngNwErrPartiallySuccessful:
            {
            MPEngXMLParser* secondXMLparser = CreateXMLParserLC();

            RReffArray<CPEngContactListModItemContainer> newContacts;
            CleanupClosePushL( newContacts );

            if ( NListLibTools::ParseContactListNickListL( aResponse,
                                                           newContacts,
                                                           iContactList,
                                                           aXMLparser,
                                                           *secondXMLparser,
                                                           ETrue ) )
                {
                iContactList.AdoptNewContactsL( newContacts );
                }
            else
                {
                // nick list was not there, consume result manually
                // commit changes and parse wrong IDs if any
                iContactList.CommitRemoveContactsL();
                iContactList.CommitAddContactsL();
                NListLibTools::RemoveBadContactsL( *transactionStatus, iContactList );
                }

            CleanupStack::PopAndDestroy( ); // newContacts
            NListLibTools::ParseContactListPropertiesL( aResponse,
                                                        iContactList.Settings(),
                                                        aXMLparser,
                                                        *secondXMLparser );
            CleanupStack::PopAndDestroy(); // secondXMLparser


            // update behavior
            iContactList.Settings().SetPropertyL( KPEngListExistsOnServer,
                                                  KPEngCntLstPropertyNativeCached,
                                                  ETrue );

            // if this was to be new default contact list, do update
            NListLibTools::HarmonizeDefaultContactListSettingsL( iContactList.Settings(),
                                                                 iManager );


            // resolve next step -  do we need to subscribe this lists
            if ( iContactList.Settings().Property( KPEngCntLstSubscribe,
                                                   KPEngCntLstPropertyNativePermanent ) )
                {
                iContactListSyncState = EPEngSubscribeCntList;
                }
            else
                {
                CompleteCntListTransactionL();
                }
            break;
            }


        case KPEngNwErrUnknownUser:
            {
            // remove all contact ids and repeat creation of the contact list
            TInt count ( iContactList.Count() - 1 );
            for (  ; count >= 0 ; --count )
                {
                // add bad contact it to the transaction status
                TPtrC badContactId( iContactList.ContactItem( count ).Id() );

                iTransactionStatus->AddDetailedResultL( iOperationId,
                                                        KPEngNwErrUnknownUser,
                                                        NULL,
                                                        &badContactId,
                                                        &( iContactList.StorageId() ),
                                                        NULL );
                iContactList.RemoveBadContactL( badContactId );
                }
            break;
            }


        default:
            {
            iManager.CompleteCntListDeletionL( iContactList.Settings().Name() );
            iContactListSyncState = EPEngTransactionCompleted;
            break;
            }
        }

    // update transaction status
    iTransactionStatus->ImportStatusFrom( *transactionStatus );
    CleanupStack::Pop(); // transactionStatus
    }



// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::DoGetXMLFetchCntListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransCreate::DoGetXMLFetchCntListL(
    MPEngXMLSerializer& /*aXmlSerializer*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListTransCreate::DoParseFetchCntListResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListTransCreate::DoParseFetchCntListResponseL(
    const TDesC8& /*aResponse*/,
    MPEngXMLParser& /*aXMLparser*/ )
    {
    }


//  End of File





















