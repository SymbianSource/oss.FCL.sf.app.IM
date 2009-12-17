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
#include "CSubscribePresentityGroupMembersPresenceRequest.h"
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
// CSubscribePresentityGroupMembersPresenceRequest::CSubscribePresentityGroupMembersPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupMembersPresenceRequest::CSubscribePresentityGroupMembersPresenceRequest( MImpsPrtPluginConnectionManager& aConnMan,
        TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
        //iPtrbuffer( NULL, 0 )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::CSubscribePresentityGroupMembersPresenceRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::CSubscribePresentityGroupMembersPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupMembersPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    iConnMan.PresNotifier()->StartListeningL();

    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupMembersPresenceRequest* CSubscribePresentityGroupMembersPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::NewL Start" ) );

    CSubscribePresentityGroupMembersPresenceRequest* self = new( ELeave )
    CSubscribePresentityGroupMembersPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupMembersPresenceRequest* CSubscribePresentityGroupMembersPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::NewLC Start" ) );

    CSubscribePresentityGroupMembersPresenceRequest* self = new( ELeave )
    CSubscribePresentityGroupMembersPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::~CSubscribePresentityGroupMembersPresenceRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupMembersPresenceRequest::~CSubscribePresentityGroupMembersPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::~CSubscribePresentityGroupMembersPresenceRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::~CSubscribePresentityGroupMembersPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupMembersPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupMembersPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( " SubscribePresentityGroupMembersPresenceL wvErrorCode::%d" ), wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresentityGroupMembersPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupMembersPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupMembersPresenceRequest::SubscribePresentityGroupMembersPresenceL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupMembersPresenceRequest::SubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aIdentity,
    const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupMembersPresenceL Start" ) );

    //iPtrbuffer.Set( iConnMan.DataHandler().TransferBufferL() );
    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    IMPSPRTPLUGIN_UNUSED_PARAM( aPif );

    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupMembersPresenceL Manufacture XML " ) );

    //serializer->StartTagL(KTransactionContent);
    //serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );
    serializer->StartTagL( KSubscribePresence );

    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KContactList
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( aIdentity.Identity()
                                                  ).EndTagL( KContactList );

    // if attribute list is not present all attributes are requested
    serializer->StartTagL( KAutoSubscribe ).NarrowTextL( KXMLValueTrue ).EndTagL( KAutoSubscribe );

    serializer->EndTagL( KSubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupMembersPresenceL End" ) );
    }

// End of file
