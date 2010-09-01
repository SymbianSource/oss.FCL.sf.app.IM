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
#include <ximpprotocolconnectionhost.h>
#include "cimpsprotocolpresentitygroups.h"
#include "impsdebugprint.h"
#include "cimpsPluginConnectionManager.h"
#include "CSubscribePresentityGroupListRequest.h"

#include "ccreatepresentitygrouprequest.h"
#include "cdeletepresentitygrouprequest.h"
#include "caddPresentityGroupMemberRequest.h"
#include "cremovePresentityGroupMemberRequest.h"
#include "cupdatepresentitygroupmemberdisplaynamerequest.h"
#include "cupdatePresentityGroupDisplayNameRequest.h"
#include "csubscribepresentitygroupcontentrequest.h"
#include "cunsubscribepresentitygroupcontentrequest.h"

#include "impsconnectionmanagercontrolif.h"
#include "impspluginpanics.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsProtocolPresentityGroups::GetInterface( TInt32 aInterfaceId,
                                                   TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresentityGroups::KInterfaceId )
        {
        MProtocolPresentityGroups* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsProtocolPresentityGroups, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsProtocolPresentityGroups::GetInterface( TInt32 aInterfaceId,
                                                         TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresentityGroups::KInterfaceId )
        {
        const MProtocolPresentityGroups* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )

        {
        User::Panic( KImpsProtocolPresentityGroups, KErrExtensionNotSupported );
        }

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CImpsProtocolPresentityGroups::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::GetInterfaceId() Start-End" ) );
    return MProtocolPresentityGroups::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::CImpsProtocolPresentityGroups()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresentityGroups::CImpsProtocolPresentityGroups(
    MImpsPrtPluginConnectionManager& aConnMan )
        : iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "Groups::CImpsProtocolPresentityGroups() Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::ConstructL() Start" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::NewL()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresentityGroups* CImpsProtocolPresentityGroups::NewL(
    MImpsPrtPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::NewL() Start" ) );

    CImpsProtocolPresentityGroups* self =
        new( ELeave ) CImpsProtocolPresentityGroups( aConnMan );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresentityGroups::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::~CImpsProtocolPresentityGroups()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresentityGroups::~CImpsProtocolPresentityGroups()
    {
    IMPS_DP( D_IMPS_LIT( "Groups::~CImpsProtocolPresentityGroups() Start" ) );


    IMPS_DP( D_IMPS_LIT( "Groups::~CImpsProtocolPresentityGroups() End" ) );
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoSubscribePresentityGroupListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoSubscribePresentityGroupListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Groups::DoSubscribePresentityGroupListL() Start" ) );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresentityGroupListRequest* req =
        CSubscribePresentityGroupListRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribePresentityGroupListRequestL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Groups::DoSubscribePresentityGroupListL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoUnsubscribePresentityGroupListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoUnsubscribePresentityGroupListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoUnsubscribePresentityGroupListL() Start" );

    // Just complete the request for the presence framework.
    iConnMan.HandleToHost().HandleRequestCompleted( aReqId, KErrNone );

    IMPS_DP_TXT( "Groups::DoUnsubscribePresentityGroupListL() End" );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoCreatePresentityGroupL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoCreatePresentityGroupL(
    const MXIMPIdentity& aIdentity,
    const TDesC16& aDisplayName,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoCreatePresentityGroupL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CCreatePresentityGroupRequest* req =
        CCreatePresentityGroupRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );
    req->CreatePresentityGroupRequestL( aIdentity, aDisplayName );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoCreatePresentityGroupL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoDeletePresentityGroupL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoDeletePresentityGroupL(
    const MXIMPIdentity& aIdentity,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoDeletePresentityGroupL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CDeletePresentityGroupRequest* req =
        CDeletePresentityGroupRequest::NewLC( iConnMan, aReqId );

    req->DeletePresentityGroupRequestL( aIdentity );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoDeletePresentityGroupL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoUpdatePresentityGroupDisplayNameL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoUpdatePresentityGroupDisplayNameL(
    const MXIMPIdentity& aIdentity,
    const TDesC16& aDisplayName,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoUpdatePresentityGroupDisplayNameL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresentityGroupDisplayNameRequest* req =
        CUpdatePresentityGroupDisplayNameRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdatePresentityGroupDisplayNameL( aIdentity, aDisplayName );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoUpdatePresentityGroupDisplayNameL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoSubscribePresentityGroupContentL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoSubscribePresentityGroupContentL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoSubscribePresentityGroupContentL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresentityGroupContentRequest* req =
        CSubscribePresentityGroupContentRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribePresentityGroupContentL( aGroupId );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoSubscribePresentityGroupContentL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoUnsubscribePresentityGroupContentL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoUnsubscribePresentityGroupContentL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoUnsubscribePresentityGroupContentL() Start" );

    IMPSPRTPLUGIN_UNUSED_PARAM( aGroupId );
    // just complete the request to PrFw Successfully

    iConnMan.HandleToHost().HandleRequestCompleted( aReqId, KErrNone );

    IMPS_DP_TXT( "Groups::DoUnsubscribePresentityGroupContentL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoAddPresentityGroupMemberL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoAddPresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& aMemberDisplayName,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoAddPresentityGroupMemberL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CAddPresentityGroupMemberRequest* req =
        CAddPresentityGroupMemberRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->AddPresentityGroupMemberL( aGroupId, aMemberId, aMemberDisplayName );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoAddPresentityGroupMemberL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoRemovePresentityGroupMemberL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoRemovePresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoRemovePresentityGroupMemberL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CRemovePresentityGroupMemberRequest* req =
        CRemovePresentityGroupMemberRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->RemovePresentityGroupMemberL( aGroupId, aMemberId );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoRemovePresentityGroupMemberL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::DoUpdatePresentityGroupMemberDisplayNameL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::DoUpdatePresentityGroupMemberDisplayNameL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& aMemberDisplayName,
    TXIMPRequestId aReqId )
    {
    IMPS_DP_TXT( "Groups::DoUpdatePresentityGroupMemberDisplayNameL() Start" );

    CreateDataHandlerL();

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresentityGroupMemberDisplayNameRequest* req =
        CUpdatePresentityGroupMemberDisplayNameRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdatePresentityGroupMemberDisplayNameL( aGroupId, aMemberId, aMemberDisplayName );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP_TXT( "Groups::DoUpdatePresentityGroupMemberDisplayNameL() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresentityGroups::CreateDataHandler()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresentityGroups::CreateDataHandlerL()
    {
    IMPS_DP_TXT( "CImpsProtocolPresentityGroups::CreateDataHandler() Start" );
    if ( !( iConnMan.IsDataHandlerCreatedL() ) )
        {
        iConnMan.ManufactureDataHandlerL();
        }
    IMPS_DP_TXT( "CImpsProtocolPresentityGroups::CreateDataHandler() End" );
    }

// End of file

