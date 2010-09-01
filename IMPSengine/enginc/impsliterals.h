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
* Description: 
*       Provides common literals.
*
*
*/


#ifndef IMPSLITERALS_H
#define IMPSLITERALS_H

/* NOTE                                         *
 * If CSP version changes, there maid be need   *
 * to modify this file                          *
 */

// _LIT -> KImps..., _LIT8 -> K...

_LIT( KImpsTrue,                "T" );
_LIT( KImpsFalse,               "F" );
_LIT( KImpsContentType,         "text/plain" );

// group properties
_LIT( KImpsName,                "Name" );
_LIT( KImpsAccessType,          "Accesstype" );
_LIT( KImpsType,                "Type" );
_LIT( KImpsPrivateMessaging,    "PrivateMessaging" );
_LIT( KImpsSearchable,          "Searchable" );
_LIT( KImpsTopic,               "Topic" );
_LIT( KImpsMaxActiveUsers,      "MaxActiveUsers" );
_LIT( KImpsIsMember,            "IsMember" );
_LIT( KImpsPriviledgeLevel,     "PrivilegeLevel" );
_LIT( KImpsUser,                "User" );
_LIT( KImpsMod,                 "Mod" );
_LIT( KImpsAdmin,               "Admin" );
_LIT( KImpsOpen,                "Open" );
_LIT( KImpsRestricted,          "Restricted" );
_LIT( KImpsNumberOfUsers,       "ActiveUsers" );
_LIT( KImpsPublic,              "Public" );
_LIT( KImpsPrivate,             "Private" );
_LIT( KImpsAutoDelete,          "AutoDelete" );
// user availability
_LIT( KImpsAvailable,           "AVAILABLE" );
_LIT( KImpsNotAvailable,        "NOT_AVAILABLE" );
_LIT( KImpsDiscreet,            "DISCREET" );


