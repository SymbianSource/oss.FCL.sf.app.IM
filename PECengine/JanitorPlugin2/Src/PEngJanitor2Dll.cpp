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
* Description:  ECOM  implementation.
*
*/

#include <E32Base.h>
#include <ECom/ImplementationProxy.h>
#include "CPEngJanitorPlugin.h"


//Store implementation constructors
const TImplementationProxy ImplementationTable[] =
    {
    // hex number is related to the Plugin framework
    IMPLEMENTATION_PROXY_ENTRY( 0x101FB0E5, CPEngJanitorPlugin::NewL )
    };



//Return implementation table
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( ImplementationTable[0] );
    return ImplementationTable;
    }


// End of file


