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
#include "CPresenceAuthUserRequest.h"
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
// CPresenceAuthUserRequest::CPresenceAuthUserRequest
// ---------------------------------------------------------------------------
//
CPresenceAuthUserRequest::CPresenceAuthUserRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CPresenceAuthUserRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CPresenceAuthUserRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CPresenceAuthUserRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::NewL
// ---------------------------------------------------------------------------
//
CPresenceAuthUserRequest* CPresenceAuthUserRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::NewL Start" ) );

    CPresenceAuthUserRequest* self =
        new( ELeave ) CPresenceAuthUserRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::NewLC
// ---------------------------------------------------------------------------
//
CPresenceAuthUserRequest* CPresenceAuthUserRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::NewLC Start" ) );

    CPresenceAuthUserRequest* self =
        CPresenceAuthUserRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::~CPresenceAuthUserRequest
// ---------------------------------------------------------------------------
//
CPresenceAuthUserRequest::~CPresenceAuthUserRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::~CPresenceAuthUserRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iMemberid;
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::~CPresenceAuthUserRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresenceAuthUserRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::RunL()
// ---------------------------------------------------------------------------
//
void CPresenceAuthUserRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::RunL Start" ) );

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
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest Return Code %d" ), wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CPresenceAuthUserRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresenceAuthUserRequest::PresenceAuthUserRequest
// ---------------------------------------------------------------------------
//
void CPresenceAuthUserRequest::PresenceAuthUserRequestL(
    const MXIMPIdentity & aIdentity,
    const MPresenceInfoFilter & aPif )
    {
    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::PresenceAuthUserRequest Start" ) );


    iMemberid = aIdentity.Identity().AllocL();

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );
    serializer->StartTagL( KPresenceAuthUser );

    if ( KErrNone == iMemberid->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aIdentity.Identity()
                                         ).EndTagL( KUserIDXMLTag );
        }
    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aIdentity.Identity()
                                                      ).EndTagL( KUserIDXMLTag );
        }

    serializer->StartTagL( KAcceptance
                         ).NarrowTextL( KXMLValueTrue
                                      ).EndTagL( KAcceptance );

    serializer->EndTagL( KPresenceAuthUser );

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
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "PresenceAuthUserRequest" ) );

    CleanupStack::PopAndDestroy( 1 ); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CPresenceAuthUserRequest::PresenceAuthUserRequest End" ) );
    }

// End of file
