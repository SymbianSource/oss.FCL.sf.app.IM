/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Definitions for engine's internal use.
*
*/


#ifndef IMPSSERVERCOMMON_H
#define IMPSSERVERCOMMON_H

#include "impsconst.h"

// CONSTANTS

const TUint KImpsServMajorVersionNumber = 3;
const TUint KImpsServMinorVersionNumber = 0;
const TUint KImpsServBuildVersionNumber = 0;

const TInt KImpsNullId = -1;

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;
const TUint KMaxHeapSize = 0x100000; // This is the default as in ARM

_LIT( KImpsServerName, "!CImpsServer" );
_LIT( KImpsServerExe, "impsexe.exe" );
_LIT( KImpsLibName, "impseng.dll" );

// Maximum pending messages allowed for transport
// Notice: This limits outgoing messages regardless of
//         MultiTrans in CSP session
#ifdef _FAKE_RESPONSE
const TInt KImpsMaxPending = 1;
#else
// 4=HTTP stack queue limit, try to avoid problems
const TInt KImpsMaxPending = 4;
#endif

// Default size for packed and streamed messages in Symbian client-server
const TInt KImpsStreamSize = 500;

// Threshold for IPC client buffer in bytes.
// If the current message size exceeds this limit then the buffer
// is deleted after the message is handled in a client handle.
// This buffer speeds up delivery of short messages by avoiding
// extra client-server reaquests for message buffer allocation.
const TInt KImpsIPCThreshold = 1500;

_LIT( KImpsPanicCategory, "Imps" );
_LIT( KImpsVersionStr, "http://www.wireless-village.org/CSP1.1" );
_LIT( KImpsVersionStr12, "http://www.openmobilealliance.org/WV-CSP1.2" );
_LIT( KSubListHeader, "xmlns=\"http://www.wireless-village.org/PA1.1\"" );

_LIT( KImpsDataType, "text/plain" );
_LIT( KImpsDataAndCharset, "text/plain" );

#ifdef _DEBUG
// wvengine.ini is for DEBUG mode only to allow overwriting Cent Rep values
_LIT( KImpsIniFile, "\\system\\data\\wvengine.ini" );
#endif


// Maximum lengths of strings in WV protocol
const TInt KImpsMaxTID = 50;    // maximum Transaction ID length
const TInt KImpsMaxSID = 50;    // maximum Session ID
const TInt KImpsMaxUserId = 50;    // maximum User ID
const TInt KImpsMaxClientId = 64;  // maximum Client ID, this is not sure!
const TInt KImpsMaxSAP = 255;      // maximum SAP size - updated from 200 to 255
const TInt KImpsMaxPwd = 50;       // maximum password
const TInt KImpsMaxCookie = 50;    // maximum cookie size

// default values if not given in static settings in CentRep
// *********************************************************
const TInt KImpsAcceptedContentLength = 102400;  // 100kB
const TInt KImpsParserSizeMax = 131072;  // 128kB if previous is not bigger
const TInt KImpsMultiTrans = 3;
const TInt KImpsPollTime = 8;
const TInt KImpsAliveTime = 3600;
// maximum poll time that's used for adaptive polling in seconds
const TInt KImpsMaxPoll = 90;
// Poll request time-out in seconds
const TInt KImpsPollTO = 20;
// KeepAlive request time-out in seconds
const TInt KImpsKeepATO = 20;
// Default client request expiry time in seconds
const TInt KImpsClientExpr = 40;
const TInt KImpsFundClientExpr = 120;
const TInt KImpsPresClientExpr = 120;
// Default CIR expiry time in seconds
const TInt KImpsCIRExpr = 8;
// Default Poll time in CIR mode in seconds
const TInt KImpsCIRModePollTime = 86400;  //24h
// Maximum idle time for PDP contect in seconds until it is closed
const TInt KImpsPDPMaxIdleTime = 300;
// PDP open re-try time period in seconds
const TInt KImpsPDPRetryOpenTime = 60;
// *********************************************************

// status codes from IMPS server
const TInt32 KImpsStatusOk = 200;

