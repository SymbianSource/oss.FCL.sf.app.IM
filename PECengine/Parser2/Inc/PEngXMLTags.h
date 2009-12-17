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
* Description:  All used XML tags collected in one place.
*
*/

#ifndef __PENGXMLTAGS_H__
#define __PENGXMLTAGS_H__

//XML default name space
_LIT8( KXmlXmlns, "xmlns" );


// General stuff
_LIT8( KTransactionContentNS, "http://www.wireless-village.org/TRC1.1" );
_LIT8( KTransactionContentNS_CSP12, "http://www.openmobilealliance.org/DTD/WV-TRC1.2" );
_LIT8( KTransactionContent, "TransactionContent" );

_LIT8( KPresenceSubListAttributesNS, "http://www.wireless-village.org/PA1.1" );
_LIT8( KPresenceSubListAttributesNS_CSP12, "http://www.openmobilealliance.org/DTD/WV-PA1.2" );


// Presence Update tags
_LIT8( KGetPresenceRequest, "GetPresence-Request" );
_LIT8( KPresenceSubList, "PresenceSubList" );
_LIT8( KUpdatePresenceRequest, "UpdatePresence-Request" );
_LIT8( KGetPresenceResponse, "GetPresence-Response" );
_LIT8( KPresenceNotificationRequest, "PresenceNotification-Request" );



// List tags
_LIT8( KListMnageRequest, "ListManage-Request" );
_LIT8( KContactList, "ContactList" );
_LIT8( KDefaultContactList, "DefaultContactList" );
_LIT8( KNickList, "NickList" );
_LIT8( KDefaultList, "DefaultList" );
_LIT8( KAddNickList, "AddNickList" );
_LIT8( KRemoveNickList, "RemoveNickList" );
_LIT8( KContactListProperties, "ContactListProperties" );
_LIT8( KProperty, "Property" );
_LIT8( KDispName, "DisplayName" );
_LIT8( KGetListOfList, "GetList-Request" );
_LIT8( KGetWatchers, "GetWatcherList-Request" );
_LIT8( KSubscribePresence, "SubscribePresence-Request" );
_LIT8( KCreateList, "CreateList-Request" );
_LIT8( KDeleteList, "DeleteList-Request" );
_LIT8( KCreateAttributeList, "CreateAttributeList-Request" );
_LIT8( KDeleteAttributeList, "DeleteAttributeList-Request" );
_LIT8( KUnsubscribePresence, "UnsubscribePresence-Request" );
_LIT8( KAutoSubscribe, "AutoSubscribe" );
_LIT8( KReceiveList, "ReceiveList" );

// Authorization tags
_LIT8( KPresenceAuthRequest, "PresenceAuth-Request" );
_LIT8( KPresenceAuthUser, "PresenceAuth-User" );
_LIT8( KCancelAuthRequest, "CancelAuth-Request" );
_LIT8( KAcceptance, "Acceptance" );



// Presence tags
_LIT8( KResultXMLTag, "Result" );
_LIT8( KDescriptionXMLTag, "Description" );
_LIT8( KPresenceXMLTag, "Presence" );
_LIT8( KPresenceValueXMLTag, "PresenceValue" );
_LIT8( KQualifierXMLTag, "Qualifier" );
_LIT8( KDetailedResultXMLTag, "DetailedResult" );
_LIT8( KXMLValueTrue, "T" );
_LIT8( KXMLValueFalse, "F" );
_LIT8( KUserIDXMLTag, "UserID" );
_LIT8( KUser, "User" );
_LIT8( KWVXMLTag, "wv:" );
_LIT8( KNickName, "NickName" );
_LIT8( KName, "Name" );
_LIT8( KValue, "Value" );
_LIT8( KDefault, "Default" );
_LIT8( KStatusTag, "Status" );
_LIT8( KCodeXMLTag, "Code" );
_LIT8( KWVCodeOK, "200" );
_LIT8( K200Code, "200" );
_LIT8( K201Code, "201" );
_LIT8( K700Code, "700" );
_LIT8( K752Code, "752" );
_LIT8( K531Code, "531" );
_LIT8( K604Code, "604" );
_LIT8( K506Code, "506" );
_LIT8( K405Code, "405" );


// StatusContent attribute value tags
_LIT8( KStatusContentXMLTag, "StatusContent" );
_LIT8( KDirectContentXMLTag, "DirectContent" );
_LIT8( KContentTypeXMLTag, "ContentType" );


// Availability attribute value tags
_LIT8( KUserAvailabilityXMLTag, "UserAvailability" );
_LIT8( KAvailabilityValueAvailable, "AVAILABLE" );
_LIT8( KAvailabilityValueNotAvailable, "NOT_AVAILABLE" );
_LIT8( KAvailabilityValueDiscreet, "DISCREET" );

// StatusText attribute value tags
_LIT8( KStatusTextXMLTag, "StatusText" );

// Online Status attribute value tags
_LIT8( KOnlineStatusXMLTag, "OnlineStatus" );

// CommCap attribute value tags
_LIT8( KCommCapXMLTag, "CommCap" );
_LIT8( KCommCXMLTag, "CommC" );
_LIT8( KCommCapXMLTagCap, "Cap" );
_LIT8( KCommCapStatusXMLTag, "Status" );
_LIT8( KCommCapNoteXMLTag, "Note" );
_LIT8( KCommCapIMXMLTag, "IM" );
_LIT8( KCommCapStatusOpen, "OPEN" );
_LIT8( KCommCapStatusClosed, "CLOSED" );

// ClientInfo attribute value tags
_LIT8( KClientInfoXMLTag, "ClientInfo" );
_LIT8( KClientTypeXMLTag, "ClientType" );
_LIT8( KClientLanguageXMLTag, "Language" );
_LIT8( KDevManufacturerXMLTag, "DevManufacturer" );
_LIT8( KClientProducerXMLTag, "ClientProducer" );
_LIT8( KDeviceModelXMLTag, "Model" );
_LIT8( KClientVersionXMLTag, "ClientVersion" );
_LIT8( KClientTypeMobilePhoneXMLTag, "MOBILE_PHONE" );
_LIT8( KClientTypeComputerXMLTag, "COMPUTER" );
_LIT8( KClientTypePDAXMLTag, "PDA" );
_LIT8( KClientTypeCLIXMLTag, "CLI" );
_LIT8( KClientTypeOtherXMLTag, "OTHER" );


// InfoLink attribute value tags
_LIT8 ( KInfoLinkXMLTag, "InfoLink" );
_LIT8 ( KInfoLinkXMLTagInf, "Inf_link" );
_LIT8 ( KInfoLinkXMLTagLink, "Link" );
_LIT8 ( KInfoLinkXMLTagText, "Text" );

// Alias attribute value tags
_LIT8( KAliasXMLTag, "Alias" );


#endif      // __PENGXMLTAGS_H__


