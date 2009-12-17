/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles the always online functionality for the presence
*				 engine
*
*/



// INCLUDE FILES
#include "MIMPSReconnectNoteObserver.h"
#include "CIMPSReconnectNoteHandler.h"
#include "IMPSCommonUiDebugPrint.h"
#include "cimpspresenceaaconnectionng.h"
#include "impscommonuibuilddefinitions.h"

#include <AknGlobalConfirmationQuery.h>
#include <flogger.h>
#include <e32std.h>
#include <PEngAOPluginNG.rsg>
#include <bautils.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

_LIT( KAOPluginResourceFile, "PENGAOPluginNG.rsc" );

_LIT( KAOPluginResourcePath, "\\Resource\\" );


// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSReconnectNoteHandler::CIMPSReconnectNoteHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSReconnectNoteHandler::CIMPSReconnectNoteHandler() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIMPSReconnectNoteHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSReconnectNoteHandler::ConstructL( TIMPSConnectionClient aClient )
    {
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    iConnectionUI = CIMPSPresenceAAConnection::NewL( aClient );
    iClient = aClient;

    User::LeaveIfError( iFs.Connect() );

    TFileName resourceFileName;
    resourceFileName.Zero();
    //artificial variable scope to ease the stack usage
    TFileName drivePath;
    Dll::FileName( drivePath );
    resourceFileName.Append( TParsePtrC( drivePath ).Drive() );
    //resourceFileName.Append( KDC_RESOURCE_FILES_DIR() )
    //
    // contains big letters
    resourceFileName.Append( KAOPluginResourcePath() );
    resourceFileName.Append( KAOPluginResourceFile );

    if ( resourceFileName.Length() > 0 )
        {
        // when baflutils gets an empty string, it returns "C:",
        // which breaks things
        BaflUtils::NearestLanguageFile( iFs, resourceFileName );
        }

    iResFile.OpenL( iFs, resourceFileName );
    iResFile.ConfirmSignatureL();
    }

