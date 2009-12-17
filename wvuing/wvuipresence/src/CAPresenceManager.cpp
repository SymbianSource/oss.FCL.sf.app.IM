/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Factory for presence manager
*
*/



// INCLUDE FILES
#include    "CAPresenceManager.h"
#include    "CCAPEngPresenceManager.h"
#include    "CAPresenceDll.h"
#include    "ChatDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAPresenceManager::InstanceL
// Create an instance of the presence manager
// -----------------------------------------------------------------------------
//
EXPORT_C MCAPresence* CAPresenceManager::InstanceL( MCASettings* aSettingsAPI )
    {
    MCAPresence* mFactory = static_cast<MCAPresence*>( Dll::Tls() );

    if ( !mFactory )
        {
        CHAT_DP_TXT( "Creating IM Presence module singleton" );
        CCAPEngPresenceManager *cFactory = CCAPEngPresenceManager::NewL( aSettingsAPI );
        mFactory = cFactory;
        CleanupStack::PushL( cFactory );
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( mFactory ) ) );
        CleanupStack::Pop( cFactory );
        }
    else
        {
        CCAPEngPresenceManager *cFactory = static_cast<CCAPEngPresenceManager*> ( mFactory );
        cFactory->SetSettingsAPIL( aSettingsAPI );
        }

    return mFactory;
    }

// -----------------------------------------------------------------------------
// CAPresenceManager::ReleaseL
// Release this Singleton
// Used mainly for testing purposes
// -----------------------------------------------------------------------------
//
EXPORT_C void CAPresenceManager::Release()
    {
    MCAPresence *mFactory = static_cast<MCAPresence*>( Dll::Tls() );

    if ( mFactory )
        {
        CCAPEngPresenceManager *cFactory = static_cast<CCAPEngPresenceManager*>( mFactory );
        delete cFactory;
        Dll::SetTls( NULL ); // Returning error can be safely ignored
        CHAT_DP_TXT( "Destroying IM Presence module singleton" );
        }
    }

//  End of File