// Mapping used between message type and keys
const TUint32 KImpsKeySession                       = 0x0000002D;
const TUint32 KImpsKeySessionDescriptor             = 0x0000002E;
const TUint32 KImpsKeySessionType                   = 0x00000030;
const TUint32 KImpsKeySessionID                     = 0x0000002F;
const TUint32 KImpsKeyTransaction                   = 0x00000032;
const TUint32 KImpsKeyTransactionDescriptor         = 0x00000034;
const TUint32 KImpsKeyTransactionMode               = 0x00000036;
const TUint32 KImpsKeyTransactionID                 = 0x00000035;
const TUint32 KImpsKeyPoll                          = 0x00000021;
const TUint32 KImpsKeyTransactionContent            = 0x00000033;
const TUint32 KImpsKeyStatus                        = 0x00000031;
const TUint32 KImpsKeyPolling_Request               = 0x00000122;
const TUint32 KImpsKeyLogin_Request                 = 0x0000011D;
const TUint32 KImpsKeyLogin_Response                = 0x0000011E;
const TUint32 KImpsKeyService_Request               = 0x0000012E;
const TUint32 KImpsKeyService_Response              = 0x0000012F;
const TUint32 KImpsKeyClientCapability_Request      = 0x0000010C;
const TUint32 KImpsKeyClientCapability_Response     = 0x0000010D;
const TUint32 KImpsKeyLogout_Request                = 0x0000011F;
const TUint32 KImpsKeyDisconnect                    = 0x00000110;
const TUint32 KImpsKeyKeepAlive_Request             = 0x0000011B;
const TUint32 KImpsKeyKeepAlive_Response            = 0x00000129;
const TUint32 KImpsKeyGetSPInfo_Request             = 0x00000112;
const TUint32 KImpsKeyGetSPInfo_Response            = 0x00000113;
const TUint32 KImpsKeySearch_Request                = 0x0000012B;
const TUint32 KImpsKeyStopSearch_Request            = 0x00000131;
const TUint32 KImpsKeySearch_Response               = 0x0000012C;
const TUint32 KImpsKeyCompletionFlag                = 0x00000134;
const TUint32 KImpsKeyInvite_Request                = 0x00000116;
const TUint32 KImpsKeyInvite_Response               = 0x00000117;
const TUint32 KImpsKeyInviteUser_Request            = 0x00000119;
const TUint32 KImpsKeyInviteUser_Response           = 0x0000011A;
const TUint32 KImpsKeyCancelInvite_Request          = 0x00000107;
const TUint32 KImpsKeyCancelInviteUser_Request      = 0x00000108;
const TUint32 KImpsKeySubscribePresence_Request     = 0x0000041D;
const TUint32 KImpsKeyUnsubscribePresence_Request   = 0x00000418;
const TUint32 KImpsKeyPresenceNotification_Request  = 0x0000041B;
const TUint32 KImpsKeyGetWatcherList_Request        = 0x00000414;
const TUint32 KImpsKeyGetWatcherList_Response       = 0x00000415;
const TUint32 KImpsKeyGetPresence_Request           = 0x00000412;
const TUint32 KImpsKeyGetPresence_Response          = 0x00000413;
const TUint32 KImpsKeyPresenceAuth_Request          = 0x00000419;
const TUint32 KImpsKeyPresenceAuth_User             = 0x0000041A;
const TUint32 KImpsKeyCancelAuth_Request            = 0x00000405;
const TUint32 KImpsKeyUpdatePresence_Request        = 0x0000041C;
const TUint32 KImpsKeyGetList_Request               = 0x00000410;
const TUint32 KImpsKeyGetList_Response              = 0x00000411;
const TUint32 KImpsKeyCreateList_Request            = 0x00000408;
const TUint32 KImpsKeyDeleteList_Request            = 0x0000040D;
const TUint32 KImpsKeyListManage_Request            = 0x00000416;
const TUint32 KImpsKeyListManage_Response           = 0x00000417;
const TUint32 KImpsKeyCreateAttributeList_Request   = 0x00000407;
const TUint32 KImpsKeyDeleteAttributeList_Request   = 0x0000040C;
const TUint32 KImpsKeyGetAttributeList_Request      = 0x0000040E;
const TUint32 KImpsKeyGetAttributeList_Response     = 0x0000040F;
const TUint32 KImpsKeySendMessage_Request           = 0x00000617;
const TUint32 KImpsKeySendMessage_Response          = 0x00000618;
const TUint32 KImpsKeySetDeliveryMethod_Request     = 0x00000619;
const TUint32 KImpsKeyGetMessageList_Request        = 0x0000060D;
const TUint32 KImpsKeyGetMessageList_Response       = 0x0000060E;
const TUint32 KImpsKeyRejectMessage_Request         = 0x00000616;
const TUint32 KImpsKeyMessageNotification           = 0x00000614;
const TUint32 KImpsKeyGetMessage_Request            = 0x0000060F;
const TUint32 KImpsKeyGetMessage_Response           = 0x00000610;
const TUint32 KImpsKeyNewMessage                    = 0x00000615;
const TUint32 KImpsKeyMessageDelivered              = 0x00000612;
const TUint32 KImpsKeyDeliveryReport_Request        = 0x00000609;
const TUint32 KImpsKeyDeliveryTime                  = 0x0000061A;
const TUint32 KImpsKeyForwardMessage_Request        = 0x0000060A;
const TUint32 KImpsKeyGetBlockedList_Request        = 0x0000060B;
const TUint32 KImpsKeyGetBlockedList_Response       = 0x0000060C;
const TUint32 KImpsKeyBlockEntity_Request           = 0x00000606;
const TUint32 KImpsKeyCreateGroup_Request           = 0x00000707;
const TUint32 KImpsKeyDeleteGroup_Request           = 0x00000708;
const TUint32 KImpsKeyJoinGroup_Request             = 0x00000711;
const TUint32 KImpsKeyJoinGroup                     = 0x00000721;
const TUint32 KImpsKeySubscribeNotification         = 0x00000722;
const TUint32 KImpsKeyJoinGroup_Response            = 0x00000712;
const TUint32 KImpsKeyLeaveGroup_Request            = 0x00000713;
const TUint32 KImpsKeyLeaveGroup_Response           = 0x00000714;
const TUint32 KImpsKeyGetGroupMembers_Request       = 0x00000709;
const TUint32 KImpsKeyGetGroupMembers_Response      = 0x0000070A;
const TUint32 KImpsKeyAddGroupMembers_Request       = 0x00000705;
const TUint32 KImpsKeyRemoveGroupMembers_Request    = 0x0000071B;
const TUint32 KImpsKeyMemberAccess_Request          = 0x00000716;
const TUint32 KImpsKeyGetGroupProps_Request         = 0x0000070B;
const TUint32 KImpsKeyGetGroupProps_Response        = 0x0000070C;
const TUint32 KImpsKeySetGroupProps_Request         = 0x0000071C;
const TUint32 KImpsKeyRejectList_Request            = 0x00000719;
const TUint32 KImpsKeyRejectList_Response           = 0x0000071A;
const TUint32 KImpsKeySubscribeGroupNotice_Request  = 0x0000071D;
const TUint32 KImpsKeySubscribeGroupNotice_Response = 0x0000071E;
const TUint32 KImpsKeyGroupChangeNotice             = 0x0000070D;
const TUint32 KImpsKeyDigestBytes                   = 0x0000010E;
const TUint32 KImpsKeyDigestSchema                  = 0x0000010F;
const TUint32 KImpsKeyPassword                      = 0x00000121;
const TUint32 KImpsKeyNonce                         = 0x00000120;
const TUint32 KImpsKeyCapabilityRequest             = 0x0000010B;
const TUint32 KImpsKeyCapabilityList                = 0x0000010A;
const TUint32 KImpsKeyClientType                    = 0x0000050F;
const TUint32 KImpsKeyInitialDeliveryMethod         = 0x0000030B;
const TUint32 KImpsKeyAnyContent                    = 0x00000309;
const TUint32 KImpsKeyAcceptedCharSet               = 0x00000305;
const TUint32 KImpsKeyAcceptedContentType           = 0x00000307;
const TUint32 KImpsKeyAcceptedTransferEncoding      = 0x00000308;
const TUint32 KImpsKeyAcceptedContentLength         = 0x00000306;
const TUint32 KImpsKeySupportedBearer               = 0x0000030F;
const TUint32 KImpsKeyMultiTrans                    = 0x0000030C;
const TUint32 KImpsKeyParserSize                    = 0x0000030D;
const TUint32 KImpsKeySupportedCIRMethod            = 0x00000310;
const TUint32 KImpsKeyUDPPort                       = 0x00000313;
const TUint32 KImpsKeyTCPAddress                    = 0x00000311;
const TUint32 KImpsKeyTCPPort                       = 0x00000312;
const TUint32 KImpsKeyServerPollMin                 = 0x0000030E;
const TUint32 KImpsKeyDefaultLanguage               = 0x0000030A;
const TUint32 KImpsKeyResult                        = 0x0000002A;
const TUint32 KImpsKeyCode                          = 0x0000000B;
const TUint32 KImpsKeyDescription                   = 0x00000012;
const TUint32 KImpsKeyDetailedResult                = 0x00000013;
const TUint32 KImpsKeySessionCookie                 = 0x00000130;
const TUint32 KImpsKeySender                        = 0x0000002C;
const TUint32 KImpsKeyRecipient                     = 0x00000027;
const TUint32 KImpsKeyUser                          = 0x00000039;
const TUint32 KImpsKeyGroup                         = 0x00000015;
const TUint32 KImpsKeyUserID                        = 0x0000003A;
const TUint32 KImpsKeyClientID                      = 0x0000000A;
const TUint32 KImpsKeyGroupID                       = 0x00000016;
const TUint32 KImpsKeyMessageID                     = 0x0000001B;
const TUint32 KImpsKeyMessageURI                    = 0x0000001C;
const TUint32 KImpsKeyScreenName                    = 0x0000002B;
const TUint32 KImpsKeyNickName                      = 0x00000020;
const TUint32 KImpsKeyURLList                       = 0x00000038;
const TUint32 KImpsKeyURL                           = 0x00000037;
const TUint32 KImpsKeyMSISDN                        = 0x0000001D;
const TUint32 KImpsKeyGroupList                     = 0x00000017;
const TUint32 KImpsKeyUserList                      = 0x0000003B;
const TUint32 KImpsKeyContactList                   = 0x0000000C;
const TUint32 KImpsKeyDefaultContactList            = 0x0000040A;
const TUint32 KImpsKeyNickList                      = 0x0000001F;
const TUint32 KImpsKeyAddNickList                   = 0x00000007;
const TUint32 KImpsKeyRemoveNickList                = 0x00000029;
const TUint32 KImpsKeyDefaultList                   = 0x0000040B;
const TUint32 KImpsKeySearchPairList                = 0x0000012A;
const TUint32 KImpsKeySearchElement                 = 0x00000124;
const TUint32 KImpsKeySearchString                  = 0x00000133;
const TUint32 KImpsKeySearchLimit                   = 0x00000128;
const TUint32 KImpsKeySearchID                      = 0x00000126;
const TUint32 KImpsKeySearchIndex                   = 0x00000127;
const TUint32 KImpsKeySearchFindings                = 0x00000125;
const TUint32 KImpsKeySearchResult                  = 0x0000012D;
const TUint32 KImpsKeyInviteID                      = 0x00000114;
const TUint32 KImpsKeyInviteType                    = 0x00000118;
const TUint32 KImpsKeyInviteNote                    = 0x00000115;
const TUint32 KImpsKeyAcceptance                    = 0x00000005;
const TUint32 KImpsKeyResponseNote                  = 0x00000123;
const TUint32 KImpsKeyDefaultAttributeList          = 0x00000409;
const TUint32 KImpsKeyPresence                      = 0x00000022;
const TUint32 KImpsKeyPresenceSubList               = 0x00000023;
const TUint32 KImpsKeyKeepAliveTime                 = 0x0000011C;
const TUint32 KImpsKeyTimeToLive                    = 0x00000132;
const TUint32 KImpsKeySName                         = 0x00000008;
const TUint32 KImpsKeyName                          = 0x0000001E;
const TUint32 KImpsKeyValue                         = 0x0000003D;
const TUint32 KImpsKeyValidity                      = 0x0000003C;
const TUint32 KImpsKeyMessageInfo                   = 0x00000613;
const TUint32 KImpsKeyDateTime                      = 0x00000011;
const TUint32 KImpsKeyMessageCount                  = 0x0000001A;
const TUint32 KImpsKeyDeliveryReport                = 0x00000608;
const TUint32 KImpsKeyDeliveryMethod                = 0x00000607;
const TUint32 KImpsKeyBlockList                     = 0x00000605;
const TUint32 KImpsKeyGrantList                     = 0x00000611;
const TUint32 KImpsKeyEntityList                    = 0x00000014;
const TUint32 KImpsKeyAddList                       = 0x00000006;
const TUint32 KImpsKeyRemoveList                    = 0x00000028;
const TUint32 KImpsKeyInUse                         = 0x00000018;
const TUint32 KImpsKeyContactListProperties         = 0x00000406;
const TUint32 KImpsKeyGroupProperties               = 0x0000070E;
const TUint32 KImpsKeyOwnProperties                 = 0x00000718;
const TUint32 KImpsKeyProperty                      = 0x00000025;
const TUint32 KImpsKeyWelcomeNote                   = 0x00000720;
const TUint32 KImpsKeyJoinedRequest                 = 0x00000710;
const TUint32 KImpsKeyAdmin                         = 0x00000706;
const TUint32 KImpsKeyMod                           = 0x00000717;
const TUint32 KImpsKeyUsers                         = 0x0000071F;
const TUint32 KImpsKeySubscribeType                 = 0x00000723;
const TUint32 KImpsKeyJoined                        = 0x0000070F;
const TUint32 KImpsKeyLeft                          = 0x00000715;
const TUint32 KImpsKeyLogo                          = 0x00000019;
const TUint32 KImpsKeyContentType                   = 0x00000010;
const TUint32 KImpsKeyContentData                   = 0x0000000D;
const TUint32 KImpsKeyContentSize                   = 0x0000000F;
const TUint32 KImpsKeyContentEncoding               = 0x0000000E;
const TUint32 KImpsKeyAllFunctionsRequest           = 0x00000106;
const TUint32 KImpsKeyAllFunctions                  = 0x00000105;
const TUint32 KImpsKeyFunctions                     = 0x00000111;
const TUint32 KImpsKeyWVCSPFeat                     = 0x0000023C;
const TUint32 KImpsKeyFundamentalFeat               = 0x00000211;
const TUint32 KImpsKeyPresenceFeat                  = 0x0000022F;
const TUint32 KImpsKeyIMFeat                        = 0x00000223;
const TUint32 KImpsKeyGroupFeat                     = 0x0000021F;
const TUint32 KImpsKeyServiceFunc                   = 0x00000235;
const TUint32 KImpsKeySearchFunc                    = 0x00000234;
const TUint32 KImpsKeyInviteFunc                    = 0x00000227;
const TUint32 KImpsKeyContListFunc                  = 0x0000020C;
const TUint32 KImpsKeyPresenceAuthFunc              = 0x0000022D;
const TUint32 KImpsKeyPresenceDeliverFunc           = 0x0000022E;
const TUint32 KImpsKeyAttListFunc                   = 0x00000206;
const TUint32 KImpsKeyIMSendFunc                    = 0x00000225;
const TUint32 KImpsKeyIMReceiveFunc                 = 0x00000224;
const TUint32 KImpsKeyIMAuthFunc                    = 0x00000222;
const TUint32 KImpsKeyGroupMgmtFunc                 = 0x00000220;
const TUint32 KImpsKeyGroupUseFunc                  = 0x00000221;
const TUint32 KImpsKeyGroupAuthFunc                 = 0x0000021E;
const TUint32 KImpsKeyGETSPI                        = 0x0000021A;
const TUint32 KImpsKeySRCH                          = 0x00000238;
const TUint32 KImpsKeySTSRC                         = 0x00000239;
const TUint32 KImpsKeyINVIT                         = 0x00000226;
const TUint32 KImpsKeyCAINV                         = 0x00000209;
const TUint32 KImpsKeyGCLI                          = 0x00000214;
const TUint32 KImpsKeyCCLI                          = 0x0000020B;
const TUint32 KImpsKeyDCLI                          = 0x0000020F;
const TUint32 KImpsKeyMCLS                          = 0x00000229;
const TUint32 KImpsKeyGETWL                         = 0x0000021B;
const TUint32 KImpsKeyREACT                         = 0x00000230;
const TUint32 KImpsKeyCAAUT                         = 0x00000208;
const TUint32 KImpsKeyGETPR                         = 0x00000219;
const TUint32 KImpsKeyUPDPR                         = 0x0000023B;
const TUint32 KImpsKeyCALI                          = 0x0000020A;
const TUint32 KImpsKeyDALI                          = 0x0000020E;
const TUint32 KImpsKeyGALS                          = 0x00000213;
const TUint32 KImpsKeyMDELIV                        = 0x0000022A;
const TUint32 KImpsKeyFWMSG                         = 0x00000212;
const TUint32 KImpsKeySETD                          = 0x00000236;
const TUint32 KImpsKeyGETLM                         = 0x00000217;
const TUint32 KImpsKeyGETM                          = 0x00000218;
const TUint32 KImpsKeyREJCM                         = 0x00000231;
const TUint32 KImpsKeyNOTIF                         = 0x0000022C;
const TUint32 KImpsKeyNEWM                          = 0x0000022B;
const TUint32 KImpsKeyGLBLU                         = 0x0000021C;
const TUint32 KImpsKeyBLENT                         = 0x00000207;
const TUint32 KImpsKeyCREAG                         = 0x0000020D;
const TUint32 KImpsKeyDELGR                         = 0x00000210;
const TUint32 KImpsKeyGETGP                         = 0x00000216;
const TUint32 KImpsKeySETGP                         = 0x00000237;
const TUint32 KImpsKeySUBGCN                        = 0x0000023A;
const TUint32 KImpsKeyGRCHN                         = 0x0000021D;
const TUint32 KImpsKeyGETGM                         = 0x00000215;
const TUint32 KImpsKeyADDGM                         = 0x00000205;
const TUint32 KImpsKeyRMVGM                         = 0x00000233;
const TUint32 KImpsKeyMBRAC                         = 0x00000228;
const TUint32 KImpsKeyREJEC                         = 0x00000232;

