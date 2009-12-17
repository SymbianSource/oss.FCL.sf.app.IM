/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementationf for contact and group list factory
*
*/



// INCLUDE FILES
#include <e32std.h>

#include "CCAStorageManagerFactory.h"
#include "CCAStorageManager.h"
#include "MCAStoredContacts.h"
#include "MCAStoredGroups.h"
#include "CCAStorageDefs.h"
#include "ChatDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAStorageManagerFactory::ContactListInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAStoredContacts* CCAStorageManagerFactory::ContactListInterfaceL()
    {
    return ( MCAStoredContacts* ) CCAStorageManager::InstanceL();
    }

// -----------------------------------------------------------------------------
// CCAStorageManagerFactory::GroupListInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAStoredGroups* CCAStorageManagerFactory::GroupListInterfaceL()
    {
    return ( MCAStoredGroups* ) CCAStorageManager::InstanceL();
    }


// -----------------------------------------------------------------------------
// CCAStorageManagerFactory::ReleaseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAStorageManagerFactory::ReleaseL()
    {
    CCAStorageManager::ReleaseL();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
