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
* Description:  Provides interface to register and unregister all interfaces.
*
*/



// INCLUDE FILES
#include    "TCAInterfaceSignaller.h"
#include    "MCAImpsFactory.h"
#include    <E32STD.H>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCAInterfaceSignaller::TCAInterfaceSignaller
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TCAInterfaceSignaller::TCAInterfaceSignaller()
    {
    }

// -----------------------------------------------------------------------------
// TCAInterfaceSignaller::ConnectL
// Calls factory to connect to IMPS engine
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCAInterfaceSignaller::ConnectL()
    {
    MCAImpsFactory *factory = static_cast<MCAImpsFactory*> ( Dll::Tls() );
    if ( ! factory )
        {
        User::Leave( KErrNotFound );
        }
    factory->ConnectL();
    }

// -----------------------------------------------------------------------------
// TCAInterfaceSignaller::RegisterAllL
// Calls factory to register all interfaces
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCAInterfaceSignaller::RegisterAllL()
    {
    MCAImpsFactory *factory = static_cast<MCAImpsFactory*> ( Dll::Tls() );
    if ( ! factory )
        {
        User::Leave( KErrNotFound );
        }
    factory->RegisterL();
    }

// -----------------------------------------------------------------------------
// TCAInterfaceSignaller::UnregisterAllL
// Calls factory to unregister all interfaces
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCAInterfaceSignaller::UnregisterAllL()
    {
    MCAImpsFactory *factory = static_cast<MCAImpsFactory*> ( Dll::Tls() );
    if ( ! factory )
        {
        User::Leave( KErrNotFound );
        }
    factory->Unregister();
    }

//  End of File
