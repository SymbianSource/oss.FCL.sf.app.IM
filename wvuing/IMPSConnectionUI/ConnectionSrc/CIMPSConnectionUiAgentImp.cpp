/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Connection UI Agent implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <cimpsconnuiclntfilteredpreseventnotifierng.h>

#include "CIMPSConnectionUiAgentImp.h"
#include "MCnUiUiFacade.h"

#include "CnUiCommon.hrh"
#include "CCnUiClientGroupUiNotifyHandler.h"
#include "CCnUiClientGroupUiConnectionLostHandler.h"
#include "MCnUiConnModeHandler.h"




// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CIMPSConnectionUiAgentImp* CIMPSConnectionUiAgentImp::NewL( TIMPSConnectionClient aClient )
    {
    CIMPSConnectionUiAgentImp* self = new ( ELeave ) CIMPSConnectionUiAgentImp( aClient );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CIMPSConnectionUiAgentImp::~CIMPSConnectionUiAgentImp()
    {
    delete iClientGroupRemoteNotifyHandler;
    delete iClientGroupConnectionLostHandler;
    delete iClientConnModeHandler;

    delete iUi;
    delete iClntNotifier;
    delete iBrandedResourceFileName;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSConnectionUiAgentImp::CIMPSConnectionUiAgentImp( TIMPSConnectionClient aClient )
        : iClient( aClient )
    {
    }


// Symbian OS default constructor can leave.
void CIMPSConnectionUiAgentImp::ConstructL()
    {
    //agent services
    iClntNotifier = CIMPSConnUiClntFilteredPresEventNotifier::NewL( iClient );
    iClntNotifier->AddObserverL( this );
    iClntNotifier->StartL();

    //agent service handlers
    iClientGroupRemoteNotifyHandler = CCnUiClientGroupUiNotifyHandler::NewL( iClient );
    }

// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::StartServiceL()
// -----------------------------------------------------------------------------
//
TInt CIMPSConnectionUiAgentImp::StartServiceL( TIMPSConnectionUiAgentService aService )
    {
    switch ( aService )
        {
        case EIMPSNotifyActiveConnectionLostReasons:
            {
            if ( !iClientGroupConnectionLostHandler )
                {
                iClientGroupConnectionLostHandler =
                    CCnUiClientGroupUiConnectionLostHandler::NewL( iClient );
                }

            break;
            }

        case EIMPSQueryActiveConnectionScheduledClose:
            {
            if ( iBrandedResourceFileName )
                {
                iClientGroupRemoteNotifyHandler->UseResourceFileL( *iBrandedResourceFileName );
                }
            else
                {
                iClientGroupRemoteNotifyHandler->UseResourceFileL( KNullDesC() );
                }

            iClientGroupRemoteNotifyHandler->SetHandled(
                ECGQActiveConnectionScheduledClose,
                ETrue );
            break;
            }


        case EIMPSQueryActiveConnectionSuppressForScheduledConnection:
            {
            if ( iBrandedResourceFileName )
                {
                iClientGroupRemoteNotifyHandler->UseResourceFileL( *iBrandedResourceFileName );
                }
            else
                {
                iClientGroupRemoteNotifyHandler->UseResourceFileL( KNullDesC() );
                }
            iClientGroupRemoteNotifyHandler->SetHandled(
                ECGQActiveConnectionSuppressForScheduledConnection,
                ETrue );
            break;
            }


        case EIMPSReqisterAsSignificantConnectionClientInScheduling :
            {
            if ( !iClientConnModeHandler )
                {
                iClientConnModeHandler = CreateConnModeHandlerL();
                iClientConnModeHandler->CancelRegisteringAsSSClient();
                }
            break;
            }

        default:
            {
            break; //unknown service
            }
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::StopService()
// -----------------------------------------------------------------------------
//
TInt CIMPSConnectionUiAgentImp::StopService( TIMPSConnectionUiAgentService aService )
    {
    switch ( aService )
        {
        case EIMPSNotifyActiveConnectionLostReasons:
            {
            if ( iClientGroupConnectionLostHandler )
                {
                delete iClientGroupConnectionLostHandler;
                iClientGroupConnectionLostHandler = NULL;
                }
            break;
            }

        case EIMPSQueryActiveConnectionScheduledClose:
            {
            iClientGroupRemoteNotifyHandler->SetHandled( ECGQActiveConnectionScheduledClose,
                                                         EFalse );
            break;
            }


        case EIMPSQueryActiveConnectionSuppressForScheduledConnection:
            {
            iClientGroupRemoteNotifyHandler->SetHandled( ECGQActiveConnectionSuppressForScheduledConnection,
                                                         EFalse );
            break;
            }


        case EIMPSReqisterAsSignificantConnectionClientInScheduling :
            {
            if ( iClientConnModeHandler )
                {
                iClientConnModeHandler->CancelRegisteringAsSSClient();
                delete iClientConnModeHandler;
                iClientConnModeHandler = NULL;
                }

            break;
            }

        default:
            {
            break; //unknown service
            }
        }

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::StopAllServices()
// -----------------------------------------------------------------------------
//
void CIMPSConnectionUiAgentImp::StopAllServices()
    {
    //NOTE!! When adding new services, remember add those also here

    StopService( EIMPSNotifyActiveConnectionLostReasons );
    StopService( EIMPSQueryActiveConnectionScheduledClose );
    StopService( EIMPSQueryActiveConnectionSuppressForScheduledConnection );
    StopService( EIMPSReqisterAsSignificantConnectionClientInScheduling );
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::HandlePresenceEventL()
// From MIMPSConnUiPresenceEventObserver
// -----------------------------------------------------------------------------
//
void CIMPSConnectionUiAgentImp::HandlePresenceEventL( CIMPSConnUiPresEventNotifier* /*aNotifier*/,
                                                      const CPEngNWSessionSlotID2& /*aSessionSlotID*/,
                                                      TIMPSConnectionClient /*aClient*/,
                                                      TIMPSPresenceServiceEvent aEvent )
    {
    MCnUiUiFacade* tempUi = CreateUiFacadeL();
    if ( iUi )
        {
        delete iUi;
        }
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {

        if ( iUi )
            {
            iUi->SwitchResourceFileL( *iBrandedResourceFileName );
            }
        }

    if ( iClientGroupConnectionLostHandler )
        {
        if ( iBrandedResourceFileName )
            {
            iClientGroupConnectionLostHandler->UseResourceFileL( *iBrandedResourceFileName );
            }
        else
            {
            iClientGroupConnectionLostHandler->UseResourceFileL( KNullDesC() );
            }
        TRAPD( err, iClientGroupConnectionLostHandler->HandlePresenceEventL( aEvent ) );


        //When userA logs in with some user id. Now userB logs in with same user id.
        //Before UserA gets the notification from network of the server disconnection, userA
        //tries to exit the application. The Application crashes.

        //Reason:
        //*******
        //The UiSessionHandler calls for a HandlePresenceEventL(..) when UserA exits the application.
        //Before the completion of this handler, the netwrok sends the notification, the current active
        //object is put on a hold and the network disconnection active object takes the control.
        //This results in calling the same HandlePresenceEventL(..), wherein the iUi is deleted.
        //After completion the other thread which was interrupted resumes, and since the iUi was deleted
        //this thread crashes.

        //Solution:
        //*********
        //1: Remove delete which is below. So that some iUi is available for the other
        //thread to complete. This soln was not followed as this commonUi has to be revamped
        //take care of the same while revamping this component.
        //2: better check for iUi before performing any action on the same. This is
        //now followed as it is simpler solution.

        if ( iUi )
            {
            iUi->HandleIfError( err );
            }
        }

    if ( iClientGroupRemoteNotifyHandler )
        {
        if ( iBrandedResourceFileName )
            {
            iClientGroupConnectionLostHandler->UseResourceFileL( *iBrandedResourceFileName );
            }
        else
            {
            iClientGroupConnectionLostHandler->UseResourceFileL( KNullDesC() );
            }
        TRAPD( err, iClientGroupRemoteNotifyHandler->HandlePresenceEventL( aEvent ) );

        if ( iUi )
            {
            iUi->HandleIfError( err );
            }
        }

    delete iUi;
    iUi = NULL;
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::HandlePresenceEventNotifyError()
// From MIMPSConnUiPresenceEventObserver
// -----------------------------------------------------------------------------
//
void CIMPSConnectionUiAgentImp::HandlePresenceEventNotifyError( CIMPSConnUiPresEventNotifier* /*aNotifier*/,
                                                                TInt aError )
    {
    MCnUiUiFacade* tempUi = NULL;
    // if we don't get facade we just don't show anything
    TRAPD( err, tempUi = CreateUiFacadeL() );
    if ( err )
        {
        return;
        }

    if ( iUi )
        {
        delete iUi;
        }
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {

        if ( iUi )
            {
            TRAP( err, iUi->SwitchResourceFileL( *iBrandedResourceFileName ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }
        }


    if ( iUi )
        {
        iUi->HandleIfError( aError );
        }

    delete iUi;
    iUi = NULL;
    }


// -----------------------------------------------------------------------------
// CIMPSConnectionUiAgentImp::UseResourceFileL()
// -----------------------------------------------------------------------------
//
void CIMPSConnectionUiAgentImp::UseResourceFileL( const TDesC& aNewResourceFile )
    {
    if ( aNewResourceFile.Length() == 0 )
        {
        //empty resource file name ==> use the default resource
        delete iBrandedResourceFileName;
        iBrandedResourceFileName = NULL;
        }
    else
        {
        // use this resourcefile
        HBufC* tempFile = aNewResourceFile.AllocL();
        delete iBrandedResourceFileName;
        iBrandedResourceFileName = tempFile;
        }
    }


//  End of File





