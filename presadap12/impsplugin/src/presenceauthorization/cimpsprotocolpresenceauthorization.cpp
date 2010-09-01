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


#include <badesca.h>

#include <ximpbase.h>
#include <ximpidentity.h>
#include <ximpprotocolconnectionhost.h>

#include "cimpsprotocolpresenceauthorization.h"
#include "impsdebugprint.h"
#include "cimpspluginconnectionmanager.h"

#include "csubscribepresencegrantrequestlistrequest.h"
#include "cgrantpresenceforpresentityrequest.h"
#include "cupdatepresencegrantpifforpresentityrequest.h"
#include "cwithdrawpresencegrantfrompresentityrequest.h"
#include "cgrantpresenceforpresentitygroupmembersrequest.h"
#include "cupdatepresencegrantpifforpresentitygroupmembersrequest.h"
#include "cwithdrawpresencegrantfrompresentitygroupmembersrequest.h"
#include "cgrantpresenceforeveryonerequest.h"
#include "cupdatepresencegrantpifforeveryonerequest.h"
#include "cwithdrawpresencegrantfromeveryonerequest.h"
#include "csubscribepresenceblocklistrequest.h"
#include "cblockpresenceforpresentityrequest.h"
#include "ccancelpresenceblockfrompresentityrequest.h"
#include "cpresenceauthuserrequest.h"
#include "impspluginpanics.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsConnection::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CImpsProtocolPresenceAuthorization::GetInterface( TInt32 aInterfaceId,
                                                        TIfGetOps aOptions )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresenceAuthorization::KInterfaceId )
        {
        MProtocolPresenceAuthorization* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsProtocolPresenceAuthorization, KErrExtensionNotSupported );
        }
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CImpsProtocolPresenceAuthorization::GetInterface( TInt32 aInterfaceId,
                                                              TIfGetOps aOptions ) const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::GetInterface() Start" ) );
    if ( aInterfaceId == MProtocolPresenceAuthorization::KInterfaceId )
        {
        const MProtocolPresenceAuthorization* self = this;
        return self;
        }

    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KImpsProtocolPresenceAuthorization, KErrExtensionNotSupported );
        }
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::GetInterface() End" ) );
    return NULL;
    }


// ---------------------------------------------------------------------------
// CImpsConnection::NewInstanceLC()
// ---------------------------------------------------------------------------
//
TInt32 CImpsProtocolPresenceAuthorization::GetInterfaceId() const
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::GetInterfaceId() Start-End" ) );
    return MProtocolPresenceAuthorization::KInterfaceId;
    }



// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::CImpsProtocolPresenceAuthorization()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceAuthorization::CImpsProtocolPresenceAuthorization(
    MImpsPrtPluginConnectionManager& aConnMan )
        : iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CImpsProtocolPresenceAuthorization() Start-End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::ConstructL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::ConstructL() Start" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::ConstructL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::NewL()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceAuthorization* CImpsProtocolPresenceAuthorization::NewL(
    MImpsPrtPluginConnectionManager& aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::NewL() Start" ) );

    CImpsProtocolPresenceAuthorization* self =
        new( ELeave ) CImpsProtocolPresenceAuthorization( aConnMan );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsProtocolPresenceAuthorization::NewL() End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::~CImpsProtocolPresenceAuthorization()
