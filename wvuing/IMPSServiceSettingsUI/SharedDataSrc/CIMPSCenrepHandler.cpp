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
* Description:  Central repository handler
*
*/

// INCLUDE FILES
#include    "CIMPSCenrepHandler.h"
#include	"MIMPSSharedDataObserver.h"
#include    "CIMPSCenrepObserver.h"
#include    "IMPSSharedDataDefs.h"
#include	<centralrepository.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSCenrepHandler* CIMPSCenrepHandler::NewL( MIMPSSharedDataObserver* aObserver, const TUid aUid )
    {
    CIMPSCenrepHandler* self = new( ELeave ) CIMPSCenrepHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;
    }


// Symbian OS default constructor can leave.
void CIMPSCenrepHandler::ConstructL( const TUid aUid )
    {
    // assign
    iUid = aUid;

    iRepository = CRepository::NewL( iUid );

    AppendKeyPairsL();
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::SubscribeChange( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSubscribeChangeL( aUid, aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::SubscribeSet( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSubscribeSetL( aUid, aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::UnSubscribe( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    for ( TInt count( 0 ); count < iCenrepObservers.Count(); count++ )
        {
        CIMPSCenrepObserver* observer = iCenrepObservers[ count ];
        if ( ( observer->Category() == aUid ) &&
             ( ( TInt )observer->Key() == aKey ) )
            {
            iCenrepObservers.Remove( count );
            delete observer;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::GetStringKey( const TDesC& aKey, TDes& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue )
    {
    TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> temp;
    TInt err = iRepository->Get( aKey, temp );
    if ( err != KErrNone )
        {
        return err;
        }
    // empty the value
    aValue.Zero();
    aValue.Copy( temp );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::GetIntKey( const TDesC& aKey, TInt& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::GetIntKey( const TIMPSSharedKeys aKey, TInt& aValue )
    {
    TInt err = iRepository->Get( aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::SetStringKey( const TDesC& aKey, const TDesC& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue )
    {
    TInt err = iRepository->Set( aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::SetIntKey( const TDesC& aKey, TInt aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::SetIntKey( const TIMPSSharedKeys aKey, TInt aValue )
    {
    // we can ignore the return value here, since we know the type is correct
    // and if the key is already defined we can safely set it
    TInt err = iRepository->Set( aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::Signal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::Signal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSignalL( aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::DoSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::DoSignalL( const TIMPSSharedKeys aKey )
    {
    TInt current( 0 );
    TInt err = iRepository->Get( aKey, current );
    if ( err == KErrNotFound )
        {
        // we don't need to worry about this since if the key is not found
        // the correct value for the current value is 0
        // but we have to define the key
        err = KErrNone;

        // we can ignore the return value here, since we know the type is correct
        // and if the key is already defined we can safely set it
        // we set the initial value to 0
        iRepository->Create( aKey, 0 );
        }
    User::LeaveIfError( err );


    User::LeaveIfError( iRepository->Set( aKey, ( current + 1 ) ) );
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::CancelSignal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::CancelSignal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoCancelSignalL( aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::DoCancelSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::DoCancelSignalL( const TIMPSSharedKeys aKey )
    {
    TInt current( 0 );
    TInt err = iRepository->Get( aKey, current );
    if ( err == KErrNotFound )
        {
        // we don't need to worry about this since if the key is not found
        // the correct value for the current value is 0
        // but we have to define the key
        err = KErrNone;

        // we can ignore the return value here, since we know the type is correct
        // and if the key is already defined we can safely set it
        // we set the initial value to 0
        iRepository->Create( aKey, 0 );
        }
    User::LeaveIfError( err );

    User::LeaveIfError( iRepository->Set( aKey, ( current - 1 ) ) );
    }



// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::DoSubscribeChangeL( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::DoSubscribeChangeL( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    CIMPSCenrepObserver* observer = CIMPSCenrepObserver::NewL( *this );
    CleanupStack::PushL( observer );
    observer->ObservePropertyChangeL( aUid, aKey );
    User::LeaveIfError( iCenrepObservers.Append( observer ) );
    CleanupStack::Pop( observer );
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::DoSubscribeSetL( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::DoSubscribeSetL( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    CIMPSCenrepObserver* observer = CIMPSCenrepObserver::NewL( *this );
    CleanupStack::PushL( observer );
    observer->ObservePropertyChangeL( aUid, aKey );
    User::LeaveIfError( iCenrepObservers.Append( observer ) );
    CleanupStack::Pop( observer );
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSCenrepHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest )
    {
    TInt err = MapKeysToClient( aDest, aSrc );
    return err;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSCenrepHandler::CIMPSCenrepHandler( MIMPSSharedDataObserver* aObserver )
        : iObserver( aObserver )
    {
    }

// Destructor
CIMPSCenrepHandler::~CIMPSCenrepHandler()
    {
    iCenrepObservers.ResetAndDestroy();
    iCenrepObservers.Close();
    iKeys.Close();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// CIMPSCenrepHandler::HandlePropertyNotificationEventL( TUid aCategory, TUint aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSCenrepHandler::HandlePropertyNotificationEventL( TUid aCategory, TUint aKey )
    {
    if ( iObserver )
        {
        iObserver->HandleTemporaryKeyNotifyL( aCategory, ( TIMPSSharedKeys )aKey );
        }
    }


// ---------------------------------------------------------
// CIMPSCenrepHandler::MapKeysToClient()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CIMPSCenrepHandler::MapKeysToClient( TIMPSSharedKeys& aKey, const TDesC& aSharedDataKey )
    {
    TInt count( 0 );
    for ( count = 0; count < iKeys.Count(); count++ )
        {
        if ( 0 == iKeys[count].iKeyForSharedData.Compare( aSharedDataKey ) )
            {
            aKey = iKeys[count].iKeyForClient;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CIMPSCenrepHandler::DoAppendKeyPairL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSCenrepHandler::DoAppendKeyPairL( TIMPSSharedKeys aKey, const TDesC& aSharedDataKey )
    {
    TIMPSSharedKeyPairs temp;
    temp.iKeyForSharedData.Set( aSharedDataKey );
    temp.iKeyForClient = aKey;
    User::LeaveIfError( iKeys.Append( temp ) );
    }

// ---------------------------------------------------------
// CIMPSCenrepHandler::AppendKeyPairsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSCenrepHandler::AppendKeyPairsL()
    {
    // Connection UI keys
    // Connection UI Group channel keys
    DoAppendKeyPairL( EIMPSSharedKeysIMClientLoginLogoutStateChannel,
                      KIMPSSharedKeysIMClientLoginLogoutStateChannel );
    DoAppendKeyPairL( EIMPSSharedKeysIMLoginLogoutEventChannel,
                      KIMPSSharedKeysIMLoginLogoutEventChannel );
    DoAppendKeyPairL( EIMPSSharedKeysIMSSClientReqistrationChannel,
                      KIMPSSharedKeysIMSSClientReqistrationChannel );
    DoAppendKeyPairL( EIMPSSharedKeysIMGlobalOperationSignalChannel,
                      KIMPSSharedKeysIMGlobalOperationSignalChannel );
    DoAppendKeyPairL( EIMPSSharedKeysIMRemoteUiNotificationsChannel,
                      KIMPSSharedKeysIMRemoteUiNotificationsChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECClientLoginLogoutStateChannel,
                      KIMPSSharedKeysPECClientLoginLogoutStateChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECLoginLogoutEventChannel,
                      KIMPSSharedKeysPECLoginLogoutEventChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECSSClientReqistrationChannel,
                      KIMPSSharedKeysPECSSClientReqistrationChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECGlobalOperationSignalChannel,
                      KIMPSSharedKeysPECGlobalOperationSignalChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECRemoteUiNotificationsChannel,
                      KIMPSSharedKeysPECRemoteUiNotificationsChannel );

    DoAppendKeyPairL( EIMPSSharedKeysCommonClientLoginLogoutStateChannel,
                      KIMPSSharedKeysCommonClientLoginLogoutStateChannel );
    DoAppendKeyPairL( EIMPSSharedKeysCommonLoginLogoutEventChannel,
                      KIMPSSharedKeysCommonLoginLogoutEventChannel );
    DoAppendKeyPairL( EIMPSSharedKeysCommonSSClientReqistrationChannel,
                      KIMPSSharedKeysCommonSSClientReqistrationChannel );
    DoAppendKeyPairL( EIMPSSharedKeysCommonGlobalOperationSignalChannel,
                      KIMPSSharedKeysCommonGlobalOperationSignalChannel );
    DoAppendKeyPairL( EIMPSSharedKeysCommonRemoteUiNotificationsChannel,
                      KIMPSSharedKeysCommonRemoteUiNotificationsChannel );
    // Connection UI Group data keys
    DoAppendKeyPairL( EIMPSSharedKeysIMClientLoginLogoutStateData,
                      KIMPSSharedKeysIMClientLoginLogoutStateData );
    DoAppendKeyPairL( EIMPSSharedKeysIMLoginLogoutEventData,
                      KIMPSSharedKeysIMLoginLogoutEventData );
    DoAppendKeyPairL( EIMPSSharedKeysIMSSClientReqistrationData,
                      KIMPSSharedKeysIMSSClientReqistrationData );
    DoAppendKeyPairL( EIMPSSharedKeysIMGlobalOperationSignalData,
                      KIMPSSharedKeysIMGlobalOperationSignalData );
    DoAppendKeyPairL( EIMPSSharedKeysIMRemoteUiNotificationsData,
                      KIMPSSharedKeysIMRemoteUiNotificationsData );
    DoAppendKeyPairL( EIMPSSharedKeysPECClientLoginLogoutStateData,
                      KIMPSSharedKeysPECClientLoginLogoutStateData );
    DoAppendKeyPairL( EIMPSSharedKeysPECLoginLogoutEventData,
                      KIMPSSharedKeysPECLoginLogoutEventData );
    DoAppendKeyPairL( EIMPSSharedKeysPECSSClientReqistrationData,
                      KIMPSSharedKeysPECSSClientReqistrationData );
    DoAppendKeyPairL( EIMPSSharedKeysPECGlobalOperationSignalData,
                      KIMPSSharedKeysPECGlobalOperationSignalData );
    DoAppendKeyPairL( EIMPSSharedKeysPECRemoteUiNotificationsData,
                      KIMPSSharedKeysPECRemoteUiNotificationsData );

    DoAppendKeyPairL( EIMPSSharedKeysCommonClientLoginLogoutStateData,
                      KIMPSSharedKeysCommonClientLoginLogoutStateData );
    DoAppendKeyPairL( EIMPSSharedKeysCommonLoginLogoutEventData,
                      KIMPSSharedKeysCommonLoginLogoutEventData );
    DoAppendKeyPairL( EIMPSSharedKeysCommonSSClientReqistrationData,
                      KIMPSSharedKeysCommonSSClientReqistrationData );
    DoAppendKeyPairL( EIMPSSharedKeysCommonGlobalOperationSignalData,
                      KIMPSSharedKeysCommonGlobalOperationSignalData );
    DoAppendKeyPairL( EIMPSSharedKeysCommonRemoteUiNotificationsData,
                      KIMPSSharedKeysCommonRemoteUiNotificationsData );

    // Connection UI Global channel keys
    DoAppendKeyPairL( EIMPSSharedKeysIMGlobalChannel,
                      KIMPSSharedKeysIMGlobalChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPECGlobalChannel,
                      KIMPSSharedKeysPECGlobalChannel );

    // Connection UI Global data keys
    DoAppendKeyPairL( EIMPSSharedKeysIMGlobalData,
                      KIMPSSharedKeysIMGlobalData );
    DoAppendKeyPairL( EIMPSSharedKeysPECGlobalData,
                      KIMPSSharedKeysPECGlobalData );

    // Connection UI Permanent global data key
    DoAppendKeyPairL( EIMPSSharedKeysPermanentGlobalChannel,
                      KIMPSSharedKeysPermanentGlobalChannel );
    DoAppendKeyPairL( EIMPSSharedKeysPermanentGlobalData,
                      KIMPSSharedKeysPermanentGlobalData );

    DoAppendKeyPairL( EIMPSSharedKeysIMPresenceAuthSettingKey,
                      KIPMSSharedKeysIMPresenceAuthSettingKey );

    // Profile engines key
    DoAppendKeyPairL( EIMPSSharedKeysProEngAllowUpdatePresence,
                      KIMPSSharedKeysProEngAllowUpdatePresence );

    // Presence UI keys
    DoAppendKeyPairL( EIMPSSharedKeysPECAppKeyNamePublishing,
                      KIMPSSharedKeysPECAppKeyNamePublishing );
    }


//  End of File
