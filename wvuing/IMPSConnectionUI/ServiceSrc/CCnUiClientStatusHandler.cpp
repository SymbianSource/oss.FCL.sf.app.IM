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
* Description:  Concrete client login status handler.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "CCnUiClientStatusHandler.h"

/**
* Channel value for no logged
* in clients. Logged in clients are
* logically orred channel value.
* @since 2.1
*/
const TInt KCnUiNoLoggedInClients = 0;



// ================= GLOBAL FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CreateClientStatusHandlerLC()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiClientStatusHandler* CreateClientStatusHandlerLC()
    {
    return CCnUiClientStatusHandler::NewLC();
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiClientStatusHandler* CCnUiClientStatusHandler::NewLC()
    {
    CCnUiClientStatusHandler* self = new ( ELeave ) CCnUiClientStatusHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiClientStatusHandler::~CCnUiClientStatusHandler()
    {
    delete iLoginStastusChannel;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientStatusHandler::CCnUiClientStatusHandler()
    {
    }


// Symbian OS default constructor can leave.
void CCnUiClientStatusHandler::ConstructL()
    {
    iLoginStastusChannel = CCnUiGroupChannel::NewL( ECnUiGlobalGroup,
                                                    ECnUiClientLoginLogoutStateChannel,
                                                    EFalse ); //permanent data
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::AnyClientLoggedIn()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientStatusHandler::AnyClientLoggedIn()
    {
    TInt statusWord = KCnUiNoLoggedInClients;
    iLoginStastusChannel->Read( statusWord );

    //any client logged in
    if ( statusWord != 0 )
        {
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::ClientLoggedIn()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientStatusHandler::ClientLoggedIn( TIMPSConnectionClient aClient )
    {
    TInt statusWord = KCnUiNoLoggedInClients;
    iLoginStastusChannel->Read( statusWord );

    //Is the asked client logged in
    if ( statusWord & ( TInt ) aClient )
        {
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::SetNoClientsLoggedInL()
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::SetNoClientsLoggedInL()
    {
    iLoginStastusChannel->WriteL( KCnUiNoLoggedInClients );
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::SetClientLoggedInL()
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::SetClientLoggedInL( TIMPSConnectionClient aClient )
    {
    TInt existingClients = KCnUiNoLoggedInClients;
    iLoginStastusChannel->Read( existingClients );

    //this client is among the other ones
    TInt loggedInClients = existingClients | ( TInt ) aClient;
    iLoginStastusChannel->WriteL( loggedInClients );
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::SetClientLoggedOutL()
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::SetClientLoggedOutL( TIMPSConnectionClient aClient )
    {
    TInt existingClients = KCnUiNoLoggedInClients;
    iLoginStastusChannel->Read( existingClients );


    TInt loggedInClients = existingClients & ~( TInt ) aClient;
    iLoginStastusChannel->WriteL( loggedInClients );
    }



// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::NotifyClientStatusChangesL()
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::NotifyClientLoginStatusChangesL(
    MCnUiClientStatusObserver* aObserver,
    TIMPSConnectionClient aClientToNotify )
    {
    __ASSERT_ALWAYS( !iClntStatusObserver, User::Leave( KErrInUse ) );
    __ASSERT_ALWAYS( aObserver, User::Leave( KErrArgument ) );

    //start the notifier
    iLoginStastusChannel->ListenL( this );

    //store observer information
    iClntStatusObserver = aObserver;
    iObservedClient = aClientToNotify;
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::CancelClientStatusNotify()
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::CancelClientStatusNotify()
    {
    iLoginStastusChannel->CancelListen();
    iClntStatusObserver = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiClientStatusHandler::HandleNotifyL()
// from MSharedDataNotifyHandler
// -----------------------------------------------------------------------------
//
void CCnUiClientStatusHandler::HandleChannelMsg( TInt /*aGroupId*/,
                                                 TGCChannelID /*aChannelId*/,
                                                 TInt aRetMsg )
    {
    TBool clientNowLoggedIn = EFalse;
    //Is the asked client logged in
    if ( aRetMsg & ( TInt ) iObservedClient )
        {
        clientNowLoggedIn = ETrue;
        }

    iClntStatusObserver->HandleClientLoginStatusChange( iObservedClient, clientNowLoggedIn );
    }

//  End of File
