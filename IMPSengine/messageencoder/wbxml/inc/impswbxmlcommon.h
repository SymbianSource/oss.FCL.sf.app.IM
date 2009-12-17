/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common WBXML related definitions used in message
*               encoder and decoder
*
*/


#ifndef CImpsWbXmlCommon_H
#define CImpsWbXmlCommon_H

// CONSTANTS
const TInt KImpsSpace                              = 0x20;

// WBXML 
const TInt KImpsWbXmlVersion                       = 3;
const TUint KImpsDefaultCharset                     = HTTP_utf_8;
const TInt KImpsMessageRootElement                 = 0x09;
const TInt KImpsTokenMask                          = 0xff3f;
const TInt KImpsMaskCodePage                       = 0xff00;
const TInt KImpsTokenEmpty                         = 0x0040;
const TInt KImpsByteMask                           = 0xff;
const TInt KImpsWbXmlMaxStringLength               = 400;
const TInt KImpsWbXmlMaxBufferSize                 = 1000;
const TInt KImpsWbXmlPublicIdUnknown               = 1;
const TInt KImpsWvCsp11PublicIdentifier            = 0x10;
const TInt KImpsWvCsp12PublicIdentifier            = 0x11;
const TInt KImpsWbXmlPublicID                      = 1;

// WV NAMESPACES
const TInt KImpsNameSpaceCspMessage                = 1;
const TInt KImpsNameSpaceTransactionContent        = 2;
const TInt KImpsNameSpacePresenceSubList           = 3;

// WV CODEPAGES
const TInt KImpsCodePagePresence                   = 0x0500;

// DTD
const   TInt KImpsHeaderDtd11Count =              12;
const   TInt KImpsTransactionDtd11Count =         249;
const   TInt KImpsPresenceDtd11Count =            54;

_LIT8( KImpsWbXmlMessageMimeType,           "application/vnd.wv.csp.wbxml" );
_LIT8( KImpsMessageVersion11,               "1.1");
_LIT8( KImpsMessageVersion12,               "1.2");
_LIT8( KImpsPresenceSubList,                "PresenceSubList");
_LIT8( KImpsEntityList,                     "EntityList");
_LIT8( KImpsAddList,                        "AddList");
_LIT8( KImpsRemoveList,                     "RemoveList");
_LIT8( KImpsAddNickList,                    "AddNickList");
_LIT8( KImpsRemoveNickList,                 "RemoveNickList");
_LIT8( KImpsContactListProperties,          "ContactListProperties");
_LIT8( KImpsHttp8,                          "http://");
_LIT8( KImpsHttps8,                         "https://");
_LIT8( KImpsOpen,                           "OPEN");
_LIT8( KImpsClosed,                         "CLOSED");
_LIT8( KImpsPWD,                            "PWD");
_LIT8( KImpsSHA,                            "SHA");
_LIT8( KImpsMD4,                            "MD4");
_LIT8( KImpsMD5,                            "MD5");
_LIT8( KImpsMD6,                            "MD6");
_LIT8( KImpsHTTPS,                          "HTTPS");
_LIT8( KImpsBASE64_8,                       "BASE64");
_LIT8( KImpsNone_8,                         "None");
_LIT8( KImpsAPIClient_8,					"APIClient");


_LIT( KPcData,                              "PCDATA" );
_LIT( KEmpty,                               "EMPTY" );
_LIT( KImpsEmpty,                           "");
_LIT( KImpsTrue,                            "T");
_LIT( KImpsFalse,                           "F");

