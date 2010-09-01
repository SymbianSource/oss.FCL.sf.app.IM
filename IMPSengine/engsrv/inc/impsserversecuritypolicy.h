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
* Description: Class defined constant used in imps server
* 
*/


#ifndef CIMPSSERVERSECURITYPOLICY_H
#define CIMPSSERVERSECURITYPOLICY_H

//  INCLUDES
#include "impsclientsrv.h"

// DATA TYPES

// Server Security Policy Definition
const TUint KImpsServerRangeCount = 3;

const TInt KImpsServerRanges[KImpsServerRangeCount] =
    {
    EImpsServNone,              // NetworkControl
    EImpsServCloseSession,      // EAlwaysPass - Close command
    EImpsServCloseSession + 1   // can't fail since client masks bits in a request
    };

// Mapping IPCs to policy element
const TUint8 KImpsServerElementsIndex[KImpsServerRangeCount] =
    {
    0,
    CPolicyServer::EAlwaysPass,
    0
    };

// Individual policy elements
const CPolicyServer::TPolicyElement KImpsServerElements[] =
    {
        {_INIT_SECURITY_POLICY_C1( ECapabilityNetworkServices ), CPolicyServer::EFailClient}
    };

// Main policy
const CPolicyServer::TPolicy KImpsServerPolicy =
    {
    CPolicyServer::ECustomCheck, // CustomCheckL cheks connect request
    KImpsServerRangeCount,
    KImpsServerRanges,
    KImpsServerElementsIndex,
    KImpsServerElements,
    };

#endif // CIMPSSERVERSECURITYPOLICY_H
