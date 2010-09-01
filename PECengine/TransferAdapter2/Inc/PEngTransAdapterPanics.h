/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Transfer adapter panics
*
*/

#ifndef __PENGTRANSADAPTERPANICS_H__
#define __PENGTRANSADAPTERPANICS_H__

//  INCLUDES
#include    <e32std.h>
#include    <ImpsServices.h>
#include    "CPEngSessionSlotId.h"


// CONSTANTS
/**
* Presence Engine Transfer adapter Library panic category.
*
* @since 3.0
*/
_LIT( KPEngTransferAdapterPanic, "PEngTranAdap" );

/**
* Presence Engine Transfer adapter Library panic reasons.
*
* Following panic reasons are used in
* Transfer adapter Library panics.
*
* @since 3.0
*/
enum TPEngTransferAdapterPanics
    {
    ESesionConsistencyLost                      = 1,
    EOpIdConsistencyLost                        = 2,
    ENoActiveOperations                         = 3,
    EUnknownUnumeration                         = 4,
    EReferenceCountUnderflow                    = 5,
    EIlegalSingletonDeletion                    = 6,
    EAccessHandlerObserversUnregistered         = 7,
    EUnknowPureData                             = 8,
    EListenerAlreadyActive                      = 9,
    ELogInDataNotValid                          = 10

    };


/**
 *  Check Consistency of the Session ids
 *  Panics for Debug version if inconsistent
 *
 *  @since 3.0
 *  @param aSessionId PEC engine session Id
 *  @param aCspId WV Engine session Id
 */
inline void CheckConsistency(
    const CPEngSessionSlotId& aSessionId,
    TImpsCspIdentifier& aCspId )
    {
    // panic if we get other session notification
    __ASSERT_DEBUG(
        ( aCspId.UserId().CompareF( aSessionId.UserId() ) == KErrNone ),
        User::Panic( KPEngTransferAdapterPanic, ESesionConsistencyLost ) );
    __ASSERT_DEBUG(
        ( aCspId.Sap().CompareF( aSessionId.ServiceAddress() ) == KErrNone ),
        User::Panic( KPEngTransferAdapterPanic, ESesionConsistencyLost ) );

    ( void ) aSessionId; // Prevent compilation warning about unused variable
    ( void ) aCspId; // Prevent compilation warning about unused variable
    }

#endif  //__PENGTRANSADAPTERPANICS_H__

//  End of File

