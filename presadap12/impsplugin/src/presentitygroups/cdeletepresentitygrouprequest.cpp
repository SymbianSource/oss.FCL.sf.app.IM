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
* Description:  Delete Presentity Group Request to the Network Server.
*
*/


#include <ximpprotocolconnectionhost.h>
#include "CDeletePresentityGroupRequest.h"
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
// CDeletePresentityGroupRequest::CDeletePresentityGroupRequest
// ---------------------------------------------------------------------------
//
CDeletePresentityGroupRequest::CDeletePresentityGroupRequest(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId ) :
        CActive( EPriorityNormal ),
        iRequestId( aRequestId ),
        iConnMan( aConnMan )
    {
    IMPS_DP( D_IMPS_LIT( "::CDeletePresentityGroupRequest Start" ) );
    CActiveScheduler::Add( this );
    IMPS_DP( D_IMPS_LIT( "::CDeletePresentityGroupRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::ConstructL
// ---------------------------------------------------------------------------
//
void CDeletePresentityGroupRequest::ConstructL()
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::ConstructL Start" ) );

    iParser = CreateXMLParserL();
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::ConstructL End" ) );
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::NewL
// ---------------------------------------------------------------------------
//
CDeletePresentityGroupRequest* CDeletePresentityGroupRequest::NewL(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::NewL Start" ) );

    CDeletePresentityGroupRequest* self =
        new( ELeave ) CDeletePresentityGroupRequest( aConnMan, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::NewL End" ) );
    return self;
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::NewLC
// ---------------------------------------------------------------------------
//
CDeletePresentityGroupRequest* CDeletePresentityGroupRequest::NewLC(
    MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId )
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::NewLC Start" ) );

    CDeletePresentityGroupRequest* self =
        CDeletePresentityGroupRequest::NewL( aConnMan, aRequestId );
    CleanupStack::PushL( self );



    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::NewLC End" ) );
    return self;
    }

// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::~CDeletePresentityGroupRequest
// ---------------------------------------------------------------------------
//
CDeletePresentityGroupRequest::~CDeletePresentityGroupRequest()
    {
    IMPS_DP( D_IMPS_LIT( "::~CDeletePresentityGroupRequest Start" ) );
    CActive::Cancel();
    iConnMan.Remove( this );
    delete iGroupName;
    delete iParser;
    IMPS_DP( D_IMPS_LIT( "::~CDeletePresentityGroupRequest End" ) );
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::DoCancel()
// ---------------------------------------------------------------------------
//
void CDeletePresentityGroupRequest::DoCancel()
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::DoCancel Start" ) );

    iConnMan.DataHandler().CancelSending( iSendId );

    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::DoCancel End" ) );
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::RunL()
// ---------------------------------------------------------------------------
//
void CDeletePresentityGroupRequest::RunL()
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::RunL Start" ) );

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

    IMPS_DP( D_IMPS_LIT( "DeletePresentityGroupRequest %d" ), wvErrorCode );

    // issue request to prfw datacache for updated info
    if ( KWVRequestResponseResultCode == wvErrorCode )
        {
        MXIMPObjectFactory& prfwObjFact = iConnMan.HandleToHost().ObjectFactory();
        MXIMPIdentity* groupName = prfwObjFact.NewIdentityLC();
        groupName->SetIdentityL( *iGroupName );

        iConnMan.HandleToHost().ProtocolPresenceDataHost().GroupsDataHost().HandlePresentityGroupDeletedL( groupName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        IMPS_DP( D_IMPS_LIT( "---------------------Data from presadap1.2 to PresenceFramework-----------" ) );

        IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::RunL:GroupId:%S " ), iGroupName );

        IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

        CleanupStack::Pop( 1 ); // groupName
        }

    // tell prfw that request is complete
    if ( wvErrorCode == KWVRequestResponseResultCode )
        {
        wvErrorCode = KErrNone;
        }
    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, wvErrorCode );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::RunL End" ) );
    }


// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::RunError
// ---------------------------------------------------------------------------
//
TInt CDeletePresentityGroupRequest::RunError( TInt  aError )
    {
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::RunError %d" ), aError );

    iConnMan.HandleToHost().HandleRequestCompleted( iRequestId, aError );

    delete this;
    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::RunError End" ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDeletePresentityGroupRequest::DeletePresentityGroupRequestL
// ---------------------------------------------------------------------------
//
void CDeletePresentityGroupRequest::DeletePresentityGroupRequestL(
    const MXIMPIdentity& aIdentity )
    {
    IMPS_DP( D_IMPS_LIT( "::DeletePresentityGroupRequestL Start" ) );

    IMPSPRTPLUGIN_UNUSED_PARAM( aIdentity );

    IMPS_DP( D_IMPS_LIT( "------------------------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "---------------------Data from framework to presadap1.2-----------" ) );

    IMPS_DP( D_IMPS_LIT( "CDeletePresentityGroupRequest::DeletePresentityGroupRequestL:aIdentity:%S " ), & aIdentity.Identity() );

    TPtr8 ptrbuffer( iConnMan.DataHandler().TransferBufferL() );
    MPEngXMLSerializer* serializer = CreateXmlSerializerLC( ptrbuffer );

    iGroupName = aIdentity.Identity().AllocL();

    IMPS_DP( D_IMPS_LIT( "DeletePresentityGroupRequestL Manufacture XML " ) );

    NImpsAdapXMLTools::AppendTransactionContentTagXmlL( *serializer, iConnMan.GetVersion() );

    serializer->StartTagL( KDeleteList );

    // <ContactList>
    // Contact list name value
    // </ContactList>
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

    // </DeleteList-Request>
    serializer->EndTagL( KDeleteList );
    serializer->EndTagL( KTransactionContent );

    CleanupStack::PopAndDestroy(); //serializer

    IMPS_DP( D_IMPS_LIT( "DeletePresentityGroupRequestL SendData to the DataHandler" ) );
    iSendId = iConnMan.DataHandler().SendDataL( iStatus );


    IMPS_DP( D_IMPS_LIT( "DeletePresentityGroupRequestL Before SetActive" ) );
    // signal the scheduler
    SetActive();

    IMPS_DP( D_IMPS_LIT( "::DeletePresentityGroupRequestL End" ) );
    }

// End of file
