/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CPEngPlgSess
*
*/


//  Include Files
#include <e32base.h>
#include <e32svr.h>
#include <s32mem.h>
#include "CPEngPlgSess.h"
#include "MPEngPlgSrv.h"
#include "PEngPlgSrvCommon.h"

// CONSTRUCTION
// Static constructor
CPEngPlgSess* CPEngPlgSess::NewL( MPEngPlgSrv& aServer )
    {
    CPEngPlgSess* self = new( ELeave ) CPEngPlgSess( aServer );

    return self;
    }

// Destructor (virtual by CBase)
CPEngPlgSess::~CPEngPlgSess()
    {
    }

// Default constructor, protected
CPEngPlgSess::CPEngPlgSess( MPEngPlgSrv& aServer )
        : iPlgServer( aServer )
    {
    }

TBool CPEngPlgSess::DispatchMessageL( const RMessage2 &aMessage )
    {
    // by default, complete the message
    TBool completeMsg( ETrue );

    switch ( aMessage.Function() )
        {
        case EPEngPlgShutdownServer:
            {
            iPlgServer.SetStateL( EPEngPlgSvrStateUnknown );
            iPlgServer.StopServer();
            break;
            }
        case EPEngPlgSetStateOffline:
            {
            iPlgServer.SetStateL( EPEngPlgSvrStateOffline );
            break;
            }
        case EPEngPlgSetStateOnline:
            {
            iPlgServer.SetStateL( EPEngPlgSvrStateOnline );
            break;
            }
        case EPEngPlgPluginCount:
            {
            // complete the message here with the plugin count value
            aMessage.Complete( iPlgServer.PluginCount() );
            completeMsg = EFalse;
            break;
            }
        case EPEngPlgPluginUid:
            {
            // complete the message here with the plugin count value
            aMessage.Complete( iPlgServer.Plugin( aMessage.Int0() ) );
            completeMsg = EFalse;
            break;
            }
        default:
            {
            PanicClient( aMessage, EBadRequest );
            break;
            }
        }

    return completeMsg;
    }

void CPEngPlgSess::PanicClient( const RMessage2& aMessage, const TInt aPanic ) const
    {
    aMessage.Panic( KSessionName, aPanic );
    }

void CPEngPlgSess::ServiceL( const RMessage2 &aMessage )
    {
    if ( DispatchMessageL( aMessage ) )
        {
        aMessage.Complete( KErrNone );
        }
    }

void CPEngPlgSess::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    aMessage.Complete( aError );
    }


// END OF FILE


