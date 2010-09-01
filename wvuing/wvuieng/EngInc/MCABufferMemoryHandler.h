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
* Description:  For handling memory status.
*
*/


#ifndef __MCABUFFERMEMORYHANDLER_H__
#define __MCABUFFERMEMORYHANDLER_H__

#include <e32base.h>

//	FORWARD CLASS DECLERATIONS
class MCAChatInterface;

// CLASS DECLARATION

/**
 *  For handling memory status.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCABufferMemoryHandler
    {
    public:
        /**
         * Free memory if needed.
         * @param aSize Size needed.
         * @return ETrue, if successfull, EFalse if not.
         */
        virtual TBool FreeMemoryIfNeededL( TInt aSize ) = 0;

        /**
         * Set access interface to get access to messages.
         * @param aAccess Contains access to message containers.
         */
        virtual void SetAccessInterface( MCAChatInterface* aAccess ) = 0;
    };
#endif      // __MCABUFFERMEMORYHANDLER_H__

// End of File


