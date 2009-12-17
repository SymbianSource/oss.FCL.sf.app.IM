/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute fetch handler.
*
*/

// INCLUDE FILES
#include "CPEngAttributeFetchHandler.h"
#include "MPEngAttrFetchProcessor.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "CPEngTransactionStatus.h"

#include "PresenceDebugPrint.h"
#include <PEngPresenceEngineConsts2.h>
#include <E32Std.h>
#include <badesca.h>





//LOCAL constants
namespace
    {
    /**
     * Granularity for array of fetched presence ids.
     * Usually there is just few fetched presence id's
     * at time.
     */
    const TInt KPEngFetchPresIdGranurality = 5;

    //Panic
    _LIT( KPEngAttrFetchHandlerPanic, "PEngAtFeHand" );

    //Panic reasons
    enum TPEngAttrFetchHandlerPanicReasons
        {
        EFetchHandlerInUse
        };

    void PEngAttrFetchHandlerPanic( TPEngAttrFetchHandlerPanicReasons aPanicReason )
        {
        User::Panic( KPEngAttrFetchHandlerPanic, aPanicReason );
        }
    }



// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeFetchHandler* CPEngAttributeFetchHandler::NewLC( MPEngPresenceAttrManager& aAttrManager,
                                                               const TDesC& aDomain,
                                                               const TDesC16& aData,
                                                               TInt aTransactionOperation,
                                                               TPEngWVCspVersion aCspVersion )
    {
    CPEngAttributeFetchHandler* self = new ( ELeave ) CPEngAttributeFetchHandler(
        aAttrManager,
        aTransactionOperation );

    CleanupStack::PushL( self );
    self->ConstructL( aDomain, aCspVersion, aData );
    return self;
    }