_LIT( KImpsWvCspMessage,                    "WV-CSP-Message");
_LIT( KImpsAnyContent,                      "AnyContent");
_LIT( KImpsAcceptedCharSet,                 "AcceptedCharSet");
_LIT( KImpsAcceptedContentType,             "AcceptedContentType");
_LIT( KImpsUserID,                          "UserID");
_LIT( KImpsContactList,                     "ContactList");
_LIT( KImpsMobilePhone,                     "MOBILE_PHONE");
_LIT( KImpsComputer,                        "COMPUTER");
_LIT( KImpsPda,                             "PDA");
_LIT( KImpsCli,                             "CLI");
_LIT( KImpsOther,                           "OTHER");
_LIT( KImpsAvailable,                       "AVAILABLE");
_LIT( KImpsNotAvailable,                    "NOT_AVAILABLE");
_LIT( KImpsDiscreet,                        "DISCREET");
_LIT( KImpsUser,                            "User");
_LIT( KImpsGroup,                           "Group");
_LIT( KImpsGroupID,                         "GroupID");
_LIT( KImpsScreenName,                      "ScreenName");
_LIT( KImpsNickName,                        "NickName");
_LIT( KImpsName,                            "Name");
_LIT( KImpsHttp,                            "http://");
_LIT( KImpsHttps,                           "https://");
_LIT( KImpsType,                            "Type");
_LIT( KImpsTopic,                           "Topic");
_LIT( KImpsPrivateMessaging,                "PrivateMessaging");
_LIT( KImpsPrivilegeLevel,                  "PrivilegeLevel");
_LIT( KImpsSearchable,                      "Searchable");
_LIT( KImpsOpen16,                          "Open");
_LIT( KImpsActiveUsers,                     "ActiveUsers");
_LIT( KImpsMaxActiveUsers,                  "MaxActiveUsers");
_LIT( KImpsAutoJoin,                        "AutoJoin");
_LIT( KImpsAccessType,                      "Accesstype");
_LIT( KImpsRestricted,                      "Restricted");
_LIT( KImpsPublic,                          "Public");
_LIT( KImpsPrivate,                         "Private");
_LIT( KImpsAdmin,                           "Admin");
_LIT( KImpsMod,                             "Mod");
_LIT( KImpsBASE64,                          "BASE64");
_LIT( KImpsNone,                            "None");
_LIT( KImpstextplain,                       "text/plain");
_LIT( KImpsIM,                              "IM");
_LIT( KImpsDisplayName,                     "DisplayName");
_LIT( KImpsDefault,                         "Default");
_LIT( KImpsApplicationVndWapMmsMessage,     "application/vnd.wap.mms-message");
_LIT( KImpsApplicationXSms,     			"application/x-sms");
_LIT( KImpsAutoDelete,                      "AutoDelete");
_LIT( KImpsValidity,                        "Validity");
_LIT( KImpsDenied,                          "DENIED");
_LIT( KImpsGranted,                         "GRANTED");
_LIT( KImpsPending,                         "PENDING");



_LIT( KImpsStatus,                          "Status");
_LIT( KImpsAPIClient,						"APIClient");


_LIT8( KImpsCSP12Xmlns,                        " xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP\"" );
_LIT8( KImpsPA12Xmlns,                        " xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA\"" );
_LIT8( KImpsTRC12Xmlns,                        " xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC\"" );

_LIT8(KAPIClientAttributeValue, 				"www.nokia.com/apiclient");
_LIT8(KAPIClientAttribute, 						"xmlns");



                          
// CSP WBXML tokens
const TInt KImpsWbXmlAcceptance                    = 0x0005;
const TInt KImpsWbXmlAddNickList                   = 0x0007;
const TInt KImpsWbXmlSName                         = 0x0008;
const TInt KImpsWbXmlContactList                   = 0x000c;
const TInt KImpsWbXmlWV_CSP_Message                = 0x0009;
const TInt KImpsWbXmlClientID                      = 0x000a;
const TInt KImpsWbXmlCode                          = 0x000b;
const TInt KImpsWbXmlContentData                   = 0x000d;
const TInt KImpsWbXmlContentEncoding               = 0x000e;
const TInt KImpsWbXmlContentSize                   = 0x000f;
const TInt KImpsWbXmlDescription                   = 0x0012;
const TInt KImpsWbXmlGroup                         = 0x0015;
const TInt KImpsWbXmlDetailedResult                = 0x0013;
const TInt KImpsWbXmlGroupID                       = 0x0016;
const TInt KImpsWbXmlInUse                         = 0x0018;
const TInt KImpsWbXmlMessageID                     = 0x001b;
const TInt KImpsWbXmlMessageURI                    = 0x001c;
const TInt KImpsWbXmlName                          = 0x001e;
const TInt KImpsWbXmlNickList                      = 0x001f;
const TInt KImpsWbXmlPoll                          = 0x0021;
const TInt KImpsWbXmlPresence                      = 0x0022;
const TInt KImpsWbXmlPresenceSubList               = 0x0023;
const TInt KImpsWbXmlSender                        = 0x002c;
const TInt KImpsWbXmlSession                       = 0x002d;
const TInt KImpsWbXmlSessionDescriptor             = 0x002e;
const TInt KImpsWbXmlSessionID                     = 0x002f;
const TInt KImpsWbXmlSessionType                   = 0x0030;
const TInt KImpsWbXmlTransaction                   = 0x0032;
const TInt KImpsWbXmlTransactionContent            = 0x0033;
const TInt KImpsWbXmlTransactionDescriptor         = 0x0034;
const TInt KImpsWbXmlTransactionID                 = 0x0035;
const TInt KImpsWbXmlTransactionMode               = 0x0036;
const TInt KImpsWbXmlURL                           = 0x0037;
const TInt KImpsWbXmlUserID                        = 0x003a;
const TInt KImpsWbXmlValue                         = 0x003d;
const TInt KImpsWbXmlStatus                        = 0x0031;
const TInt KImpsWbXmlResult                        = 0x002a;
const TInt KImpsWbXmlRecipient                     = 0x0027;
const TInt KImpsWbXmlUser                          = 0x0039;
const TInt KImpsWbXmlScreenName                    = 0x002b;
const TInt KImpsWbXmlURLList                       = 0x0038;
const TInt KImpsWbXmlMSISDN                        = 0x001d;
const TInt KImpsWbXmlUserList                      = 0x003b;
const TInt KImpsWbXmlNickName                      = 0x0020;
const TInt KImpsWbXmlGroupList                     = 0x0017;
const TInt KImpsWbXmlRemoveNickList                = 0x0029;
const TInt KImpsWbXmlValidity                      = 0x003c;
const TInt KImpsWbXmlMessageCount                  = 0x001a;
const TInt KImpsWbXmlDateTime                      = 0x0011;
const TInt KImpsWbXmlEntityList                    = 0x0014;
const TInt KImpsWbXmlAddList                       = 0x0006;
const TInt KImpsWbXmlRemoveList                    = 0x0028;
const TInt KImpsWbXmlProperty                      = 0x0025;
const TInt KImpsWbXmlLogo                          = 0x0019;
const TInt KImpsWbXmlContentType                   = 0x0010;
const TInt KImpsWbXmlQualifier                     = 0x0026;
const TInt KImpsWbXmlPresenceValue                 = 0x0024;

