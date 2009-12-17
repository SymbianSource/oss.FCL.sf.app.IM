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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/

#include <ximpbase.h>
#include <ximpidentity.h>
#include "impsconnectionmanagercontrolif.h"
#include "cimpsprotocolpresencewatching.h"
#include "impsdebugprint.h"
#include "csubscribepresentitypresence.h"
#include "cunsubscribepresentitypresence.h"
#include "csubscribepresentitygroupmemberspresence.h"
#include "cunsubscribepresentitygroupmemberspresence.h"
#include "cupdatepresentitypresence.h"
#include "cupdatepresentitygroupmemberspresence.h"
#include "impspluginpanics.h"


// ======== MEMBER FUNCTIONS ========



// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::CImpsProtocolPresenceWatching()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceWatching::CImpsProtocolPresenceWatching(
    MImpsPrtPluginConnectionManager& aConnMan )
        : iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::CImpsProtocolPresenceWatching() Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::ConstructL(  )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::ConstructL() Start" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::NewL()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceWatching* CImpsProtocolPresenceWatching::NewL(
    MImpsPrtPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::NewL() Start" ) );

    CImpsProtocolPresenceWatching* self = new( ELeave ) CImpsProtocolPresenceWatching( aConnMan );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::~CImpsProtocolPresenceWatching()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceWatching::~CImpsProtocolPresenceWatching()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::~CImpsProtocolPresenceWatching() Start" ) );


    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::~CImpsProtocolPresenceWatching() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsProtocolPresenceWatching::GetInterface( TInt32 aInterfaceId,
                                                   TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresenceWatching::KInterfaceId )
        {
        MProtocolPresenceWatching* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsProtocolPresenceWatching, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsProtocolPresenceWatching::GetInterface( TInt32 aInterfaceId,
                                                         TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresenceWatching::KInterfaceId )
        {
        const MProtocolPresenceWatching* self = this;
        return self;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsProtocolPresenceWatching, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::NewInstanceLC()
// ---------------------------------------------------------------------------
//
TInt32 CImpsProtocolPresenceWatching::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceWatching::GetInterfaceId() Start-End" ) );
    return MProtocolPresenceWatching::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoSubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoSubscribePresentityPresenceL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoSubscribePresentityPresenceL() Start" );

    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresentityPresence* req =
        CSubscribePresentityPresence::NewLC( iConnMan, aReqId );

    req->SubscribePresentityPresenceRequestL( aIdentity.Identity(), aPif );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP_TXT( "Watching::DoSubscribePresentityPresenceL() End" );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoUpdatePresentityPresenceSubscriptionL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoUpdatePresentityPresenceSubscriptionPifL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoUpdatePresentityPresenceSubscriptionPifL() Start" );

    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresentityPresence* req =
        CUpdatePresentityPresence::NewLC( iConnMan, aReqId );

    req->UpdatePresentityPresenceRequestL( aIdentity.Identity(), aPif );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP_TXT( "Watching::DoUpdatePresentityPresenceSubscriptionPifL() End" );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoUnsubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoUnsubscribePresentityPresenceL(
    const MXIMPIdentity& aIdentity,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoUnsubscribePresentityPresenceL() Start" );

    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUnsubscribePresentityPresence* req =
        CUnsubscribePresentityPresence::NewLC( iConnMan, aReqId );

    req->UnsubscribePresentityPresenceRequestL( aIdentity.Identity() );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Watching::DoUnsubscribePresentityPresenceL() End" );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoSubscribePresentityGroupMembersPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoSubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoSubscribePresentityGroupMembersPresenceL() Start" );


    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresentityGroupMembersPresence* req =
        CSubscribePresentityGroupMembersPresence::NewLC( iConnMan, aReqId );

    req->SubscribePresentityGroupMembersPresenceL( aIdentity.Identity(), aPif );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP_TXT( "Watching::DoSubscribePresentityGroupMembersPresenceL() End" );
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoUpdatePresentityGroupMembersPresenceSubscriptionL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoUpdatePresentityGroupMembersPresenceSubscriptionPifL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoUpdatePresentityGroupMembersPresenceSubscriptionPifL() Start" );

    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresentityGroupMembersPresence* req =
        CUpdatePresentityGroupMembersPresence::NewLC( iConnMan, aReqId );

    req->UpdatePresentityGroupMembersPresenceL( aIdentity.Identity(), aPif );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP_TXT( "Watching::DoUpdatePresentityGroupMembersPresenceSubscriptionPifL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceWatching::DoUnsubscribePresentityGroupMembersPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceWatching::DoUnsubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aIdentity,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Watching::DoUnsubscribePresentityGroupMembersPresenceL() Start" );

    iConnMan.ManufactureDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUnsubscribePresentityGroupMembersPresence* req =
        CUnsubscribePresentityGroupMembersPresence::NewLC( iConnMan, aReqId );

    req->UnsubscribePresentityGroupMembersPresenceL( aIdentity.Identity() );

    iConnMan.AddRequest( req ); //ownership transfered
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Watching::DoUnsubscribePresentityGroupMembersPresenceL() End" );
    }


// End of file

