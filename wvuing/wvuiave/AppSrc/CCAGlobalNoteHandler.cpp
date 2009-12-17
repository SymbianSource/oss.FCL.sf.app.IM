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
* Description:  Shows global confirmation query and
*                follows user answering
*
*/



// INCLUDE FILES
#include "CCAGlobalNoteHandler.h"
#include "impsbuilddefinitions.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include <AknGlobalConfirmationQuery.h>
#include <chatNG.rsg>
#include <StringLoader.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAGlobalNoteHandler::CCAGlobalNoteHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAGlobalNoteHandler::CCAGlobalNoteHandler() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCAGlobalNoteHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAGlobalNoteHandler::ConstructL()
    {
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    }

// -----------------------------------------------------------------------------
// CCAGlobalNoteHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAGlobalNoteHandler* CCAGlobalNoteHandler::NewL()
    {
    CCAGlobalNoteHandler* self = new( ELeave ) CCAGlobalNoteHandler;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCAGlobalNoteHandler::~CCAGlobalNoteHandler()
    {
    Cancel();
    delete iGlobalConfirmationQuery;
    }

// ---------------------------------------------------------
// CCAGlobalNoteHandler::DoCancel
//
// ---------------------------------------------------------
//
void CCAGlobalNoteHandler::DoCancel()
    {
    iGlobalConfirmationQuery->CancelConfirmationQuery();
    }

// ---------------------------------------------------------
// CCAGlobalNoteHandler::RunL
//
// ---------------------------------------------------------
//
void CCAGlobalNoteHandler::RunL()
    {
    iAnswer = iStatus.Int();
    Cancel();
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CCAGlobalNoteHandler::ShowNoteL
//
// ---------------------------------------------------------
//
TInt CCAGlobalNoteHandler::ShowNoteL( TInt aResourceId )
    {
    iAnswer = 0;
    if ( !IsActive() )
        {
        HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC( aResourceId );
        iGlobalConfirmationQuery->ShowConfirmationQueryL(
            iStatus, *prompt, R_AVKON_SOFTKEYS_YES_NO__YES );
        CleanupStack::PopAndDestroy( prompt ); // prompt
        SetActive();
        if ( ! iWait.IsStarted() )
            {
            iWait.Start();  // CSI: 10 # iWait is not an active object
            }
        }

    return iAnswer;
    }

// ---------------------------------------------------------
// CCAGlobalNoteHandler::CancelNoteShowing
//
// ---------------------------------------------------------
//
void CCAGlobalNoteHandler::CancelNoteShowing()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    iAnswer = KErrCancel;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CCAGlobalNoteHandler::RunError
//
// ---------------------------------------------------------
//
TInt CCAGlobalNoteHandler::RunError( TInt aError )
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iAnswer = aError;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    return KErrNone;
    }

//  End of File

