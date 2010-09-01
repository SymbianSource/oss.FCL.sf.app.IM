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
#include "CSubscribeOwnPresenceNotificationRequest.h"
#include "impsconnectionmanagercontrolif.h"
#include "impsdebugprint.h"
#include "CImpsPluginAccessHandler.h"
#include "CImpsPluginPureDataHandler.h"
#include "impsplugintags.h"

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
#include <protocolpresencepublishingdatahost.h>


#include <personpresenceinfo.h>
#include <servicepresenceinfo.h>
#include <devicepresenceinfo.h>
#include <presenceinfo.h>



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::CSubscribeOwnPresenceNotificationRequest
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceNotificationRequest::CSubscribeOwnPresenceNotificationRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribeOwnPresenceNotificationRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CSubscribeOwnPresenceNotificationRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceNotificationRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceNotificationRequest* CSubscribeOwnPresenceNotificationRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::NewL Start" ) );

    CSubscribeOwnPresenceNotificationRequest* self = new( ELeave )
    CSubscribeOwnPresenceNotificationRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceNotificationRequest* CSubscribeOwnPresenceNotificationRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::NewLC Start" ) );

    CSubscribeOwnPresenceNotificationRequest* self =
        CSubscribeOwnPresenceNotificationRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::~CSubscribeOwnPresenceNotificationRequest
// ---------------------------------------------------------------------------
//
CSubscribeOwnPresenceNotificationRequest::~CSubscribeOwnPresenceNotificationRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribeOwnPresenceNotificationRequest Start" ) );

    delete iResponse;
    delete iListId;
    Cancel();
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribeOwnPresenceNotificationRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceNotificationRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelListening();

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceNotificationRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    // get the incoming response from the server with transaction id equals zero
    iResponse = iConnMan.DataHandler().ResponseL( 0 );

    // Check if we got Notification Request
    if ( iParser->DecodeL( *iResponse, KPresenceNotificationRequest, ETrue ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got Notification Request From Server" ) );
        }

    if ( iParser->DecodeL( *iResponse, KPresenceXMLTag, EFalse ) )
        {

        TInt countResults( iParser->Count() );
        IMPS_DP( D_IMPS_LIT( " Got Presence Tag %d" ), countResults );
        }
    // is user tag present in the result?
    if ( iParser->DecodeL( *iResponse, KUserTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got UserID Tag " ) );
        }

    // is PresenceSubList tag present in the result?
    if ( iParser->DecodeL( *iResponse, KPresenceSubList, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got PresenceSubList Tag " ) );
        }

    // is PresenceSubList tag present in the result?
    if ( iParser->DecodeL( *iResponse, KOnlineStatusXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KOnlineStatusXMLTag Tag " ) );
        }


    // is PresenceSubList tag present in the result?
    if ( iParser->DecodeL( *iResponse, KRegistrationXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KOnlineStatusXMLTag Tag " ) );
        }


    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KTimeZoneXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KGeoLocationXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    // is KClientInfoXMLTag tag present in the result?
    if ( iParser->DecodeL( *iResponse, KClientInfoXMLTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Got KClientInfoXMLTag Tag " ) );
        }

    iConnMan.DataHandler().ListenIncomingData( iStatus );

    //delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribeOwnPresenceNotificationRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::MakeSubscribeOwnPresenceNotificationActiveL
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceNotificationRequest::MakeSubscribeOwnPresenceNotificationActiveL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::MakeSubscribeOwnPresenceNotificationActiveL Start" ) );

    // Ready to receive notification from the server

    // signal the scheduler
    TInt count( iConnMan.DataHandler().ListenIncomingData( iStatus ) );
    SetActive();

    for ( TInt x( 0 ) ;  x < count ; ++x )
        {
        HandleIncomingDataL();
        }

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::MakeSubscribeOwnPresenceNotificationActiveL End" ) );
    }

// ---------------------------------------------------------------------------
// CSubscribeOwnPresenceNotificationRequest::HandleIncomingDataL
// ---------------------------------------------------------------------------
//
void CSubscribeOwnPresenceNotificationRequest::HandleIncomingDataL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::HandleIncomingDataL Start" ) );

    // first get new data from the pure data handler
    HBufC8* newData;
    newData = iConnMan.DataHandler().ResponseL( 0 ); // zero is incoming Data
    if ( !newData )
        {
        MakeSubscribeOwnPresenceNotificationActiveL();
        return;
        }
    CleanupStack::PushL( newData );
    // do some processing here
    RunL();
    CleanupStack::PopAndDestroy(); // newData

    IMPS_DP( D_IMPS_LIT( "CSubscribeOwnPresenceNotificationRequest::HandleIncomingDataL End" ) );
    }

// End of file
