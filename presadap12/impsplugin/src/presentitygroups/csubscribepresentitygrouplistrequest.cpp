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
#include "csubscribepresentitygrouplistrequest.h"
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
#include <presenceobjectfactory.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <ximpdatasubscriptionstate.h>
#include <protocolpresencedatahost.h>
#include "PImpsAdapXMLTools.h"




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::CSubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupListRequest::CSubscribePresentityGroupListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :

        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityGroupListRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityGroupListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupListRequest* CSubscribePresentityGroupListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::NewL Start" ) );

    CSubscribePresentityGroupListRequest* self = new( ELeave )
    CSubscribePresentityGroupListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupListRequest* CSubscribePresentityGroupListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::NewLC Start" ) );

    CSubscribePresentityGroupListRequest* self =
        CSubscribePresentityGroupListRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::~CSubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupListRequest::~CSubscribePresentityGroupListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityGroupListRequest Start" ) );

    delete iResponse;
    delete iListId;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityGroupListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    MXIMPObjectFactory& objFact = iConnMan.HandleToHost().ObjectFactory();
    MPresenceObjectFactory& prObjFact = iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
    MXIMPObjectCollection* listOfLists = objFact.NewObjectCollectionLC();

    IMPS_DP( D_IMPS_LIT( " listOfLists::%x" ), &listOfLists );
    IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

    if ( iParser->DecodeL( *iResponse, KContactList, EFalse ) )
        {

        TInt countResults( iParser->Count() );

        IMPS_DP( D_IMPS_LIT( " COUNT RESULT::%d" ), countResults );

        for ( TInt i = 0 ; i < countResults; ++i )
            {
            // convert to 16 unicode
            //HBufC16* listId = iParser->ResultAsUnicodeTextL( i );
            iListId = iParser->ResultAsUnicodeTextL( i );
            iListId->Des().Fold();

            IMPS_DP( D_IMPS_LIT( " iResponse::%x" ), &iListId );

            MPresentityGroupInfo* groupInfo = prObjFact.NewPresentityGroupInfoLC();

            IMPS_DP( D_IMPS_LIT( " groupInfo::%x" ), &groupInfo );
            MXIMPIdentity* groupIdentity = objFact.NewIdentityLC();
            IMPS_DP( D_IMPS_LIT( " groupIdentity::%x" ), &groupIdentity );

            groupIdentity->SetIdentityL( *iListId );
            groupInfo->SetGroupIdL( groupIdentity );

            listOfLists->AddObjectL( groupInfo );

            delete iListId;
            iListId = NULL;
            CleanupStack::Pop( 2 ); // groupInfo, groupIdentity
            }

        }
    if ( iParser->DecodeL( *iResponse, KDefaultContactList, EFalse ) )
        {
        iListId = iParser->ResultAsUnicodeTextL();
        iListId->Des().Fold();
        MPresentityGroupInfo* groupInfo = prObjFact.NewPresentityGroupInfoLC();
        MXIMPIdentity* groupIdentity = objFact.NewIdentityLC();
        groupIdentity->SetIdentityL( *iListId );
        groupInfo->SetGroupIdL( groupIdentity );
        listOfLists->AddObjectL( groupInfo );

        delete iListId;
        iListId = NULL;
        CleanupStack::Pop( 2 ); // groupInfo, groupIdentity
        }

    // generate internal request to the prfw
    iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupListL( listOfLists );
    CleanupStack::Pop(); //listOfLists

    MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();

    // issue request to prfw such that client can get updated events

    MXIMPDataSubscriptionState* dataSubscriptionState = prfwObjFact.NewDataSubscriptionStateLC();
    dataSubscriptionState->SetDataStateL( MXIMPDataSubscriptionState::EDataAvailable );
    dataSubscriptionState->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionActive );

    iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().SetPresentityGroupListDataSubscriptionStateL(
        dataSubscriptionState, NULL );
    CleanupStack::Pop(); //dataSubscriptionState

    // complete the request to prfw
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresentityGroupListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequestL()
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupListRequest Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );
    serializer->StartTagL( KGetListOfList ).EndTagL( KGetListOfList );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupListRequest End" ) );
    }

// End of file
