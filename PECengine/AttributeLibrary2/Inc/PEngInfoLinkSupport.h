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
* Description:  Wireless Village InfoLink attribute support selector.
*
*/

#ifndef __PENGINFOLINKSUPPORT_H
#define __PENGINFOLINKSUPPORT_H

//  INCLUDES
#include <E32std.h>


// CLASS DECLARATION
/**
 * Wireless Village InfoLink attribute support selector.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
class PEngInfoLinkSupport
    {
    public: // New functions

        /**
         * Gets InfoLink attribute support status from
         * Presence Engine CenRep file.
         *
         * @since 3.0
         * @return ETrue if InfoLink is supported. Else EFalse.
         */
        IMPORT_C static TBool InfoLinkSupportedL();

        /**
         * Sets InfoLink attribute support status to
         * Presence Engine CenRep file.
         *
         * @since 3.0
         * @param ETrue if InfoLink should be supported. Else EFalse.
         */
        IMPORT_C static void SetInfoLinkSupportedL( TBool aSupport );


    protected:  // Prohibited constructor & destructor

        PEngInfoLinkSupport();
        ~PEngInfoLinkSupport();
    };

#endif      // __PENGINFOLINKSUPPORT_H

//  End of File

