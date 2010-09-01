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
* Description: Add presentity group member request
*
*/


#include <ximpprotocolconnectionhost.h>
#include "caddpresentitygroupmemberrequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"


#include "CPEngXMLParser.h"
#include "MPEngXMLParser.h"
#include "CPEngXmlSerializer.h"
#include "MPEngXmlSerializer.h"
#include "CImpsPluginConnectionManager.h"
#include "PEngXMLTags.h"

#include <ximpobjectfactory.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::CAddPresentityGroupMemberRequest
// ---------------------------------------------------------------------------
//
CAddPresentityGroupMemberRequest::CAddPresentityGroupMemberRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CAddPresentityGroupMemberRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CAddPresentityGroupMemberRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CAddPresentityGroupMemberRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::NewL
// ---------------------------------------------------------------------------
//
CAddPresentityGroupMemberRequest* CAddPresentityGroupMemberRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::NewL Start" ) );

    CAddPresentityGroupMemberRequest* self = new( ELeave )
    CAddPresentityGroupMemberRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::NewLC
// ---------------------------------------------------------------------------
//
CAddPresentityGroupMemberRequest* CAddPresentityGroupMemberRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::NewLC Start" ) );

    CAddPresentityGroupMemberRequest* self = CAddPresentityGroupMemberRequest::NewL(
                                                 aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::~CAddPresentityGroupMemberRequest
// ---------------------------------------------------------------------------
//
CAddPresentityGroupMemberRequest::~CAddPresentityGroupMemberRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CAddPresentityGroupMemberRequest Start" ) );

    delete iResponse;
    delete iGroupName;
    delete iMemberName;
    delete iMemberDisplayName;

    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CAddPresentityGroupMemberRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CAddPresentityGroupMemberRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::RunL()
// ---------------------------------------------------------------------------
//
void CAddPresentityGroupMemberRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    //Look for mandatory parts
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


    // issue one more request to prfw to update datacache so that
    // client can receive the event
    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MPresenceObjectFactory& objFact =
            iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
        MXIMPIdentity* groupId = prfwObjFact.NewIdentityLC();
        MPresentityGroupMemberInfo* groupmemberinfo = objFact.NewPresentityGroupMemberInfoLC();

        groupId->SetIdentityL( *iGroupName );

        MXIMPIdentity* memberId = prfwObjFact.NewIdentityLC();
        memberId->SetIdentityL( *iMemberName );
        groupmemberinfo->SetGroupMemberIdL( memberId );
        groupmemberinfo->SetGroupMemberDisplayNameL( *iMemberDisplayName );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupMemberAddedL(
            groupId, groupmemberinfo );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunL:GroupId:%S " ), iGroupName );

        IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunL:GroupMemberID:%S " ), iMemberName );

        IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunL:GroupMemberDisplayName:%S " ), iMemberDisplayName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        CleanupStack::Pop( 3 ); // groupId, memberName, memberId
        }

    // issue request to prfw for the complete of the request
    IMPS_DP( D_IMPS_LIT( "AddPresentityGroupMemberRequest %d" ), wvErrorCode );
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CAddPresentityGroupMemberRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& aMemberDisplayName )
    {
    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL:GroupId:%S " ), & aGroupId.Identity() );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL:MemberId:%S " ), & aMemberId.Identity() );

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL:aMemberDisplayName:%S " ), &aMemberDisplayName );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aGroupId.Identity().AllocL();
    iMemberName = aMemberId.Identity().AllocL();
    iMemberDisplayName = aMemberDisplayName.AllocL();

    IMPS_DP( D_IMPS_LIT( "AddPresentityGroupMemberL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KListMnageRequest );
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
    serializer->StartTagL( KAddNickList );
    // <NickName>
    // <Name>
    serializer->StartTagL( KNickName ).StartTagL( KName );

    // Nick name
    // </Name>
    serializer->UnicodeTextL( aMemberDisplayName ).EndTagL( KName );

    // <UserID>
    // contact Id
    // </UserID>
    // </NickName>
    if ( KErrNone == iMemberName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aMemberId.Identity()
                                         ).EndTagL( KUserIDXMLTag
                                                  ).EndTagL( KNickName );
        }


    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aMemberId.Identity()
                                                      ).EndTagL( KUserIDXMLTag
                                                               ).EndTagL( KNickName );
        }

    serializer->EndTagL( KAddNickList );
    serializer->StartTagL( KReceiveList ).NarrowTextL( KXMLValueFalse ).EndTagL( KReceiveList );

    serializer->EndTagL( KListMnageRequest );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CAddPresentityGroupMemberRequest::AddPresentityGroupMemberL End" ) );
    }

// End of file
