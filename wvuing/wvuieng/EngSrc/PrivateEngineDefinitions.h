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
* Description:  Private constants for engine
*
*/


#ifndef PRIVATEENGINEDEFINITIONS_H
#define PRIVATEENGINEDEFINITIONS_H

#include <e32base.h>

// Settings-manager constants
// Flow controller...overwrited in the beginning of the app
/*const TInt KDefaulFlowControllerValue   = 333000;   // 333000
const TInt KKeyNameMaxNumberLength = 5; // 65536
_LIT( KKeyNameFormatter, "%d" );
const TInt KMessageFlowMaxValue = 10; // Maximum value of setting slider
const TInt KMessageFlowCoEfficientValue = 200;	// CoEfficient, that is used
												// to calculate messages min
												// display interval with
												// level parameters (in msecs).
												// CoEfficient * level = result
*/
// Default values for settings
const TInt KDefaultMessageFlowSettingLevel = 2;

const TBool KDefaultAutoLogin = EFalse;
const TBool KDefaultDefaultScreenNameInUse = EFalse;
const TBool KDefaultFirstLoginDone = EFalse;
const TBool KDefaultAutomaticPresenceUpdate = ETrue;
const TBool KDefaultUpdateSelectedContacts = EFalse;
const TBool KDefaultShowHistory = EFalse;
const TBool KDefaultShowOffline = ETrue;
const TBool KDefaultShowTimeStamps = ETrue;

_LIT( KDefaultScreenName, "" );

// MEMORY SECTION USED WHEN CHECKING FREE SPACE (OOM SITUATIONS)///////////////
// This is used in consumed memory calculation (history).
#ifdef _UNICODE
const TInt KCharacterLengthInBytes = 2;
#else
const TInt KCharacterLengthInBytes = 1;
#endif

// Resource name ( CaEngine.rsc )
// remember add KDC_RESOURCE_FILES_DIR to beginning when using
_LIT( KResourceEngineFilePathWithoutDrive, "CaEngineNG.rsc" );

// userid-part (wv:userid) length of groupid (wv:userid/resource@domain.fi)
const TInt KGroupIdFirstPartMaxLength = 20;
// resource-part (resource) length of groupid (wv:userid/resource@domain.fi)
const TInt KGroupIdSecondPartMaxLength = 20;

// wv-address resource separator
_LIT( KResourceSeparator, "/" );

// Line end
_LIT( KLineEnd, "\n" );

// Default timeout for invite when received invite does not contain timeout value (seconds)
const TInt KDefaultInviteTimeout		= 7200;
// Treshold of invite expiry timeout in seconds (x < TIntMax / 1000000 )
const TInt KInviteTimeoutCycleLength 	= 1000;
// Local echo
const TBool KLocalEcho                  = ETrue;
// No view is active
const TInt KNoViewIsActive              = -1;

// ARRAY GRANULARITIES/////////////////////////////////////////////////////////
// general array granularity
const TInt KArrayGranularity = 3;

// Increase chats by this amount
const TInt KChatGranularity = 2;

// Message buffer granularities/////////////
// Send message buffer granularity
/*const TInt KSendMessageGranularity = 5;
// Incoming message buffer granularity
const TInt KIncomingMessageGranularity = 5;
// One message buffer granularity
const TInt KOneMessageGranularity = 2;
// Screen message buffer granularity
const TInt KScreenMessageGranularity = 5;
// /////////////////////////////////////////

// Chat groups granularity ( favorites )
const TInt KChatGroupsGranularity = 1;*/
// Joined members in a group
const TInt KJoinedMembersArrayGranularity = 3;
// Contacts granularity. This is all the "friends"
/*const TInt KContacts = 1;

// Group event observer granularities
const TInt KGroupObserverGranularity = 2;
*/
// Search data granularities
const TInt KSearchDataGranularity = 2;
/*
// Removed indexes granularity
const TInt KRemovedIndexGranularity = 2;

// ////////////////////////////////////////////////////////////////////////////

// Max length of one IM message. Effects to the fragmentation of messages
const TInt KMessageMaxLength = 128;

// Maximum memory consumption section//////////////////
const TInt KMaximumScreenMemoryConsumption      = 2000;
const TInt KMaximumIncomingMemoryConsumption    = 2000;

const TInt KMaximumSendMemoryConsumption        = 500;
// ////////////////////////////////////////////////////

// Safety boundaries for screen buffer in split messages
const TInt KUpperSafetyBoundary     = 3;
const TInt KLowerSafetyBoundary     = 3;
// /////////////////////////////////////////////////////

const TInt KTimerDelay = 1;
*/


