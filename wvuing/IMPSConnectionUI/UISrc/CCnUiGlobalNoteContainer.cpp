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
* Description:  Active object based global note container.
*
*/


// INCLUDE FILES
#include <E32std.h>

#include <avkon.rsg>
#include <Avkon.hrh>
#include <aknglobalnote.h>
#include <impsconnectionuing.rsg>

#include "CCnUiGlobalNoteContainer.h"
#include "IMPSCommonUiDebugPrint.h"
#include "impscommonuibuilddefinitions.h"


// CONSTANTS
/**
 * Global note framework seems to handle too
 * long prompt texts with leaving. This might be
 * a fatal problem for AA plug-ins
 * ==> clip the text length to maximum allowed by S60.
 */
const TInt KGlobalNoteMaxTextLength = 256;


// ================= LOCAL FUNCTIONS =======================

// -----------------------------------------------------------------------------
// AllocMaxGlobalTextL()
// -----------------------------------------------------------------------------
//
HBufC* AllocMaxGlobalTextL( const TDesC& aPromptText )
    {
    return aPromptText.Left( KGlobalNoteMaxTextLength ).AllocL();
    }



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiGlobalNoteContainer* CCnUiGlobalNoteContainer::NewLC()
    {
    CCnUiGlobalNoteContainer* self = new ( ELeave ) CCnUiGlobalNoteContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiGlobalNoteContainer::~CCnUiGlobalNoteContainer()
    {
    Cancel();
    delete iGlobalNote;

    delete iPromptText;
    delete iDetailedPromptText;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiGlobalNoteContainer::CCnUiGlobalNoteContainer()
        : CActive( CActive::EPriorityStandard ),
        iState( ECnUiGNCIdle )
    {
    CActiveScheduler::Add( this );
    }


// EPOC default constructor can leave.
void CCnUiGlobalNoteContainer::ConstructL()
    {
    iGlobalNote = CAknGlobalNote::NewL();
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::ShowWaitigDetailedNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::ShowWaitigDetailedNoteL( const TDesC& aPromptText,
                                                        const TDesC& aDetailedPromptText )
    {
    Cancel();
    ResetState();
    iPromptText = AllocMaxGlobalTextL( aPromptText );
    iDetailedPromptText = AllocMaxGlobalTextL( aDetailedPromptText );

    IssueDetailedMainNoteL();
    WaitCompletion();
    }



// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::ShowWaitingNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::ShowWaitingNoteL( const TDesC& aPromptText )
    {
    Cancel();
    ResetState();
    iPromptText = AllocMaxGlobalTextL( aPromptText );

    IssueWaitingNoteL();
    WaitCompletion();
    }



// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::RunL
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::RunL()
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGlobalNoteContainer::RunL() state=%d, status=%d" ), iState, iStatus );
    switch ( iState )
        {
        case ECnUiGNCShowingDetailedNoteMain:
            {
            //Main note completed
            if ( iStatus == EAknSoftkeyDetails )
                {
                //user selected details ==> show second detailed note
                IssueDetailedSecondNoteL();
                }
            break;
            }

        //Flow trough...
        case ECnUiGNCShowingDetailedNoteSecond:
        case ECnUiGNCShowingWaitingNote:
        case ECnUiGNCIdle:
        default:
            {
            break;
            }
        }


    if ( !IsActive() )
        {
        //if no subsequent processing steps started,
        //break away from scheduler loop...
        Completed();
        ResetState();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::DoCancel
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::DoCancel()
    {
    //Note container needs to be dismissed
    //If note cancellation fails, there isn't anything to do
    //since this container is executed on AA plug-in or similar side and
    //actual note is in holded Series60 UI framework...
    TInt ignore;
    TRAP( ignore, iGlobalNote->CancelNoteL( iNoteId ) );
    Completed();
    ResetState();
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::RunError
// -----------------------------------------------------------------------------
//
TInt CCnUiGlobalNoteContainer::RunError( TInt /*aError */ )
    {
    Completed();
    ResetState();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::WaitCompletion()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::WaitCompletion()
    {
    //wait with the scheduler loop
    IMPSCUI_DP_TXT( "CCnUiGlobalNoteContainer::WaitCompletion() - starting wait" );
    if ( !iWait.IsStarted() )
        {
        // Code scanner warning "active object called without checking
        // whether it is active or cancelling it first" ignored because
        // CActiveSchedulerWait is not an active object
        iWait.Start(); // CSI: 10 # See above
        }
    IMPSCUI_DP_TXT( "CCnUiGlobalNoteContainer::WaitCompletion() - wait done" );
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::Completed()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::Completed()
    {
    //break away from the waiting scheduler loop
    iWait.AsyncStop();
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::ResetState()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::ResetState()
    {
    delete iPromptText;
    delete iDetailedPromptText;
    iPromptText = NULL;
    iDetailedPromptText = NULL;

    iState = ECnUiGNCIdle;
    iStatus = KErrNotFound;
    iNoteId = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::IssueDetailedMainNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::IssueDetailedMainNoteL()
    {
    __ASSERT_ALWAYS( iPromptText, User::Leave( KErrArgument ) );

    iGlobalNote->SetSoftkeys( R_CNUI_SOFTKEYS_OK_DETAILS__OK );
    iNoteId = iGlobalNote->ShowNoteL( iStatus,
                                      EAknGlobalInformationNote,
                                      *iPromptText );
    SetActive();
    iState = ECnUiGNCShowingDetailedNoteMain;
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::IssueDetailedSecondNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::IssueDetailedSecondNoteL()
    {
    __ASSERT_ALWAYS( iDetailedPromptText, User::Leave( KErrArgument ) );

    iGlobalNote->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    iNoteId = iGlobalNote->ShowNoteL( iStatus,
                                      EAknGlobalInformationNote,
                                      *iDetailedPromptText );
    SetActive();
    iState = ECnUiGNCShowingDetailedNoteSecond;
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNoteContainer::IssueWaitingNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNoteContainer::IssueWaitingNoteL()
    {
    __ASSERT_ALWAYS( iPromptText, User::Leave( KErrArgument ) );


    iGlobalNote->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    iNoteId = iGlobalNote->ShowNoteL( iStatus,
                                      EAknGlobalInformationNote,
                                      *iPromptText );
    SetActive();
    iState = ECnUiGNCShowingWaitingNote;
    }


// end of file

