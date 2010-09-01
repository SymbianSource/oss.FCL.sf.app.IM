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
* Description:  Create attribute list transaction handler
*
*/

// INCLUDE FILES
#include "CPEngAttributeListTransDeleteAttrList.h"
#include "CPEngAttributeListItem.h"
#include "MPEngAttributeListTransactionManager.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include <e32std.h>

//Default granurality for ID lists
const TInt KAttributeListIdGranurality = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransDeleteAttrList::CPEngAttributeListTransDeleteAttrList()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransDeleteAttrList::CPEngAttributeListTransDeleteAttrList(
    MPEngAttributeListTransactionManager& aAttrListTransManager,
    CPEngContactListTransactionManager& aCntListTransManager,
    MDesCArray& aDeleteContactList,
    MDesCArray& aDeleteContactIds,
    TBool aDeleteDefault,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : CPEngAttributeListTransBase(  aAttrListTransManager,
                                        aCntListTransManager,
                                        aCSPVersion,
                                        aOperationId ),
        iArrayDeleteContactList( aDeleteContactList ),
        iArrayDeleteContactIds( aDeleteContactIds ),
        iDeleteDefaultAttrList( aDeleteDefault )
    {
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListTransDeleteAttrList::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransDeleteAttrList::ConstructL()
    {
    CPEngAttributeListTransBase::ConstructL();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransDeleteAttrList::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransDeleteAttrList* CPEngAttributeListTransDeleteAttrList::NewLC(
    MPEngAttributeListTransactionManager& aAttrListTransManager,
    CPEngContactListTransactionManager& aCntListTransManager,
    MDesCArray& aDeleteContactList,
    MDesCArray& aDeleteContactIds,
    TBool aDeleteDefault,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngAttributeListTransDeleteAttrList* self =
        new ( ELeave ) CPEngAttributeListTransDeleteAttrList(
        aAttrListTransManager,
        aCntListTransManager,
        aDeleteContactList,
        aDeleteContactIds,
        aDeleteDefault,
        aCSPVersion,
        aOperationId );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CPEngAttributeListTransDeleteAttrList::~CPEngAttributeListTransDeleteAttrList()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransDeleteAttrList::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransDeleteAttrList::RequestL(
    TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    // <DeleteAttributeList-Request>
    xmlSerializer->StartTagL( KDeleteAttributeList );

    XMLAppendContactIdsL( *xmlSerializer, iArrayDeleteContactIds );
    XMLAppendContactListsL( *xmlSerializer, iArrayDeleteContactList );
    XMLAppendDefaultFlagL( *xmlSerializer, iDeleteDefaultAttrList );

    // </DeleteAttributeList-Request>
    xmlSerializer->EndTagL( KDeleteAttributeList );

    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransDeleteAttrList::DoConsumeMessageFromServerL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransDeleteAttrList::DoConsumeMessageFromServerL(
    const TDesC8& aResponse )
    {
    MPEngXMLParser* XMLparser = CreateXMLParserLC();
    TInt err ( XMLparser->ParseResultL( aResponse,
                                        iOperationId,
                                        *iTransactionStatus ) );


    CDesC16ArraySeg* contactIDs =
        new ( ELeave ) CDesC16ArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( contactIDs );
    CopyDesArrayL( iArrayDeleteContactIds, * contactIDs );

    CDesC16ArraySeg* contactLists =
        new ( ELeave ) CDesC16ArraySeg( KAttributeListIdGranurality );
    CleanupStack::PushL( contactLists );
    CopyDesArrayL( iArrayDeleteContactList, *contactLists );


    switch ( err )
        {
        case KErrNone:
        case KPEngNwErrUnknownUser:
        case KPEngNwErrContactListDoesNotExist:
        case KPEngNwErrPartiallySuccessful:
            {
            // successful operation, all went fine, process according to it
            // when it failed cause of unknown contact list/wv ID, commit as
            // well since we are deleting anyway
            iAttributeListTransManager.CommitDefinedContactsL( contactLists, contactIDs );

            // if we were deleting also delfaul attribute lists, commit it as well
            if ( iDeleteDefaultAttrList )
                {
                iAttributeListTransManager.CommitDefaultL();
                }
            break;
            }

        default:
            {
            // none of the IDs/cnts list was deleted, remove them from the delete lists
            iAttributeListTransManager.RollBackDefinedContactsL( contactLists, contactIDs );
            if ( iDeleteDefaultAttrList )
                {
                iAttributeListTransManager.RollBackDefaultL();
                }
            break;
            }
        }


    iAttributeListTransManager.StoreChangesToStoreL();
    CleanupStack::PopAndDestroy( 3 ); // XMLparser, contactLists, contactIDs
    }


//  End of File

