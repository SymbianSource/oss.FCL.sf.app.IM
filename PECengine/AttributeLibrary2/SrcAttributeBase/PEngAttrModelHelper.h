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
* Description:  Tools for handling presence attribute models in arrays.
*
*/

#ifndef PENGATTRMODELHELPER_H__
#define PENGATTRMODELHELPER_H__


//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class MPEngPresenceAttrModel2;



// CLASS DECLARATION

/**
 * Tools for handling presence attribute models in arrays.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngAttrModelHelper )
    {
public: //Attribute model handling


    /**
     * Adds the given model to destination array.
     * If destination array has already a model with
     * same type and presence id, delets the original one
     * replaces the original one with given one.
     *
     * @since 3.0
     * @param aModels The model array where to add the given model.
     * @param aModels The model to add.
     */
    static void AddOrReplaceModelL( RPointerArray< MPEngPresenceAttrModel2 >& aModels,
                                    MPEngPresenceAttrModel2& aModel );


    /**
     * Moves models from given source array to
     * destination array.
     *
     * @since 3.0
     * @param aFrom The model array where from to move models.
     * @param aTo The destination array to move.
     */
    static void MoveModelsL( RPointerArray< MPEngPresenceAttrModel2 >& aFrom,
                             RPointerArray< MPEngPresenceAttrModel2 >& aTo );


    /**
     * Checks that the given model array doesn't contain
     * duplicate attribute types. If it contains, returns ETrue.
     * Else returns EFalse.
     *
     * @since 3.0
     * @param aModels The model array to check.
     * @retrun ETrue if model array contains duplicates.
     * Else EFalse.
     */
    static TBool HasDuplicateTypes(
        RPointerArray< MPEngPresenceAttrModel2 >& aModels );



private: //Prohibited constructor and destructor

    PEngAttrModelHelper();
    ~PEngAttrModelHelper();
    };

#endif      //  PENGATTRMODELHELPER_H__



//  End of File

