/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utils for message handling
*
*/



#ifndef MCAMESSAGERWINTERFACEPC_H
#define MCAMESSAGERWINTERFACEPC_H


// FORWARD DECLARATIONS
class MCAMessagesReadInterface;
class MCAMessagesWriteInterface;

// CLASS DECLARATION

/**
*  Utils for message handling
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class MCAMessageRWInterfacePC
    {
    public:

        virtual MCAMessagesReadInterface& ReadInterfaceL() const = 0;

        virtual MCAMessagesWriteInterface& WriteInterfaceL() const = 0;

        /**
         * Destructor.
         */
        virtual ~MCAMessageRWInterfacePC() {};

    };

#endif      // MCAMESSAGERWINTERFACEPC_H

// End of File
