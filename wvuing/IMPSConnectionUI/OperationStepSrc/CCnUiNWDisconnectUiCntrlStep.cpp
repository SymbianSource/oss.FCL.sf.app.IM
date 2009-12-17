/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network connection close UI control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>

#include "CCnUiNWDisconnectUiCntrlStep.h"
#include "CCnUiConnCloser.h"

#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiSignaller.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiClientPlugin.h"

#include "CnUiErrors.h"



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiNWDisconnectUiCntrlStep* CCnUiNWDisconnectUiCntrlStep::NewLC(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLogoutSap,
    RArray< TIMPSConnectionClient >& aDisconnectedClients,
    TBool aMainLevelOperation,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiNWDisconnectUiCntrlStep* self = new ( ELeave )
    CCnUiNWDisconnectUiCntrlStep( aCCntxt,
                                  aLogoutSap,
                                  aDisconnectedClients,
                                  aMainLevelOperation );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    return self;
    }


// Destructor
CCnUiNWDisconnectUiCntrlStep::~CCnUiNWDisconnectUiCntrlStep()
    {
    iClientsToDisconn.Reset();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiNWDisconnectUiCntrlStep::CCnUiNWDisconnectUiCntrlStep(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLogoutSap,
    RArray< TIMPSConnectionClient >& aDisconnectedClients,
    TBool aMainLevelOperation )
        : CCnUiLogoutUiCntrlStep( aCCntxt, aLogoutSap ),
        iRequestedDisconnClients( aDisconnectedClients ),
        iMainlevelOperation( aMainLevelOperation )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiNWDisconnectUiCntrlStep::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiLogoutUiCntrlStep::ConstructL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CCnUiNWDisconnectUiCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiNWDisconnectUiCntrlStep::HandleCompleteL()
    {
    //client disconnect went ok

    //if there has been a client to really logout,
    //it is the first one in iClientsToDisconn
    if ( iClientsToDisconn.Count() > 0 )
        {
        //add it among disconnected clients
        TIMPSConnectionClient clientId = iClientsToDisconn[ 0 ];
        User::LeaveIfError( iRequestedDisconnClients.Append( clientId ) );

        //and remove from internal count array
        iClientsToDisconn.Remove( 0 );
        }


    //If there is more clients to disconnect, do it
    if ( iClientsToDisconn.Count() > 0 )
        {
        //there is still clients to disconnect
        return ECnUiRestartStep;
        }
    else
        {
        //all disconnects done
        return ECnUiStepContinueTeardown;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiNWDisconnectUiCntrlStep::UndoStepL()
// -----------------------------------------------------------------------------
//
void CCnUiNWDisconnectUiCntrlStep::UndoStepL()
    {
    //nothing to undo
    }


// -----------------------------------------------------------------------------
// CCnUiNWDisconnectUiCntrlStep::DoInitLogoutStepL()
// Template method
// -----------------------------------------------------------------------------
//
TInt CCnUiNWDisconnectUiCntrlStep::DoInitLogoutStepL( TIMPSConnectionClient& aClient )
    {
    if ( !iInitDone )
        {
        //NWDisconnect initialization is done only at first run round

        //if this disconnect is main level operation, then
        //here is needed check for simultaneous operations, else not
        if ( iMainlevelOperation )
            {
            if ( iCCntxt.Signaller().OperationRunning() )
                {
                ShowSimultaneousLoginOperationNoteL();
                return KCnUiErrorLoginOperationAlreadyInUse;
                }
            }

        //gather the disconnect data & clients
        InitNWDisconnectL();
        iInitDone = ETrue;
        if ( iClientsToDisconn.Count() == 0 )
            {
            //there isn't any clients to disconnect
            return KCnUiErrorNoClientsToDisconnect;
            }
        }

    //in all rounds a next client is initialized for logout
    //index zero indicates here that the first client id is handled
    if ( iClientsToDisconn.Count() > 0 )
        {
        aClient = iClientsToDisconn[ 0 ];
        return ECnUiLogoutInitContinue;
        }

    else
        {
        //no more clients to logout
        return KErrNone; //breaks away from RunStepL() with KErrNone
        }
    }



// -----------------------------------------------------------------------------
// CCnUiNWDisconnectUiCntrlStep::DoShowLogoutOkNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiNWDisconnectUiCntrlStep::DoShowLogoutOkNoteL( TIMPSConnectionClient aClient )
    {
    switch ( aClient )
        {
        case EIMPSConnClientPEC:
            {
            iCCntxt.Ui().ShowNoteL( ECnUiDisconnectedPEC );
            break;
            }

        case EIMPSConnClientIM:
            {
            iCCntxt.Ui().ShowNoteL( ECnUiDisconnectedIM );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCnUiNWDisconnectUiCntrlStep::InitNWDisconnectL()
// -----------------------------------------------------------------------------
//
void CCnUiNWDisconnectUiCntrlStep::InitNWDisconnectL()
    {
    //external parameters
    iRequestedDisconnClients.Reset();
    iLogoutSap.Reset();

    //internal count array
    iClientsToDisconn.Reset();

    MCnUiConnectionHandler& connHandler = iCCntxt.ConnHandler();
    //
    if ( connHandler.GetLoggedInSapL( iLogoutSap, EIMPSConnClientIM ) )
        {
        //get clients to be disconnected
        //NOTE!! All currently connected clients needs to be disconnected
        //so no filtering here
        connHandler.GetLoggedInClientsL( iClientsToDisconn );
        }
    }



//  End of File
