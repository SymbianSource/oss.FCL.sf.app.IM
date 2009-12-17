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
#include "CUnsubscribePresentityPresence.h"
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
#include <ximpprotocolconnectionhost.h>
#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include "PImpsAdapXMLTools.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::CUnsubscribePresentityPresence
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresence::CUnsubscribePresentityPresence(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresentityPresence Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUnsubscribePresentityPresence End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::ConstructL
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresence::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::NewL
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresence* CUnsubscribePresentityPresence::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::NewL Start" ) );

    CUnsubscribePresentityPresence* self = new( ELeave )
    CUnsubscribePresentityPresence( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::NewLC
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresence* CUnsubscribePresentityPresence::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::NewLC Start" ) );

    CUnsubscribePresentityPresence* self =
        CUnsubscribePresentityPresence::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::~CUnsubscribePresentityPresence
// ---------------------------------------------------------------------------
//
CUnsubscribePresentityPresence::~CUnsubscribePresentityPresence()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUnsubscribePresentityPresence Start-End" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::DoCancel()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresence::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::RunL()
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresence::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    HBufC8* response = iConnMan.DataHandler().ResponseL( iSendId );
    CleanupStack::PushL( response );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    //Look for mandatory parts
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

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::RunError
// ---------------------------------------------------------------------------
//
TInt CUnsubscribePresentityPresence::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUnsubscribePresentityPresence::SubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
void CUnsubscribePresentityPresence::UnsubscribePresentityPresenceRequestL( const TDesC& aContactId )
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUnsubscribePresentityPresence::UnsubscribePresentityPresenceRequestL:PresentityId:%S " ), &aContactId );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupListRequest Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    // <UnsubscribePresence-Request>
    serializer->StartTagL( KUnsubscribePresence );

    // <UserID>
    // Contact ID
    // </UserID>
    serializer->StartTagL( KUserIDXMLTag
                         ).WvAddressL( aContactId
                                     ).EndTagL( KUserIDXMLTag );

    // </UnsubscribePresence-Request>
    serializer->EndTagL( KUnsubscribePresence );

    // </TransactionContent>
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy( ); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest End" ) );
    }

// End of file

