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
* Description: 
* Description:  Publish & Subscribe property change observer
*
*/


// INCLUDE FILES
#include    <CoreApplicationUIsSDKCRKeys.h>
#include    "impsutils.h"
#include    "impsdatautils.h"
#include    "impsserver.h"
#include    "impspropertyobserver.h"
#include    "impspropertyobserverapi.h"
#include    "impstoffobserver.h"

// ================= MEMBER FUNCTIONS =======================

/**
* CImpsTOffObserver
* Observer for terminal off line events. Uses WV engine's common property
* observer class CImpsPropertyObserver.
* This is auxiliary class of CImpsServer.
*/
// -----------------------------------------------------------------------------
// CImpsTOffObserver::NewL
// -----------------------------------------------------------------------------
//
CImpsTOffObserver* CImpsTOffObserver::NewL( CImpsServer& aServer )
    {
    CImpsTOffObserver* self = new ( ELeave ) CImpsTOffObserver( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    };


// -----------------------------------------------------------------------------
// CImpsTOffObserver::CImpsTOffObserver
// -----------------------------------------------------------------------------
//
CImpsTOffObserver::CImpsTOffObserver( CImpsServer& aServer ):
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CImpsTOffObserver::~CImpsTOffObserver()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsTOffObserver: destructor" ) );
#endif
    delete iObserver;
    }

// -----------------------------------------------------------------------------
// CImpsTOffObserver::ConstructL
// -----------------------------------------------------------------------------
void CImpsTOffObserver::ConstructL()
    {
    iObserver = CImpsPropertyObserver::NewL( *this,
                                             KCRUidCoreApplicationUIs, KCoreAppUIsNetworkConnectionAllowed );
    }

// -----------------------------------------------------------------------------
// CImpsTOffObserver::CheckConnAllowed
// -----------------------------------------------------------------------------
TBool CImpsTOffObserver::CheckConnAllowed()
    {
    TInt allowed( ECoreAppUIsNetworkConnectionAllowed );
    TInt errx = KErrNone;
    TRAP( errx, allowed = TImpsDataUtils::GetCenRepIntValueL(
                              KCRUidCoreApplicationUIs, KCoreAppUIsNetworkConnectionAllowed ) );
    if ( allowed == ECoreAppUIsNetworkConnectionNotAllowed )
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsTOffObserver: CheckConnAllowed returns FALSE ***" ) );
#endif
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CImpsTOffObserver::HandlePropertyChangeL
// -----------------------------------------------------------------------------
void CImpsTOffObserver::HandlePropertyChangeL( TInt aValue )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsTOffObserver: HandlePropertyChangeL val=%d" ), aValue );
#endif
    iServer.SetConnAllowed( aValue );
    }


//  End of File

