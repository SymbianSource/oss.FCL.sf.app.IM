/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Some classes for describing imps internel data structure
* 
*/


#ifndef MImpsDataAccessorApi_H
#define MImpsDataAccessorApi_H

//  INCLUDES
#include "Impsservercommon.h"

// CONSTANTS

enum TImpsDataType
    {
    EImpsDataTypeNone = 0,   // This is for elements not having value in DTD
    EImpsDataTypeInt = 1,
    EImpsDataTypeDesc = 2,
    EImpsDataTypeDesc8 = 3,
    EImpsDataTypeBoolean = 4,
    EImpsDataTypeNotSupported = 5,   // Data accessor does not support this
    EImpsDataTypeExt = 6,    // Data type for extension attributes
    EImpsDataTypeContentData = 7    // special data type for content data handling
    };

enum TImpsKeyType
    {
    EImpsKeyTypeIM = 0,  // IM
    EImpsKeyTypePre = 1  // Presence
    };


// CLASS DECLARATION
class CImpsFields;
class RFs;

// MACROS
#define GET_INDEX( aElemvalue ) ( (aElemvalue>>16 ) & 0xFFFF )
#define GET_ENUM( aElemvalue ) ( aElemvalue& 0xFFFF )
#define CREATEKEY( elem, index ) ( ( elem& 0xFFFF ) + ( index<< 16 ) )
// if Upper bit is one then we have presence attribute
#define IS_PRESENCE( aElemvalue ) ( aElemvalue>>31 )

// DATA TYPES

enum TImpsPresenceContent
    {
    EImpsKeyPRPresenceSubList                           = KImpsKeyPresenceSubList, // duplicate!!!
    EImpsKeyPRQualifier                      			= KImpsKeyPRQualifier,
    EImpsKeyPRPresenceValue                             = KImpsKeyPRPresenceValue,
    EImpsKeyPROnlineStatus                              = KImpsKeyPROnlineStatus,
    EImpsKeyPRRegistration                              = KImpsKeyPRRegistration,
    EImpsKeyPRFreeTextLocation                          = KImpsKeyPRFreeTextLocation,
    EImpsKeyPRPLMN                                      = KImpsKeyPRPLMN,
    EImpsKeyPRUserAvailability                          = KImpsKeyPRUserAvailability,
    EImpsKeyPRPreferredLanguage                         = KImpsKeyPRPreferredLanguage,
    EImpsKeyPRStatusText                                = KImpsKeyPRStatusText,
    EImpsKeyPRStatusMood                                = KImpsKeyPRStatusMood,
    EImpsKeyPRAlias                                     = KImpsKeyPRAlias,
    EImpsKeyPRClientInfo                                = KImpsKeyPRClientInfo,
    EImpsKeyPRClientType                                = KImpsKeyClientType,
    EImpsKeyPRDevManufacturer                           = KImpsKeyPRDevManufacturer,
    EImpsKeyPRClientProducer                            = KImpsKeyPRClientProducer,
    EImpsKeyPRModel                                     = KImpsKeyPRModel,
    EImpsKeyPRClientVersion                             = KImpsKeyPRClientVersion,
    EImpsKeyPRLanguage                                  = KImpsKeyPRLanguage,
    EImpsKeyPRTimeZone                                  = KImpsKeyPRTimeZone,
    EImpsKeyPRZone                                      = KImpsKeyPRZone,
    EImpsKeyPRGeoLocation                               = KImpsKeyPRGeoLocation,
    EImpsKeyPRLongitude                                 = KImpsKeyPRLongitude,
    EImpsKeyPRLatitude                                  = KImpsKeyPRLatitude,
    EImpsKeyPRAltitude                                  = KImpsKeyPRAltitude,
    EImpsKeyPRAccuracy                                  = KImpsKeyPRAccuracy,
    EImpsKeyPRAddress                                   = KImpsKeyPRAddress,
    EImpsKeyPRCountry                                   = KImpsKeyPRCountry,
    EImpsKeyPRCity                                      = KImpsKeyPRCity,
    EImpsKeyPRStreet                                    = KImpsKeyPRStreet,
    EImpsKeyPRCrossing1                                 = KImpsKeyPRCrossing1,
    EImpsKeyPRCrossing2                                 = KImpsKeyPRCrossing2,
    EImpsKeyPRBuilding                                  = KImpsKeyPRBuilding,
    EImpsKeyPRNamedArea                                 = KImpsKeyPRNamedArea,
    EImpsKeyPRCommCap                                   = KImpsKeyPRCommCap,
    EImpsKeyPRCommC                                     = KImpsKeyPRCommC,
    EImpsKeyPRCap                                       = KImpsKeyPRCap,
    EImpsKeyPRStatus                                    = KImpsKeyPRStatus,
    EImpsKeyPRContact                                   = KImpsKeyPRContact,
    EImpsKeyPRNote                                      = KImpsKeyPRNote,
    EImpsKeyPRPreferredContacts                         = KImpsKeyPRPreferredContacts,
    EImpsKeyPRAddrPref                                  = KImpsKeyPRAddrPref,
    EImpsKeyPRPrefC                                     = KImpsKeyPRPrefC,
    EImpsKeyPRCaddr                                     = KImpsKeyPRCaddr,
    EImpsKeyPRCstatus                                   = KImpsKeyPRCstatus,
    EImpsKeyPRCname                                     = KImpsKeyPRCname,
    EImpsKeyPRCpriority                                 = KImpsKeyPRCpriority,
    EImpsKeyPRStatusContent                             = KImpsKeyPRStatusContent,
    EImpsKeyPRDirectContent                             = KImpsKeyPRDirectContent,
    EImpsKeyPRReferredContent                           = KImpsKeyPRReferredContent,
    EImpsKeyPRContactInfo                               = KImpsKeyPRContactInfo,
    EImpsKeyPRContainedvCard                            = KImpsKeyPRContainedvCard,
    EImpsKeyPRReferredvCard                             = KImpsKeyPRReferredvCard,
    EImpsKeyPRExt                                       = KImpsKeyPRExt,

    // CSP 1.2
    EImpsKeyPRInf_link                                  = KImpsKeyInf_link,
    EImpsKeyPRInfoLink                                  = KImpsKeyInfoLink,
    EImpsKeyPRLink                                      = KImpsKeyLink,
    EImpsKeyPRText                                      = KImpsKeyText,

    EImpsKeyPREND                                       = KImpsKeyPREND
    };

