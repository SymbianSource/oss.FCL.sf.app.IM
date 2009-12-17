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
* Description:  Connection UI event mapper
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <PEngPresenceEngineConsts2.h>
#include "CnUiEventMapper.h"



/**
 * Conversion struture to map
 * PEC engine & Connection UI events.
 */
struct SEventConversionItem
    {
    TPEngNWSessionSlotEvent         iPecEvent;
    TIMPSPresenceServiceEvent       iAppEvent;
    };


/**
 * Presence event conversion table.
 * NOTE!! The Connection UI unknown event
 * (EIMPSPresenceServiceUnknownEvent) may not
 * be used here. It is ment purely for unknown
 * events coming from PEC engine side.
 */

LOCAL_D SEventConversionItem const KEventEnums[] =
    {

        { EPEngEventAppNWPresenceSessionClosed,     EIMPSPresenceServiceOffline       },
    { EPEngEventAppNWPresenceSessionOpened,     EIMPSPresenceServiceOnline        },
    { EPEngEventNWSessionSlotRemoved,           EIMPSPresenceServiceSlotRemoved   },
    { EPEngEventNWSessionSlotChanged,           EIMPSPresenceServiceSlotChanged   },
    { EPEngEventNWSessionClosedByServer,        EIMPSPresenceServiceForceLogOut   }

    };


/**
 * Count of conversion items.
 */
const TInt KEventEnumsCount = sizeof ( KEventEnums ) / sizeof( SEventConversionItem );



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CnUiEventMapper::ConvertToAppEvent()
// -----------------------------------------------------------------------------
//
TBool CnUiEventMapper::ConvertToAppEvent( TPEngNWSessionSlotEvent aPecEvent,
                                          TIMPSPresenceServiceEvent& aAppEvent )
    {
    //it is possible that some PEC engine release  introduces new events
    //that do not have a matching pair in application level events
    //==> those are defaulted to EIMPSPresenceServiceUnknownEvent

    for ( TInt ii( 0 ); ii < KEventEnumsCount; ii++ )
        {
        if ( KEventEnums[ ii ].iPecEvent == aPecEvent )
            {
            //matching event enum found, use it
            aAppEvent = KEventEnums[ ii ].iAppEvent;
            return ETrue;
            }
        }

    aAppEvent = EIMPSPresenceServiceUnknownEvent;

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CnUiEventMapper::ConvertToPecEvent()
// -----------------------------------------------------------------------------
//
TBool CnUiEventMapper::ConvertToPecEvent( TIMPSPresenceServiceEvent aAppEvent,
                                          TPEngNWSessionSlotEvent& aPecEvent )
    {

    for ( TInt ii( 0 ); ii < KEventEnumsCount; ii++ )
        {
        if ( KEventEnums[ ii ].iAppEvent == aAppEvent )
            {
            //matching enum found, use it
            aPecEvent = KEventEnums[ ii ].iPecEvent;
            return ETrue;
            }
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CnUiEventMapper::AddAllKnownPecEventsL()
// -----------------------------------------------------------------------------
//
void CnUiEventMapper::AddAllKnownPecEventsL( RArray< TPEngNWSessionSlotEvent >& aPecEvents )
    {

    for ( TInt ii( 0 ); ii < KEventEnumsCount; ii++ )
        {
        User::LeaveIfError( aPecEvents.Append( KEventEnums[ ii ].iPecEvent ) );
        }

    }


//  End of File

