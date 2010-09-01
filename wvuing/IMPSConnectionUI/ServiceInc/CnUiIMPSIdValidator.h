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

#ifndef __CNUIIMPSIDVALIDATOR_H
#define __CNUIIMPSIDVALIDATOR_H


//  INCLUDES
#include <E32Std.h>


// CLASS DECLARATION
/**
 * Validator to check the IMPS id correctness.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CnUiIMPSIdValidator )
    {
public: // New functions

    /**
     * Validates given IMPS login id.
     *
     * Checks is the given IMPS id a valid login
     * id or not. Assumes the IMPS id to be a WV id.
     *
     * Checks the following details from the IMPS (WV) id:
     * - forbidden characters
     * - user part format
     * - domain part format
     * - id not too long
     *
     * @since 2.1
     * @param aImpsId The IMPS id to check.
     * @return ETrue if the id is a valid login id.
     * Else EFalse.
     */
    static TBool ValidLoginIdL( const TDesC& aImpsId );

private: // helper method
    /**
     * Validates given IMPS login id.
     *
     * Checks the given IMPS id for forbidden characters
     *
     * @since 3.0
     * @param aImpsId The IMPS id to check.
     * @return ETrue if the id is a valid login id.
     * Else EFalse.
     */
    static TBool ForbiddenChars( const TDesC& aImpsId );


private: //prohibited constructors / destructors
    CnUiIMPSIdValidator();
    ~CnUiIMPSIdValidator();
    };

#endif      // __CNUIIMPSIDVALIDATOR_H

// End of File
