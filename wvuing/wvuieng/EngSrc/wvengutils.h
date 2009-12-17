/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  wvuiengine utility methods
*
*/


#ifndef WVENGUTILS_H
#define WVENGUTILS_H

#include <e32base.h>

/**
 *  wvuiengine utility methods.
 *
 *  @lib caengine.lib
 *  @since S60 v3.1
 */
class WVEngUtils
    {

    public:

        /**
         * @param aUid Uid for cenrep
         * @param aKey Central repository key
         * @return Value for key
         */
        static TUint32 CRKeyL( TUid aUid, TUint32 aKey );

    };
#endif // WVENGUTILS_H
