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
* Description:  Interface for creator of MCALoggerMessage:s.
*
*/


#ifndef __MCALOGGERMESSAGEFACTORY_H__
#define __MCALOGGERMESSAGEFACTORY_H__

//INCLUDES

#include <e32std.h>

//	FORWARD DECLERATIONS

class MCALoggerMessage;
class MCALoggerMessageHeader;

// CLASS DECLARATION

/**
 *  Interface for creator of MCALoggerMessages.
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerMessageFactory
    {
    protected:

        /**
         * Destructor.
         */
        virtual ~MCALoggerMessageFactory() {};

    public: // Interface

        /**
         *	Creates logger message
         *	@since 2.1
         *	@return Instance to MCALoggerMessage
         */
        virtual MCALoggerMessage* CreateLoggerMessageL( RReadStream& aStream ) = 0;

        /**
         *	Creates logger message header
         *	@since 2.1
         *	@return Instance to MCALoggerMessageHeader
         */
        virtual MCALoggerMessageHeader* CreateLoggerMessageHeaderL() = 0;
    };

#endif      // __MCALOGGERMESSAGEFACTORY_H__

// End of File