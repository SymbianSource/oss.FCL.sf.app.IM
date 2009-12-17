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
* Description:  Handler to get watcher list.
*
*/

// INCLUDE FILES
#include "CPEngGetWatchersTransaction.h"

#include "CPEngWatcherList.h"
#include "CPEngContactListModItemContainer.h"
#include "CPEngContactListModChangeMonitor.h"

#include "PEngListLibTools.h"
#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "CPEngTransactionStatus.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::CPEngGetWatchersTransaction()
// -----------------------------------------------------------------------------
//
CPEngGetWatchersTransaction::CPEngGetWatchersTransaction(
    CPEngContactListModBase& aWatchers,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
        : iIsTransactionCompleted( EFalse ),
        iWatchers( aWatchers ),
        iCSPVersion( aCSPVersion ),
        iOperationId( aOperationId )
    {
    PENG_DP( D_PENG_LIT( "CPEngGetWatchersTransaction::CPEngGetWatchersTransaction()" ) );

    iWatchers.Open();                   // CSI: 65 #
    iWatchers.BufferServerSideNotifications();
    iWatchers.Lock( EStorageLockLevelHigh );
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::ConstructL()
    {
    iTransactionStatus =  CPEngTransactionStatus::NewL();
    iIsTransactionCompleted = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::NewLC()
// -----------------------------------------------------------------------------
//
CPEngGetWatchersTransaction* CPEngGetWatchersTransaction::NewLC(
    CPEngContactListModBase& aWatchers,
    TPEngWVCspVersion& aCSPVersion,
    TInt aOperationId )
    {
    CPEngGetWatchersTransaction* self =
        new ( ELeave ) CPEngGetWatchersTransaction(
        aWatchers,
        aCSPVersion,
        aOperationId );

    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngGetWatchersTransaction::~CPEngGetWatchersTransaction()
    {
    iWatchers.ReleaseServerSideBuffering();
    iWatchers.Unlock();
    iWatchers.Close();
    delete iTransactionStatus;
    }


// ===========================================================================
// ============Function from MPEngOutgoingTransactionHandler =================
// ===========================================================================

// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::RequestL(
    TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* xmlSerializer = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *xmlSerializer );

    // <TransactionContent xmlns="http://www.wireless -village.org/TRC1.1">
    NListLibTools::AppendTransactionContentTagXmlL( *xmlSerializer, iCSPVersion );

    // <GetWatcherList-Request/>
    xmlSerializer->StartTagL( KGetWatchers
                            ).EndTagL( KGetWatchers );

    // </TransactionContent>
    xmlSerializer->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy( ); // xmlSerializer
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::LastRunningTransactionHandler()
    {
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::ProcessResponseL(
    const TDesC8& aResponse,
    TRequestStatus& /* aStatus */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngGetWatchersTransaction::ProcessResponseL()" ) );

    // transaction to server was successful
    RReffArray<CPEngContactListModItemContainer> newContacts;
    CleanupClosePushL( newContacts );
    MPEngXMLParser* XMLparser = CreateXMLParserLC();

    // parse result if it exists
    TBool resultFound( EFalse );
    TInt err( XMLparser->ParseResultL( aResponse,
                                       iOperationId,
                                       *iTransactionStatus,
                                       resultFound ) );

    if ( ( err == KErrNone ) && NListLibTools::ParseContactListIDsL( aResponse,
                                                                     newContacts,
                                                                     iWatchers,
                                                                     *XMLparser,
                                                                     ETrue ) )
        {
        iWatchers.AdoptNewContactsL( newContacts );
        }

    CleanupStack::PopAndDestroy( 2 ); // newContacts, XMLparser
    iIsTransactionCompleted =  ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::CancelProcessing( )
    {
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::GetNewOutGoingTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /* aTransactionsArray*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngGetWatchersTransaction::TransactionCompleted()
    {
    return iIsTransactionCompleted;
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngGetWatchersTransaction::TransactionResult()
    {
    CPEngTransactionStatus* temp = iTransactionStatus;
    iTransactionStatus = NULL;
    return temp;
    }


// -----------------------------------------------------------------------------
// CPEngGetWatchersTransaction::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngGetWatchersTransaction::ReleaseHandler()
    {
    iIsTransactionCompleted =  ETrue;
    }

//  End of File





















