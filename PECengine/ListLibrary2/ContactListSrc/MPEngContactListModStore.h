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
* Description:  Abstract interface for contact list model store
*
*/

#ifndef __MPENGCONTACTLISTMODSTORE_H__
#define __MPENGCONTACTLISTMODSTORE_H__


//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION
/**
 * Abstract interface for contact list model store
 *
 * @since 3.0
 */
class MPEngContactListModStore
    {
    public:

        /**
         * Gets reference to the store entry size
         * Size can be modified through this reference
         * @since 3.0
         * @return store size reference
         */
        virtual TInt& StoreSizeCount() = 0;


        /**
         * Store Entry to the store.
         *
         * @since 3.0
         */
        virtual void StoreEntryL() = 0;


    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibite deletion through interface.
         */
        virtual ~MPEngContactListModStore() {};

    };
#endif      //  __MPENGCONTACTLISTMODSTORE_H__

//  End of File

