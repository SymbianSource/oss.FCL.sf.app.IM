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
* Description:  Contact sorter
*
*/


#ifndef CCACONTACTSORTER_H
#define CCACONTACTSORTER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
#include "MCAStoredContact.h"

// CLASS DECLARATION

/**
*  Friends array sorter
*  Changes used sorting algorithm based on setting item value.
*  It also listen to changes
*  in setting item value. This class is used by CCAFriendsArray.
*  Offers also an API to compare two contacts.
*
*  @lib castorage.lib
*  @since 3.0
*/
NONSHARABLE_CLASS( CCAContactSorter ) : public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCAContactSorter* NewL();

    /**
    * Destructor.
    */
    virtual ~CCAContactSorter();

public: //New Methods

    /**
    * Sorts given array.
    * @param aList Reference to list which is sorted.
    */
    void Sort( RPointerArray< MCAStoredContact >& aList );

    /**
    * Returns which given contact is first in list.
    * @param aFirst Reference to first contact.
    * @param aSecond Reference to second contact.
    * @return Integer, -1 if aFirst is greater, 0 if equal,
    *                   1 if aSecond is greater.
    */
    TInt Compare( const MCAStoredContact& aFirst,
                  const MCAStoredContact& aSecond ) const;

    /**
     *
     */
    void SetSortAlgorithm( MCAStoredContact::TSortAlgorithm aAlgorithm );

    TLinearOrder< MCAStoredContact >& InsertOrder();

private:
    /**
    * C++ default constructor.
    */
    CCAContactSorter();

private:    // Data

    // This contains a pointer to a function used in sorting and comparing.
    TLinearOrder< MCAStoredContact > iAlgorithm;
    };

#endif // CCACONTACTSORTER_H
