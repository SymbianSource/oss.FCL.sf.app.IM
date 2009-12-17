/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Create Presentity Group Request to the Network Server.
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CImpsPluginGroupUnSubscribePresentityGroupContentRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"


#include "CPEngXMLParser.h"
#include "MPEngXMLParser.h"
#include "CPEngXmlSerializer.h"
#include "MPEngXmlSerializer.h"
#include "CImpsPluginConnectionManager.h"
#include "PEngXMLTags.h"

#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::CImpsPluginGroupUnSubscribePresentityGroupContentRequest
// ---------------------------------------------------------------------------
//
CImpsPluginGroupUnSubscribePresentityGroupContentRequest::CImpsPluginGroupUnSubscribePresentityGroupContentRequest( MImpsPrtPluginConnectionManager& aConnMan,
        TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::CImpsPluginGroupUnSubscribePresentityGroupContentRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::CImpsPluginGroupUnSubscribePresentityGroupContentRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupUnSubscribePresentityGroupContentRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewL
// ---------------------------------------------------------------------------
//
CImpsPluginGroupUnSubscribePresentityGroupContentRequest* CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewL Start" ) );

    CImpsPluginGroupUnSubscribePresentityGroupContentRequest* self = new( ELeave ) CImpsPluginGroupUnSubscribePresentityGroupContentRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewLC
// ---------------------------------------------------------------------------
//
CImpsPluginGroupUnSubscribePresentityGroupContentRequest* CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewLC Start" ) );

    CImpsPluginGroupUnSubscribePresentityGroupContentRequest* self = new( ELeave ) CImpsPluginGroupUnSubscribePresentityGroupContentRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::~CImpsPluginGroupUnSubscribePresentityGroupContentRequest
// ---------------------------------------------------------------------------
//
CImpsPluginGroupUnSubscribePresentityGroupContentRequest::~CImpsPluginGroupUnSubscribePresentityGroupContentRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::~CImpsPluginGroupUnSubscribePresentityGroupContentRequest Start-End" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupUnSubscribePresentityGroupContentRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunL()
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    //Look for mandatory parts
    if ( iParser->DecodeL( *iResponse, KResultXMLTag, ETrue ) )
        {
        resultBlock.Set( iParser->ResultL() );
        if ( iParser->DecodeL( resultBlock, KCodeXMLTag, EFalse ) )
            {
            TPtrC8 cspStatusCodeBlock( iParser->ResultL() );
            TInt cspStatusInt;
            TLex8 lexer( cspStatusCodeBlock );
            lexer.SkipSpace();
            TInt lexErr = lexer.Val( cspStatusInt );
            if ( lexErr == KErrNone )
                {
                wvErrorCode = cspStatusInt;
                }
            }
        }

    IMPS_DP( D_IMPS_LIT( "CreatePresentityGroupRequest Return Code %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunL Start" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CImpsPluginGroupUnSubscribePresentityGroupContentRequest::UnSubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupUnSubscribePresentityGroupContentRequest::UnSubscribePresentityGroupContentL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::UnSubscribePresentityGroupContentL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    serializer->StartTagL( KTransactionContent );
    serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );



    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "UnSubscribePresentityGroupContentL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupUnSubscribePresentityGroupContentRequest::UnSubscribePresentityGroupContentL End" ) );
    }

// End of file
