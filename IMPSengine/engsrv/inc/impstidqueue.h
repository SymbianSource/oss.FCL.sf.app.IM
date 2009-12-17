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
* Description: Class for storing imps tid queue
* 
*/


#ifndef CImpsTidQueue_H
#define CImpsTidQueue_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  FIFO queue for transaction ids. When the queue at max
*  capacity the oldest item gets deleted.
*/

class CImpsTidQueue : public CBase
    {
    public:

        /**
        * 2-phase Constructor
        */
        static CImpsTidQueue* NewL();

        // Destructor
        ~CImpsTidQueue();

        /**
        * Adds a new orphan message to the end of the queue
        * @param aTid CSP transaction id
        */
        void Add( const TDesC& aTid );

        /**
        * Check if TID queued already
        * @param aTid CSP transaction id
        * @return ETrue if TID found
        */
        TBool TidFound( const TDesC& aTid );

        /**
        * Deletes all entities
        */
        void DeleteAll();

    private:

        /**
        * Constructor
        */
        void ConstructL();

        /**
        * Constructs a new COrpanQueue with given capacity
        * @param aCapacity the capacity of the queue
        */
        CImpsTidQueue( TInt aCapacity );

    private:

        TInt                    iCapacity;
        TInt                    iNext;  // position to write data
        CDesCArrayFlat*         iArray;
    };


#endif
