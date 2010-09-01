/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of creator of MCALoggerMessages.
*
*/


#ifndef TCALOGGERMESSAGEFACTORY_H
#define TCALOGGERMESSAGEFACTORY_H

//INCLUDES

#include <e32std.h>
#include "MCALoggerMessageFactory.h"

//  FORWARD DECLERATIONS

class MCALoggerMessage;
class MCALoggerMessageHeader;

// CLASS DECLARATION

/**
 *  Implementation for creator of MCALoggerMessages.
 *
 *  T-class, because there is no dynamic member variables and because
 *  there is no other class which suites in this case.
 *  Class methods cannot be static, because pointer to MCALoggerMessageFactory
 *  is passed to client.
 *  @lib CALogger.dll
 *  @since 2.1
 */
class TCALoggerMessageFactory : public MCALoggerMessageFactory
    {
    public: // Interface

        /**
         *  Creates logger message
         *  @since 3.0
         *  @param aStream Stream for getting information for factory.
         *  @return Instance to MCALoggerMessage
         */
        MCALoggerMessage* CreateLoggerMessageL( RReadStream& aStream );

        /**
         *  Creates logger message header
         *  @since 2.1
         *  @return Instance to MCALoggerMessageHeader
         */
        MCALoggerMessageHeader* CreateLoggerMessageHeaderL();
    };

#endif      // CALOGGERMESSAGEFACTORY_H

// End of File
