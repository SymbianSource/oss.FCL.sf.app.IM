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
#include "CGrantPresenceForPresentityRequest.h"
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
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <presenceinfofilter.h>
#include <ximpdatasubscriptionstate.h>

#include <presencegrantrequestinfo.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::CGrantPresenceForPresentityRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityRequest::CGrantPresenceForPresentityRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForPresentityRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::NewL
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityRequest* CGrantPresenceForPresentityRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::NewL Start" ) );

    CGrantPresenceForPresentityRequest* self = new( ELeave )
    CGrantPresenceForPresentityRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::NewLC
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityRequest* CGrantPresenceForPresentityRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::NewLC Start" ) );

    CGrantPresenceForPresentityRequest* self =
        CGrantPresenceForPresentityRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );


    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::~CGrantPresenceForPresentityRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForPresentityRequest::~CGrantPresenceForPresentityRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForPresentityRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;

    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::RunL()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::RunL Start" ) );

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

    MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
    MPresenceObjectFactory& prObjFact =
        iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
    MXIMPIdentity* grantIdentity  = prfwObjFact.NewIdentityLC();
    grantIdentity->SetIdentityL( *iGroupName );

    MPresenceGrantRequestInfo* grantInfo  = prObjFact.NewPresenceGrantRequestInfoLC();
    grantInfo->SetRequestorIdL( grantIdentity );
    // we do not set the display name

    // issue the request to prfw
    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().HandlePresenceGrantRequestReceivedL(
        grantInfo );

    CleanupStack::Pop( 1 ); // blockIdentity
    CleanupStack::Pop( 1 ); // blockInfo

    // make the data available to the clients

    MXIMPDataSubscriptionState* dataSubscriptionState = prfwObjFact.NewDataSubscriptionStateLC();
    dataSubscriptionState->SetDataStateL( MXIMPDataSubscriptionState::EDataAvailable );
    dataSubscriptionState->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionActive );

    // change the state
    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().SetPresenceGrantRequestDataSubscriptionStateL(
        dataSubscriptionState, NULL );

    CleanupStack::Pop( 1 ); // dataSubscriptionState

    IMPS_DP( D_IMPS_LIT( "GrantPresenceForPresentityRequest Return Code %d" ), wvErrorCode );
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CGrantPresenceForPresentityRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::RunError Start %d" ), aError );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForPresentityRequest::GrantPresenceForPresentityL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForPresentityRequest::GrantPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::GrantPresenceForPresentityL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::GrantPresenceForPresentityL:PresentityId:%S " ), &aPresentityId.Identity() );

    //  IMPS_DP( D_IMPS_LIT( "CGrantPresenceForPresentityRequest::GrantPresenceForPresentityL:PresenceInfoFilter:%S "),&aPif);

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aPresentityId.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

    // fetch attributes from the pif and send it over to the server
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
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aPresentityId.Identity()
                                         ).EndTagL( KUserIDXMLTag );
        }
    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aPresentityId.Identity()
                                                      ).EndTagL( KUserIDXMLTag );
        }
    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "GrantPresenceForPresentityL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::GrantPresenceForPresentityL End" ) );
    }

// End of file
