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
* Description:  Serialization tools for SAP Settings Store.
*
*/

#ifndef __IMPSSAPSERIALIZER_H
#define __IMPSSAPSERIALIZER_H

//  INCLUDES
#include <s32strm.h>
#include <s32buf.h>

// CLASS DECLARATION
/**
 * Buffer etc. serialization tools.
 */
NONSHARABLE_CLASS( IMPSSAPSerializer )
    {
public:  // New methods

    /**
     * Externalizes the given buffer to given write stream.
     * Preserves NULL pointers.
     *
     * @since 3.0
     * @param aBuffer Buffer to stream. Buffer can be NULL.
     * @param aStream Destination stream.
     */
    static void ExternalizeBufferL( const HBufC* aBuffer, RWriteStream& aStream );


    /**
     * Internalizes buffer from stream.
     *
     * @since 3.0
     * @param aStream The stream to read to.
     * @return Internalized buffer. If externalized buffer was
     * NULL or zero length, NULL pointer is returned.
     */
    static HBufC* InternalizeBufferL( RReadStream& aStream );

    /**
     * Returns the serialized size of passed buffer.
     * (As space is needed by the ExternalizeBufferL(). )
     *
     * @since 3.0
     * @param aBuffer The buffer which stream consumption
     * to get.
     * @return The serialized size. Returned size is
     * rounded up approximation.
     */
    static TInt BufferSizeInBytes( const HBufC* aBuffer );


private:  // Prohibited constructor & destructor

    IMPSSAPSerializer();
    ~IMPSSAPSerializer();

    };

#endif      // __IMPSSAPSERIALIZER_H

//  End of File
