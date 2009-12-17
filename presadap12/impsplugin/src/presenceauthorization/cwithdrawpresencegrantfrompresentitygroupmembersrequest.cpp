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
* Description:  Create Presentity Group Request to the Network Server.
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CWithdrawPresenceGrantFromPresentityGroupMembersRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include <protocolpresenceauthorizationdatahost.h>

#include "CPEngXMLParser.h"
#include "MPEngXMLParser.h"
#include "CPEngXmlSerializer.h"
#include "MPEngXmlSerializer.h"
#include "CImpsPluginConnectionManager.h"
#include "PEngXMLTags.h"
#include <ximpprotocolconnectionhost.h>
#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::CWithdrawPresenceGrantFromPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityGroupMembersRequest::CWithdrawPresenceGrantFromPresentityGroupMembersRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CWithdrawPresenceGrantFromPresentityGroupMembersRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CWithdrawPresenceGrantFromPresentityGroupMembersRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityGroupMembersRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "::ConstructL Start" ) );

    iParser = CreateXMLParserL();

    IMPS_DP( D_IMPS_LIT( "::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewL
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityGroupMembersRequest* CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewL Start" ) );

    CWithdrawPresenceGrantFromPresentityGroupMembersRequest* self = new( ELeave ) CWithdrawPresenceGrantFromPresentityGroupMembersRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewLC
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityGroupMembersRequest* CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewLC Start" ) );

    CWithdrawPresenceGrantFromPresentityGroupMembersRequest* self =
        CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::~CWithdrawPresenceGrantFromPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityGroupMembersRequest::~CWithdrawPresenceGrantFromPresentityGroupMembersRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CWithdrawPresenceGrantFromPresentityGroupMembers Start-End" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityGroupMembersRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunL()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    // Check if we got 200
    if ( iParser->DecodeL( *iResponse, KResultXMLTag, ETrue ) )
        {
        resultBlock.Set( iParser->ResultL() );
        if ( iParser->DecodeL( resultBlock, KCodeXMLTag, EFalse ) )
            {
            TPtrC8 cspStatusCodeBlock( iParser->ResultL() );
            TInt cspStatusInt;
            TLex8 lexer( cspStatusCodeBlock );
            lexer.SkipSpace();
            TInt lexErr = lexer.Val( cspStatusInt );
            if ( lexErr == KErrNone )
                {
                wvErrorCode = cspStatusInt;
                }
            }
        }

    IMPS_DP( D_IMPS_LIT( "WithdrawPresenceGrantFromPresentityGroupMembersL  %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityGroupMembersRequest::WithdrawPresenceGrantFromPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityGroupMembersRequest::WithdrawPresenceGrantFromPresentityGroupMembersL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromPresentityGroupMembersL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityGroupMembersRequest::WithdrawPresenceGrantFromPresentityGroupMembersL:MemberId:%S " ), &aIdentity.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aIdentity.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

    serializer->EndTagL( KPresenceSubList );
    if ( KErrNone == iGroupName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KContactList
                             ).WvAddressL( aIdentity.Identity()
                                         ).EndTagL( KContactList );
        }


    else
        {
        serializer->StartTagL( KContactList
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aIdentity.Identity()
                                                      ).EndTagL( KContactList );
        }

    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );

    serializer->EndTagL( KCreateAttributeList );

    serializer->EndTagL( KTransactionContent );


    IMPS_DP( D_IMPS_LIT( "WithdrawPresenceGrantFromPresentityGroupMembersL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromPresentityGroupMembersL End" ) );
    }

// End of file
