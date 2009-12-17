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
* Description:  Interface for header information of history file
*
*/


#ifndef __MCALOGGERMESSAGEHEADER_H__
#define __MCALOGGERMESSAGEHEADER_H__

//INCLUDES

#include <e32std.h>
#include <e32base.h>

//FORWARD DECLARATIONS

class RWriteStream;

// CLASS DECLARATION

/**
 *  Interface for header information of history file
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerMessageHeader
    {
    public:

        /**
         * Destructor.
         */
        virtual ~MCALoggerMessageHeader() {};

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

        /**
         *	Get filename
         *	@since 2.1
         *	@return Real filename
         */
        virtual const TDesC& FilenameL() = 0;

        /**
         *	Set filename
         *	@since 2.1
         *	@param aFilename Filename to set
         */
        virtual void SetFilenameL( const TDesC& aFilename ) = 0;

        /**
         *	This triggers footer information like time when logging ends.
         */
        virtual void EndLogging() = 0;
    };

#endif      // __MCALOGGERMESSAGEHEADER_H__

// End of File