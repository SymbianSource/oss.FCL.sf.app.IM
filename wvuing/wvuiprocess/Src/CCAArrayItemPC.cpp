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
* Description:  CCAContactItemPC, CCAContactListItemPC, CCAOwnDataItemPC,
				 CCAConversationItemPC:  Method implementation
*
*/


#include "CCAArrayItemPC.h"

// -----------------------------------------------------------------------------
// CCAContactItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAContactItemPC* CCAContactItemPC::NewL()
    {
    CCAContactItemPC* self = new( ELeave ) CCAContactItemPC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAContactItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAContactItemPC::ConstructL()
    {
    iItemNameTextContact = HBufC::NewL( KFriendsListMaxIdentificationLength );
    }

// -----------------------------------------------------------------------------
// CCAContactItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAContactItemPC::CCAContactItemPC()
    {

    }

// -----------------------------------------------------------------------------
// CCAContactItemPC::c++ destructor
// -----------------------------------------------------------------------------
//
CCAContactItemPC::~CCAContactItemPC()
    {
    delete iItemNameTextContact;
    iItemNameTextContact = NULL;
    }


// -----------------------------------------------------------------------------
//CCAContactItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAContactItemPC::Type()
    {
    return TEnumsPC::EContactItem;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: GetOnlineStatus
// -----------------------------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCAContactItemPC::GetOnlineStatus()
    {
    return iOnlineStatus;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: SetOnlineStatus
// -----------------------------------------------------------------------------
//
void CCAContactItemPC::SetOnlineStatus( TEnumsPC::TOnlineStatus aOnlineStatus )
    {
    iOnlineStatus = aOnlineStatus;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: IsBlocked
// -----------------------------------------------------------------------------
//
TBool CCAContactItemPC::IsBlocked()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: IsMsgPending
// -----------------------------------------------------------------------------
//
TBool CCAContactItemPC::IsMsgPending()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: SetBlockedStatus
// -----------------------------------------------------------------------------
//
void CCAContactItemPC::SetBlockedStatus( TBool aBlocked )
    {
    iBlockedStatus = aBlocked;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: SetMsgPending
// -----------------------------------------------------------------------------
//
void CCAContactItemPC::SetMsgPending( TBool aMsgPending )
    {
    iUnreadmsgFlag = aMsgPending;
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAContactItemPC::GetItemNameText()
    {
    return iItemNameTextContact->Des();
    }

// -----------------------------------------------------------------------------
//CCAContactItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAContactItemPC::SetItemNameText( const TDesC& aItemName )
    {
    *iItemNameTextContact = aItemName;
    }


// -----------------------------------------------------------------------------
// CCAContactListItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAContactListItemPC* CCAContactListItemPC::NewL()
    {
    CCAContactListItemPC* self = new( ELeave ) CCAContactListItemPC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();//Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAContactListItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAContactListItemPC::ConstructL()
    {
    iItemNameTextList = HBufC::NewL( KFriendsListMaxIdentificationLength );
    }
// -----------------------------------------------------------------------------
// CCAContactListItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAContactListItemPC::CCAContactListItemPC()
    {

    }
// -----------------------------------------------------------------------------
// CCAContactListItemPC::~CCAContactListItemPC destructor
// -----------------------------------------------------------------------------
//
CCAContactListItemPC::~CCAContactListItemPC()
    {
    delete iItemNameTextList;
    iItemNameTextList = NULL;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAContactListItemPC::Type()
    {
    return TEnumsPC::EContactListItem;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: IsCollapsed
// -----------------------------------------------------------------------------
//
TBool CCAContactListItemPC::IsCollapsed()
    {
    return iCollapsedStatus;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: IsMsgPending
// -----------------------------------------------------------------------------
//
TBool CCAContactListItemPC::IsMsgPending()
    {
    return iUnreadmsgFlag;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: SetCollapsed
// -----------------------------------------------------------------------------
//
void CCAContactListItemPC::SetCollapsed( TBool aCollapsedStatus )
    {
    iCollapsedStatus = aCollapsedStatus;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: SetMsgPending
// -----------------------------------------------------------------------------
//
void CCAContactListItemPC::SetMsgPending( TBool aUnreadmsgFlag )
    {
    iUnreadmsgFlag = aUnreadmsgFlag;
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAContactListItemPC::GetItemNameText()
    {
    return iItemNameTextList->Des();
    }

// -----------------------------------------------------------------------------
//CCAContactListItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAContactListItemPC::SetItemNameText( const TDesC& aItemName )
    {
    *iItemNameTextList = aItemName;
    }

// -----------------------------------------------------------------------------
// CCAConversationItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAConversationItemPC* CCAConversationItemPC::NewL( const TDesC& aUserId )
    {
    CCAConversationItemPC* self = new( ELeave ) CCAConversationItemPC( );
    CleanupStack::PushL( self );
    self->ConstructL( aUserId );
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAConversationItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAConversationItemPC::ConstructL( const TDesC& aUserId )
    {
    iItemNameTextConv = HBufC::NewL( KFriendsListMaxIdentificationLength );
    iUserId = aUserId.AllocL();
    }

// -----------------------------------------------------------------------------
// CCAConversationItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAConversationItemPC::CCAConversationItemPC( )
    {

    }

// -----------------------------------------------------------------------------
// CCAConversationItemPC::c++ destructor
// -----------------------------------------------------------------------------
//
CCAConversationItemPC::~CCAConversationItemPC()
    {
    delete iUserId;
    delete iItemNameTextConv;
    iItemNameTextConv = NULL;
    }


// -----------------------------------------------------------------------------
//CCAConversationItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAConversationItemPC::Type()
    {
    return TEnumsPC::EConversationItem;
    }

// -----------------------------------------------------------------------------
//CCAConversationItemPC: IsMsgPending
// -----------------------------------------------------------------------------
//
TBool CCAConversationItemPC::IsMsgPending()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
//CCAConversationItemPC: SetMsgPending
// -----------------------------------------------------------------------------
//
void CCAConversationItemPC::SetMsgPending( TBool aMsgPending )
    {
    iUnreadmsgFlag = aMsgPending;
    }

// -----------------------------------------------------------------------------
//CCAConversationItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAConversationItemPC::GetItemNameText()
    {
    return iItemNameTextConv->Des();
    }

// -----------------------------------------------------------------------------
//CCAConversationItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAConversationItemPC::SetItemNameText( const TDesC& aItemName )
    {
    *iItemNameTextConv = aItemName;
    }


// -----------------------------------------------------------------------------
//CCAConversationItemPC: UserId
// -----------------------------------------------------------------------------
//
const TDesC& CCAConversationItemPC::UserId() const
    {
    return *iUserId;
    }

// -----------------------------------------------------------------------------
//CCAConversationItemPC: Identification
// -----------------------------------------------------------------------------
//
const TDesC& CCAConversationItemPC::Identification() const
    {
    return *( iItemNameTextConv );
    }

// -----------------------------------------------------------------------------
// CCAInviteItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAInviteItemPC* CCAInviteItemPC::NewL( const TDesC& aUserId, const TDesC& aInviteId )
    {
    CCAInviteItemPC* self = new( ELeave ) CCAInviteItemPC( aUserId, aInviteId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAInviteItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAInviteItemPC::ConstructL()
    {
    iItemNameTextInvite = HBufC::NewL( KFriendsListMaxIdentificationLength );
    }

// -----------------------------------------------------------------------------
// CCAInviteItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAInviteItemPC::CCAInviteItemPC( const TDesC& aUserId, const TDesC& aInviteId )
        : iUserId( aUserId ),
        iInviteID( aInviteId )
    {

    }

// -----------------------------------------------------------------------------
// CCAInviteItemPC::c++ destructor
// -----------------------------------------------------------------------------
//
CCAInviteItemPC::~CCAInviteItemPC()
    {
    delete iItemNameTextInvite;
    iItemNameTextInvite = NULL;
    }


// -----------------------------------------------------------------------------
//CCAInviteItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAInviteItemPC::Type()
    {
    return TEnumsPC::EInviteItem;
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAInviteItemPC::GetItemNameText()
    {
    return iItemNameTextInvite->Des();
    }
// -----------------------------------------------------------------------------
//CCAInviteItemPC: GetInvitation
// -----------------------------------------------------------------------------
//
MCAInvitation* CCAInviteItemPC::GetInvitation()
    {
    return const_cast<MCAInvitation *>( iInvitation );
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: GetInvitation
// -----------------------------------------------------------------------------
//
void CCAInviteItemPC::SetInvitation( const MCAInvitation* aInvitation )
    {
    iInvitation = aInvitation;
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAInviteItemPC::SetItemNameText( const TDesC& aItemName )
    {
    *iItemNameTextInvite = aItemName;
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: UserId
// -----------------------------------------------------------------------------
//
const TDesC& CCAInviteItemPC::UserId() const
    {
    return iUserId;
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: GetInviteID
// -----------------------------------------------------------------------------
//
const TDesC& CCAInviteItemPC::GetInviteID() const
    {
    return iInviteID;
    }

// -----------------------------------------------------------------------------
//CCAInviteItemPC: Identification
// -----------------------------------------------------------------------------
//
const TDesC& CCAInviteItemPC::Identification() const
    {
    return *( iItemNameTextInvite );
    }
// -----------------------------------------------------------------------------
// CCAContactListItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAOpenChatsListItemPC* CCAOpenChatsListItemPC::NewL()
    {
    CCAOpenChatsListItemPC* self = new( ELeave ) CCAOpenChatsListItemPC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAContactListItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAOpenChatsListItemPC::ConstructL()
    {
    iItemNameTextChat = HBufC::NewL( KFriendsListMaxIdentificationLength );
    }

// -----------------------------------------------------------------------------
// CCAContactListItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAOpenChatsListItemPC::CCAOpenChatsListItemPC()
    {

    }
// -----------------------------------------------------------------------------
// CCAOpenChatsListItemPC::CCAOpenChatsListItemPC destructor
// -----------------------------------------------------------------------------
//
CCAOpenChatsListItemPC::~CCAOpenChatsListItemPC()
    {
    delete iItemNameTextChat;
    iItemNameTextChat = NULL;
    }

// -----------------------------------------------------------------------------
//CCAOpenChatsListItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAOpenChatsListItemPC::Type()
    {
    return TEnumsPC::EOpenChatsListItem;
    }

// -----------------------------------------------------------------------------
//CCAOpenChatsListItemPC: IsCollapsed
// -----------------------------------------------------------------------------
//
TBool CCAOpenChatsListItemPC::IsCollapsed()
    {
    return iCollapsedStatus;
    }

// -----------------------------------------------------------------------------
//CCAOpenChatsListItemPC: SetCollapsed
// -----------------------------------------------------------------------------
//
void CCAOpenChatsListItemPC::SetCollapsed( TBool aCollapsedStatus )
    {
    iCollapsedStatus = aCollapsedStatus;
    }

// -----------------------------------------------------------------------------
//CCAOpenChatsListItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAOpenChatsListItemPC::GetItemNameText()
    {
    return iItemNameTextChat->Des();
    }

// -----------------------------------------------------------------------------
//CCAOpenChatsListItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAOpenChatsListItemPC::SetItemNameText( const TDesC& aItemName )
    {
    delete iItemNameTextChat;
    iItemNameTextChat = NULL;
    TRAP_IGNORE( iItemNameTextChat = HBufC::NewL( KFriendsListMaxIdentificationLength ) );
    *iItemNameTextChat = aItemName;
    }

// -----------------------------------------------------------------------------
// CCAOwnDataItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAOwnDataItemPC* CCAOwnDataItemPC::NewL()
    {
    CCAOwnDataItemPC* self = new( ELeave ) CCAOwnDataItemPC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAOwnDataItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAOwnDataItemPC::ConstructL()
    {
    iItemNameTextOwnData = HBufC::NewL( KFriendsListMaxIdentificationLength );
    }

// -----------------------------------------------------------------------------
// CCAOwnDataItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAOwnDataItemPC::CCAOwnDataItemPC()
    {

    }
// -----------------------------------------------------------------------------
// CCAOwnDataItemPC::c++ default destructor
// -----------------------------------------------------------------------------
//
CCAOwnDataItemPC::~CCAOwnDataItemPC()
    {
    delete iItemNameTextOwnData;
    iItemNameTextOwnData = NULL;
    }

// -----------------------------------------------------------------------------
//CCAOwnDataItemPC: Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAOwnDataItemPC::Type()
    {
    return TEnumsPC::EOwnStatusItem;
    }

// -----------------------------------------------------------------------------
//CCAOwnDataItemPC: GetOwnStatus
// -----------------------------------------------------------------------------
//
TEnumsPC::TOnlineStatus CCAOwnDataItemPC::GetOwnStatus()
    {
    return iOwnStatus;
    }

// -----------------------------------------------------------------------------
//CCAOwnDataItemPC: SetOwnStatus
// -----------------------------------------------------------------------------
//
void CCAOwnDataItemPC::SetOwnStatus( TEnumsPC::TOnlineStatus aStatus )
    {
    iOwnStatus = aStatus;
    }

// -----------------------------------------------------------------------------
//CCAOwnDataItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAOwnDataItemPC::GetItemNameText()
    {
    return iItemNameTextOwnData->Des();
    }

// -----------------------------------------------------------------------------
//CCAOwnDataItemPC: SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAOwnDataItemPC::SetItemNameText( const TDesC& aItemName )
    {
    *iItemNameTextOwnData = aItemName;
    }


// -----------------------------------------------------------------------------
// CCAGroupItemPC::NewL
// -----------------------------------------------------------------------------
//
CCAGroupItemPC* CCAGroupItemPC::NewL( const TDesC& aGroupId )
    {
    CCAGroupItemPC* self = new( ELeave ) CCAGroupItemPC();
    CleanupStack::PushL( self );
    self->ConstructL( aGroupId );
    CleanupStack::Pop(); //Codescanner warning ignored
    return self;
    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCAGroupItemPC::ConstructL( const TDesC& aGroupId )
    {
    iItemNameTextGroup = HBufC::NewL( KFriendsListMaxIdentificationLength );
    iGroupId = aGroupId.AllocL();
    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::c++ default constructor
// -----------------------------------------------------------------------------
//
CCAGroupItemPC::CCAGroupItemPC()
    {

    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::c++ destructor
// -----------------------------------------------------------------------------
//
CCAGroupItemPC::~CCAGroupItemPC()
    {
    delete iGroupId;
    delete iItemNameTextGroup;
    iItemNameTextGroup = NULL;
    }


// -----------------------------------------------------------------------------
//CCAGroupItemPC: GroupId
// -----------------------------------------------------------------------------
//
const TDesC& CCAGroupItemPC::GroupId() const
    {
    return *iGroupId;
    }

// -----------------------------------------------------------------------------
//CCAGroupItemPC: Identification
// -----------------------------------------------------------------------------
//
const TDesC& CCAGroupItemPC::Identification() const
    {
    return *( iItemNameTextGroup );
    }
// -----------------------------------------------------------------------------
// CCAGroupItemPC::Type
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAGroupItemPC::Type()
    {
    return TEnumsPC::EGroupItem;
    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::IsMsgPending
// -----------------------------------------------------------------------------
//
TBool CCAGroupItemPC::IsMsgPending()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::SetMsgPending
// -----------------------------------------------------------------------------
//
void CCAGroupItemPC::SetMsgPending( TBool aMsgPending )
    {
    iUnreadmsgFlag = aMsgPending;
    }

// -----------------------------------------------------------------------------
// CCAConversationItemPC: GetItemNameText
// -----------------------------------------------------------------------------
//
TPtrC CCAGroupItemPC::GetItemNameText()
    {
    return iItemNameTextGroup->Des();
    }

// -----------------------------------------------------------------------------
// CCAGroupItemPC::SetItemNameText
// -----------------------------------------------------------------------------
//
void CCAGroupItemPC::SetItemNameText( const TDesC& aItemName )
    {
    delete iItemNameTextGroup;
    iItemNameTextGroup = NULL;
    TRAP_IGNORE( iItemNameTextGroup = HBufC::NewL( KFriendsListMaxIdentificationLength ) );
    *iItemNameTextGroup = aItemName;
    }

