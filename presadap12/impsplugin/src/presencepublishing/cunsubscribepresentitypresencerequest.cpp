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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CUnsubscribePresentityPresenceRequest.h"
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
// CUnsubscribePresentityPresenceRequest::CUnsubscribePresentityPresenceRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresenceRequest::CUnsubscribePresentityPresenceRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresentityPresenceRequest Start" ) );

    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresentityPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresenceRequest* CUnsubscribePresentityPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::NewL Start" ) );

    CUnsubscribePresentityPresenceRequest* self = new( ELeave )
    CUnsubscribePresentityPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresenceRequest* CUnsubscribePresentityPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::NewLC Start" ) );

    CUnsubscribePresentityPresenceRequest* self = new( ELeave )
    CUnsubscribePresentityPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::~CUnsubscribePresentityPresenceRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresenceRequest::~CUnsubscribePresentityPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresentityPresenceRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresentityPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( " UnsubscribePresentityPresenceL wvErrorCode::%d" ), wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );
    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUnsubscribePresentityPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresenceRequest::UnsubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresenceRequest::UnsubscribePresentityPresenceL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresentityPresenceL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "UnsubscribePresentityPresenceL Manufacture XML " ) );

    //serializer->StartTagL(KTransactionContent);
    //serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KUnsubscribePresence );

    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KUserIDXMLTag
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( aIdentity.Identity()
                                                  ).EndTagL( KUserIDXMLTag );

    serializer->EndTagL( KUnsubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresentityPresenceL End" ) );
    }

// End of file
