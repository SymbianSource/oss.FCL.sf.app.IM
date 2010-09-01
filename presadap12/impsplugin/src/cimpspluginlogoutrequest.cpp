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
* Description: IMPS plugin logout request
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CImpsPluginLogoutRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "CImpsPluginAccessHandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::CImpsPluginLogoutRequest
// ---------------------------------------------------------------------------
//
CImpsPluginLogoutRequest::CImpsPluginLogoutRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::CImpsPluginLogoutRequest Start" ) );

    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::CImpsPluginLogoutRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CImpsPluginLogoutRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::ConstructL Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::NewL
// ---------------------------------------------------------------------------
//
CImpsPluginLogoutRequest* CImpsPluginLogoutRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId  )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::NewL Start" ) );

    CImpsPluginLogoutRequest* self =
        new( ELeave ) CImpsPluginLogoutRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::NewL End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::NewL
// ---------------------------------------------------------------------------
//
CImpsPluginLogoutRequest* CImpsPluginLogoutRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId  )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::NewLC Start" ) );

    CImpsPluginLogoutRequest* self =
        CImpsPluginLogoutRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::NewLC End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::~CImpsPluginLogoutRequest
// ---------------------------------------------------------------------------
//
CImpsPluginLogoutRequest::~CImpsPluginLogoutRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::~CImpsPluginLogoutRequest Start-End" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CImpsPluginLogoutRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::DoCancel Start-End" ) );

    iConnMan.AccessHandler().Cancel( KErrCancel );

    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::RunL()
// ---------------------------------------------------------------------------
//
void CImpsPluginLogoutRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, iStatus.Int() );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CImpsPluginLogoutRequest::RunError( TInt aError )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLogoutRequest::RunError Start-End" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );
    delete this;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CImpsPluginLogoutRequest::IssueLoginRequest
// ---------------------------------------------------------------------------
//
void CImpsPluginLogoutRequest::IssueLogoutRequestL()
    {
    // request to the IMPS Server,
    iConnMan.AccessHandler().LogOutL( iStatus );

    // signal the scheduler
    SetActive();
    }

// End of file
