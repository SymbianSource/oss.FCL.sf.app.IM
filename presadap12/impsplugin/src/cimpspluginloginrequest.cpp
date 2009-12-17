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
* Description: IMPS plugin login request
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CImpsPluginLoginRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::CImpsPluginLoginRequest
// ---------------------------------------------------------------------------
//
CImpsPluginLoginRequest::CImpsPluginLoginRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::CImpsPluginLoginRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::CImpsPluginLoginRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CImpsPluginLoginRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::CImpsPluginLoginRequest Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::NewL
// ---------------------------------------------------------------------------
//
CImpsPluginLoginRequest* CImpsPluginLoginRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::NewL Start" ) );

    CImpsPluginLoginRequest* self =
        new( ELeave ) CImpsPluginLoginRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::NewLC
// ---------------------------------------------------------------------------
//
CImpsPluginLoginRequest* CImpsPluginLoginRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::NewLC Start" ) );

    CImpsPluginLoginRequest* self =
        CImpsPluginLoginRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::~CImpsPluginLoginRequest
// ---------------------------------------------------------------------------
//
CImpsPluginLoginRequest::~CImpsPluginLoginRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CImpsPluginLoginRequest Start-End" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CImpsPluginLoginRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::DoCancel Start" ) );

    iConnMan.AccessHandler().Cancel( KErrCancel );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::RunL()
// ---------------------------------------------------------------------------
//
void CImpsPluginLoginRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    // complete prfw request

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::Login successfully" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CImpsPluginLoginRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    IMPS_DP( D_IMPS_LIT( "-------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "Data Back to Presence framwork" ) );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::RunError - id:  %d" ), aError );

    IMPS_DP( D_IMPS_LIT( "-------------------------------------------" ) );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CImpsPluginLoginRequest::IssueLoginRequest
// ---------------------------------------------------------------------------
//
void CImpsPluginLoginRequest::IssueLoginRequestL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::IssueLoginRequestL Start" ) );

    iConnMan.AccessHandler().LogInL( iStatus,
                                     iConnMan.Password(),
                                     iConnMan.UserName(),
                                     iConnMan.ServerAddress(),
                                     iConnMan.IapId()
                                   );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CImpsPluginLoginRequest::IssueLoginRequestL End" ) );
    }

// End of file
