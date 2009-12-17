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
* Description:  AA logout control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>
#include "CCnUiAALogoutCntrlStep.h"
#include "CCnUiConnCloser.h"

#include "MCnUiBaseControlContext.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiSignaller.h"
#include "CnUiErrors.h"
#include "MCnUiGlobalNotificationUiFacade.h"



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiAALogoutCntrlStep* CCnUiAALogoutCntrlStep::NewLC( MCnUiBaseControlContext& aCCntxt,
                                                       CIMPSSAPSettings& aLogoutSap,
                                                       TBool aIsScheduled,
                                                       CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiAALogoutCntrlStep* self = new ( ELeave ) CCnUiAALogoutCntrlStep( aCCntxt,
                                                                          aLogoutSap,
                                                                          aIsScheduled );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    return self;
    }


// Destructor
CCnUiAALogoutCntrlStep::~CCnUiAALogoutCntrlStep()
    {
    delete iConnCloser;
    delete iNWSessionSlotID;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiAALogoutCntrlStep::CCnUiAALogoutCntrlStep( MCnUiBaseControlContext& aCCntxt,
                                                CIMPSSAPSettings& aLogoutSap,
                                                TBool aIsScheduled )
        : iCCntxt( aCCntxt ),
        iLogoutSap( aLogoutSap ),
        iIsScheduled( aIsScheduled )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiAALogoutCntrlStep::ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iConnCloser = CCnUiConnCloser::NewL();

    iNWSessionSlotID = aNWSessionSlotID.CloneL();
    }


// -----------------------------------------------------------------------------
// CCnUiAALogoutCntrlStep::RunStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiAALogoutCntrlStep::RunStepL()
    {
    TIMPSConnectionClient clientId = iCCntxt.ControlledClient();

    //is this client logged in?
    //if not, then no need to proceed
    if ( !iCCntxt.ConnHandler().TheClientLoggedInL( clientId ) )
        {
        return KErrNone;
        }

    //Is this operation free to proceed?
    //However no signalling about logout is used...
    if ( iCCntxt.Signaller().OperationRunning() )
        {
        return KCnUiErrorLoginOperationAlreadyInUse;
        }

    // we only ask confirmation from user about the logout if this is
    // a scheduled logout
    if ( iIsScheduled )
        {
        // create global notification ui facade for sending channel message
        MCnUiGlobalNotificationUiFacade* uiFacade = &( iCCntxt.GlobalNotificationUiL() );

        RArray<TIMPSConnectionClient> client;
        CleanupClosePushL( client );
        User::LeaveIfError( client.Append( clientId ) );

        // ask confirmation from user about the logout
        if ( uiFacade->CGQActiveConnectionScheduledCloseL( client.Array() ) )
            {
            // user accepted the logout or query timed out -> do the logout

            //get SAP from where logging out..
            iCCntxt.ConnHandler().GetLoggedInSapL( iLogoutSap, clientId );

            //close the client connection using the connection closer
            TInt logoutStatus = iConnCloser->MakeLogoutForClient( clientId,
                                                                  iLogoutSap,
                                                                  iCCntxt.ClientPluginL( clientId, *iNWSessionSlotID ),
                                                                  iCCntxt.ConnHandler(),
                                                                  NULL,
                                                                  *iNWSessionSlotID );
            CleanupStack::PopAndDestroy(); // client
            return logoutStatus;
            }
        else
            {
            // user denied the query
            CleanupStack::PopAndDestroy(); // client
            return KErrCouldNotDisconnect;
            }
        }
    else
        {
        // this is not a scheduled logout, we do not need to query from user
        //get SAP from where logging out..
        iCCntxt.ConnHandler().GetLoggedInSapL( iLogoutSap, clientId );

        //close the client connection using the connection closer
        TInt logoutStatus = iConnCloser->MakeLogoutForClient( clientId,
                                                              iLogoutSap,
                                                              iCCntxt.ClientPluginL( clientId, *iNWSessionSlotID ),
                                                              iCCntxt.ConnHandler(),
                                                              NULL,
                                                              *iNWSessionSlotID );
        return logoutStatus;
        }
    }



// -----------------------------------------------------------------------------
// CCnUiAALogoutCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiAALogoutCntrlStep::HandleCompleteL()
    {
    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiAALogoutCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiAALogoutCntrlStep::UndoStepL()
    {
    }

//  End of File