const TUint32 KImpsKeyPRQualifier				    = 0x00000026;
const TUint32 KImpsKeyPRPresenceValue               = 0x00000024;
const TUint32 KImpsKeyPROnlineStatus                = 0x00000521;
const TUint32 KImpsKeyPRRegistration                = 0x00000528;
const TUint32 KImpsKeyPRFreeTextLocation            = 0x0000051A;
const TUint32 KImpsKeyPRPLMN                        = 0x00000522;
const TUint32 KImpsKeyPRUserAvailability            = 0x0000052E;
const TUint32 KImpsKeyPRPreferredLanguage           = 0x00000525;
const TUint32 KImpsKeyPRStatusText                  = 0x0000052B;
const TUint32 KImpsKeyPRStatusMood                  = 0x0000052A;
const TUint32 KImpsKeyPRAlias                       = 0x00000508;
const TUint32 KImpsKeyPRClientInfo                  = 0x0000050D;
const TUint32 KImpsKeyPRClientType                  = 0x0000050F;
const TUint32 KImpsKeyPRDevManufacturer             = 0x00000518;
const TUint32 KImpsKeyPRClientProducer              = 0x0000050E;
const TUint32 KImpsKeyPRModel                       = 0x0000051F;
const TUint32 KImpsKeyPRClientVersion               = 0x00000510;
const TUint32 KImpsKeyPRLanguage                    = 0x0000051C;
const TUint32 KImpsKeyPRTimeZone                    = 0x0000052D;
const TUint32 KImpsKeyPRZone                        = 0x00000535;
const TUint32 KImpsKeyPRGeoLocation                 = 0x0000051B;
const TUint32 KImpsKeyPRLongitude                   = 0x0000051E;
const TUint32 KImpsKeyPRLatitude                    = 0x0000051D;
const TUint32 KImpsKeyPRAltitude                    = 0x00000509;
const TUint32 KImpsKeyPRAccuracy                    = 0x00000505;
const TUint32 KImpsKeyPRAddress                     = 0x00000506;
const TUint32 KImpsKeyPRCountry                     = 0x00000515;
const TUint32 KImpsKeyPRCity                        = 0x0000050C;
const TUint32 KImpsKeyPRStreet                      = 0x0000052C;
const TUint32 KImpsKeyPRCrossing1                   = 0x00000516;
const TUint32 KImpsKeyPRCrossing2                   = 0x00000517;
const TUint32 KImpsKeyPRBuilding                    = 0x0000050A;
const TUint32 KImpsKeyPRNamedArea                   = 0x00000520;
const TUint32 KImpsKeyPRCommCap                     = 0x00000512;
const TUint32 KImpsKeyPRCommC                       = 0x00000511;
const TUint32 KImpsKeyPRCap                         = 0x0000052F;
const TUint32 KImpsKeyPRStatus                      = 0x00000031; //???????????????
const TUint32 KImpsKeyPRContact                     = 0x00000531;
const TUint32 KImpsKeyPRNote                        = 0x00000534;
const TUint32 KImpsKeyPRPreferredContacts           = 0x00000524;
const TUint32 KImpsKeyPRAddrPref                    = 0x00000507;
const TUint32 KImpsKeyPRPrefC                       = 0x00000523;
const TUint32 KImpsKeyPRCaddr                       = 0x0000050B;
const TUint32 KImpsKeyPRCstatus                     = 0x00000533;
const TUint32 KImpsKeyPRCname                       = 0x00000530;
const TUint32 KImpsKeyPRCpriority                   = 0x00000532;
const TUint32 KImpsKeyPRStatusContent               = 0x00000529;
const TUint32 KImpsKeyPRDirectContent               = 0x00000519;
const TUint32 KImpsKeyPRReferredContent             = 0x00000526;
const TUint32 KImpsKeyPRContactInfo                 = 0x00000513;
const TUint32 KImpsKeyPRContainedvCard              = 0x00000514;
const TUint32 KImpsKeyPRReferredvCard               = 0x00000527;

