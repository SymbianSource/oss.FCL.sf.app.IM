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
* Description:  Search criteria.
*
*/

#ifndef __CPENGSEARCHCRITERIA2_H
#define __CPENGSEARCHCRITERIA2_H

//  INCLUDES
#include <E32Base.h>



/**
 * Container for one search criteria.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngSearchCriteria2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Two-phased constructor.
         *
         * Initializes the search criteria with:
         *  - KPEngNullSearchElement (from PEngPresenceEngineConsts2.h)
         *  - Empty search string
         *
         * @return New CPEngSearchCriteria2 instance.
         */
        IMPORT_C static CPEngSearchCriteria2* NewL();
        IMPORT_C static CPEngSearchCriteria2* NewLC();


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngSearchCriteria2();



    private:

        CPEngSearchCriteria2();
        void ConstructL();



        //-----------------------------------------------------------------------
    public: /* Criterias */

        /**
         * Set the search request.
         *
         * @since 3.0
         * @param aElement What is searched. Supported search elements
         * are listed in PEngWVPresenceSearch2.h, TPEngWVSearchElement2
         * enumeration.
         * @param aSearchValue The string value that is searched
         * from network. String value meaning depends from given
         * search element value.
         */
        IMPORT_C void SetSearchL( TInt aElement,
                                  const TDesC& aSearchValue );

        /**
         * Gets the search request details
         * as set previously.
         *
         * @since 3.0
         */
        IMPORT_C TInt Element() const;
        IMPORT_C const TDesC& SearchValue() const;




        //-----------------------------------------------------------------------
    private: /* Data */

        //OWN: Search element
        TInt    iElement;

        //OWN: Search value
        HBufC*  iSearchValue;

    };

#endif      // __CPENGSEARCHCRITERIA2_H

//End of file