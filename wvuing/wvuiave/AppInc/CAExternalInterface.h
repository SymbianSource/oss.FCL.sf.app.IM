/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file with UID definitions
 *
*/

#ifndef CAEXTERNALINTERFACE_H
#define CAEXTERNALINTERFACE_H
#include "ChatNGCommonUIDs.h"

#include <e32std.h>

// Application UID
const TUid KUidChatClient = {KWVUIAVEEXEUIDS2};

// View UIDs
const TUid KUidMainView = {0};

const TUid KUidChatListView = {2};
const TUid KUidChatView = {3};
const TUid KUidChatPropertiesView = {4};
const TUid KUidSettingsView = {5};
const TUid KUidSearchView = {6};
const TUid KUidSearchViewMsgIdUser = {101};
const TUid KUidSearchViewMsgIdGroup = {102};

const TUid KUidChatPrivateChatListView = {7};
const TUid KUidFriendsListView = {7}; //similar to KUidChatPrivateChatListView to launch application thro soft notification
const TUid KUidChatPrivateChatListViewSoftNoteActivationId = {42};
const TUid KUidConversationsView = {8};
const TUid KUidChatViewMsgId = {9};
const TUid KUidConvViewMsgId = {10};
const TUid KUidRecordedChatListView = {11};
const TUid KUidRecordedChatView = {12};
const TUid KUidRefreshView = {13};
const TUid KUidInvitationView = {20};
const TUid KUidInvViewMsgId = {21};
const TUid KConvInvGroupFocusId = {43};
// UID for external view activation message. Use TCADnlConvView to relay needed
// data in custom message
const TUid KUidExtViewActivationId = {0x10203040};

#endif // CAEXTERNALINTERFACE_H

// End of File
