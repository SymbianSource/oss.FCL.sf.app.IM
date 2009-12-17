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
* Description:  Interface for search operations
*
*/


#ifndef MCASEARCHINTERFACE_H
#define MCASEARCHINTERFACE_H

#include "MCAImpsFundClient.h"

// FORWARD DECLARATIONS
class   MCASearchData;
class   MCASearchObserverInterface;

// CLASS DECLARATION

/**
 *  Search services interface for UI and Engine.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class MCASearchInterface
    {
    public:

        /**
         * Start the search.
         * @param aPairs defines what is to be searched.
         * @param aSearchLimit defines how many results you want.
         * @param aObserver The observer for this search operation
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        virtual TInt StartSearchL(
            const CSearchPairs& aPairs,
            TInt aSearchLimit,
            MCASearchObserverInterface* aObserver ) = 0;

        /**
         * Stops the current search. Must be called before starting new search.
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        virtual TInt EndSearchL() = 0;

        /**
         * Continues search. This function must only be called when more results
         * are wanted.
         * @param aIndex specifies the next index where to start search.
         * @param aObserver The observer for this search operation
         * @return Error code.
         * Leaves
         *   ERequestedResultsButNotAvailable
         *   ESearchNotSupported
         */
        virtual TInt SearchNextL( TInt aIndex,
                                  MCASearchObserverInterface* aObserver ) = 0;

        /**
         * Get handle to the search data representation.
         * @return Pointer to the search data interface.
         */
        virtual MCASearchData* SearchDataInterface() const = 0;

    protected:

        /**
         * Destructor.
         */
        virtual ~MCASearchInterface() {};
    };

#endif      // MCASEARCHINTERFACE_H

// End of File
