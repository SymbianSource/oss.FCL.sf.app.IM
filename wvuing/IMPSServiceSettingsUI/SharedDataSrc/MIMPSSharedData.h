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
* Description:  Tls holder used to check if resource file is loaded or not
*
*/

#ifndef __MIMPSSHAREDDATA_H
#define __MIMPSSHAREDDATA_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

/**
 * Keys to be used with sharing data
 * @since 2.6
 */

const TInt KIMPSSharedKeysServiceSettingsUIBase = 100;
const TInt KIMPSSharedKeysConnectionUIBase = 200;
const TInt KIMPSSharedKeysAlwayOnlineBase = 300;

enum TIMPSSharedKeys
    {
    // Service Settings UI keys
    EIMPSSharedKeysIMLogin = KIMPSSharedKeysServiceSettingsUIBase + 1,
    EIMPSSharedKeysIMConnStart = KIMPSSharedKeysServiceSettingsUIBase + 2,
    EIMPSSharedKeysIMConnEnd = KIMPSSharedKeysServiceSettingsUIBase + 3,
    EIMPSSharedKeysIMConnDays = KIMPSSharedKeysServiceSettingsUIBase + 4,

    EIMPSSharedKeysPECLogin = KIMPSSharedKeysServiceSettingsUIBase + 5,
    EIMPSSharedKeysPECConnStart = KIMPSSharedKeysServiceSettingsUIBase + 6,
    EIMPSSharedKeysPECConnEnd = KIMPSSharedKeysServiceSettingsUIBase + 7,
    EIMPSSharedKeysPECConnDays = KIMPSSharedKeysServiceSettingsUIBase + 8,

#ifdef RD_MULTIPLE_DRIVE

    EIMPSSharedKeysMemoryDrive = KIMPSSharedKeysServiceSettingsUIBase + 9,

#endif

    EIMPSSharedKeysServSettLoginTypeChangedPEC = KIMPSSharedKeysServiceSettingsUIBase + 51,
    EIMPSSharedKeysServSettLoginTypeChangedIM = KIMPSSharedKeysServiceSettingsUIBase + 52,
    EIMPSSharedKeysServSettScheduleChangedPEC = KIMPSSharedKeysServiceSettingsUIBase + 53,
    EIMPSSharedKeysServSettScheduleChangedIM = KIMPSSharedKeysServiceSettingsUIBase + 54,

    // Connection UI keys

    // Connection UI Group channel keys
    EIMPSSharedKeysIMClientLoginLogoutStateChannel = KIMPSSharedKeysConnectionUIBase + 1,
    EIMPSSharedKeysIMLoginLogoutEventChannel = KIMPSSharedKeysConnectionUIBase + 2,
    EIMPSSharedKeysIMSSClientReqistrationChannel = KIMPSSharedKeysConnectionUIBase + 3,
    EIMPSSharedKeysIMGlobalOperationSignalChannel = KIMPSSharedKeysConnectionUIBase + 4,
    EIMPSSharedKeysIMRemoteUiNotificationsChannel = KIMPSSharedKeysConnectionUIBase + 5,

    EIMPSSharedKeysPECClientLoginLogoutStateChannel = KIMPSSharedKeysConnectionUIBase + 6,
    EIMPSSharedKeysPECLoginLogoutEventChannel = KIMPSSharedKeysConnectionUIBase + 7,
    EIMPSSharedKeysPECSSClientReqistrationChannel = KIMPSSharedKeysConnectionUIBase + 8,
    EIMPSSharedKeysPECGlobalOperationSignalChannel = KIMPSSharedKeysConnectionUIBase + 9,
    EIMPSSharedKeysPECRemoteUiNotificationsChannel = KIMPSSharedKeysConnectionUIBase + 10,

    EIMPSSharedKeysCommonClientLoginLogoutStateChannel = KIMPSSharedKeysConnectionUIBase + 11,
    EIMPSSharedKeysCommonLoginLogoutEventChannel = KIMPSSharedKeysConnectionUIBase + 12,
    EIMPSSharedKeysCommonSSClientReqistrationChannel = KIMPSSharedKeysConnectionUIBase + 13,
    EIMPSSharedKeysCommonGlobalOperationSignalChannel = KIMPSSharedKeysConnectionUIBase + 14,
    EIMPSSharedKeysCommonRemoteUiNotificationsChannel = KIMPSSharedKeysConnectionUIBase + 15,

    // Connection UI Group data keys

    EIMPSSharedKeysIMClientLoginLogoutStateData = KIMPSSharedKeysConnectionUIBase + 41,
    EIMPSSharedKeysIMLoginLogoutEventData = KIMPSSharedKeysConnectionUIBase + 42,
    EIMPSSharedKeysIMSSClientReqistrationData = KIMPSSharedKeysConnectionUIBase + 43,
    EIMPSSharedKeysIMGlobalOperationSignalData = KIMPSSharedKeysConnectionUIBase + 44,
    EIMPSSharedKeysIMRemoteUiNotificationsData = KIMPSSharedKeysConnectionUIBase + 45,

    EIMPSSharedKeysPECClientLoginLogoutStateData = KIMPSSharedKeysConnectionUIBase + 46,
    EIMPSSharedKeysPECLoginLogoutEventData = KIMPSSharedKeysConnectionUIBase + 47,
    EIMPSSharedKeysPECSSClientReqistrationData = KIMPSSharedKeysConnectionUIBase + 48,
    EIMPSSharedKeysPECGlobalOperationSignalData = KIMPSSharedKeysConnectionUIBase + 49,
    EIMPSSharedKeysPECRemoteUiNotificationsData = KIMPSSharedKeysConnectionUIBase + 50,

    EIMPSSharedKeysCommonClientLoginLogoutStateData = KIMPSSharedKeysConnectionUIBase + 51,
    EIMPSSharedKeysCommonLoginLogoutEventData = KIMPSSharedKeysConnectionUIBase + 52,
    EIMPSSharedKeysCommonSSClientReqistrationData = KIMPSSharedKeysConnectionUIBase + 53,
    EIMPSSharedKeysCommonGlobalOperationSignalData = KIMPSSharedKeysConnectionUIBase + 54,
    EIMPSSharedKeysCommonRemoteUiNotificationsData = KIMPSSharedKeysConnectionUIBase + 55,

    // Connection UI Global channel keys

    EIMPSSharedKeysIMGlobalChannel = KIMPSSharedKeysConnectionUIBase + 71,
    EIMPSSharedKeysPECGlobalChannel = KIMPSSharedKeysConnectionUIBase + 72,

    // Connection UI Global data keys

    EIMPSSharedKeysIMGlobalData = KIMPSSharedKeysConnectionUIBase + 81,
    EIMPSSharedKeysPECGlobalData = KIMPSSharedKeysConnectionUIBase + 82,

    // Connection UI pernanent global data key
    EIMPSSharedKeysPermanentGlobalChannel = KIMPSSharedKeysConnectionUIBase + 83,
    EIMPSSharedKeysPermanentGlobalData = KIMPSSharedKeysConnectionUIBase + 84,

    // Connection UI IM Presence Auth Setting Key
    EIMPSSharedKeysIMPresenceAuthSettingKey = 0x00000004,
    EIMPSSharedKeysIMStatusMsgOnlineKey     = 0x0000000e,

    // Profile engines key
    EIMPSSharedKeysProEngAllowUpdatePresence = 0x80000200,
    // Presence UI keys

    EIMPSSharedKeysPECAppKeyNamePublishing = 0x00000001,

    // IM keys
    EIMPSSharedKeysIMUserId             = 0x0000000a,
    EIMPSSharedKeysIMAliasName			= 0x0000000b,


    EIMPSSharedKeysRoaming,
    EIMPSSharedKeysRoamingDlgShown
    };