// -----------------------------------------------------------------------------
// CIMPSReconnectNoteHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSReconnectNoteHandler* CIMPSReconnectNoteHandler::NewL( TIMPSConnectionClient aClient )
    {
    CIMPSReconnectNoteHandler* self = new( ELeave ) CIMPSReconnectNoteHandler;

    CleanupStack::PushL( self );
    self->ConstructL( aClient );
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CIMPSReconnectNoteHandler::~CIMPSReconnectNoteHandler()
    {
    Cancel();
    delete iGlobalConfirmationQuery;
    delete iConnectionUI;
    iObservers.Close();
    iResFile.Close();
    iFs.Close();
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::DoCancel
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::DoCancel()
    {
    iGlobalConfirmationQuery->CancelConfirmationQuery();
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::RunL
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::RunL()
    {
    TInt answer( iStatus.Int() );
    HandleNoteAnswered( answer );
    Cancel();
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::RemoveObserver
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::RemoveObserver( MIMPSReconnectNoteObserver* aObserver )
    {
    TInt index( iObservers.Find( aObserver ) );

    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::ShowNote
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::ShowNote( MIMPSReconnectNoteObserver* aObserver )
    {
    TRAPD( err, DoShowNoteL( aObserver ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        RemoveObserver( aObserver );
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::CancelNoteShowing
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::CancelNoteShowing( MIMPSReconnectNoteObserver* aObserver )
    {
    if ( IsActive() )
        {
        Cancel();
        }
    RemoveObserver( aObserver );
    HandleNoteCancelled();
    iObservers.Reset();
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::DoShowNoteL
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::DoShowNoteL( MIMPSReconnectNoteObserver* aObserver )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSReconnectNoteHandler::StartShowing" ) );

    AddObserverL( aObserver );

    if ( !IsActive() )
        {
        HBufC* prompt = PromptTextLC();
        iGlobalConfirmationQuery->ShowConfirmationQueryL(
            iStatus, *prompt, R_AVKON_SOFTKEYS_YES_NO__YES );
        CleanupStack::PopAndDestroy( prompt ); // prompt
        SetActive();
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::RunError
//
// ---------------------------------------------------------
//
TInt CIMPSReconnectNoteHandler::RunError( TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSReconnectNoteHandler::RunError [%d]" ), aError );
    if ( IsActive() )
        {
        Cancel();
        }
    HandleNoteCancelled();
    iObservers.Reset();
    return KErrNone;
    }


// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::AddObserverL
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::AddObserverL( MIMPSReconnectNoteObserver* aObserver )
    {
    if ( iObservers.Find( aObserver ) == KErrNotFound )
        {
        User::LeaveIfError( iObservers.Append( aObserver ) );
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::HandleNoteAnswered
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::HandleNoteAnswered( TInt aAnswer )
    {
    for ( TInt index( 0 ); index < iObservers.Count(); ++index )
        {
        iObservers[index]->HandleNoteAnswered( aAnswer );
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::HandleNoteCanceled
//
// ---------------------------------------------------------
//
void CIMPSReconnectNoteHandler::HandleNoteCancelled()
    {
    for ( TInt index( 0 ); index < iObservers.Count(); ++index )
        {
        iObservers[index]->HandleNoteCancelled();
        }
    }

// ---------------------------------------------------------
// CIMPSReconnectNoteHandler::PromptTextLC
//
// ---------------------------------------------------------
//
HBufC* CIMPSReconnectNoteHandler::PromptTextLC()
    {
    HBufC* srvname = iConnectionUI->DefaultServerNameL( iClient );
    CleanupStack::PushL( srvname );
    HBufC* prompt = ReadResourceWithDynTextLC( R_QTN_CHAT_RECONNECT_FAILED, *srvname );
    CleanupStack::Pop( prompt );
    CleanupStack::PopAndDestroy( srvname );
    CleanupStack::PushL( prompt );
    return prompt;
    }

// -----------------------------------------------------------------------------
// CIMPSReconnectNoteHandler::ReadResourceLC()
// -----------------------------------------------------------------------------
//
HBufC* CIMPSReconnectNoteHandler::ReadResourceLC( TInt aNoteTextResourceId )
    {
    //Own resource reader for AA plug-in environment (no CONE facilities available)

    //resource texts are hold in TBUFs
    //TBUF == "A utility struct that holds one non-zero-terminated string."
    //==> no lead bytes in resource string

    TInt plainResourceId = 0x00000fff & aNoteTextResourceId; // Remove offset from id
    HBufC8* rawDataBuf = iResFile.AllocReadLC( plainResourceId );

    //raw data buffer is actually unicode text ==> treat it so
    TPtrC16 rawData( ( TUint16* ) rawDataBuf->Ptr(),
                     rawDataBuf->Length() / 2 );

    HBufC16* resourceData = rawData.AllocL();
    CleanupStack::PopAndDestroy( rawDataBuf );

    CleanupStack::PushL( resourceData );

    return resourceData;
    }


// -----------------------------------------------------------------------------
// CIMPSReconnectNoteHandler::ReadResourceWithDynTextLC()
// -----------------------------------------------------------------------------
//
HBufC* CIMPSReconnectNoteHandler::ReadResourceWithDynTextLC( TInt aNoteTextResourceId,
                                                             const TDesC& aInsertText )
    {
    //Own dynamic string loader for AA plug-in environment (no CONE facilities available)

    HBufC* resStringBuf = ReadResourceLC( aNoteTextResourceId );

    TInt reqDynLen = resStringBuf->Length() + aInsertText.Length();
    HBufC* dynStringBuf = HBufC::NewL( reqDynLen );
    TPtr dynString = dynStringBuf->Des();
    StringLoader::Format( dynString,
                          *resStringBuf,
                          -1,             //No index code in source string
                          aInsertText );

    CleanupStack::PopAndDestroy( resStringBuf );
    CleanupStack::PushL( dynStringBuf );
    return dynStringBuf;
    }


//  End of File

