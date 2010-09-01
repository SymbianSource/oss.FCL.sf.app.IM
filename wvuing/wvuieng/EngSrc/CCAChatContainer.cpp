/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for message containers
*
*/


#include "CCAChatContainer.h"
#include "CCAMessageContainer.h"
#include "PublicEngineDefinitions.h"
#include "MCAChatObserver.h"
#include "MCASettings.h"
#include "mcamessage.h"

#include "ChatDebugAssert.h"
#include "ChatDebugPrint.h"
#include <hal.h>

const TInt KMinAvailableMemory = 0x32000; // 200kb

//-----------------------------------------------------------------------------
// CCAChatContainer::CCAChatContainer
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAChatContainer::CCAChatContainer( MCASettings& aSettingsInterface )
        : iSettingsInterface( aSettingsInterface )
    {
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::CCAChatContainer
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAChatContainer::~CCAChatContainer()
    {
    iChatContainer.ResetAndDestroy();
    iGroupContainer.ResetAndDestroy();
    iSendContainer.ResetAndDestroy();
    iObservers.Reset();
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAChatContainer* CCAChatContainer::NewL( MCASettings& aSettingsInterface )
    {
    CCAChatContainer* self =
        new ( ELeave ) CCAChatContainer( aSettingsInterface );

    return self;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::MessageReadInterfaceL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface& CCAChatContainer::MessageReadInterfaceL(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCAMessagesReadInterface::TContainerType aType )
    {
    return *ChatL( ChatL( aServerAddress, aUserId, aTargetId, aType ) );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::MessageWriteInterfaceL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesWriteInterface& CCAChatContainer::MessageWriteInterfaceL(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCAMessagesReadInterface::TContainerType aType )
    {
    return *ChatL( ChatL( aServerAddress, aUserId, aTargetId, aType ) );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::DeleteChatL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::DeleteChatL( const TDesC& aServerAddress,
                                    const TDesC& aUserId,
                                    const TDesC& aTargetId )
    {
    // Find out if chat exist or not. ChatExists() not used,
    // because we need index.
    CCAMessageContainer* tempChat = CreateContainerLC(
                                        aServerAddress,
                                        aUserId,
                                        aTargetId,
                                        *this,
                                        MCAMessagesReadInterface::ENoneContainer );
    TInt index( FindChat( *tempChat ) );
    CleanupStack::PopAndDestroy( tempChat );

    // If index is positive  value, Chat exists and we can remove it.
    if ( index >= 0 )
        {
        CCAMessageContainer* target = ChatL( index, EFalse );

        // Get count of pending messages before removing chat
        TInt ignore;
        TInt pendignMsgs = MessagesPendingCount( ignore );

        RemoveChat( index );

        // Check if removed chat had unread messages
        if ( pendignMsgs != MessagesPendingCount( ignore ) )
            {
            // Notify about changed unread count
            HandleChatEvent( EUnreadCountChanged );
            }

        // Observers are notified, delete target.
        delete target;
        }
    else if ( index != KErrNotFound )
        {
        // some other error than not found occurred. Inform caller.
        User::Leave( index );
        }
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::CloseAllContainers
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::CloseAllContainers()
    {
    iChatContainer.ResetAndDestroy();
    HandleChatEvent( EChatListChanged );
    iGroupContainer.ResetAndDestroy();
    HandleChatEvent( EGroupListChanged );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChatExistsL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface* CCAChatContainer::ChatExistsL(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId )
    {
    CCAMessageContainer* tempChat = CreateContainerLC(
                                        aServerAddress,
                                        aUserId,
                                        aTargetId,
                                        *this,
                                        MCAMessagesReadInterface::ENoneContainer );

    TInt index( FindChat( *tempChat ) );
    CleanupStack::PopAndDestroy( tempChat );
    if ( index < 0 )
        {
        return NULL;
        }
    return ChatL( index );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChangeChatIdL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface& CCAChatContainer::ChangeChatIdL(
    const TDesC& aOldServerAddress,
    const TDesC& aOldUserId,
    const TDesC& aOldTargetId,
    const TDesC& aNewServerAddress,
    const TDesC& aNewUserId,
    const TDesC& aNewTargetId,
    MCAMessagesReadInterface::TContainerType aType )
    {
    // If old does not exists. ChatL creates it.
    TInt index = ChatL(  aOldServerAddress, aOldUserId, aOldTargetId  );
    CCAMessageContainer* target = ChatL( index, EFalse );

    MCAMessagesReadInterface::TContainerType oldType = target->ContainerType();

    // Update chat to new type.
    target->ChangeIdL(  aNewServerAddress, aNewUserId, aNewTargetId, aType  );

    MCAMessagesReadInterface::TContainerType newType = target->ContainerType();

    // If type is changed, container must be changed.
    if ( oldType != newType )
        {
        // Remove chat from current container.
        RemoveChat( index );
        CleanupStack::PushL( target );

        // Append to right container.
        TLinearOrder< CCAMessageContainer > order(
            CCAMessageContainer::OrderUid );
        switch ( newType )
            {
            case MCAMessagesReadInterface::EGroupContainer:
                {
                // User::LeaveIfError returns value if value is positive.
                index = User::LeaveIfError( iGroupContainer.InsertInOrder(
                                                target, order ) );
                HandleChatEvent( EGroupListChanged );
                break;
                }
            case MCAMessagesReadInterface::EChatContainer:
                {
                index = User::LeaveIfError( iChatContainer.InsertInOrder(
                                                target, order ) );
                HandleChatEvent( EChatListChanged );
                break;
                }
            case MCAMessagesReadInterface::ESendContainer:
                {
                index = User::LeaveIfError(
                            iSendContainer.InsertInOrder( target, order ) );
                break;
                }
            default:
                {
                User::Leave( KErrArgument );
                break;
                }
            }
        CleanupStack::Pop( target );
        }
    else
        {
        switch ( newType )
            {
            case MCAMessagesReadInterface::EGroupContainer:
                {
                HandleChatEvent( EGroupListChanged );
                break;
                }
            case MCAMessagesReadInterface::EChatContainer:
                {
                HandleChatEvent( EChatListChanged );
                break;
                }
            default:
                {
                User::Leave( KErrArgument );
                break;
                }
            }
        }
    iLatestType = newType;
    return *target;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::GroupCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::GroupCount() const
    {
    return iGroupContainer.Count();
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChatCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::ChatCount() const
    {
    return iChatContainer.Count();
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::GroupAt
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface& CCAChatContainer::GroupAt( TInt aIndex ) const
    {
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < GroupCount() );
    return *iGroupContainer[ aIndex ];
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChatAt
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface& CCAChatContainer::ChatAt( TInt aIndex ) const
    {
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < ChatCount() )
    return *iChatContainer[ aIndex ];
    }


//-----------------------------------------------------------------------------
// CCAChatContainer::RegisterChatObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::RegisterChatObserver( MCAChatObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if ( index == KErrNotFound )
        {
        return iObservers.Append( aObserver );
        }
    return KErrAlreadyExists;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::RegisterChatObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::UnregisterChatObserver( MCAChatObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index );
        return KErrNone;
        }
    return index;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ResetPendingCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::ResetPendingCount()
    {
    TInt count( ChatCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        ChatAt( a ).Read( MCAMessagesReadInterface::EReadAll );
        }
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::PendingMessagesCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::MessagesPendingCount( TInt& aCountOfChats,
                                             MCAMessagesReadInterface::TUnreadFilter aUnreadFilter
                                             /*= MCAMessagesReadInterface::EUnreadAll*/ ) const
    {
    TInt count( ChatCount() );
    TInt pendingCount( 0 );
    aCountOfChats = 0;
    for ( TInt a( 0 ); a < count; ++a )
        {
        TInt unreadCount = ChatAt( a ).UnreadCount( aUnreadFilter );
        if ( unreadCount > 0 )
            {
            pendingCount += unreadCount;
            ++aCountOfChats;
            }
        }
    return pendingCount;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::PendingMessagesCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessageContainerInfo* CCAChatContainer::PendingMessageInfo() const
    {
    TInt count( ChatCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        if ( ChatAt( a ).UnreadCount( MCAMessagesReadInterface::EUnreadReceived ) > 0 )
            {
            return iChatContainer[ a ];
            }
        }
    return NULL;
    }

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
//-----------------------------------------------------------------------------
// CCAChatContainer::ChatGroupMessagesPendingCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::ChatGroupMessagesPendingCount( TInt& aCountOfChats ) const
    {
    TInt count = GroupCount();
    TInt pendingCount = 0;
    aCountOfChats = 0;

    for ( TInt i = 0; i < count; ++i )
        {
        TInt unreadCount = GroupAt( i ).UnreadCount(
                               MCAMessagesReadInterface::EUnreadReceived );
        if ( unreadCount > 0 )
            {
            pendingCount += unreadCount;
            ++aCountOfChats;
            }
        }

    return pendingCount;
    }


//-----------------------------------------------------------------------------
// CCAChatContainer::ChatGroupPendingMessageInfo
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessageContainerInfo* CCAChatContainer::ChatGroupPendingMessageInfo() const
    {
    TInt count( GroupCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        if ( GroupAt( a ).UnreadCount( MCAMessagesReadInterface::EUnreadReceived ) > 0 )
            {
            return iGroupContainer[ a ];
            }
        }
    return NULL;
    }

#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

//-----------------------------------------------------------------------------
// CCAChatContainer::MessageInterface
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAChatContainer::FreeMemoryIfNeededL( TInt aSize )
    {
    CHAT_DP_FUNC_ENTER( "CCAChatContainer::FreeMemoryIfNeededL" );

    TInt halSize;
    TInt biggestBlock;

    HAL::Get( HALData::EMemoryRAMFree, halSize );
    TInt maxHeap = Min( User::Heap().MaxLength(), halSize );
    TInt available = maxHeap - ( User::Heap().Size() - User::Heap().Available( biggestBlock ) );
    TInt memoryNeed = KMinAvailableMemory + aSize;

    TTime oldestUnread;
    TInt unreadIndex;

    CHAT_DP( D_CHAT_LIT( "CCAChatContainer::FreeMemoryIfNeededL, heapSize = %d" ), maxHeap );
    CHAT_DP( D_CHAT_LIT( "****heapSize, available, need = %d, %d, %d" ),
             maxHeap, available, memoryNeed );

    TBool ready = EFalse;
    TBool memLowNotified = EFalse;
    while ( !ready )
        {
        TBool freeMemoryNeeded( EFalse );
        if ( iExternalMemoryHandler )
            {
            freeMemoryNeeded = !iExternalMemoryHandler->FreeMemoryIfNeededL( aSize );
            }
        if ( freeMemoryNeeded || memoryNeed > available )
            {
            // we have to react.
            TInt count = GroupCount();

            // Check how much memory can be made free
            TInt freeableBytes = 0;
            for ( TInt i = 0; i < count; ++i )
                {
                TBool locked = iGroupContainer[i]->IsLocked();
                if ( iGroupContainer[i]->AllMessagesCount() && !locked )
                    {
                    freeableBytes += iGroupContainer[i]->ContainerSizeInBytes();
                    }
                }

            if ( ( ( available + freeableBytes ) < memoryNeed )
                 && !memLowNotified )
                {
                // Can't free enough memory
                return EFalse;
                }

            oldestUnread.HomeTime();
            unreadIndex = KErrNotFound;

            for ( TInt a( 0 ); a < count; ++a )
                {
                MCAMessagesReadInterface& chat = GroupAt( a );
                TBool locked = iGroupContainer[ a ]->IsLocked();
                if ( iGroupContainer[ a ]->AllMessagesCount() && !locked )
                    {
                    MCAMessage& message = iGroupContainer[ a ]->MessageFromAll( 0 );
                    if ( oldestUnread > message.TimeStamp() )
                        {
                        oldestUnread = message.TimeStamp();
                        unreadIndex = a;
                        }
                    }
                }

            if ( unreadIndex == KErrNotFound )
                {
                return EFalse;
                }
            else
                {
                // Inform observers about memory handling
                if ( !memLowNotified )
                    {
                    HandleChatEvent( EMemoryLow );
                    memLowNotified = ETrue;
                    }
                iGroupContainer[ unreadIndex ]->DeleteMessageFromAll( 0 );
                }
            available = maxHeap - ( User::Heap().Size() - User::Heap().Available( biggestBlock ) );
            CHAT_DP( D_CHAT_LIT( "****heapSize, available, need = %d, %d, %d" ),
                     maxHeap, available, memoryNeed );
            }
        else
            {
            ready = ETrue;
            }
        }
    CHAT_DP_FUNC_DONE( "CCAChatContainer::FreeMemoryIfNeededL" );
    return ETrue;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::SetAccessInterface
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::SetAccessInterface( MCAChatInterface* /*aAccess*/ )
    {
    // Not need to use in here. We have access already.
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::HandleChatEvent
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::HandleChatEvent( TChatEventType aEvent,
                                        MCAMessage* aMessage )
    {
    // Inform all observer about chat event.
    TInt count( iObservers.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        iObservers[ a ]->HandleChatEvent( aEvent, aMessage );
        }
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::MessageRecipient
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAChatContainer::MessageRecipient( TInt aOperationCode,
                                                 TInt& aStatus ) const
    {
    TInt count( ChatCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        const TDesC& res = iChatContainer[ a ]->MessageRecipient(
                               aOperationCode, aStatus );
        if ( aStatus == KErrNone )
            {
            return res;
            }
        }

    count = GroupCount();
    for ( TInt a( 0 ); a < count; ++a )
        {
        const TDesC& res = iGroupContainer[ a ]->MessageRecipient(
                               aOperationCode, aStatus );
        if ( aStatus == KErrNone )
            {
            return res;
            }
        }

    count = iSendContainer.Count();
    for ( TInt a( 0 ); a < count; ++a )
        {
        const TDesC& res = iSendContainer[ a ]->MessageRecipient(
                               aOperationCode, aStatus );
        if ( aStatus == KErrNone )
            {
            return res;
            }
        }

    aStatus = KErrNotFound;
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CCAChatContainer::SetExternalMemoryHandler
// ( Other items commented in header )
// -----------------------------------------------------------------------------
void CCAChatContainer::SetExternalMemoryHandler(
    MCABufferMemoryHandler* aMemoryHandler )
    {
    iExternalMemoryHandler = aMemoryHandler;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChatL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::ChatL(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCAMessagesReadInterface::TContainerType aType
    /*= MCAMessagesReadInterface::ENoneContainer*/ )
    {
    CCAMessageContainer* tempChat =
        CreateContainerLC( aServerAddress, aUserId, aTargetId, *this, aType );

    TInt index( FindChat( *tempChat ) );
    if ( index == KErrNotFound )
        {
        TLinearOrder< CCAMessageContainer > order(
            CCAMessageContainer::OrderUid );
        // User::LeaveIfError returns value if value is positive.
        iLatestType = tempChat->ContainerType();
        switch ( iLatestType )
            {
            case MCAMessagesReadInterface::EGroupContainer:
                {
                User::LeaveIfError( iGroupContainer.InsertInOrder( tempChat,
                                                                   order ) );
                index = FindChat( *tempChat );
                iGroupContainer[ index ]->SetChatObserver( this );
                HandleChatEvent( EGroupListChanged );
                break;
                }
            case MCAMessagesReadInterface::EChatContainer:
                {
                User::LeaveIfError( iChatContainer.InsertInOrder( tempChat,
                                                                  order ) );
                index = FindChat( *tempChat );
                iChatContainer[ index ]->SetChatObserver( this );
                HandleChatEvent( EChatListChanged );
                break;
                }
            case MCAMessagesReadInterface::ESendContainer:
                {
                User::LeaveIfError( iSendContainer.InsertInOrder( tempChat,
                                                                  order ) );
                index = FindChat( *tempChat );
                break;
                }
            default:
                {
                User::Leave( KErrArgument );
                break;
                }
            }
        CleanupStack::Pop( tempChat );
        }
    else
        {
        CleanupStack::PopAndDestroy( tempChat );
        }

    return User::LeaveIfError( index );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::ChatL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer* CCAChatContainer::ChatL( TInt aIndex,
                                              TBool aResetLatest /* = ETrue*/ )
    {
    MCAMessagesReadInterface::TContainerType type = iLatestType;

    if ( aResetLatest )
        {
        iLatestType = MCAMessagesReadInterface::ENoneContainer;
        }

    switch ( type )
        {
        case MCAMessagesReadInterface::EGroupContainer:
            {
            return iGroupContainer[ aIndex ];
            }
        case MCAMessagesReadInterface::EChatContainer:
            {
            return iChatContainer[ aIndex ];
            }
        case MCAMessagesReadInterface::ESendContainer:
            {
            return iSendContainer[ aIndex ];
            }
        default:
            {
            User::Leave( KErrArgument );
            break;
            }
        }
    // Can never get this far
    __CHAT_ASSERT_DEBUG( EFalse );
    return NULL;
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::RemoveChatL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAChatContainer::RemoveChat( TInt aIndex,
                                   TBool aResetLatest /* = ETrue */ )
    {
    MCAMessagesReadInterface::TContainerType type = iLatestType;

    if ( aResetLatest )
        {
        iLatestType = MCAMessagesReadInterface::ENoneContainer;
        }

    switch ( type )
        {
        case MCAMessagesReadInterface::EGroupContainer:
            {
            iGroupContainer.Remove( aIndex );
            HandleChatEvent( EGroupListChanged );
            break;
            }
        case MCAMessagesReadInterface::EChatContainer:
            {
            iChatContainer.Remove( aIndex );
            HandleChatEvent( EChatListChanged );
            break;
            }
        case MCAMessagesReadInterface::ESendContainer:
            {
            iSendContainer.Remove( aIndex );
            break;
            }
        default:
            {
            break;
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::FindChat
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAChatContainer::FindChat( const CCAMessageContainer& aChat )
    {
    TInt index( 0 );
    TLinearOrder< CCAMessageContainer > order( CCAMessageContainer::OrderUid );
    TInt status = iChatContainer.FindInOrder( &aChat, index, order );
    if ( status == KErrNone )
        {
        iLatestType = MCAMessagesReadInterface::EChatContainer;
        return index;
        }
    status = iGroupContainer.FindInOrder( &aChat, index, order );
    if ( status == KErrNone )
        {
        iLatestType = MCAMessagesReadInterface::EGroupContainer;
        return index;
        }
    iLatestType = MCAMessagesReadInterface::ESendContainer;
    status = iSendContainer.FindInOrder( &aChat, index, order );
    return ( status == KErrNone ? index : status );
    }

//-----------------------------------------------------------------------------
// CCAChatContainer::CreateContainerLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer* CCAChatContainer::CreateContainerLC(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCABufferMemoryHandler& aMemoryHandler,
    MCAMessagesReadInterface::TContainerType aType )
    {
    HBufC* accesspoint = NULL;
    HBufC* userid = NULL;

    if ( aServerAddress == KNullDesC )
        {
        accesspoint = iSettingsInterface.ValueL( MCASettings::EServiceAddress );
        CleanupStack::PushL( accesspoint );
        }
    if ( aUserId == KNullDesC )
        {
        userid = iSettingsInterface.ValueL( MCASettings::EOwnWVUserID );
        CleanupStack::PushL( userid );
        }

    const TDesC& accessDes = ( accesspoint ? *accesspoint : aServerAddress );
    const TDesC& userDes = ( userid ? *userid : aUserId );

    CCAMessageContainer* messageContainer = CCAMessageContainer::NewL(
                                                accessDes,
                                                userDes,
                                                aTargetId,
                                                aMemoryHandler,
                                                aType );
    if ( userid )
        {
        CleanupStack::PopAndDestroy( userid );
        }
    if ( accesspoint )
        {
        CleanupStack::PopAndDestroy( accesspoint );
        }
    CleanupStack::PushL( messageContainer );
    return messageContainer;
    }

// end of file
