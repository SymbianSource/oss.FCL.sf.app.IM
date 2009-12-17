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
#include "CUnSubscribePresentityGroupMembersPresenceRequest.h"
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
#include "PImpsAdapXMLTools.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::CUnSubscribePresentityGroupMembersPresenceRequest
// ---------------------------------------------------------------------------
//
CUnSubscribePresentityGroupMembersPresenceRequest::CUnSubscribePresentityGroupMembersPresenceRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUnSubscribePresentityGroupMembersPresenceRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CUnSubscribePresentityGroupMembersPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CUnSubscribePresentityGroupMembersPresenceRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::NewL
// ---------------------------------------------------------------------------
//
CUnSubscribePresentityGroupMembersPresenceRequest* CUnSubscribePresentityGroupMembersPresenceRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::NewL Start" ) );

    CUnSubscribePresentityGroupMembersPresenceRequest* self = new( ELeave )
    CUnSubscribePresentityGroupMembersPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::NewLC
// ---------------------------------------------------------------------------
//
CUnSubscribePresentityGroupMembersPresenceRequest* CUnSubscribePresentityGroupMembersPresenceRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::NewLC Start" ) );

    CUnSubscribePresentityGroupMembersPresenceRequest* self = new( ELeave )
    CUnSubscribePresentityGroupMembersPresenceRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::~CUnSubscribePresentityGroupMembersPresenceRequest
// ---------------------------------------------------------------------------
//
CUnSubscribePresentityGroupMembersPresenceRequest::~CUnSubscribePresentityGroupMembersPresenceRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUnSubscribePresentityGroupMembersPresenceRequest Start" ) );

    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CUnSubscribePresentityGroupMembersPresenceRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnSubscribePresentityGroupMembersPresenceRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::RunL()
// ---------------------------------------------------------------------------
//
void CUnSubscribePresentityGroupMembersPresenceRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( " UnsubscribePresentityGroupMembersPresenceL wvErrorCode::%d" ), wvErrorCode );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CUnSubscribePresentityGroupMembersPresenceRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnSubscribePresentityGroupMembersPresenceRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnSubscribePresentityGroupMembersPresenceRequest::
// UnsubscribePresentityGroupMembersPresenceL
// ---------------------------------------------------------------------------
//
void CUnSubscribePresentityGroupMembersPresenceRequest::UnsubscribePresentityGroupMembersPresenceL( const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresentityGroupMembersPresenceL Start" ) );

    //iPtrbuffer.Set( iConnMan.DataHandler().TransferBufferL() );
    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "UnsubscribePresentityGroupMembersPresenceL Manufacture XML " ) );

    //serializer->StartTagL(KTransactionContent);
    //serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KUnsubscribePresence );

    // userId or contactlist Id or both are allowed
    serializer->StartTagL( KContactList
                         ).NarrowTextL( KWVXMLTag
                                      ).WvAddressL( aIdentity.Identity()
                                                  ).EndTagL( KContactList );

    serializer->EndTagL( KUnsubscribePresence );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UnsubscribePresentityGroupMembersPresenceL End" ) );
    }

// End of file
