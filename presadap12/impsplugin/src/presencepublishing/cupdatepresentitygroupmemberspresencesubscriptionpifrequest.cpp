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
#include "CImpsPluginGroupSubscribePresentityGroupListRequest.h"
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




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::CImpsPluginGroupSubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
CImpsPluginGroupSubscribePresentityGroupListRequest::CImpsPluginGroupSubscribePresentityGroupListRequest( MImpsPrtPluginConnectionManager& aConnMan,
        TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
        //iPtrbuffer( NULL, 0 )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::CImpsPluginGroupSubscribePresentityGroupListRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::CImpsPluginGroupSubscribePresentityGroupListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupSubscribePresentityGroupListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::NewL
// ---------------------------------------------------------------------------
//
CImpsPluginGroupSubscribePresentityGroupListRequest* CImpsPluginGroupSubscribePresentityGroupListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::NewL Start" ) );

    CImpsPluginGroupSubscribePresentityGroupListRequest* self = new( ELeave )
    CImpsPluginGroupSubscribePresentityGroupListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::NewLC
// ---------------------------------------------------------------------------
//
CImpsPluginGroupSubscribePresentityGroupListRequest* CImpsPluginGroupSubscribePresentityGroupListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::NewLC Start" ) );

    CImpsPluginGroupSubscribePresentityGroupListRequest* self = new( ELeave )
    CImpsPluginGroupSubscribePresentityGroupListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::~CImpsPluginGroupSubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
CImpsPluginGroupSubscribePresentityGroupListRequest::~CImpsPluginGroupSubscribePresentityGroupListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::~CImpsPluginGroupSubscribePresentityGroupListRequest Start" ) );

    delete iResponse;
    delete iListId;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::~CImpsPluginGroupSubscribePresentityGroupListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupSubscribePresentityGroupListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupSubscribePresentityGroupListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    MXIMPObjectFactory& objFact = iConnMan.HandleToHost().ObjectFactory();
    MXIMPObjectCollection* listOfLists = objFact.NewObjectCollectionLC();

    IMPS_DP( D_IMPS_LIT( " listOfLists::%x" ), &listOfLists );

    if ( iParser->DecodeL( *iResponse, KContactList, EFalse ) )
        {

        TInt countResults( iParser->Count() );
        for ( TInt i = 0 ; i < countResults; ++i )
            {
            // convert to 16 unicode
            //HBufC16* listId = iParser->ResultAsUnicodeTextL( i );
            iListId = iParser->ResultAsUnicodeTextL( i );
            iListId->Des().Fold();

            IMPS_DP( D_IMPS_LIT( " iResponse::%x" ), &iListId );

            MPresentityGroupInfo* groupInfo = objFact.NewPresentityGroupInfoLC();

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

    iConnMan.HandleToHost().GroupsDataHost().HandlePresentityGroupListL( listOfLists );
    CleanupStack::Pop(); //listOfLists

    IMPS_DP( D_IMPS_LIT( " iResponse::%x" ), &iResponse );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CImpsPluginGroupSubscribePresentityGroupListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CImpsPluginGroupSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequest
// ---------------------------------------------------------------------------
//
void CImpsPluginGroupSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequestL()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequest Start" ) );

    //iPtrbuffer.Set( iConnMan.DataHandler().TransferBufferL() );
    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupListRequest Manufacture XML " ) );

    serializer->StartTagL( KTransactionContent );
    serializer->AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
    serializer->StartTagL( KGetListOfList ).EndTagL( KGetListOfList );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer


    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CImpsPluginGroupSubscribePresentityGroupListRequest::SubscribePresentityGroupListRequest End" ) );
    }

// End of file