const TInt KImpsWbXmlAllFunctions                  = 0x0105;
const TInt KImpsWbXmlAllFunctionsRequest           = 0x0106;
const TInt KImpsWbXmlCancelInviteRequest           = 0x0107;
const TInt KImpsWbXmlCancelInviteUserRequest       = 0x0108;
const TInt KImpsWbXmlCapabilityRequest             = 0x010b;
const TInt KImpsWbXmlCapabilityList                = 0x010a;
const TInt KImpsWbXmlCompletionFlag                = 0x0134;
const TInt KImpsWbXmlInviteType                    = 0x0118;
const TInt KImpsWbXmlSearchElement                 = 0x0124;
const TInt KImpsWbXmlPassword                      = 0x0121;
const TInt KImpsWbXmlSessionCookie                 = 0x0130;
const TInt KImpsWbXmlSearchString                  = 0x0133;
const TInt KImpsWbXmlDigestBytes                   = 0x010e;
const TInt KImpsWbXmlInviteID                      = 0x0114;
const TInt KImpsWbXmlInviteNote                    = 0x0115;
const TInt KImpsWbXmlResponseNote                  = 0x0123;
const TInt KImpsWbXmlPollingRequest                = 0x0122;
const TInt KImpsWbXmlLoginRequest                  = 0x011d;
const TInt KImpsWbXmlLoginResponse                 = 0x011e;
const TInt KImpsWbXmlServiceRequest                = 0x012E;
const TInt KImpsWbXmlServiceResponse               = 0x012F;
const TInt KImpsWbXmlClientCapabilityRequest       = 0x010c;
const TInt KImpsWbXmlClientCapabilityResponse      = 0x010D;
const TInt KImpsWbXmlStopSearchRequest             = 0x0131;
const TInt KImpsWbXmlSearchRequest                 = 0x012b;
const TInt KImpsWbXmlSearchResponse                = 0x012c;
const TInt KImpsWbXmlInviteRequest                 = 0x0116;
const TInt KImpsWbXmlLogoutRequest                 = 0x011f;
const TInt KImpsWbXmlDisconnect                    = 0x0110;
const TInt KImpsWbXmlKeepAliveRequest              = 0x011b;
const TInt KImpsWbXmlKeepAliveResponse             = 0x0129;
const TInt KImpsWbXmlInviteResponse                = 0x0117;
const TInt KImpsWbXmlInviteUserRequest             = 0x0119;
const TInt KImpsWbXmlInviteUserResponse            = 0x011a;
const TInt KImpsWbXmlGetSPInfoRequest              = 0x0112;
const TInt KImpsWbXmlGetSPInfoResponse             = 0x0113;
const TInt KImpsWbXmlDigestSchema                  = 0x010f;
const TInt KImpsWbXmlNonce                         = 0x0120;
const TInt KImpsWbXmlSearchPairList                = 0x012a;
const TInt KImpsWbXmlSearchLimit                   = 0x0128;
const TInt KImpsWbXmlSearchID                      = 0x0126;
const TInt KImpsWbXmlSearchIndex                   = 0x0127;
const TInt KImpsWbXmlSearchFindings                = 0x0125;
const TInt KImpsWbXmlSearchResult                  = 0x012d;
const TInt KImpsWbXmlKeepAliveTime                 = 0x011c;
const TInt KImpsWbXmlTimeToLive                    = 0x0132;
const TInt KImpsWbXmlFunctions                     = 0x0111;

