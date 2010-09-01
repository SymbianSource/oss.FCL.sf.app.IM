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
#include "CCreatePresentityGroupRequest.h"
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
#include <presenceobjectfactory.h>
#include <protocolpresencedatahost.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::CCreatePresentityGroupRequest
// ---------------------------------------------------------------------------
//
CCreatePresentityGroupRequest::CCreatePresentityGroupRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CCreatePresentityGroupRequest Start" ) );
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest request id is %d %d" ), iRequestId, aRequestId );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CCreatePresentityGroupRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CCreatePresentityGroupRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::ConstructL Start" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::NewL
// ---------------------------------------------------------------------------
//
CCreatePresentityGroupRequest* CCreatePresentityGroupRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::NewL Start" ) );

    CCreatePresentityGroupRequest* self =
        new( ELeave ) CCreatePresentityGroupRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::NewLC
// ---------------------------------------------------------------------------
//
CCreatePresentityGroupRequest* CCreatePresentityGroupRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::NewLC Start" ) );

    CCreatePresentityGroupRequest* self =
        CCreatePresentityGroupRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::~CCreatePresentityGroupRequest
// ---------------------------------------------------------------------------
//
CCreatePresentityGroupRequest::~CCreatePresentityGroupRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CCreatePresentityGroupRequest Start" ) );
    delete iResponse;
    delete iGroupDisplayName;
    delete iParser;
    delete iGroupId;
    CActive::Cancel();
    iConnMan.Remove( this );

    IMPS_DP( D_IMPS_LIT( "::~CCreatePresentityGroupRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CCreatePresentityGroupRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::RunL()
// ---------------------------------------------------------------------------
//
void CCreatePresentityGroupRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( "CreatePresentityGroupRequest Return Code %d" ), wvErrorCode );


    // issue request to prfw datacache for updated info
    // so that client can get the updateded event

    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MPresenceObjectFactory& objFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
        MXIMPIdentity* groupIdentity = prfwObjFact.NewIdentityLC();
        groupIdentity->SetIdentityL( *iGroupId );

        MPresentityGroupInfo* groupInfo = objFact.NewPresentityGroupInfoLC();
        groupInfo->SetGroupIdL( groupIdentity );
        groupInfo->SetGroupDisplayNameL( *iGroupDisplayName );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupCreatedL( groupInfo );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunL():GroupId:%S " ), iGroupId );

        IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunL():GroupDisplayName:%S " ), iGroupDisplayName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        CleanupStack::Pop( 2 ); // groupInfo,groupName
        }
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );
    delete this;
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CCreatePresentityGroupRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunError %d " ), aError );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCreatePresentityGroupRequest::CreatePresentityGroupRequestL
// ---------------------------------------------------------------------------
//
void CCreatePresentityGroupRequest::CreatePresentityGroupRequestL(
    const MXIMPIdentity& aIdentity,
    const TDesC16& aDisplayName )
    {
    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::CreatePresentityGroupRequestL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::CreatePresentityGroupRequestL:aIdentity:%S " ), & aIdentity.Identity() );

    IMPS_DP( D_IMPS_LIT( "CCreatePresentityGroupRequest::CreatePresentityGroupRequestL:aDisplayName:%S " ), &aDisplayName );


    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupDisplayName = aDisplayName.AllocL();

    iGroupId = aIdentity.Identity().AllocL();
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer,
                                                        iConnMan.GetVersion() );


    // <CreateList-Request>
    serializer->StartTagL( KCreateList );
    if ( KErrNone == iGroupId->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KContactList ).
        WvAddressL( aIdentity.Identity() ).
        //NarrowTextL( KServerAddressTag ).
        EndTagL( KContactList );
        }


    else
        {
        serializer->StartTagL( KContactList ).
        NarrowTextL( KWVXMLTag ).
        //NarrowTextL(KHash).
        WvAddressL( aIdentity.Identity() ).
        //NarrowTextL( KServerAddressTag ).
        EndTagL( KContactList );
        }
    // <ContactListProperties>
    // <Property>
    serializer->StartTagL( KContactListProperties ).
    StartTagL( KProperty );

    // <Name>
    // DisplayName
    // </Name>
    serializer->StartTagL( KName ).
    NarrowTextL( KDispName ).
    EndTagL( KName );

    // <Value>
    // display name value
    // </Value>
    serializer->StartTagL( KValue ).
    UnicodeTextL( aDisplayName ).
    EndTagL( KValue );

    serializer->EndTagL( KProperty );
    serializer->StartTagL( KProperty );
    // <Name>
    // Default
    // </Name>
    serializer->StartTagL( KName ).
    NarrowTextL( KDefault ).
    EndTagL( KName );

    // <Value>
    // display name value
    // </Value>
    serializer->StartTagL( KValue ).
    NarrowTextL( KXMLValueFalse ).
    EndTagL( KValue );

    // </Property>
    serializer->EndTagL( KProperty );
    serializer->EndTagL( KContactListProperties );


    // </CreateList-Request>
    serializer->EndTagL( KCreateList );
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "CreatePresentityGroupRequest" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // complete the request to prfw
//	iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::CreatePresentityGroupRequestL End" ) );
    }

// End of file
