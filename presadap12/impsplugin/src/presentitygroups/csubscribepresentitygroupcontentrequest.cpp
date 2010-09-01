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
#include "CSubscribePresentityGroupContentRequest.h"
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
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <ximpidentity.h>
#include <ximpdatasubscriptionstate.h>
#include "PImpsAdapXMLTools.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::CSubscribePresentityGroupContentRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupContentRequest::CSubscribePresentityGroupContentRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityGroupContentRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CSubscribePresentityGroupContentRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupContentRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::ConstructL Start-End" ) );

    iParser = CreateXMLParserL();

    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::NewL
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupContentRequest* CSubscribePresentityGroupContentRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::NewL Start" ) );

    CSubscribePresentityGroupContentRequest* self = new( ELeave )
    CSubscribePresentityGroupContentRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::NewLC
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupContentRequest* CSubscribePresentityGroupContentRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::NewLC Start" ) );

    CSubscribePresentityGroupContentRequest* self =
        CSubscribePresentityGroupContentRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::~CSubscribePresentityGroupContentRequest
// ---------------------------------------------------------------------------
//
CSubscribePresentityGroupContentRequest::~CSubscribePresentityGroupContentRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityGroupContentRequest Start" ) );
    delete iResponse;
    delete iParser;
    delete iGroupName;
    delete iContactName;
    delete iGroupDisplayName;
    delete iUserId;

    CActive::Cancel();
    iConnMan.Remove( this );
    IMPS_DP( D_IMPS_LIT( "::~CSubscribePresentityGroupContentRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupContentRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::RunL()
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupContentRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL Start" ) );

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

    // if we get 200 then fetch the details else complete the request

    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MPresenceObjectFactory& prObjFact =
            iConnMan.HandleToHost().ProtocolPresenceDataHost().PresenceObjectFactory();
        MXIMPObjectCollection* listOfContacts = prfwObjFact.NewObjectCollectionLC();

        MXIMPIdentity* contactIdentity;
        MPresentityGroupMemberInfo* groupMemberInfo;

        TBool nicknameflag = EFalse;
        TBool useridflag = EFalse;

        // NickName exists
        if ( iParser->DecodeL( *iResponse, KNickName, EFalse ) )
            {
            IMPS_DP( D_IMPS_LIT( "NickName Exists in the result" ) );
            nicknameflag = ETrue;
            }

        // UserID exists
        if ( iParser->DecodeL( *iResponse, KNickName, EFalse ) )
            {
            IMPS_DP( D_IMPS_LIT( "UserID Exists in the result" ) );
            useridflag = ETrue;
            }
        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        if ( nicknameflag && useridflag )
            {

            // this is to be tested for boundary conditions of contacts.
            if ( iParser->DecodeL( *iResponse, KNickName, EFalse ) )
                {

                TInt countResults( iParser->Count() );
                IMPS_DP( D_IMPS_LIT( "UserID and NickName Both exists count %d" ), countResults );
                for ( TInt i = 0 ; i < countResults; ++i )
                    {

                    contactIdentity = prfwObjFact.NewIdentityLC();
                    iParser->DecodeL( *iResponse, KName, EFalse );
                    iContactName = iParser->ResultAsUnicodeTextL( i );
                    iContactName->Des().Fold();


                    iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse );
                    iUserId = iParser->ResultAsUnicodeTextL( i );
                    iUserId->Des().Fold();

                    contactIdentity->SetIdentityL( *iUserId );

                    groupMemberInfo = prObjFact.NewPresentityGroupMemberInfoLC();

                    groupMemberInfo->SetGroupMemberIdL( contactIdentity );
                    groupMemberInfo->SetGroupMemberDisplayNameL( *iContactName );

                    listOfContacts->AddObjectL( groupMemberInfo );

                    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL:UserId:%S " ), iUserId );

                    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL:ContactName:%S " ), iContactName );

                    delete iContactName;
                    iContactName = NULL;

                    delete iUserId;
                    iUserId = NULL;

                    CleanupStack::Pop( 2 ); // groupMemberInfo, contactIdentity
                    }

                }
            }
        else
            {

            // this is to be tested for boundary conditions of contacts.
            if ( iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse ) )
                {
                TInt countResults( iParser->Count() );
                IMPS_DP( D_IMPS_LIT( "UserID and NickName either of them exists count %d" ), countResults );
                for ( TInt i = 0 ; i < countResults; ++i )
                    {
                    contactIdentity = prfwObjFact.NewIdentityLC();

                    iParser->DecodeL( *iResponse, KUserIDXMLTag, EFalse );
                    iUserId = iParser->ResultAsUnicodeTextL( i );
                    iUserId->Des().Fold();

                    contactIdentity->SetIdentityL( *iUserId );
                    groupMemberInfo = prObjFact.NewPresentityGroupMemberInfoLC();
                    groupMemberInfo->SetGroupMemberIdL( contactIdentity );

                    listOfContacts->AddObjectL( groupMemberInfo );

                    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL:UserId:%S " ), iUserId );

                    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL:ContactName:%S " ), &contactIdentity->Identity() );

                    delete iUserId;
                    iUserId = NULL;

                    CleanupStack::Pop( 2 ); // groupMemberInfo, contactIdentity
                    }

                }
            }

        MXIMPIdentity* groupIdentity1 = prfwObjFact.NewIdentityLC();
        groupIdentity1->SetIdentityL( *iGroupName );
        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupContentL(
            groupIdentity1, listOfContacts );

        iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, KErrNone );


        IMPS_DP( D_IMPS_LIT( " before clprop " ) );
        if ( iParser->DecodeL( *iResponse, KContactListProperties, EFalse ) )
            {
            MPEngXMLParser* Parser2 = CreateXMLParserLC();
            if ( iParser->DecodeL( *iResponse, KProperty, ETrue ) ) // EFalse ))
                {
                TInt countResults( iParser->Count() );

                for ( TInt i = 0 ; i < countResults; ++i )
                    {
                    IMPS_DP( D_IMPS_LIT( " clprop index::%d %d" ), i, countResults );
                    if ( Parser2->DecodeL( iParser->ResultL( i ), KName, EFalse ) ) // *iResponse, KName, EFalse ))
                        {
                        IMPS_DP( D_IMPS_LIT( " name" ) );
                        if ( Parser2->ResultL().CompareF( KDispName ) == KErrNone  )
                            {

                            if ( Parser2->DecodeL( iParser->ResultL( i ), KValue, EFalse ) )  // *iResponse, KValue, EFalse ))
                                {
                                MXIMPIdentity* groupIdentity = prfwObjFact.NewIdentityLC();
                                iGroupDisplayName = Parser2->ResultAsUnicodeTextL();
                                iGroupDisplayName->Des().Fold();
                                IMPS_DP( D_IMPS_LIT( " CSubscribePresentityGroupContentRequest::RunL:displayname::%s" ), iGroupDisplayName );
                                MPresentityGroupInfo* groupInfo = prObjFact.NewPresentityGroupInfoLC();
                                groupIdentity->SetIdentityL( *iGroupName );
                                groupInfo->SetGroupIdL( groupIdentity );
                                groupInfo->SetGroupDisplayNameL( *iGroupDisplayName );
                                iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupDisplayNameUpdatedL( groupInfo );
                                delete iGroupDisplayName;
                                iGroupDisplayName = NULL;

                                CleanupStack::Pop( 2 ); //groupInfo,groupIdentity
                                }
                            continue;
                            }
                        // parse default flag
                        if ( Parser2->ResultL().CompareF( KDefault ) == KErrNone  )
                            {
                            IMPS_DP( D_IMPS_LIT( " default" ) );
                            // OK that is settings of the default list
                            if ( Parser2->DecodeL( iParser->ResultL( i ),
                                                   KValue,
                                                   EFalse ) )
                                {
                                //TBool defValue( aXMLparser2.ResultL().CompareF(
                                //                  KXMLValueTrue ) == KErrNone );

                                }
                            }
                        } //
                    } // for end

                IMPS_DP( D_IMPS_LIT( " out of for" ) );
                } // if end
            CleanupStack::PopAndDestroy(); // Parser2
            }

        CleanupStack::Pop( 2 ); //groupIdentity1,listOfContacts
        }
    else
        {
        IMPS_DP( D_IMPS_LIT( "HRC error code %d" ), wvErrorCode );
        iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );
        }

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CSubscribePresentityGroupContentRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunError Start" ) );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSubscribePresentityGroupContentRequest::SubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CSubscribePresentityGroupContentRequest::SubscribePresentityGroupContentL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupContentL Start" ) );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::SubscribePresentityGroupContentL Start" ) );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CSubscribePresentityGroupContentRequest::SubscribePresentityGroupContentL:GroupId:%S " ), &aIdentity.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aIdentity.Identity().AllocL();

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );
    serializer->StartTagL( KListMnageRequest );

    if ( KErrNone == iGroupName->Left( KPEngAttrWVIdPrefixLength ).CompareF( KPengWVIDPrefix ) )
        {
        serializer->StartTagL( KContactList
                             ).WvAddressL( aIdentity.Identity()
                                         ).EndTagL( KContactList );
        }

    else
        {
        serializer->StartTagL( KContactList
                             ).NarrowTextL( KWVXMLTag
                                          ).WvAddressL( aIdentity.Identity()
                                                      ).EndTagL( KContactList );
        }

    serializer->StartTagL( KReceiveList ).NarrowTextL( KXMLValueTrue ).EndTagL( KReceiveList );

    serializer->EndTagL( KListMnageRequest );

    serializer->EndTagL( KTransactionContent );


    IMPS_DP( D_IMPS_LIT( "SubscribePresentityGroupContentL" ) );

    CleanupStack::PopAndDestroy(); //serializer

    iSendId = iConnMan.DataHandler().SendDataL( iStatus );

    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::SubscribePresentityGroupContentL End" ) );
    }

// End of file
