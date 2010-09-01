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
#include "CBlockPresenceForPresentityRequest.h"
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
#include "impsplugintags.h"

#include <ximpobjectfactory.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <presenceblockinfo.h>
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::CBlockPresenceForPresentityRequest
// ---------------------------------------------------------------------------
//
CBlockPresenceForPresentityRequest::CBlockPresenceForPresentityRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CBlockPresenceForPresentityRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CBlockPresenceForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CBlockPresenceForPresentityRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::NewL
// ---------------------------------------------------------------------------
//
CBlockPresenceForPresentityRequest* CBlockPresenceForPresentityRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::NewL Start" ) );

    CBlockPresenceForPresentityRequest* self = new( ELeave )
    CBlockPresenceForPresentityRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::NewLC
// ---------------------------------------------------------------------------
//
CBlockPresenceForPresentityRequest* CBlockPresenceForPresentityRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::NewLC Start" ) );

    CBlockPresenceForPresentityRequest* self =
        CBlockPresenceForPresentityRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::~CBlockPresenceForPresentityRequest
// ---------------------------------------------------------------------------
//
CBlockPresenceForPresentityRequest::~CBlockPresenceForPresentityRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CBlockPresenceForPresentityRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    IMPS_DP( D_IMPS_LIT( "::~CBlockPresenceForPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CBlockPresenceForPresentityRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::RunL()
// ---------------------------------------------------------------------------
//
void CBlockPresenceForPresentityRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::RunL Start" ) );

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

    // prfw does not provide any interface for MPresenceBlockInfo
    // write this interface in prfw and then compile this code

    MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
    MPresenceObjectFactory& prObjFact =
        iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
    MXIMPIdentity* blockIdentity  = prfwObjFact.NewIdentityLC();
    blockIdentity->SetIdentityL( *iGroupName );

    MPresenceBlockInfo* blockInfo  = prObjFact.NewPresenceBlockInfoLC();
    blockInfo->SetBlockedEntityIdL( blockIdentity );

    // issue the request to prfw
    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().HandlePresenceBlockedL( blockInfo );
    CleanupStack::Pop( 1 ); // blockIdentity
    CleanupStack::Pop( 1 ); // blockInfo

    MXIMPDataSubscriptionState* dataSubscriptionState = prfwObjFact.NewDataSubscriptionStateLC();
    dataSubscriptionState->SetDataStateL( MXIMPDataSubscriptionState::EDataAvailable );
    dataSubscriptionState->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionActive );

    // change the state
    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().SetPresenceBlockDataSubscriptionStateL(
        dataSubscriptionState, NULL );

    CleanupStack::Pop( 1 ); // dataSubscriptionState

    // update datacache
    // completet the group content request
    IMPS_DP( D_IMPS_LIT( "BlockPresenceForPresentityL Return Code %d" ), wvErrorCode );
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CBlockPresenceForPresentityRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CBlockPresenceForPresentityRequest::BlockPresenceForPresentityL
// ---------------------------------------------------------------------------
//
void CBlockPresenceForPresentityRequest::BlockPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId )
    {
    IMPS_DP( D_IMPS_LIT( "::BlockPresenceForPresentityL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CBlockPresenceForPresentityRequest::BlockPresenceForPresentityL:PresentityId:%S " ), &aPresentityId.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aPresentityId.Identity().AllocL();


    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );

    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );


    serializer->EndTagL( KPresenceSubList );

    if ( KErrNone == iGroupName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aPresentityId.Identity()
                                         ).EndTagL( KUserIDXMLTag );
        }
    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aPresentityId.Identity()
                                                      ).EndTagL( KUserIDXMLTag );
        }

    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueFalse ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );

    serializer->EndTagL( KTransactionContent );

    IMPS_DP( D_IMPS_LIT( "BlockPresenceForPresentityL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::BlockPresenceForPresentityL End" ) );
    }

// End of file
