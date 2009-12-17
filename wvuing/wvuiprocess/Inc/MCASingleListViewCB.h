/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract factory for creating command objects.
*
*/



#ifndef MCASINGLELISTVIEWCB_H
#define MCASINGLELISTVIEWCB_H

//  INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
*  Interface for CallBack Method for singleview,
*  to retreive current item index and set the details of the retrived item.
*  @lib
*  @since 2.6
*/
class MCASingleListViewCB
    {
    public:
        /**
         * Gets the current item index from singlelistview
         * before resetting the array
         * @param aSettingsDialogParams - Retreive by reference from UI Component
         * @since 2.6
         */
        virtual TInt GetCurrentItemIndex( ) = 0;

        /**
        * Sets the item index to onwdata for singlelistview
        * on logout
        * @since 2.6
        */
        virtual void SetCurrentItemIndexL( TBool aFreshLogin /* = EFalse */ ) = 0;

        /**
         * Destructor
         */
        virtual ~MCASingleListViewCB() {};

    };

#endif      // MCASINGLELISTVIEWCB_H
// End of File
