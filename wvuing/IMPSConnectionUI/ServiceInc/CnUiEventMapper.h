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

#ifndef __CNUIEVENTMAPPER_H
#define __CNUIEVENTMAPPER_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include <PEngPresenceEngineConsts2.h>



// CLASS DECLARATION
/**
 * CnUiEventMapper.
 *
 * Maps the Connection UI
 * presence events to PEC engine
 * presence events and vice versa.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CnUiEventMapper )
    {
public: //conversion methods

    /**
     * Converts the given presence engine event to
     * Connection UI event enumeration.
     *
     * @since 2.1
     * @param aPecEvent The PEC event enumeration to convert.
     * @param aAppEvent The Connection UI event corresponding
     * the  PEC event. If PEC event is unknown, returns
     * EIMPSPresenceServiceUnknownEvent
     * @return ETrue if event given presence engine event is
     * known one. Else EFalse and EIMPSPresenceServiceUnknownEvent
     * event is returned.
     */
    static TBool ConvertToAppEvent( TPEngNWSessionSlotEvent aPecEvent,
                                    TIMPSPresenceServiceEvent& aAppEvent );


    /**
     * Converts the given Connection UI event to PEC event
     * enumeration.
     *
     * @since 2.1
     * @param aAppEvent The Connection UI app event to convert.
     * @param aPecEvent The PEC event corresponding the
     * Connection UI event.
     * @return ETrue if given Connection UI event is
     * known one. Else EFalse.
     */
    static TBool ConvertToPecEvent( TIMPSPresenceServiceEvent aAppEvent,
                                    TPEngNWSessionSlotEvent& aPecEvent );


    /**
     * Adds all known PEC events to passed
     * array.
     *
     * @since 2.1
     * @param aPecEvents The array where PEC
     * events are added.
     */
    static void AddAllKnownPecEventsL( RArray< TPEngNWSessionSlotEvent >& aPecEvents );


private: //Prohibited constructor / destructor

    CnUiEventMapper();
    ~CnUiEventMapper();

    };

#endif      //__CNUIEVENTMAPPER_H
//  End of File


