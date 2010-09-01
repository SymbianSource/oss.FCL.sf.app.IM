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

#ifndef CPENGSEARCHRESULTENTRY_H
#define CPENGSEARCHRESULTENTRY_H

//  INCLUDES
#include <E32Base.h>
#include <Badesca.h>

//FORWARD DECLARATIONS
class CPEngSearchResult2;


/**
 * Search results entry.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSearchResultEntry ) : public CBase
    {

    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngSearchResultEntry object.
     *
     * @return New CPEngSearchResultEntry instance.
     */
    static CPEngSearchResultEntry* NewL();
    static CPEngSearchResultEntry* NewLC();


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngSearchResultEntry();



protected:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngSearchResultEntry();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


    //-----------------------------------------------------------------------
public: /* Interface */


    /**
     * Interface getter.
     *
     * @since 3.0
     * @return Reference to interface.
     */
    CPEngSearchResult2& Interface();


    /**
     *
     *
     * @since 3.0
     *
     */
    CDesC16Array& UserResults();


    /**
     *
     *
     * @since 3.0
     *
     */
    void SetDetails( TInt aFindingsCount,
                     TInt aContinuationIndex,
                     TBool aSearchCompleted );


    /**
     *
     *
     * @since 3.0
     *
     */
    void Reset();




private: //Data


    /**
     * Interface is friend.
     * @since 3.0
     */
    friend class CPEngSearchResult2;        // CSI: 36 #

    //OWN: Exported class interface
    CPEngSearchResult2*   iInterface;

    //OWN: The current findings count
    TInt    iCurrentFindingsCount;

    //OWN: Index to continue
    TInt    iContinuationIndex;

    //OWN: Is search completed
    TBool   iSearchCompleted;

    //OWN: User results
    CDesC16ArraySeg  iUserResults;
    };

#endif      //CPENGSEARCHRESULTENTRY_H



