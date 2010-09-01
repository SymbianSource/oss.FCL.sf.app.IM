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
* Description:  Container for detailed errors
*
*/



#ifndef CCAPRESENCEERRORS_H
#define CCAPRESENCEERRORS_H

//  INCLUDES
#include <e32base.h>
#include "MCAPresenceErrors.h"

// FORWARD DECLARATIONS
class CCAPresenceError;

// CLASS DECLARATION

/**
*  Represents error result of operation
*
*  @lib CAPresence.lib
*  @since 2.1
*/
class CCAPresenceErrors : public CBase, public MCAPresenceErrors
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAPresenceErrors* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAPresenceErrors();

    public: // from MCAPresenceErrors

        /**
        * Sets error code
        * @since 2.1
        * @param aError Error code
        */
        void SetError( TInt aError );

        /**
        * Returns error code
        * @since 2.1
        * @return Interger that is error code
        */
        TInt Error() const;

        /**
        * Adds detailed presence error
        * @since 2.1
        * @param aError Presence error instance
        */
        void AddDetailedErrorL( CCAPresenceError* aError );

        /**
        * Removes detailed error
        * @since 2.1
        * @param aError Presence error that is removed
        */
        void RemoveDetailedError( CCAPresenceError* aError );

        /**
        * Resets error container
        * @since 2.1
        */
        void Reset();

        /**
        * Returns array of presence errors
        * @since 2.1
        * @return Array of detailed presence errors
        */
        const RPointerArray<MCAPresenceError>& DetailedErrors() const;

    private:

        /**
        * C++ default constructor.
        */
        CCAPresenceErrors();

        /**
        * By default Symbian 2nd phase constructor is private.
        */

    private:    // Data

        // Array of errors
        RPointerArray<MCAPresenceError> iErrors;
        TInt iMainError;
    };

#endif      // CCAPRESENCEERRORS_H

// End of File