// CLASS DECLARATION

/**
*  Shared data handler
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( MIMPSSharedData )
    {
public: // New functions

    /**
    * Subscribes to listen for change of a key
    * @param aUid       TUid identifying the shared data file that this
    *                   request is involved with.
    * @param aKey       Keyword of which changes are to be observed.
    * @return error code
    */
    virtual TInt SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey ) = 0;

    /**
    * Subscribes to listen for set of a key
    * @param aUid       TUid identifying the shared data file that this
    *                   request is involved with.
    * @param aKey       Keyword of which changes are to be observed.
    * @return error code
    */
    virtual TInt SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey ) = 0;

    /**
    * Unsubscribes a request to listen for change of a key
    * @param aUid       TUid identifying the shared data file that this
    *                   request is involved with.
    * @param aKey       Keyword of which changes are to be observed.
    * @return error code
    */
    virtual void UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey ) = 0;

    /**
    * Gets the string value corresponding the given key.
    * @param aKey       keyword
    * @param aValue     descriptor containing the string
    *                   associated with the keyword
    * @return error code
    */
    virtual TInt GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue ) = 0;

    /**
    * Gets the integer value corresponding the given key.
    * @param aKey keyword
    * @param aValue integer value associated with the keyword
    * @return error code
    */
    virtual TInt GetIntKey( const TIMPSSharedKeys aKey, TInt& aValue ) = 0;

    /**
    * Sets the string value associated with the given key.
    * @param aKey keyword
    * @param aValue integer value associated with the keyword
    * @return error code
    */
    virtual TInt SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue ) = 0;

    /**
    * Sets the integer value associated with the given key.
    * @param aKey keyword
    * @param aValue integer value associated with the keyword
    * @return error code
    */
    virtual TInt SetIntKey( const TIMPSSharedKeys aKey, TInt aValue ) = 0;

    /**
    * Signals the given key.
    * @param aKey keyword
    * @return error code
    */
    virtual TInt Signal( const TIMPSSharedKeys aKey ) = 0;

    /**
    * Cancels the signals to the given key.
    * @param aKey keyword
    * @return error code
    */
    virtual TInt CancelSignal( const TIMPSSharedKeys aKey ) = 0;

    /**
    * Converts shared data keys to TIMPSSharedKeys
    * @param aSrc source
    * @param aDest destination
    * @return error code
    */
    virtual TInt ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest ) = 0;

    /**
    * Destructor
    */
    virtual ~MIMPSSharedData() {};

    };

#endif      // __MIMPSSHAREDDATA_H

// End of File
