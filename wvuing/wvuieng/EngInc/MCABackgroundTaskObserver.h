/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface for Group Synchronisation events
*
*/

#ifndef MCABACKGROUNDTASKOBSERVER_H
#define MCABACKGROUNDTASKOBSERVER_H

// INCLUDES

#include <e32std.h>

// CLASS DECLARATION

/**
*  Interface for Group Synchronisation events.
*
*
*  @lib CAEngine.lib
*
*/



class MCABackGroundTaskObserver
    {
    public:


        /**
         * Notify observers of background state change
         * MCABackGroundTaskObserver
         * @param aCompleted is state
         * @return none
         */
        virtual void HandleBackGroundTaskCompleteL( TBool aCompleted ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCABackGroundTaskObserver() {};
    };

#endif      // MCABACKGROUNDTASKOBSERVER_H

// End of File
