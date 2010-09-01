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
#include "CSubscribePresenceWatcherListRequest.h"
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
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <protocolpresencepublishingdatahost.h>
#include <presencewatcherinfo.h>
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::CSubscribePresenceWatcherListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceWatcherListRequest::CSubscribePresenceWatcherListRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceWatcherListRequest Start" ) );
    CActiveScheduler::Add( this );

    IMPS_DP( D_IMPS_LIT( "::CSubscribePresenceWatcherListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceWatcherListRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresenceWatcherListRequest* CSubscribePresenceWatcherListRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::NewL Start" ) );

    CSubscribePresenceWatcherListRequest* self = new( ELeave )
    CSubscribePresenceWatcherListRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresenceWatcherListRequest* CSubscribePresenceWatcherListRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::NewLC Start" ) );

    CSubscribePresenceWatcherListRequest* self =
        CSubscribePresenceWatcherListRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::~CSubscribePresenceWatcherListRequest
// ---------------------------------------------------------------------------
//
CSubscribePresenceWatcherListRequest::~CSubscribePresenceWatcherListRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceWatcherListRequest Start" ) );

    delete iResponse;
    delete iWatcherId;
    delete iWatcherStatus;
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iParser;

    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresenceWatcherListRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceWatcherListRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresenceWatcherListRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunL Start" ) );

    User::LeaveIfError( iStatus.Int() );

    iResponse = iConnMan.DataHandler().ResponseL( iSendId );

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;

    /* parse the watcherlist content.
         in watcherlist response there is no status of 200 */

    TInt countResults = 0;

    if ( iParser->DecodeL( *iResponse, KWatcherTag, ETrue ) )
        {
        IMPS_DP( D_IMPS_LIT( "PresenceWatcherListRequest Server did not send the Watcher Tag:" ) );
        countResults = iParser->Count();
        }

    if ( iParser->DecodeL( *iResponse, KUserTag, EFalse ) )
        {
        IMPS_DP( D_IMPS_LIT( " Serever did send user tag in response" ) );
        countResults = iParser->Count();
        }

    MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
    MPresenceObjectFactory& prObjFact =
        iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
    MXIMPObjectCollection* listOfWatchers = prfwObjFact.NewObjectCollectionLC();

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunL:Total Watcher:%d " ), countResults );

    for ( TInt i = 0 ; i < countResults; ++i )
        {
        MXIMPIdentity* watcherIdentity = prfwObjFact.NewIdentityLC();
        MPresenceWatcherInfo* watcherInfo = prObjFact.NewPresenceWatcherInfoLC();

        // check user tag
        if ( iParser->DecodeL( *iResponse, KUserTag, EFalse ) )
            {

            if ( iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse ) )
                {
                iWatcherId = iParser->ResultAsUnicodeTextL( i );
                iWatcherId->Des().Fold();
                watcherIdentity->SetIdentityL( *iWatcherId );
                watcherInfo->SetWatcherIdL( watcherIdentity );
                IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunL:wather id:%S " ), iWatcherId );

                IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunL:watherIdentity:%S " ), & watcherIdentity->Identity() );

                IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

                }

            // check clientID tag
            if ( iParser->DecodeL( *iResponse, KClientIdTag, EFalse ) )
                {
                // check url
                if ( iParser->DecodeL( *iResponse, KURLTag, EFalse ) )
                    {
                    // where do we store in the watcher info object?
                    // either parse and store this object locally in the class
                    }
                }
            }

        // check watcher status
        if ( iParser->DecodeL( *iResponse, KWatcherStatusTag, EFalse ) )
            {
            iWatcherStatus = iParser->ResultAsUnicodeTextL( i );
            iWatcherStatus->Des().Fold();

            // compare the string with current subscriber and set type
            _LIT16( KCurrentSubscriber, "CURRENT_SUBSCRIBER" );

            // check if the string contains the above substring
            TInt value = iWatcherStatus->CompareF( KCurrentSubscriber ) ;

            if ( KErrNone == value )
                {// current subscriber
                watcherInfo->SetWatcherTypeL( MPresenceWatcherInfo::EPresenceSubscriber );
                }
            else
                {
                // other type of subscriber
                watcherInfo->SetWatcherTypeL( MPresenceWatcherInfo::EUnknown );
                }
            }

        listOfWatchers->AddObjectL( watcherInfo );
        delete iWatcherId;
        iWatcherId = NULL;
        delete iWatcherStatus;
        iWatcherStatus = NULL;

        CleanupStack::Pop( 2 ); // watcherIdentity, watcherInfo

        } // for loop end

    // issue internal request to the prfw.
    iConnMan.HandleToHost().ProtocolPresenceDataHost().PublishingDataHost().HandlePresenceWatcherListL( listOfWatchers );

    MXIMPDataSubscriptionState* dataSubscriptionState = prfwObjFact.NewDataSubscriptionStateLC();

    dataSubscriptionState->SetDataStateL( MXIMPDataSubscriptionState::EDataAvailable );
    dataSubscriptionState->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionActive );

    iConnMan.HandleToHost().ProtocolPresenceDataHost().PublishingDataHost().SetPresenceWatcherListDataSubscriptionStateL(
        dataSubscriptionState, NULL );
    CleanupStack::Pop(); // listOfWatchers
    CleanupStack::Pop(); // dataSubscriptionState


    IMPS_DP( D_IMPS_LIT( " SubscribePresenceWatcherListL wvErrorCode::%d" ), wvErrorCode );

    // complete the request from prfw
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresenceWatcherListRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresenceWatcherListRequest::SubscribePresenceWatcherListL
// ---------------------------------------------------------------------------
//
void CSubscribePresenceWatcherListRequest::SubscribePresenceWatcherListL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::SubscribePresenceWatcherListL Start" ) );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );


    IMPS_DP( D_IMPS_LIT( "SubscribePresenceWatcherListL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KGetWatchers );

    serializer->StartTagL( KHistoryPeriod ).NarrowTextL( KHistoryPeriodValue ).EndTagL( KHistoryPeriod );
    serializer->StartTagL( KMaxWatcherList ).NarrowTextL( KMaxWatcherListValue ).EndTagL( KMaxWatcherList );

    serializer->EndTagL( KGetWatchers );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    IMPS_DP( D_IMPS_LIT( " SendData Request id %d " ), iSendId );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "CSubscribePresenceWatcherListRequest::SubscribePresenceWatcherListL End" ) );
    }

// End of file
