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
* Description:  Presence attribute publisher.
*
*/

// INCLUDE FILES

#include "CPEngAttributePublishHandler.h"
#include "CPEngTransactionStatus.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"
#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAttrTransactionEnv.h"
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "PEngWVPresenceAttributes2.h"

#include "PresenceDebugPrint.h"

#include <E32Std.h>




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributePublishHandler* CPEngAttributePublishHandler::NewLC(
    MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    const TDesC16& aData,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion )
    {
    CPEngAttributePublishHandler* self = new ( ELeave ) CPEngAttributePublishHandler(
        aAttrManager,
        aTransEnv,
        aTransactionOperation,
        aCspVersion );

    CleanupStack::PushL( self );
    self->ConstructL( aData );

    return self;
    }


// Destructor
CPEngAttributePublishHandler::~CPEngAttributePublishHandler()
    {
    delete iTransactionStatus;
    iAttributeModels.ResetAndDestroy(); //destroy all models
    delete iXmlParser;
    }




// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::CPEngAttributePublishHandler
// C++ default constructor can NOT contain any code, that
// might leave.
//
// There is approximately 5 attribute models to publish at time
// ==> granurality 5.
// -----------------------------------------------------------------------------
//
CPEngAttributePublishHandler::CPEngAttributePublishHandler(
    MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion )
        : iAttributeManager( aAttrManager ),
        iTransEnv( aTransEnv ),
        iTransactionOperation( aTransactionOperation ),
        iCspVersion( aCspVersion )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::ConstructL( const TDesC16& aData )
    {
    iTransactionStatus = CPEngTransactionStatus::NewL();
    iAttributeManager.UnpackModelArrayL( aData,
                                         EPEngUserAttribute | EPEngStorableModel,
                                         iAttributeModels );

    iXmlParser = CreateXMLParserL();
    }



// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::RequestL()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::RequestL( TDes8& aSendBuffer )
    {
    PENG_DP_TXT( "CPEngAttributePublishHandler::RequestL()" );

    __ASSERT_ALWAYS( iAttributeModels.Count() > 0, User::Leave( KErrArgument ) );

    TPtrC8 transactionContentNS( KTransactionContentNS );
    TPtrC8 presenceSubListNS( KPresenceSubListAttributesNS );
    if ( iCspVersion == EWVCspV12 )
        {
        transactionContentNS.Set( KTransactionContentNS_CSP12 );
        presenceSubListNS.Set( KPresenceSubListAttributesNS_CSP12 );
        }


    aSendBuffer.Zero();
    MPEngXMLSerializer* serialize = CreateXmlSerializerL( aSendBuffer );
    CleanupClosePushL( *serialize );

    serialize->StartTagL( KTransactionContent ).AttributeL( KXmlXmlns,
                                                            transactionContentNS );
    serialize->StartTagL( KUpdatePresenceRequest );
    serialize->StartTagL( KPresenceSubList ).AttributeL( KXmlXmlns,
                                                         presenceSubListNS );

    const TInt count = iAttributeModels.Count();
    for ( TInt ii( 0 ); ii < count; ii++ )
        {
        MPEngPresenceAdvancedAttrModel2* model = iAttributeModels[ ii ]->Advanced();
        model->EncodeToXmlL( *serialize );
        model->HandleAttributeUpdatedToNetwork();
        if ( model->Type() == KUidPrAttrOnlineStatus )
            {
            iOnlineStPublished = ETrue;
            }
        }

    serialize->EndTagL( KPresenceSubList );
    serialize->EndTagL( KUpdatePresenceRequest );
    serialize->EndTagL( KTransactionContent );
    CleanupStack::PopAndDestroy(); //release serialize


    PENG_DP_TXT( "CPEngAttributePublishHandler::RequestL() - Done" );
    }



// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::LastRunningTransactionHandler()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::LastRunningTransactionHandler()
    {
    PENG_DP_TXT( "CPEngAttributePublishHandler::LastRunningTransactionHandler()" );

    //nothing to do
    }



// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::ProcessResponseL()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::ProcessResponseL( const TDesC8& aResponse,
                                                     TRequestStatus& /*aStatus*/ )

    {
    PENG_DP_TXT( "CPEngAttributePublishHandler::ProcessResponseL()" );


    iXmlParser->ParseResultL( aResponse,
                              iTransactionOperation,
                              *iTransactionStatus );

    if ( iTransactionStatus->Status() == KErrNone )
        {
        // if there was not online status published, load model for notification
        if ( !iOnlineStPublished )
            {
            MPEngPresenceAttrModel2* model = NULL;
            iAttributeManager.LoadAttributeLC( model,
                                               KNullDesC,
                                               KUidPrAttrOnlineStatus,
                                               EPEngUserAttribute | EPEngStorableModel );

            iAttributeModels.AppendL( model );
            CleanupStack::Pop(); // model
            }
        iAttributeManager.ForceStoreBatchL( iAttributeModels );

        //NOTE: UpdateSessionTracingAttributesL() removes some attributes from
        //iAttributeModels array and takes their ownership
        iTransEnv.UpdateSessionTracingAttributesL( iAttributeModels );

        iAttributeModels.ResetAndDestroy();
        }


    PENG_DP_TXT( "CPEngAttributePublishHandler::ProcessResponseL() - Done" );
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::CancelProcessing()
    {
    //nothing to do
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::NewTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::NewTransactionHandlersL(
    RPointerArray<MPEngOutgoingTransactionHandler>& /*aHandlers*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::TransactionCompleted()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributePublishHandler::TransactionCompleted()
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::TransactionResult()
// -----------------------------------------------------------------------------
//
CPEngTransactionStatus* CPEngAttributePublishHandler::TransactionResult()
    {
    CPEngTransactionStatus* tmp = iTransactionStatus;
    iTransactionStatus = NULL;
    return tmp;
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishHandler::ReleaseHandler()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishHandler::ReleaseHandler()
    {
    //nothing to do
    }


//  End of File
