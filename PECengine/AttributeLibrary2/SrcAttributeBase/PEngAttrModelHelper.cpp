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

// INCLUDE FILES
#include "PEngAttrModelHelper.h"
#include "PresenceDebugPrint.h"

#include <E32Std.h>
#include <MPEngPresenceAttrModel2.h>




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngAttrModelHelper::AddOrReplaceModelL()
// -----------------------------------------------------------------------------
//
void PEngAttrModelHelper::AddOrReplaceModelL(
    RPointerArray< MPEngPresenceAttrModel2 >& aModels,
    MPEngPresenceAttrModel2& aModel )
    {
    const TInt modelCount = aModels.Count();
    for ( TInt ix = 0; ix < modelCount; ix++ )
        {
        MPEngPresenceAttrModel2* model = aModels[ ix ];
        if ( ( model->Type() == aModel.Type() ) &&
             ( model->PresenceID() == aModel.PresenceID() ) )
            {
            //Matching model found ==> replace it
            delete model;
            aModels[ ix ] = &aModel;
            return;
            }
        }

    //no matching model found
    //Add the given one
    aModels.AppendL( &aModel );
    }



// -----------------------------------------------------------------------------
// PEngAttrModelHelper::MoveModelsL()
// -----------------------------------------------------------------------------
//
void PEngAttrModelHelper::MoveModelsL(
    RPointerArray< MPEngPresenceAttrModel2 >& aFrom,
    RPointerArray< MPEngPresenceAttrModel2 >& aTo )

    {
    const TInt modelCount = aFrom.Count();
    for ( TInt ix = 0; ix < modelCount; ix++ )
        {
        MPEngPresenceAttrModel2* model = aFrom[ 0 ];
        aTo.AppendL( model );
        aFrom.Remove( 0 );
        }
    }


// -----------------------------------------------------------------------------
// PEngAttrModelHelper::HasDuplicateTypes()
// -----------------------------------------------------------------------------
//
TBool PEngAttrModelHelper::HasDuplicateTypes(
    RPointerArray< MPEngPresenceAttrModel2 >& aModels )
    {
    //Check against duplicate types
    const TInt modelCount = aModels.Count();
    for ( TInt ix = 0; ix < modelCount - 1; ix++ )
        {
        for ( TInt jj( ix + 1 ); jj < modelCount; jj++ )
            {
            if ( aModels[ ix ]->Type() == aModels[ jj ]->Type() )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }


//  End of File

