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
* Description:  This header contains constants that are used in various places in IM
*
*/


#ifndef CHATDEFINITIONS_H
#define CHATDEFINITIONS_H

#include "impsbuilddefinitions.h"

#include <ApEngineConsts.h>
#include <eikapp.h>
#include <eikappui.h>
#include <eikenv.h>


class MCAConversationMessage;

// SAP setting store keys for IM settings
_LIT( KIMOwnMessageColourKey, "IMOwnMessageColour" );
_LIT( KIMOthersMessageColourKey, "IMOthersMessageColour" );
_LIT( KIMAlertTonePath, "IMAlertTone" );

#ifdef __WINS__
const TInt KIMBearerTypes = EApBearerTypeCSD | EApBearerTypeHSCSD | EApBearerTypeGPRS;
#else
const TInt KIMBearerTypes = EApBearerTypeGPRS;
#endif

const TInt KServerWVUserIdMaxLength = 50;

const TInt KScreenNameMaxLength = 50;
const TInt KNicknameMaxLength = 50;

// maximum length in chars in query dialog
const TInt KScreenNameQueryMaxLength = 10;

// maximum length in chars for alias
const TInt KAliasMaxLength = 50;

// default maximum length in chars for message
const TInt KDefaultMsgLength = 400;

// Contact size estimate in bytes for low disk checking,
// estimate based on member variables of CCAContact
const TInt KContactSizeEstimate = 210;

_LIT( KEmptyDesC, " " );
_LIT( KTab, "\t" );
_LIT( KEmptyCharacters, " \t" );

const TInt KFriendsListMaxIdentificationLength = 50;

// chat list icons
_LIT( KUserCreatedActiveTab,            "0\t" );
_LIT( KUserCreatedInActiveTab,          "1\t" );
_LIT( KNonUserCreatedActiveTab,         "2\t" );
_LIT( KNonUserCreatedInActiveTab,       "3\t" );
_LIT( KJoinedNotSavedTab,               "4\t" );
#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
_LIT( KUserCreatedActiveUnreadTab,      "5\t" );
_LIT( KNonUserCreatedActiveUnreadTab,   "6\t" );
_LIT( KJoinedNotSavedUnreadTab,         "7\t" );
#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

//Chat list constants
const TInt KArrayGranularity( 5 );
_LIT( KSpace, " " );
const TInt KMaxWVIDLength( 50 );
const TInt KMaxChatGroupLenght( 50 );
const TInt KMaxTabLength( 3 );

//Chat view
_LIT( KSemicolon, ": " );
_LIT( KIconTabulator, "1\t" );
_LIT( KArabicSign, "\x200f" );
const TInt KChatSmileyLength( 3 );
const TUint KColon( ':' );

_LIT( KIMHTTPPREFIX, "http://" );
_LIT( KIMRTSP, "rtsp" );

//Used In Chat View
//Length of "http"
const TInt KHttpLength( 4 );

//Group properties dialog

_LIT( KAt, "@" );
_LIT( KWV, "wv:" );

//Used in Chat/Conversation Container
const TInt KHashKey = 127;
const TInt KMaxLines = 2;
const TInt KPictureChar = 2;//2 is for space and semicolon
const TInt KControlCount = 2;
//Fadecontlor constants
const TInt KTimerInterval( 100000 );

//Main view
const TInt KMaxListboxTitleLength( 50 );
//Friend list view
const TInt KTextLimit( 40 ); // Text-limit for find-field

//Search
const TInt KMaxSearchStringLength( 50 );
const TInt KMaxFirstNameLength( 50 );
const TInt KMaxSurnameLength( 50 );
const TInt KMaxSearchResults( 10 );
const TInt KSearchLimit( 100 );

//Access array
_LIT( KTabulator, "\t" );

//Chat info array
const TInt KMaxLengthOfHBufC( 100 );

//Editor
const TInt KTwo( 2 );
const TInt KBackspaceFix ( 1 );
const TInt KInsertSmileyFix( 2 );
const TInt KNewLineFix( 3 );
const TInt KRemoveSmileyFix( 4 );

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;

//Tab format for listbox item with an icon
_LIT( KTabFormat, "%d\t" );
const TInt KTabFormatLenght = 4;

enum TItem
    {
    EConversationItem,
    EInviteItem,
    EGroupItem
    };

//View indexes
enum TViewIndexes
    {
    EConvViewIndex = 0,
    EFriendsViewIndex,
    EChatRoomViewIndex,
    ERecordedChatListViewIndex,
    ERecordedChatViewIndex
    };
//Appui constants
const TInt KMaxResourcePathLength = 256;
_LIT( KLineFeed, "\n" );

_LIT( KPanicText, "WVUI" );
enum TCAPanics
    {
    EChatListArrayIndexOutOfBounds,
    EPrivateChatListArrayIndexOutOfBounds,
    EAccessArrayOutOfBounds,
    EChatInfoArrayNotExists,
    EChatInfoArrayIndexOutOfBounds,
    EMainViewListBoxNotExists,
    EMainViewListArrayNotExists,
    EMainViewListArrayIndexOutOfBounds,
    EIconDoesNotExist,
    EChatColourSettingItemWronglyConstructed
    };

// Invite message max-length
const TInt KInviteMessageMaxLength = 50;


//DNL support
//Gorupchat
struct TCADnlChatView
    {
    // WV specification states that GroupID can not be longer than 50
    // characters, but since there are servers which are not implemented
    // that way, we reserve twice the amount of space.
    TBuf<2 * KMaxWVIDLength> iGroupId;
    TBuf<KMaxChatGroupLenght> iGroupName;
    TBool iIsForwarded;      // ETrue if the following pointer contains forwarded message
    const MCAConversationMessage* iForwardedMessage; // pointer to forwarded message
    TBool iInviteQuery;      // ETrue if the invite query should be launched
    TBool iSwitchTab;
    TInt iTabId;
    };

typedef TPckgBuf<TCADnlChatView> TCADnlChatViewBuf;

//conversations
struct TCADnlConvView
    {
    TBuf<KMaxWVIDLength> iWVID;
    TBuf<KMaxWVIDLength> iListID;
    TBool iIsForwarded;      // ETrue if the following pointer contains forwarded message
    const MCAConversationMessage* iForwardedMessage; // pointer to forwarded message
    TBool iSAPChanged;   // ETrue if SAP changed in opened conversation
    TBool iSwitchTab;
    TInt iTabId;
    };

typedef TPckgBuf<TCADnlConvView> TCADnlConvViewBuf;

//Invitation
const TInt KMaxIDNumberLength = 5; //also available in CCAInvitemanager
//make sure both are synchronous
struct TCADnlInvView
    {
    TInt iInvitationIndex;
    TBuf < KMaxWVIDLength + KMaxIDNumberLength > iInviteID;
    TBool iSwitchTab;
    TInt iTabId;
    };


typedef TPckgBuf<TCADnlInvView> TCADnlInvViewBuf;

struct SCADnlSearchView
    {
    TBuf< KMaxWVIDLength > iListId;
    };

typedef TPckgBuf< SCADnlSearchView > TCADnlSearchViewBuf;


//conversations
struct TCADnlConvInvGroupFocus
    {
    TBuf<2 * KMaxWVIDLength> iInvConvGroupID;
    TBuf<KMaxWVIDLength> iListID;
    TItem iType;
    };
typedef TPckgBuf< TCADnlConvInvGroupFocus > TCADnlConvInvGroupFocusBuf;
#endif // CHATDEFINITIONS_H
