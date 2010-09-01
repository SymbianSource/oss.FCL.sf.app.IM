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
#include "CUnsubscribePresenceGrantRequestListRequest.h"
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
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::CUnsubscribePresenceGrantRequestListRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceGrantRequestListRequest::CUnsubscribePresenceGrantRequestListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresenceGrantRequestListRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresenceGrantRequestListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceGrantRequestListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::NewL
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceGrantRequestListRequest* CUnsubscribePresenceGrantRequestListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::NewL Start" ) );

    CUnsubscribePresenceGrantRequestListRequest* self = new( ELeave )
    CUnsubscribePresenceGrantRequestListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::NewLC
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceGrantRequestListRequest* CUnsubscribePresenceGrantRequestListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::NewLC Start" ) );

    CUnsubscribePresenceGrantRequestListRequest* self = new( ELeave )
    CUnsubscribePresenceGrantRequestListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::~CUnsubscribePresenceGrantRequestListRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceGrantRequestListRequest::~CUnsubscribePresenceGrantRequestListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresenceGrantRequestListRequest Start-End" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceGrantRequestListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceGrantRequestListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;
    const TInt KWVRequestResponseResultCode = 200;

    // Check if we got 200
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

    // if we get 200 then fetch the details else complete the request


    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUnsubscribePresenceGrantRequestListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceGrantRequestListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresenceGrantRequestListRequest::SubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceGrantRequestListRequest::UnsubscribePresenceGrantRequestListL()
    {
    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresenceGrantRequestListL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );



    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresenceGrantRequestListL End" ) );
    }

// End of file
