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
#include "CRemovePresentityGroupMemberRequest.h"
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
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::CRemovePresentityGroupMemberRequest
// ---------------------------------------------------------------------------
//
CRemovePresentityGroupMemberRequest::CRemovePresentityGroupMemberRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CRemovePresentityGroupMemberRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CRemovePresentityGroupMemberRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CRemovePresentityGroupMemberRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::ConstructL Start" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::NewL
// ---------------------------------------------------------------------------
//
CRemovePresentityGroupMemberRequest* CRemovePresentityGroupMemberRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::NewL Start" ) );

    CRemovePresentityGroupMemberRequest* self = new( ELeave )
    CRemovePresentityGroupMemberRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::NewLC
// ---------------------------------------------------------------------------
//
CRemovePresentityGroupMemberRequest* CRemovePresentityGroupMemberRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::NewLC Start" ) );

    CRemovePresentityGroupMemberRequest* self =
        CRemovePresentityGroupMemberRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::~CRemovePresentityGroupMemberRequest
// ---------------------------------------------------------------------------
//
CRemovePresentityGroupMemberRequest::~CRemovePresentityGroupMemberRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CRemovePresentityGroupMemberRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );

    delete iGroupName;
    delete iMemberName;
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CRemovePresentityGroupMemberRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CRemovePresentityGroupMemberRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::RunL()
// ---------------------------------------------------------------------------
//
void CRemovePresentityGroupMemberRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( " RemovePresentityGroupMemberRequest wvErrorCode %d " ), wvErrorCode );


    // issue request to prfw to update datacache so that
    // client can receive the event
    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MXIMPIdentity* groupName = prfwObjFact.NewIdentityLC();
        MXIMPIdentity* memberName = prfwObjFact.NewIdentityLC();

        groupName->SetIdentityL( *iGroupName );
        memberName->SetIdentityL( *iMemberName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunL:GroupId:%S " ), iGroupName );

        IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunL:GroupMemberName:%S " ), iMemberName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupMemberRemovedL(
            groupName, memberName );
        CleanupStack::Pop( 2 ); // groupName, memberName
        }

    // issue request complete to prfw
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CRemovePresentityGroupMemberRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRemovePresentityGroupMemberRequest::RemovePresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CRemovePresentityGroupMemberRequest::RemovePresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId )
    {

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RemovePresentityGroupMemberL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RemovePresentityGroupMemberL:GroupId:%S " ), & aGroupId.Identity() );

    IMPS_DP( D_IMPS_LIT( "CRemovePresentityGroupMemberRequest::RemovePresentityGroupMemberL:MemberId:%S " ), & aMemberId.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aGroupId.Identity().AllocL();
    iMemberName = aMemberId.Identity().AllocL();

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

    serializer->StartTagL( KRemoveNickList );

    // <UserID>
    // contact Id
    // </UserID>
    // </NickName>
    if ( KErrNone == iMemberName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aMemberId.Identity()
                                         ).EndTagL( KUserIDXMLTag
                                                  );
        }


    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aMemberId.Identity()
                                                      ).EndTagL( KUserIDXMLTag
                                                               );
        }


    serializer->EndTagL( KRemoveNickList );

    serializer->StartTagL( KReceiveList ).NarrowTextL( KXMLValueFalse ).EndTagL( KReceiveList );

    serializer->EndTagL( KListMnageRequest );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::RemovePresentityGroupMemberL End" ) );
    }

// End of file
