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
* Description:  Presence Server class
*
*/


#ifndef __CPENGACTIVESCHEDULER_H__
#define __CPENGACTIVESCHEDULER_H__

// INCLUDES
#include    <e32std.h>


// CLASS DECLARATION
/**
 * Active Scheduler
 *
 * Own derived CActiveScheduler which is overwritting
 * Error() function so it does not panic there as default implementation
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngActiveScheduler ):
        public CActiveScheduler
    {
public:
    CPEngActiveScheduler() : CActiveScheduler()
        {
        };

    ~CPEngActiveScheduler()
        {
        };

    void Error( TInt /*anError*/ ) const
        {
        };
    };


#endif      // __CPENGACTIVESCHEDULER_H__

