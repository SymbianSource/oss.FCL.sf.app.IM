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
* Description:  Presence Serve Panics
*
*/


#ifndef __PENGTRANSACTIONSERVERPANICS_H__
#define __PENGTRANSACTIONSERVERPANICS_H__

//  INCLUDES
#include <e32std.h>

// DATA TYPES
enum TPEngTransServerPanicReason
    {
    EReservedProcessAccessCountCorrupted = 1
    };


// FUNCTION PROTOTYPES
/**
 * Transaction Server panic handler.
 * Panics current user thread with category "PEng TransServer" and
 * reason number from TPEngTransServerPanicReason.
 * @param aReason The reason of panic.
 */
GLREF_C inline void Panic( TPEngTransServerPanicReason aReason )
    {
    _LIT( KTransServer , "PEng TransServer" );
    User::Panic( KTransServer, aReason );
    }

#endif  //__PENGTRANSACTIONSERVERPANICS_H__


//  End of File

