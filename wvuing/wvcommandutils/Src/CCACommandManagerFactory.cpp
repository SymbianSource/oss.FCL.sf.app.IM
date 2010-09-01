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
* Description:  Command Manager Factory
*
*/



// INCLUDE FILES
#include    "CCACommandManagerFactory.h"
#include    "CCACommandManager.h"
#include    "CCASessionHandlerCmd.h"
#include    "MCAProcessManager.h"
#include    "CCAEngine.h"



// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ConstructSingletonInstanceL
// Factory function for creating a singleton instance of commandmanager factory.
// Returns: A void pointer to the created instance.
// -----------------------------------------------------------------------------
//
TAny* ConstructSingletonInstanceL()
    {
    return CCACommandManagerFactory::NewL();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::InstanceL
// -----------------------------------------------------------------------------
//
EXPORT_C CCACommandManagerFactory* CCACommandManagerFactory::InstanceL()
    {
    return ( CCACommandManagerFactory* ) BaseInstanceL();
    }



// -----------------------------------------------------------------------------
// CCACommandManagerFactory::ReleaseResources
// -----------------------------------------------------------------------------
//
EXPORT_C void CCACommandManagerFactory::ReleaseResources()
    {
    CCACommandManagerFactory* instance =
        reinterpret_cast<CCACommandManagerFactory*>( Dll::Tls() );

    if ( instance )
        {
        delete instance;
        }
    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::InitializeSessionHandlerL
// -----------------------------------------------------------------------------
//

EXPORT_C void CCACommandManagerFactory::InitializeSessionHandlerL(
    MCAProcessManager& aProcessManager,
    MCAUiLoginCmdCB& aUiLoginCmdCB )
    {
    iSessionHandlerCmd = CCASessionHandlerCmd::NewL(
                             aProcessManager,
                             aUiLoginCmdCB );
    iCommandManager->Initialize( iSessionHandlerCmd );
    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::CCACommandManagerFactory
// -----------------------------------------------------------------------------
//
inline CCACommandManagerFactory::CCACommandManagerFactory()

    {

    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::ConstructL
// -----------------------------------------------------------------------------
//
inline void CCACommandManagerFactory::ConstructL()
    {
    // These are needed in start up

    iCommandManager = CCACommandManager::NewL();

    }

// -----------------------------------------------------------------------------
// CCACommandManagerFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CCACommandManagerFactory* CCACommandManagerFactory::NewL()
    {
    CCACommandManagerFactory* self =
        new( ELeave ) CCACommandManagerFactory();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::~CCACommandManagerFactory
// Destructor
// -----------------------------------------------------------------------------
//

CCACommandManagerFactory::~CCACommandManagerFactory()
    {
    delete iCommandManager;
    delete iSessionHandlerCmd;
    }


// -----------------------------------------------------------------------------
// CCACommandManagerFactory::GetCommandManager
// -----------------------------------------------------------------------------
//

EXPORT_C CCACommandManager* CCACommandManagerFactory::GetCommandManager()
    {
    return iCommandManager;
    }

//    End of File

