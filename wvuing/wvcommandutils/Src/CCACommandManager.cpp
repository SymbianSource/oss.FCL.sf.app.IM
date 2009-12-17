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
* Description:  Command Manager used for creating the commands
*
*/



// INCLUDE FILES

#include 	"CCAAppSettCommand.h"
#include    "CCACommandManager.h"
#include    "CCALoginCmd.h"
#include    "chatngclient.hrh"
#include    "avkon.hrh"
#include    "uikon.hrh"
#include 	"CCAContactEditCmd.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCACommandManager::NewL
// -----------------------------------------------------------------------------
//
CCACommandManager* CCACommandManager::NewL()
    {
    return new( ELeave ) CCACommandManager();
    }

// -----------------------------------------------------------------------------
// CCACommandManager::CCACommandManager
// Default Constructor
// -----------------------------------------------------------------------------
//
CCACommandManager::CCACommandManager()
    {

    }


// -----------------------------------------------------------------------------
// CCACommandManager::CCACommandManager
// Default Constructor
// -----------------------------------------------------------------------------
//
CCACommandManager::~CCACommandManager()
    {

    }

// -----------------------------------------------------------------------------
// CCACommandManager::NewCommandLC
//
// PLEASE NOTE VERY IMPT : THE CALLER OF THIS METHOD HAS TO DELETE THE COMMAND OBJECT.
// -----------------------------------------------------------------------------
//
MCACommand* CCACommandManager::NewCommandLC( TInt aCommandId,
                                             MCAProcessManager& aProcessManager,
                                             MCAUiLoginCmdCB& aUiLoginCmdCB )
    {
    MCACommand* command = NULL;

    switch ( aCommandId )
        {

        case EChatClientMainViewCmdLogin:
        case EChatClientMainViewCmdLogout:
        case EAknCmdExit:         // fall-through, handled similarily
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EChatClientCmdExit:
            {
            command = CCALoginCmd::NewLC( aProcessManager,
                                          aUiLoginCmdCB,
                                          *iSessionHandlerCmd );
            break;
            }

        default:
            {
            break;
            }
        }

    return command;
    }

// -----------------------------------------------------------------------------
// CCACommandManager::NewCommandLC
// -----------------------------------------------------------------------------

MCASettingsCommand* CCACommandManager::NewCommandLC( TInt aCommandId,
                                                     MCAProcessManager& aProcessManager,
                                                     MCAUiSettingsDialogCmdCB& aUiCmdCB )
    {
    MCASettingsCommand* command = NULL;

    switch ( aCommandId )
        {

        case EChatClientApplicationSettings:
            {
            command = CCAAppSettCommand::NewLC( aProcessManager,
                                                aUiCmdCB );
            break;
            }


        default:
            {
            break;
            }
        }

    return command;
    }


// -----------------------------------------------------------------------------
// CCACommandManager::NewCommandL
// -----------------------------------------------------------------------------

MCASettingsCommand* CCACommandManager::NewCommandL( TInt aCommandId,
                                                    MCAProcessManager& aProcessManager,
                                                    MCAUiSettingsDialogCmdCB& aUiCmdCB )
    {
    MCASettingsCommand* command = NULL;

    switch ( aCommandId )
        {

        case EChatClientApplicationSettings:
            {
            command = CCAAppSettCommand::NewL( aProcessManager,
                                               aUiCmdCB );
            break;
            }


        default:
            {
            break;
            }
        }

    return command;
    }

// -----------------------------------------------------------------------------
// CCACommandManager::Initialize
// Initializes the session handler
// -----------------------------------------------------------------------------
//
void CCACommandManager::Initialize( CCASessionHandlerCmd* aSessionHandlerCmd )
    {
    iSessionHandlerCmd = aSessionHandlerCmd;
    }


//added by saravana

// -----------------------------------------------------------------------------
// CCACommandManager::NewCommandLC
// -----------------------------------------------------------------------------

MCACommand* CCACommandManager::NewCommandLC( TInt aCommandId,
                                             MCAProcessManager& aProcessManager,
                                             MCAUiContactEditCmdCB& aUiCmdCB )
    {
    CCAContactEditCmd* command = NULL;

    switch ( aCommandId )
        {

        case EChatClientDetailsEdit:
            {
            command = CCAContactEditCmd::NewLC( aProcessManager,
                                                aUiCmdCB );
            break;
            }


        default:
            {
            break;
            }
        }

    return command;
    }



// -----------------------------------------------------------------------------
// CCACommandManager::NewCommandLC
// -----------------------------------------------------------------------------

MCACommand* CCACommandManager::NewCommandL( TInt aCommandId,
                                            MCAProcessManager& aProcessManager,
                                            MCAUiContactEditCmdCB& aUiCmdCB )
    {
    CCAContactEditCmd* command = NULL;

    switch ( aCommandId )
        {

        case EChatClientDetailsEdit:
            {
            command = CCAContactEditCmd::NewL( aProcessManager,
                                               aUiCmdCB );
            break;
            }


        default:
            {
            break;
            }
        }

    return command;
    }

// -----------------------------------------------------------------------------
// CCACommandManager::GetSessionHandlerCmd
// -----------------------------------------------------------------------------
//

MCASessionHandlerCmd* CCACommandManager::GetSessionHandlerCmd()
    {
    return iSessionHandlerCmd;
    }


//  End of File
