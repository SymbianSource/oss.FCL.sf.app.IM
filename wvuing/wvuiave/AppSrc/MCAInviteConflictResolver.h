/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for resolving invite conflicts
*
*/

#ifndef MCAINVITECONFLICTRESOLVER_H
#define MCAINVITECONFLICTRESOLVER_H

#include <badesca.h> // CDesCArray

// CLASS DECLARATION

/**
*  Interface for text viewer
*
*  @lib chat.app
*  @since Series 60 3.0
*/
class MCAInviteConflictResolver
    {
    public: // New functions

        /**
         * Resolves invite conflicts with banned users
         * @param aGroup Reference of GroupId
         * @param aArray of invited wvids
         */
        virtual void ResolveInviteConflictL( const TDesC& aGroup,
                                             CDesCArray& aArray ) = 0;

    protected:

        /**
         * Destructor.
         */
        virtual ~MCAInviteConflictResolver() {};

    };

#endif      // MCAINVITECONFLICTRESOLVER_H

// End of File