//////////////////////////////////////////////////////////////////////////
// CSP 1.2 additions
const TUint32 KImpsKeyAgreedCapabilityList          = 0x0000013A;
const TUint32 KImpsKeyExtended_Request              = 0x00000138;
const TUint32 KImpsKeyExtended_Response             = 0x00000139;
const TUint32 KImpsKeyExtendedData                  = 0x0000013B;
const TUint32 KImpsKeyOtherServer                   = 0x0000013C;
const TUint32 KImpsKeyPresenceAttributeNSName       = 0x0000013D;
const TUint32 KImpsKeyReceiveList                   = 0x00000136;
const TUint32 KImpsKeySessionNSName                 = 0x0000013E;
const TUint32 KImpsKeyTransactionNSName             = 0x0000013F;
const TUint32 KImpsKeyVerifyID_Request              = 0x00000137;
const TUint32 KImpsKeyMF                            = 0x0000023D;
const TUint32 KImpsKeyMG                            = 0x0000023E;
const TUint32 KImpsKeyMM                            = 0x0000023F;
const TUint32 KImpsKeyAuto_Subscribe                = 0x0000041E;
const TUint32 KImpsKeyGetReactiveAuthStatus_Request = 0x0000041F;
const TUint32 KImpsKeyGetReactiveAuthStatus_Response = 0x00000420;
const TUint32 KImpsKeyInf_link                      = 0x00000537;
const TUint32 KImpsKeyInfoLink                      = 0x00000538;
const TUint32 KImpsKeyLink                          = 0x00000539;
const TUint32 KImpsKeyText                          = 0x0000053A;
const TUint32 KImpsKeyAdminMapList                  = 0x00000726;
const TUint32 KImpsKeyAdminMapping                  = 0x00000727;
const TUint32 KImpsKeyGetJoinedUsers_Request        = 0x00000724;
const TUint32 KImpsKeyGetJoinedUsers_Respone        = 0x00000725;
const TUint32 KImpsKeyMapping                       = 0x00000728;
const TUint32 KImpsKeyUserMapList                   = 0x0000072A;
const TUint32 KImpsKeyUserMapping                   = 0x0000072B;
const TUint32 KImpsKeyGETAUT                        = 0x00000806;
const TUint32 KImpsKeyGETJU                         = 0x00000807;
const TUint32 KImpsKeyMP                            = 0x00000805;
const TUint32 KImpsKeyVRID                          = 0x00000808;
const TUint32 KImpsKeyVerifyIDFunc                  = 0x00000809;
const TUint32 KImpsKeyCIR                           = 0x00000905;
const TUint32 KImpsKeyDomain                        = 0x00000906;
const TUint32 KImpsKeyExtBlock                      = 0x00000907;
const TUint32 KImpsKeyHistoryPeriod                 = 0x00000908;
const TUint32 KImpsKeyIDList                        = 0x00000909;
const TUint32 KImpsKeyMaxWatcherList                = 0x0000090A;
const TUint32 KImpsKeyReactiveAuthState             = 0x0000090B;
const TUint32 KImpsKeyReactiveAuthStatus            = 0x0000090C;
const TUint32 KImpsKeyReactiveAuthStatusList        = 0x0000090D;
const TUint32 KImpsKeyWatcher                       = 0x0000090E;
const TUint32 KImpsKeyWatcherStatus                 = 0x0000090F;
const TUint32 KImpsKeyWV_CSPVersionDiscovery_Request  = 0x00000A05;
const TUint32 KImpsKeyWV_CSPVersionDiscovery_Response = 0x00000A06;
const TUint32 KImpsKeyVersionList                   = 0x00000A07;
//////////////////////////////////////////////////////////////////////////