_LIT8( KTrue,                   "T" );
_LIT8( KFalse,                  "F" );
_LIT8( KInband,                 "Inband" );
_LIT8( KOutband,                "Outband" );
_LIT8( KRequest,                "Request" );
_LIT8( KResponse,               "Response" );
_LIT8( KXmlMime,                "application/vnd.wv.csp.xml" );
_LIT8( KSupportedCspVersion,    "CSP1.1" );
// digest schema
_LIT8( KPWD,                    "PWD" );
_LIT8( KSHA,                    "SHA" );
_LIT8( KMD4,                    "MD4" );
_LIT8( KMD5,                    "MD5" );
_LIT8( KMD6,                    "MD6" );
// client type
_LIT8( KMOBILE_PHONE,           "MOBILE_PHONE" );
_LIT8( KCOMPUTER,               "COMPUTER" );
_LIT8( KPDA,                    "PDA" );
_LIT8( KCLI,                    "CLI" );
_LIT8( KOTHER,                  "OTHER" );
// delivery method
_LIT8( KNotify,                 "N" );
_LIT8( KPush,                   "P" );
// cap
_LIT8( KMMS,                    "MMS" );
_LIT8( KCALL,                   "CALL" );
_LIT8( KEMAIL,                  "EMAIL" );
_LIT8( KSMS,                    "SMS" );
_LIT8( KWSP,                    "WSP" );
_LIT8( KHTTP,                   "HTTP" );
_LIT8( KHTTPS,                  "HTTPS" );
// supported cir method
_LIT8( KWAPSMS ,                "WAPSMS" );
_LIT8( KWAPUDP,                 "WAPUDP" );
_LIT8( KSUDP,                   "SUDP" );
_LIT8( KSTCP,                   "STCP" );
// search element
_LIT8( KUSER_ID,                "USER_ID" );
_LIT8( KUSER_FIRST_NAME,        "USER_FIRST_NAME" );
_LIT8( KUSER_LAST_NAME,         "USER_LAST_NAME" );
_LIT8( KUSER_EMAIL_ADDRESS,     "USER_EMAIL_ADDRESS" );
_LIT8( KUSER_ALIAS,             "USER_ALIAS" );
_LIT8( KUSER_ONLINE_STATUS,     "USER_ONLINE_STATUS" );
_LIT8( KUSER_MOBILE_NUMBER,     "USER_MOBILE_NUMBER" );
_LIT8( KGROUP_ID,               "GROUP_ID" );
_LIT8( KGROUP_NAME,             "GROUP_NAME" );
_LIT8( KGROUP_TOPIC,            "GROUP_TOPIC" );
_LIT8( KGROUP_USER_ID_JOINED,   "GROUP_USER_ID_JOINED" );
_LIT8( KGROUP_USER_ID_OWNER,    "GROUP_USER_ID_OWNER" );
// invite type
_LIT8( KGR,                     "GR" );
_LIT8( KIM,                     "IM" );
_LIT8( KPR,                     "PR" );
_LIT8( KSC,                     "SC" );
// subscribe type
_LIT8( KGet,                    "G" );
_LIT8( KSet,                    "S" );
_LIT8( KUnset,                  "U" );
// encoding
_LIT8( KNone,                   "NONE" );
_LIT8( KBASE64,                 "BASE64" );
// user availability
_LIT8( KAVAILABLE,              "AVAILABLE" );
_LIT8( KNOT_AVAILABLE,          "NOT_AVAILABLE" );
_LIT8( KDISCREET,               "DISCREET" );
_LIT8( KOPEN,                   "OPEN" );
_LIT8( KCLOSED,                 "CLOSED" );
// primitives
_LIT8( KLoginRequest,               "Login-Request" );
_LIT8( KLoginResponse,              "Login-Response" );
_LIT8( KLogoutRequest,              "Logout-Request" );
_LIT8( KClientCapabilityRequest,    "ClientCapability-Request" );
_LIT8( KClientCapabilityResponse,   "ClientCapability-Response" );
_LIT8( KPollingRequest,             "Polling-Request" );
_LIT8( KKeepAliveRequest,           "KeepAlive-Request" );
_LIT8( KKeepAliveResponse,          "KeepAlive-Response" );
_LIT8( KDisconnect,                 "Disconnect" );
_LIT8( KGetSPInfoRequest,           "GetSPInfo-Request" );
_LIT8( KGetSPInfoResponse,          "GetSPInfo-Response" );
_LIT8( KServiceRequest,             "Service-Request" );
_LIT8( KServiceResponse,            "Service-Response" );
_LIT8( KGetMessageRequest,          "GetMessage-Request" );
_LIT8( KGetMessageResponse,         "GetMessage-Response" );
_LIT8( KMessageDelivered,           "MessageDelivered" );
_LIT8( KSendMessageRequest,         "SendMessage-Request" );
_LIT8( KSendMessageResponse,        "SendMessage-Response" );
_LIT8( KNewMessage,                 "NewMessage" );
_LIT8( KForwardMessageRequest,      "ForwardMessage-Request" );
_LIT8( KDeliveryReportRequest,      "DeliveryReport-Request" );
_LIT8( KCreateGroupRequest,         "CreateGroup-Request" );
_LIT8( KDeleteGroupRequest,         "DeleteGroup-Request" );
_LIT8( KJoinGroupRequest,           "JoinGroup-Request" );
_LIT8( KJoinGroupResponse,          "JoinGroup-Response" );
_LIT8( KGetGroupMembersRequest,     "GetGroupMembers-Request" );
_LIT8( KGetGroupMembersResponse,    "GetGroupMembers-Response" );
_LIT8( KLeaveGroupRequest,          "LeaveGroup-Request" );
_LIT8( KLeaveGroupResponse,         "LeaveGroup-Response" );
_LIT8( KRemoveGroupMembersRequest,  "RemoveGroupMembers-Request" );
_LIT8( KGetGroupPropsRequest,       "GetGroupProps-Request" );
_LIT8( KGetGroupPropsResponse,      "GetGroupProps-Response" );
_LIT8( KSetGroupPropsRequest,       "SetGroupProps-Request" );
_LIT8( KAddGroupMembersRequest,     "AddGroupMembers-Request" );
_LIT8( KCancelInviteRequest,        "CancelInvite-Request" );
_LIT8( KCancelInviteUserRequest,    "CancelInviteUser-Request" );
_LIT8( KInviteRequest,              "Invite-Request" );
_LIT8( KInviteResponse,             "Invite-Response" );
_LIT8( KInviteUserRequest,          "InviteUser-Request" );
_LIT8( KInviteUserResponse,         "InviteUser-Response" );
_LIT8( KSearchRequest,              "Search-Request" );
_LIT8( KSearchResponse,             "Search-Response" );
_LIT8( KStopSearchRequest,          "StopSearch-Request" );
_LIT8( KSubscribePresenceRequest,   "SubscribePresence-Request" );
_LIT8( KUnsubscribePresenceRequest, "UnsubscribePresence-Request" );
_LIT8( KPresenceNotificationRequest, "PresenceNotification-Request" );
_LIT8( KGetPresenceRequest,         "GetPresence-Request" );
_LIT8( KGetPresenceResponse,        "GetPresence-Response" );
_LIT8( KPresenceAuthRequest,        "PresenceAuth-Request" );
_LIT8( KPresenceAuthResponse,       "PresenceAuth-Response" );
_LIT8( KCancelAuthRequest,          "CancelAuth-Request" );
_LIT8( KUpdatePresenceRequest,      "UpdatePresence-Request" );
_LIT8( KGetWatcherListRequest,      "GetWatcherList-Request" );
_LIT8( KPresenceAuthUser,           "PresenceAuth-User" );
_LIT8( KGetListRequest,             "GetList-Request" );
_LIT8( KCreateListRequest,          "CreateList-Request" );
_LIT8( KDeleteListRequest,          "DeleteList-Request" );
_LIT8( KListManageRequest,          "ListManage-Request" );
_LIT8( KDeleteAttributeListRequest, "DeleteAttributeList-Request" );
_LIT8( KCreateAttributeListRequest, "CreateAttributeList-Request" );
_LIT8( KGetAttributeListRequest,    "GetAttributeList-Request" );
_LIT8( KMemberAccessRequest,        "MemberAccess-Request" );
_LIT8( KRejectListRequest,          "RejectList-Request" );
_LIT8( KSubscribeGroupNoticeRequest, "SubscribeGroupNotice-Request" );
_LIT8( KSetDeliveryMethodRequest,   "SetDeliveryMethod-Request" );
_LIT8( KGetMessageListRequest,      "GetMessageList-Request" );
_LIT8( KRejectMessageRequest,       "RejectMessage-Request" );
_LIT8( KBlockEntityRequest,         "BlockEntity-Request" );
_LIT8( KGetBlockedListRequest,      "GetBlockedList-Request" );
_LIT8( KGetBlockedListResponse,     "GetBlockedList-Response" );
_LIT8( KRejectListResponse,         "RejectList-Response" );
_LIT8( KSubscribeGroupNoticeResponse,   "SubscribeGroupNotice-Response" );
_LIT8( KGroupChangeNotice,          "GroupChangeNotice" );
_LIT8( KGetListResponse,            "GetList-Response" );
_LIT8( KListManageResponse,         "ListManage-Response" );
_LIT8( KGetAttributeListResponse,   "GetAttributeList-Response" );
_LIT8( KGetMessageListResponse,     "GetMessageList-Response" );
_LIT8( KMessageNotification,        "MessageNotification" );
_LIT8( KGetWatcherListResponse,     "GetWatcherList-Response" );
// CSP 1.2
_LIT8( KGetReactiveAuthStatusRequest,   "GetReactiveAuthStatus-Request" );
_LIT8( KGetReactiveAuthStatusResponse,   "GetReactiveAuthStatus-Response" );
//////////////////////////////////////////////////////////////////////////

