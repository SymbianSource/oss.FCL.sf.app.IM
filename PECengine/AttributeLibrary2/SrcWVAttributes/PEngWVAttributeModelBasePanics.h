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
* Description:  Panics for WV attribute models.
*
*/

#ifndef __PENGWVATTRIBUTEMODELBASEPANICS_H
#define __PENGWVATTRIBUTEMODELBASEPANICS_H


// INCLUDE FILES
#include <E32Base.h>

//LOCAL constants
#if defined(_DEBUG)
namespace
    {

    //Panic
    _LIT( KWVAttrModBasePanic, "WVAttrModBase" );

    //Panic reasons
    enum TWVAttrModBasePanicReasons
        {
        EAttrEnumValueOutOfBounds
        };
    }
#endif

#endif //  __PENGWVATTRIBUTEMODELBASEPANICS_H


//  End of File
