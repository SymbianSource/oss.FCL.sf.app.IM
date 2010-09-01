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
#include "CSubscribePresentityPresenceRequest.h"
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
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <presenceinfofilter.h>
#include "PImpsAdapXMLTools.h"
#include "CPresenceNotificationHandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::CSubscribePresentityPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresenceRequest::CSubscribePresentityPresenceRequest( MImpsPrtPluginConnectionManager& aConnMan,
                                                                          TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::CSubscribePresentityPresenceRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::CSubscribePresentityPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    iConnMan.PresNotifier()->StartListeningL();


    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresenceRequest* CSubscribePresentityPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::NewL Start" ) );

    CSubscribePresentityPresenceRequest* self = new( ELeave )
    CSubscribePresentityPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresenceRequest* CSubscribePresentityPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::NewLC Start" ) );

    CSubscribePresentityPresenceRequest* self = new( ELeave )
    CSubscribePresentityPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::~CSubscribePresentityPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresenceRequest::~CSubscribePresentityPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityPresenceRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::RunL Start" ) );

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

    // we have to process notifications for this from the server

    IMPS_DP( D_IMPS_LIT( " SubscribePresentityPresenceL::wvErrorCode %d" ), wvErrorCode );

    // Make request complete over here.
    if ( wvErrorCode == 200 )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresentityPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityPresenceRequest::SubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresenceRequest::SubscribePresentityPresenceL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::SubscribePresentityPresenceL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP( D_IMPS_LIT( "SubscribePresentityPresenceL Manufacture XML " ) );

    //serializer->StartTagL(KTransactionContent);
    //serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
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
                                      ).WvAddressL( aIdentity.Identity()
                                                  ).EndTagL( KUserIDXMLTag );
    serializer->EndTagL( KUser );
    // if attribute list is not present all attributes are requested
    serializer->StartTagL( KAutoSubscribe ).NarrowTextL( KXMLValueTrue ).EndTagL( KAutoSubscribe );

    serializer->EndTagL( KSubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresenceRequest::SubscribePresentityPresenceL End" ) );
    }

// End of file
