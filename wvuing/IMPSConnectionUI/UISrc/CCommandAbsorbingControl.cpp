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
* Description:  Command absorber to eat all key presses.
*
*/

//INCLUDE FILES
#include <E32std.h>
#include <eikappui.h>
#include <eikenv.h>
#include <eikbtgpc.h>

#include "CCommandAbsorbingControl.h"
#include "CnUiPanics.h"
#include "impscommonuibuilddefinitions.h"


// ================= CCommandFlush MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::CCommandFlush()
// -----------------------------------------------------------------------------
//
CCommandAbsorbingControl::CCommandFlush::CCommandFlush()
        : CActive( EPriorityLow )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::~CCommandFlush()
// -----------------------------------------------------------------------------
//
CCommandAbsorbingControl::CCommandFlush::~CCommandFlush()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::Flush()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::CCommandFlush::Flush()
    {
    if ( iWait.IsStarted() )
        {
        //only one flush at time
        return;
        }

    //issue one active scheduler round
    //actually it might be several because this is a low priority one
    //and it lets other higher ones proceed first...
    TRequestStatus *pS = &iStatus;
    User::RequestComplete( pS, KErrNone );
    SetActive();

    //wait with the scheduler loop
    // Code scanner warning "active object called without checking
    // whether it is active or cancelling it first" ignored because
    // CActiveSchedulerWait is not an active object
    iWait.Start(); // CSI: 10 # See above
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::RunL()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::CCommandFlush::RunL()
    {
    Completed();
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::RunError()
// -----------------------------------------------------------------------------
//
TInt CCommandAbsorbingControl::CCommandFlush::RunError( TInt /*aError*/ )
    {
    Completed();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::DoCancel()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::CCommandFlush::DoCancel()
    {
    Completed();
    }

// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::CCommandFlush::Completed()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::CCommandFlush::Completed()
    {
    //break away from the waiting scheduler loop
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCommandAbsorbingControl* CCommandAbsorbingControl::NewL()
    {
    CCommandAbsorbingControl* self = new ( ELeave ) CCommandAbsorbingControl();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCommandAbsorbingControl::~CCommandAbsorbingControl()
    {
    StopAbsorbingIfNeeded();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCommandAbsorbingControl::CCommandAbsorbingControl()
        : iAbsorbOn( EFalse )
    {
    }


// Symbian OS default constructor can leave.
void CCommandAbsorbingControl::ConstructL()
    {
    //if we don't have application ui,
    //then there is no need to consume commands
    iAppUi = CEikonEnv::Static()->EikAppUi();
    iCba = CEikButtonGroupContainer::Current();
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::AbsorbCommandsLC()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::AbsorbCommandsLC()
    {
    if ( !iAbsorbOn )
        {
        CleanupStack::PushL( TCleanupItem( CCommandAbsorbingControl::ClearCommandAbsorb,
                                           this ) );

        iAbsorbOn = ETrue;
        StartAbsorbingL();
        }

    else
        {
        //already absorbing commands, don't create a new
        //just push NULL object to meet the method semantics
        //this doesn't do actually any harm, just eats one slot from
        //CleanupStack but anyway it is simpler than letting the client
        //manage that it doesn't have anything to pop and destroy...
        CleanupStack::PushL( ( CBase* ) NULL );
        }
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::FlushCommands()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::FlushCommands()
    {
    if ( iAbsorbOn )
        {
        iCommandFlusher.Flush();
        }
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CCommandAbsorbingControl::OfferKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                                       TEventCode /*aType*/ )
    {
    return EKeyWasConsumed;
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::StartAbsorbingL()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::StartAbsorbingL()
    {
    if ( iAppUi )
        {
        //add self to eat key events
        iAppUi->AddToStackL( this, ECoeStackPriorityDialog );
        }

    if ( iCba )
        {
        //clear CBA
        MakeCbaVisible( EFalse );
        }
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::StopAbsorbingIfNeeded()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::StopAbsorbingIfNeeded()
    {
    if ( iAbsorbOn )
        {
        if ( iAppUi )
            {
            //remove self
            iAppUi->RemoveFromStack( this );
            }

        if ( iCba )
            {
            //make CBA visible
            MakeCbaVisible( ETrue );
            }

        iAbsorbOn = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::MakeCbaVisible()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::MakeCbaVisible( TBool aVisible ) const
    {
    if ( iCba && iCba->ButtonGroup() )
        {
        TInt leftId( iCba->ButtonGroup()->CommandId( 0 ) );  //position 0 --> left
        TInt rightId( iCba->ButtonGroup()->CommandId( 2 ) ); //position 2 --> right
        TInt mskId = iCba->ButtonGroup()->CommandId( 3 );   // position 3 --> MSK

        iCba->MakeCommandVisible( leftId, aVisible );
        iCba->MakeCommandVisible( rightId, aVisible );
        iCba->MakeCommandVisible( mskId, aVisible );

        iCba->DrawNow();
        }
    }


// -----------------------------------------------------------------------------
// CCommandAbsorbingControl::ClearCommandAbsorb()
// -----------------------------------------------------------------------------
//
void CCommandAbsorbingControl::ClearCommandAbsorb( TAny* aCommandAbsorber )
    {
    __ASSERT_ALWAYS( aCommandAbsorber, CnUiPanic( EIMPSConn_NULLPtr ) );

    CCommandAbsorbingControl* self = static_cast< CCommandAbsorbingControl* >( aCommandAbsorber );
    self->StopAbsorbingIfNeeded();
    }


//  End of File






