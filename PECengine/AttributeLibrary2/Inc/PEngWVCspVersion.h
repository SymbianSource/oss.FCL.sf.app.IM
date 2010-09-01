/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wireless Village CSP version selector.
*
*/

#ifndef __PENGWVCSPVERSION_H
#define __PENGWVCSPVERSION_H

//  INCLUDES
#include <E32std.h>


// DATA TYPES
/**
 * WV CSP versions.
 *
 * @since 3.0
 */
enum TPEngWVCspVersion
    {
    EWVCspV11 = 11,
    EWVCspV12 = 12,
    };


// CLASS DECLARATION
/**
 * Wireless Village CSP version selector.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
class PEngWVCspVersion
    {
    public: // New functions

        /**
         * Selects which WV CSP version to use.
         * Loads used WV CSP version from WV Engine CenRep file.
         *
         * @since 3.0
         * @return CSP version enumeration.
         */
        IMPORT_C static TPEngWVCspVersion SelectVersionL();


        /**
         * Sets used WV CSP version to WV Engine CenRep file.
         *
         * @since 3.0
         * @param CSP version enumeration.
         */
        IMPORT_C static void SetVersionL( TPEngWVCspVersion aVersion );


    protected:  // Prohibited constructor & destructor

        PEngWVCspVersion();
        ~PEngWVCspVersion();
    };

#endif      // __PENGWVCSPVERSION_H

//  End of File

