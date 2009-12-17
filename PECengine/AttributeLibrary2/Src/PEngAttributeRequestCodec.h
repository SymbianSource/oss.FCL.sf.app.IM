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
* Description:  Codec to code attribute requests.
*
*/

#ifndef PENGATTRIBUTEREQUESTCODEC_H
#define PENGATTRIBUTEREQUESTCODEC_H


//  INCLUDES
#include "MPEngPresenceAttrModel2.h"
#include "MPEngPresenceAttrManager.h"


#include <E32Std.h>



// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;



// CLASS DECLARATION


/**
 * Codec to code attribute requests.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngAttributeRequestCodec )
    {
public: // New encode / decode functions



    /**
     * Packs a attribute model array to buffer.
     *
     * @since 3.0
     * @param aModels Models to pack.
     * @return Heap buffer containing models data.
     * Buffer ownership is returned to caller.
     */
    static HBufC16* PackModelArrayL( const RPointerArray<MPEngPresenceAttrModel2>& aModels );


    /**
     * Extracts models from package.
     *
     * @since 3.0
     * @param aModelPkg Model package to extract.
     * @param aInstanceOptions Instance options
     * for instantiated attribute models.
     * @param aModels On the return contains extracted attribute models.
     * Models are appended to array.
     * @param  aAttributeManager The attribute manger to
     * use in attribute model instantiation.
     */
    static void UnpackModelArrayL( const TDesC16& aModelPkg,
                                   TInt aInstanceOptions,
                                   RPointerArray<MPEngPresenceAttrModel2>& aModels,
                                   MPEngPresenceAttrManager& aAttributeManager );


    /**
     * Packs a attribute fetch request.
     *
     * @since 3.0
     * @param aPresenceIDs Presence ID's to pack.
     * @param aTypes Attribute types to pack.
     * @return Heap buffer containing fetch request data.
     * Buffer ownership is returned to caller.
     */
    static HBufC16* PackFetchRequestL( const MDesCArray& aPresenceIDs,
                                       const TArray<TUint32>& aTypes );


    /**
     * Extracts a attribute fetch request from package.
     *
     * @since 3.0
     * @param aFetchRequestPkg Attribute fetch request package to extract.
     * @param aPresenceIDs On the return contains extracted Presence IDs.
     * Presence IDs are appended to the end of array.
     * @param aTypes On the return contains extracted attribute type IDs.
     * Attribute types are appended to the end of array.
     */
    static void UnpackFetchRequestL( const TDesC16& aFetchRequestPkg,
                                     CDesCArray& aPresenceIDs,
                                     RArray<TUint32>& aTypes );


    /**
     * Copies one attribute model data from source model
     * to destination model.
     *
     * @since 3.0
     * @param aSource Source model copy from.
     * @param aDest The destination model to copy to.
     */
    static void CopyModelDataL( const MPEngPresenceAttrModel2& aSource,
                                MPEngPresenceAttrModel2& aDest );





private: //Helpers
    static void WritePresenceIdL( RWriteStream& aWStream, const TDesC& aPresenceId );
    static HBufC* ReadPresenceIdLC( RReadStream& aRStream );


private: //protected constructor & destructor

    PEngAttributeRequestCodec();
    ~PEngAttributeRequestCodec();
    };

#endif      //PENGATTRIBUTEREQUESTCODEC_H


// End of File
