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
* Description:  Validator to check the IMPS id correctness.
*
*/

#ifndef __IMPSPRESENCELOGINDETAILVALIDATOR_H
#define __IMPSPRESENCELOGINDETAILVALIDATOR_H


//  INCLUDES
#include <E32Std.h>


// CLASS DECLARATION
/**
 * Validator to check the Presence login id correctness.
 *
 * @since 2.1
 */
class IMPSPresenceLoginDetailValidator
    {
    public: // New functions

        /**
         * Validates given presence login id.
         *
         * Checks is the given presence id a valid login
         * id or not. Assumes the presence id to be a WV id.
         *
         * Checks the following details from the presence (WV) id:
         * - forbidden characters
         * - user part format
         * - domain part format
         * - id not too long
         *
         * @since 2.1
         * @param aWvId The presence id to check.
         * @return ETrue if the id is a valid login id.
         * Else EFalse.
         */
        IMPORT_C static TBool ValidWVLoginIdL( const TDesC& aWvId );


        /**
         * Validates given IAP validity for presence login.
         *
         * Checks is the given IAP a valid IAP
         * for presence login or not.
         *
         * @since 2.1
         * @param aIAP The IAP to check.
         * @return ETrue if the IAP is a valid login IAP.
         * Else EFalse.
         */
        IMPORT_C static TBool ValidWVLoginIAPL( TUint32 aIAP );




    private: //prohibited constructors / destructors
        IMPSPresenceLoginDetailValidator();
        ~IMPSPresenceLoginDetailValidator();
    };

#endif      // __IMPSPRESENCELOGINDETAILVALIDATOR_H

// End of File
