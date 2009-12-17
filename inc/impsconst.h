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
* Description: 
*     General constant values for WV engine interface and engine 
*
*/


#ifndef IMPSCONST_H
#define IMPSCONST_H

// CONSTANTS
const TInt  KImpsDefaultMessageSlots = 15;

// All content type strings are in KContentTypeTable
// some are defined here to be used separately
_LIT( KImpsAny, "*/*" );
_LIT( KImpsTextPlain, "text/plain" );

_LIT( KImpsCspVersion11, "CSP1.1" );
_LIT( KImpsCspVersion12, "CSP1.2" );


// DATA TYPES
// General boolean value
enum TImpsBoolean
    {
    EImpsUndefined = 0,
    EImpsTrue,
    EImpsFalse
    };


// These panics should only be used in ASSERT_DEBUGs
// (The user should not see these)
enum TImpsPanic
    {
    EImpsBadRequest = 0,
    EImpsNotAssigned = 1,
    EImpsNotConnected = 2,
    EImpsNotifyHandlerMissing = 3,
    EImpsNoActiveScheduler = 4,
    EImpsAlreadyBusy = 5,
    EImpsCorrupted = 6      // internal error in engine
    };


#endif      // IMPSCONST_H
            
// End of File
