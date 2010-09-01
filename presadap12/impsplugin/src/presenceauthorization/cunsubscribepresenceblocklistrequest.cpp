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
#include "CUnsubscribePresenceBlockListRequest.h"
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
// CUnsubscribePresenceBlockListRequest::CUnsubscribePresenceBlockListRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceBlockListRequest::CUnsubscribePresenceBlockListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresenceBlockListRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresenceBlockListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceBlockListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::NewL
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceBlockListRequest* CUnsubscribePresenceBlockListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::NewL Start" ) );

    CUnsubscribePresenceBlockListRequest* self = new( ELeave )
    CUnsubscribePresenceBlockListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::NewLC
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceBlockListRequest* CUnsubscribePresenceBlockListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::NewLC Start" ) );

    CUnsubscribePresenceBlockListRequest* self = new( ELeave )
    CUnsubscribePresenceBlockListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::~CUnsubscribePresenceBlockListRequest
// ---------------------------------------------------------------------------
//
CUnsubscribePresenceBlockListRequest::~CUnsubscribePresenceBlockListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresenceBlockListRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresenceBlockListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceBlockListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceBlockListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;


    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupContentRequest Return Code %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUnsubscribePresenceBlockListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresenceBlockListRequest::UnsubscribePresenceBlockListL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresenceBlockListRequest::UnsubscribePresenceBlockListL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::UnsubscribePresenceBlockListL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresenceBlockListRequest::UnsubscribePresenceBlockListL End" ) );
    }

// End of file
