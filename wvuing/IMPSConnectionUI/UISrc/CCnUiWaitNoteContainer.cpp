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
* Description:  Wait note container.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <aknwaitdialog.h>
#include "MCnUiWaitNoteObserver.h"
#include "CCnUiWaitNoteContainer.h"


// CONSTANTS
// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiWaitNoteContainer* CCnUiWaitNoteContainer::NewL( TInt aNoteStructureResourceId,
                                                      const TDesC& aLabelText,
                                                      MCnUiWaitNoteObserver* aObserver )
    {
    CCnUiWaitNoteContainer* self = new ( ELeave ) CCnUiWaitNoteContainer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aLabelText, aNoteStructureResourceId );
    CleanupStack::Pop( self ); //self
    return self;
    }


// Destructor
CCnUiWaitNoteContainer::~CCnUiWaitNoteContainer()
    {
    //NULL observer before dismissing the wait note
    //==>protects the observer from getting the wait note
    //cancel events caused by the note deletion
    iObserver = NULL;

    if ( iWaitNote )
        {
        //wait note showing, try to dismiss it
        TInt ignore;
        TRAP( ignore, iWaitNote->ProcessFinishedL() );

        //if the ProcessFinishedL() leaves,
        //force the dialog away with delete
        delete iWaitNote;
        }
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiWaitNoteContainer::CCnUiWaitNoteContainer( MCnUiWaitNoteObserver* aObserver )
        : iObserver( aObserver )
    {
    }

// Symbian OS default constructor can leave.
void CCnUiWaitNoteContainer::ConstructL( const TDesC& aLabelText, TInt aNoteStructureResourceId )
    {
    iWaitNote = new ( ELeave ) CAknWaitDialog( reinterpret_cast< CEikDialog** >
                                               ( &iWaitNote ) );

    // prepare LC:s push to CleanUpStack isn't a problem in case of leave, cos
    // CAknWaitDialog sets self pointer to NULL in its destructor
    iWaitNote->PrepareLC( aNoteStructureResourceId );

    iWaitNote->SetTextL( aLabelText );
    iWaitNote->SetCallback( this );

    //pops the dialog
    iWaitNote->RunLD();  // CSI: 50 # This sets iWaitNote as NULL
    }


// -----------------------------------------------------------------------------
// CCnUiWaitNoteContainer::PushLC()
// -----------------------------------------------------------------------------
//
void CCnUiWaitNoteContainer::PushLC()
    {
    CleanupStack::PushL( this );
    }


// -----------------------------------------------------------------------------
// CCnUiWaitNoteContainer::DialogDismissedL()
// from MProgressDialogCallback
// -----------------------------------------------------------------------------
//
void CCnUiWaitNoteContainer::DialogDismissedL( TInt /*aButtonId*/ )
    {
    if ( iObserver )
        {
        iObserver->HandleWaitNoteCancel();
        }
    }

//  End of File
