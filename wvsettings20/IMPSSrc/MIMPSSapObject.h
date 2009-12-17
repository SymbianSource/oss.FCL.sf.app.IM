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
* Description: Generic object interface
*
*/

#ifndef __MIMPSSAPOBJECT_H
#define __MIMPSSAPOBJECT_H

//  INCLUDES
#include <s32strm.h>


// DATA TYPES

// Enumeration for SAP object IDs
enum TIMPSSapObjectID
    {
    EIMPSSapObjSapSettings   = 0x1230001,
    EIMPSSapObjKeyValuePairs = 0x1230002
    };

// Initial version number for SAP object
const TInt KIMPSSapObjVersionInitial = 1;



// CLASS DECLARATION
/**
 * Generic object interface.
 *
 *
 */
class MIMPSSapObject
    {
    public:  // New methods

        virtual TIMPSSapObjectID ObjectID() const = 0;
        virtual TInt ObjectVersion() const = 0;
        virtual void ExternalizeVersionDataL( TInt aVersion, RWriteStream& aStream ) const = 0;
        virtual void InternalizeVersionDataL( TInt aVersion, RReadStream& aStream ) = 0;
        virtual TInt DataSize() const = 0;
        virtual void Reset() = 0;



    protected:
        virtual ~MIMPSSapObject() {}

    };


#endif      // __MIMPSSAPOBJECT_H

//  End of File
