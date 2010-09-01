/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Template of the Server request handler implementation
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngRequestHandler.h"

//	Debug prints
#include	"PresenceDebugPrint.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngRequestHandler::CPEngRequestHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngRequestHandler::CPEngRequestHandler(
    TPEngStorageServerMessages aRequestFunction,
    TUint32 aSessionID,
    TInt aSubSessionID )
        : iSessionID( aSessionID ),
        iSubSessionID( aSubSessionID ),
        iRequestFunction( aRequestFunction )

    {
    }

// -----------------------------------------------------------------------------
// CPEngRequestHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngRequestHandler::ConstructL()
    {
    // nothing here, client is listening all events
    }


// Destructor
CPEngRequestHandler::~CPEngRequestHandler()
    {
    // might be that it left before arra was created
    iRequestMessage.Complete( KErrCancel );
    // base class will delete itself from container of the folder class
    }


// =============================================================================
// ================= new functions from the MPEngRequestHandler ================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngRequestHandler::SessionId
// -----------------------------------------------------------------------------
//
TUint32 CPEngRequestHandler::SessionId() const
    {
    return iSessionID;
    }

// -----------------------------------------------------------------------------
// CPEngRequestHandler::SubSessionId
// -----------------------------------------------------------------------------
//
TInt CPEngRequestHandler::SubSessionId() const
    {
    return iSubSessionID;
    }

// -----------------------------------------------------------------------------
// CPEngRequestHandler::RequestFunction
// -----------------------------------------------------------------------------
//
TPEngStorageServerMessages CPEngRequestHandler::RequestFunction() const
    {
    return iRequestFunction;
    }

// -----------------------------------------------------------------------------
// CPEngRequestHandler::CancelRequest
// -----------------------------------------------------------------------------
//
void CPEngRequestHandler::CancelRequestD()
    {
    PENG_DP( D_PENG_LIT( "CPEngRequestHandler::CancelRequest" ) );
    // In that case nothing much to do, just complete request with KErrCanceled
    // and remove request from the list of request handlers of the state machine
    iRequestMessage.Complete( KErrCancel );
    // now delet me, destructor will remove class from the list of State machine
    this->Close();
    }

//  End of File
