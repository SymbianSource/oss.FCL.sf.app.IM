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
* Description:  Login data codec.
*
*/

#ifndef __PENGLOGINDATACODEC_H
#define __PENGLOGINDATACODEC_H

//  INCLUDES
#include    <E32Std.h>


// FORWARD DECLARATIONS
class CIMPSSAPSettings;


// CLASS DECLARATION

/**
 * Login data codec.
 *
 * @lib
 * @since 3.0
 */
class PEngLoginDataCodec
    {
    public: // Login data packing

        /**
         * Packs login data to flat HBufC.
         *
         * @since 3.0
         * @param aSAP The SAP settings to pack.
         * @return Login data packed as one HBufC.
         * Buffer ownership is returned to caller.
         */
        IMPORT_C static HBufC8* PackLoginDataL( const CIMPSSAPSettings& aSAP );


        /**
         * Extracts login data from descriptor.
         *
         * @since 3.0
         * @param aLoginData The login data get previously with PackLoginDataL()
         * @param aSAP The SAP settings object to fill with login data.
         */
        IMPORT_C static void UnPackLoginDataL( const TDesC8& aLoginData, CIMPSSAPSettings& aSAP );



    protected: // Prohibited constructors

        PEngLoginDataCodec();
        ~PEngLoginDataCodec();

    };


#endif      // __PENGLOGINDATACODEC_H

// End of File
