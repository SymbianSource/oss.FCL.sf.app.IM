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
#include "CSubscribePresenceBlockListRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include "impsplugintags.h"
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
#include <ximpdatasubscriptionstate.h>

#include "PImpsAdapXMLTools.h"
#include <E32Base.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::CSubscribePresenceBlockListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceBlockListRequest::CSubscribePresenceBlockListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceBlockListRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceBlockListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceBlockListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresenceBlockListRequest* CSubscribePresenceBlockListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::NewL Start" ) );

    CSubscribePresenceBlockListRequest* self = new( ELeave )
    CSubscribePresenceBlockListRequest( aConnMan, aRequestId );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresenceBlockListRequest* CSubscribePresenceBlockListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::NewLC Start" ) );

    CSubscribePresenceBlockListRequest* self =
        CSubscribePresenceBlockListRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::~CSubscribePresenceBlockListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceBlockListRequest::~CSubscribePresenceBlockListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceBlockListRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceBlockListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceBlockListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceBlockListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresenceBlockListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceBlockListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceBlockListRequest::SubscribePresenceBlockListL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceBlockListRequest::SubscribePresenceBlockListL()
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresenceBlockListL Start" ) );


    // signal the scheduler
    SetActive();

    TRequestStatus *status = &iStatus;
    User::RequestComplete( status, KErrNone );
    IMPS_DP( D_IMPS_LIT( "::SubscribePresenceBlockListL End" ) );
    }

// End of file
