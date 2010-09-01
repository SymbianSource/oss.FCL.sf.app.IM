/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Enumerations and Typedefs on Process Component Side
*
*/
#ifndef TENUMSPC_H
#define TENUMSPC_H

#include <e32base.h>
#include <bamdesca.h>

/*
	This is main interface of arraywrapper on Process component side.
	UI Side array decorator has to use this array and get the details required....
*/

class TEnumsPC
    {

    public:	// Enumerations and typedefs

        enum TUnreadFilter
            {
            EUnreadAll,
            EUnreadReceived
            };
        enum TImpsCSPAllErrors
            {
            EImpsErrNone = 0,
            ECSPInsufficientGroupPrivileges,
            ECSPNotSupported,
            ECSPInvalidParameter,
            ECSPMultipleErrors //general errors
            };


        enum TContactEditorMode
            {
            EEditContact = 0,
            EAddToContact = 1,
            ECreateNew = 2,
            EEditAfterView = 3,
            EMoveContact = 4,
            EViewContact = 5,
            EUnknownMode = -1
            };
        enum TItem
            {
            EInvalid = -1,
            EOwnStatusItem,
            EContactItem,
            EContactListItem,
            EOpenChatsListItem,
            EConversationItem,
            EInviteItem,
            EGroupItem,
            EAll
            };

        enum TClientType
            {
            EUnknownClient = 0,
            EMobile,
            EPC
            };

        enum TFilterType
            {
            EFilterNotDefined = -1,
            EFilterAll,
            EFilterAndOperation,
            EFilterOnline,
            EFilterAway,
            EFilterNonOffline, // this means online and away
            EFilterNonBlocked,
            EFilterAllNonBlockedAndOperation,
            EFilterNonOfflineNonBlockedAndOperation,
            EFilterNonOfflineNonBlockedNonOfflineOperation,
            EFilterAllNonBlockedNonOfflineOperation
            };

        enum TOnlineStatus
            {
            EUnknown = -1,
            EOffline,
            EOnline,
            EInvisible,
            EAway,
            EBusy
            };

        enum TChange
            {
            EUnknownEvent = -1,
            EGroupChange, // this can be change in column B.....most of times....
            EAddition,
            EDelete,
            EChanged,
            EMultipleChanges,
            EUnReadCountChanged
            };

        enum TSyncStatus
            {
            ESyncUnknown = -1,
            ESyncFailed,
            ESyncNotDone,
            ESyncSuccess,
            };
        enum TCASettingStrings
            {
            EUnknownSettings = -1,
            EDefaultScreenName = 0,
            EOwnAlias,
            EStatusMsgOnline,
            EStatusMsgAway,
            EStatusMsgBusy,
            EOwnWVUserID
            };


        enum TCASettingValues
            {
            EUnknownValue = -1,
            EAllValue = 0,
            EFriends,
            ENobody,
            EDefaultScreenNameInUse,
            EAuthorizeIMPresence,
            EReceiveIMessages,
            EReceiveInvitations,
            EMessageFlowSettingLevel,
            EShowTimeStamps,
            EAutomaticPresenceUpdate,
            EShowOffline,
            EFriendsListOrdering
            };

        enum TCAFriendsListOrdering
            {
            EAlphabetical = 0,
            EPresence
            };


        enum TCABackgroundTasks
            {
            EGroupFetch = 1,
            EPresenceTask = 2,
            EWhisperSync = 4,
            //used just in 2.1
            EBlockGrantListFetch = 8,
            EFailed = -1//Not there in the engine side
            };


        enum TCABackgroundStatus
            {
            //add or removed
            EUnknownStatus = 0
            //enqueued and CIdle has been started
            , EIdleStarted = 1
            //CIdle is working
            , EStarting = 2
            //CActiveSchedulerWait has been started
            , EWaiting = 4
            //task or subtask has been completed
            , ECompleted = 8
            //task or subtask has been cancelled due to network loss or exit
            , ECancelled = 16
            //task or subtask has been failed and left
            , EFailedStatus = 32
            };

        // Search by which criteria
        enum TSearchCriteria
            {
            ESearchGroupName = 0,
            ESearchGroupTopic,
            ESearchGroupUserIDJoined,
            ESearchUserName,
            ESearchUserID,
            ESearchUserMobileNumber,
            ESearchUserEmailAddress,
            ESearchUserLastName
            };
    public: // Definitions for Message class

        /**
         * Different message types
         */
        enum TMessageType
            {
            EMessagePTOP,
            EMessageWhisper,
            EMessageGroup,
            EMessageSystem,
            EMessageInvalid
            };

        /**
         * System message enumerations.
         */
        enum TSystemMessage
            {
            ESystemMessageNone,
            ESystemMessageWelcome,
            ESystemMessageNewUsers,
            ESystemMessageTopicChanged,
            ESystemMessagePrivatChanged,
            ESystemMessageLeftUsers,
            ESystemMessageDateChange
            };

        /**
         * Direction of message. Coming or going or other.
         */
        enum TMessagerType
            {
            EMessageSent,
            EMessageReceived,
            EMessageOther
            };

        /**
         * Different message content types
         */
        enum TContentType
            {
            EContentText,
            EContentPicture,
            EContentOther,
            EContentInvalid
            };

        /**
         * Content processing states
         */
        enum TContentProcessState
            {
            EContentNotProcessed = 0,
            EContentNotSupported,
            EContentNotSupportedDrm,
            EContentCorrupted,
            EBitmapDecoded,
            EBitmapScaled,
            EContentReady,
            EContentDecoded,
            EThumbnailReady
            };

        enum TChatType
            {
            EPTOPChat   = 1,
            EGroupChat,
            EUnknownChatType = -1
            };
        enum TInviteType
            {
            ENewInvite,
            EInviteCancelOrExpired,
            EInviteRead,
            EInviteUnknown
            };
        enum TListSelectionType
            {
            EUnknownSelect = -1,
            ESingleListViewSelect,
            ESingleSelect,
            EMultiSelect,
            ESingleListMultiSelect
            };

        enum TImpsPropertyBoolPC
            {
            EImpsPropUndef = -1,    // undefined
            EImpsPropNo ,        // false, no, ...
            EImpsPropYes         // true, yes
            };


        /**
         * Events for observer to handle.
         */
        enum TChatEventType
            {
            EGroupListChanged,
            EChatListChanged,
            ESendListChanged,
            EUnreadCountChanged, // Happens with new message and when messages are read.
            EMemoryLow,
            EEventNotSpecified
            };

        /**
        * Events for Forward status
        */
        enum TForwardStatus
            {
            EUnregistered = 0,
            EToBeUnregistered,
            ERegister,
            EAlreadyRegistered
            };


    };

#endif      // TENUMSPC_H

