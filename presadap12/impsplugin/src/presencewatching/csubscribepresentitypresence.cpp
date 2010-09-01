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
#include "CSubscribePresentityPresence.h"
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
#include "PImpsAdapXMLTools.h"
#include <servicepresenceinfo.h>
#include <personpresenceinfo.h>
#include <presenceinfofield.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceinfofieldvaluetext.h>
#include <ximpidentity.h>
#include "CPresenceNotificationHandler.h"
#include <presenceinfofilter.h>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::CSubscribePresentityPresence
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresence::CSubscribePresentityPresence(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityPresence Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityPresence End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresence::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();


    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresence* CSubscribePresentityPresence::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::NewL Start" ) );

    CSubscribePresentityPresence* self = new( ELeave )
    CSubscribePresentityPresence( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresence* CSubscribePresentityPresence::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::NewLC Start" ) );

    CSubscribePresentityPresence* self =
        CSubscribePresentityPresence::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::~CSubscribePresentityPresence
// ---------------------------------------------------------------------------
//
CSubscribePresentityPresence::~CSubscribePresentityPresence()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityPresence Start-End" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresence::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresence::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::RunL Start" ) );

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
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::RunL End %d" ), wvErrorCode );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresentityPresence::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityPresence::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityPresence::SubscribePresentityPresenceRequestL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityPresence::SubscribePresentityPresenceRequestL(
    const TDesC& aContactId, const MPresenceInfoFilter& aPif )
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupListRequest Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    // <SubscribePresence-Request>
    serializer->StartTagL( KSubscribePresence );

    //<User>
    // <UserID>
    // Contact ID
    // </UserID>
    //</User>
    serializer->StartTagL( KUser );
    serializer->StartTagL( KUserIDXMLTag
                         ).WvAddressL( aContactId
                                     ).EndTagL( KUserIDXMLTag );
    serializer->EndTagL( KUser );

    // <PresenceSubList
    serializer->StartTagL( KPresenceSubList );
    // xmlns="http://www.openmobilealliance.org/DTD/WV-PA1.2"
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

    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest End" ) );
    }

// End of file

