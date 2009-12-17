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
* Description:  Que for orphan presence notifications.
*
*/

#ifndef __CPENGORPHANNOTIFICATIONQUE_H
#define __CPENGORPHANNOTIFICATIONQUE_H


//  INCLUDES
#include <E32Base.h>


// CLASS DECLARATION
/**
 * Size limited que for orphan presence notifications.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngOrphanNotificationQue ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @param aQueSize Size for the que.
     */
    static CPEngOrphanNotificationQue* NewL( TInt aQueSize );


    /**
     * Destructor.
     */
    ~CPEngOrphanNotificationQue();

private:

    /**
     * C++ default constructor.
     */
    CPEngOrphanNotificationQue( TInt aQueSize );


public: // New functions


    /**
     * Adds a element to que.
     * If the que is full, drops the oldes element out from que.
     *
     * @since 3.0
     * @param aPresenceBlock The presence block to add to que.
     */
    void AddToQueL( const TDesC8& aPresenceBlock );


    /**
     * Sets the current element cursor to the first
     * (oldest) element in the que and returns it.
     * If que is empty, returns NULL.
     *
     * @since 3.0
     * @param First element from que, or NULL if the que is empty.
     */
    const TDesC8* GetFirst();


    /**
     * Deletes the element pointed by the current element cursor.
     * Sets the current element cursor to point to next
     * element following the deleted one and also returns it.
     * If there isn't following elements, returns NULL.
     *
     * @since 3.0
     * @param Next element from que, or NULL if the que is empty.
     */
    const TDesC8* DeleteCurrentAndGetNext();


    /**
     * Sets the current element cursor to point to next element
     * and also returns it. If there isn't anymore elements,
     * returns NULL.
     *
     * @since 3.0
     * @param Next element from que, or NULL if the que is empty.
     */
    const TDesC8* GetNext();




private: // New helpers

    /**
     * Checks is there element in the que at given index.
     *
     * @since 3.0
     * @param aIndex The index to verify.
     * @return ETrue if index is valid. Else EFalse.
     */
    TBool DoIndexValid( TInt aIndex ) const;


    /**
     * Returns the current element or NULL if
     * current index is out of bounds.
     *
     * @since 3.0
     * @return Current element or NULL.
     */
    const TDesC8* DoGetCurrent() const;


private: // Data

    //OWN: Max que size
    const TInt iMaxQueSize;

    //OWN: Current index
    TInt iIndex;

    //OWN: The que
    RPointerArray< HBufC8 > iQue;
    };


#endif      // __CPENGORPHANNOTIFICATIONQUE_H

// End of File
