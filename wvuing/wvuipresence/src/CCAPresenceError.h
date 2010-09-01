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
* Description:  Represents one detailed error result
*
*/



#ifndef CCAPRESENCEERROR_H
#define CCAPRESENCEERROR_H

//  INCLUDES
#include <e32base.h>
#include "MCAPresenceError.h"

// CLASS DECLARATION

/**
*  Represents one detailed error result
*
*  @lib CAPresence.lib
*  @since 2.1
*/
class CCAPresenceError : public CBase, public MCAPresenceError
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAPresenceError* NewL( TInt aErrorCode, const TDesC& aUserId );

        /**
        * Two-phased constructor.
        */
        static CCAPresenceError* NewLC( TInt aErrorCode, const TDesC& aUserId );

        /**
        * Destructor.
        */
        virtual ~CCAPresenceError();

    public: // From MCAPresenceError

        /**
        * Returns error code
        * @since 2.1
        * @return TInt that contains error code
        */
        TInt ErrorCode() const;

        /**
        * Returns user id
        * @since 2.1
        * @return Descriptor that contains user id
        */
        const TDesC& UserId() const;

    private:

        /**
        * C++ default constructor.
        */
        CCAPresenceError( TInt aErrorCode );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aUserId );

    private:    // Data
        // error code
        TInt    iErrorCode;
        // user id
        HBufC*  iUserId;
    };

#endif      // CCAPRESENCEERROR_H

// End of File
