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
#include "CUpdatePresentityPresence.h"
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
#include "PImpsAdapXMLTools.h"
#include <presenceinfofilter.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::CUpdatePresentityPresence
// ---------------------------------------------------------------------------
//
CUpdatePresentityPresence::CUpdatePresentityPresence(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityPresence Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CUpdatePresentityPresence End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::ConstructL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityPresence::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    //CleanupStack::Pop( iParser );

    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::NewL
// ---------------------------------------------------------------------------
//
CUpdatePresentityPresence* CUpdatePresentityPresence::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::NewL Start" ) );

    CUpdatePresentityPresence* self =
        new( ELeave ) CUpdatePresentityPresence( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::NewLC
// ---------------------------------------------------------------------------
//
CUpdatePresentityPresence* CUpdatePresentityPresence::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::NewLC Start" ) );

    CUpdatePresentityPresence* self =
        CUpdatePresentityPresence::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::~CUpdatePresentityPresence
// ---------------------------------------------------------------------------
//
CUpdatePresentityPresence::~CUpdatePresentityPresence()
    {
    IMPS_DP( D_IMPS_LIT( "::~CUpdatePresentityPresence Start-End" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::DoCancel()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityPresence::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::RunL()
// ---------------------------------------------------------------------------
//
void CUpdatePresentityPresence::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::RunL Start" ) );

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
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::RunError
// ---------------------------------------------------------------------------
//
TInt CUpdatePresentityPresence::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpdatePresentityPresence::UpdatePresentityPresenceRequestL
// ---------------------------------------------------------------------------
//
void CUpdatePresentityPresence::UpdatePresentityPresenceRequestL(
    const TDesC& aContactId, const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityPresenceRequestL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CUpdatePresentityPresence::UpdatePresentityPresenceRequestL:ContactId:%S " ), &aContactId );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    IMPS_DP( D_IMPS_LIT( "UpdatePresentityPresenceRequestL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    // <SubscribePresence-Request>
    serializer->StartTagL( KSubscribePresence );

    // <UserID>
    // Contact ID
    // </UserID>
    serializer->StartTagL( KUserIDXMLTag
                         ).WvAddressL( aContactId
                                     ).EndTagL( KUserIDXMLTag );


    // <PresenceSubList
    serializer->StartTagL( KPresenceSubList );
    // xmlns="http://www.openmobilealliance.org/DTD/WV-PA1.2"
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
    // </PresenceSubList>
    serializer->EndTagL( KPresenceSubList );

    // <AutoSubscribe>T</AutoSubscribe>
    serializer->StartTagL( KAutoSubscribe ).
    NarrowTextL( KXMLValueTrue ).
    EndTagL( KAutoSubscribe );

    // </SubscribePresence-Request>
    serializer->EndTagL( KSubscribePresence );

    // </TransactionContent>
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy( ); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::UpdatePresentityPresenceRequestL End" ) );
    }

// End of file

