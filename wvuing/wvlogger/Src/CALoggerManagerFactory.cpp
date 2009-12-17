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
* Description:  Factory class for creating logger manager and for getting
*				 interfaces for read and write access to history files.
*
*/


// INCLUDE FILES

#include "CALoggerManagerFactory.h"
#include "CCALoggerManager.h"

// ================= CONSTRUCTORS AND DESTRUCTORS =======================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CALoggerManagerFactory::~CALoggerManagerFactory()
    {
    }
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CALoggerManagerFactory::CALoggerManagerFactory()
    {
    }
// -----------------------------------------------------------------------------
// CCALoggerManagerFactory::CCALoggerManagerFactory
// Copy constructor.
// -----------------------------------------------------------------------------
//
CALoggerManagerFactory::CALoggerManagerFactory( const CALoggerManagerFactory&
                                                /*aOther*/ )
    {
    }
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// Get read instance to access for history data.
// -----------------------------------------------------------------------------
//
EXPORT_C MCALoggerReadInterface* CALoggerManagerFactory::ReadInstanceL(
    MCALoggerMessageFactory* aMessageFactory , TBool aFirstTime /*EFalse*/ )
    {
    return CCALoggerManager::ReadInstanceL( aMessageFactory, aFirstTime );
    }

// -----------------------------------------------------------------------------
// Get write instance to access writing history data.
// -----------------------------------------------------------------------------
//
EXPORT_C MCALoggerWriteInterface* CALoggerManagerFactory::WriteInstanceL(
    MCALoggerMessageFactory* aMessageFactory )
    {
    return CCALoggerManager::WriteInstanceL( aMessageFactory );
    }

// -----------------------------------------------------------------------------
// For testing only
// -----------------------------------------------------------------------------
//
EXPORT_C void CALoggerManagerFactory::ReleaseInstanceL()
    {
    CCALoggerManager::ReleaseInstanceL();
    }

//end of file
