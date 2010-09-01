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
#include "CWithdrawPresenceGrantFromEveryoneRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include <protocolpresenceauthorizationdatahost.h>

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
// CWithdrawPresenceGrantFromEveryoneRequest::CWithdrawPresenceGrantFromEveryoneRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromEveryoneRequest::CWithdrawPresenceGrantFromEveryoneRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CWithdrawPresenceGrantFromEveryoneRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CWithdrawPresenceGrantFromEveryoneRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromEveryoneRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::NewL
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromEveryoneRequest* CWithdrawPresenceGrantFromEveryoneRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::NewL Start" ) );

    CWithdrawPresenceGrantFromEveryoneRequest* self = new( ELeave )
    CWithdrawPresenceGrantFromEveryoneRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::NewLC
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromEveryoneRequest* CWithdrawPresenceGrantFromEveryoneRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::NewLC Start" ) );

    CWithdrawPresenceGrantFromEveryoneRequest* self =
        CWithdrawPresenceGrantFromEveryoneRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::~CWithdrawPresenceGrantFromEveryoneRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromEveryoneRequest::~CWithdrawPresenceGrantFromEveryoneRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CWithdrawPresenceGrantFromEveryoneRequest Start-End" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromEveryoneRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::RunL()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromEveryoneRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

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

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest  %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CWithdrawPresenceGrantFromEveryoneRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromEveryoneRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromEveryoneRequest::WithdrawPresenceGrantFromEveryoneL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromEveryoneRequest::WithdrawPresenceGrantFromEveryoneL()
    {
    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromEveryoneL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );
    // empty list so that all attributes
    serializer->EndTagL( KPresenceSubList );

    // set some duplicate attribute to true.
    serializer->StartTagL( KTimeZone ).EndTagL( KTimeZone );
    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "WithdrawPresenceGrantFromEveryoneL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromEveryoneL End" ) );
    }

// End of file