enum TImpsContent
    {
    EImpsKeySession                                     = KImpsKeySession,
    EImpsKeySessionDescriptor                           = KImpsKeySessionDescriptor,
    EImpsKeySessionType                                 = KImpsKeySessionType,
    EImpsKeySessionID                                   = KImpsKeySessionID,
    EImpsKeyTransaction                                 = KImpsKeyTransaction,
    EImpsKeyTransactionDescriptor                       = KImpsKeyTransactionDescriptor,
    EImpsKeyTransactionMode                             = KImpsKeyTransactionMode,
    EImpsKeyTransactionID                               = KImpsKeyTransactionID,
    EImpsKeyPoll                                        = KImpsKeyPoll,
    EImpsKeyTransactionContent                          = KImpsKeyTransactionContent,
    EImpsKeyStatus                                      = KImpsKeyStatus,
    EImpsKeyPolling_Request                             = KImpsKeyPolling_Request,
    EImpsKeyLogin_Request                               = KImpsKeyLogin_Request,
    EImpsKeyLogin_Response                              = KImpsKeyLogin_Response,
    EImpsKeyService_Request                             = KImpsKeyService_Request,
    EImpsKeyService_Response                            = KImpsKeyService_Response,
    EImpsKeyClientCapability_Request                    = KImpsKeyClientCapability_Request,
    EImpsKeyClientCapability_Response                   = KImpsKeyClientCapability_Response,
    EImpsKeyLogout_Request                              = KImpsKeyLogout_Request,
    EImpsKeyDisconnect                                  = KImpsKeyDisconnect,
    EImpsKeyKeepAlive_Request                           = KImpsKeyKeepAlive_Request,
    EImpsKeyKeepAlive_Response                          = KImpsKeyKeepAlive_Response,
    EImpsKeyGetSPInfo_Request                           = KImpsKeyGetSPInfo_Request,
    EImpsKeyGetSPInfo_Response                          = KImpsKeyGetSPInfo_Response,
    EImpsKeySearch_Request                              = KImpsKeySearch_Request,
    EImpsKeyStopSearch_Request                          = KImpsKeyStopSearch_Request,
    EImpsKeySearch_Response                             = KImpsKeySearch_Response,
    EImpsKeyCompletionFlag                              = KImpsKeyCompletionFlag,
    EImpsKeyInvite_Request                              = KImpsKeyInvite_Request,
    EImpsKeyInvite_Response                             = KImpsKeyInvite_Response,
    EImpsKeyInviteUser_Request                          = KImpsKeyInviteUser_Request,
    EImpsKeyInviteUser_Response                         = KImpsKeyInviteUser_Response,
    EImpsKeyCancelInvite_Request                        = KImpsKeyCancelInvite_Request,
    EImpsKeyCancelInviteUser_Request                    = KImpsKeyCancelInviteUser_Request,
    EImpsKeySubscribePresence_Request                   = KImpsKeySubscribePresence_Request,
    EImpsKeyUnsubscribePresence_Request                 = KImpsKeyUnsubscribePresence_Request,
    EImpsKeyPresenceNotification_Request                = KImpsKeyPresenceNotification_Request,
    EImpsKeyGetWatcherList_Request                      = KImpsKeyGetWatcherList_Request,
    EImpsKeyGetWatcherList_Response                     = KImpsKeyGetWatcherList_Response,
    EImpsKeyGetPresence_Request                         = KImpsKeyGetPresence_Request,
    EImpsKeyGetPresence_Response                        = KImpsKeyGetPresence_Response,
    EImpsKeyPresenceAuth_Request                        = KImpsKeyPresenceAuth_Request,
    EImpsKeyPresenceAuth_User                           = KImpsKeyPresenceAuth_User,
    EImpsKeyCancelAuth_Request                          = KImpsKeyCancelAuth_Request,
    EImpsKeyUpdatePresence_Request                      = KImpsKeyUpdatePresence_Request,
    EImpsKeyGetList_Request                             = KImpsKeyGetList_Request,
    EImpsKeyGetList_Response                            = KImpsKeyGetList_Response,
    EImpsKeyCreateList_Request                          = KImpsKeyCreateList_Request,
    EImpsKeyDeleteList_Request                          = KImpsKeyDeleteList_Request,
    EImpsKeyListManage_Request                          = KImpsKeyListManage_Request,
    EImpsKeyListManage_Response                         = KImpsKeyListManage_Response,
    EImpsKeyCreateAttributeList_Request                 = KImpsKeyCreateAttributeList_Request,
    EImpsKeyDeleteAttributeList_Request                 = KImpsKeyDeleteAttributeList_Request,
    EImpsKeyGetAttributeList_Request                    = KImpsKeyGetAttributeList_Request,
    EImpsKeyGetAttributeList_Response                   = KImpsKeyGetAttributeList_Response,
    EImpsKeySendMessage_Request                         = KImpsKeySendMessage_Request,
    EImpsKeySendMessage_Response                        = KImpsKeySendMessage_Response,
    EImpsKeySetDeliveryMethod_Request                   = KImpsKeySetDeliveryMethod_Request,
    EImpsKeyGetMessageList_Request                      = KImpsKeyGetMessageList_Request,
    EImpsKeyGetMessageList_Response                     = KImpsKeyGetMessageList_Response,
    EImpsKeyRejectMessage_Request                       = KImpsKeyRejectMessage_Request,
    EImpsKeyMessageNotification                         = KImpsKeyMessageNotification,
    EImpsKeyGetMessage_Request                          = KImpsKeyGetMessage_Request,
    EImpsKeyGetMessage_Response                         = KImpsKeyGetMessage_Response,
    EImpsKeyNewMessage                                  = KImpsKeyNewMessage,
    EImpsKeyMessageDelivered                            = KImpsKeyMessageDelivered,
    EImpsKeyDeliveryReport_Request                      = KImpsKeyDeliveryReport_Request,
    EImpsKeyDeliveryTime                                = KImpsKeyDeliveryTime,
    EImpsKeyForwardMessage_Request                      = KImpsKeyForwardMessage_Request,
    EImpsKeyGetBlockedList_Request                      = KImpsKeyGetBlockedList_Request,
    EImpsKeyGetBlockedList_Response                     = KImpsKeyGetBlockedList_Response,
    EImpsKeyBlockEntity_Request                         = KImpsKeyBlockEntity_Request,
    EImpsKeyCreateGroup_Request                         = KImpsKeyCreateGroup_Request,
    EImpsKeyDeleteGroup_Request                         = KImpsKeyDeleteGroup_Request,
    EImpsKeyJoinGroup_Request                           = KImpsKeyJoinGroup_Request,
    EImpsKeyJoinGroup                                   = KImpsKeyJoinGroup,
    EImpsKeySubscribeNotification                       = KImpsKeySubscribeNotification,
    EImpsKeyJoinGroup_Response                          = KImpsKeyJoinGroup_Response,
    EImpsKeyLeaveGroup_Request                          = KImpsKeyLeaveGroup_Request,
    EImpsKeyLeaveGroup_Response                         = KImpsKeyLeaveGroup_Response,
    EImpsKeyGetGroupMembers_Request                     = KImpsKeyGetGroupMembers_Request,
    EImpsKeyGetGroupMembers_Response                    = KImpsKeyGetGroupMembers_Response,
    EImpsKeyAddGroupMembers_Request                     = KImpsKeyAddGroupMembers_Request,
    EImpsKeyRemoveGroupMembers_Request                  = KImpsKeyRemoveGroupMembers_Request,
    EImpsKeyMemberAccess_Request                        = KImpsKeyMemberAccess_Request,
    EImpsKeyGetGroupProps_Request                       = KImpsKeyGetGroupProps_Request,
    EImpsKeyGetGroupProps_Response                      = KImpsKeyGetGroupProps_Response,
    EImpsKeySetGroupProps_Request                       = KImpsKeySetGroupProps_Request,
    EImpsKeyRejectList_Request                          = KImpsKeyRejectList_Request,
    EImpsKeyRejectList_Response                         = KImpsKeyRejectList_Response,
    EImpsKeySubscribeGroupNotice_Request                = KImpsKeySubscribeGroupNotice_Request,
    EImpsKeySubscribeGroupNotice_Response               = KImpsKeySubscribeGroupNotice_Response,
    EImpsKeyGroupChangeNotice                           = KImpsKeyGroupChangeNotice,
    EImpsKeyDigestBytes                                 = KImpsKeyDigestBytes,
    EImpsKeyDigestSchema                                = KImpsKeyDigestSchema,
    EImpsKeyPassword                                    = KImpsKeyPassword,
    EImpsKeyNonce                                       = KImpsKeyNonce,
    EImpsKeyCapabilityRequest                           = KImpsKeyCapabilityRequest,
    EImpsKeyCapabilityList                              = KImpsKeyCapabilityList,
    EImpsKeyClientType                                  = KImpsKeyClientType,
    EImpsKeyInitialDeliveryMethod                       = KImpsKeyInitialDeliveryMethod,
    EImpsKeyAnyContent                                  = KImpsKeyAnyContent,
    EImpsKeyAcceptedCharSet                             = KImpsKeyAcceptedCharSet,
    EImpsKeyAcceptedContentType                         = KImpsKeyAcceptedContentType,
    EImpsKeyAcceptedTransferEncoding                    = KImpsKeyAcceptedTransferEncoding,
    EImpsKeyAcceptedContentLength                       = KImpsKeyAcceptedContentLength,
    EImpsKeySupportedBearer                             = KImpsKeySupportedBearer,
    EImpsKeyMultiTrans                                  = KImpsKeyMultiTrans,
    EImpsKeyParserSize                                  = KImpsKeyParserSize,
    EImpsKeySupportedCIRMethod                          = KImpsKeySupportedCIRMethod,
    EImpsKeyUDPPort                                     = KImpsKeyUDPPort,
    EImpsKeyTCPAddress                                  = KImpsKeyTCPAddress,
    EImpsKeyTCPPort                                     = KImpsKeyTCPPort,
    EImpsKeyServerPollMin                               = KImpsKeyServerPollMin,
    EImpsKeyDefaultLanguage                             = KImpsKeyDefaultLanguage,
    EImpsKeyResult                                      = KImpsKeyResult,
    EImpsKeyCode                                        = KImpsKeyCode,
    EImpsKeyDescription                                 = KImpsKeyDescription,
    EImpsKeyDetailedResult                              = KImpsKeyDetailedResult,
    EImpsKeySessionCookie                               = KImpsKeySessionCookie,
    EImpsKeySender                                      = KImpsKeySender,
    EImpsKeyRecipient                                   = KImpsKeyRecipient,
    EImpsKeyUser                                        = KImpsKeyUser,
    EImpsKeyGroup                                       = KImpsKeyGroup,
    EImpsKeyUserID                                      = KImpsKeyUserID,
    EImpsKeyClientID                                    = KImpsKeyClientID,
    EImpsKeyGroupID                                     = KImpsKeyGroupID,
    EImpsKeyMessageID                                   = KImpsKeyMessageID,
    EImpsKeyMessageURI                                  = KImpsKeyMessageURI,
    EImpsKeyScreenName                                  = KImpsKeyScreenName,
    EImpsKeyNickName                                    = KImpsKeyNickName,
    EImpsKeyURLList                                     = KImpsKeyURLList,
    EImpsKeyURL                                         = KImpsKeyURL,
    EImpsKeyMSISDN                                      = KImpsKeyMSISDN,
    EImpsKeyGroupList                                   = KImpsKeyGroupList,
    EImpsKeyUserList                                    = KImpsKeyUserList,
    EImpsKeyContactList                                 = KImpsKeyContactList,
    EImpsKeyDefaultContactList                          = KImpsKeyDefaultContactList,
    EImpsKeyNickList                                    = KImpsKeyNickList,
    EImpsKeyAddNickList                                 = KImpsKeyAddNickList,
    EImpsKeyRemoveNickList                              = KImpsKeyRemoveNickList,
    EImpsKeyDefaultList                                 = KImpsKeyDefaultList,
    EImpsKeySearchPairList                              = KImpsKeySearchPairList,
    EImpsKeySearchElement                               = KImpsKeySearchElement,
    EImpsKeySearchString                                = KImpsKeySearchString,
    EImpsKeySearchLimit                                 = KImpsKeySearchLimit,
    EImpsKeySearchID                                    = KImpsKeySearchID,
    EImpsKeySearchIndex                                 = KImpsKeySearchIndex,
    EImpsKeySearchFindings                              = KImpsKeySearchFindings,
    EImpsKeySearchResult                                = KImpsKeySearchResult,
    EImpsKeyInviteID                                    = KImpsKeyInviteID,
    EImpsKeyInviteType                                  = KImpsKeyInviteType,
    EImpsKeyInviteNote                                  = KImpsKeyInviteNote,
    EImpsKeyAcceptance                                  = KImpsKeyAcceptance,
    EImpsKeyResponseNote                                = KImpsKeyResponseNote,
    EImpsKeyDefaultAttributeList                        = KImpsKeyDefaultAttributeList,
    EImpsKeyPresence                                    = KImpsKeyPresence,
    EImpsKeyPresenceSubList                             = KImpsKeyPresenceSubList,
    EImpsKeyKeepAliveTime                               = KImpsKeyKeepAliveTime,
    EImpsKeyTimeToLive                                  = KImpsKeyTimeToLive,
    EImpsKeySName                                       = KImpsKeySName,
    EImpsKeyName                                        = KImpsKeyName,
    EImpsKeyValue                                       = KImpsKeyValue,
    EImpsKeyValidity                                    = KImpsKeyValidity,
    EImpsKeyMessageInfo                                 = KImpsKeyMessageInfo,
    EImpsKeyDateTime                                    = KImpsKeyDateTime,
    EImpsKeyMessageCount                                = KImpsKeyMessageCount,
    EImpsKeyDeliveryReport                              = KImpsKeyDeliveryReport,
    EImpsKeyDeliveryMethod                              = KImpsKeyDeliveryMethod,
    EImpsKeyBlockList                                   = KImpsKeyBlockList,
    EImpsKeyGrantList                                   = KImpsKeyGrantList,
    EImpsKeyEntityList                                  = KImpsKeyEntityList,
    EImpsKeyAddList                                     = KImpsKeyAddList,
    EImpsKeyRemoveList                                  = KImpsKeyRemoveList,
    EImpsKeyInUse                                       = KImpsKeyInUse,
    EImpsKeyContactListProperties                       = KImpsKeyContactListProperties,
    EImpsKeyGroupProperties                             = KImpsKeyGroupProperties,
    EImpsKeyOwnProperties                               = KImpsKeyOwnProperties,
    EImpsKeyProperty                                    = KImpsKeyProperty,
    EImpsKeyWelcomeNote                                 = KImpsKeyWelcomeNote,
    EImpsKeyJoinedRequest                               = KImpsKeyJoinedRequest,
    EImpsKeyAdmin                                       = KImpsKeyAdmin,
    EImpsKeyMod                                         = KImpsKeyMod,
    EImpsKeyUsers                                       = KImpsKeyUsers,
    EImpsKeySubscribeType                               = KImpsKeySubscribeType,
    EImpsKeyJoined                                      = KImpsKeyJoined,
    EImpsKeyLeft                                        = KImpsKeyLeft,
    EImpsKeyLogo                                        = KImpsKeyLogo,
    EImpsKeyContentType                                 = KImpsKeyContentType,
    EImpsKeyContentData                                 = KImpsKeyContentData,
    EImpsKeyContentSize                                 = KImpsKeyContentSize,
    EImpsKeyContentEncoding                             = KImpsKeyContentEncoding,
    EImpsKeyAllFunctionsRequest                         = KImpsKeyAllFunctionsRequest,
    EImpsKeyAllFunctions                                = KImpsKeyAllFunctions,
    EImpsKeyFunctions                                   = KImpsKeyFunctions,
    EImpsKeyWVCSPFeat                                   = KImpsKeyWVCSPFeat,
    EImpsKeyFundamentalFeat                             = KImpsKeyFundamentalFeat,
    EImpsKeyPresenceFeat                                = KImpsKeyPresenceFeat,
    EImpsKeyIMFeat                                      = KImpsKeyIMFeat,
    EImpsKeyGroupFeat                                   = KImpsKeyGroupFeat,
    EImpsKeyServiceFunc                                 = KImpsKeyServiceFunc,
    EImpsKeySearchFunc                                  = KImpsKeySearchFunc,
    EImpsKeyInviteFunc                                  = KImpsKeyInviteFunc,
    EImpsKeyContListFunc                                = KImpsKeyContListFunc,
    EImpsKeyPresenceAuthFunc                            = KImpsKeyPresenceAuthFunc,
    EImpsKeyPresenceDeliverFunc                         = KImpsKeyPresenceDeliverFunc,
    EImpsKeyAttListFunc                                 = KImpsKeyAttListFunc,
    EImpsKeyIMSendFunc                                  = KImpsKeyIMSendFunc,
    EImpsKeyIMReceiveFunc                               = KImpsKeyIMReceiveFunc,
    EImpsKeyIMAuthFunc                                  = KImpsKeyIMAuthFunc,
    EImpsKeyGroupMgmtFunc                               = KImpsKeyGroupMgmtFunc,
    EImpsKeyGroupUseFunc                                = KImpsKeyGroupUseFunc,
    EImpsKeyGroupAuthFunc                               = KImpsKeyGroupAuthFunc,
    EImpsKeyGETSPI                                      = KImpsKeyGETSPI,
    EImpsKeySRCH                                        = KImpsKeySRCH,
    EImpsKeySTSRC                                       = KImpsKeySTSRC,
    EImpsKeyINVIT                                       = KImpsKeyINVIT,
    EImpsKeyCAINV                                       = KImpsKeyCAINV,
    EImpsKeyGCLI                                        = KImpsKeyGCLI,
    EImpsKeyCCLI                                        = KImpsKeyCCLI,
    EImpsKeyDCLI                                        = KImpsKeyDCLI,
    EImpsKeyMCLS                                        = KImpsKeyMCLS,
    EImpsKeyGETWL                                       = KImpsKeyGETWL,
    EImpsKeyREACT                                       = KImpsKeyREACT,
    EImpsKeyCAAUT                                       = KImpsKeyCAAUT,
    EImpsKeyGETPR                                       = KImpsKeyGETPR,
    EImpsKeyUPDPR                                       = KImpsKeyUPDPR,
    EImpsKeyCALI                                        = KImpsKeyCALI,
    EImpsKeyDALI                                        = KImpsKeyDALI,
    EImpsKeyGALS                                        = KImpsKeyGALS,
    EImpsKeyMDELIV                                      = KImpsKeyMDELIV,
    EImpsKeyFWMSG                                       = KImpsKeyFWMSG,
    EImpsKeySETD                                        = KImpsKeySETD,
    EImpsKeyGETLM                                       = KImpsKeyGETLM,
    EImpsKeyGETM                                        = KImpsKeyGETM,
    EImpsKeyREJCM                                       = KImpsKeyREJCM,
    EImpsKeyNOTIF                                       = KImpsKeyNOTIF,
    EImpsKeyNEWM                                        = KImpsKeyNEWM,
    EImpsKeyGLBLU                                       = KImpsKeyGLBLU,
    EImpsKeyBLENT                                       = KImpsKeyBLENT,
    EImpsKeyCREAG                                       = KImpsKeyCREAG,
    EImpsKeyDELGR                                       = KImpsKeyDELGR,
    EImpsKeyGETGP                                       = KImpsKeyGETGP,
    EImpsKeySETGP                                       = KImpsKeySETGP,
    EImpsKeySUBGCN                                      = KImpsKeySUBGCN,
    EImpsKeyGRCHN                                       = KImpsKeyGRCHN,
    EImpsKeyGETGM                                       = KImpsKeyGETGM,
    EImpsKeyADDGM                                       = KImpsKeyADDGM,
    EImpsKeyRMVGM                                       = KImpsKeyRMVGM,
    EImpsKeyMBRAC                                       = KImpsKeyMBRAC,
    EImpsKeyREJEC                                       = KImpsKeyREJEC,
    EImpsKeyPureData                                    = KImpsKeyPureData,
    // CSP 1.2 additions
    EImpsKeyExtBlock                                    = KImpsKeyExtBlock,
    EImpsKeyAPIClient                                   = KImpsKeyAPIClient,
    EImpsKeyAgreedCapabilityList                        = KImpsKeyAgreedCapabilityList,
    EImpsKeyExtended_Request                            = KImpsKeyExtended_Request,
    EImpsKeyExtended_Response                           = KImpsKeyExtended_Response,
    EImpsKeyExtendedData                                = KImpsKeyExtendedData,
    EImpsKeyOtherServer                                 = KImpsKeyOtherServer,
    EImpsKeyPresenceAttributeNSName                     = KImpsKeyPresenceAttributeNSName,
    EImpsKeyReceiveList                                 = KImpsKeyReceiveList,
    EImpsKeySessionNSName                               = KImpsKeySessionNSName,
    EImpsKeyTransactionNSName                           = KImpsKeyTransactionNSName,
    EImpsKeyVerifyID_Request                            = KImpsKeyVerifyID_Request,
    EImpsKeyMF                                          = KImpsKeyMF,
    EImpsKeyMG                                          = KImpsKeyMG,
    EImpsKeyMM                                          = KImpsKeyMM,
    EImpsKeyAuto_Subscribe                              = KImpsKeyAuto_Subscribe,
    EImpsKeyGetReactiveAuthStatus_Request               = KImpsKeyGetReactiveAuthStatus_Request,
    EImpsKeyGetReactiveAuthStatus_Response              = KImpsKeyGetReactiveAuthStatus_Response,
    EImpsKeyAdminMapList                                = KImpsKeyAdminMapList,
    EImpsKeyAdminMapping                                = KImpsKeyAdminMapping,
    EImpsKeyGetJoinedUsers_Request                      = KImpsKeyGetJoinedUsers_Request,
    EImpsKeyGetJoinedUsers_Respone                      = KImpsKeyGetJoinedUsers_Respone,
    EImpsKeyMapping                                     = KImpsKeyMapping,
    EImpsKeyUserMapList                                 = KImpsKeyUserMapList,
    EImpsKeyUserMapping                                 = KImpsKeyUserMapping,
    EImpsKeyGETAUT                                      = KImpsKeyGETAUT,
    EImpsKeyGETJU                                       = KImpsKeyGETJU,
    EImpsKeyMP                                          = KImpsKeyMP,
    EImpsKeyVRID                                        = KImpsKeyVRID,
    EImpsKeyVerifyIDFunc                                = KImpsKeyVerifyIDFunc,
    EImpsKeyCIR                                         = KImpsKeyCIR,
    EImpsKeyDomain                                      = KImpsKeyDomain,
    EImpsKeyHistoryPeriod                               = KImpsKeyHistoryPeriod,
    EImpsKeyIDList                                      = KImpsKeyIDList,
    EImpsKeyMaxWatcherList                              = KImpsKeyMaxWatcherList,
    EImpsKeyReactiveAuthState                           = KImpsKeyReactiveAuthState,
    EImpsKeyReactiveAuthStatus                          = KImpsKeyReactiveAuthStatus,
    EImpsKeyReactiveAuthStatusList                      = KImpsKeyReactiveAuthStatusList,
    EImpsKeyWatcher                                     = KImpsKeyWatcher,
    EImpsKeyWatcherStatus                               = KImpsKeyWatcherStatus,
    EImpsKeyVersionList                                 = KImpsKeyVersionList,
    EImpsKeyEND                                         = KImpsKeyEND
    };

