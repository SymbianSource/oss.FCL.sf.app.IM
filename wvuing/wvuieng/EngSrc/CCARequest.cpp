/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds information of one network request
*
*/

// INCLUDE FILES

#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "ChatDebugPrint.h"

#include <e32base.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCARequest::CCARequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCARequest::CCARequest( const TInt aOpId,
                        TBool aDestroyAfterComplete,
                        TCallBack aCallBack )
        : iOpId( aOpId ),
        iCallBack( aCallBack ),
        iDestroyAfterComplete( aDestroyAfterComplete )
    {
    }

// -----------------------------------------------------------------------------
// CCARequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARequest* CCARequest::NewL( TInt aOpId,
                              TBool aDestroyAfterComplete,
                              TCallBack aCallBack )
    {
    CCARequest* self = new( ELeave ) CCARequest( aOpId, aDestroyAfterComplete,
                                                 aCallBack );

    return self;
    }


// Destructor
CCARequest::~CCARequest()
    {
    }

// ---------------------------------------------------------
// CCARequest::ExecuteCallBackFunction
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARequest::ExecuteCallBackFunction() const
    {
    return iCallBack.CallBack();
    }

// ---------------------------------------------------------
// CCARequest::DestroyAfterComplete
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCARequest::DestroyAfterComplete() const
    {
    return iDestroyAfterComplete;
    }

// ---------------------------------------------------------
// CCARequest::IdMatches
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCARequest::IdMatches( TInt aOpId ) const
    {
    return aOpId == iOpId;
    }

// ---------------------------------------------------------
// CCARequest::StartWait
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::StartWait()
    {
    CHAT_DP_FUNC_ENTER( "CCARequest::StartWait" );
    CHAT_DP( D_CHAT_LIT( "CCARequest::StartWait() Request type is %d" ),
             iRequestType );

    if ( iRequestMapper )
        {
        iRequestMapper->ReportWaitStart();
        }
    if ( !iWait.IsStarted() )
        {
        iWait.Start();  // CSI: 10 # iWait is not active object.
        }
    CHAT_DP_FUNC_DP( "CCARequest::StartWait", "Continue processing" );

    if ( iRequestMapper )
        {
        iRequestMapper->ReportWaitStop();
        }
    CHAT_DP_FUNC_DONE( "CCARequest::StartWait" );
    }

// ---------------------------------------------------------
// CCARequest::StopWaitIfNeeded
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::StopWaitIfNeeded()
    {
    CHAT_DP_FUNC_ENTER( "CCARequest::StopWaitIfNeeded" );
    if ( iWait.IsStarted() )
        {
        CHAT_DP( D_CHAT_LIT( "CCARequest::StopWaitIfNeeded - \
                              Stop is needed: %d" ), iWait.CanStopNow() );
        iWait.AsyncStop();
        }
    CHAT_DP_FUNC_DONE( "CCARequest::StopWaitIfNeeded" );
    }

// ---------------------------------------------------------
// CCARequest::SetErrorCode
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::SetErrorCode( TInt aErrorCode )
    {
    iErrorCode = aErrorCode;
    }

// ---------------------------------------------------------
// CCARequest::ErrorCode
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARequest::ErrorCode() const
    {
    return iErrorCode;
    }

// ---------------------------------------------------------
// CCARequest::SetRequestType
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::SetRequestType( TInt aRequestType )
    {
    iRequestType = aRequestType;
    }

// ---------------------------------------------------------
// CCARequest::RequestType
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARequest::RequestType() const
    {
    return iRequestType;
    }

// ---------------------------------------------------------
// CCARequest::SetAdditionalData
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::SetAdditionalData( const MDesCArray& aAdditionalArray )
    {
    iAdditionalArray = &aAdditionalArray;
    }

// ---------------------------------------------------------
// CCARequest::AdditionalData
// (other items were commented in a header).
// ---------------------------------------------------------
//
const MDesCArray* CCARequest::AdditionalData() const
    {
    return iAdditionalArray;
    }

// ---------------------------------------------------------
// CCARequest::SetRequestMapper
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARequest::SetRequestMapper( CCARequestMapper* aRequestMapper )
    {
    iRequestMapper = aRequestMapper;
    }

//  End of File