const TUint32 KImpsKeyAPIClient                     = 0x0000FFFB;
const TUint32 KImpsKeyPureData                      = 0x0000FFFC;
const TUint32 KImpsKeyPRExt                         = 0x0000FFFD;
const TUint32 KImpsKeyPREND                         = 0x0000FFFE;
const TUint32 KImpsKeyEND                           = 0x0000FFFF;


// Event types (service type)
enum TImpsEventType
    {
    EImpsEventNone = 0x0000,
    EImpsEventServerLogin = 0x0001,
    EImpsEventMessage = 0x0002,
    EImpsEventPresence = 0x0004,
    EImpsEventGroup = 0x0010,
    EImpsEventCommon = 0x0100,
    EImpsEventPure = 0x01000,  // pure client
    EImpsEventPresencePure = 0x01004,
    EImpsEventAll = 0xFFFF     // special bit mask enum
    };


// Values for Message-Type field
enum TImpsMessageType
    {
    // Undefined
    EImpsMessageNone                   = 0,
    // Common
    EImpsStatus                        = KImpsKeyStatus,
    EImpsKeepAliveReq                  = KImpsKeyKeepAlive_Request,
    EImpsKeepAliveRes                  = KImpsKeyKeepAlive_Response,
    EImpsPolling                       = KImpsKeyPolling_Request,
    EImpsClientCapabilityReq           = KImpsKeyClientCapability_Request,
    EImpsClientCapabilityRes           = KImpsKeyClientCapability_Response,
    EImpsServiceReq                    = KImpsKeyService_Request,
    EImpsServiceRes                    = KImpsKeyService_Response,
    // Access
    EImpsLoginReq                      = KImpsKeyLogin_Request,
    EImpsLoginRes                      = KImpsKeyLogin_Response,
    EImpsLogoutReq                     = KImpsKeyLogout_Request,
    EImpsDisconnect                    = KImpsKeyDisconnect,
    // IM
    EImpsSendMessageReq                = KImpsKeySendMessage_Request,
    EImpsSendMessageRes                = KImpsKeySendMessage_Response,
    EImpsGetMessageReq                 = KImpsKeyGetMessage_Request,
    EImpsNewMessage                    = KImpsKeyNewMessage,
    EImpsMessageDelivered              = KImpsKeyMessageDelivered,
    EImpsGetBlockedListReq             = KImpsKeyGetBlockedList_Request,
    EImpsGetBlockedListRes             = KImpsKeyGetBlockedList_Response,
    EImpsBlockEntityReq                = KImpsKeyBlockEntity_Request,
    EImpsDeliveryReportReq             = KImpsKeyDeliveryReport_Request,
    // Presence
    EImpsSubscribePresenceReq          = KImpsKeySubscribePresence_Request,
    EImpsUnsubscribePresenceReq        = KImpsKeyUnsubscribePresence_Request,
    EImpsGetPresenceReq                = KImpsKeyGetPresence_Request,
    EImpsGetPresenceRes                = KImpsKeyGetPresence_Response,
    EImpsUpdatePresenceReq             = KImpsKeyUpdatePresence_Request,
    EImpsPresenceNotification          = KImpsKeyPresenceNotification_Request,
    EImpsCreateAttrListReq             = KImpsKeyCreateAttributeList_Request,
    EImpsPresenceAuthReq               = KImpsKeyPresenceAuth_Request,
    EImpsGetWatcherListReq             = KImpsKeyGetWatcherList_Request,
    EImpsGetWatcherListRes             = KImpsKeyGetWatcherList_Response,
    EImpsPresenceAuthUser              = KImpsKeyPresenceAuth_User,
    EImpsCancelAuthReq                 = KImpsKeyCancelAuth_Request,
    EImpsGetListReq                    = KImpsKeyGetList_Request,
    EImpsGetListRes                    = KImpsKeyGetList_Response,
    EImpsCreateListReq                 = KImpsKeyCreateList_Request,
    EImpsDeleteListReq                 = KImpsKeyDeleteList_Request,
    EImpsListManageReq                 = KImpsKeyListManage_Request,
    EImpsListManageRes                 = KImpsKeyListManage_Response,
    EImpsDeleteAttrListReq             = KImpsKeyDeleteAttributeList_Request,
    EImpsGetAttrListReq                = KImpsKeyGetAttributeList_Request,
    EImpsGetAttrListRes                = KImpsKeyGetAttributeList_Response,
    // CSP 1.2
    EImpsGetReactiveAuthStatusReq      = KImpsKeyGetReactiveAuthStatus_Request,
    EImpsGetReactiveAuthStatusRes      = KImpsKeyGetReactiveAuthStatus_Response,

    // Group
    EImpsCreateGroupReq                = KImpsKeyCreateGroup_Request,
    EImpsDeleteGroupReq                = KImpsKeyDeleteGroup_Request,
    EImpsJoinGroupReq                  = KImpsKeyJoinGroup_Request,
    EImpsJoinGroupRes                  = KImpsKeyJoinGroup_Response,
    EImpsLeaveGroupReq                 = KImpsKeyLeaveGroup_Request,
    EImpsLeaveGroupRes                 = KImpsKeyLeaveGroup_Response,
    EImpsGroupMembersReq               = KImpsKeyGetGroupMembers_Request,
    EImpsGroupMembersRes               = KImpsKeyGetGroupMembers_Response,
    EImpsAddGroupMembersReq            = KImpsKeyAddGroupMembers_Request,
    EImpsRemoveGroupMembersReq         = KImpsKeyRemoveGroupMembers_Request,
    EImpsGroupMemberAccessReq          = KImpsKeyMemberAccess_Request,
    EImpsGroupPropertiesReq            = KImpsKeyGetGroupProps_Request,
    EImpsGroupPropertiesRes            = KImpsKeyGetGroupProps_Response,
    EImpsSetGroupPropertiesReq         = KImpsKeySetGroupProps_Request,
    EImpsGroupRejectListReq            = KImpsKeyRejectList_Request,
    EImpsGroupRejectListRes            = KImpsKeyRejectList_Response,
    EImpsSubsGroupNoticeReq            = KImpsKeySubscribeGroupNotice_Request,
    EImpsSubsGroupNoticeRes            = KImpsKeySubscribeGroupNotice_Response,
    EImpsGroupChangeNotice             = KImpsKeyGroupChangeNotice,
    // CSP 1.2
    EImpsGetJoinedUsersReq             = KImpsKeyGetJoinedUsers_Request,
    EImpsGetJoinedUsersRes             = KImpsKeyGetJoinedUsers_Respone,


    // Fundamental
    EImpsSearchReq                     = KImpsKeySearch_Request,
    EImpsStopSearchReq                 = KImpsKeyStopSearch_Request,
    EImpsSearchRes                     = KImpsKeySearch_Response,
    EImpsInviteReq                     = KImpsKeyInvite_Request,
    EImpsInviteRes                     = KImpsKeyInvite_Response,
    EImpsInviteUserReq                 = KImpsKeyInviteUser_Request,
    EImpsInviteUserRes                 = KImpsKeyInviteUser_Response,
    EImpsCancelInviteReq               = KImpsKeyCancelInvite_Request,
    EImpsCancelInviteUserReq           = KImpsKeyCancelInviteUser_Request,
    // CSP 1.2
    EImpsVerifyIDReq                   = KImpsKeyVerifyID_Request,

    // Pure data
    EImpsPureData                      = KImpsKeyPureData,
    // THE FOLLOWING ONES ARE NOT REAL PRIMITIVES BUT USED
    // INTERNALLY IN ENGINE
    EImpsSubsGroupNoticeUnset          = KImpsKeyEND + 1,
    EImpsSubsGroupNoticeGet            = KImpsKeyEND + 2,
    EImpsSubsGroupNoticeSet            = KImpsKeyEND + 3,
    EImpsNewUsers                      = KImpsKeyEND + 4,
    EImpsLeftUsers                     = KImpsKeyEND + 5
    };

