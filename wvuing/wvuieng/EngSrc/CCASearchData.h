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
* Description:  Represents the searched data
*
*/


#ifndef CCASEARCHDATA_H
#define CCASEARCHDATA_H

//  INCLUDES
#include    "MCASearchData.h"

#include    <e32base.h>
#include    <badesca.h>

// CLASS DECLARATION

/**
*  This class represents the searched data.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCASearchData : public CBase,
            public MCASearchData
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CCASearchData* NewL();

        /**
        * Destructor.
        */
        virtual ~CCASearchData();

    public: // Functions derived from MCASearchData

        /**
        * From MCASearchData.
        * @see MCASearchData::NoMoreResultsAvailable.
        */
        IMPORT_C TBool NoMoreResultsAvailable() const;

        /**
        * From MCASearchData.
        * @see MCASearchData::SearchResultType.
        */
        IMPORT_C TImpsSearchResultType SearchResultType() const;

        /**
        * From MCASearchData.
        * @see MCASearchData::SearchData.
        */
        IMPORT_C TPtrC SearchData( TInt aIndex ) const;

        /**
        * From MCASearchData.
        * @see MCASearchData::SearchDataCount.
        */
        IMPORT_C TInt SearchDataCount() const;

        /**
        * From MCASearchData.
        * @see MCASearchData::HowManyResultsAvailable.
        */
        IMPORT_C TInt HowManyResultsAvailable() const;


    public: // New functions

        /**
        * Needed in search continue. This is the next index to the server.
        * @return Next search index.
        */
        TInt NextSearchIndex() const;

        /**
        * Updates the searched data.
        * @param aIndex specifies the next search index.
        * @param aCompleted ETrue if no more results is available and
        *        EFalse if available.
        * @param aType is the type of the results
        * @param aResults are the results.
        * @param aSearchLimit is the amount of data observer wants.
        */
        void UpdateDataL( TInt                    aIndex,
                          TBool                   aCompleted,
                          TImpsSearchResultType   aType,
                          MDesCArray*             aResults,
                          TInt                    aSearchLimit );

        /**
        * Empties all the search information.
        * Used in stopping search.
        */
        void ClearResults();

        /**
        * Used search next. No need to initiate web transactions if
        * we already have the data.
        * @param aNextIndex is the next place to search.
        * @return ETrue if we have and EFalse if we do not have.
        */
        TBool WeHaveData( TInt aNextIndex ) const;

        /**
        * Updates the maximum available results.
        * @param aMaxResults is the new available results.
        */
        void UpdateMaximumPossibleResults( TInt aMaxResults );

    private:

        /**
        * C++ default constructor.
        */
        CCASearchData();


        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private:    // Data

        // Is the search completed or is there more data
        TBool iCompleted;

        // Owns: Search result array
        CDesCArrayFlat* iResults;

        // Index from the next search can start in the server side
        TInt iServerNextIndex;

        //result type of the search
        TImpsSearchResultType iSearchResultType;

        // The amount of results that subsequent searches can retrieve.
        TInt iMaximumResultsAvailable;
    };

#endif      // CCASEARCHDATA_H

// End of File
