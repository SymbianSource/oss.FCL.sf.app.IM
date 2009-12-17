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
* Description:  Base class for attribute list transactions
*
*/

// INCLUDE FILES
#include "CPEngAttributeListTransBase.h"
#include "CPEngAttributeListItem.h"
#include "MPEngAttributeListTransactionManager.h"

#include "CPEngContactListTransactionManager.h"
#include "CPEngContactListSettings.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "MPEngPresenceAttrManager.h"

#include <e32std.h>
#include <bamdesca.h>


// MACROS
// call continue if condition is fulfilled
#define CONTINUE_IF( a )\
    if ( a )\
        {\
        continue;\
        }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::CPEngAttributeListTransBase()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransBase::CPEngAttributeListTransBase(
    MPEngAttributeListTransactionManager& aAttrListTransManager,
    CPEngContactListTransactionManager& aCntListTransManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iTransactionCompleted( EFalse ),
        iAttributeListTransManager( aAttrListTransManager ),
        iCntListTransManager( aCntListTransManager ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )
    {

    }

// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// Destructor
CPEngAttributeListTransBase::~CPEngAttributeListTransBase()
    {
    delete iTransactionStatus;
    }


// ==============================================================================
// =============From MPEngOutgoingTransactionHandler ============================
// ==============================================================================

// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    DoConsumeMessageFromServerL( aResponse );
    iTransactionCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray */ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransBase::TransactionCompleted()
    {
    return iTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus*  CPEngAttributeListTransBase::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::ReleaseHandler()
    {
    iTransactionCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::XMLAppendContactListsL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::XMLAppendContactListsL(
    MPEngXMLSerializer& aXmlSerializer,
    const MDesCArray& aContactLists )
    {
    // append Contact lists one by one
    TInt count ( aContactLists.MdcaCount() );
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        CPEngContactListSettings* cntSettings =
            iCntListTransManager.ContactListSettingsOrNull(
                aContactLists.MdcaPoint( i ) );

        CONTINUE_IF( ( !cntSettings
                       ||
                       !( cntSettings->Property( KPEngListExistsOnServer,
                                                 KPEngCntLstPropertyNativeCached ) )
                     ) );

        TPtrC listServerName( cntSettings->ServerName() );
        // <ContactList>
        // contact list name
        // </ContactList>
        aXmlSerializer.StartTagL( KContactList
                                ).WvAddressL( listServerName
                                            ).EndTagL( KContactList );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::XMLAppendContactIdsL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::XMLAppendContactIdsL(
    MPEngXMLSerializer& aXmlSerializer,
    const MDesCArray& aContactIds )
    {
    // append WV IDs one by one
    TInt count ( aContactIds.MdcaCount() );
    for ( TInt x( 0 ); x < count ; x++ )
        {
        // <UserID>
        aXmlSerializer.StartTagL( KUserIDXMLTag );

        aXmlSerializer.WvAddressL( aContactIds.MdcaPoint( x ) );

        // </UserID>
        aXmlSerializer.EndTagL( KUserIDXMLTag );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::XMLAppendDefaultFlagL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::XMLAppendDefaultFlagL(
    MPEngXMLSerializer& aXmlSerializer,
    const TBool aFlag )
    {
    // <DefaultList>
    aXmlSerializer.StartTagL( KDefaultList );

    // write attribute list default flag
    aXmlSerializer.RawValueL( aFlag ? KXMLValueTrue : KXMLValueFalse  );

    // </DefaultList>
    aXmlSerializer.EndTagL( KDefaultList );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransBase::CopyDesArrayL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransBase::CopyDesArrayL(
    const MDesC16Array& aOldDesArray,
    CDesC16Array& aNewArray )
    {
    TInt count ( aOldDesArray.MdcaCount()  );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        aNewArray.AppendL( aOldDesArray.MdcaPoint( x ) );
        }
    }

//  End of File

