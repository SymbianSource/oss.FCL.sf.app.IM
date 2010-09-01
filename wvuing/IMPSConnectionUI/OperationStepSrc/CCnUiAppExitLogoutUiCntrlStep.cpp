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
* Description:  Application exit logout UI control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>

#include "CCnUiAppExitLogoutUiCntrlStep.h"
#include "CCnUiConnCloser.h"

#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiClientPlugin.h"
#include "MCnUiSignaller.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiConnModeHandler.h"

#include "CnUiErrors.h"



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiAppExitLogoutUiCntrlStep* CCnUiAppExitLogoutUiCntrlStep::NewLC(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLogoutSap,
    TIMPSExitType aExitType,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiAppExitLogoutUiCntrlStep* self = new ( ELeave ) CCnUiAppExitLogoutUiCntrlStep( aCCntxt,
            aLogoutSap,
            aExitType );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    return self;
    }


// Destructor
CCnUiAppExitLogoutUiCntrlStep::~CCnUiAppExitLogoutUiCntrlStep()
    {
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiAppExitLogoutUiCntrlStep::CCnUiAppExitLogoutUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                                              CIMPSSAPSettings& aLogoutSap,
                                                              TIMPSExitType aExitType )
        : CCnUiLogoutUiCntrlStep( aCCntxt, aLogoutSap ),
        iExitType( aExitType )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiAppExitLogoutUiCntrlStep::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiLogoutUiCntrlStep::ConstructL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CCnUiAppExitLogoutUiCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiAppExitLogoutUiCntrlStep::HandleCompleteL()
    {
    if ( iExitType == EIMPSNormalExit )
        {
        //Let the base class issue the connection restore if needed.
        return CCnUiLogoutUiCntrlStep::HandleCompleteL();
        }

    //in other cases the reconnection is not done
    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiAppExitLogoutUiCntrlStep::UndoStepL()
// -----------------------------------------------------------------------------
//
void CCnUiAppExitLogoutUiCntrlStep::UndoStepL()
    {
    //nothing to undo
    }


// -----------------------------------------------------------------------------
// CCnUiAppExitLogoutUiCntrlStep::DoInitLogoutStepL()
// Template method
// -----------------------------------------------------------------------------
//
TInt CCnUiAppExitLogoutUiCntrlStep::DoInitLogoutStepL( TIMPSConnectionClient& aClient )
    {

    const TIMPSConnectionClient clientId = iCCntxt.ControlledClient();

    //what is the current client current login mode??
    //if it is something different than manual, then logout isn't needed
    MCnUiClientPlugin& cPlugin = iCCntxt.ClientPluginL( clientId, *iNWSessionSlotID );
    if ( cPlugin.CurrentConnectionModeSettingL() != ECnUiCMManual )
        {
        return KErrNone;    //breaks away from RunStepL() with KErrNone
        }


    //Let first the base class to do its initializations & checks
    //base will initialize the aClient properly to current client if logout is really needed
    TInt baseInitStatus = CCnUiLogoutUiCntrlStep::DoInitLogoutStepL( aClient );
    if ( baseInitStatus != ECnUiLogoutInitContinue )
        {
        //base class init wants to break here
        //(there isn't no need for logout or it can't be issued)
        return baseInitStatus;
        }



    //Exit is needed
    //If this normal exit case, ask confirmation from user
    if ( iExitType == EIMPSNormalExit )
        {
        if ( !iCCntxt.Ui().ConfirmationQueryL( ECnUiCloseConnectionToNetwork ) )
            {
            //User declined the logout
            return KErrCancel;
            }
        }

    //In other cases the logout can be carried on.
    //Because the base class didn't wan't to break,
    //it initialized the contexts properly.
    return ECnUiLogoutInitContinue;
    }


//  End of File