const TInt KImpsWbxmlReceiveList                   = 0x0136;
const TInt KImpsWbxmlAutoSubscribe                 = 0x041E;

const TInt KImpsWbXmlWVCSPFeat                     = 0x023c;
const TInt KImpsWbXmlGroupFeat                     = 0x021f;
const TInt KImpsWbXmlPresenceFeat                  = 0x022f;
const TInt KImpsWbXmlIMFeat                        = 0x0223;
const TInt KImpsWbXmlFundamentalFeat               = 0x0211;
const TInt KImpsWbXmlServiceFunc                   = 0x0235;
const TInt KImpsWbXmlSearchFunc                    = 0x0234;
const TInt KImpsWbXmlInviteFunc                    = 0x0227;
const TInt KImpsWbXmlPresenceAuthFunc              = 0x022d;
const TInt KImpsWbXmlPresenceDeliverFunc           = 0x022e;
const TInt KImpsWbXmlAttListFunc                   = 0x0206;
const TInt KImpsWbXmlContListFunc                  = 0x020c;
const TInt KImpsWbXmlIMSendFunc                    = 0x0225;
const TInt KImpsWbXmlIMAuthFunc                    = 0x0222;
const TInt KImpsWbXmlIMReceiveFunc                 = 0x0224;
const TInt KImpsWbXmlGroupMgmtFunc                 = 0x0220;
const TInt KImpsWbXmlGroupUseFunc                  = 0x0221;
const TInt KImpsWbXmlGroupAuthFunc                 = 0x021e;
const TInt KImpsWbXmlSRCH                          = 0x0238;
const TInt KImpsWbXmlSTSRC                         = 0x0239;
const TInt KImpsWbXmlINVIT                         = 0x0226;
const TInt KImpsWbXmlCAINV                         = 0x0209;
const TInt KImpsWbXmlGCLI                          = 0x0214;
const TInt KImpsWbXmlCCLI                          = 0x020B;
const TInt KImpsWbXmlDCLI                          = 0x020F;
const TInt KImpsWbXmlMCLS                          = 0x0229;
const TInt KImpsWbXmlGETWL                         = 0x021B;
const TInt KImpsWbXmlGETSPI                        = 0x021A;
const TInt KImpsWbXmlREACT                         = 0x0230;
const TInt KImpsWbXmlCAAUT                         = 0x0208;
const TInt KImpsWbXmlUPDPR                         = 0x023B;
const TInt KImpsWbXmlCALI                          = 0x020A;
const TInt KImpsWbXmlMDELIV                        = 0x021A;
const TInt KImpsWbXmlGETLM                         = 0x0217;
const TInt KImpsWbXmlSETD                          = 0x0236;
const TInt KImpsWbXmlFWMSG                         = 0x0212;
const TInt KImpsWbXmlGETPR                         = 0x0219;
const TInt KImpsWbXmlDALI                          = 0x020E;
const TInt KImpsWbXmlGALS                          = 0x0213;
const TInt KImpsWbXmlGETM                          = 0x0218;
const TInt KImpsWbXmlREJCM                         = 0x0231;
const TInt KImpsWbXmlNOTIF                         = 0x022C;
const TInt KImpsWbXmlNEWM                          = 0x022B;
const TInt KImpsWbXmlGLBLU                         = 0x021C;
const TInt KImpsWbXmlBLENT                         = 0x0207;
const TInt KImpsWbXmlCREAG                         = 0x020D;
const TInt KImpsWbXmlDELGR                         = 0x0210;
const TInt KImpsWbXmlGETGP                         = 0x0216;
const TInt KImpsWbXmlSETGP                         = 0x0237;
const TInt KImpsWbXmlSUBGCN                        = 0x023A;
const TInt KImpsWbXmlADDGM                         = 0x0205;
const TInt KImpsWbXmlGETGM                         = 0x0215;
const TInt KImpsWbXmlREJEC                         = 0x0232;
const TInt KImpsWbXmlMBRAC                         = 0x0228;
const TInt KImpsWbXmlGRCHN                         = 0x021D;
const TInt KImpsWbXmlRMVGM                         = 0x0233;