// CLASS DECLARATION

/**
*  MImpsKey class
*  Pure virtual key class
*/
class MImpsKey
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        */
        virtual void Destroy() = 0;

    public: // New functions

        /**
        * Add new elem to the key
        * @param aElemvalue Element value
        * @param aKeyType  Key type
        * @return void
        */
        virtual void AddL( const TInt aElemvalue ,
                           enum TImpsKeyType aKeyType  = EImpsKeyTypeIM ) = 0;

        /**
        * Count the elements in the key
        * @return TInt Number of elements in key
        */
        virtual TInt Count( ) const = 0;

        /**
        * Pop the last element from the key
        * @param how many items to pop. default is 1
        * If too much is tried the the function leaves
        * @return void
        */
        virtual void PopL( TInt aCount = 1 ) = 0;

        /**
        * Get the requested element from the key
        * @return TInt
        */
        virtual TInt GetElementL( const TInt aIndex,
                                  TImpsKeyType& aKeyType ) const = 0;

        /**
        * Replaces the last element in the key
        * @return void
        */
        virtual void ReplaceLastL( const TInt aElemvalue,
                                   enum TImpsKeyType = EImpsKeyTypeIM ) = 0;

        /**
        * Resets the key
        * @return void
        */
        virtual void Reset() = 0;

        /**
        * Equal operator
        * @param aKey to compare with
        * @return 1 = true, 0 is false
        */
        virtual TInt Compare( MImpsKey &aKey ) = 0;
    };


