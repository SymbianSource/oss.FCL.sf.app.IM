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
* Description:  Publish and subscribe implementation of the shared data factory
*
*/

// INCLUDE FILES
#include    "CIMPSSharedDataFactory.h"
#include	"CIMPSPubSubHandler.h"
#include	"CIMPSCenrepHandler.h"
#include	"CIMPSSystemNotifierPubSub.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSharedDataFactory::CreatePermanentKeyHandlerL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MIMPSSharedData* CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
    MIMPSSharedDataObserver* aObserver, const TUid aUid )
    {
    CIMPSCenrepHandler* cenRepHandler = CIMPSCenrepHandler::NewL( aObserver, aUid );
    return cenRepHandler;
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MIMPSSharedData* CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL(
    MIMPSSharedDataObserver* aObserver, const TUid aUid )
    {
    CIMPSPubSubHandler* pubSubHandler = CIMPSPubSubHandler::NewL( aObserver, aUid );
    return pubSubHandler;
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataFactory::CreateSystemNotifyHandlerL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MIMPSSystemNotifier* CIMPSSharedDataFactory::CreateSystemNotifyHandlerL(
    MIMPSSystemNotifierObserver& aObserver, const TUid aKey )
    {
    CIMPSSystemNotifierPubSub* systemAgent =
        CIMPSSystemNotifierPubSub::NewL( aObserver, aKey );
    return systemAgent;
    }

//  End of File