// Destructor
CPEngAttributeFetchHandler::~CPEngAttributeFetchHandler()
    {
    Cancel();
    delete iFetchedProcessor;
    iResultModels.ResetAndDestroy();
    delete iResultStatus;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::CPEngAttributeFetchHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeFetchHandler::CPEngAttributeFetchHandler( MPEngPresenceAttrManager& aAttrManager,
                                                        TInt aTransactionOperation )
        : CActive( CActive::EPriorityIdle ),    //Do parsing in low priority
        iAttributeManager( aAttrManager ),
        iTransactionOperation( aTransactionOperation )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::ConstructL( const TDesC& aDomain,
                                             TPEngWVCspVersion aCspVersion,
                                             const TDesC16& aData )
    {
    iResultStatus = CPEngTransactionStatus::NewL();

    RArray< TUint32 > attributes;
    CleanupClosePushL( attributes );

    CDesC16ArraySeg* presenceIds =  new ( ELeave ) CDesC16ArraySeg(
        KPEngFetchPresIdGranurality );
    CleanupStack::PushL( presenceIds );

    iAttributeManager.UnpackFetchRequestL( aData,
                                           *presenceIds,
                                           attributes );

    iFetchedProcessor = CreateAttrFetchProcessorL( iAttributeManager,
                                                   *presenceIds,
                                                   attributes.Array(),
                                                   aDomain,
                                                   iTransactionOperation,
                                                   aCspVersion );

    CleanupStack::PopAndDestroy( presenceIds );
    CleanupStack::PopAndDestroy(); //attributes
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::RequestL()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::RequestL( TDes8& aSendBuffer )
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::RequestL()" );

    aSendBuffer.Zero();
    iFetchedProcessor->GenerateRequestL( aSendBuffer );

    PENG_DP_TXT( "CPEngAttributeFetchHandler::RequestL() - Done" );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::LastRunningTransactionHandler()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::LastRunningTransactionHandler()
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::LastRunningTransactionHandler()" );
    //nothing to do
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::ProcessResponseL()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::ProcessResponseL( const TDesC8& aResponse,
                                                   TRequestStatus& /*aStatus*/ )

    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::ProcessResponseL()" );

    __ASSERT_ALWAYS( !iRequest,
                     PEngAttrFetchHandlerPanic( EFetchHandlerInUse ) );


    // Initialize the response parsing
    // Will leave if response is corrupted
    iFetchedProcessor->InitResponseProcessingL( aResponse,
                                                iResultModels,
                                                *iResultStatus );

    //Temporary solution to do response handling synchronously
    while ( iFetchedProcessor->ProcessStepL() )
        {
        }

    if ( iTransactionOperation == EPEngTransOpAttributeFetchToObjects )
        {
        PENG_DP_TXT( "CPEngAttributeFetchHandler::ProcessResponseL() - parsing done. Streaming result models to objects." );

        HBufC16* dataResult = iAttributeManager.PackModelArrayL( iResultModels );
        CleanupStack::PushL( dataResult );
        iResultStatus->AddDataResultL( iTransactionOperation, dataResult );
        CleanupStack::Pop( dataResult );
        }
    else
        {
        PENG_DP_TXT( "CPEngAttributeFetchHandler::ProcessResponseL() - parsing done. Storing result models to cache." );

        iAttributeManager.ForceStoreBatchL( iResultModels );
        }


    //And start asynchronous processing
    /*
    iRequest = &aStatus;
    *iRequest = KRequestPending;
    IssueNewRound();
    */

    PENG_DP_TXT( "CPEngAttributeFetchHandler::ProcessResponseL() - Done" );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::CancelProcessing()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::CancelProcessing()
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::CancelProcessing()" );
    Cancel();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::NewTransactionHandlersL()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /*aHandlers*/ )
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::NewTransactionHandlersL()" );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::TransactionCompleted()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeFetchHandler::TransactionCompleted()
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::TransactionCompleted()" );
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::TransactionResult()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngAttributeFetchHandler::TransactionResult()
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::TransactionResult()" );

    CPEngTransactionStatus* tmp = iResultStatus;
    iResultStatus = NULL;
    return tmp;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::ReleaseHandler()
// From MPEngOutgoingTransactionHandler
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::ReleaseHandler()
    {
    //nothing to do
    }



// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::RunL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::RunL()
    {
    PENG_DP_TXT( "CPEngAttributeFetchHandler::RunL()" );
    if ( iFetchedProcessor->ProcessStepL() )
        {
        PENG_DP_TXT( "CPEngAttributeFetchHandler::RunL() - issuing new round." );
        IssueNewRound();
        return;
        }

    if ( iTransactionOperation == EPEngTransOpAttributeFetchToObjects )
        {
        PENG_DP_TXT( "CPEngAttributeFetchHandler::RunL() - parsing done. Streaming result models to objects." );

        HBufC16* dataResult = iAttributeManager.PackModelArrayL( iResultModels );
        CleanupStack::PushL( dataResult );
        iResultStatus->AddDataResultL( iTransactionOperation, dataResult );
        CleanupStack::Pop( dataResult );
        }
    else
        {
        PENG_DP_TXT( "CPEngAttributeFetchHandler::RunL() - parsing done. Storing result models to cache." );

        iAttributeManager.ForceStoreBatchL( iResultModels );
        }


    iResultModels.ResetAndDestroy();
    User::RequestComplete( iRequest, KErrNone );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::RunError()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeFetchHandler::RunError( TInt aError )
    {
    PENG_DP( D_PENG_LIT( "CPEngAttributeFetchHandler::RunError(): %d" ), aError );

    iResultModels.ResetAndDestroy();
    User::RequestComplete( iRequest, aError );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::DoCancel()
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::DoCancel()
    {
    iResultModels.ResetAndDestroy();
    User::RequestComplete( iRequest, KErrCancel );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchHandler::IssueNewRound()
// -----------------------------------------------------------------------------
//
void CPEngAttributeFetchHandler::IssueNewRound()
    {
    iStatus = KRequestPending;

    TRequestStatus *pS = &iStatus;
    User::RequestComplete( pS, KErrNone );
    SetActive();
    }


//  End of File


