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
* Description:  Container for detailed errors
*
*/



#ifndef MCAPRESENCEERRORS_H
#define MCAPRESENCEERRORS_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MCAPresenceError;
class CCAPresenceError;

// CLASS DECLARATION

/**
*  Container class for presence errors
*
*  @lib CAPresence.lib
*  @since 2.1
*/
class MCAPresenceErrors
    {
    public: // New functions

        /**
        * Sets error
        * @since 2.1
        * @param aError Error id
        */
        virtual void SetError( TInt aError ) = 0;

        /**
        * Returns error code
        * @since 2.1
        * @return Integer that contains error id
        */
        virtual TInt Error() const = 0;

        /**
        * Adds detailed presence error to error container
        * @since 2.1
        * @param aError Presence error
        */
        virtual void AddDetailedErrorL( CCAPresenceError* aError ) = 0;

        /**
        * Removes presence error from container
        * @since 2.1
        * @param aError
        */
        virtual void RemoveDetailedError( CCAPresenceError* aError ) = 0;

        /**
        * Resets container
        * @since 2.1
        */
        virtual void Reset() = 0;

        /**
        * Returns array of errors
        * @since 2.1
        * @return Array of presence errors
        */
        virtual const RPointerArray<MCAPresenceError>& DetailedErrors() const = 0;
    };

#endif      // MCAPRESENCEERRORS_H

// End of File
