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
* Description:  Reactive authorization transaction manager.
*
*/

// INCLUDE FILES
#include "CPEngAuthorizationTransFactory.h"

#include "CPEngAuthorizationRequest.h"
#include "CPEngAuthorizationResponse.h"
#include "CPEngAuthorizationTransactionIn.h"
#include "CPEngAuthorizationTransactionOut.h"
#include "MPEngListLibFactory.h"

#include "PresenceDebugPrint.h"

#include "MPEngPresenceAttrManager.h"
#include "PEngAttrLibFactory.h"
#include "PEngPresenceEngineConsts2.h"
#include "MPEngXMLParser.h"

#include <e32std.h>
#include <s32mem.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::CPEngAuthorizationTransFactory()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransFactory::CPEngAuthorizationTransFactory(
    MPEngListLibFactory& aFactory,
    TPEngWVCspVersion& aCSPVersion )
        : CPEngAuthorizationEngine( aFactory ),
        iCSPVersion( aCSPVersion )
    {
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransFactory::ConstructL( const CPEngSessionSlotId& aSessionId )
    {
    CPEngAuthorizationEngine::ConstructL( aSessionId );

    iXMLParser = CreateXMLParserLC();
    CleanupStack::Pop();

    iAttributeManager = PEngAttrLibFactory::AttributeManagerInstanceLC( aSessionId );
    CleanupStack::Pop();
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransFactory* CPEngAuthorizationTransFactory::NewLC(
    MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionId,
    TPEngWVCspVersion& aCSPVersion )
    {
    CPEngAuthorizationTransFactory* self =
        new( ELeave ) CPEngAuthorizationTransFactory( aFactory,
                                                      aCSPVersion );

    CleanupClosePushL( *self );
    self->ConstructL( aSessionId );

    return self;
    }


// Destructor
CPEngAuthorizationTransFactory::~CPEngAuthorizationTransFactory()
    {
    if ( iXMLParser )
        {
        iXMLParser->Close();
        }

    if ( iAttributeManager )
        {
        iAttributeManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::Close()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransFactory::Close()
    {
    iAccessCount--;
    if ( iAccessCount == 0 )
        {
        iFactory.RemoveAuthorizationTransManager( this );
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::GetIncomingNwTransactionHandlersL()
// -----------------------------------------------------------------------------
//
MPEngIncomingTransactionHandler* CPEngAuthorizationTransFactory::IncomingTransactionHandlerL(
    const TDesC8& aIncomingRequest )
    {
    // is it authorization request
    if ( iXMLParser->DecodeL( aIncomingRequest, KPresenceAuthRequest, ETrue ) )
        {
        return CPEngAuthorizationTransactionIn::NewL(
                   *this,
                   *iAttributeManager,
                   *iXMLParser,
                   iXMLParser->ResultL(),
                   CPEngAuthorizationTransactionIn::EPEngAuthorizationRequest );
        }


    // is it specific authorization request in different direction
    if ( iXMLParser->DecodeL( aIncomingRequest, KPresenceAuthUser, ETrue ) )
        {
        return CPEngAuthorizationTransactionIn::NewL(
                   *this,
                   *iAttributeManager,
                   *iXMLParser,
                   iXMLParser->ResultL(),
                   CPEngAuthorizationTransactionIn::EPEngAuthorizationResponse );
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::GetOutgoingNwTransactionHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransFactory::OutgoingTransactionHandlerL(
    TInt aOperation,
    const TDesC& aData,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers )
    {
    PENG_DP( D_PENG_LIT( "CPEngContactListTransactionManager::OutgoingTransactionHandlerL()" ) );

    switch ( aOperation )
        {
        case EPEngTransOpAuthorizationResponse:
            {
            CreateAuthResponseHandlersL( aData, aHandlers );
            break;
            }
        default:
            {
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAuthorizationTransFactory::CreateAuthResponseHandlersL()
// -----------------------------------------------------------------------------
//
void CPEngAuthorizationTransFactory::CreateAuthResponseHandlersL(
    const TDesC& aData,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers  )
    {
    // get responses from array, data des is 16 bit descriptor, but will be accessed
    // as 8 bit, therefore length is twice as for 16 bit.
    TPtrC8 bufferDes( reinterpret_cast<const TUint8*> ( aData.Ptr() ),
                      aData.Length() * 2 );
    RDesReadStream rs( bufferDes );
    CleanupClosePushL( rs );
    TInt count( rs.ReadInt32L() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngAuthorizationTransactionOut* transaction =
            CPEngAuthorizationTransactionOut::NewLC( *this,
                                                     *iAttributeManager,
                                                     *iXMLParser,
                                                     iCSPVersion,
                                                     EPEngTransOpAuthorizationResponse );
        CPEngAuthorizationResponse* authResponse =
            CPEngAuthorizationResponse::NewLC( rs, iSize );
        transaction->SetAuthResponse( authResponse );
        CleanupStack::Pop( authResponse ); // authResponse

        aHandlers.AppendL( transaction );
        CleanupStack::Pop( transaction ); // transaction
        }
    CleanupStack::PopAndDestroy(); // rs
    }


//  End of File











