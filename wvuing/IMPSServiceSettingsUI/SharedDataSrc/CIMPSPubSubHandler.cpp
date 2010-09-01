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
* Description:  Shared data handler
*
*/

// INCLUDE FILES
#include    "CIMPSPubSubHandler.h"
#include	"MIMPSSharedDataObserver.h"
#include    "CIMPSPropertyObserver.h"
#include    "IMPSSharedDataDefs.h"
#include	<e32property.h>    // RProperty

_LIT_SECURITY_POLICY_PASS( KIMPSSharedReadPolicy );
_LIT_SECURITY_POLICY_PASS( KIMPSSharedWritePolicy );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSPubSubHandler* CIMPSPubSubHandler::NewL( MIMPSSharedDataObserver* aObserver, const TUid aUid )
    {
    CIMPSPubSubHandler* self = new( ELeave ) CIMPSPubSubHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;
    }


// Symbian OS default constructor can leave.
void CIMPSPubSubHandler::ConstructL( const TUid aUid )
    {
    // assign
    iUid = aUid;

    AppendKeyPairsL();
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::SubscribeChange( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSubscribeChangeL( aUid, aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::SubscribeSet( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSubscribeSetL( aUid, aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::UnSubscribe( const TUid aUid, const TDesC* aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    for ( TInt count( 0 ); count < iPropertyObservers.Count(); count++ )
        {
        CIMPSPropertyObserver* observer = iPropertyObservers[ count ];
        if ( ( observer->Category() == aUid ) &&
             ( ( TInt )observer->Key() == aKey ) )
            {
            iPropertyObservers.Remove( count );
            delete observer;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::GetStringKey( const TDesC& aKey, TDes& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue )
    {
    RProperty::Define( iUid, aKey, RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    TBuf<RProperty::KMaxPropertySize> temp;
    TInt err = RProperty::Get( iUid, aKey, temp );
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
// CIMPSPubSubHandler::GetIntKey( const TDesC& aKey, TInt& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::GetIntKey( const TIMPSSharedKeys aKey, TInt& aValue )
    {
    RProperty::Define( iUid, aKey, RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    TInt err = RProperty::Get( iUid, aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::SetStringKey( const TDesC& aKey, const TDesC& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue )
    {
    RProperty::Define( iUid, aKey, RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    TInt err = RProperty::Set( iUid, aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::SetIntKey( const TDesC& aKey, TInt aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::SetIntKey( const TIMPSSharedKeys aKey, TInt aValue )
    {
    RProperty::Define( iUid, aKey, RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    TInt err = RProperty::Set( iUid, aKey, aValue );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::Signal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::Signal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSignalL( aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::DoSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::DoSignalL( const TIMPSSharedKeys aKey )
    {
    TInt current( 0 );
    RProperty::Define( iUid, aKey, RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    User::LeaveIfError( RProperty::Get( iUid, aKey, current ) );
    User::LeaveIfError( RProperty::Set( iUid, aKey, ( current + 1 ) ) );
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::CancelSignal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::CancelSignal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoCancelSignalL( aKey ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::DoCancelSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::DoCancelSignalL( const TIMPSSharedKeys aKey )
    {
    TInt current( 0 );
    RProperty::Define( iUid, aKey, RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    User::LeaveIfError( RProperty::Get( iUid, aKey, current ) );
    User::LeaveIfError( RProperty::Set( iUid, aKey, ( current - 1 ) ) );
    }



// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::DoSubscribeChangeL( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::DoSubscribeChangeL( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    CIMPSPropertyObserver* observer = CIMPSPropertyObserver::NewL( *this );
    CleanupStack::PushL( observer );
    observer->ObservePropertyChangeL( aUid, aKey );
    User::LeaveIfError( iPropertyObservers.Append( observer ) );
    CleanupStack::Pop( observer );
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::DoSubscribeSetL( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::DoSubscribeSetL( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    CIMPSPropertyObserver* observer = CIMPSPropertyObserver::NewL( *this );
    CleanupStack::PushL( observer );
    observer->ObservePropertyChangeL( aUid, aKey );
    User::LeaveIfError( iPropertyObservers.Append( observer ) );
    CleanupStack::Pop( observer );
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSPubSubHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest )
    {
    TInt err = MapKeysToClient( aDest, aSrc );
    return err;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSPubSubHandler::CIMPSPubSubHandler( MIMPSSharedDataObserver* aObserver )
        : iObserver( aObserver )
    {
    }

// Destructor
CIMPSPubSubHandler::~CIMPSPubSubHandler()
    {
    iPropertyObservers.ResetAndDestroy();
    iPropertyObservers.Close();
    iKeys.Close();
    }

// -----------------------------------------------------------------------------
// CIMPSPubSubHandler::HandlePropertyNotificationEventL( const TDesC& aKey, TInt aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSPubSubHandler::HandlePropertyNotificationEventL( TUid aCategory, TUint aKey )
    {
    if ( iObserver )
        {
        iObserver->HandleTemporaryKeyNotifyL( aCategory, ( TIMPSSharedKeys )aKey );
        }
    }


// ---------------------------------------------------------
// CIMPSPubSubHandler::MapKeysToClient()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CIMPSPubSubHandler::MapKeysToClient( TIMPSSharedKeys& aKey, const TDesC& aSharedDataKey )
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
// CIMPSPubSubHandler::DoAppendKeyPairL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSPubSubHandler::DoAppendKeyPairL( TIMPSSharedKeys aKey, const TDesC& aSharedDataKey )
    {
    TIMPSSharedKeyPairs temp;
    temp.iKeyForSharedData.Set( aSharedDataKey );
    temp.iKeyForClient = aKey;
    User::LeaveIfError( iKeys.Append( temp ) );
    }

// ---------------------------------------------------------
// CIMPSPubSubHandler::AppendKeyPairsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSPubSubHandler::AppendKeyPairsL()
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

    // Service settings login type change notifier key
    DoAppendKeyPairL( EIMPSSharedKeysServSettLoginTypeChangedPEC,
                      KIMPSSharedKeysServiceSettingsLoginTypesChangedPEC );
    // Service settings login type change notifier key
    DoAppendKeyPairL( EIMPSSharedKeysServSettLoginTypeChangedIM,
                      KIMPSSharedKeysServiceSettingsLoginTypesChangedIM );

    // Service settings schedule change notifier key
    DoAppendKeyPairL( EIMPSSharedKeysServSettScheduleChangedPEC,
                      KIMPSSharedKeysServiceSettingsScheduleChangedPEC );
    // Service settings schedule change notifier key
    DoAppendKeyPairL( EIMPSSharedKeysServSettScheduleChangedIM,
                      KIMPSSharedKeysServiceSettingsScheduleChangedIM );

    }

//  End of File
