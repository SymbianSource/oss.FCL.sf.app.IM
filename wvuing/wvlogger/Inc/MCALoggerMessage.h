/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for message information of history file
*
*/


#ifndef __MCALOGGERMESSAGE_H__
#define __MCALOGGERMESSAGE_H__

//INCLUDES

#include <e32std.h>

//FORWARD DECLARATIONS

class RWriteStream;

// CLASS DECLARATION

/**
 *  Interface for message information of history file
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerMessage
    {

    public:

        /**
         * Destructor.
         */
        virtual ~MCALoggerMessage() {};

    public: // Interface

        /**
         * Internalize the contact instance data.
         * Used by MCALoggerFactory.
         * @since 2.1
         * @param aStream The stream into which to internalize
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;

        /**
         * Externalize the contact instance data.
         * Used by CCALoggerWriter
         * @since 2.1
         * @param aStream The stream into which to externalize
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;

        /**
         *	Disk space needed for saving message in externalize.
         *	@return Memory need in bytes.
         */
        virtual TInt MessageSizeInBytesL() const = 0;
    };

#endif      // __MCALOGGERMESSAGE_H__

// End of File