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
#include "CUpdateOwnPresenceSubscriptionPifRequest.h"
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
// CUpdateOwnPresenceSubscriptionPifRequest::CUpdateOwnPresenceSubscriptionPifRequest
// ---------------------------------------------------------------------------
//
CUpdateOwnPresenceSubscriptionPifRequest::CUpdateOwnPresenceSubscriptionPifRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdateOwnPresenceSubscriptionPifRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CUpdateOwnPresenceSubscriptionPifRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdateOwnPresenceSubscriptionPifRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::ConstructL Start" ) );

    iParser = CreateXMLParserL();

    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::NewL
// ---------------------------------------------------------------------------
//
CUpdateOwnPresenceSubscriptionPifRequest* CUpdateOwnPresenceSubscriptionPifRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::NewL Start" ) );

    CUpdateOwnPresenceSubscriptionPifRequest* self =
        new( ELeave ) CUpdateOwnPresenceSubscriptionPifRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::NewLC
// ---------------------------------------------------------------------------
//
CUpdateOwnPresenceSubscriptionPifRequest* CUpdateOwnPresenceSubscriptionPifRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::NewLC Start" ) );

    CUpdateOwnPresenceSubscriptionPifRequest* self =
        CUpdateOwnPresenceSubscriptionPifRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::~CUpdateOwnPresenceSubscriptionPifRequest
// ---------------------------------------------------------------------------
//
CUpdateOwnPresenceSubscriptionPifRequest::~CUpdateOwnPresenceSubscriptionPifRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdateOwnPresenceSubscriptionPifRequest Start" ) );

    delete iResponse;
    delete iListId;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CUpdateOwnPresenceSubscriptionPifRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdateOwnPresenceSubscriptionPifRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUpdateOwnPresenceSubscriptionPifRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::RunL Start" ) );

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

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );
    // handle notifications.

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdateOwnPresenceSubscriptionPifRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdateOwnPresenceSubscriptionPifRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdateOwnPresenceSubscriptionPifRequest::UpdateOwnPresenceSubscriptionPifL
// ---------------------------------------------------------------------------
//
void CUpdateOwnPresenceSubscriptionPifRequest::UpdateOwnPresenceSubscriptionPifL(
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdateOwnPresenceSubscriptionPifL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KSubscribePresence );

    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KUserIDXMLTag
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( iConnMan.UserName()
                                                  ).EndTagL( KUserIDXMLTag );

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

    serializer->StartTagL( KAutoSubscribe ).NarrowTextL( KXMLValueTrue ).EndTagL( KAutoSubscribe );

    serializer->EndTagL( KSubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdateOwnPresenceSubscriptionPifL End" ) );
    }

// End of file
