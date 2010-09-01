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
#include "CSubscribePresenceGrantRequestListRequest.h"
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
#include <ximpdatasubscriptionstate.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::CSubscribePresenceGrantRequestListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceGrantRequestListRequest::CSubscribePresenceGrantRequestListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceGrantRequestListRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceGrantRequestListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceGrantRequestListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "::ConstructL Start" ) );

    iParser = CreateXMLParserL();

    IMPS_DP( D_IMPS_LIT( "::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresenceGrantRequestListRequest* CSubscribePresenceGrantRequestListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::NewL Start" ) );

    CSubscribePresenceGrantRequestListRequest* self = new( ELeave )
    CSubscribePresenceGrantRequestListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresenceGrantRequestListRequest* CSubscribePresenceGrantRequestListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::NewLC Start" ) );

    CSubscribePresenceGrantRequestListRequest* self =
        CSubscribePresenceGrantRequestListRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::~CSubscribePresenceGrantRequestListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceGrantRequestListRequest::~CSubscribePresenceGrantRequestListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceGrantRequestListRequest Start" ) );

    delete iResponse;
    delete iContactName;
    delete iUserId;

    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceGrantRequestListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceGrantRequestListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceGrantRequestListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresenceGrantRequestListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceGrantRequestListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceGrantRequestListRequest::SubscribePresenceGrantRequestListL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceGrantRequestListRequest::SubscribePresenceGrantRequestListL()
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresenceGrantRequestListL Start" ) );
    // signal the scheduler
    SetActive();

    //by default adaptation 1.2 is ready for receiving presence authorisation requests from the server.no need to subscribe for it
    TRequestStatus *status = &iStatus;
    User::RequestComplete( status, KErrNone );

    IMPS_DP( D_IMPS_LIT( "::SubscribePresenceGrantRequestListL End" ) );
    }

// End of file
