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
* Description:  Container class for messages.
*
*/


#include "CCAMessageContainer.h"
#include "PublicEngineDefinitions.h"
#include "CAUtils.h"
#include "MCABufferMemoryHandler.h"
#include "MCAMessageObserver.h"
#include "MCAChatObserver.h"
#include "CCAStorageManagerFactory.h"
#include "MCAStoredGroups.h"
#include "MCAStoredGroup.h"
#include "MCALoggerWriteInterface.h"
#include "ChatDebugPrint.h"
#include "CCALoggerMessageHeader.h"
#include "CALoggerManagerFactory.h"
#include "MCAChatInterface.h"           // KSendBufferId
#include "CCAMessageBase.h"
#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"

#include "impsbuilddefinitions.h"

#include "ChatDebugAssert.h"

#include <escapeutils.h>

_LIT( KGroupIdPart, "/" );

//-----------------------------------------------------------------------------
// CCAMessageContainer::CCAMessageContainer
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer::CCAMessageContainer(
    MCABufferMemoryHandler& aMemoryHandler )
        : iMemoryHandler( aMemoryHandler ), iAddToContactAsked( EFalse )
    {
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::~CCAMessageContainer
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer::~CCAMessageContainer()
    {
    if ( iObserver )
        {
        iObserver->HandleMessageEvent( MCAMessageObserver::EChatDeleted );
        }

    for ( TInt a( 0 ); a < iMessageContainer.Count(); ++a )
        {
        if ( iMessageContainer[ a ]->DecreaseOwnerCount() )
            {
            delete iMessageContainer[ a ];
            }
        }
    iMessageContainer.Reset();

    NotifyContactForPending();

    delete iLogger;
    delete iServerAddress;
    delete iUserId;
    delete iTargetId;
    delete iScreenName;
    delete iDecodedTargetId;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer* CCAMessageContainer::NewL(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCABufferMemoryHandler& aMemoryHandler,
    TContainerType aType )
    {
    CCAMessageContainer* self = CCAMessageContainer::NewLC( aServerAddress,
                                                            aUserId,
                                                            aTargetId,
                                                            aMemoryHandler,
                                                            aType );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::NewLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAMessageContainer* CCAMessageContainer::NewLC(
    const TDesC& aServerAddress,
    const TDesC& aUserId,
    const TDesC& aTargetId,
    MCABufferMemoryHandler& aMemoryHandler,
    TContainerType aType )
    {
    CCAMessageContainer* self =
        new ( ELeave ) CCAMessageContainer( aMemoryHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aServerAddress, aUserId, aTargetId, aType );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::ConstructL( const TDesC& aServerAddress,
                                      const TDesC& aUserId,
                                      const TDesC& aTargetId,
                                      TContainerType aType )
    {
    iServerAddress = aServerAddress.AllocL();
    iUserId = aUserId.AllocL();
    iTargetId = aTargetId.AllocL();
    iDecodedTargetId = EscapeUtils::EscapeDecodeL( *iTargetId );
    iScreenName = KNullDesC().AllocL();

    ResolveType( aType );

    iLogger = CALoggerManagerFactory::WriteInstanceL();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::UnreadCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageContainer::UnreadCount(
    MCAMessagesReadInterface::TUnreadFilter aUnreadFilter ) const
    {
    TInt countOfAll = iMessageContainer.Count();
    TInt unreadCount = countOfAll - iUnreadOffset;
    if ( aUnreadFilter != EUnreadAll )
        {
        for ( TInt a = countOfAll - 1; a >= iUnreadOffset; --a )
            {
            if ( iMessageContainer[ a ]->MessagerType() == MCAMessage::EMessageSent )
                {
                unreadCount--;
                }
            }
        }
    return unreadCount;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::MessageCount
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageContainer::MessageCount() const
    {
    // Offset is index to first unread message. It is exatly same as count of
    // read messages.
    return iUnreadOffset;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::Read
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::Read( TReadType aReadType, TInt aAmount /* = 0 */ )
    {
    TInt messageCount( iMessageContainer.Count() );
    TInt originalUnreadCount = UnreadCount( );

    switch ( aReadType )
        {
        case EReadAll:
            iUnreadOffset = messageCount;
            break;
        case EReadAmount:
            iUnreadOffset += aAmount;
            break;
        case EReadOne:
            ++iUnreadOffset;
            break;
        default:
            __CHAT_ASSERT_DEBUG( EFalse );
            return;
        }

    // Cannot be more than messageCount
    if ( iUnreadOffset > messageCount )
        {
        iUnreadOffset = messageCount;
        }

    // Inform observers only if unreadcount is changed.
    if ( originalUnreadCount != UnreadCount() )
        {
        if ( iChatObserver )
            {
            iChatObserver->HandleChatEvent(
                MCAChatObserver::EUnreadCountChanged );
            }
        }

    NotifyContactForPending();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::Message
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage& CCAMessageContainer::Message( TInt aIndex ) const
    {
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < MessageCount() );
    return *iMessageContainer[ aIndex ];
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ReadNextUnread
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage& CCAMessageContainer::ReadNextUnread()
    {
    __CHAT_ASSERT_DEBUG( UnreadCount() != 0 );
    Read( EReadOne );
    return *iMessageContainer[ MessageCount() - 1 ];
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ReadUnreadFromIndex
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage& CCAMessageContainer::ReadUnreadFromIndex( TInt aIndex )
    {
    __CHAT_ASSERT_DEBUG( UnreadCount() != 0 );
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < iMessageContainer.Count() );
    Read( EReadOne );
    return *iMessageContainer[aIndex];
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::SetObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::SetObserver( MCAMessageObserver* aObserver )
    {
    iObserver = aObserver;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::SetObserver
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TPtrC CCAMessageContainer::Name() const
    {
    // Resolve name
    switch ( iContainerType )
        {
        case EChatContainer:
            {
            return CAUtils::DisplayId( *iTargetId );
            }
        case EGroupContainer:
            {
            MCAStoredGroup* group = NULL;
            TRAPD( err, group =
                       CCAStorageManagerFactory::GroupListInterfaceL()->FindGroup(
                           *iTargetId ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            if ( group )
                {
                return group->GroupName();
                }
            return *iTargetId;
            }
        default:
            {
            __CHAT_ASSERT_DEBUG( EFalse );
            return KNullDesC();
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::DeleteMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::DeleteMessage( TInt aIndex )
    {
    MCAMessage* target = &Message( aIndex );
    iMessageContainer.Remove( aIndex );
    iContainerSizeInBytes -= target->SizeInBytes();
    --iUnreadOffset;

    if ( iObserver )
        {
        iObserver->HandleMessageEvent( MCAMessageObserver::EMessageDeleted,
                                       aIndex );
        }

    if ( target->DecreaseOwnerCount() )
        {
        delete target;
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::AppendL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::AppendL( MCAMessage* aMessage, TBool aSharedOwnership )
    {
    if ( !aMessage )
        {
        User::Leave( KErrArgument );
        }

    if ( !aSharedOwnership )
        {
        CleanupDeletePushL( aMessage );
        }

    // Check memory status
    // if failes, leave with KErrNoMemory
    if ( !iMemoryHandlingLocked )
        {
        if ( !iMemoryHandler.FreeMemoryIfNeededL( aMessage->SizeInBytes() ) )
            {
            User::Leave( KErrNoMemory );
            }
        }

    iMessageContainer.AppendL( aMessage );
    iContainerSizeInBytes += aMessage->SizeInBytes();

    if ( !aSharedOwnership )
        {
        CleanupStack::Pop( aMessage );
        }

    aMessage->IncreaseOwnerCount();

    aMessage->SetContainerInfo( this );

    // Record message if logging is on
    if ( iLogger->IsLoggingOn() )
        {
        iLogger->MakeLogL( aMessage->LoggerMessage() );
        }

    CHAT_DP( D_CHAT_LIT( "Address: %s, UserId: %s, Sender %s, Recipient %s" ),
             &aMessage->ServerAddress(),
             &aMessage->UserId(),
             &aMessage->Sender(),
             &aMessage->Recipient() );

    CCAMessageBase* messageBase = static_cast< CCAMessageBase* >( aMessage );
    messageBase->SetContentProcessObserver( this );

    if ( iObserver )
        {
        iObserver->HandleMessageEvent( MCAMessageObserver::ENewMessage );
        }

    if ( iChatObserver )
        {
        iChatObserver->HandleChatEvent( MCAChatObserver::EUnreadCountChanged,
                                        aMessage );
        }

    NotifyContactForPending();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::SetScreenNameL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::SetScreenNameL( const TDesC& aScreenName )
    {
    HBufC* newSn = aScreenName.AllocL();
    delete iScreenName;
    iScreenName = newSn;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::OwnScreenName
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::OwnScreenName() const
    {
    return ScreenName();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::LockBufferMemoryHandling
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
void CCAMessageContainer::LockBufferMemoryHandling( TBool aLocked )
    {
    iMemoryHandlingLocked = aLocked;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ServerAddress
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
TTime CCAMessageContainer::Time()
    {
    TTime time = Time::NullTTime();
    TInt count = iMessageContainer.Count();
    if ( count == 0 )
        {
        // no messages, return null
        return time;
        }
    return iMessageContainer[count - 1]->TimeStamp();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ServerAddress
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::ServerAddress() const
    {
    if ( !iServerAddress )
        {
        return KNullDesC;
        }
    return *iServerAddress;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::UserId
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::UserId() const
    {
    if ( !iUserId )
        {
        return KNullDesC;
        }
    return *iUserId;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::Target
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::Target() const
    {
    if ( !iTargetId )
        {
        return KNullDesC;
        }
    return *iTargetId;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ScreenName
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::ScreenName() const
    {
    if ( iScreenName )
        {
        return *iScreenName;
        }
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::SetAddToContactAsked
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::SetAddToContactAsked( TBool aIsAsked )
    {
    iAddToContactAsked = aIsAsked;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::IsAddToContactAsked
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAMessageContainer::IsAddToContactAsked() const
    {
    return iAddToContactAsked;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::Target
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::HandleProcessingComplete(
    MCAContentProcessor& /* aProcessor */,
    MCAMessage& aMessage, TInt /* aStatus */ )
    {
    TInt count( iMessageContainer.Count() );
    if ( !iObserver ) // No observer. No need of informing anything.
        {
        return;
        }
    for ( TInt index( count - 1 ); index >= 0; --index )
        {
        if ( iMessageContainer[ index ] == &aMessage )
            {
            iObserver->HandleMessageEvent( MCAMessageObserver::EMessageChanged,
                                           index );
            return;
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::OrderUid
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageContainer::OrderUid( const CCAMessageContainer& aSourceA,
                                    const CCAMessageContainer& aSourceB )
    {
    // First Server
    // Then UserId
    // Last Target


    /*	TInt res = CAUtils::NeutralCompare( aSourceA.ServerAddress(),
                                            aSourceB.ServerAddress() );
    	if( res != 0 )
    	    {
    	    return res;
    	    }
    	res = CAUtils::NeutralCompare( aSourceA.UserId(), aSourceB.UserId() );
    	if( res != 0 )
    	    {
    	    return res;
    	    }*/

    return CAUtils::NeutralCompare( aSourceA.ComparableTarget(),
                                    aSourceB.ComparableTarget() );
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ContainerType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessagesReadInterface::TContainerType CCAMessageContainer::ContainerType() const
    {
    return iContainerType;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ResolveType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::ChangeIdL( const TDesC& aServerAddress,
                                     const TDesC& aUserId,
                                     const TDesC& aTargetId,
                                     TContainerType aType )
    {
    HBufC* newServer = aServerAddress.AllocLC();
    HBufC* newUser = aUserId.AllocLC();
    HBufC* newTarget = aTargetId.AllocLC();
    HBufC* newDecoded = EscapeUtils::EscapeDecodeL( *newTarget );
    CleanupStack::Pop( 3, newServer );

    delete iServerAddress;
    delete iUserId;
    delete iTargetId;
    delete iDecodedTargetId;

    iServerAddress = newServer;
    iUserId = newUser;
    iTargetId = newTarget;
    iDecodedTargetId = newDecoded;

    ResolveType( aType );
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ResolveType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
const TDesC& CCAMessageContainer::MessageRecipient( TInt aOperationCode,
                                                    TInt& aStatus ) const
    {
    TInt index = FindIndex( aOperationCode );
    if ( index >= 0 )
        {
        aStatus = KErrNone;
        return iMessageContainer[ index ]->Recipient();
        }
    aStatus = index;
    return KNullDesC;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ResolveType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::SetChatObserver( MCAChatObserver* aObserver )
    {
    iChatObserver = aObserver;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ResolveType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::ResolveType( TContainerType aType )
    {
    if ( aType != ENoneContainer )
        {
        iContainerType = aType;
        }
    else if ( iTargetId->Compare( KSendBufferId ) == 0
              || iTargetId->Compare( KLoggerBufferId ) == 0 )
        {
        iContainerType = ESendContainer;
        }
    else if ( iTargetId->FindC( KGroupIdPart ) == KErrNotFound )
        {
        iContainerType = EChatContainer;
        }
    else
        {
        iContainerType = EGroupContainer;
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::StartLoggingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::StartLoggingL( const TDesC& aLogFilename )
    {
    CHAT_DP_FUNC_ENTER( "StartLoggingL" );
    __CHAT_ASSERT_DEBUG( iLogger );

    if ( iLogger->IsLoggingOn() )
        {
        User::Leave( KErrInUse );
        }

    TChatType type = ( iContainerType == EGroupContainer )
                     ? EGroupChat : EPTOPChat;
    //Create new headermessage for history file.
    CCALoggerMessageHeader* file =
        CCALoggerMessageHeader::NewL(	aLogFilename,
                                      Name(),
                                      type );
    //Start logging. The ownership of the file is transferred
    iLogger->StartLoggingL( file );

    // Flush already got messages to logfile.
    if ( iLogger->IsLoggingOn() )
        {
        // Write the contents of message buffer to log
        TInt count( iMessageContainer.Count() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            iLogger->MakeLogL( iMessageContainer[ a ]->LoggerMessage() );
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::EndLoggingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::EndLoggingL()
    {
    CHAT_DP_FUNC_ENTER( "EndLoggingL" );

    //If logging is not on. we do not call endlogging.
    //Endlogging will leave with KErrNotReady if we call it without
    //logging on.
    if ( !iLogger->IsLoggingOn() )
        {
        CHAT_DP_FUNC_DONE( "EndLoggingL" );
        return;
        }

    //End logging.
    iLogger->EndLoggingL();
    CHAT_DP_FUNC_DONE( "EndLoggingL" );
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::IsLoggingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TBool CCAMessageContainer::IsLoggingOn() const
    {
    return iLogger->IsLoggingOn();
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::FindIndex
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageContainer::FindIndex( TInt aOperationCode ) const
    {
    // Use count of all messages when finding with operationcode.
    TInt count( iMessageContainer.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        if ( iMessageContainer[ a ]->OperationCode() == aOperationCode )
            {
            return a;
            }
        }
    return KErrNotFound;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::FindIndex
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAMessageContainer::FindIndex( MCAMessage& aMessage ) const
    {
    // Use count of all messages when finding with operationcode.
    TInt count( iMessageContainer.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        if ( iMessageContainer[ a ] == &aMessage )
            {
            return a;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCAMessageContainer::TargetId
// ( Other items commented in header )
// -----------------------------------------------------------------------------
//
MCAMessageContainerInfo& CCAMessageContainer::TargetId()
    {
    return *this;
    }



// -----------------------------------------------------------------------------
// CCAMessageContainer::Lock
// locks messagecontainer if needed.
// -----------------------------------------------------------------------------
//
void CCAMessageContainer::Lock( TBool aLocked )
    {
    iLocked = aLocked;
    }

// -----------------------------------------------------------------------------
// CCAMessageContainer::MakeLogL
// Tries to make log if possible.
// -----------------------------------------------------------------------------
//
void CCAMessageContainer::MakeLogL( MCALoggerMessage& aMessage )
    {
    CHAT_DP_FUNC_ENTER( "MakeLogL" );
    if ( iLogger->IsLoggingOn() )
        {
        TRAPD( ret, iLogger->MakeLogL( aMessage ) )
        if ( ret != KErrNone )
            {
            TInt ignore;
            TRAP( ignore, iLogger->EndLoggingL() )
            CActiveScheduler::Current()->Error( ret );
            }
        }
    CHAT_DP_FUNC_DONE( "MakeLogL" );
    }

// -----------------------------------------------------------------------------
// CCAMessageContainer::NotifyContactForPending
// ( Other items commented in header )
// -----------------------------------------------------------------------------
//
void CCAMessageContainer::NotifyContactForPending() const
    {
    TInt ignore;
    TRAP( ignore,
          CCAStorageManagerFactory::ContactListInterfaceL()->
          SetPendingMessages( *iTargetId, UnreadCount(
                                  MCAMessagesReadInterface::EUnreadReceived ) ) );
    }

// -----------------------------------------------------------------------------
// CCAMessageContainer::ComparableTarget
// ( Other items commented in header )
// -----------------------------------------------------------------------------
//
const TDesC& CCAMessageContainer::ComparableTarget() const
    {
    if ( !iDecodedTargetId )
        {
        return KNullDesC;
        }
    return *iDecodedTargetId;
    }

// -----------------------------------------------------------------------------
// CCAMessageContainer::AllMessagesCount
// ( Other items commented in header )
// -----------------------------------------------------------------------------
//
TInt CCAMessageContainer::AllMessagesCount() const
    {
    return iMessageContainer.Count();
    }

// -----------------------------------------------------------------------------
// CCABufferManager::NotifyContactForPending
// ( Other items commented in header )
// -----------------------------------------------------------------------------
//
MCAMessage& CCAMessageContainer::MessageFromAll( TInt aIndex ) const
    {
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < AllMessagesCount() );
    return *iMessageContainer[ aIndex ];
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::DeleteMessageFromAll
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
void CCAMessageContainer::DeleteMessageFromAll( TInt aIndex )
    {
    MCAMessage* target = &MessageFromAll( aIndex );
    iMessageContainer.Remove( aIndex );
    iContainerSizeInBytes -= target->SizeInBytes();
    if ( iUnreadOffset > aIndex )
        {
        --iUnreadOffset;
        }

    if ( iObserver )
        {
        iObserver->HandleMessageEvent( MCAMessageObserver::EMessageDeleted,
                                       aIndex );
        }

    if ( target->DecreaseOwnerCount() )
        {
        delete target;
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::IsLocked
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
TBool CCAMessageContainer::IsLocked() const
    {
    return iLocked || iMemoryHandlingLocked;
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::ContainerSizeInBytes
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
TInt CCAMessageContainer::ContainerSizeInBytes() const
    {
    return iContainerSizeInBytes;
    }

//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."
//-----------------------------------------------------------------------------
// CCAMessageContainer::ReLoggingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
//
void CCAMessageContainer::ReLoggingL( MCALoggerMessageHeader& aFile )
    {
    CHAT_DP_FUNC_ENTER( "ReLoggingL" );
    __CHAT_ASSERT_DEBUG( iLogger );

    if ( iLogger->IsLoggingOn() )
        {
        User::Leave( KErrInUse );
        }

    //Start logging. The ownership of the file is transferred
    iLogger->ReLoggingL( aFile );

    // Flush already got messages to logfile.
    if ( iLogger->IsLoggingOn() )
        {
        // Write the contents of message buffer to log
        TInt count( iMessageContainer.Count() );
        for ( TInt a( 0 ); a < count; ++a )
            {
            iLogger->MakeLogL( iMessageContainer[ a ]->LoggerMessage() );
            }
        }
    }

//-----------------------------------------------------------------------------
// CCAMessageContainer::EndReLoggingL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAMessageContainer::EndReLoggingL()
    {
    CHAT_DP_FUNC_ENTER( "EndReLoggingL" );

    //If logging is not on. we do not call endlogging.
    //Endlogging will leave with KErrNotReady if we call it without
    //logging on.
    if ( !iLogger->IsLoggingOn() )
        {
        CHAT_DP_FUNC_DONE( "EndReLoggingL" );
        return;
        }

    //End logging.
    iLogger->EndReLogging();
    CHAT_DP_FUNC_DONE( "EndReLoggingL" );
    }


// end of file
