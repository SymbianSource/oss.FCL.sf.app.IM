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
* Description:  Codec to code attribute type and presence id.
*
*/

#ifndef __PENGATTRIBUTESTOREIDCODEC_H
#define __PENGATTRIBUTESTOREIDCODEC_H


//  INCLUDES
#include <E32Std.h>


// CLASS DECLARATION

/**
 * Codec to code attribute type and presence id
 * as one store id descriptor.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngAttributeStoreIDCodec )
    {
public: // New encode / decode functions


    /**
     * Generates a attribute store id from given
     * attribute type and presence id.
     *
     * @since 3.0
     * @param aAttributeType The type of the attribute
     * @param aPresenceID The presenceID of the attribute
     * @return Attribute StoreId.
     */
    static HBufC* GenerateStoreIdL( TUint32 aAttributeType,
                                    const TDesC& aPresenceId );




    /**
     * Resolves attribute type and owner presence id
     * from store id.
     *
     * @since 3.0
     * @param aStoreId The store id to resolve.
     * @param aAttributeType On succesful return contains resolved attribute type.
     * @param aPresenceId On successful return contains resolved presence id.
     * Returned aPresenceId is valid only lifetime of given source store id.
     * @return KErrNone if succesful. KErrCorrupt if given aStoreId
     * isn't valid attribute store id.
     */
    static TInt ResolveStoreId( const TDesC& aStoreId,
                                TUint32& aAttributeType,
                                TPtrC& aPresenceId );



public: // Match functions

    /**
     * Checks is there given attribute type present in given store id.
     *
     * @since 3.0
     * @param aStoreId Store id to check.
     * @param aAttributeType Attribute type to find.
     */
    static TBool AttributeTypePresentInStoreId( const TDesC& aStoreId,
                                                TUint32 aAttributeType );


    /**
     * Checks is there given presence id present in given store id.
     *
     * @since 3.0
     * @param aStoreId Store id to check.
     * @param aPresenceId Presence id to find.
     */
    static TBool PresenceIdPresentInStoreId( const TDesC& aStoreId,
                                             const TDesC& aPresenceId );



private: //protected constructor & destructor

    PEngAttributeStoreIDCodec();
    ~PEngAttributeStoreIDCodec();
    };

#endif      //__PENGATTRIBUTESTOREIDCODEC_H


// End of File