// LOGGING (HISTORY) section///////////////////////////////////////////////////
// Clock section
const TInt KMaxDateLength           = 50;
const TInt KMaxTimeLength           = 20;

_LIT( KFromMe, "O->" );
_LIT( KToMe, "->O" );
_LIT( KWhiteSpace, " " );
// LOGGING section ends////////////////////////////////////////////////////////

// LITERALS
_LIT( KAt, "@" );
_LIT( KSlash, "/" );
_LIT( KWV, "wv:" );
_LIT( KPlus, "+" );
_LIT( KTabulator, "\t" );
_LIT( KPercent, "%" );
_LIT( KPercentHTTP, "%25" );

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;

// For message fragmenting
_LIT( KMessageContinuesString, "..." );

// UIDs////////////////////////////////////////////////////////////////////////
const TInt KUIDWVIDField                    = 1;
const TInt KUIDNameField                    = 2;
const TInt KUIDRealNameField                = 3;

const TInt KUIDGroupField                   = 4;
const TInt KUIDGroupNameField               = 5;
const TInt KUIDGroupTopicField              = 6;
const TInt KUIDGroupWelcomeField            = 7;
const TInt KUIDGroupSizeField               = 8;
const TInt KUIDGroupAllowSearchField        = 9;
const TInt KUIDGroupAccessField             = 10;
const TInt KUIDGroupAllowWhisperingField    = 11;
const TInt KUIDGroupAdministratorField      = 12;
const TInt KUIDGroupIsFavouriteField        = 13;
const TInt KUIDGroupIsOpenField             = 14;
const TInt KUIDGroupAdministratorsField     = 15;

const TInt KUIDWatcherStateField            = 30;
// ////////////////////////////////////////////////////////////////////////////


// Flow controller states
enum TTimerStates
    {
    ETimerActivated,
    ETimerDeactivated
    };

// Request for our own inner mapping for operation codes and
// completion functions
enum TOperationRequests
    {
    EJoinGroup                              = 1,
    ECreateGroup                            = 2,
    ECreateAndJoinGroup                     = 3,
    ECreateAlreadyExistingFavouriteGroup    = 4,
    ELeaveGroup                             = 5,
    EDeleteGroup                            = 6,
    EDeleteNonExistentGroup                 = 7,

    // GROUP PROPERTIES////////////////////////////////////////////////////////
    // Used to get group name after join
    ENoNeedToGetGroupProperties             = 10,
    // Used to get properties of a group we do not have
    EUnknownGroupGetPropertiesFromNetwork   = 11,
    EAfterJoinGroupProperties               = 12,
    EGroupProperties                        = 13,
    // ////////////////////////////////////////////////////////////////////////

    EJoinToNonFavouriteGroup                = 14,

    EModifyMembersAccess        = 15,
    EGroupMembers               = 16,
    ESynchGroupMembers          = 17,
    ESetGroupProperties         = 18,
    EAddMembers                 = 19,
    ERemoveMembers              = 20,

    // In application shutdown
    ELeaveInApplicationShutdown = 21,

    // Subscription
    ESubscribe                  = 30,
    EUnSubscribe                = 31,
    ECheckSubscription          = 32,
    ESetRejectList              = 33,

    // Blocking
    EBlockRequest               = 34,
    EUnBlockRequest             = 35,
    EGetBlockedList             = 36,

    // Access
    ELoginRequest               = 37,
    ELogoutRequest              = 38

    };

// ////////////////////////////////////////////////////////////////////////////

#endif      // PRIVATEENGINEDEFINITIONS_H

// End of File