/**
*  If restore functions return EFalse then the output parameter is undefined
*/
class MImpsDataAccessor
    {
    public:
        /**
        * Accessor creates an instance of MImpsKey entity.
        * It is NOT deleted by Accessor! You must do that yourself.
        * @return MImpsKey new key
        */
        virtual MImpsKey* NewKeyL() = 0;

        /**
        * descriptor value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        virtual void StoreDescL( MImpsKey* aKey, TPtrC aValue ) = 0;

        /**
        * descriptor value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        virtual void StoreDesc8L( MImpsKey* aKey, TPtrC8 aValue ) = 0;

        /**
        * integer value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        virtual void StoreIntegerL( MImpsKey* aKey, TInt aValue ) = 0;

        /**
        * boolean value mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aValue Value
        */
        virtual void StoreBooleanL( MImpsKey* aKey, TBool aValue ) = 0;

        /**
        * store empty value
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        */
        virtual void StoreEmptyL( MImpsKey* aKey ) = 0;

        /**
        * extension attribute mutator
        * key is a byte sequence, which defines how value is stored
        * @param aKey Key
        * @param aName Extension attribute name
        * @param aValue Extension attribute value
        */
        virtual void StoreExtL( MImpsKey* aKey,
                                TPtrC8 aName, TPtrC8 aValue ) = 0;

        /**
        * descriptor value accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        * @param aDes OUT Pointer to TDesC object.
        */
        virtual TBool RestoreDescL( MImpsKey* aKey, TDesC*& aDes ) = 0;

        /**
        * descriptor value accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        * @param aBuf OUT Pointer to TDesC8 object
        */
        virtual TBool RestoreDesc8L( MImpsKey* aKey,  TDesC8*& aDes ) = 0;

        /**
        * integer value accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        * @param aBuf OUT TInt object
        */
        virtual TBool RestoreIntegerL( MImpsKey* aKey, TInt& aInt ) = 0;

        /**
        * Boolean value accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        * @param aBuf OUT Boolean object
        */
        virtual TBool RestoreBooleanL( MImpsKey* aKey, TBool& aBool ) = 0;

        /**
        * empty value accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        */
        virtual TBool RestoreEmptyL( MImpsKey* aKey ) = 0;

        /**
        * extension attribute accessor
        * Function return EFalse if key is not found
        * Function return ETrue if key is found
        * @param aKey Key for restore
        * @param aName OUT Pointer to TDesC8 object
        * @param aValue OUT Pointer to TDesC8 object
        */
        virtual TBool RestoreExtL( MImpsKey* aKey,
                                   TDesC8*& aName, TDesC8*& aValue ) = 0;

        /**
        * data type accessor
        * Leaves with KErrArgument if illegal key not in WV DTD.
        * return data type. Note that one possible value is "not supported".
        * @param aKey Key type
        * @return TImpsDataType Datatype
        */
        virtual TImpsDataType KeyTypeL( MImpsKey* aKey ) = 0;

        /**
        * Finds the alternative key
        * This function finds if the supplied key has a child in the
        * datastorage and returns the information of the element.
        * This is usefull with e.g. TransactionContent
        * @param aKey key in
        * @param aEnum Enum value out
        * @param aIndex index out
        * @param aType type out
        * @return ETrue if found, EFalse if not found
        */
        virtual TBool RestoreAlternativeL( const MImpsKey* aKey,
                                           TInt& aEnum,
                                           TInt& aIndex,
                                           TImpsKeyType& aType ) const = 0;

        /**
        * Checks if the key exists
        * This function finds if the supplied key exists as it self or with
        * children and returns ETrue or EFalse
        * @param aKey key in
        * @return ETrue if found, EFalse if not found
        */
        virtual TBool CheckBranchExistenceL( const MImpsKey* aKey ) const = 0;

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aKey which part of message is copied. NULL means that all data
        *        is copied.
        */
        virtual void CopyDataL(
            MImpsDataAccessor& aSource,
            MImpsKey* aKey = NULL ) = 0;

        /**
        * Get CImpsFields
        * @return CImpsFields*
        */
        virtual CImpsFields* GetImpsFields( ) const = 0;

#ifdef _DEBUG
        virtual void DumpToFileL( RFs& aFs, const TDesC& aFilename ) = 0;
#endif
    };



#endif      // MImpsDataAccessorApi_H

// End of File
