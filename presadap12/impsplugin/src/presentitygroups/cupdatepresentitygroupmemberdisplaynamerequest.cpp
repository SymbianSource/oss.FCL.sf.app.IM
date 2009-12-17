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


#include <ximpprotocolconnectionhost.h>
#include "CUpdatePresentityGroupMemberDisplayNameRequest.h"
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
// CUpdatePresentityGroupMemberDisplayNameRequest::CUpdatePresentityGroupMemberDisplayNameRequest
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupMemberDisplayNameRequest::CUpdatePresentityGroupMemberDisplayNameRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityGroupMemberDisplayNameRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityGroupMemberDisplayNameRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupMemberDisplayNameRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupMemberDisplayNameRequest* CUpdatePresentityGroupMemberDisplayNameRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::NewL Start" ) );

    CUpdatePresentityGroupMemberDisplayNameRequest* self = new( ELeave )
    CUpdatePresentityGroupMemberDisplayNameRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupMemberDisplayNameRequest* CUpdatePresentityGroupMemberDisplayNameRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::NewLC Start" ) );

    CUpdatePresentityGroupMemberDisplayNameRequest* self =
        CUpdatePresentityGroupMemberDisplayNameRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::~CUpdatePresentityGroupMemberDisplayNameRequest
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupMemberDisplayNameRequest::~CUpdatePresentityGroupMemberDisplayNameRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresentityGroupMemberDisplayNameRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iMemberDisplayName;
    delete iMemberId;
    delete iParser;
    delete iGroupId;
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresentityGroupMemberDisplayNameRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupMemberDisplayNameRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupMemberDisplayNameRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::RunL Start" ) );

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

    // issue request to prfw datacache for updated info
    // so that client can get the updateded event

    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MPresenceObjectFactory& objFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
        MXIMPIdentity* memberIdentity = prfwObjFact.NewIdentityLC();
        memberIdentity->SetIdentityL( *iMemberId );

        MPresentityGroupMemberInfo* groupmemberInfo = objFact.NewPresentityGroupMemberInfoLC();
        MXIMPIdentity* groupIdentity = prfwObjFact.NewIdentityLC();
        groupIdentity->SetIdentityL( *iGroupId );
        groupmemberInfo->SetGroupMemberIdL( memberIdentity );
        groupmemberInfo->SetGroupMemberDisplayNameL( *iMemberDisplayName );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );
        IMPS_DP( D_IMPS_LIT( " CUpdatePresentityGroupMemberDisplayNameRequest::RunL :GroupId  %S " ), iGroupId );

        IMPS_DP( D_IMPS_LIT( " CUpdatePresentityGroupMemberDisplayNameRequest::RunL :MemberId %S " ), iMemberId );

        IMPS_DP( D_IMPS_LIT( " CUpdatePresentityGroupMemberDisplayNameRequest::RunL :MemberDisplayName %S " ), iMemberDisplayName );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupMemberDisplayNameUpdatedL( groupIdentity, groupmemberInfo );
        CleanupStack::Pop( 3 ); // groupIdentity,groupmemberInfo,memberIdentity
        }
    IMPS_DP( D_IMPS_LIT( " UpdateGrpMemDisplayName wvErrorCode %d " ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );
    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresentityGroupMemberDisplayNameRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityGroupMemberDisplayNameRequest::UpdatePresentityGroupMemberDisplayNameL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupMemberDisplayNameRequest::UpdatePresentityGroupMemberDisplayNameL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& aMemberDisplayName )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityGroupMemberDisplayNameL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::UpdatePresentityGroupMemberDisplayNameL:GroupId:%S " ), &aGroupId.Identity() );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::UpdatePresentityGroupMemberDisplayNameL:MemberId:%S " ), &aMemberId.Identity() );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupMemberDisplayNameRequest::UpdatePresentityGroupMemberDisplayNameL:MemberDisplayName:%S " ), &aMemberDisplayName );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );
    iMemberDisplayName = aMemberDisplayName.AllocL();
    iGroupId = aGroupId.Identity().AllocL();
    iMemberId = aMemberId.Identity().AllocL();
    IMPS_DP( D_IMPS_LIT( "UpdatePresentityGroupMemberDisplayNameL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KListMnageRequest );

    if ( KErrNone == iGroupId->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
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
    if ( KErrNone == iMemberId->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
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

    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityGroupMemberDisplayNameL End" ) );
    }

// End of file
