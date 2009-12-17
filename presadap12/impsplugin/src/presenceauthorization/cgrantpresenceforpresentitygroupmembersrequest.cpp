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
#include "CGrantPresenceForPresentityGroupMembersRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include <protocolpresenceauthorizationdatahost.h>
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
// CGrantPresenceForPresentityGroupMembersRequest::CGrantPresenceForPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityGroupMembersRequest::CGrantPresenceForPresentityGroupMembersRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForPresentityGroupMembersRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForPresentityGroupMembersRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityGroupMembersRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "::ConstructL Start" ) );
    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::NewL
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityGroupMembersRequest* CGrantPresenceForPresentityGroupMembersRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::NewL Start" ) );

    CGrantPresenceForPresentityGroupMembersRequest* self = new( ELeave )
    CGrantPresenceForPresentityGroupMembersRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::NewLC
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityGroupMembersRequest* CGrantPresenceForPresentityGroupMembersRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::NewLC Start" ) );

    CGrantPresenceForPresentityGroupMembersRequest* self =
        CGrantPresenceForPresentityGroupMembersRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::~CGrantPresenceForPresentityGroupMembersRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityGroupMembersRequest::~CGrantPresenceForPresentityGroupMembersRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForPresentityGroupMembersRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForPresentityGroupMembersRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityGroupMembersRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::RunL()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityGroupMembersRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( "GrantPresenceForPresentityGroupMembersL Return Code %d" ), wvErrorCode );
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CGrantPresenceForPresentityGroupMembersRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::RunError Start %d" ), aError );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityGroupMembersRequest::GrantPresenceForPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityGroupMembersRequest::GrantPresenceForPresentityGroupMembersL(
    const MXIMPIdentity& aGroupId,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::GrantPresenceForPresentityGroupMembersL Start" ) );
    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::GrantPresenceForPresentityGroupMembersL:GroupId:%S " ), &aGroupId.Identity() );

//   IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityGroupMembersRequest::GrantPresenceForPresentityGroupMembersL:PresenceInfoFilter:%S "),&aPif);

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aGroupId.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );



    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

    // update some attributes from pif here

    TBool statusmessage = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KStatusMessage );
    TBool availability = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KAvailability );
    TBool allattributes = aPif.MatchPersonFilter( NPresenceInfo::NFieldType::KAcceptAll );

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
    if ( KErrNone == iGroupName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KContactList
                             ).WvAddressL( aGroupId.Identity()
                                         ).EndTagL( KContactList );
        }
    else
        {
        serializer->StartTagL( KContactList
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aGroupId.Identity()
                                                      ).EndTagL( KContactList );
        }
    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );


    IMPS_DP( D_IMPS_LIT( "GrantPresenceForPresentityGroupMembersL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::GrantPresenceForPresentityGroupMembersL End" ) );
    }

// End of file
