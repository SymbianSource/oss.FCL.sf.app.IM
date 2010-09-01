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
#include "CPEngAttributeListTransCreateAttrList.h"

#include "CPEngAttributeListItem.h"
#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"
#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngAttributeListTransactionManager.h"
#include "MPEngTransactionFactory.h"

#include <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransCreateAttrList::CPEngAttributeListTransCreateAttrList()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransCreateAttrList::CPEngAttributeListTransCreateAttrList(
    MPEngAttributeListTransactionManager& aAttrListTransManager,
    CPEngContactListTransactionManager& aCntListTransManager,
    MPEngPresenceAttrManager& aPresenceAttributeManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId,
    TBool aSync )
        : CPEngAttributeListTransBase(  aAttrListTransManager,
                                        aCntListTransManager,
                                        aCSPVersion,
                                        aOperationId ),
        iPresenceAttributeManager( aPresenceAttributeManager ),
        iSynchronization ( aSync )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransCreateAttrList::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransCreateAttrList::ConstructL(
    const CPEngAttributeListItem& aAttributeListItem )
    {
    CPEngAttributeListTransBase::ConstructL();
    iAttributeListItem = CPEngAttributeListItem::NewL( aAttributeListItem );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransCreateAttrList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransCreateAttrList* CPEngAttributeListTransCreateAttrList::NewLC(
    const CPEngAttributeListItem& aAttributeListItem,
    MPEngAttributeListTransactionManager& aAttrListTransManager,
    CPEngContactListTransactionManager& aCntListTransManager,
    MPEngPresenceAttrManager& aPresenceAttributeManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId,
    TBool aSync )
    {
    CPEngAttributeListTransCreateAttrList* self =
        new ( ELeave ) CPEngAttributeListTransCreateAttrList(
        aAttrListTransManager,
        aCntListTransManager,
        aPresenceAttributeManager,
        aCSPVersion,
        aOperationId,
        aSync );

    CleanupStack::PushL( self );
    self->ConstructL( aAttributeListItem );

    return self;
    }


// Destructor
CPEngAttributeListTransCreateAttrList::~CPEngAttributeListTransCreateAttrList()
    {
    delete iAttributeListItem;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransCreateAttrList::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransCreateAttrList::RequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    // <CreateAttributeList-Request>
    xmlSerializer->StartTagL( KCreateAttributeList );

    // <PresenceSubList
    xmlSerializer->StartTagL( KPresenceSubList );

    // check if attribute lists is empty, then we append empty standart atrribute's name space
    if ( 0 == iAttributeListItem->PresenceAttributes().Count() )
        {
        // attribute name space
        if ( iCSPVersion == EWVCspV11 )
            {
            // xmlns="http://www.wirelessvillage.org/PA1.1"
            xmlSerializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS );
            }
        else
            {
            // xmlns="http://www.openmobilealliance.org/DTD/WV-PA1.2"
            xmlSerializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );
            }
        }
    else
        {
        // xmlns="Name space"
        // append attribute's namespace according to attributes on the list
        // Append Attribute name space
        TPtrC8 attributeName;
        TPtrC8 attributeNameSpace;
        User::LeaveIfError( iPresenceAttributeManager.GetAttributeXmlNameAndNameSpace(
                                iAttributeListItem->PresenceAttributes()[0],
                                attributeName, attributeNameSpace ) );

        xmlSerializer->AttributeL( KXmlXmlns, attributeNameSpace );

        // Append attributes one by one
        TInt count ( iAttributeListItem->PresenceAttributes().Count() );
        for ( TInt x( 0 ) ; x < count ; x++ )
            {
            User::LeaveIfError( iPresenceAttributeManager.GetAttributeXmlNameAndNameSpace(
                                    iAttributeListItem->PresenceAttributes()[x],
                                    attributeName, attributeNameSpace ) );
            // append attribute name
            xmlSerializer->StartTagL( attributeName ).EndTagL( attributeName );
            }
        }

    //</PresenceSubList>
    xmlSerializer->EndTagL( KPresenceSubList );

    // now it depends if this is synchronization handler or not
    if ( iSynchronization )
        {
        // this us sync handler,
        // all settings are taken from current and new at the same time
        XMLAppendContactIdsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                  CPEngAttributeListItem::ECurrentContactIDs ) );

        XMLAppendContactIdsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                  CPEngAttributeListItem::ENewContactIDs ) );

        XMLAppendContactListsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                    CPEngAttributeListItem::ECurrentContactLists ) );

        XMLAppendContactListsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                    CPEngAttributeListItem::ENewContactLists ) );

        XMLAppendDefaultFlagL( *xmlSerializer, iAttributeListItem->CurrentlyDefault() );
        }
    else
        {
        // this is update handler so settings are taken from new
        XMLAppendContactIdsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                  CPEngAttributeListItem::ENewContactIDs ) );

        XMLAppendContactListsL( *xmlSerializer, iAttributeListItem->ArrayOfContacts(
                                    CPEngAttributeListItem::ENewContactLists ) );

        XMLAppendDefaultFlagL( *xmlSerializer, iAttributeListItem->NewDefault() );
        }

    // </CreateAttributeList-Request>
    // </TransactionContent>
    xmlSerializer->EndTagL( KCreateAttributeList
                          ).EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransCreateAttrList::ConsumeMessageFromServerL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransCreateAttrList::DoConsumeMessageFromServerL(
    const TDesC8& aResponse )
    {
    MPEngXMLParser* XMLparser = CreateXMLParserLC();
    // try if there were some errors
    TInt err ( XMLparser->ParseResultL( aResponse, iOperationId, *iTransactionStatus ) );
    RArray<TUint32> wrongAttributes;
    CleanupClosePushL( wrongAttributes );
    switch ( err )
        {
        case KErrNone:
            {
            // successful operation, all went fine, process according to it
            // if this was synchronization, just mark those attr list as in sync
            if ( iSynchronization )
                {
                iAttributeListTransManager.AttributeListCreated( iAttributeListItem->PresenceAttributes() );
                }
            else
                {
                // since we have copy of the Attribute list item, just commit it
                iAttributeListTransManager.CommitDefinedContactsL(
                    &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ENewContactLists ),
                    &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ENewContactIDs ) );

                if ( iAttributeListItem->NewDefault() )
                    {
                    iAttributeListTransManager.CommitDefaultL();
                    }
                }
            break;
            }

        case KPEngNwErrPartiallySuccessful:
            {
            // partial success, figure what went fine and what went OK
            // we are interested in failed WV IDs, contact list and attributes
            // anyhow we managed to create attr list
            if ( iSynchronization )
                {
                iAttributeListTransManager.AttributeListCreated( iAttributeListItem->PresenceAttributes() );
                }

            TInt count ( iTransactionStatus->DetailedResultCount() );
            for ( TInt x ( 0 ) ; x < count ; x++ )
                {
                const MPEngDetailedResultEntry2& detailedResult = iTransactionStatus->DetailedResult( x );

                switch ( detailedResult.Error() )
                    {
                    case KPEngNwErrUnknownUser:
                        {
                        // rollback wrong WV ID
                        TPtrC wrongWVWID;
                        detailedResult.GetDetailedDesc( wrongWVWID, EPEngDTPresenceID );
                        iAttributeListTransManager.RollBackContactIdL( wrongWVWID );
                        break;
                        }

                    case KPEngNwErrContactListDoesNotExist:
                        {
                        // rollback wrong cnt list
                        TPtrC wrongCntList;
                        detailedResult.GetDetailedDesc( wrongCntList, EPEngDTContactListID );
                        iAttributeListTransManager.RollBackContactListL( wrongCntList );
                        break;
                        }

                    case KPEngNwErrInvalidOrUnSupportedPresenceAttribute:
                        {
                        TUint32 wrongAttrib;
                        detailedResult.GetDetailedInt( wrongAttrib, EPEngDIAttributeID );
                        wrongAttributes.AppendL( wrongAttrib );
                        break;
                        }

                    default:
                        {
                        }
                    }
                }
            break;
            }

        case KPEngNwErrUnknownUser:
        case KPEngNwErrContactListDoesNotExist:
            {
            // none of the IDs/cnts list was added,
            //remove them from the delta list of the attribute list
            iAttributeListTransManager.RollBackDefinedContactsL(
                &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ENewContactLists ) ,
                &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ENewContactIDs ) );

            // it this was sync, roll back also current
            if ( iSynchronization )
                {
                iAttributeListTransManager.RollBackDefinedContactsL(
                    &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ECurrentContactLists ) ,
                    &iAttributeListItem->ArrayOfContacts( CPEngAttributeListItem::ECurrentContactIDs ) );

                }

            if ( iAttributeListItem->NewDefault() )
                {
                iAttributeListTransManager.RollBackDefaultL();
                }
            }

        // handle all folllowing errors in the same, default, way
        case KPEngNwErrInvalidOrUnSupportedPresenceAttribute:
        default:
            {
            if ( iAttributeListItem->NewDefault() )
                {
                iAttributeListTransManager.RollBackDefaultL();
                }
            break;
            }
        }

    // store changes and continue
    iAttributeListTransManager.StoreChangesToStoreL();
    CleanupStack::PopAndDestroy( 2 ); // wrongAttributes, XMLparser

    if ( iSynchronization )
        {
        iAttributeListTransManager.AttributeEngineSynchronizedL();
        }
    }

//  End of File

