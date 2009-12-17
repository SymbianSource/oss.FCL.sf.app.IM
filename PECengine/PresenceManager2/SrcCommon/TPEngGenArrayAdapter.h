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
* Description:  One value adapter for generic array.
*
*/

#ifndef TPENGGENARRAYADAPTER_H__
#define TPENGGENARRAYADAPTER_H__

//  INCLUDES
#include <E32Std.h>



// CLASS DECLARATION
/**
 * One value adapter for generic array.
 *
 * @since 3.0
 */
template <class T>
class TPEngGenArrayAdapter
    {
    public: //Constructor & destructor

        /**
         * C++ constructor.
         * Takes the value to adapt.
         */
        inline TPEngGenArrayAdapter( const T& aValue );


        /**
         * Destructor.
         */
        inline ~TPEngGenArrayAdapter();


    public: //Generic array


        /**
         * Generic array access.
         *
         * Returns generic array containing the value
         * gived in TPEngGenArrayAdapter construction
         * parameter.
         */
        inline TArray< T > Array() const;



    private: //Generic array functions

        inline static TInt CountFunction( const CBase* aThis );
        inline static const TAny* AtFunction( const CBase* aThis,
                                              TInt aIndex );


    private: //Data

        //REF: Adapted value
        const T& iValue;

    };


// Inline methods
#include "TPEngGenArrayAdapter.inl"

#endif      //TPENGGENARRAYADAPTER_H__
//  End of File