const TInt KImpsWbXmlInitialDeliveryMethod         = 0x030b;
const TInt KImpsWbXmlAnyContent                    = 0x0309;
const TInt KImpsWbXmlSupportedBearer               = 0x030f;
const TInt KImpsWbXmlSupportedCIRMethod            = 0x0310;
const TInt KImpsWbXmlAcceptedTransferEncoding      = 0x0308;
const TInt KImpsWbXmlAcceptedCharset               = 0x0305;
const TInt KImpsWbXmlAcceptedContentType           = 0x0307;
const TInt KImpsWbXmlAcceptedContentLength         = 0x0306;
const TInt KImpsWbXmlMultiTrans                    = 0x030c;
const TInt KImpsWbXmlParserSize                    = 0x030d;
const TInt KImpsWbXmlUDPPort                       = 0x0313;
const TInt KImpsWbXmlTCPPort                       = 0x0312;
const TInt KImpsWbXmlServerPollMin                 = 0x030e;
const TInt KImpsWbXmlDefaultLanguage               = 0x030a;
const TInt KImpsWbXmlTCPAddress                    = 0x0311;

const TInt KImpsWbXmlDefaultContactList            = 0x040a;
const TInt KImpsWbXmlDefaultList                   = 0x040b;
const TInt KImpsWbXmlGetPresenceRequest            = 0x0412;
const TInt KImpsWbXmlListManageRequest             = 0x0416;
const TInt KImpsWbXmlSubscribePresenceRequest      = 0x041d;
const TInt KImpsWbXmlUnsubscribePresenceRequest    = 0x0418;
const TInt KImpsWbXmlPresenceNotificationRequest   = 0x041b;
const TInt KImpsWbXmlGetWatcherListRequest         = 0x0414;
const TInt KImpsWbXmlGetWatcherListResponse        = 0x0415;
const TInt KImpsWbXmlGetPresenceResponse           = 0x0413;
const TInt KImpsWbXmlPresenceAuthRequest           = 0x0419;
const TInt KImpsWbXmlGetListRequest                = 0x0410;
const TInt KImpsWbXmlGetListResponse               = 0x0411;
const TInt KImpsWbXmlCreateListRequest             = 0x0408;
const TInt KImpsWbXmlPresenceAuthUser              = 0x041a;
const TInt KImpsWbXmlCancelAuthRequest             = 0x0405;
const TInt KImpsWbXmlUpdatePresenceRequest         = 0x041c;
const TInt KImpsWbXmlDeleteListRequest             = 0x040d;
const TInt KImpsWbXmlCreateAttributeListRequest    = 0x0407;
const TInt KImpsWbXmlDeleteAttributeListRequest    = 0x040c;
const TInt KImpsWbXmlGetAttributeListRequest       = 0x040e;
const TInt KImpsWbXmlGetAttributeListResponse      = 0x040f;
const TInt KImpsWbXmlListManageResponse            = 0x0417;
const TInt KImpsWbXmlDefaultAttributeList          = 0x0409;
const TInt KImpsWbXmlContactListProperties         = 0x0406;

