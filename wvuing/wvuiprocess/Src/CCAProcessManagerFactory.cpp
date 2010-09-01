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
* Description:  Process Manager Factory for creating the Process Manager
*
*/



// INCLUDE FILES

#include	"MCAProcessManager.h"
#include	"CCAProcessManager.h"
#include 	"CCAProcessManagerFactory.h"




// -----------------------------------------------------------------------------
// ConstructSingletonInstanceL
// Factory function for creating a singleton instance of commandmanager factory.
// Returns: A void pointer to the created instance.
// -----------------------------------------------------------------------------
//
TAny* ConstructSingletonInstanceL()
    {
    return CCAProcessManagerFactory::NewL();
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::NewL
// -----------------------------------------------------------------------------
//
CCAProcessManagerFactory* CCAProcessManagerFactory::NewL()
    {
    CCAProcessManagerFactory* self = new( ELeave ) CCAProcessManagerFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::ConstructL
// -----------------------------------------------------------------------------
//
inline void CCAProcessManagerFactory::ConstructL()
    {
    iProcessManager = CCAProcessManager::NewL();
    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::InstanceL
// -----------------------------------------------------------------------------
//
EXPORT_C CCAProcessManagerFactory* CCAProcessManagerFactory::InstanceL()
    {
    return ( CCAProcessManagerFactory* ) BaseInstanceL();
    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::GetProcessManager
// -----------------------------------------------------------------------------
//
EXPORT_C MCAProcessManager* CCAProcessManagerFactory::GetProcessManager()
    {
    return iProcessManager;
    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::ReleaseResources
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAProcessManagerFactory::ReleaseResources()
    {
    CCAProcessManagerFactory* instance =
        reinterpret_cast<CCAProcessManagerFactory*>( Dll::Tls() );

    if ( instance )
        {
        delete instance;
        }

    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::CCAProcessManagerFactory
// Default Constructor
// -----------------------------------------------------------------------------
//

inline CCAProcessManagerFactory::CCAProcessManagerFactory()
    {
    iProcessManager = NULL;
    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::CCAProcessManagerFactory
// Destructor
// -----------------------------------------------------------------------------
//
CCAProcessManagerFactory::~CCAProcessManagerFactory()
    {
    delete iProcessManager;
    }

//    End of File

