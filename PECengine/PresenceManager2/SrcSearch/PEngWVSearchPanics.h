/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class defines some panic functions
*
*/

#ifndef PENGWVSEARCHPANICS_H__
#define PENGWVSEARCHPANICS_H__

//  INCLUDES
#include    <e32std.h>
#include    <ImpsServices.h>
#include    "CPEngSessionSlotId.h"


// CONSTANTS
/**
*
*
* @since 3.0
*/
_LIT( KPEngSearchPanicCategory, "PEngPMSearch" );


/**
 *
 *
 * @since 3.0
 */
enum TPEngSearchPanics
    {
    EPEngSearchElementUnknown = 1,
    EPEngNoSapContainerLoaded = 2,
    EPEngSearchSessionDestructionReqPending = 3,
    EPEngSearchHandlerDestructionReqPending = 4,
    EPEngSearchControlRunStateMismatch = 4
    };



/**
 *
 *
 * @param aReason The reason for panic.
 * @param aLeaveCode The leave code.
 */
GLREF_C void SearchPanicOrLeaveL( TPEngSearchPanics aReason,
                                  TInt aLeaveCode );


/**
 *
 *
 * @param aReason The reason for panic.
 */
GLREF_C void SearchPanic( TPEngSearchPanics aReason );





#endif  //PENGWVSEARCHPANICS_H__

//  End of File