const TInt KImpsWbXmlAccuracy                      = 0x0505;
const TInt KImpsWbXmlAddress                       = 0x0506;
const TInt KImpsWbXmlAddrPref                      = 0x0507;
const TInt KImpsWbXmlAlias                         = 0x0508;
const TInt KImpsWbXmlAltitude                      = 0x0509;
const TInt KImpsWbXmlBuilding                      = 0x050a;
const TInt KImpsWbXmlCaddr                         = 0x050b;
const TInt KImpsWbXmlCity                          = 0x050c;
const TInt KImpsWbXmlClientInfo                    = 0x050d;
const TInt KImpsWbXmlClientProducer                = 0x050e;
const TInt KImpsWbXmlPrClientType                  = 0x050f;
const TInt KImpsWbXmlClientType                    = 0x050f;
const TInt KImpsWbXmlClientVersion                 = 0x0510;
const TInt KImpsWbXmlCommC                         = 0x0511;
const TInt KImpsWbXmlCommCap                       = 0x0512;
const TInt KImpsWbXmlContactInfo                   = 0x0513;
const TInt KImpsWbXmlContainedvCard                = 0x0514;
const TInt KImpsWbXmlCountry                       = 0x0515;
const TInt KImpsWbXmlCrossing1                     = 0x0516;
const TInt KImpsWbXmlCrossing2                     = 0x0517;
const TInt KImpsWbXmlDevManufacturer               = 0x0518;
const TInt KImpsWbXmlDirectContent                 = 0x0519;
const TInt KImpsWbXmlFreeTextLocation              = 0x051a;
const TInt KImpsWbXmlGeoLocation                   = 0x051b;
const TInt KImpsWbXmlLanguage                      = 0x051c;
const TInt KImpsWbXmlLatitude                      = 0x051d;
const TInt KImpsWbXmlLongitude                     = 0x051e;
const TInt KImpsWbXmlModel                         = 0x051f;
const TInt KImpsWbXmlNamedArea                     = 0x0520;
const TInt KImpsWbXmlOnlineStatus                  = 0x0521;
const TInt KImpsWbXmlPLMN                          = 0x0522;
const TInt KImpsWbXmlPrefC                         = 0x0523;
const TInt KImpsWbXmlPreferredContacts             = 0x0524;
const TInt KImpsWbXmlPreferredLanguage             = 0x0525;
const TInt KImpsWbXmlReferredContent               = 0x0526;
const TInt KImpsWbXmlReferredvCard                 = 0x0527;
const TInt KImpsWbXmlRegistration                  = 0x0528;
const TInt KImpsWbXmlStatusContent                 = 0x0529;
const TInt KImpsWbXmlStatusMood                    = 0x052a;
const TInt KImpsWbXmlStatusText                    = 0x052b;
const TInt KImpsWbXmlStreet                        = 0x052c;
const TInt KImpsWbXmlTimeZone                      = 0x052d;
const TInt KImpsWbXmlUserAvailability              = 0x052e;
const TInt KImpsWbXmlCap                           = 0x052f;
const TInt KImpsWbXmlCname                         = 0x0530;
const TInt KImpsWbXmlContact                       = 0x0531;
const TInt KImpsWbXmlCpriority                     = 0x0532;
const TInt KImpsWbXmlCstatus                       = 0x0533;
const TInt KImpsWbXmlNote                          = 0x0534;
const TInt KImpsWbXmlZone                          = 0x0535;

const TInt KImpsWbXmlBlockList                     = 0x0605;
const TInt KImpsWbXmlBlockUserRequest              = 0x0606;
const TInt KImpsWbXmlDeliveryMethod                = 0x0607;
const TInt KImpsWbXmlDeliveryReport                = 0x0608;
const TInt KImpsWbXmlDeliveryReportRequest         = 0x0609;
const TInt KImpsWbXmlForwardMessageRequest         = 0x060a;
const TInt KImpsWbXmlGetBlockedListRequest         = 0x060b;
const TInt KImpsWbXmlGetBlockedListResponse        = 0x060c;
const TInt KImpsWbXmlGetMessageListRequest         = 0x060d;
const TInt KImpsWbXmlGetMessageListResponse        = 0x060e;
const TInt KImpsWbXmlGetMessageRequest             = 0x060f;
const TInt KImpsWbXmlGetMessageResponse            = 0x0610;
const TInt KImpsWbXmlGrantList                     = 0x0611;
const TInt KImpsWbXmlMessageDelivered              = 0x0612;
const TInt KImpsWbXmlMessageInfo                   = 0x0613;
const TInt KImpsWbXmlMessageNotification           = 0x0614;
const TInt KImpsWbXmlNewMessage                    = 0x0615;
const TInt KImpsWbXmlRejectMessageRequest          = 0x0616;
const TInt KImpsWbXmlSendMessageRequest            = 0x0617;
const TInt KImpsWbXmlSendMessageResponse           = 0x0618;
const TInt KImpsWbXmlSetDeliveryMethodRequest      = 0x0619;
const TInt KImpsWbXmlDeliveryTime                  = 0x061a;

