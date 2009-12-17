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
* Description:  SId container abstract interface
*
*/



#ifndef __MPENGSIDCONTAINER_H__
#define __MPENGSIDCONTAINER_H__

//  INCLUDES

#include	<e32def.h>

// FORWARD DECLARATIONS
class CPEngObserverContainer;

/**
 *  Storage Id container
 *  Abstract interface of one Store Id
 *
 *  @lib PEngStorMan2.dll
 *  @since 3.0
 */
class MPEngSIdContainer
    {
    public: // New functions

        /**
         * Returns engaged SId
         *
         * @since 3.0
         * @return engaged SId
         */
        virtual const TDesC& SId() const = 0;

        /**
         *	Add Observer container to the container array
         *
         *	@since 3.0
         *	@param aObserver observer to be added
         *	@return KErrNone if operation went fine
         *     otherwise system wide error
         */
        virtual TInt AddObserverContainer(
            const CPEngObserverContainer* aObserver ) = 0;

        /**
         *	Remove Observer from the container from container array
         *
         *	@since 3.0
         *	@param aObserver observer to be removed
         */
        virtual void RemoveObserverContainer(
            const CPEngObserverContainer* aObserver ) = 0;

    };

#endif      // __MPENGSIDCONTAINER_H__

// End of File
