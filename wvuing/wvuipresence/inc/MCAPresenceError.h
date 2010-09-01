/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents one detailed error result
*
*/



#ifndef MCAPRESENCEERROR_H
#define MCAPRESENCEERROR_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Represents one detailed error result
*
*  @lib CAPresence.lib
*  @since 2.1
*/
class MCAPresenceError
    {
    public: // New functions

        /**
        * Returns error code
        * @since 2.1
        * @return Integer value that is error code
        */
        virtual TInt ErrorCode() const = 0;

        /**
        * Returns user id
        * @since 2.1
        * @return Descriptor that contains user id
        */
        virtual const TDesC& UserId() const = 0;

        virtual ~MCAPresenceError() {};

    };

#endif      // MCAPRESENCEERROR_H

// End of File
