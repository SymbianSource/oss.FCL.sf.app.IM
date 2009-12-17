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
* Description:  Search results.
*
*/

#ifndef __CPENGSEARCHRESULT2_H
#define __CPENGSEARCHRESULT2_H


//  INCLUDES
#include <E32Base.h>
#include <bamdesca.h>


//FORWARD DECLARATIONS
class CPEngSearchResultEntry;



/**
 * Search results.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngSearchResult2 : public CBase
    {

        //-----------------------------------------------------------------------
    protected: /* Construction */

        /**
         * C++ constructor.
         */
        CPEngSearchResult2( CPEngSearchResultEntry& aImp );


        /**
         * Destructor.
         */
        ~CPEngSearchResult2();




        //-----------------------------------------------------------------------
    public: /* Search result flags */


        /**
         * The number of the current findings, as the
         * network server has found those.
         * (Note the difference to returned results count.)
         *
         * @since 3.0
         * @return The number of the current findings.
         */
        IMPORT_C TInt CurrentFindingsCount() const;


        /**
         * Indicates the particular index from which the
         * next search can start.
         *
         * To continue search further, the client must
         * provide this or modified value to
         * CPEngSearchTransaction2::ContinueSearchFromNetwork().
         * By modifying continuation index, client can affect
         * from which point the search is continued.
         *
         * @since 3.0
         * @return The default continuation index.
         */
        IMPORT_C TInt ContinuationIndex() const;


        /**
         * Completion flag.
         *
         * Indicates whether the client can continue search or not.
         * EFalse if server may provide new results (still searching).
         * ETrue if new results will not be provided.
         *
         * @since 3.0
         * @return Completion flag.
         */
        IMPORT_C TBool SearchCompleted() const;



        //-----------------------------------------------------------------------
    public: /* Search results */


        /**
         * User results.
         *
         * @since 3.0
         * @return User results array.
         * If there isn't any user results, the array is empty.
         */
        IMPORT_C const MDesCArray& UserResults() const;





        //-----------------------------------------------------------------------
    private: /* Data */


        /**
         * Implementation is friend.
         * @since 3.0
         */
        friend class CPEngSearchResultEntry;	// CSI: 36 #

        //REF: Implementation
        CPEngSearchResultEntry&    iImp;

    };

#endif      //__CPENGSEARCHRESULT2_H


//End of file




