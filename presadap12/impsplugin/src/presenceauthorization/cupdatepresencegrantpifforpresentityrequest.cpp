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
#include "CUpdatePresenceGrantPifForPresentityRequest.h"
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
#include <ximpprotocolconnectionhost.h>
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
// CUpdatePresenceGrantPifForPresentityRequest::CUpdatePresenceGrantPifForPresentityRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityRequest::CUpdatePresenceGrantPifForPresentityRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForPresentityRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityRequest* CUpdatePresenceGrantPifForPresentityRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::NewL Start" ) );

    CUpdatePresenceGrantPifForPresentityRequest* self = new( ELeave )
    CUpdatePresenceGrantPifForPresentityRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityRequest* CUpdatePresenceGrantPifForPresentityRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::NewLC Start" ) );

    CUpdatePresenceGrantPifForPresentityRequest* self =
        CUpdatePresenceGrantPifForPresentityRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::~CUpdatePresenceGrantPifForPresentityRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForPresentityRequest::~CUpdatePresenceGrantPifForPresentityRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresenceGrantPifForPresentityRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresenceGrantPifForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::RunL Start" ) );

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


    IMPS_DP( D_IMPS_LIT( "UpdatePresenceGrantPifForPresentityRequest %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresenceGrantPifForPresentityRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForPresentityRequest::UpdatePresenceGrantPifForPresentityL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForPresentityRequest::UpdatePresenceGrantPifForPresentityL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForPresentityL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::UpdatePresenceGrantPifForPresentityL:PresentityId:%S " ), &aPresentityId.Identity() );

//   IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForPresentityRequest::UpdatePresenceGrantPifForPresentityL:PresenceInfoFilter:%S "),&aPif);

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aPresentityId.Identity().AllocL();

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

    serializer->StartTagL( KUserIDXMLTag
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( aPresentityId.Identity()
                                                  ).EndTagL( KUserIDXMLTag );

    // all attributes are targeted as default
    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "UpdatePresenceGrantPifForPresentityL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForPresentityL End" ) );
    }

// End of file
