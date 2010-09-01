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
* Description:  Represents the search data for UI
*
*/


#ifndef MCASEARCHDATA_H
#define MCASEARCHDATA_H

// INCLUDES
#include	"MCAImpsFundClient.h"

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Search data representation for the UI.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCASearchData
    {
    public:

        /**
        * Can we search for more results?
        * @return ETrue if the search cannot be continued and
        *         EFalse if the search can be continued.
        */
        virtual TBool NoMoreResultsAvailable() const = 0;

        /**
        * Returns the type of the search.
        * @return The type.
        */
        virtual TImpsSearchResultType SearchResultType() const = 0;

        /**
        * Returns searched data.
        * @param aIndex specifies the index to the wanted data.
        * @return This is the search data.
        */
        virtual TPtrC SearchData( TInt aIndex ) const = 0;

        /**
        * Returns the current search data count
        * ( I.e. maximum number of available items ).
        * @return Count of data items.
        */
        virtual TInt SearchDataCount() const = 0;

        /**
        * The maximum amount of results available in the server.
        * @return The amount.
        */
        virtual TInt HowManyResultsAvailable() const = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCASearchData() {};
    };

#endif      // MCASEARCHDATA_H

// End of File
