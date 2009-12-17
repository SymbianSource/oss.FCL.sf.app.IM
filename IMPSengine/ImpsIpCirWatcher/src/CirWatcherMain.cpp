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
* Description: Proxy file for CirWatcher
*
*/



// INCLUDE FILES
#include <ecom/ImplementationProxy.h>
#include "TcpCirWatcher.h"
#include "UdpCirWatcher.h"

// CONSTANTS
// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    IMPLEMENTATION_PROXY_ENTRY( 0x101FB0EB,	CTcpCirWatcher::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x101FB0EC,	CUdpCirWatcher::NewL )
#else
        { { 0x101FB0EB }, CTcpCirWatcher::NewL },
        { { 0x101FB0EC }, CUdpCirWatcher::NewL }
#endif
    };


// ----------------------------------------------------
// CTcpCirWatcher::~CTcpCirWatcher
// Destructor
// ----------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
//  End of File
