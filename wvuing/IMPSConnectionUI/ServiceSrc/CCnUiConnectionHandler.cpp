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
* Description:  Connection handler implementation.
*
*/

// INCLUDE FILES
#include "CCnUiConnectionHandler.h"
#include "CCnUiSapStoreProxy.h"

#include "MCnUiClientStatusHandler.h"
#include "CnUiErrors.h"
#include "IMPSCommonUiDebugPrint.h"

#include <e32std.h>

#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsStore.h>
#include <CPEngNWSessionSlot2.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>



/**
 * WV protocol prefix.
 */
_LIT( KWVIdPrefix, "wv:" );


/**
 * Domain separator @.
 */
_LIT( KDomainSeparatorAt, "@" );

/**
 * Length of WV protocol prefix.
 */
const TInt KWVIdPrefixLength = 3;


// ================= GLOBAL FUNCTIONS ====================
// -----------------------------------------------------------------------------
// CreateConnHandlerL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiConnectionHandler* CreateConnHandlerL( CCnUiSapStoreProxy& aSapProxy )
    {
    return CCnUiConnectionHandler::NewL( aSapProxy );
    }


// ================= LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// StripWVSchemaPrefix()
// -----------------------------------------------------------------------------
//
LOCAL_D TPtrC StripWVSchemaPrefix( const TDesC& aFullPresenceId )
    {
    TPtrC prefix( aFullPresenceId.Left( KWVIdPrefixLength ) );
    if ( prefix.CompareF( KWVIdPrefix ) == 0 )
        {
        //has prefix
        return aFullPresenceId.Mid( KWVIdPrefixLength );
        }
    return aFullPresenceId;
    }


