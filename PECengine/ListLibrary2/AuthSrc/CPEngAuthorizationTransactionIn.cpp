/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Authorization in transaction handler.
*
*/


// INCLUDE FILES
#include "CPEngAuthorizationTransactionIn.h"
#include "CPEngAuthorizationRequest.h"
#include "CPEngAuthorizationResponse.h"
#include "MPEngAuthorizationEngine.h"
#include "MPEngXMLParser.h"

#include "MPEngPresenceAttrManager.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::CPEngAuthorizationTransactionIn()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransactionIn::CPEngAuthorizationTransactionIn(
    MPEngAuthorizationEngine& aAuthEngine,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aXMLParser,
    const TDesC8& aParseBlock,
    TPEngAuthorizationTransType aType )
        : iAuthEngine( aAuthEngine ),
        iAttributeManager( aAttrManager ),
        iXMLParser( aXMLParser ),
        iParseBlock( aParseBlock ),
        iResponseType( aType )
    {
    PENG_DP( D_PENG_LIT( "CPEngAuthorizationTransactionIn::CPEngAuthorizationTransactionIn" ) );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionIn::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::NewL()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransactionIn* CPEngAuthorizationTransactionIn::NewL(
    MPEngAuthorizationEngine& aAuthEngine,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aXMLParser,
    const TDesC8& aParseBlock,
    TPEngAuthorizationTransType aType )
    {
    CPEngAuthorizationTransactionIn* self = NewLC( aAuthEngine,
                                                   aAttrManager,
                                                   aXMLParser,
                                                   aParseBlock,
                                                   aType );

    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransactionIn* CPEngAuthorizationTransactionIn::NewLC(
    MPEngAuthorizationEngine& aAuthEngine,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aXMLParser,
    const TDesC8& aParseBlock,
    TPEngAuthorizationTransType aType )
    {
    CPEngAuthorizationTransactionIn* self =
        new ( ELeave ) CPEngAuthorizationTransactionIn(
        aAuthEngine,
        aAttrManager,
        aXMLParser,
        aParseBlock,
        aType );
    CleanupStack::PushL( self );
    self->ConstructL( );

    return self;
    }


// Destructor
CPEngAuthorizationTransactionIn::~CPEngAuthorizationTransactionIn()
    {
    }



// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::ProcessRequestL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionIn::ProcessRequestL(
    const TDesC8& /*aRequest*/,
    TRequestStatus& /*aStatus*/ )
    {
    HBufC* userId = ParseUserIdLC();
    switch ( iResponseType )
        {
        case EPEngAuthorizationRequest:
            {
            PENG_DP( D_PENG_LIT( "CPEngAuthorizationTransactionIn::ProcessRequestL() - AuthRequest[%S]" ),
                     userId );


            CPEngAuthorizationRequest* authRequest =
                CPEngAuthorizationRequest::NewLC( *userId,
                                                  iAuthEngine.SizeCounter() );
            ParseAttrListL( *authRequest,
                            &CPEngAuthorizationRequest::AddRequestedAttributeL );

            // store authorization request
            // removes authRequest from cleanupstack
            iAuthEngine.AddAuthRequestLX( authRequest );
            break;
            }


        case EPEngAuthorizationResponse:
            {
            PENG_DP( D_PENG_LIT( "CPEngAuthorizationTransactionIn::ProcessRequestL() - AuthResponse[%S]" ),
                     userId );

            CPEngAuthorizationResponse* authStatus =
                CPEngAuthorizationResponse::NewLC( *userId,
                                                   iAuthEngine.SizeCounter() );

            ParseAttrListL( *authStatus,
                            &CPEngAuthorizationResponse::AddAttributeL );
            ParseAuthorizationStatusL( *authStatus );

            // store authorization status
            // removes authStatus from cleanupstack
            iAuthEngine.AddAuthStatusLX(  authStatus );
            break;
            }


        default:
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy( userId );
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::CancelProcessing()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionIn::CancelProcessing()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::ParseUserIdL()
// -----------------------------------------------------------------------------
//
HBufC* CPEngAuthorizationTransactionIn::ParseUserIdLC()
    {
    if ( !iXMLParser.DecodeL( iParseBlock, KUserIDXMLTag, EFalse ) )
        {
        // message is corrupted, ignore it
        User::Leave( KErrArgument );
        return NULL;
        }

    HBufC* userId = iXMLParser.ResultAsWVAddressL();
    CleanupStack::PushL( userId );
    return userId;
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::ParseAttrListL()
// -----------------------------------------------------------------------------
//
template <class AuthItem>
void CPEngAuthorizationTransactionIn::ParseAttrListL(
    AuthItem& aAuthItem,
    void ( AuthItem::* aAddFunc )( TUint32 ) )
    {
    // read list of attributes
    if ( iXMLParser.DecodeL( iParseBlock, KPresenceSubList, ETrue ) )
        {
        // store list of attributes
        TPtrC8 attributesBlock = iXMLParser.ResultL();

        //look for all supported attributes
        TArray<TUint32> supportedAt = iAttributeManager.KnownAttributeTypes();
        TInt count( supportedAt.Count() );
        for ( TInt x( 0 ); x < count ; ++x )
            {
            // get attribute's name
            TPtrC8 nameSpace;
            TPtrC8 attrName;
            if ( ( KErrNone == iAttributeManager.GetAttributeXmlNameAndNameSpace(
                       supportedAt[ x ],
                       attrName,
                       nameSpace ) )
                 &&
                 ( iXMLParser.DecodeL( attributesBlock, attrName, EFalse ) ) )
                {
                ( aAuthItem.*aAddFunc )( supportedAt[ x ] );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransactionIn::ParseAuthorizationStatusL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransactionIn::ParseAuthorizationStatusL(
    CPEngAuthorizationResponse& aAuthStatus )
    {
    TBool acceptance( EFalse );
    if ( iXMLParser.DecodeL( iParseBlock,
                             KAcceptance,
                             EFalse ) )
        {
        acceptance = ( iXMLParser.ResultL().CompareF( KXMLValueTrue ) == KErrNone );
        }

    if ( acceptance )
        {
        aAuthStatus.SetResponseType( MPEngAuthorizationStatus::EPEngAuthAccepted );
        }
    else
        {
        aAuthStatus.SetResponseType( MPEngAuthorizationStatus::EPEngAuthDenied );
        }
    }


//  End of File










































