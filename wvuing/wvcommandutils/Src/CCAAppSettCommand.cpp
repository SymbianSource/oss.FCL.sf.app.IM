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
* Description:  Command to get the Settings Interface for getting/setting tonefilename etc
*
*/


//  INCLUDE FILES

#include <e32std.h>
#include <eikdef.h>
#include <e32base.h>

#include "MCAUiSettingsDialogCmdCB.h"
#include "CCACommandSettingSapExtCmd.h"

#include "MCASettingsCommand.h"
#include "CCAAppSettCommand.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCAAppSettCommand::~CCAAppSettCommand()
    {
    delete iSapExtension;
    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CCAAppSettCommand::CCAAppSettCommand( MCAProcessManager& aProcessManager,
                                      MCAUiSettingsDialogCmdCB& aUiSettingsCB )
        : iProcessManager( aProcessManager ),
        iAppSetting( aUiSettingsCB )
    {

    }



// -----------------------------------------------------------------------------
// CCAAppSettCommand::NewL
// -----------------------------------------------------------------------------
//
CCAAppSettCommand* CCAAppSettCommand::NewL( MCAProcessManager& aProcessManager,
                                            MCAUiSettingsDialogCmdCB& aUiSettingsCB )
    {

    CCAAppSettCommand* self = CCAAppSettCommand::NewLC( aProcessManager,
                                                        aUiSettingsCB );
    CleanupStack::Pop( self );
    return self;

    }


// -----------------------------------------------------------------------------
// CCAAppSettCommand::NewLC
// -----------------------------------------------------------------------------
//
CCAAppSettCommand* CCAAppSettCommand::NewLC( MCAProcessManager& aProcessManager,
                                             MCAUiSettingsDialogCmdCB& aUiSettingsCB )
    {

    CCAAppSettCommand* self = new( ELeave ) CCAAppSettCommand( aProcessManager,
                                                               aUiSettingsCB );
    CleanupStack::PushL( self );
    return self;

    }

// -----------------------------------------------------------------------------
// CCAAppSettCommand::SetObserver
// -----------------------------------------------------------------------------
//
void CCAAppSettCommand::SetObserver( MCACommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CCAAppSettCommand::Completed
// -----------------------------------------------------------------------------
//
void CCAAppSettCommand::CompletedL( TInt aError )
    {
    if ( iCommandObserver )
        {
        iCommandObserver->CommandCompletedL( aError );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppSettCommand::CancelCommand
// -----------------------------------------------------------------------------
//
void CCAAppSettCommand::CancelCommand()
    {

    }

// -----------------------------------------------------------------------------
// CCAAppSettCommand::CancelCommand
// -----------------------------------------------------------------------------
//
void CCAAppSettCommand::ExecuteCommandL()
    {
    MCAUiSettingsDialogCmdCB::TSettingsDialogParams params;
    iAppSetting.GetParameters( params );
    iSapExtension = CCACommandSettingSapExtCmd::NewL(
                        iProcessManager.GetSettingsInterface(),
                        params.iPreserveSessionIdInExit,
                        params.iSap );
    }

// -----------------------------------------------------------------------------
// CCAAppSettCommand::IsAsynchronous
// -----------------------------------------------------------------------------
//
TBool CCAAppSettCommand::IsAsynchronous()
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CCAAppSettCommand::GetAppSettingsSAPExt
// -----------------------------------------------------------------------------
//
MCASettingSapExt* CCAAppSettCommand::GetAppSettingsSAPExt()
    {
    return iSapExtension;
    }

//END FILE




