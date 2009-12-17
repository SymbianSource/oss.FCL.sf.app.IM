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
* Description:  Client plugin factory
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <impspresenceconnectionuiconstsng.h>
#include "MCnUiClientPlugin.h"

#include "CCnUiClientPluginPEC.h"
#include "CCnUiClientPluginIM.h"



// ================= GLOBAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// CreateClientPluginL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiClientPlugin* CreateClientPluginL( TIMPSConnectionClient aClientId,
                                                CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    if ( aClientId == EIMPSConnClientPEC )
        {
        return CCnUiClientPluginPEC::NewL( aNWSessionSlotID );
        }

    else if ( aClientId == EIMPSConnClientIM )
        {
        return CCnUiClientPluginIM::NewL( aNWSessionSlotID );
        }

    return NULL;
    }

//  End of File

