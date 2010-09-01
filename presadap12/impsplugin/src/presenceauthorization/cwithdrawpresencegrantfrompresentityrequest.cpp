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
#include "CWithdrawPresenceGrantFromPresentityRequest.h"
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
#include <ximpprotocolconnectionhost.h>
#include <ximpobjectfactory.h>
#include <protocolpresencedatahost.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::CWithdrawPresenceGrantFromPresentityRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityRequest::CWithdrawPresenceGrantFromPresentityRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "Constructor::CWithdrawPresenceGrantFromPresentityRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "Constructor::CWithdrawPresenceGrantFromPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::NewL
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityRequest* CWithdrawPresenceGrantFromPresentityRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::NewL Start" ) );

    CWithdrawPresenceGrantFromPresentityRequest* self = new( ELeave )
    CWithdrawPresenceGrantFromPresentityRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::NewLC
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityRequest* CWithdrawPresenceGrantFromPresentityRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::NewLC Start" ) );

    CWithdrawPresenceGrantFromPresentityRequest* self =
        CWithdrawPresenceGrantFromPresentityRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::~CWithdrawPresenceGrantFromPresentityRequest
// ---------------------------------------------------------------------------
//
CWithdrawPresenceGrantFromPresentityRequest::~CWithdrawPresenceGrantFromPresentityRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CWithdrawPresenceGrantFromPresentityRequest Start" ) );
    delete iResponse;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iUserId;
    IMPS_DP( D_IMPS_LIT( "::~CWithdrawPresenceGrantFromPresentityRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::RunL()
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::RunL Start" ) );

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

    MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
    MXIMPIdentity* withdrawGrantIdentity  = prfwObjFact.NewIdentityLC();
    withdrawGrantIdentity->SetIdentityL( *iGroupName );

    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().HandlePresenceGrantRequestObsoletedL(
        withdrawGrantIdentity );

    CleanupStack::Pop( 1 ); // withdrawGrantIdentity

    MXIMPDataSubscriptionState* dataSubscriptionState = prfwObjFact.NewDataSubscriptionStateLC();
    dataSubscriptionState->SetDataStateL( MXIMPDataSubscriptionState::EDataAvailable );
    dataSubscriptionState->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionActive );

    // change the state
    iConnMan.HandleToHost().ProtocolPresenceDataHost().AuthorizationDataHost().SetPresenceGrantRequestDataSubscriptionStateL(
        dataSubscriptionState, NULL );

    CleanupStack::Pop( 1 ); // dataSubscriptionState

    IMPS_DP( D_IMPS_LIT( "WithdrawPresenceGrantFromPresentityL Return Code %d" ), wvErrorCode );
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CWithdrawPresenceGrantFromPresentityRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWithdrawPresenceGrantFromPresentityRequest::WithdrawPresenceGrantFromPresentityL
// ---------------------------------------------------------------------------
//
void CWithdrawPresenceGrantFromPresentityRequest::WithdrawPresenceGrantFromPresentityL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromPresentityL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CWithdrawPresenceGrantFromPresentityRequest::WithdrawPresenceGrantFromPresentityL:PresentityId:%S " ), &aIdentity.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    // when we withdraw append to withdraw array
    iGroupName = aIdentity.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KCreateAttributeList );
    serializer->StartTagL( KPresenceSubList );
    serializer->AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );

    serializer->EndTagL( KPresenceSubList );
    if ( KErrNone == iGroupName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).WvAddressL( aIdentity.Identity()
                                         ).EndTagL( KUserIDXMLTag );
        }


    else
        {
        serializer->StartTagL( KUserIDXMLTag
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aIdentity.Identity()
                                                      ).EndTagL( KUserIDXMLTag );
        }

    serializer->StartTagL( KDefaultList ).NarrowTextL( KXMLValueFalse ).EndTagL( KDefaultList );
    serializer->EndTagL( KCreateAttributeList );

    serializer->EndTagL( KTransactionContent );


    IMPS_DP( D_IMPS_LIT( "WithdrawPresenceGrantFromPresentityL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::WithdrawPresenceGrantFromPresentityL End" ) );
    }

// End of file
