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
* Description:  Helper class for wait notes
*
*/



// INCLUDE FILES
#include    "CCAWaitNote.h"
#include    "chatdebugprint.h"

#include    <ChatNG.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAWaitNote::CCAWaitNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAWaitNote::CCAWaitNote()
    {
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAWaitNote::ConstructL( const TDesC& aText,
                                       TBool aShowImmediately,
                                       TBool aCanBeCanceledByUser,
                                       MCAWaitNoteObserver* aObserver )
    {
    // don't give dialog pointer as a parameter because
    // it gets invalid after deletion of this instance
    iWaitDialog = new( ELeave )CAknWaitDialog( NULL, aShowImmediately );
    if ( aCanBeCanceledByUser )
        {
        iWaitDialog->PrepareLC( R_CHAT_WAIT_NOTE_TEMPLATE_WITH_CANCEL );
        }
    else
        {
        iWaitDialog->PrepareLC( R_CHAT_WAIT_NOTE_TEMPLATE );
        }
    iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
    iWaitDialog->SetTextL( aText );
    iWaitDialog->SetCallback( this );

    SetObserver( aObserver );
    iWaitDialog->RunLD();   // CSI: 50 # iWaitDialog is not owned by us
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::ShowWaitNoteLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCAWaitNote* CCAWaitNote::ShowWaitNoteLC( const TDesC& aText,
                                                   TBool aShowImmediately /*= EFalse*/,
                                                   TBool aCanBeCanceledByUser /*= EFalse*/,
                                                   MCAWaitNoteObserver* aObserver /*= NULL*/ )
    {
    CCAWaitNote* self = new( ELeave ) CCAWaitNote();
    CleanupStack::PushL( self );
    self->ConstructL( aText, aShowImmediately, aCanBeCanceledByUser, aObserver );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::ShowWaitNoteLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCAWaitNote* CCAWaitNote::ShowWaitNoteLC( TInt aTextResource,
                                                   TBool aShowImmediately /*= EFalse*/,
                                                   TBool aCanBeCanceledByUser /*= EFalse*/,
                                                   MCAWaitNoteObserver* aObserver /*= NULL*/ )
    {
    CCAWaitNote* self = new( ELeave ) CCAWaitNote();
    CleanupStack::PushL( self );

    HBufC* text = CCoeEnv::Static()->AllocReadResourceLC( aTextResource );
    self->ConstructL( *text, aShowImmediately, aCanBeCanceledByUser, aObserver );
    CleanupStack::PopAndDestroy( text );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::ShowWaitNoteL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCAWaitNote* CCAWaitNote::ShowWaitNoteL( TInt aTextResource,
                                                  TBool aShowImmediately /*= EFalse*/,
                                                  TBool aCanBeCanceledByUser /*= EFalse*/,
                                                  MCAWaitNoteObserver* aObserver /*= NULL*/ )
    {
    CCAWaitNote* self = ShowWaitNoteLC( aTextResource,
                                        aShowImmediately,
                                        aCanBeCanceledByUser,
                                        aObserver );

    CleanupStack::Pop( self );
    return self;
    }




// Destructor
CCAWaitNote::~CCAWaitNote()
    {
    DismissDialog();
    }


// -----------------------------------------------------------------------------
// CCAWaitNote::DialogDismissedL
// Called when dialog is dismissed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAWaitNote::DialogDismissedL( TInt aButtonId )
    {
    CHAT_DP_FUNC_ENTER( "CCAWaitNote::DialogDismissedL" );
    iWaitDialog = NULL;
    if ( iObserver )
        {
        iObserver->NoteCanceled( aButtonId );
        }
    CHAT_DP_FUNC_DONE( "CCAWaitNote::DialogDismissedL" );
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::DismissDialog
// Dismisses the dialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAWaitNote::DismissDialog()
    {
    CHAT_DP_FUNC_ENTER( "CCAWaitNote::DismissDialog" );
    if ( iWaitDialog )
        {
        iWaitDialog->SetCallback( NULL );
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        CHAT_DP( D_CHAT_LIT( "CCAWaitNote::DismissDialog: %d" ), err );

        if ( err != KErrNone )
            {
            // don't know for sure that ProcessFinishedL deletes the dialog
            // in all cases, so let's delete it
            delete iWaitDialog;
            iWaitDialog = NULL;
            CActiveScheduler::Current()->Error( err );
            }

        TRAP_IGNORE( DialogDismissedL( 0 ) );
        }
    CHAT_DP_FUNC_DONE( "CCAWaitNote::DismissDialog" );
    }

// -----------------------------------------------------------------------------
// CCAWaitNote::SetObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAWaitNote::SetObserver( MCAWaitNoteObserver* aObserver )
    {
    iObserver = aObserver;
    }

//  End of File
