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
* Description:  IMPS Protocol Publishing implementation for Presence Framework
*
*/


#include <badesca.h>

#include <ximpbase.h>
#include <ximpidentity.h>
#include <ximpprotocolconnectionhost.h>

#include "cimpsprotocolpresencepublishing.h"
#include "ImpsDebugPrint.h"
#include "cimpspluginconnectionmanager.h"
#include "csubscribeownpresencerequest.h"
#include "cpresencenotificationhandler.h"
#include "cunsubscribeownpresencerequest.h"
#include "cpublishownpresencerequest.h"
#include "csubscribepresencewatcherlistrequest.h"
#include "cupdateownpresencesubscriptionpifrequest.h"
#include "impspluginpanics.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsProtocolPresencePublishing::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() Start" ) );


    if ( aInterfaceId == MProtocolPresencePublishing::KInterfaceId )
        {
        MProtocolPresencePublishing* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsProtocolPresencePublishing, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsProtocolPresencePublishing::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() Start" ) );

    if ( aInterfaceId == MProtocolPresencePublishing::KInterfaceId )
        {
        const MProtocolPresencePublishing* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsProtocolPresencePublishing, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::NewInstanceLC()
// ---------------------------------------------------------------------------
//
TInt32 CImpsProtocolPresencePublishing::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::GetInterfaceId() Start-End" ) );
    return MProtocolPresencePublishing::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::CImpsProtocolPresencePublishing()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresencePublishing::CImpsProtocolPresencePublishing(
    MImpsPrtPluginConnectionManager& aConnMan )
        : iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::CImpsProtocolPresencePublishing() Start" ) );
    IMPS_DP( D_IMPS_LIT( "Publishing::CImpsProtocolPresencePublishing() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::ConstructL() Start" ) );

    //iNotificationHandler = CPresenceNotificationHandler::NewL( iConnMan );


    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::NewL()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresencePublishing* CImpsProtocolPresencePublishing::NewL(
    MImpsPrtPluginConnectionManager& aConnMan  )

    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::NewL() Start" ) );

    CImpsProtocolPresencePublishing* self =
        new( ELeave ) CImpsProtocolPresencePublishing( aConnMan );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresencePublishing::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::~CImpsProtocolPresencePublishing()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresencePublishing::~CImpsProtocolPresencePublishing()
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::~CImpsProtocolPresencePublishing() Start" ) );

    //delete iNotificationHandler;

    IMPS_DP( D_IMPS_LIT( "Publishing::~CImpsProtocolPresencePublishing() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoSubscribeOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoSubscribeOwnPresenceL(
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoSubscribeOwnPresenceL() Start" ) );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribeOwnPresenceRequest* req =
        CSubscribeOwnPresenceRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribeOwnPresenceL( aPif );

    iConnMan.AddRequest( req );

    CleanupStack::Pop( req );



    IMPS_DP( D_IMPS_LIT( "Publishing::DoSubscribeOwnPresenceL() End" ) );
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoUnsubscribeOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoUnsubscribeOwnPresenceL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoUnsubscribeOwnPresenceL() Start" ) );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUnSubscribeOwnPresenceRequest* req =
        CUnSubscribeOwnPresenceRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UnSubscribeOwnPresenceL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );


    IMPS_DP( D_IMPS_LIT( "Publishing::DoUnsubscribeOwnPresenceL() End" ) );
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoUpdateOwnPresenceSubscriptionL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoUpdateOwnPresenceSubscriptionPifL(
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoUpdateOwnPresenceSubscriptionPifL() Start" ) );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdateOwnPresenceSubscriptionPifRequest* req =
        CUpdateOwnPresenceSubscriptionPifRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdateOwnPresenceSubscriptionPifL( aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Publishing::DoUpdateOwnPresenceSubscriptionPifL() End" ) );
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoPublishOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoPublishOwnPresenceL(
    const MPresenceInfo& aPresence,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoPublishOwnPresenceL() Start" ) );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CPublishOwnPresencerequest* req =
        CPublishOwnPresencerequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->PublishOwnPresenceL( aPresence );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Publishing::DoPublishOwnPresenceL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoSubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoSubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoSubscribePresenceWatcherListL() Start" ) );


    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresenceWatcherListRequest* req =
        CSubscribePresenceWatcherListRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribePresenceWatcherListL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Publishing::DoSubscribePresenceWatcherListL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::DoUnsubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::DoUnsubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Publishing::DoUnsubscribePresenceWatcherListL() Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( aReqId, KErrNone );

    IMPS_DP( D_IMPS_LIT( "Publishing::DoUnsubscribePresenceWatcherListL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresencePublishing::CreateDataHandlerL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresencePublishing::CreateDataHandlerL()
    {
    IMPS_DP_TXT( "Publishing::CreateDataHandler() Start" );
    if ( !( iConnMan.IsDataHandlerCreatedL() ) )
        {
        iConnMan.ManufactureDataHandlerL();

        IMPS_DP_TXT( "Publishing::Start Incoming Event Listener() " );

        }

    IMPS_DP_TXT( "Publishing::CreateDataHandler() End" );
    }



// End of file