// This is also in client API with different name, but if we split into dlls
// then this is better to be in this way. Or move to impsconst.h.
enum EImpsInternalStatus
    {
    EInternal_ON_LINE,
    EInternal_OFF_LINE,
    EInternal_NOT_LOGGED,
    EInternal_NO_IAP,
    EInternal_NO_IAP_AUTH,
    EInternal_IAP_OPEN,
    EInternal_SHUTTING_DOWN
    };

// Signal in process set up
class TImpsSignal
    {
    public:
        inline TImpsSignal();
        inline TInt Set( const TDesC& aData );
        inline TImpsSignal( TInt aRate );
        inline TPtrC Get() const;
        TRequestStatus* iStatus;
        TThreadId iId;
        TInt iRate;
    };

inline TImpsSignal::TImpsSignal()
        : iStatus( NULL ), iRate( 0 ) {}

inline TImpsSignal::TImpsSignal( TInt aRate )
        : iStatus( NULL ), iId( RThread().Id() ), iRate( aRate )
    {  }

inline TPtrC TImpsSignal::Get() const
    {
    return TPtrC( ( const TText* )this, sizeof( *this ) / sizeof( TText ) );
    }

inline TInt TImpsSignal::Set( const TDesC& aData )
    {
    return aData.Size() != sizeof( *this ) ?
           KErrGeneral :
           ( Mem::Copy( this, aData.Ptr(), sizeof( *this ) ), KErrNone );
    }

