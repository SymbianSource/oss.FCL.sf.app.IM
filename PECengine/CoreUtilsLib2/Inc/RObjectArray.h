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
* Description:  Object Pointer array.
*
*/

#ifndef __ROBJECTARRAY_H
#define __ROBJECTARRAY_H

//  INCLUDES
#include  <E32Base.h>


/**
 * Object Pointer array.
 *
 * Array members are owned by the array.
 * CleanupClosePushL() can be used to
 * push array to CleanupStack and
 * destroy objects in case of leave.
 *
 * @since 3.0
 */
template<class T>
class RObjectArray : public RPointerArray<T>
    {
    public: //Overwritten functions


        /**
         * Overloaded close function.
         *
         * Array members are owned by the array,
         * and calling close thus deletes
         * the holded objects.
         *
         * @since 3.0
         */
        inline void Close()
            {
            RPointerArray<T>::ResetAndDestroy();
            }
    };

#endif      // __ROBJECTARRAY_H

// End of File