const TInt KImpsWbXmlJoinGroup                     = 0x0721;
const TInt KImpsWbXmlJoinedRequest                 = 0x0710;
const TInt KImpsWbXmlSubscribeNotification         = 0x0722;
const TInt KImpsWbXmlSubscribeType                 = 0x0723;
const TInt KImpsWbXmlCreateGroupRequest            = 0x0707;
const TInt KImpsWbXmlDeleteGroupRequest            = 0x0708;
const TInt KImpsWbXmlJoinGroupRequest              = 0x0711;
const TInt KImpsWbXmlJoinGroupResponse             = 0x0712;
const TInt KImpsWbXmlLeaveGroupRequest             = 0x0713;
const TInt KImpsWbXmlLeaveGroupResponse            = 0x0714;
const TInt KImpsWbXmlGetGroupMembersRequest        = 0x0709;
const TInt KImpsWbXmlGetGroupMembersResponse       = 0x070a;
const TInt KImpsWbXmlAddGroupMembersRequest        = 0x0705;
const TInt KImpsWbXmlRemoveGroupMembersRequest     = 0x071b;
const TInt KImpsWbXmlGetGroupPropsRequest          = 0x070b;
const TInt KImpsWbXmlGetGroupPropsResponse         = 0x070c;
const TInt KImpsWbXmlSetGroupPropsRequest          = 0x071c;
const TInt KImpsWbXmlMemberAccessRequest           = 0x0716;
const TInt KImpsWbXmlRejectListRequest             = 0x0719;
const TInt KImpsWbXmlRejectListResponse            = 0x071a;
const TInt KImpsWbXmlSubscribeGroupNoticeRequest   = 0x071d;
const TInt KImpsWbXmlSubscribeGroupNoticeResponse  = 0x071e;
const TInt KImpsWbXmlGroupChangeNotice             = 0x070d;
const TInt KImpsWbXmlGroupProperties               = 0x070e;
const TInt KImpsWbXmlWelcomeNote                   = 0x0720;
const TInt KImpsWbXmlAdmin                         = 0x0706;
const TInt KImpsWbXmlMod                           = 0x0717;
const TInt KImpsWbXmlUsers                         = 0x071f;
const TInt KImpsWbXmlJoined                        = 0x070f;
const TInt KImpsWbXmlLeft                          = 0x0715;
const TInt KImpsWbXmlOwnProperties                 = 0x0718;

// CSP wbxml element value tokens
const TInt KImpsWbXmlDataNotFound                  = 0xff;
const TInt KImpsWbXmlAccessType                    = 0x00;
const TInt KImpsWbXmlActiveUsers                   = 0x01;
const TInt KImpsWbXmlAdminValue                    = 0x02;
const TInt KImpsWbXmlApplicationVndWapMmsMessage   = 0x04;
const TInt KImpsWbXmlApplicationXSms   			   = 0x05;
const TInt KImpsWbXmlAutoJoin                      = 0x06;
const TInt KImpsWbXmlBASE64                        = 0x07;
const TInt KImpsWbXmlClosed                        = 0x08;
const TInt KImpsWbXmlDefault                       = 0x09;
const TInt KImpsWbXmlDisplayName                   = 0x0a;
const TInt KImpsWbXmlFalse                         = 0x0b;
const TInt KImpsWbXmlG                             = 0x0c;
const TInt KImpsWbXmlGR                            = 0x0d;
const TInt KImpsWbXmlHttp                          = 0x0e;
const TInt KImpsWbXmlHttps                         = 0x0f;
const TInt KImpsWbXmlImage                         = 0x10;
const TInt KImpsWbXmlInband                        = 0x11;
const TInt KImpsWbXmlIM                            = 0x12;
const TInt KImpsWbXmlMaxActiveUsers                = 0x13;
const TInt KImpsWbXmlModValue                      = 0x14;
const TInt KImpsWbXmlNameValue                     = 0x15;
const TInt KImpsWbXmlNone                          = 0x16;
const TInt KImpsWbXmlDeliveryMethodN               = 0x17;
const TInt KImpsWbXmlOpen                          = 0x18;
const TInt KImpsWbXmlOutband                       = 0x19;
const TInt KImpsWbXmlPR                            = 0x1a;
const TInt KImpsWbXmlPrivate                       = 0x1b;
const TInt KImpsWbXmlPrivateMessaging              = 0x1c;
const TInt KImpsWbXmlPrivilegeLevel                = 0x1d;
const TInt KImpsWbXmlPublic                        = 0x1e;
const TInt KImpsWbXmlDeliveryMethodP               = 0x1f;
const TInt KImpsWbXmlRequest                       = 0x20;
const TInt KImpsWbXmlResponse                      = 0x21;
const TInt KImpsWbXmlRestricted                    = 0x22;
const TInt KImpsWbXmlScreenname                    = 0x23;
const TInt KImpsWbXmlSearchable                    = 0x24;
const TInt KImpsWbXmlS                             = 0x25;
const TInt KImpsWbXmlSC                            = 0x26;
const TInt KImpsWbXmltext                          = 0x27;
const TInt KImpsWbXmlTextPlain                     = 0x28;
const TInt KImpsWbXmlTopic                         = 0x2b;
const TInt KImpsWbXmlTrue                          = 0x2c;
const TInt KImpsWbXmlType                          = 0x2d;
const TInt KImpsWbXmlU                             = 0x2e;
const TInt KImpsWbXmlUS                            = 0x2f;

