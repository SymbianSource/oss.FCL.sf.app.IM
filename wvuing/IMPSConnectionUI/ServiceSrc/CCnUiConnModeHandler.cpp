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
* Description:  Connection mode handler implementation.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "CCnUiConnModeHandler.h"
#include "CnUiPanics.h"



// ================= GLOBAL FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CreateConnModeHandlerL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiConnModeHandler* CreateConnModeHandlerL()
    {
    return CCnUiConnModeHandler::NewL();
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiConnModeHandler* CCnUiConnModeHandler::NewL()
    {
    CCnUiConnModeHandler* self = new ( ELeave ) CCnUiConnModeHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCnUiConnModeHandler::~CCnUiConnModeHandler()
    {
    //Cancel any outstanding request
    CancelRegisteringAsSSClient();
    CancelConnectionModeNotify();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiConnModeHandler::CCnUiConnModeHandler()
    {
    }


// Symbian OS default constructor can leave.
void CCnUiConnModeHandler::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::SendUserSelectionL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::SendUserSelectionL( TIMPSUserConnectionSelection aUserSelection,
                                               TIMPSConnectionClient aClient )
    {
    TIMPSConnectionModeEvent event = EIMPSCMEUnknown;
    switch ( aUserSelection )
        {
        case EUserLevelLogin:
            {
            event = EIMPSCMEUserLevelLogin;
            break;
            }

        case EUserLevelLogout:
            {
            event = EIMPSCMEUserLevelLogout;
            break;
            }

        case EUserLevelAAConnectionStart:
            {
            event = EIMPSCMEUserLevelAAConnectionStart;
            break;
            }

        default:
            {
            User::Leave( KErrArgument );
            break;
            }
        }

    CCnUiGroupChannel* loginLogoutChannel =
        CCnUiGroupChannel::NewLC( aClient,
                                  ECnUiLoginLogoutEventChannel );
    loginLogoutChannel->WriteL( event );
    CleanupStack::PopAndDestroy( loginLogoutChannel ); //loginLogoutChannel
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::RegisterAsSignificantSchedulingClientL()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnModeHandler::RegisterAsSignificantSchedulingClientL( TIMPSConnectionClient aClient )
    {
    if ( iSSReqistration )
        {
        return KErrInUse;
        }

    iSSReqistration = CCnUiGroupChannel::NewL( aClient,
                                               ECnUiSSClientReqistrationChannel );
    iSSReqistration->SignalL();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::CancelRegisteringAsSSClient()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::CancelRegisteringAsSSClient()
    {
    if ( iSSReqistration )
        {
        iSSReqistration->CancelSignal();
        delete iSSReqistration;
        iSSReqistration = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::ConnectionModeL()
// -----------------------------------------------------------------------------
//
TIMPSConnectionModeEvent CCnUiConnModeHandler::ConnectionModeL(
    TIMPSConnectionClient aClient,
    TIMPSConnectionModeEvent aModeEventCateqory )
    {
    TInt cateqorizedEvent = EIMPSCMEUnknown;

    if ( aModeEventCateqory == EIMPSCMEUserLevelSelectionEventGroup )
        {
        //client wants the login logout event value
        //retrieve value from channel & verify its range

        CCnUiGroupChannel* loginLogoutChannel =
            CCnUiGroupChannel::NewLC( aClient,
                                      ECnUiLoginLogoutEventChannel );
        TInt lastEventValue = KErrNotFound;
        loginLogoutChannel->Read( lastEventValue );
        CleanupStack::PopAndDestroy( loginLogoutChannel ); //loginLogoutChannel

        if ( LoginLogoutModeCategoryEvent( lastEventValue ) )
            {
            cateqorizedEvent = lastEventValue;
            }
        }


    else if ( aModeEventCateqory == EIMPSCMESSClientEventGroup )
        {
        //client wants the SS Client Start/Stop event
        //retrieve the client registration count channel

        CCnUiGroupChannel* reqistrationChannel =
            CCnUiGroupChannel::NewLC( aClient,
                                      ECnUiSSClientReqistrationChannel );
        TInt clientReqistrationCount = KErrNotFound;
        reqistrationChannel->Read( clientReqistrationCount );
        CleanupStack::PopAndDestroy( reqistrationChannel ); //reqistrationChannel

        if ( clientReqistrationCount == 0 ) //0 client currently registered ==> last stopped
            {
            cateqorizedEvent = EIMPSCMELastSSClientStop;
            }

        else if ( clientReqistrationCount > 0 ) //more than 0 currently registered clients
            {
            cateqorizedEvent = EIMPSCMEFirstSSClientStart;
            }
        }

    return static_cast<TIMPSConnectionModeEvent> ( cateqorizedEvent );
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::NotifyConnectionModeChangesL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::NotifyConnectionModeChangesL( MCnUiConnModeObserver* aObserver,
                                                         TIMPSConnectionClient aClientToNotify )
    {
    __ASSERT_ALWAYS( !iConnModeObserver, User::Leave( KErrInUse ) );
    __ASSERT_ALWAYS( aObserver, User::Leave( KErrArgument ) );

    TRAPD( err, DoStartNotifyL( aClientToNotify ) );
    if ( err != KErrNone )
        {
        CancelConnectionModeNotify();
        User::Leave( err );
        }

    //store observer information
    iConnModeObserver = aObserver;
    iObserverClient = aClientToNotify;
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::CancelConnectionModeNotify()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::CancelConnectionModeNotify()
    {
    delete iLoginLogoutEventChannelListener;
    delete iSSReqistrationChannelListener;

    iLoginLogoutEventChannelListener = NULL;
    iSSReqistrationChannelListener = NULL;

    iConnModeObserver = NULL;
    iSSClientCountPrev = KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::HandleNotifyL()
// from MCnUiGroupChannelListener
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::HandleChannelMsg( TInt aGroupId,
                                             TGCChannelID aChannelId,
                                             TInt aChannelMsg )
    {
    if ( iObserverClient != aGroupId )
        {
        return;
        }

    if ( aChannelId == ECnUiLoginLogoutEventChannel )
        {
        //login/logout related event
        TIMPSConnectionModeEvent event = static_cast<TIMPSConnectionModeEvent>( aChannelMsg );
        if ( LoginLogoutModeCategoryEvent( event ) )
            {
            iConnModeObserver->HandleConnModeChange( iObserverClient, event ) ;
            }
        }

    else if ( aChannelId == ECnUiSSClientReqistrationChannel )
        {
        //SSClient start/stop event - actually a signal count

        //If client count increased from zero to 1 ==> first client started
        if ( ( iSSClientCountPrev == 0 ) && ( aChannelMsg == 1 ) )
            {
            iConnModeObserver->HandleConnModeChange( iObserverClient, EIMPSCMEFirstSSClientStart );
            iSSClientCountPrev = aChannelMsg;
            }

        //If client count decreased to zero ==> last client stopped
        else if ( ( iSSClientCountPrev > 0 ) && ( aChannelMsg == 0 ) )
            {
            iConnModeObserver->HandleConnModeChange( iObserverClient, EIMPSCMELastSSClientStop );
            iSSClientCountPrev = aChannelMsg;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::DoStartNotifyL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeHandler::DoStartNotifyL( TIMPSConnectionClient aClientToNotify )
    {
    //start the channel notifier for login/logout related events
    iLoginLogoutEventChannelListener = CCnUiGroupChannel::NewL( aClientToNotify,
                                                                ECnUiLoginLogoutEventChannel );
    iLoginLogoutEventChannelListener->ListenL( this );

    //start the channel notifier for client start/stops
    iSSReqistrationChannelListener = CCnUiGroupChannel::NewL( aClientToNotify,
                                                              ECnUiSSClientReqistrationChannel );
    iSSReqistrationChannelListener->ListenL( this );


    TInt clientReqistrationCount = KErrNotFound;
    iSSReqistrationChannelListener->Read( clientReqistrationCount );

    //if there is a positive registration count, use it.
    //Else default pre count to 0
    if ( clientReqistrationCount >= 0 )
        {
        iSSClientCountPrev = clientReqistrationCount;
        }
    else
        {
        iSSClientCountPrev = 0;
        }
    }



// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::LoginLogoutModeCategoryEvent()
// -----------------------------------------------------------------------------
//
TBool CCnUiConnModeHandler::LoginLogoutModeCategoryEvent( TInt aEvent )
    {
    switch ( aEvent )
        {
            //FLOW TROUGH
        case EIMPSCMEUserLevelLogout:
        case EIMPSCMEUserLevelLogin:
        case EIMPSCMEUserLevelAAConnectionStart:
            {
            //known events
            return ETrue;
            // no break needed because of return
            }

        default:
            {
            //unknown event
            break;
            }
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeHandler::SSClientStartStopEvent()
// -----------------------------------------------------------------------------
//
TBool CCnUiConnModeHandler::SSClientStartStopCategoryEvent( TInt aEvent )
    {
    switch ( aEvent )
        {
            //FLOW TROUGH
        case EIMPSCMEFirstSSClientStart:
        case EIMPSCMELastSSClientStop:
            {
            //known events
            return ETrue;
            // no break needed because of return
            }

        default:
            {
            //unknown event for this category
            break;
            }
        }

    return EFalse;
    }



//  End of File

