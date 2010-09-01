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
* Description:  Connection UI implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <PEngPresenceEngineConsts2.h>
#include <CPEngNWSessionSlot2.h>
#include <sysutil.h>

#include "CIMPSPresenceConnectionUiImp.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiSignaller.h"
#include "CCnUiUiControlContext.h"
#include "CCnUiConnModeRewaker.h"
#include "CnUiErrors.h"
#include "IMPSCommonUiDebugPrint.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CIMPSPresenceConnectionUiImp* CIMPSPresenceConnectionUiImp::NewL(
    TIMPSConnectionClient aClient,
    CIMPSSAPSettingsStore* aSapStore )
    {
    CIMPSPresenceConnectionUiImp* self =
        new ( ELeave ) CIMPSPresenceConnectionUiImp( aClient, aSapStore );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CIMPSPresenceConnectionUiImp::~CIMPSPresenceConnectionUiImp()
    {
    delete iConnModeRewaker;
    delete iConnHandler;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSPresenceConnectionUiImp::CIMPSPresenceConnectionUiImp( TIMPSConnectionClient aClient,
                                                            CIMPSSAPSettingsStore* aSapStore )
        : iSapStoreProxy( aSapStore ), iClient( aClient )
    {
    }


// Symbian OS default constructor can leave.
void CIMPSPresenceConnectionUiImp::ConstructL()
    {
    // set the client information to the TLS
    User::LeaveIfError( Dll::SetTls( reinterpret_cast<TAny*>( iClient ) ) );
    iConnHandler = CreateConnHandlerL( iSapStoreProxy );
    iConnModeRewaker = CCnUiConnModeRewaker::NewL();
    ReStoreNWSessionOwnershipL();
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::LoginL()
//
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::LoginL( TIMPSConnectionClient aClient,
                                           TIMPSLoginType aLoginType,
                                           CPEngNWSessionSlotID2& aNWSessionSlotID,
                                           MIMPSConnProcessObserver* aObserver,
                                           CIMPSSAPSettings* aSap /*= NULL*/,
                                           TBool aLastLoginSap /*= EFalse*/ )
    {

    if ( LoggedInL( aClient ) )
        {
        // it seems we are already logged in
        return KErrAlreadyExists;
        }

    // Check disk space
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( NULL, 0 ) )
        {
        // Don't show any own notes here
        User::Leave( KErrDiskFull );
        }

    iConnModeRewaker->SuspendLC();

    // add two extra param if user want to login to last server only
    CCnUiUiControlContext* uiCC = CCnUiUiControlContext::NewLC( aClient,
                                                                iSapStoreProxy,
                                                                *iConnModeRewaker,
                                                                *iConnHandler,
                                                                aObserver );

    TInt retStatus = uiCC->DoLoginL( aLoginType, aNWSessionSlotID, aSap, aLastLoginSap );
    CleanupStack::PopAndDestroy( 2 ); //uiCC, Resume ReWake

    return MapToUiLoginReturnStatus( retStatus );
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::LogoutL()
//
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::LogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                            MIMPSConnProcessObserver* aObserver )
    {
    TIMPSConnectionClient client;

    if ( 0 == aNWSessionSlotID.AppId().Compare( KPEngAppIdIM ) )
        {
        client = EIMPSConnClientIM;
        }
    else
        {
        client = EIMPSConnClientPEC;
        }

    //if client isn't logged in, nothing to do..
    if ( !iConnHandler->TheClientLoggedInL( client ) )
        {
        return KErrNone;
        }

    iConnModeRewaker->SuspendLC();

    CCnUiUiControlContext* uiCC = CCnUiUiControlContext::NewLC( client,
                                                                iSapStoreProxy,
                                                                *iConnModeRewaker,
                                                                *iConnHandler,
                                                                aObserver );


    uiCC->DoLogoutL( aNWSessionSlotID );
    CleanupStack::PopAndDestroy( 2 ); //uiCC, Resume ReWake

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::HandleApplicationExitL()
//
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::HandleApplicationExitL(
    TIMPSConnectionClient aClient,
    TIMPSExitType aExitType,
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    MIMPSConnProcessObserver* aObserver )
    {
    //if client isn't logged in, nothing to do..
    if ( !iConnHandler->TheClientLoggedInL( aClient ) )
        {
        return KErrNone;
        }

    //logged in, handle as normal commands

    if ( aExitType == EIMPSLeaveSessionOpenExit )
        {
        // we are supposed to store the ownership here
        StoreNWSessionOwnershipL( aNWSessionSlotID );
        return KErrNone;
        }

    iConnModeRewaker->SuspendLC();
    CCnUiUiControlContext* uiCC = CCnUiUiControlContext::NewLC( aClient,
                                                                iSapStoreProxy,
                                                                *iConnModeRewaker,
                                                                *iConnHandler,
                                                                aObserver );

    TInt retStatus = uiCC->DoHandleApplicationExitL( aExitType, aNWSessionSlotID );
    CleanupStack::PopAndDestroy( 2 ); //uiCC, Resume ReWake

    if ( retStatus == KErrCancel )
        {
        // query was shown, and user selected to leave the session open
        StoreNWSessionOwnershipL( aNWSessionSlotID );
        }

    return retStatus;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::GetActiveNWSessionSlotIDL()
//
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2* CIMPSPresenceConnectionUiImp::GetActiveNWSessionSlotIDL(
    TIMPSConnectionClient aClient )
    {
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
    TInt error( slotManager->GetNWSessionSlots( array,
                                                *pattern,
                                                EPEngNWPresenceSessionOpen ) );
    if ( error != KErrNone )
        {
        array.ResetAndDestroy();
        User::Leave( error );
        }
    CleanupStack::PopAndDestroy( 2, slotManager ); // pattern, slotManager
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &array ) );

    CPEngNWSessionSlotID2* slotID = NULL;

    if ( array.Count() > 0 )
        {
        // only one active slot per application at the moment
        CPEngNWSessionSlotID2* tempSlotID = array[ 0 ];

        slotID = tempSlotID->CloneL();
        }
    else
        {
        // not found
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); //array
    return slotID;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::GetLoggedInSapL()
//
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::GetLoggedInSapL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                                    CIMPSSAPSettings& aSap )
    {
    iSapStoreProxy.PushOwnedSapStoreReleaseLC();
    TIMPSConnectionClient client;
    if ( 0 == aNWSessionSlotID.AppId().Compare( KPEngAppIdIM ) )
        {
        client = EIMPSConnClientIM;
        }
    else
        {
        client = EIMPSConnClientPEC;
        }

    TBool connected = iConnHandler->GetLoggedInSapL( aSap, client );
    CleanupStack::PopAndDestroy(); //PushOwnedSapStoreReleaseLC

    if ( connected )
        {
        return KErrNone;
        }
    else
        {
        aSap.Reset();
        return KErrNotFound;
        }
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::PureServiceStatusL()
//
// -----------------------------------------------------------------------------
//
TPEngNWSessionSlotState CIMPSPresenceConnectionUiImp::PureServiceStatusL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    return iConnHandler->ServiceStatusL( aNWSessionSlotID );
    }



// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::CurrentConnectionOperationL()
//
// -----------------------------------------------------------------------------
//
TIMPSConnectionOperation CIMPSPresenceConnectionUiImp::CurrentConnectionOperationL(
    HBufC*& aAddtionalDesc )
    {
    aAddtionalDesc = NULL;
    TIMPSConnectionOperation currectOp = EIMPSConnOppUnknown;

    MCnUiSignaller* loginSignaller = CreateConnUiSignallerLC();

    if ( loginSignaller->OperationRunning() )
        {
        currectOp = EIMPSConnOppClientLogin;

        loginSignaller->OperationDetailsL( aAddtionalDesc );
        if ( !aAddtionalDesc )
            {
            //method semantic promises that if the operation is
            //EIMPSConnOppClientLogin, it will always have a
            //server name as aAddtionalDesc ==> create empty buf if missing server name
            aAddtionalDesc = HBufC::NewL( 0 );
            }
        }
    CleanupStack::PopAndDestroy(); //loginSignaller

    return currectOp;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::MapToUiLoginReturnStatus()
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::MapToUiLoginReturnStatus( TInt aOperationResult )
    {
    TInt mappedError = KErrNone;

    switch ( aOperationResult )
        {
            //falltrough
        case KErrNone: //  Login succeeded
        case KErrCancel: //User has cancelled the login at some point.
            {
            //these are returned as is
            mappedError = aOperationResult;
            break;
            }

        case KCnUiErrorNoProperDefaultSap:
            {
            //Missing the whole default SAP / or no SAP's at all
            mappedError = KErrNotFound;
            break;
            }

        case KCnUiErrorSapMissingCompulsoryFields:
            {
            //Default or selected SAP to login was missing some required fields.
            mappedError = KErrArgument;
            break;
            }

        case KCnUiErrorClientAlreadyConnected:
            {
            //Current client is already logged in. (Thus not a real error.)
            mappedError = KErrAlreadyExists;
            break;
            }

        default:
            {
            //General failure during login (no connection to network, network access denied, etc..)
            mappedError = KErrGeneral;
            break;
            }
        }

    return mappedError;
    }


// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::StoreNWSessionOwnership()
// -----------------------------------------------------------------------------
//
void CIMPSPresenceConnectionUiImp::StoreNWSessionOwnershipL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSPresenceConnectionUiImp::StoreNWSessionOwnership()" ) );

    CPEngNWSessionSlot2* sessionSlot = CPEngNWSessionSlot2::NewLC( aNWSessionSlotID );

    // get the ownership of this slot
    User::LeaveIfError( sessionSlot->OpenNWPresenceSessionOwnership() );

    // store the ownership so that PECEngine does not close the session when
    // session slot objects are deleted
    TInt err = sessionSlot->StoreNWPresenceSessionOwnership( aNWSessionSlotID.AppId() );
    // Ignore KErrAlreadyExists
    if ( err != KErrAlreadyExists && err != KErrNone )
        {
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( sessionSlot ); // sessionSlot, slotID
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::ReStoreNWSessionOwnership()
// -----------------------------------------------------------------------------
//
void CIMPSPresenceConnectionUiImp::ReStoreNWSessionOwnershipL()
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSPresenceConnectionUiImp::ReStoreNWSessionOwnership()" ) );

    CPEngNWSessionSlotID2* slotID = NULL;

    // get the active network session slot
    TRAPD( err, slotID = GetActiveNWSessionSlotIDL( iClient ) );
    if ( err == KErrNotFound )
        {
        // no active network session slot for this client
        // we can just return
        return;
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }

    // we got the active slot's ID
    CleanupStack::PushL( slotID );

    CPEngNWSessionSlot2* sessionSlot = CPEngNWSessionSlot2::NewLC( *slotID );

    TPtrC ownership;
    if ( iClient == EIMPSConnClientPEC )
        {
        ownership.Set( KPEngAppIdPEC() );
        }
    else
        {
        ownership.Set( KPEngAppIdIM() );
        }

    // restore the ownership so that PECEngine will close the session when
    // session slot objects are deleted
    err = sessionSlot->RestoreNWPresenceSessionOwnership( ownership );

    if ( err == KErrNotFound )
        {
        // there was no stored session ownership
        // so we really don't need to do anything
        CleanupStack::PopAndDestroy( 2, slotID ); // sessionSlot, slotID
        return;
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }

    // give this slot to connection handler
    iConnHandler->SetSessionSlotL( sessionSlot );
    CleanupStack::Pop( sessionSlot );

    CleanupStack::PopAndDestroy( slotID ); // sessionSlot
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::LoggedInL()
// -----------------------------------------------------------------------------
//
TBool CIMPSPresenceConnectionUiImp::LoggedInL( TIMPSConnectionClient aClient )
    {
    CPEngNWSessionSlotID2* slotID = NULL;
    TRAPD( err, slotID = GetActiveNWSessionSlotIDL( aClient ) );
    if ( err == KErrNotFound )
        {
        // no session slots -> not logged in
        return EFalse;
        }
    else
        {
        // for other errors we have to leave
        User::LeaveIfError( err );
        }
    // slot found, we are logged in
    delete slotID;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::DisplayDomainSelectionQueryL
// -----------------------------------------------------------------------------
//
TInt CIMPSPresenceConnectionUiImp::DisplayDomainSelectionQueryL(
    TDes& aSelectedDomain,
    CIMPSSAPSettings* aSap )
    {
    return CCnUiUiControlContext::DisplayDomainSelectionQueryL(
               aSelectedDomain, aSap );
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CIMPSPresenceConnectionUiImp::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast< RPointerArray< CPEngNWSessionSlotID2 >* >( aObject )->ResetAndDestroy();
    }

//  End of File

