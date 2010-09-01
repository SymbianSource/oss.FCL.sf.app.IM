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
* Description:  Public constants for IM app
*
*/


#ifndef PUBLICENGINEDEFINITIONS_H
#define PUBLICENGINEDEFINITIONS_H

#include <e32std.h>

// Any file
_LIT( KHistoryFilePattern, "*.*" );

enum TChatType
    {
    EPTOPChat   = 1,
    EGroupChat
    };

// Network state transitions
enum TNetworkState
    {
    ELoggedOut = 0,
    ELoggingOut,
    ELoggedIn,
    ESyncPresence,
    EServerPrefers
    };

// State of bacground-tasks
enum TCallBackState
    {
    EStarting = 0,
    EDone
    };

// Panic definitions
_LIT( KCAEnginePanicCategory, "Chat engine" );

enum TIMEnginePanicCodes
    {
    EBackgroundTaskNotFound = 1
    };

// User section
const TInt KWVNickMaxLength         = 50;
const TInt KWVUserIDMaxLength       = 50;
const TInt KWVNameMaxLength         = 50;
const TInt KWVRealNameMaxLength     = 50;
const TInt KWScreenNameMaxLength    = 50;
const TInt KMaxRejectReasonLength   = 50;

// Group section
const TInt KMaxGroupIDLength        = 50;
const TInt KMaxGroupNameLength      = 50;
const TInt KMaxGroupTopicLength     = 50;
const TInt KMaxWelcomeNoteLength    = 100;

// History ( logging )
const TInt KMaxLogFileName          = 30;

// ERROR CODES

const TInt KMessageErrorBase        = -13000;
const TInt KDatabaseErrorBase       = -13200;
const TInt KGroupErrorBase          = -13400;
const TInt KBuffersErrorBase        = -13600;
const TInt KUISearchErrorBase       = -14000;
const TInt KPresenceErrorBase       = -14200;
const TInt KInviteErrorBase         = -14400;
const TInt KSettingsErrorBase       = -14600;
const TInt KNetworkErrorBase        = -14800;

enum TPresenceErrors
    {
    EWatcherRemoveNotFound						= KPresenceErrorBase - 1,
    EContactNotFound							= KPresenceErrorBase - 2
    };

