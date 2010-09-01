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
* Description:  Object handler implementation.
*
*/

#ifndef __IMPSSAPOBJECTHANDLER_H
#define __IMPSSAPOBJECTHANDLER_H

//  INCLUDES
#include "MIMPSSapObject.h"
#include <s32strm.h>




// CLASS DECLARATION
/**
 * Object handler implementation
 * for generic MIMPSSapObject derived objects.
 *
 * Handles versioned objects and their data.
 * Can internalize data produced with newer
 * versions of software having more fields
 * than current implementation.
 *
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( IMPSSAPObjectHandler )
    {

public:  // New methods

    /**
     * Gets the data size.
     *
     * Calculates the data size needed to externalize
     * given object using IMPSSAPObjectHandler::ExternalizeL()
     * service.
     *
     * @since 3.0
     * @param aObj  The object which data size to get.
     * @return The data size needed to externalize the
     * given object.
     */
    static TInt DataSize( const MIMPSSapObject& aObj );


    /**
     * Externalizes object to given write stream.
     *
     * Externalizes object to given write stream.
     * Stores additional version and control information
     * inside the stream so internalization can be
     * done later e.g with higher versioned component
     * having new data fields. This implementation is
     * tested with following stream types:
     * - file
     * - buffer
     *
     * NOTE! Versioning is based on usage of:
     *  - MIMPSSapObject::ObjectVersion()
     *  - MIMPSSapObject::ExternalizeVersionDataL()
     *  - MIMPSSapObject::InternalizeVersionDataL()
     *
     * @since 3.0
     * @param aObj The object to externalize.
     * @aStream aStream The stream to write the data.
     */
    static void ExternalizeL( const MIMPSSapObject& aObj,
                              RWriteStream& aStream );


    /**
     * Internalizes object from given read stream.
     *
     * Internalizes object from given read stream.
     * Reads additional version and control information
     * from the stream so internalization is capable to
     * read streams generated e.g with older versions
     * of component having fewer data fields.
     * This implementation is tested with the following
     * stream types:
     * - file
     * - buffer
     *
     * NOTE! Versioning is based on usage of:
     *  - MIMPSSapObject::ObjectVersion()
     *  - MIMPSSapObject::InternalizeVersionDataL()
     *
     * @since 3.0
     * @param aObj The object to internalize.
     * @aStream aStream The stream to read the data.
     */
    static void InternalizeL( MIMPSSapObject& aObj, RReadStream& aStream );


    /**
     * Copies given source data / state to
     * target object.
     *
     * @since 3.0
     * @param aSource The object to copy.
     * @param aTarget The target for copy,
     */
    static void CopyL( const MIMPSSapObject& aSource,
                       MIMPSSapObject& aTarget );



private:  // Prohibited constructor & destructor

    IMPSSAPObjectHandler();
    ~IMPSSAPObjectHandler();
    };


#endif      // __IMPSSAPOBJECTHANDLER_H

//  End of File