// elements
_LIT8( KAcceptance,                 "Acceptance" );
_LIT8( KAddList,                    "AddList" );
_LIT8( KAddNickList,                "AddNickList" );
_LIT8( KSName,                      "SName" );
_LIT8( KWVCSPMessage,               "WV-CSP-Message" );
_LIT8( KClientID,                   "ClientID" );
_LIT8( KCode,                       "Code" );
_LIT8( KContactList,                "ContactList" );
_LIT8( KContentData,                "ContentData" );
_LIT8( KContentEncoding,            "ContentEncoding" );
_LIT8( KContentSize,                "ContentSize" );
_LIT8( KContentType,                "ContentType" );
_LIT8( KDateTime,                   "DateTime" );
_LIT8( KDescription,                "Description" );
_LIT8( KDetailedResult,             "DetailedResult" );
_LIT8( KEntityList,                 "EntityList" );
_LIT8( KGroup,                      "Group" );
_LIT8( KGroupID,                    "GroupID" );
_LIT8( KGroupList,                  "GroupList" );
_LIT8( KInUse,                      "InUse" );
_LIT8( KLogo,                       "Logo" );
_LIT8( KMessageCount,               "MessageCount" );
_LIT8( KMessageID,                  "MessageID" );
_LIT8( KMessageURI,                 "MessageURI" );
_LIT8( KMSISDN,                     "MSISDN" );
_LIT8( KName,                       "Name" );
_LIT8( KNickList,                   "NickList" );
_LIT8( KNickName,                   "NickName" );
_LIT8( KPoll,                       "Poll" );
_LIT8( KPresence,                   "Presence" );
_LIT8( KProperty,                   "Property" );
_LIT8( KRecipient,                  "Recipient" );
_LIT8( KRemoveList,                 "RemoveList" );
_LIT8( KRemoveNickList,             "RemoveNickList" );
_LIT8( KResult,                     "Result" );
_LIT8( KScreenName,                 "ScreenName" );
_LIT8( KSender,                     "Sender" );
_LIT8( KSession,                    "Session" );
_LIT8( KSessionDescriptor,          "SessionDescriptor" );
_LIT8( KSessionID,                  "SessionID" );
_LIT8( KSessionType,                "SessionType" );
_LIT8( KStatus,                     "Status" );
_LIT8( KTransaction,                "Transaction" );
_LIT8( KTransactionContent,         "TransactionContent" );
_LIT8( KTransactionDescriptor,      "TransactionDescriptor" );
_LIT8( KTransactionID,              "TransactionID" );
_LIT8( KTransactionMode,            "TransactionMode" );
_LIT8( KURL,                        "URL" );
_LIT8( KURLList,                    "URLList" );
_LIT8( KUser,                       "User" );
_LIT8( KUserID,                     "UserID" );
_LIT8( KUserList,                   "UserList" );
_LIT8( KValidity,                   "Validity" );
_LIT8( KValue,                      "Value" );
_LIT8( KAdmin,                      "Admin" );
_LIT8( KMod,                        "Mod" );
_LIT8( KUsers,                      "Users" );
_LIT8( KAllFunctions,               "AllFunctions" );
_LIT8( KAllFunctionsRequest,        "AllFunctionsRequest" );
_LIT8( KCapability,                 "Capability" );
_LIT8( KCapabilityList,             "CapabilityList" );
_LIT8( KCapabilityRequest,          "CapabilityRequest" );
_LIT8( KDigestBytes,                "DigestBytes" );
_LIT8( KDigestSchema,               "DigestSchema" );
_LIT8( KFunctions,                  "Functions" );
_LIT8( KInviteID,                   "InviteID" );
_LIT8( KInviteNote,                 "InviteNote" );
_LIT8( KInviteType,                 "InviteType" );
_LIT8( KKeepAliveTime,              "KeepAliveTime" );
_LIT8( KTimeToLive,                 "TimeToLive" );
_LIT8( KNonce,                      "Nonce" );
_LIT8( KPassword,                   "Password" );
_LIT8( KResponseNote,               "ResponseNote" );
_LIT8( KSearchElement,              "SearchElement" );
_LIT8( KSearchFindings,             "SearchFindings" );
_LIT8( KSearchID,                   "SearchID" );
_LIT8( KSearchIndex ,               "SearchIndex" );
_LIT8( KSearchLimit,                "SearchLimit" );
_LIT8( KSessionCookie,              "SessionCookie" );
_LIT8( KSearchPairList,             "SearchPairList" );
_LIT8( KSearchResult,               "SearchResult" );
_LIT8( KSearchString,               "SearchString" );
_LIT8( KCompletionFlag,             "CompletionFlag" );
_LIT8( KDeliveryReport,             "DeliveryReport" );
_LIT8( KMessageInfo,                "MessageInfo" );
_LIT8( KJoinGroup,                  "JoinGroup" );
_LIT8( KSubscribeNotification,      "SubscribeNotification" );
_LIT8( KGroupProperties,            "GroupProperties" );
_LIT8( KJoinedRequest,              "JoinedRequest" );
_LIT8( KWelcomeNote,                "WelcomeNote" );
_LIT8( KOwnProperties,              "OwnProperties" );
_LIT8( KSubscribeType,              "SubscribeType" );
_LIT8( KBlockList,                  "BlockList" );
_LIT8( KGrantList,                  "GrantList" );
_LIT8( KContactListProperties,      "ContactListProperties" );
_LIT8( KDeliveryMethod,             "DeliveryMethod" );
_LIT8( KDefaultList,                "DefaultList" );
_LIT8( KDefaultContactList,         "DefaultContactList" );
_LIT8( KDefaultAttributeList,       "DefaultAttributeList" );
_LIT8( KDeliveryTime,               "DeliveryTime" );
_LIT8( KJoined,                     "Joined" );
_LIT8( KLeft,                       "Left" );
// CSP 1.2
_LIT8( KAgreedCapabilityList,       "AgreedCapabilityList" );
_LIT8( KReceiveList,                "ReceiveList" );
_LIT8( KAutoSubscribe,              "AutoSubscribe" );
_LIT8( KAdminMapList,               "AdminMapList" );
_LIT8( KAdminMapping,               "AdminMapping" );
_LIT8( KMapping,                    "Mapping" );
_LIT8( KUserMapList,                "UserMapList" );
_LIT8( KUserMapping,                "UserMapping" );
_LIT8( KExtBlock,					"ExtBlock" );
_LIT8( KAPIClient,					"APIClient" );



