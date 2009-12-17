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
* Description:  Class implements IMPS factory interface.
*
*/



// INCLUDE FILES
#include    "CCAImpsFactory.h"
#include    "CCAImpsGroupClient.h"
#include    "CCAImpsFundClient.h"
#include    "CCAImpsImClient.h"
#include    "CCAImpsAccessClient.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAImpsFactory::CCAImpsFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAImpsFactory::CCAImpsFactory()
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAImpsFactory::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAImpsFactory* CCAImpsFactory::NewL()
    {
    CCAImpsFactory* self = new ( ELeave ) CCAImpsFactory;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCAImpsFactory::~CCAImpsFactory()
    {
    Unregister();

    delete iAccessClient;
    delete iImClient;
    delete iGroupClient;
    delete iFundClient;

    if ( iConnected )
        {
        iImpsEng.Close();
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::CreateAccessClientL
// Creates new concrete access client if not created earlier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAImpsAccessClient* CCAImpsFactory::CreateAccessClientL()
    {
    if ( !iAccessClient )
        {
        iAccessClient = CCAImpsAccessClient::NewL( iImpsEng );
        }

    return iAccessClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::CreateImClientL
// Creates new concrete IM client if not created earlier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAImpsImClient* CCAImpsFactory::CreateImClientL()
    {
    if ( !iImClient )
        {
        iImClient = CCAImpsImClient::NewL( iImpsEng );
        }

    return iImClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::CreateGroupClientL
// Creates new concrete group client if not created earlier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAImpsGroupClient* CCAImpsFactory::CreateGroupClientL()
    {
    if ( ! iGroupClient )
        {
        iGroupClient = CCAImpsGroupClient::NewL( iImpsEng );
        }
    return iGroupClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::CreateFundClientL
// Creates new concrete fundamental client if not created earlier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAImpsFundClient* CCAImpsFactory::CreateFundClientL()
    {
    if ( ! iFundClient )
        {
        iFundClient = CCAImpsFundClient::NewL( iImpsEng );
        }
    return iFundClient;
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::RegisterL
// Signals all interfaces to register themselfs to WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFactory::RegisterL()
    {
    if ( iAccessClient )
        {
        iAccessClient->InterfaceL();
        }

    if ( iImClient )
        {
        iImClient->InterfaceL();
        }

    if ( iGroupClient )
        {
        iGroupClient->InterfaceL();
        }

    if ( iFundClient )
        {
        iFundClient->InterfaceL();
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::Unregister
// Signals all interfaces to unregister themselfs from WV Engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFactory::Unregister()
    {
    if ( iAccessClient )
        {
        iAccessClient->Unregister();
        }

    if ( iImClient )
        {
        iImClient->Unregister();
        }

    if ( iGroupClient )
        {
        iGroupClient->Unregister();
        }

    if ( iFundClient )
        {
        iFundClient->Unregister();
        }

    if ( iConnected )
        {
        iImpsEng.Close();
        iConnected = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCAImpsFactory::ConnectL
// Connects to IMPS engine (server)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAImpsFactory::ConnectL()
    {
    if ( !iConnected )
        {
        User::LeaveIfError( iImpsEng.Connect() );
        iConnected = ETrue;
        }
    }

//  End of File
