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
* Description: 
* helper class for digital schema data.
*
*/

#ifndef TImpsDigestBytes_H
#define TImpsDigestBytes_H

//  INCLUDES
#include <hash.h>
#include "impscommonenums.h"
#include "impsdataaccessor.h"

// CLASS DECLARATION

class TImpsDigestBytes
    {
    public:
        /**
        * C++ default constructor.
        */
        TImpsDigestBytes();

        /**
        * Creates base64 encoded bytes based on MD5 hash of password and nonce.
        * @param aAc data accessor
        * @param aSchema schema - future option
        * @param aPassword password
        * @param aNonce nonce
        */
        void CreateDigestBytesL ( MImpsDataAccessor* aAc,
                                  TImpsDigestSchema aSchema,
                                  const TDesC& aPassword,
                                  const TDesC8& aNonce );

        /**
        * Creates digest bytes.
        * AOL spesific
        * @param aAc data accessor
        * @param aSchema schema - future option
        * @param aPassword password
        * @param aNonce nonce
        * @param aClientId clientID
        * @param aKey1 AOL Key 1
        * @param aKey2 AOL Key 2
        */
        void CreateAolDigestBytesL ( MImpsDataAccessor* aAc,
                                     TImpsDigestSchema aSchema,
                                     const TDesC& aPassword,
                                     const TDesC8& aNonce,
                                     const TDesC& aClientId,
                                     HBufC* aKey1,
                                     HBufC* aKey2 );
    };

#endif
