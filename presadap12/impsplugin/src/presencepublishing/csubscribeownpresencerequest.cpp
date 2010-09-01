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
#include "CSubscribeOwnPresenceRequest.h"
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
#include <servicepresenceinfo.h>
#include <personpresenceinfo.h>
#include <presenceinfofield.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceinfofieldvaluetext.h>
#include <servicepresenceinfo.h>
#include <personpresenceinfo.h>
#include "CPresenceNotificationHandler.h"


#include "PImpsAdapXMLTools.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::CSubscribeOwnPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceRequest::CSubscribeOwnPresenceRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribeOwnPresenceRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CSubscribeOwnPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceRequest* CSubscribeOwnPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::NewL Start" ) );

    CSubscribeOwnPresenceRequest* self = new( ELeave )
    CSubscribeOwnPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceRequest* CSubscribeOwnPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::NewLC Start" ) );

    CSubscribeOwnPresenceRequest* self =
        CSubscribeOwnPresenceRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::~CSubscribeOwnPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceRequest::~CSubscribeOwnPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribeOwnPresenceRequest Start" ) );

    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribeOwnPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

//	iResponse = iConnMan.DataHandler().ResponseL( iSendId );
    HBufC8* response = iConnMan.DataHandler().ResponseL( iSendId );
    CleanupStack::PushL( response );
    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    // Check if we got 200
    if ( iParser->DecodeL( *response, KResultXMLTag, ETrue ) )
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

    CleanupStack::PopAndDestroy( response );

    IMPS_DP( D_IMPS_LIT( " SubscribeOwnPresenceL::wvErrorCode %d" ), wvErrorCode );

    // Make request complete over here.
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribeOwnPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceRequest::SubscribeOwnPresenceL
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceRequest::SubscribeOwnPresenceL( const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribeOwnPresenceL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KSubscribePresence );
    //<User>
    // <UserID>
    // Contact ID
    // </UserID>
    //</User>
    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KUser );
    serializer->StartTagL( KUserIDXMLTag
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( iConnMan.UserName()
                                                  ).EndTagL( KUserIDXMLTag );
    serializer->EndTagL( KUser );
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

    serializer->StartTagL( KAutoSubscribe ).NarrowTextL( KXMLValueTrue ).EndTagL( KAutoSubscribe );

    serializer->EndTagL( KSubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::SubscribeOwnPresenceL End" ) );
    }

// End of file
