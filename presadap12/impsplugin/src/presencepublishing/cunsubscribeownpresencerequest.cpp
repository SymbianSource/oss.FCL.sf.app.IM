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
#include "CUnSubscribeOwnPresenceRequest.h"
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
// CUnSubscribeOwnPresenceRequest::CUnSubscribeOwnPresenceRequest
// ---------------------------------------------------------------------------
//
CUnSubscribeOwnPresenceRequest::CUnSubscribeOwnPresenceRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::CUnSubscribeOwnPresenceRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::CUnSubscribeOwnPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUnSubscribeOwnPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::ConstructL Start" ) );
    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CUnSubscribeOwnPresenceRequest* CUnSubscribeOwnPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::NewL Start" ) );

    CUnSubscribeOwnPresenceRequest* self = new( ELeave )
    CUnSubscribeOwnPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CUnSubscribeOwnPresenceRequest* CUnSubscribeOwnPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::NewLC Start" ) );

    CUnSubscribeOwnPresenceRequest* self =
        CUnSubscribeOwnPresenceRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::~CUnSubscribeOwnPresenceRequest
// ---------------------------------------------------------------------------
//
CUnSubscribeOwnPresenceRequest::~CUnSubscribeOwnPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::~CUnSubscribeOwnPresenceRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::~CUnSubscribeOwnPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnSubscribeOwnPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUnSubscribeOwnPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( " UnSubscribeOwnPresenceRequest wvErrorCode::%d" ), wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUnSubscribeOwnPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnSubscribeOwnPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnSubscribeOwnPresenceRequest::UnSubscribeOwnPresenceL
// ---------------------------------------------------------------------------
//
void CUnSubscribeOwnPresenceRequest::UnSubscribeOwnPresenceL()
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "UnSubscribeOwnPresenceL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KUnsubscribePresence );

    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KUserIDXMLTag
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( iConnMan.UserName()
                                                  ).EndTagL( KUserIDXMLTag );

    serializer->EndTagL( KUnsubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UnSubscribeOwnPresenceL End" ) );
    }

// End of file
