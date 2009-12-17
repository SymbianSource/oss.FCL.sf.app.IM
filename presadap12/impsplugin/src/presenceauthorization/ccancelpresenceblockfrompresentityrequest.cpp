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
#include "CCancelPresenceBlockFromPresentityRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include <protocolpresenceauthorizationdatahost.h>

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
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::CCancelPresenceBlockFromPresentityRequest
// ---------------------------------------------------------------------------
//
CCancelPresenceBlockFromPresentityRequest::CCancelPresenceBlockFromPresentityRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CCancelPresenceBlockFromPresentityRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CCancelPresenceBlockFromPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CCancelPresenceBlockFromPresentityRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::NewL
// ---------------------------------------------------------------------------
//
CCancelPresenceBlockFromPresentityRequest* CCancelPresenceBlockFromPresentityRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::NewL Start" ) );

    CCancelPresenceBlockFromPresentityRequest* self = new( ELeave )
    CCancelPresenceBlockFromPresentityRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::NewLC
// ---------------------------------------------------------------------------
//
CCancelPresenceBlockFromPresentityRequest* CCancelPresenceBlockFromPresentityRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::NewLC Start" ) );

    CCancelPresenceBlockFromPresentityRequest* self = CCancelPresenceBlockFromPresentityRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::~CCancelPresenceBlockFromPresentityRequest
// ---------------------------------------------------------------------------
//
CCancelPresenceBlockFromPresentityRequest::~CCancelPresenceBlockFromPresentityRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CCancelPresenceBlockFromPresentityRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;

    IMPS_DP( D_IMPS_LIT( "::~CCancelPresenceBlockFromPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CCancelPresenceBlockFromPresentityRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::RunL()
// ---------------------------------------------------------------------------
//
void CCancelPresenceBlockFromPresentityRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::RunL Start" ) );

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
    // update data cache
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest Return Code %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CCancelPresenceBlockFromPresentityRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCancelPresenceBlockFromPresentityRequest::CancelPresenceBlockFromPresentityL
// ---------------------------------------------------------------------------
//
void CCancelPresenceBlockFromPresentityRequest::CancelPresenceBlockFromPresentityL(
    const MXIMPIdentity& aPresentityId )
    {
    IMPS_DP( D_IMPS_LIT( "::CancelPresenceBlockFromPresentityL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CCancelPresenceBlockFromPresentityRequest::CancelPresenceBlockFromPresentityL:PresentityId:%S " ), &aPresentityId.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aPresentityId.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );

    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );
    serializer->StartTagL( KUserAvailabilityXMLTag ).EndTagL( KUserAvailabilityXMLTag );
    serializer->StartTagL( KStatusMood ).EndTagL( KStatusMood );
    serializer->StartTagL( KOnlineStatusXMLTag ).EndTagL( KOnlineStatusXMLTag );
    serializer->StartTagL( KStatusTextXMLTag ).EndTagL( KStatusTextXMLTag );
    serializer->StartTagL( KStatusContentXMLTag ).EndTagL( KStatusContentXMLTag );
    serializer->EndTagL( KPresenceSubList );

    // UserID does not have wv: tag
    serializer->StartTagL( KUserIDXMLTag
                           //).NarrowTextL(KWVXMLTag
                         ).WvAddressL( aPresentityId.Identity()
                                     ).EndTagL( KUserIDXMLTag );

    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueFalse ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );
    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "CancelPresenceBlockFromPresentityL Before SendData" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::CancelPresenceBlockFromPresentityL End" ) );
    }

// End of file
