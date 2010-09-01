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
#include    "CIMPSSharedDataHandler.h"
#include	"MIMPSSharedDataObserver.h"
#include	"WVSettingsSharedDataNG.h"
#include	"IMPSSharedDataDefs.h"

#include	<shareddatavalues.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSharedDataHandler* CIMPSSharedDataHandler::NewL(
    MIMPSSharedDataObserver* aObserver,
    const TUid aUid, TBool aTemporary )
    {
    CIMPSSharedDataHandler* self =
        new( ELeave ) CIMPSSharedDataHandler( aObserver,
                                              aUid,
                                              aTemporary );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Symbian OS default constructor can leave.
void CIMPSSharedDataHandler::ConstructL()
    {
    iSharedDataClient = this;
    // assign
    User::LeaveIfError( iSharedDataClient.Connect() );
    if ( iTemporary )
        {
        User::LeaveIfError( iSharedDataClient.AssignToTemporaryFile( iUid ) );
        }
    else
        {
        User::LeaveIfError( iSharedDataClient.Assign( iUid ) );
        }

    AppendKeyPairsL();
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    return iSharedDataClient.NotifyChange( aUid, &key );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    return iSharedDataClient.NotifySet( aUid, &key );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSharedDataHandler::UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    iSharedDataClient.CancelNotify( aUid, &key );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    return iSharedDataClient.GetString( key, aValue );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::GetIntKey( const TDesC& aKey, TInt& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::GetIntKey( const TIMPSSharedKeys aKey, TInt& aValue )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    return iSharedDataClient.GetInt( key, aValue );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );
    return iSharedDataClient.SetString( key, aValue );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::SetIntKey( const TIMPSSharedKeys aKey, TInt aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::SetIntKey( const TIMPSSharedKeys aKey, TInt aValue )
    {
    TRAPD( err, DoSetIntKeyL( aKey, aValue ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::DoSetIntKeyL( const TIMPSSharedKeys aKey, TInt aValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSharedDataHandler::DoSetIntKeyL( const TIMPSSharedKeys aKey, TInt aValue )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );

    User::LeaveIfError( iSharedDataClient.SetInt( key, aValue ) );
    }


// ---------------------------------------------------------
// CIMPSSharedDataHandler::HandleNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSSharedDataHandler::HandleNotifyL( const TUid aUid,
                                            const TDesC& aKey,
                                            const TDesC& /* aValue */ )
    {
    if ( iObserver )
        {
        TIMPSSharedKeys keyToClient;
        MapKeysToClient( keyToClient, aKey );
        if ( iTemporary )
            {
            iObserver->HandleTemporaryKeyNotifyL( aUid, keyToClient );
            }
        else
            {
            iObserver->HandlePermanentKeyNotifyL( aUid, keyToClient );
            }
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::Signal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::Signal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoSignalL( aKey ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::DoSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSharedDataHandler::DoSignalL( const TIMPSSharedKeys aKey )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );

    TInt current( 0 );
    TInt err = iSharedDataClient.GetInt( key, current );
    if ( err == KErrNotFound )
        {
        // we don't need to worry about this since if the key is not found
        // the correct value for the current value is 0
        err = KErrNone;
        }
    User::LeaveIfError( err );
    User::LeaveIfError( iSharedDataClient.SetInt( key, current + 1 ) );
    }


// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::CancelSignal( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::CancelSignal( const TIMPSSharedKeys aKey )
    {
    TRAPD( err, DoCancelSignalL( aKey ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::DoCancelSignalL( const TDesC& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSharedDataHandler::DoCancelSignalL( const TIMPSSharedKeys aKey )
    {
    TBuf<KMaxKeyName> key;
    MapKeysToSharedData( aKey, key );

    TInt current( 0 );
    TInt err = iSharedDataClient.GetInt( key, current );
    if ( err == KErrNotFound )
        {
        // we don't need to worry about this since if the key is not found
        // the correct value for the current value is 0
        err = KErrNone;
        }
    User::LeaveIfError( err );
    User::LeaveIfError( iSharedDataClient.SetInt( key, current - 1 ) );
    }

// -----------------------------------------------------------------------------
// CIMPSSharedDataHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aKey )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSharedDataHandler::ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest )
    {
    TInt err = MapKeysToClient( aDest, aSrc );
    return err;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CIMPSSharedDataHandler::CIMPSSharedDataHandler( MIMPSSharedDataObserver* aObserver,
                                                const TUid aUid,
                                                TBool aTemporary )
        : iObserver( aObserver ), iUid( aUid ), iTemporary( aTemporary )
    {
    }

// Destructor
CIMPSSharedDataHandler::~CIMPSSharedDataHandler()
    {
    iSharedDataClient.Close();
    iKeys.Close();
    }


// ---------------------------------------------------------
// CIMPSSharedDataHandler::MapKeysToSharedData()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CIMPSSharedDataHandler::MapKeysToSharedData( const TIMPSSharedKeys aKey, TDes& aSharedDataKey )
    {
    // just to make sure the descriptor is empty
    aSharedDataKey.Zero();

    TInt count( 0 );
    for ( count = 0; count < iKeys.Count(); count++ )
        {
        if ( iKeys[count].iKeyForClient == aKey )
            {
            aSharedDataKey.Append( iKeys[count].iKeyForSharedData );
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CIMPSSharedDataHandler::MapKeysToClient()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CIMPSSharedDataHandler::MapKeysToClient( TIMPSSharedKeys& aKey, const TDesC& aSharedDataKey )
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
// CIMPSSharedDataHandler::DoAppendKeyPairL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSSharedDataHandler::DoAppendKeyPairL( TIMPSSharedKeys aKey, const TDesC& aSharedDataKey )
    {
    TIMPSSharedKeyPairs temp;
    temp.iKeyForSharedData.Set( aSharedDataKey );
    temp.iKeyForClient = aKey;
    User::LeaveIfError( iKeys.Append( temp ) );
    }

// ---------------------------------------------------------
// CIMPSSharedDataHandler::AppendKeyPairsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSSharedDataHandler::AppendKeyPairsL()
    {
    // Service Settings UI
    DoAppendKeyPairL( EIMPSSharedKeysIMLogin,
                      KWVSettingsKeyNameChatLogin );
    DoAppendKeyPairL( EIMPSSharedKeysIMConnStart,
                      KWVSettingsKeyNameIMConnStart );
    DoAppendKeyPairL( EIMPSSharedKeysIMConnEnd,
                      KWVSettingsKeyNameIMConnEnd );
    DoAppendKeyPairL( EIMPSSharedKeysIMConnDays,
                      KWVSettingsKeyNameIMConnDays );
    DoAppendKeyPairL( EIMPSSharedKeysPECLogin,
                      KWVSettingsKeyNamePECLogin );
    DoAppendKeyPairL( EIMPSSharedKeysPECConnStart,
                      KWVSettingsKeyNamePECConnStart );
    DoAppendKeyPairL( EIMPSSharedKeysPECConnEnd,
                      KWVSettingsKeyNamePECConnEnd );
    DoAppendKeyPairL( EIMPSSharedKeysPECConnDays,
                      KWVSettingsKeyNamePECConnDays );

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
                      KProEngAllowUpdatePresence );

    // Presence UI keys
    DoAppendKeyPairL( EIMPSSharedKeysPECAppKeyNamePublishing,
                      KPECAppKeyNamePublishing );

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
