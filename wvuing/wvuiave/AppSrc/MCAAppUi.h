/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for AppUi class
*
*/



#ifndef MCAAPPUI_H
#define MCAAPPUI_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MCALayoutChangeObserver;
class MCAResourceChangeObserver;


// CLASS DECLARATION

/**
*  Interface for AppUi class
*
*  @lib chat.exe
*  @since Series 60 3.0
*/
class MCAAppUi
    {
    public: // New functions

        /*
         * Returns a full path to chat bitmap file.
         * @return TDes reference to chat bitmap file.
         */
        virtual TDes& MbmFullPath() = 0;

        /**
         * Adds layout change observer
         * @param aObserver Pointer to observer
         */
        virtual void AddLayoutChangeObserver( MCALayoutChangeObserver* aObserver ) = 0;

        /**
         * Removes layout observer
         * @param aObserver Pointer to removed observer
         */
        virtual void RemoveLayoutChangeObserver( MCALayoutChangeObserver* aObserver ) = 0;

        /**
         * Adds resource observer
         * @param aObserver Pointer to observer
         */
        virtual void AddResourceChangeObserver( MCAResourceChangeObserver* aObserver ) = 0;

        /**
         * Removes resource observer
         * @param aObserver Pointer to removed observer
         */
        virtual void RemoveResourceChangeObserver( MCAResourceChangeObserver* aObserver ) = 0;
    };

#endif      // MCAAPPUI_H

// End of File