const TInt KImpsWbXmlAutoDelete					   = 0x31;
const TInt KImpsWbXmlValidityValue	   			   = 0x33;
const TInt KImpsWbXmlDenied 					   = 0x34;
const TInt KImpsWbXmlGranted 					   = 0x35;
const TInt KImpsWbXmlPending 					   = 0x34;
	

const TInt KImpsWbXmlGROUP_ID                      = 0x3d;
const TInt KImpsWbXmlGROUP_NAME                    = 0x3e;
const TInt KImpsWbXmlGROUP_TOPIC                   = 0x3f;
const TInt KImpsWbXmlGROUP_USER_ID_JOINED          = 0x40;
const TInt KImpsWbXmlGROUP_USER_ID_OWNER           = 0x41;
const TInt KImpsWbXmlHTTP                          = 0x42;
const TInt KImpsWbXmlSMS                           = 0x43;
const TInt KImpsWbXmlSTCP                          = 0x44;
const TInt KImpsWbXmlSUDP                          = 0x45;
const TInt KImpsWbXmlUSER_ALIAS                    = 0x46;
const TInt KImpsWbXmlUSER_FIRST_NAME               = 0x48;
const TInt KImpsWbXmlUSER_EMAIL_ADDRESS            = 0x47;
const TInt KImpsWbXmlUSER_ID                       = 0x49;
const TInt KImpsWbXmlUSER_LAST_NAME                = 0x4a;
const TInt KImpsWbXmlUSER_ONLINE_STATUS            = 0x4c;
const TInt KImpsWbXmlUSER_MOBILE_NUMBER            = 0x4b;
const TInt KImpsWbXmlWAPSMS                        = 0x4d;
const TInt KImpsWbXmlWAPUDP                        = 0x4e;
const TInt KImpsWbXmlWSP                           = 0x4f;

const TInt KImpsWbXmlANGRY                         = 0x5b;
const TInt KImpsWbXmlANXIOUS                       = 0x5c;
const TInt KImpsWbXmlAshamed                       = 0x5d;
const TInt KImpsWbXmlAudioCall                     = 0x5e;
const TInt KImpsWbXmlAvailable                     = 0x5f;

const TInt KImpsWbXmlBored                         = 0x60;
const TInt KImpsWbXmlCALL                          = 0x61;
const TInt KImpsWbXmlCli                           = 0x62;
const TInt KImpsWbXmlComputer                      = 0x63;
const TInt KImpsWbXmlDiscreet                      = 0x64;
const TInt KImpsWbXmlEMAIL                         = 0x65;
const TInt KImpsWbXmlExcited                       = 0x66;
const TInt KImpsWbXmlHappy                         = 0x67;
const TInt KImpsWbXmlCapIM                         = 0x68;
const TInt KImpsWbXmlIM_OFFLINE                    = 0x69;
const TInt KImpsWbXmlIM_ONLINE                     = 0x6a;
const TInt KImpsWbXmlIN_LOVE                       = 0x6b;
const TInt KImpsWbXmlINVICIBLE                     = 0x6c;
const TInt KImpsWbXmlJEALOUS                       = 0x6d;
const TInt KImpsWbXmlMMS                           = 0x6e;
const TInt KImpsWbXmlMobilePhone                   = 0x6f;

const TInt KImpsWbXmlNotAvailable                  = 0x70;
const TInt KImpsWbXmlOther                         = 0x71;   
const TInt KImpsWbXmlPda                           = 0x72;
const TInt KImpsWbXmlSAD                           = 0x73;    
const TInt KImpsWbXmlSLEEPY                        = 0x74;
const TInt KImpsWbXmlSms                           = 0x75;
const TInt KImpsWbXmlVideoCall                     = 0x76;
const TInt KImpsWbXmlVideoStream                   = 0x77;

// CSP wbxml attribute value tokens
const TInt KImpsWbXmlCsp11Xmlns                      = 0x05;
const TInt KImpsWbXmlPa11Xmlns                       = 0x06;
const TInt KImpsWbXmlTrc11Xmlns                      = 0x07;
const TInt KImpsWbXmlCsp12Xmlns                      = 0x08;
const TInt KImpsWbXmlPa12Xmlns                       = 0x09;
const TInt KImpsWbXmlTrc12Xmlns                      = 0x0A;
// FORWARD DECLARATIONS
class MImpsKey;

#endif      // ?INCLUDE_H   
            
// End of File