//////////////////////////////////////////////////////////////////////////


_LIT8( KAttListFunc,                "AttListFunc" );
_LIT8( KContListFunc,               "ContListFunc" );
_LIT8( KGroupAuthFunc,              "GroupAuthFunc" );
_LIT8( KGroupMgmtFunc,              "GroupMgmtFunc" );
_LIT8( KGroupUseFunc,               "GroupUseFunc" );
_LIT8( KIMAuthFunc,                 "IMAuthFunc" );
_LIT8( KIMReceiveFunc,              "IMReceiveFunc" );
_LIT8( KIMSendFunc,                 "IMSendFunc" );
_LIT8( KPresenceAuthFunc,           "PresenceAuthFunc" );
_LIT8( KPresenceDeliverFunc,        "PresenceDeliverFunc" );

_LIT8( KInviteFunc,                 "InviteFunc" );
_LIT8( KSearchFunc,                 "SearchFunc" );
_LIT8( KServiceFunc,                "ServiceFunc" );
_LIT8( KWVCSPFeat,                  "WVCSPFeat" );
_LIT8( KGroupFeat,                  "GroupFeat" );
_LIT8( KIMFeat,                     "IMFeat" );
_LIT8( KFundamentalFeat,            "FundamentalFeat" );
_LIT8( KPresenceFeat,               "PresenceFeat" );

