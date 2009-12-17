/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence attribute transaction factory.
*
*/

// INCLUDE FILES
#include "CPEngAttributeTransactionFactory.h"
#include "CPEngAttributePublishHandler.h"
#include "CPEngPresenceNotificationHandler.h"
#include "CPEngAttributeFetchHandler.h"

#include "MPEngPresenceAttrTransactionEnv.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAdvancedAttrModel2.h"

#include "PEngWVAttributeConstructorFactory.h"
#include "RObjectArray.h"
#include "CPEngSessionSlotId.h"
#include "PEngAddressUtils.h"

#include <E32Std.h>
#include <PEngPresenceEngineConsts2.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeTransactionFactory* CPEngAttributeTransactionFactory::NewL(
    const CPEngSessionSlotId& aSlotId,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    MPEngPresenceAttrManager& aAttrManager )
    {
    CPEngAttributeTransactionFactory* self = new ( ELeave ) CPEngAttributeTransactionFactory(
        aTransEnv,
        aAttrManager );

    CleanupClosePushL( *self ); //reference counted singleton
    self->ConstructL( aSlotId );
    CleanupStack::Pop(); //self

    return self;
    }


// Destructor
CPEngAttributeTransactionFactory::~CPEngAttributeTransactionFactory()
    {
    TRAP_IGNORE( iTransEnv.HandleNetworkSessionStateChangeL( EFalse ) );
    delete iDomain;

    if ( iRefsOwned )
        {
        iTransEnv.Close();
        iAttributeManager.Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::CPEngAttributeTransactionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeTransactionFactory::CPEngAttributeTransactionFactory(
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    MPEngPresenceAttrManager& aAttrManager )
        : iTransEnv( aTransEnv ),
        iAttributeManager( aAttrManager ),
        iRefsOwned( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransactionFactory::ConstructL( const CPEngSessionSlotId& aSlotId )
    {
    CPEngSessionSlotObject::ConstructL( aSlotId );

    //resolve transaction domain
    iDomain = PEngAddressUtils::WVDomain( aSlotId.UserId() ).AllocL();
    iCspVersion = PEngWVCspVersion::SelectVersionL();

    RObjectArray< MPEngPresenceAttrModel2 > sessionAttributes;
    CleanupClosePushL( sessionAttributes );

    PEngWVAttributeConstructorFactory::LoadSessionTracingAttributesL( sessionAttributes,
                                                                      iAttributeManager );
    iTransEnv.SetSessionTracingAttributesL( sessionAttributes );
    CleanupStack::PopAndDestroy(); //closes sessionAttributes


    iTransEnv.HandleNetworkSessionStateChangeL( ETrue );

    iRefsOwned = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::Close()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransactionFactory::Close()
    {
    CPEngSessionSlotObject::Close();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::IncomingTransactionHandlerL()
// -----------------------------------------------------------------------------
//
MPEngIncomingTransactionHandler* CPEngAttributeTransactionFactory::
IncomingTransactionHandlerL( const TDesC8& aIncomingRequest )
    {
    MPEngIncomingTransactionHandler* handler = NULL;

    if ( CPEngPresenceNotificationHandler::CanHandleL( aIncomingRequest ) )
        {
        //This is a presence notification
        handler = CPEngPresenceNotificationHandler::NewL( *iDomain,
                                                          iCspVersion,
                                                          iAttributeManager,
                                                          iTransEnv );
        }

    return handler;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransactionFactory::OutgoingTransactionHandlerL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransactionFactory::OutgoingTransactionHandlerL(
    TInt aOperation,
    const TDesC16& aData,
    RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers )
    {
    if ( aOperation == EPEngTransOpOwnAttributePublish )
        {
        CPEngAttributePublishHandler* publisher = CPEngAttributePublishHandler::NewLC( iAttributeManager,
                                                                                       iTransEnv,
                                                                                       aData,
                                                                                       aOperation,
                                                                                       iCspVersion );
        aHandlers.AppendL( publisher );
        CleanupStack::Pop( publisher );
        }


    else if ( ( aOperation == EPEngTransOpAttributeFetchToCache ) ||
              ( aOperation == EPEngTransOpAttributeFetchToObjects ) )
        {
        CPEngAttributeFetchHandler* fetcher = CPEngAttributeFetchHandler::NewLC( iAttributeManager,
                                                                                 *iDomain,
                                                                                 aData,
                                                                                 aOperation,
                                                                                 iCspVersion );
        aHandlers.AppendL( fetcher );
        CleanupStack::Pop( fetcher );
        }
    }


//  End of File
