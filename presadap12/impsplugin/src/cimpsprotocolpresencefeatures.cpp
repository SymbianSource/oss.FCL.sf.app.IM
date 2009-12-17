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
* Description:  IMPS Protocol presence features implementation for Presence Framework
*
*/


#include <badesca.h>

#include "cimpsprotocolpresencefeatures.h"
#include "CImpsPluginConnectionManager.h"
#include "CImpsProtocolPresenceWatching.h"
#include "CImpsProtocolPresencePublishing.h"
#include "CImpsProtocolPresentityGroups.h"
#include "CImpsProtocolPresenceAuthorization.h"

_LIT( KImpsProtocolPresenceFeatures, "CImpsProtocolPresenceFeatures" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsConnection::CImpsConnection()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceFeatures::CImpsProtocolPresenceFeatures()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::CImpsProtocolPresenceFeatures() Start-End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceFeatures::ConstructL(
    CImpsPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::ConstructL() Start" ) );

    iWatching = CImpsProtocolPresenceWatching::NewL( aConnMan );
    iPublishing = CImpsProtocolPresencePublishing::NewL( aConnMan );
    iPresentityGroups = CImpsProtocolPresentityGroups::NewL( aConnMan );
    iAuthrozation = CImpsProtocolPresenceAuthorization::NewL( aConnMan );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::ConstructL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::NewL()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceFeatures* CImpsProtocolPresenceFeatures::NewL(
    CImpsPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::NewL() Start" ) );

    CImpsProtocolPresenceFeatures* self = new( ELeave ) CImpsProtocolPresenceFeatures;
    CleanupStack::PushL( self );
    self->ConstructL( aConnMan );
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::NewL() End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::~CImpsProtocolPresenceFeatures()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceFeatures::~CImpsProtocolPresenceFeatures()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::~CImpsProtocolPresenceFeatures() Start" ) );

    delete iWatching;
    delete iPublishing;
    delete iPresentityGroups;
    delete iAuthrozation;

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::~CImpsProtocolPresenceFeatures() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsProtocolPresenceFeatures::GetInterface( TInt32 aInterfaceId,
                                                   TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresenceFeatures::KInterfaceId )
        {
        MProtocolPresenceFeatures* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsProtocolPresenceFeatures, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsProtocolPresenceFeatures::GetInterface( TInt32 aInterfaceId,
                                                         TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::GetInterface() const Start" ) );

    if ( aInterfaceId == MProtocolPresenceFeatures::KInterfaceId )
        {
        const MProtocolPresenceFeatures* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsProtocolPresenceFeatures, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::GetInterface() const End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::NewInstanceLC()
// ---------------------------------------------------------------------------
//
TInt32 CImpsProtocolPresenceFeatures::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::GetInterfaceId() Start-End" ) );
    return MProtocolPresenceFeatures::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::PresenceWatching()
// ---------------------------------------------------------------------------
//
MProtocolPresenceWatching& CImpsProtocolPresenceFeatures::PresenceWatching()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::PresenceWatching() Start-End" ) );
    return *iWatching;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::PresencePublishing()
// ---------------------------------------------------------------------------
//
MProtocolPresencePublishing& CImpsProtocolPresenceFeatures::PresencePublishing()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::PresencePublishing() Start-End" ) );

    return *iPublishing;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::PresentityGroups()
// ---------------------------------------------------------------------------
//
MProtocolPresentityGroups& CImpsProtocolPresenceFeatures::PresentityGroups()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::PresentityGroups Start-End" ) );
    return *iPresentityGroups;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceFeatures::PresenceAuthorization()
// ---------------------------------------------------------------------------
//
MProtocolPresenceAuthorization& CImpsProtocolPresenceFeatures::PresenceAuthorization()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceFeatures::PresenceAuthorization() Start-End" ) );
    return *iAuthrozation;
    }