_LIT8( KADDGM,                      "ADDGM" );
_LIT8( KBLENT,                      "BLENT" );
_LIT8( KCAAUT,                      "CAAUT" );
_LIT8( KCAINV,                      "CAINV" );
_LIT8( KCALI,                       "CALI" );
_LIT8( KCCLI,                       "CCLI" );
_LIT8( KCREAG,                      "CREAG" );
_LIT8( KDALI,                       "DALI" );
_LIT8( KDCLI,                       "DCLI" );
_LIT8( KDELGR,                      "DELGR" );
_LIT8( KFWMSG,                      "FWMSG" );
_LIT8( KGALS,                       "GALS" );
_LIT8( KGCLI,                       "GCLI" );
_LIT8( KGETGM,                      "GETGM" );
_LIT8( KGETGP,                      "GETGP" );
_LIT8( KGETLM,                      "GETLM" );
_LIT8( KGETM,                       "GETM" );
_LIT8( KGETPR,                      "GETPR" );
_LIT8( KGETSPI,                     "GETSPI" );
_LIT8( KGETWL,                      "GETWL" );
_LIT8( KGLBLU,                      "GLBLU" );
_LIT8( KGRCHN,                      "GRCHN" );
_LIT8( KINVIT,                      "INVIT" );
_LIT8( KMBRAC,                      "MBRAC" );
_LIT8( KMCLS,                       "MCLS" );
_LIT8( KMDELIV,                     "MDELIV" );
_LIT8( KNEWM,                       "NEWM" );
_LIT8( KNOTIF,                      "NOTIF" );
_LIT8( KREACT,                      "REACT" );
_LIT8( KREJCM,                      "REJCM" );
_LIT8( KREJEC,                      "REJEC" );
_LIT8( KRMVGM,                      "RMVGM" );
_LIT8( KSETD,                       "SETD" );
_LIT8( KSETGP,                      "SETGP" );
_LIT8( KSRCH,                       "SRCH" );
_LIT8( KSTSRC,                      "STSRC" );
_LIT8( KSUBGCN,                     "SUBGCN" );
_LIT8( KUPDPR,                      "UPDPR" );

