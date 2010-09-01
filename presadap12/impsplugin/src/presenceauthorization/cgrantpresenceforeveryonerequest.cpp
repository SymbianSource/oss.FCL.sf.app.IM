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
#include "CGrantPresenceForEveryoneRequest.h"
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
// CGrantPresenceForEveryoneRequest::CGrantPresenceForEveryoneRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForEveryoneRequest::CGrantPresenceForEveryoneRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForEveryoneRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CGrantPresenceForEveryoneRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForEveryoneRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::ConstructL Start" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::NewL
// ---------------------------------------------------------------------------
//
CGrantPresenceForEveryoneRequest* CGrantPresenceForEveryoneRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::NewL Start" ) );

    CGrantPresenceForEveryoneRequest* self = new( ELeave )
    CGrantPresenceForEveryoneRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::NewLC
// ---------------------------------------------------------------------------
//
CGrantPresenceForEveryoneRequest* CGrantPresenceForEveryoneRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::NewLC Start" ) );

    CGrantPresenceForEveryoneRequest* self =
        CGrantPresenceForEveryoneRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::~CGrantPresenceForEveryoneRequest
// ---------------------------------------------------------------------------
//
CGrantPresenceForEveryoneRequest::~CGrantPresenceForEveryoneRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForEveryoneRequest Start-End" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CGrantPresenceForEveryoneRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForEveryoneRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::RunL()
// ---------------------------------------------------------------------------
//
void CGrantPresenceForEveryoneRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( "GrantPresenceForEveryoneL Return Code %d" ), wvErrorCode );
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CGrantPresenceForEveryoneRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CGrantPresenceForEveryoneRequest::GrantPresenceForEveryoneL
// ---------------------------------------------------------------------------
//
void CGrantPresenceForEveryoneRequest::GrantPresenceForEveryoneL(
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::GrantPresenceForEveryoneL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

//   IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::GrantPresenceForEveryoneL:PresenceInfoFilter:%S "),&aPif);

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

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

    IMPS_DP( D_IMPS_LIT( "GrantPresenceForEveryoneL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CGrantPresenceForEveryoneRequest::GrantPresenceForEveryoneL End" ) );
    }

// End of file