enum TErrorCodes
    {
    // general section
    EGeneralServerError                         = KNetworkErrorBase - 1,
    EServerContactListFull                      = KNetworkErrorBase - 2,

    // Message section
    EReallocFailedInSuitableLogFile             = KMessageErrorBase - 1,
    EMessageMaxLengthLessThanOne                = KMessageErrorBase - 2,
    ESuitableFileNameFailed                     = KMessageErrorBase - 3,
    ECorruptedContent                           = KMessageErrorBase - 4,
    ECouldntDeleteChat                          = KMessageErrorBase - 5,
    ECouldntFindSendMessage                     = KMessageErrorBase - 6,
    ECouldntFindChatData                        = KMessageErrorBase - 7,
    ECouldntAddToMapper                         = KMessageErrorBase - 8,
    EInvalidIndex                               = KMessageErrorBase - 9,
    EStartLoggingFailed                         = KMessageErrorBase - 10,
    EGroupAlreadyExists                         = KMessageErrorBase - 11,
    ECouldntActivateChat                        = KMessageErrorBase - 12,
    ESenderLengthZero                           = KMessageErrorBase - 13,
    EAddMessageFailed                           = KMessageErrorBase - 14,
    ELoggingAlreadyOn                           = KMessageErrorBase - 16,
    ESubscribeToGroupFailed                     = KMessageErrorBase - 17,
    ECannotFreeMemoryFromScreenBuffer           = KMessageErrorBase - 18,
    EUserNameTooLong                            = KMessageErrorBase - 19,
    ETryNewFileName                             = KMessageErrorBase - 20,
    EIncomingBufferFullOfSendMessages           = KMessageErrorBase - 21,
    ESendMessagesBufferFull                     = KMessageErrorBase - 22,
    ENoActiveView                               = KMessageErrorBase - 23,

    EDeleteGroupFailed                          = KMessageErrorBase - 23,
    ECreateGroupFailed                          = KMessageErrorBase - 24,
    ELeaveGroupFailed                           = KMessageErrorBase - 25,
    EGroupMembersFailed                         = KMessageErrorBase - 26,
    EAddMembersFailed                           = KMessageErrorBase - 27,
    ERemoveMembersFailed                        = KMessageErrorBase - 28,
    EModifyMembersAccessFailed                  = KMessageErrorBase - 29,
    ESetGroupPropsFailed                        = KMessageErrorBase - 30,
    ESetRejectListFailed                        = KMessageErrorBase - 31,
    ESubscribeFailed                            = KMessageErrorBase - 32,
    EUnsubscribeFailed                          = KMessageErrorBase - 33,
    ECheckSubscriptionFailed                    = KMessageErrorBase - 34,

    // Database section
    EUnableToOpenDatabase           = KDatabaseErrorBase - 1,
    EParameterLengthTooSmall        = KDatabaseErrorBase - 2,
    EParameterLengthTooBig          = KDatabaseErrorBase - 3,
    ENoTemplates                    = KDatabaseErrorBase - 4,
    ECouldntFindTextField           = KDatabaseErrorBase - 5,
    ECouldntFindGroupTextField      = KDatabaseErrorBase - 6,
    ECommitCouldntFindContact       = KDatabaseErrorBase - 7,
    ENullParameterPointer           = KDatabaseErrorBase - 8,
    //ECommitAccessDenied             = KDatabaseErrorBase - 9,
    ECannotUpdateAdmin              = KDatabaseErrorBase - 10,
    EFriendNotFound                 = KDatabaseErrorBase - 11,
    ECouldntFindGroup               = KDatabaseErrorBase - 12,
    ESearchByNameDidntFindName      = KDatabaseErrorBase - 13,
    ENoData                         = KDatabaseErrorBase - 14,
    ECouldntRemoveScreenName        = KDatabaseErrorBase - 15,
    ENoOwnRequestResponsePair       = KDatabaseErrorBase - 16,
    ETryingToAddNullGroupObserver   = KDatabaseErrorBase - 17,
    ETryingToRemoveNullGroupObserver = KDatabaseErrorBase - 18,
    EGroupNotFound					= KDatabaseErrorBase - 20,
    // Error to the UI side through iFriendsListObserver
    EWirelessVillageIDExists        = KDatabaseErrorBase - 21,
    EErrorLoadingEngineResourceFile = KDatabaseErrorBase - 22,
    ESavePersistentGroupFailed      = KDatabaseErrorBase - 23,
    ESavePersistentContactFailed    = KDatabaseErrorBase - 24,
    EErrorLoadingEngineLocalizationFile = KDatabaseErrorBase - 25,

    // Callbacks coming from wv engine
    EUserEnteringGroupNoGroup       = KGroupErrorBase - 1,
    EUserLeavingGroupNoGroup        = KGroupErrorBase - 2,
    EJoinGroupFailed                = KGroupErrorBase - 3,
    EGroupPropertiesFailed          = KGroupErrorBase - 4,

    EMaxMessageLengthIsGreaterThanMessageBuffer     = KBuffersErrorBase - 1,
    EScreenBufferParametersAreWrong                 = KBuffersErrorBase - 2,
    EMessageTypeIndexNotInValidRange                = KBuffersErrorBase - 3,
    EApplicationUserSentThisIndexNotInValidRange    = KBuffersErrorBase - 4
    };

enum TSearchManagerErrorCodesToUI
    {
    ESearchFailed                       = KUISearchErrorBase - 1,
    EECouldntSaveSearchResults          = KUISearchErrorBase - 2,
    EInvalidSearchArrayIndex            = KUISearchErrorBase - 3,
    ERequestedResultsButNotAvailable    = KUISearchErrorBase - 4,
    ESearchNotSupported                 = KUISearchErrorBase - 5
    };

enum TInviteErrorCodes
    {
    EMissingInviteIDTemplate			= KInviteErrorBase - 1,
    ECouldNotPrepareGroupForInvite		= KInviteErrorBase - 2,
    EUserNotAdmin						= KInviteErrorBase - 3
    };

enum TSettingsErrorCodes
    {
    EStringTooLong						= KSettingsErrorBase - 1,
    EInvalidStringEnumDefinition		= KSettingsErrorBase - 2,
    EOwnUserIdNotAvailable				= KSettingsErrorBase - 3
    };

enum TOperationErrorCodes
    {
    EOperationCancelled			= -15001
    };
// ERROR CODES END

#endif      // PUBLICENGINEDEFINITIONS_H

// End of File
