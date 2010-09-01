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
* Description:  Plus delta list outgoing update handler
*
*/

// INCLUDE FILES

#include "CPEngContactListMngTransProperties.h"
#include "MPEngContactListSettingsManager.h"
#include "CPEngContactListSettings.h"
#include "CPEngContactListModBase.h"
#include "MPEngContactListTransactionManager.h"

#include "PEngListLibTools.h"
#include "MPEngXMLParser.h"
#include "MPEngXMLSerializer.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::CPEngContactListMngTransProperties()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransProperties::CPEngContactListMngTransProperties(
    CPEngContactListModBase& aContactList,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iContactList( aContactList ),
        iManager( aManager ),
        iTransactionCompleted( EFalse ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )

    {
    PENG_DP( D_PENG_LIT( "CPEngContactListMngTransProperties::CPEngContactListMngTransProperties" ) );
    iContactList.Open();                        // CSI: 65 #
    iContactList.Lock( EStorageLockLevelHigh );
    }

// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListMngTransProperties* CPEngContactListMngTransProperties::NewLC(
    CPEngContactListModBase& aContactList,
    MPEngContactListTransactionManager& aManager,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngContactListMngTransProperties* self =
        new ( ELeave ) CPEngContactListMngTransProperties(
        aContactList,
        aManager,
        aCSPVersion,
        aOperationId );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// Destructor
CPEngContactListMngTransProperties::~CPEngContactListMngTransProperties()
    {
    iContactList.Unlock();
    iContactList.Close();
    delete iTransactionStatus;
    }


// ============================================================================
// =============Function from MPEngOutgoingTransactionHandler =================
// ============================================================================

// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::RequestL(
    TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    // <ListManage-Request>
    xmlSerializer->StartTagL( KListMnageRequest );

    NListLibTools::AppendContactListNameXmlL( *xmlSerializer, iContactList.Settings() );

    NListLibTools::AppendContactListPropertiesXmlL( *xmlSerializer, iContactList.Settings() );

    // <ReceiveList>T</ReceiveList>
    NListLibTools::ApppendReceiveListTagL( *xmlSerializer, iCSPVersion, ETrue );

    // </ListManage-Request>
    xmlSerializer->EndTagL( KListMnageRequest );

    // end of transaction
    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::LastRunningTransactionHandler()
    {
    }

// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    // parse contact list property update

    MPEngXMLParser* XMLparser = CreateXMLParserLC();
    CPEngContactListSettings& settings = iContactList.Settings();
    TInt err ( XMLparser->ParseContactListResultL( aResponse,
                                                   iOperationId,
                                                   settings.Name(),
                                                   *iTransactionStatus ) );
    switch ( err )
        {
            // parse success, same as partial success
        case KErrNone:
        case KPEngNwErrPartiallySuccessful:
            {
            MPEngXMLParser* XMLparser2 = CreateXMLParserLC();
            NListLibTools::ParseContactListPropertiesL( aResponse,
                                                        settings,
                                                        *XMLparser,
                                                        *XMLparser2 );

            CleanupStack::PopAndDestroy(); // XMLparser2
            settings.DeletePropertyL( KPEngPropertiesUpdateNeeded,
                                      KPEngCntLstPropertyNativeCached );

            // update default list settings if needed
            NListLibTools::HarmonizeDefaultContactListSettingsL( settings, iManager );
            break;
            }


        case KPEngNwErrContactListDoesNotExist:
            {
            // Nothing was updated
            // Update property
            settings.SetPropertyL( KPEngListExistsOnServer,
                                   KPEngCntLstPropertyNativeCached,
                                   EFalse );
            break;
            }


        default:
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // XMLparser
    iTransactionCompleted = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>&  /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListMngTransProperties::TransactionCompleted()
    {
    return iTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngContactListMngTransProperties::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListMngTransProperties::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngContactListMngTransProperties::ReleaseHandler()
    {
    }


//  End of File



















