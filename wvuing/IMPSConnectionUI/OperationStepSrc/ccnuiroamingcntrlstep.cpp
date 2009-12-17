/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control step driver.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <e32property.h>
#include <StringLoader.h>
#include <aknmessagequerydialog.h>
#include <ImpsConnectionUiNg.rsg>

#include "IMPSUIDDefs.h"
#include "MIMPSSharedData.h"
#include "ccnuiroamingcntrlstep.h"
#include "cimpsresourcereader.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiRoamingCntrlStep* CCnUiRoamingCntrlStep::NewLC()
    {
    CCnUiRoamingCntrlStep* self = new ( ELeave ) CCnUiRoamingCntrlStep();
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CCnUiRoamingCntrlStep::~CCnUiRoamingCntrlStep()
    {
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiRoamingCntrlStep::CCnUiRoamingCntrlStep()
    {
    }

// -----------------------------------------------------------------------------
// CCnUiRoamingCntrlStep::RunStepL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
TInt CCnUiRoamingCntrlStep::RunStepL()
    {
    // Show  warning note
    TInt okToContinue = KErrNone;

    // Ask user if wishes to continue
    TInt commandId = ShowRoamingDlgL();
    if ( commandId == EAknSoftkeyYes )
        {
        // Set the property of  dialog shown to True
        RProperty roaming;
        User::LeaveIfError( roaming.Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, ETrue ) );
        okToContinue = KErrNone;
        }
    else
        {
        okToContinue = KErrCancel;
        }

    return okToContinue;
    }


// -----------------------------------------------------------------------------
// CCnUiRoamingCntrlStep::ShowWAPQueryL
// -----------------------------------------------------------------------------
//
TInt CCnUiRoamingCntrlStep::ShowRoamingDlgL()
    {
    CAknMessageQueryDialog* msgQueryDlg = new( ELeave )CAknMessageQueryDialog( CAknMessageQueryDialog::ENoTone );
    CleanupStack::PushL( msgQueryDlg );

    msgQueryDlg->PrepareLC( R_MESSAGE_QUERY );
    msgQueryDlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_YES_NO );

    HBufC* resourceText = NULL;
    resourceText = StringLoader::LoadL( R_QTN_NOTE_ROAMING_ORANGE );
    msgQueryDlg->SetMessageText( *resourceText );

    resourceText = StringLoader::LoadL( R_QTN_NOTE_TITLE_ROAMING_ORANGE );
    msgQueryDlg->SetHeaderText( *resourceText );

    CleanupStack::Pop( msgQueryDlg );

    TInt command = msgQueryDlg->RunLD();
    return command;
    }


// -----------------------------------------------------------------------------
// CCnUiRoamingCntrlStep::HandleCompleteL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiRoamingCntrlStep::HandleCompleteL()
    {
    //nothing to complete
    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiRoamingCntrlStep::UndoStepL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
void CCnUiRoamingCntrlStep::UndoStepL()
    {
    //nothing to undo
    RProperty roaming;
    User::LeaveIfError( roaming.Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, EFalse ) );
    }

//  End of File


