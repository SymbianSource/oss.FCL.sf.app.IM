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
#include "CUpdatePresenceGrantPifForPresentityGroupMembersRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include <presenceinfo.h>

#include "CPEngXMLParser.h"
#include "MPEngXMLParser.h"
#include "CPEngXmlSerializer.h"
#include "MPEngXmlSerializer.h"
#include "CImpsPluginConnectionManager.h"
#include "PEngXMLTags.h"
#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <presenceinfofilter.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::CUpdatePresenceGrantPifForPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityGroupMembersRequest::CUpdatePresenceGrantPifForPresentityGroupMembersRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForPresentityGroupMembersRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForPresentityGroupMembersRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityGroupMembersRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityGroupMembersRequest* CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewL Start" ) );

    CUpdatePresenceGrantPifForPresentityGroupMembersRequest* self = new( ELeave )
    CUpdatePresenceGrantPifForPresentityGroupMembersRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityGroupMembersRequest* CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewLC Start" ) );

    CUpdatePresenceGrantPifForPresentityGroupMembersRequest* self =
        CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::~CUpdatePresenceGrantPifForPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityGroupMembersRequest::~CUpdatePresenceGrantPifForPresentityGroupMembersRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresenceGrantPifForPresentityGroupMembersRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresenceGrantPifForPresentityGroupMembersRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityGroupMembersRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( "UpdatePresenceGrantPifForPresentityGroupMembers Returns  %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ::UpdatePresenceGrantPifForPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityGroupMembersRequest::UpdatePresenceGrantPifForPresentityGroupMembersL(
    const MXIMPIdentity& aGroupId,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForPresentityGroupMembersL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityGroupMembersRequest::UpdatePresenceGrantPifForPresentityGroupMembersL:PresentityId:%S " ), &aGroupId.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aGroupId.Identity().AllocL();

    TBool statusmessage = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KStatusMessage );
    TBool availability = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KAvailability );
    TBool allattributes = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KAcceptAll );

    // extract all device and service attributes

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

    if ( !allattributes )
        {
        if ( statusmessage )
            {
            serializer->StartTagL( KStatusMood ).EndTagL( KStatusMood );
            serializer->StartTagL( KOnlineStatusXMLTag ).EndTagL( KOnlineStatusXMLTag );
            serializer->StartTagL( KStatusTextXMLTag ).EndTagL( KStatusTextXMLTag );
            serializer->StartTagL( KStatusContentXMLTag ).EndTagL( KStatusContentXMLTag );
            }
        if ( availability )
            {
            serializer->StartTagL( KUserAvailabilityXMLTag ).EndTagL( KUserAvailabilityXMLTag );
            }
        }
    serializer->EndTagL( KPresenceSubList );

    serializer->StartTagL( KContactList
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( aGroupId.Identity()
                                                  ).EndTagL( KContactList );

    // all attributes are targeted as default
    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );


    IMPS_DP( D_IMPS_LIT( "UpdatePresenceGrantPifForPresentityGroupMembersL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForPresentityGroupMembersL End" ) );
    }

// End of file
