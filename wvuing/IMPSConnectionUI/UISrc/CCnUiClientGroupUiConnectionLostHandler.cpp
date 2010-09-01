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
* Description:  Client group connection lost notify handler.
*
*/


// INCLUDE FILES
#include <E32std.h>
#include "CCnUiClientGroupUiConnectionLostHandler.h"
#include "MCnUiUiFacade.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiClientGroupUiConnectionLostHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiClientGroupUiConnectionLostHandler* CCnUiClientGroupUiConnectionLostHandler::NewL(
    TIMPSConnectionClient aClient )
    {
    CCnUiClientGroupUiConnectionLostHandler* self = new ( ELeave )
    CCnUiClientGroupUiConnectionLostHandler( aClient );
    return self;
    }


// Destructor
CCnUiClientGroupUiConnectionLostHandler::~CCnUiClientGroupUiConnectionLostHandler()
    {
    delete iBrandedResourceFileName;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientGroupUiConnectionLostHandler::CCnUiClientGroupUiConnectionLostHandler(
    TIMPSConnectionClient aClient )
        : iClient( aClient )
    {
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiConnectionLostHandler::HandlePresenceEventL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiConnectionLostHandler::HandlePresenceEventL(
    TIMPSPresenceServiceEvent aEvent )
    {
    MCnUiUiFacade* tempUi = CreateUiFacadeL();

    delete iUi;
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {
        iUi->SwitchResourceFileL( *iBrandedResourceFileName );
        }

    if ( aEvent == EIMPSPresenceServiceForceLogOut )
        {
        //Client is force logouted from network server
        iUi->ShowNoteL( ECnUiConnForceLogout );
        }

    delete iUi;
    iUi = NULL;
    }

// -----------------------------------------------------------------------------
// CCnUiClientGroupUiConnectionLostHandler::UseResourceFileL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiConnectionLostHandler::UseResourceFileL( const TDesC& aNewResourceFile )
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

// end of file
