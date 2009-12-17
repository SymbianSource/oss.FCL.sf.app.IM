/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Command for Login/Logout/Exit
*
*/


//  INCLUDE FILES
#include    "CCALoginCmd.h"
#include    "MCALoginPC.h"
#include    "MCAProcessManager.h"
#include    "CCASessionHandlerCmd.h"
#include    "MCAUiLoginCmdCB.h"
#include    "CCAEngine.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCALoginCmd::~CCALoginCmd()
    {

    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CCALoginCmd::CCALoginCmd( MCAProcessManager& aProcessManager,
                          CCASessionHandlerCmd& aSessionHandlerCmd,
                          MCAUiLoginCmdCB& aUiLoginCmdCB )
        : iProcessManager( aProcessManager ),
        iSessionHandlerCmd( aSessionHandlerCmd ),
        iUiLoginCmdCB( aUiLoginCmdCB )
    {

    }


// -----------------------------------------------------------------------------
// CCALoginCmd::NewL
// -----------------------------------------------------------------------------
//
CCALoginCmd* CCALoginCmd::NewL( MCAProcessManager& aProcessManager,
                                MCAUiLoginCmdCB& aUiLoginCmdCB,
                                CCASessionHandlerCmd& aSessionHandlerCmd )
    {
    CCALoginCmd* self = CCALoginCmd::NewLC( aProcessManager,
                                            aUiLoginCmdCB,
                                            aSessionHandlerCmd );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCALoginCmd::NewLC
// -----------------------------------------------------------------------------
//
CCALoginCmd* CCALoginCmd::NewLC( MCAProcessManager& aProcessManager,
                                 MCAUiLoginCmdCB& aUiLoginCmdCB,
                                 CCASessionHandlerCmd& aSessionHandlerCmd )
    {
    CCALoginCmd* self = new( ELeave ) CCALoginCmd( aProcessManager,
                                                   aSessionHandlerCmd,
                                                   aUiLoginCmdCB );
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCALoginCmd::SetObserver
// -----------------------------------------------------------------------------
//
void CCALoginCmd::SetObserver( MCACommandObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CCALoginCmd::CompletedL
// -----------------------------------------------------------------------------
//
void CCALoginCmd::CompletedL( TInt aError )
    {
    if ( iObserver )
        {
        iObserver->CommandCompletedL( aError );
        }
    }

// -----------------------------------------------------------------------------
// CCALoginCmd::CancelCommand
// -----------------------------------------------------------------------------
//
void CCALoginCmd::CancelCommand()
    {

    }

// -----------------------------------------------------------------------------
// CCALoginCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CCALoginCmd::ExecuteCommandL()
    {

    MCAUiLoginCmdCB::TLoginParams params;

    iUiLoginCmdCB.GetParameters( params );

    switch ( params )
        {
        case MCAUiLoginCmdCB::ELogin:
            {
            iSessionHandlerCmd.LoginL( EFalse );
            break;
            }
        case MCAUiLoginCmdCB::ELogout:
            {
            iSessionHandlerCmd.LogoutL();
            break;
            }
        case MCAUiLoginCmdCB::EQueryLogin:
            {
            iSessionHandlerCmd.LoginL( ETrue );
            break;
            }

        case MCAUiLoginCmdCB::EExit:
            {
            iSessionHandlerCmd.DoApplicationExitL();
            break;
            }

        default:
            {
            break;
            }

        }


    }

// -----------------------------------------------------------------------------
// CCALoginCmd::IsAsynchronous
// -----------------------------------------------------------------------------
//
TBool CCALoginCmd::IsAsynchronous()
    {
    return ETrue;
    }





