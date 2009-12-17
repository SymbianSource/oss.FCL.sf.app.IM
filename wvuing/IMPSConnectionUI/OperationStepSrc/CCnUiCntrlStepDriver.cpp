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
* Description:  Control step driver.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include "CCnUiCntrlStepDriver.h"
#include "MCnUiCntrlStep.h"

#include "CnUiPanics.h"
#include "IMPSCommonUiDebugPrint.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiCntrlStepDriver* CCnUiCntrlStepDriver::NewL()
    {
    CCnUiCntrlStepDriver* self = CCnUiCntrlStepDriver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// Two-phased constructor.
CCnUiCntrlStepDriver* CCnUiCntrlStepDriver::NewLC()
    {
    CCnUiCntrlStepDriver* self = new ( ELeave ) CCnUiCntrlStepDriver();
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
CCnUiCntrlStepDriver::~CCnUiCntrlStepDriver()
    {
    //step driver owns steps
    iSteps.ResetAndDestroy();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiCntrlStepDriver::CCnUiCntrlStepDriver()
    {
    }



// -----------------------------------------------------------------------------
// CCnUiCntrlStepDriver::AppendStepL()
// -----------------------------------------------------------------------------
//
void CCnUiCntrlStepDriver::AppendStepL( MCnUiCntrlStep* aStep )
    {
    __ASSERT_ALWAYS( aStep,
                     CnUiPanicOrLeaveL( EIMPSConn_NULLPtr,
                                        KErrArgument ) );

    User::LeaveIfError( iSteps.Append( aStep ) );
    }


// -----------------------------------------------------------------------------
// CCnUiCntrlStepDriver::ExecuteL()
// -----------------------------------------------------------------------------
//
TInt CCnUiCntrlStepDriver::ExecuteL()
    {
    __ASSERT_ALWAYS( iSteps.Count() > 0,
                     CnUiPanicOrLeaveL( EIMPSConnStepDriverUninitialized,
                                        KErrNotSupported ) );

    //Run each step untill all done or first error occured
    //At first execution round start form initial step.
    TInt stepResult = KErrNone;
    const TInt totalStepCount = iSteps.Count();
    TInt lastRunnedIndex = 0;
    TInt runIndex = 0;

    TBool restartNeeded( ETrue );
    while ( restartNeeded )
        {
        // by default we do not need to restart
        restartNeeded = EFalse;
        stepResult = KErrNone;

        while ( ( runIndex < totalStepCount ) && ( stepResult == KErrNone ) )
            {
            MCnUiCntrlStep* cStep = iSteps[ runIndex ];
            stepResult = cStep->RunStepL();
            lastRunnedIndex = runIndex;
            runIndex++;
            }

        //Teardown each so far run step untill all are teardowned.
        //Correct teardown operation is called according the last
        //runned step return status.
        //If steps were runned succesfully, some operation may
        //restart processing if it wishes so. Restart is
        //done from step wishing so forward.
        TInt teardownIndex = lastRunnedIndex;
        while ( ( teardownIndex >= 0 ) && ( !restartNeeded ) )
            {
            MCnUiCntrlStep* cStep = iSteps[ teardownIndex ];
            if ( stepResult == KErrNone )
                {
                if ( cStep->HandleCompleteL() == ECnUiRestartStep )
                    {
                    //Restart runing from this step
                    runIndex = teardownIndex;
                    // we need to restart the whole thing from start
                    restartNeeded = ETrue;
                    }
                }
            else
                {
                cStep->UndoStepL();
                }

            teardownIndex--;
            }
        }

    //return the status from last run step
    return stepResult;
    }


//  End of File


