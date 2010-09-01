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
* Description:  PresenceManager private defines.
*
*/

#ifndef PENGPRESENCEMANAGERCONSTS_H
#define PENGPRESENCEMANAGERCONSTS_H

//  INCLUDES
#include <E32Std.h>


// CONSTANTS

/**
 * Completion code for asynchronous operations.
 *
 * @since 3.0
 */
enum TPEngAsyncOpResult
    {
    EPEngAsyncOpCompleted,
    EPEngAsyncOpNotCompleted
    };


// MACROS
#define RETURN_IF_ERROR( aErrorCode )\
        { \
        TInt __err(  aErrorCode ); \
        if ( __err != KErrNone ) \
            { \
            return __err; \
            } \
        } \
     

#endif //PENGPRESENCEMANAGERCONSTS_H


