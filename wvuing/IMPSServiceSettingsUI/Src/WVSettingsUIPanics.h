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
* Description:  Declares panics for Service Settings UI
*
*/



#ifndef WVSETTINGSUIPANICS_H
#define WVSETTINGSIPANICS_H

//  INCLUDES

// CONSTANTS
enum TWVSettingsViewPanicReason
    {
    EServersArrayIndexOutOfBounds = -2000,
    EWVSettingsViewPanicStoreValue,
    EWVSettingsListArrayIndexOutOfBounds,
    KInvalidServerData
    };


// FUNCTION PROTOTYPES
/**
 * Service settings UI panic handler.
 * Panics current user thread with
 * reason number from TWVSettingsViewPanicReason.
 * @param aReason The reason of panic.
 */
GLREF_C void Panic( TWVSettingsViewPanicReason aReason );


#endif      // WVSETTINGSIPANICS_H

// End of File