// -----------------------------------------------------------------------------
// StripDomainPart()
// -----------------------------------------------------------------------------
//
LOCAL_D TPtrC StripDomainPart( const TDesC& aFullPresenceId )
    {
    TInt domainStartPos = aFullPresenceId.Find( KDomainSeparatorAt );
    if ( domainStartPos == KErrNotFound )
        {
        return aFullPresenceId;
        }
    return aFullPresenceId.Left( domainStartPos );
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiConnectionHandler* CCnUiConnectionHandler::NewL( CCnUiSapStoreProxy& aSapProxy )
    {
    CCnUiConnectionHandler* self = new ( ELeave ) CCnUiConnectionHandler( aSapProxy );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCnUiConnectionHandler::~CCnUiConnectionHandler()
    {
    if ( iNWSessionSlotID )
        {
        CancelSapConnectionOpen( *iNWSessionSlotID );
        CancelSapConnectionClose( *iNWSessionSlotID );
        delete iNWSessionSlotID;
        }

    delete iClientStatusHandler;
    delete iNWSessionSlot;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiConnectionHandler::CCnUiConnectionHandler( CCnUiSapStoreProxy& aSapProxy )
        : iSapProxy( aSapProxy )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiConnectionHandler::ConstructL()
    {
    iClientStatusHandler = CreateClientStatusHandlerLC();
    CleanupStack::Pop(); //iClientStatusHandler


    CPEngNWSessionSlotID2* slotID = CPEngNWSessionSlotID2::NewLC();
    slotID->SetServiceAddressMatchAnyL();
    slotID->SetUserIdMatchAnyL();
    slotID->SetAppIdL( KPEngAppIdIM() );

    //Clear logged in flags if there doesn't exist a connection
    if ( !( NwConnectionActiveL( *slotID ) ) )
        {
        iClientStatusHandler->SetClientLoggedOutL( EIMPSConnClientIM );
        }

    CleanupStack::PopAndDestroy( slotID );
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::OpenSapConnectionL()
//
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::OpenSapConnectionL(
    const CIMPSSAPSettings& aSapToLogin,
    CPEngNWSessionSlotID2& aNWSessionSlotID,
    MPEngNWSessionOperationObserver2& aSlotOperationObserver )
    {
    if ( !NwConnectionActiveL( aNWSessionSlotID ) )
        {
        //initially there isn't any logged in clients
        TIMPSConnectionClient client;
        if ( 0 == aNWSessionSlotID.AppId().Compare( KPEngAppIdPEC() ) )
            {
            client = EIMPSConnClientPEC;
            }
        else
            {
            client = EIMPSConnClientIM;
            }
        iClientStatusHandler->SetClientLoggedOutL( client );
        }


    CPEngNWSessionSlot2* sessionSlot = CPEngNWSessionSlot2::NewL( aNWSessionSlotID );
    delete iNWSessionSlot;
    iNWSessionSlot = sessionSlot;

    TInt err ( iNWSessionSlot->OpenNWPresenceSession( aSapToLogin,
                                                      aSlotOperationObserver ) );

    if ( err == KErrInUse )
        {
        // this error suggests, that we have a session open already going on from
        // this same object. This should not happen, but if it does we can ignore it
        // since we are already doing what we should start doing here
        err = KErrNone;
        }

    //handle connection manager errors
    if ( err == KErrAccessDenied )
        {
        //network operations not allowed due offline profile
        User::Leave( KCnUiErrorNetworkConnectionNotAllowed );
        }
    User::LeaveIfError( err );

    delete iNWSessionSlotID;
    iNWSessionSlotID = NULL;
    iNWSessionSlotID = aNWSessionSlotID.CloneL();
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::CancelSapConnectionOpen()
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::CancelSapConnectionOpen( CPEngNWSessionSlotID2& /* aNWSessionSlotID */ )
    {
    if ( iNWSessionSlot )
        {
        iNWSessionSlot->CancelOpenNWPresenceSession();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::CloseSapConnectionL()
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::CloseSapConnectionL(
    CPEngNWSessionSlotID2& aNWSessionSlotID,
    MPEngNWSessionOperationObserver2& aSlotOperationObserver )
    {
    // if we already have the same slot id as in the parameter, we are in sync
    // and slot is also good. in that case we must NOT delete the slot id - if we
    // do, then PEC Engine CSP session gets closed and OpenNWPresenceSessionOwnership
    // will leave with -18.
    CPEngNWSessionSlotID2* tempSlot = aNWSessionSlotID.CloneL();
    delete iNWSessionSlotID;
    iNWSessionSlotID = tempSlot;

    CPEngNWSessionSlotID2* bSlot = NULL;
    if ( !iNWSessionSlot )
        {
        CPEngNWSessionSlot2* sessionSlot = CPEngNWSessionSlot2::NewL( *iNWSessionSlotID );
        delete iNWSessionSlot;
        iNWSessionSlot = sessionSlot;
        }
    else
        {
        bSlot = CPEngNWSessionSlotID2::NewLC();
        iNWSessionSlot->GetNWSessionSlotID( *bSlot );
        if ( aNWSessionSlotID.MatchBasePart( *bSlot ) != KErrNone )
            {
            CPEngNWSessionSlot2* sessionSlot = CPEngNWSessionSlot2::NewL( *iNWSessionSlotID );
            delete iNWSessionSlot;
            iNWSessionSlot = sessionSlot;
            }
        CleanupStack::PopAndDestroy( bSlot );
        }

    TInt err( KErrNone );

    err = iNWSessionSlot->OpenNWPresenceSessionOwnership();

    // we can ignore KErrAlreadyExists
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    err = iNWSessionSlot->ForceCloseNWPresenceSession( aSlotOperationObserver );

    if ( err == KErrInUse )
        {
        // this error suggests, that we have a session close already going on from
        // this same object. This should not happen, but if it does we can ignore it
        // since we are already doing what we should start doing here
        err = KErrNone;
        }
    User::LeaveIfError( err );
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::CancelSapConnectionClose()
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::CancelSapConnectionClose( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngNWSessionSlot2* slot = NULL;
    // we can ignore the error, since if we don't find the slot
    // there's not really much else we can do here
    TInt ignore;
    TRAP( ignore, slot = NetworkSessionSlotForIDL( aNWSessionSlotID ) );
    if ( slot )
        {
        slot->CancelCloseNWPresenceSession();
        delete slot;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::LoginTheClientL()
// Client login handler
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::LoginTheClientL( TIMPSConnectionClient aClient )
    {
    if ( iClientStatusHandler )
        {
        iClientStatusHandler->SetClientLoggedInL( aClient );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::LogoutTheClientL()
// Client login handler
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::LogoutTheClientL( TIMPSConnectionClient aClient )
    {
    //remove client from among the logged in ones
    iClientStatusHandler->SetClientLoggedOutL( aClient );
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::TheClientLoggedInL()
// Status getter
// -----------------------------------------------------------------------------
//
TBool CCnUiConnectionHandler::TheClientLoggedInL( TIMPSConnectionClient aClient )
    {

    CPEngNWSessionSlotID2* slotID = CPEngNWSessionSlotID2::NewLC();
    slotID->SetServiceAddressMatchAnyL();
    slotID->SetUserIdMatchAnyL();
    if ( aClient == EIMPSConnClientPEC )
        {
        slotID->SetAppIdL( KPEngAppIdPEC() );
        }
    else
        {
        slotID->SetAppIdL( KPEngAppIdIM() );
        }

    //Clear logged in flags if there doesn't exist a connection
    if ( !NwConnectionActiveL( *slotID ) )
        {
        iClientStatusHandler->SetClientLoggedOutL( aClient );
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnectionHandler::TheClientLoggedInL( %d ) - [%d] (no nw)" ), aClient, EFalse );
        CleanupStack::PopAndDestroy( slotID );
        return EFalse;
        }

    CleanupStack::PopAndDestroy( slotID );
    //active connection so client is in
    return ETrue;
    }



// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::GetLoggedInSapL()
// Status getter
// -----------------------------------------------------------------------------
//
TBool CCnUiConnectionHandler::GetLoggedInSapL(
    CIMPSSAPSettings& aSap,
    TIMPSConnectionClient aClient )
    {

    aSap.Reset();

    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    CPEngNWSessionSlotID2* pattern = CPEngNWSessionSlotID2::NewLC();
    pattern->SetServiceAddressMatchAnyL();
    pattern->SetUserIdMatchAnyL();
    if ( aClient == EIMPSConnClientPEC )
        {
        pattern->SetAppIdL( KPEngAppIdPEC );
        }
    else
        {
        pattern->SetAppIdL( KPEngAppIdIM );
        }

    RPointerArray< CPEngNWSessionSlotID2 > array;
    TInt err( slotManager->GetNWSessionSlots( array, *pattern, EPEngNWPresenceSessionOpen ) );
    CleanupStack::PopAndDestroy( 2, slotManager ); // slotManager, pattern

    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &array ) );
    if ( err == KErrNotFound )
        {
        CleanupStack::PopAndDestroy(); //array
        return EFalse;
        }
    else
        {
        User::LeaveIfError( err );
        }

    if ( array.Count() > 0 )
        {
        CPEngNWSessionSlot2* tempSlot = NULL;
        TRAPD( error, tempSlot = NetworkSessionSlotForIDL( *array[0] ) );
        if ( error == KErrNotFound )
            {
            CleanupStack::PopAndDestroy(); //array
            return EFalse;
            }
        else
            {
            User::LeaveIfError( error );
            }
        if ( tempSlot )
            {
            tempSlot->OpenNWPresenceSessionOwnership();
            TInt err = tempSlot->GetNWPresenceSessionSap( aSap );
            delete tempSlot;
            if ( err == KErrNone )
                {
                // this slot was active and SAP is now filled
                CleanupStack::PopAndDestroy(); //array
                return ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy(); //array
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::SapConnectionStatusL()
// Status getter
// -----------------------------------------------------------------------------
//
TCnUiSapCnStatus CCnUiConnectionHandler::SapConnectionStatusL( const CIMPSSAPSettings& aSap,
                                                               TIMPSConnectionClient aClient )
    {
    CIMPSSAPSettings* loggedinSAP = CIMPSSAPSettings::NewLC();
    TBool nwConnectionActive = GetLoggedInSapL( *loggedinSAP, aClient );

    //verify connection settings (address & user id)
    TBool sapAddressMatch = ( loggedinSAP->SAPAddress().Compare( aSap.SAPAddress() ) == 0 );

    TPtrC loggedInPlainUserId(
        StripWVSchemaPrefix( StripDomainPart( loggedinSAP->SAPUserId() ) ) );
    TPtrC loginPlainUserId( StripWVSchemaPrefix( StripDomainPart( aSap.SAPUserId() ) ) );
    TBool userIdsMatch = ( loggedInPlainUserId.Compare( loginPlainUserId ) == 0 );


    //verify passwords
    TBool pwdsMatch = EFalse;
    if ( loggedinSAP->SAPUserPassword().Compare( aSap.SAPUserPassword() ) == 0 )
        {
        pwdsMatch = ETrue;
        }
    CleanupStack::PopAndDestroy( loggedinSAP ); //loggedinSAP


    if ( !nwConnectionActive )
        {
        //no active NW connection
        return ECnUiSCS_NotConnected;
        }


    //there is a some network connection, what its state is ?
    if ( sapAddressMatch && userIdsMatch )
        {
        //session to similar SAP than gived one exist already
        if ( pwdsMatch )
            {
            //exactly same SAP connected
            return ECnUiSCS_SapConnected;
            }
        //similar SAP but with different password connected
        return ECnUiSCS_SapConnected_PwdMissMatch;
        }

    else
        {
        //session to some other SAP exist

        //is there known clients logged in to this session ?
        if ( iClientStatusHandler->AnyClientLoggedIn() )
            {
            return ECnUiSCS_AnotherSapConnected;
            }
        return ECnUiSCS_AnotherSapConnected_ClientsNotKnown;
        }
    }



// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::GetLoggedInClientsL()
// Status getter
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::GetLoggedInClientsL( RArray< TIMPSConnectionClient >& aClients )
    {
    aClients.Reset();
    if ( iClientStatusHandler->ClientLoggedIn( EIMPSConnClientPEC ) )
        {
        User::LeaveIfError( aClients.Append( EIMPSConnClientPEC ) );
        }

    if ( iClientStatusHandler->ClientLoggedIn( EIMPSConnClientIM ) )
        {
        User::LeaveIfError( aClients.Append( EIMPSConnClientIM ) );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::NwConnectionActiveL()
// Status getter
// -----------------------------------------------------------------------------
//
TBool CCnUiConnectionHandler::NwConnectionActiveL( CPEngNWSessionSlotID2& aIdToMatch )
    {
    //check do we have a connection or not

    // assume that if both service address and userid are given it's a full ID
    // if either is wildcard then assume that only appID is wanted to be matched
    TBool fullID ( !aIdToMatch.IsServiceAddressWild() | !aIdToMatch.IsUserIdWild() );

    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    RPointerArray< CPEngNWSessionSlotID2 > array;
    TInt error( slotManager->GetNWSessionSlots( array, aIdToMatch,
                                                EPEngNWPresenceSessionOpen ) );
    if ( error != KErrNone )
        {
        array.ResetAndDestroy();
        User::Leave( error );
        }
    CleanupStack::PopAndDestroy( slotManager ); // slotManager

    TBool returnValue( EFalse );
    if ( array.Count() > 0 )
        {
        returnValue = ETrue;
        }

    array.ResetAndDestroy();
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::ServiceStatusL()
// Status getter
// -----------------------------------------------------------------------------
//
TPEngNWSessionSlotState CCnUiConnectionHandler::ServiceStatusL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    TPEngNWSessionSlotState slotState ( EPEngNWPresenceSessionClosed );

    CPEngNWSessionSlot2* slot = NetworkSessionSlotForIDL( aNWSessionSlotID );
    slot->GetNWSessionSlotState( slotState );
    delete slot;

    return slotState;
    }

// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::ServiceStatusL()
// Status getter
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlot2* CCnUiConnectionHandler::NetworkSessionSlotForIDL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    return CPEngNWSessionSlot2::NewL( aNWSessionSlotID );
    }

// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::SetSessionSlotL()
// Status getter
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::SetSessionSlotL( CPEngNWSessionSlot2* aSlot )
    {
    delete iNWSessionSlot;
    iNWSessionSlot = aSlot;
    }

// -----------------------------------------------------------------------------
// CCnUiConnectionHandler::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CCnUiConnectionHandler::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }
//  End of File