// ---------------------------------------------------------------------------
//
CImpsProtocolPresenceAuthorization::~CImpsProtocolPresenceAuthorization()
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::~CImpsProtocolPresenceAuthorization() Start" ) );


    IMPS_DP( D_IMPS_LIT( "Authorization::~CImpsProtocolPresenceAuthorization() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUnsubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoSubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoSubscribePresenceGrantRequestListL() Start" ) );

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresenceGrantRequestListRequest* req =
        CSubscribePresenceGrantRequestListRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribePresenceGrantRequestListL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoSubscribePresenceGrantRequestListL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUnsubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoUnsubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoUnsubscribePresenceGrantRequestListL() Start" ) );

    // complete the request successfully
    iConnMan.HandleToHost().HandleRequestCompleted( aReqId, KErrNone );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoUnsubscribePresenceGrantRequestListL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoGrantPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoGrantPresenceForPresentityL(
    const MXIMPIdentity & aIdentity,
    const MPresenceInfoFilter & aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForPresentityL() Start" ) );
    iRequestId = aReqId;

    CreateDataHandlerL();
    HBufC* PresentityName;
    // find to the iPendingReactiveAuthPresenceGrantList array
    PresentityName = aIdentity.Identity().AllocL();
    TInt index = iConnMan.PresenceAuthGrantReqList()->Find( *PresentityName );
    TInt count = iConnMan.PresenceAuthGrantReqList()->Count();
    delete PresentityName;
    if ( index != KErrNotFound )
        {
        IMPS_DP( D_IMPS_LIT( "iPresenceAuthGrantReqList Find returns %d" ), index );

        // send response to presence-auth request to the server
        SendResponseForReactiveAuthRequestL( aIdentity, aPif, aReqId );

        return;
        }

    IMPS_DP( D_IMPS_LIT( " No pending ReactiveAuthrequest " ) );

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CGrantPresenceForPresentityRequest* req =
        CGrantPresenceForPresentityRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->GrantPresenceForPresentityL( aIdentity, aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForPresentityL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUpdateGrantPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoUpdatePresenceGrantPifForPresentityL(
    const MXIMPIdentity &aIdentity,
    const MPresenceInfoFilter &aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoUpdatePresenceGrantPifForPresentityL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresenceGrantPifForPresentityRequest* req =
        CUpdatePresenceGrantPifForPresentityRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdatePresenceGrantPifForPresentityL( aIdentity, aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );


    IMPS_DP( D_IMPS_LIT( "Authorization::DoUpdatePresenceGrantPifForPresentityL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUpdateGrantPresenceForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoWithdrawPresenceGrantFromPresentityL(
    const MXIMPIdentity &aIdentity,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoWithdrawPresenceGrantFromPresentityL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();

    // Append to the iWithDrawnPresenceGrantList array

    // check for pending reactive authorization requests

    iConnMan.ReserveSpaceForOneMoreRequestL();

    CWithdrawPresenceGrantFromPresentityRequest* req =
        CWithdrawPresenceGrantFromPresentityRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->WithdrawPresenceGrantFromPresentityL( aIdentity );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    // remove from the array
    //TInt removestatus = iWithDrawnPresenceGrantList.Remove(findstatus);

    IMPS_DP( D_IMPS_LIT( "Authorization::DoWithdrawPresenceGrantFromPresentityL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoGrantPresenceForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoGrantPresenceForPresentityGroupMembersL(
    const  MXIMPIdentity &aIdentity,
    const MPresenceInfoFilter &aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForPresentityGroupMembersL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CGrantPresenceForPresentityGroupMembersRequest* req =
        CGrantPresenceForPresentityGroupMembersRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->GrantPresenceForPresentityGroupMembersL( aIdentity, aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForPresentityGroupMembersL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUpdateGrantPresenceForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoUpdatePresenceGrantPifForPresentityGroupMembersL(
    const MXIMPIdentity &aIdentity,
    const MPresenceInfoFilter &aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "DoUpdatePresenceGrantPifForPresentityGroupMembersL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresenceGrantPifForPresentityGroupMembersRequest* req =
        CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdatePresenceGrantPifForPresentityGroupMembersL( aIdentity, aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "DoUpdatePresenceGrantPifForPresentityGroupMembersL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUpdateGrantPresenceForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoWithdrawPresenceGrantFromPresentityGroupMembersL(
    const MXIMPIdentity &aIdentity,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "DoWithdrawPresenceGrantFromPresentityGroupMembersL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CWithdrawPresenceGrantFromPresentityGroupMembersRequest* req =
        CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->WithdrawPresenceGrantFromPresentityGroupMembersL( aIdentity );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "DoWithdrawPresenceGrantFromPresentityGroupMembersL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoGrantPresenceForEveryoneL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoGrantPresenceForEveryoneL(
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForEveryoneL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CGrantPresenceForEveryoneRequest* req =
        CGrantPresenceForEveryoneRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->GrantPresenceForEveryoneL( aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoGrantPresenceForEveryoneL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUpdatePresenceGrantPifForEveryoneL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoUpdatePresenceGrantPifForEveryoneL(
    const MPresenceInfoFilter& aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoUpdatePresenceGrantPifForEveryoneL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CUpdatePresenceGrantPifForEveryoneRequest* req =
        CUpdatePresenceGrantPifForEveryoneRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->UpdatePresenceGrantPifForEveryoneL( aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoUpdatePresenceGrantPifForEveryoneL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoWithdrawPresenceGrantFromEveryoneL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoWithdrawPresenceGrantFromEveryoneL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoWithdrawPresenceGrantFromEveryoneL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CWithdrawPresenceGrantFromEveryoneRequest* req =
        CWithdrawPresenceGrantFromEveryoneRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->WithdrawPresenceGrantFromEveryoneL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );


    IMPS_DP( D_IMPS_LIT( "Authorization::DoWithdrawPresenceGrantFromEveryoneL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoSubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoSubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoSubscribePresenceBlockListL() Start" ) );

    iRequestId = aReqId;

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CSubscribePresenceBlockListRequest* req =
        CSubscribePresenceBlockListRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->SubscribePresenceBlockListL();

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoSubscribePresenceBlockListL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoUnsubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoUnsubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoUnsubscribePresenceBlockListL() Start" ) );

    // complete the request successfully
    iConnMan.HandleToHost().HandleRequestCompleted( aReqId, KErrNone );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoUnsubscribePresenceBlockListL() End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoBlockPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoBlockPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoBlockPresenceForPresentityL() Start" ) );

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CBlockPresenceForPresentityRequest* req =
        CBlockPresenceForPresentityRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->BlockPresenceForPresentityL( aPresentityId );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoBlockPresenceForPresentityL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::DoCancelPresenceBlockFromPresentityL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::DoCancelPresenceBlockFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::DoCancelPresenceBlockFromPresentityL() Start" ) );

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CCancelPresenceBlockFromPresentityRequest* req =
        CCancelPresenceBlockFromPresentityRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->CancelPresenceBlockFromPresentityL( aPresentityId );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::DoCancelPresenceBlockFromPresentityL() End" ) );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::CreateDataHandlerL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::CreateDataHandlerL()
    {
    IMPS_DP_TXT( "Authorization::CreateDataHandler() Start" );
    if ( !( iConnMan.IsDataHandlerCreatedL() ) )
        {
        iConnMan.ManufactureDataHandlerL();
        }
    IMPS_DP_TXT( "Authorization::CreateDataHandler() End" );
    }

// ---------------------------------------------------------------------------
// CImpsProtocolPresenceAuthorization::SendResponseForReactiveAuthRequestL()
// ---------------------------------------------------------------------------
//
void CImpsProtocolPresenceAuthorization::SendResponseForReactiveAuthRequestL(
    const MXIMPIdentity & aIdentity,
    const MPresenceInfoFilter & aPif,
    TXIMPRequestId aReqId )
    {
    IMPS_DP( D_IMPS_LIT( "Authorization::SendResponseForReactiveAuthRequestL() Start" ) );

    // PresenceAuthUser Request

    CreateDataHandlerL();
    iConnMan.ReserveSpaceForOneMoreRequestL();

    CPresenceAuthUserRequest* req =
        CPresenceAuthUserRequest::NewLC( iConnMan, aReqId );

    CleanupStack::Check( req );

    req->PresenceAuthUserRequestL( aIdentity, aPif );

    iConnMan.AddRequest( req );
    CleanupStack::Pop( req );

    IMPS_DP( D_IMPS_LIT( "Authorization::SendResponseForReactiveAuthRequestL() End" ) );
    }

// End of file