// Parameter passing for the launcher process
class TImpsLaunchParams
    {
    public:
        inline TImpsLaunchParams () {};
        inline TImpsLaunchParams(
            const TDesC& aAppID,
            const TDesC& aSAP,
            const TDesC& aUserID );
        inline TInt Set( const TDesC& aData );
        inline TPtrC Get() const;

        // data
        TBuf<KImpsMaxClientId> 			iApplicationId;
        TBuf<KImpsMaxSAP>            	iSAP;
        TBuf<KImpsMaxUserId>         	iUserId;
    };

inline TImpsLaunchParams::TImpsLaunchParams( const TDesC& aAppID,
                                             const TDesC& aSAP,
                                             const TDesC& aUserID )
        : iApplicationId( aAppID ), iSAP( aSAP ),  iUserId( aUserID ) {}

inline TPtrC TImpsLaunchParams::Get() const
    {
    return TPtrC( ( const TText* )this, sizeof( *this ) / sizeof( TText ) );
    }

inline TInt TImpsLaunchParams::Set( const TDesC& aData )
    {
    return aData.Size() != sizeof( *this ) ?
           KErrGeneral :
           ( Mem::Copy( this, aData.Ptr(), sizeof( *this ) ), KErrNone );
    }
#endif      // ?INCLUDE_H   

// End of File
