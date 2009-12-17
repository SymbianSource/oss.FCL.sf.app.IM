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
#include "CUpdatePresenceGrantPifForEveryoneRequest.h"
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
// CUpdatePresenceGrantPifForEveryoneRequest::CUpdatePresenceGrantPifForEveryoneRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForEveryoneRequest::CUpdatePresenceGrantPifForEveryoneRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForEveryoneRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresenceGrantPifForEveryoneRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForEveryoneRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForEveryoneRequest* CUpdatePresenceGrantPifForEveryoneRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::NewL Start" ) );

    CUpdatePresenceGrantPifForEveryoneRequest* self = new( ELeave )
    CUpdatePresenceGrantPifForEveryoneRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForEveryoneRequest* CUpdatePresenceGrantPifForEveryoneRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::NewLC Start" ) );

    CUpdatePresenceGrantPifForEveryoneRequest* self =
        CUpdatePresenceGrantPifForEveryoneRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::~CUpdatePresenceGrantPifForEveryoneRequest
// ---------------------------------------------------------------------------
//
CUpdatePresenceGrantPifForEveryoneRequest::~CUpdatePresenceGrantPifForEveryoneRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresenceGrantPifForEveryoneRequest Start-End" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForEveryoneRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForEveryoneRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::RunL Start" ) );

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

    // completet the group content request
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest Return Code %d" ),
             wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresenceGrantPifForEveryoneRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdatePresenceGrantPifForEveryoneRequest::UpdatePresenceGrantPifForEveryoneL
// ---------------------------------------------------------------------------
//
void CUpdatePresenceGrantPifForEveryoneRequest::UpdatePresenceGrantPifForEveryoneL(
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForEveryoneL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresenceGrantPifForEveryoneRequest::UpdatePresenceGrantPifForEveryoneL:PresenceInfoFilter:%S " ), &aPif );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

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
    // empty list so that all attributes
    serializer->EndTagL( KPresenceSubList );

    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueTrue ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );



    IMPS_DP( D_IMPS_LIT( "UpdatePresenceGrantPifForEveryoneL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdatePresenceGrantPifForEveryoneL End" ) );
    }

// End of file
