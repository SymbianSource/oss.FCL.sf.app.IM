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
* Description:  Panic codes for Storage
 *
*/


#ifndef TCASTORAGEPANICS_H
#define TCASTORAGEPANICS_H

// CONSTANTS
_LIT( KChatStoragePanicString, "WV Storage" );

// DATA TYPES
enum TCAStoragePanics
    {
    ENoSuitableIdentification,  // for some reason contact identification is NULL
    EObserverIsNull,            // Storage manager panics if given observer is NULL
    EStreamLengthIsZero,        // if read stream length is zero during internalizing
    ENoSuitableUserId,          // for some reason contact userID is NULL
    ESortingCorupted,			// Sorting of the contact has failed
    EContactsArrayOutOfSync     // Two arrays of the contact list are out of synchronization
    };

// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Panic handler
// ----------------------------------------------------
//
GLDEF_C void Panic( TCAStoragePanics aPanic );

#endif // TCASTORAGEPANICS_H
