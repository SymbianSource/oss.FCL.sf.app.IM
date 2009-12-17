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

#ifndef __IMPSSHAREDDATADEFS_H
#define __IMPSSHAREDDATADEFS_H

struct TIMPSSharedKeyPairs
    {
    TPtrC iKeyForSharedData;
    TIMPSSharedKeys iKeyForClient;
    };

struct TIMPSCenrepKeyPairs
    {
    TUint32 iKeyForCenrep;
    TIMPSSharedKeys iKeyForClient;
    };

// constants from connection ui keys
// GroupChannelKeyNames:
_LIT( KIMPSSharedKeysIMClientLoginLogoutStateChannel, "TMC2G0" );
_LIT( KIMPSSharedKeysIMLoginLogoutEventChannel, "TMC2G1" );
_LIT( KIMPSSharedKeysIMSSClientReqistrationChannel, "TMC2G2" );
_LIT( KIMPSSharedKeysIMGlobalOperationSignalChannel, "TMC2G3" );
_LIT( KIMPSSharedKeysIMRemoteUiNotificationsChannel, "TMC2G4" );
_LIT( KIMPSSharedKeysPECClientLoginLogoutStateChannel, "TMC1G0" );
_LIT( KIMPSSharedKeysPECLoginLogoutEventChannel, "TMC1G1" );
_LIT( KIMPSSharedKeysPECSSClientReqistrationChannel, "TMC1G2" );
_LIT( KIMPSSharedKeysPECGlobalOperationSignalChannel, "TMC1G3" );
_LIT( KIMPSSharedKeysPECRemoteUiNotificationsChannel, "TMC1G4" );

_LIT( KIMPSSharedKeysCommonClientLoginLogoutStateChannel, "TMC0G0" );
_LIT( KIMPSSharedKeysCommonLoginLogoutEventChannel, "TMC0G1" );
_LIT( KIMPSSharedKeysCommonSSClientReqistrationChannel, "TMC0G2" );
_LIT( KIMPSSharedKeysCommonGlobalOperationSignalChannel, "TMC0G3" );
_LIT( KIMPSSharedKeysCommonRemoteUiNotificationsChannel, "TMC0G4" );

// Connection UI Group data keys
_LIT( KIMPSSharedKeysIMClientLoginLogoutStateData, "TDC2G0" );
_LIT( KIMPSSharedKeysIMLoginLogoutEventData, "TDC2G1" );
_LIT( KIMPSSharedKeysIMSSClientReqistrationData, "TDC2G2" );
_LIT( KIMPSSharedKeysIMGlobalOperationSignalData, "TDC2G3" );
_LIT( KIMPSSharedKeysIMRemoteUiNotificationsData, "TDC2G4" );
_LIT( KIMPSSharedKeysPECClientLoginLogoutStateData, "TDC1G0" );
_LIT( KIMPSSharedKeysPECLoginLogoutEventData, "TDC1G1" );
_LIT( KIMPSSharedKeysPECSSClientReqistrationData, "TDC1G2" );
_LIT( KIMPSSharedKeysPECGlobalOperationSignalData, "TDC1G3" );
_LIT( KIMPSSharedKeysPECRemoteUiNotificationsData, "TDC1G4" );

_LIT( KIMPSSharedKeysCommonClientLoginLogoutStateData, "TDC0G0" );
_LIT( KIMPSSharedKeysCommonLoginLogoutEventData, "TDC0G1" );
_LIT( KIMPSSharedKeysCommonSSClientReqistrationData, "TDC0G2" );
_LIT( KIMPSSharedKeysCommonGlobalOperationSignalData, "TDC0G3" );
_LIT( KIMPSSharedKeysCommonRemoteUiNotificationsData, "TDC0G4" );

// Connection UI Global channel keys
_LIT( KIMPSSharedKeysIMGlobalChannel, "TGMCG2" );
_LIT( KIMPSSharedKeysPECGlobalChannel, "TGMCG1" );
// Connection UI Global data keys
_LIT( KIMPSSharedKeysIMGlobalData, "TGDCG2" );
_LIT( KIMPSSharedKeysPECGlobalData, "TGDCG1" );
// Connection UI permanent global data key
_LIT( KIMPSSharedKeysPermanentGlobalChannel, "PGMC0" );
_LIT( KIMPSSharedKeysPermanentGlobalData, "PGDC0" );

// Connection UI IM Presence Auth Setting Key
_LIT( KIPMSSharedKeysIMPresenceAuthSettingKey, "1001" );

// Profiles engine key for PubSub internal usage
_LIT( KIMPSSharedKeysProEngAllowUpdatePresence, "PresUpdAlw" );

//  Presence UI key for PubSub internal usage
_LIT( KIMPSSharedKeysPECAppKeyNamePublishing, "Publ" );

//  Service settings login type change notifier key for pec
_LIT( KIMPSSharedKeysServiceSettingsLoginTypesChangedPEC, "LCPEC" );
//  Service settings login type change notifier key for IM
_LIT( KIMPSSharedKeysServiceSettingsLoginTypesChangedIM, "LCIM" );

//  Service settings schedule change notifier key for pec
_LIT( KIMPSSharedKeysServiceSettingsScheduleChangedPEC, "SCHEPEC" );
//  Service settings schedule change notifier key for IM
_LIT( KIMPSSharedKeysServiceSettingsScheduleChangedIM, "SCHEIM" );



// Maximun length of keys name is 15 characters
const TInt KMaxKeyName = 15;

// maximun size of the value
const TInt KMaxValueSize = 512;

#endif      // __IMPSSHAREDDATADEFS_H

// End of File