_LIT8( KAcceptedCharSet,            "AcceptedCharSet" );
_LIT8( KAcceptedContentLength,      "AcceptedContentLength" );
_LIT8( KAcceptedContentType,        "AcceptedContentType" );
_LIT8( KAcceptedTransferEncoding,   "AcceptedTransferEncoding" );
_LIT8( KAnyContent,                 "AnyContent" );
_LIT8( KDefaultLanguage,            "DefaultLanguage" );
_LIT8( KInitialDeliveryMethod,      "InitialDeliveryMethod" );
_LIT8( KMultiTrans,                 "MultiTrans" );
_LIT8( KParserSize,                 "ParserSize" );
_LIT8( KServerPollMin,              "ServerPollMin" );
_LIT8( KSupportedBearer,            "SupportedBearer" );
_LIT8( KSupportedCIRMethod,         "SupportedCIRMethod" );
_LIT8( KTCPAddress,                 "TCPAddress" );
_LIT8( KTCPPort,                    "TCPPort" );
_LIT8( KUDPPort,                    "UDPPort" );
// presence elements
_LIT8( KPresenceSubList,            "PresenceSubList" );
_LIT8( KQualifier,                  "Qualifier" );
_LIT8( KPresenceValue,              "PresenceValue" );
_LIT8( KOnlineStatus,               "OnlineStatus" );
_LIT8( KRegistration,               "Registration" );
_LIT8( KFreeTextLocation,           "FreeTextLocation" );
_LIT8( KPLMN,                       "PLMN" );
_LIT8( KUserAvailability,           "UserAvailability" );
_LIT8( KPreferredLanguage,          "PreferredLanguage" );
_LIT8( KStatusText,                 "StatusText" );
_LIT8( KStatusMood,                 "StatusMood" );
_LIT8( KAlias,                      "Alias" );
_LIT8( KClientInfo,                 "ClientInfo" );
_LIT8( KClientType,                 "ClientType" );
_LIT8( KDevManufacturer,            "DevManufacturer" );
_LIT8( KClientProducer,             "ClientProducer" );
_LIT8( KModel,                      "Model" );
_LIT8( KClientVersion,              "ClientVersion" );
_LIT8( KLanguage,                   "Language" );
_LIT8( KTimeZone,                   "TimeZone" );
_LIT8( KZone,                       "Zone" );
_LIT8( KGeoLocation,                "GeoLocation" );
_LIT8( KLongitude,                  "Longitude" );
_LIT8( KLatitude,                   "Latitude" );
_LIT8( KAltitude,                   "Altitude" );
_LIT8( KAccuracy,                   "Accuracy" );
_LIT8( KAddress,                    "Address" );
_LIT8( KCountry,                    "Country" );
_LIT8( KCity,                       "City" );
_LIT8( KStreet,                     "Street" );
_LIT8( KCrossing1,                  "Crossing1" );
_LIT8( KCrossing2,                  "Crossing2" );
_LIT8( KBuilding,                   "Building" );
_LIT8( KNamedArea,                  "NamedArea" );
_LIT8( KCommCap,                    "CommCap" );
_LIT8( KCommC,                      "CommC" );
_LIT8( KCap,                        "Cap" );
_LIT8( KContact,                    "Contact" );
_LIT8( KNote,                       "Note" );
_LIT8( KPreferredContacts,          "PreferredContacts" );
_LIT8( KAddrPref,                   "AddrPref" );
_LIT8( KPrefC,                      "PrefC" );
_LIT8( KCaddr,                      "Caddr" );
_LIT8( KCstatus,                    "Cstatus" );
_LIT8( KCname,                      "Cname" );
_LIT8( KCpriority,                  "Cpriority" );
_LIT8( KStatusContent,              "StatusContent" );
_LIT8( KDirectContent,              "DirectContent" );
_LIT8( KReferredContent,            "ReferredContent" );
_LIT8( KContactInfo,                "ContactInfo" );
_LIT8( KContainedvCard,             "ContainedvCard" );
_LIT8( KReferredvCard,              "ReferredvCard" );
// CSP 1.2
_LIT8( KInf_link,                   "Inf_link" );
_LIT8( KInfoLink,                   "InfoLink" );
_LIT8( KLink,                       "Link" );
_LIT8( Ktext,                       "Text" );



_LIT8( KStartOfTag,         "<" );
_LIT8( KEndOfTag,           ">" );
_LIT8( KEndTag,             "/>" );
_LIT8( KElementTerminator,  "</" );

_LIT8( KReplace,    "_zqf_" );
_LIT8( KAndRep8,    "&amp;" );
_LIT8( KltRep8,     "&lt;" );
_LIT8( KgtRep8,     "&gt;" );
_LIT8( KQuotRep8,   "&quot;" );
_LIT8( KAposRep8,   "&apos;" );
_LIT8( KApos8,      "'" );

_LIT8( KAnd8,     "&" );
_LIT8( Klt8,      "<" );
_LIT8( Kgt8,      ">" );
_LIT8( KQuot8,    "\"" );

_LIT8( KAttr, " xmlns=\"http://www.wireless-village.org/TRC1.1\"" );
_LIT8( KPresSubListAttr8, " xmlns=\"http://www.wireless-village.org/PA1.1\"" );

#endif

// End of File
