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
* Description:  Query container implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <Avkon.hrh>
#include <IMPSConnectionUiNG.rsg>


#include "CCnUiQueryContainer.h"
#include "MCnUiConnQueryObserver.h"
#include "CnUiPanics.h"


// CONSTANTS
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::NewL()
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer::CCnUiNonWaitingQueryDialog*
CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::NewL(
    TNonBlockingQueryType aType,
    const TDesC& aLabelText,
    MCnUiConnQueryObserver& aObserver,
    CCnUiNonWaitingQueryDialog** aSelfPtr )
    {
    CCnUiNonWaitingQueryDialog* self = new ( ELeave ) CCnUiNonWaitingQueryDialog( aObserver,
                                                                                  aSelfPtr );
    CleanupStack::PushL( self );
    self->LaunchQueryLD( aType, aLabelText );
    CleanupStack::Pop( self ); //self
    return self;
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::~CCnUiNonWaitingQueryDialog()
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::~CCnUiNonWaitingQueryDialog()
    {
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        iSelfPtr = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::CCnUiNonWaitingQueryDialog()
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::CCnUiNonWaitingQueryDialog(
    MCnUiConnQueryObserver& aObserver,
    CCnUiNonWaitingQueryDialog** aSelfPtr )
        : iObserver( aObserver ),
        iSelfPtr( aSelfPtr )
    {
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::LaunchQueryLD()
// -----------------------------------------------------------------------------
//
void CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::LaunchQueryLD(
    TNonBlockingQueryType aType,
    const TDesC& aLabelText )
    {
    SetPromptL( aLabelText );
    switch ( aType )
        {
        case ENBConfirmationQuery:
            {
            //non waiting dialog ==> result is reported to observer from OkToExitL()
            ExecuteLD( R_NON_WAITING_CONFIRMATION_QUERY );
            break;
            }

        case ENBConfirmedErrorNote:
            {
            //non waiting dialog ==> result is reported to observer from OkToExitL()
            ExecuteLD( R_NON_WAITING_CONFIRMED_ERROR_NOTE );
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::OkToExitL()
// -----------------------------------------------------------------------------
//
TBool CCnUiQueryContainer::CCnUiNonWaitingQueryDialog::OkToExitL( TInt aButtonId )
    {
    TBool queryAccepted = ( aButtonId == EAknSoftkeyYes );
    iObserver.HandleQueryResult( queryAccepted );
    return ETrue;
    }



// -----------------------------------------------------------------------------
// CCnUiQueryContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer* CCnUiQueryContainer::NewL( TNonBlockingQueryType aType,
                                                const TDesC& aLabelText,
                                                MCnUiConnQueryObserver& aObserver )
    {
    CCnUiQueryContainer* self = new ( ELeave ) CCnUiQueryContainer(  );
    CleanupStack::PushL( self );
    self->iDlg = CCnUiNonWaitingQueryDialog::NewL( aType,
                                                   aLabelText,
                                                   aObserver,
                                                   &self->iDlg );

    CleanupStack::Pop( self ); //self
    return self;
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::~CCnUiQueryContainer()
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer::~CCnUiQueryContainer()
    {
    delete iDlg;
    }


// -----------------------------------------------------------------------------
// CCnUiQueryContainer::CCnUiQueryContainer()
// -----------------------------------------------------------------------------
//
CCnUiQueryContainer::CCnUiQueryContainer()
    {
    }

//  End of File
