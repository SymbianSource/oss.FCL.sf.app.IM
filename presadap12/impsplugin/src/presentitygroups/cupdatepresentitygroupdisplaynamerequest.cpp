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
#include "CUpdatePresentityGroupDisplayNameRequest.h"
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
// CUpdatePresentityGroupDisplayNameRequest::CUpdatePresentityGroupDisplayNameRequest
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupDisplayNameRequest::CUpdatePresentityGroupDisplayNameRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityGroupDisplayNameRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityGroupDisplayNameRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupDisplayNameRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    //CleanupStack::Pop( iParser );

    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupDisplayNameRequest* CUpdatePresentityGroupDisplayNameRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::NewL Start" ) );

    CUpdatePresentityGroupDisplayNameRequest* self = new( ELeave )
    CUpdatePresentityGroupDisplayNameRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupDisplayNameRequest* CUpdatePresentityGroupDisplayNameRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::NewLC Start" ) );

    CUpdatePresentityGroupDisplayNameRequest* self =
        CUpdatePresentityGroupDisplayNameRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::~CUpdatePresentityGroupDisplayNameRequest
// ---------------------------------------------------------------------------
//
CUpdatePresentityGroupDisplayNameRequest::~CUpdatePresentityGroupDisplayNameRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresentityGroupDisplayNameRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iGroupDisplayName;
    delete iGroupId;
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresentityGroupDisplayNameRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupDisplayNameRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupDisplayNameRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::RunL Start" ) );

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
        MPresenceObjectFactory& objFact =
            iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
        MXIMPIdentity* groupName = prfwObjFact.NewIdentityLC();
        groupName->SetIdentityL( *iGroupId );

        MPresentityGroupInfo* groupInfo = objFact.NewPresentityGroupInfoLC();
        groupInfo->SetGroupIdL( groupName );
        groupInfo->SetGroupDisplayNameL( *iGroupDisplayName );


        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        IMPS_DP( D_IMPS_LIT( " CUpdatePresentityGroupDisplayNameRequest::RunL :GroupId  %S " ), iGroupId );

        IMPS_DP( D_IMPS_LIT( " CUpdatePresentityGroupDisplayNameRequest::RunL :GroupName %S " ), iGroupDisplayName );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupDisplayNameUpdatedL( groupInfo );
        const TDesC* dispname = &( groupInfo->GroupDisplayName() );
        CleanupStack::Pop( 2 ); // groupName
        }
    IMPS_DP( D_IMPS_LIT( " UpdatePresentityGroupDisplayNameRequest wvErrorCode %d " ),
             wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresentityGroupDisplayNameRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityGroupDisplayNameRequest::UpdatePresentityGroupDisplayNameL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityGroupDisplayNameRequest::UpdatePresentityGroupDisplayNameL(
    const MXIMPIdentity& aIdentity,
    const TDesC16& aDisplayName )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityGroupDisplayNameL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::UpdatePresentityGroupDisplayNameL:GroupId:%S " ), &aIdentity.Identity() );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityGroupDisplayNameRequest::UpdatePresentityGroupDisplayNameL:DisplayNameL:DisplayName:%S " ), &aDisplayName );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );
    iGroupDisplayName = aDisplayName.AllocL();
    iGroupId = aIdentity.Identity().AllocL();
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer,
                                                        iConnMan.GetVersion() );
    serializer->StartTagL( KListMnageRequest );

    if ( KErrNone == iGroupId->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
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

    serializer->EndTagL( KContactListProperties );
    serializer->StartTagL( KReceiveList ).NarrowTextL( KXMLValueFalse ).EndTagL( KReceiveList );

    serializer->EndTagL( KListMnageRequest );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityGroupDisplayNameL End" ) );
    }

// End of file
