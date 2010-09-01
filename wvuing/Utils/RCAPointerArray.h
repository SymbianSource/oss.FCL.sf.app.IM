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
* Description:  Wrapper for RPointerArray that destructs it's contents automatically
*
*/


#ifndef RCAPOINTERARRAY_H
#define RCAPOINTERARRAY_H

//  INCLUDES
#include <e32cmn.h>

// CLASS DECLARATION

/**
 *  Wrapper for RPointerArray.
 *	Destructs it's contents automatically.
 *
 *  @lib none
 *  @since Series 60 3.0
 */
template <class T>
class RCAPointerArray : public RPointerArray<T>
    {
    public:		// Constructors and destructor

        /**
         * C++ default constructor.
         */
        inline RCAPointerArray( TBool aOwnsItems = EFalse )
                : iOwnsItems( aOwnsItems ) {};

        /**
         * Destructor.
         */
        inline virtual ~RCAPointerArray()
            {
            iOwnsItems ? RPointerArray<T>::ResetAndDestroy() : RPointerArray<T>::Close();
            }

    public: 	// new function

        /**
         * Set item ownership
         */
        inline void SetItemOwnership( TBool aOwnsItems )
            {
            iOwnsItems = aOwnsItems;
            }

    private:	// data

        TBool iOwnsItems;

    };

#endif      // RCAPOINTERARRAY_H

// End of File
