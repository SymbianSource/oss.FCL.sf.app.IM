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
* Description:  Process Component for Conversations/Groups
*
*/



// INCLUDE FILES

#include 	"CCAConversationPC.h"

#include 	"MCAGroupManagerInterface.h"
#include 	"MCAConversationObserverPC.h"
#include    "CCAStorageManagerFactory.h"
#include	"ChatDebugPrint.h"
#include    "ChatDefinitions.h"
#include    "MCAStoredContacts.h"
#include    "MCAStoredContact.h"
#include    "CAUtils.h"
#include 	"MCAMessage.h"
#include	"MCAChatInterface.h"
#include    "MCAMessagesWriteInterface.h"
#include    "MCAMessagesReadInterface.h"
#include	"MCAMessageContainerInfo.h"
#include	"PublicEngineDefinitions.h"
#include	"MCAStoredGroups.h"
#include	"MCAStoredGroup.h"
#include	"CCAEngine.h"
#include    "ImpsCSPAllErrors.h"
#include	"MCAMessageUtils.h"
#include	"MCAMessageErrorInformer.h"
#include	"MCASettings.h"
#include	"MCAMessageCreator.h"
#include    "TEnumsPC.h"
#include	"CCAServerContactsArrayPC.h"
#include	"MCAServerContactsArrayPC.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAConversationPC::NewL
// -----------------------------------------------------------------------------
//
CCAConversationPC* CCAConversationPC::NewL( CCAEngine& aEngine )
    {

    CCAConversationPC* self = new ( ELeave ) CCAConversationPC( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCAConversationPC::~CCAConversationPC()
    {
    delete iContactId;

    iObserver = NULL;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC::CCAConversationPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAConversationPC::CCAConversationPC( CCAEngine& aEngine )
        : iEngine( aEngine )
    {

    }


// -----------------------------------------------------------------------------
// CCAConversationPC::ConstructL
// -----------------------------------------------------------------------------
void CCAConversationPC::ConstructL()
    {
    iContacts = CCAStorageManagerFactory::ContactListInterfaceL();

    iGroups = CCAStorageManagerFactory::GroupListInterfaceL();

    iChatInterface = &iEngine.ChatInterface();

    }



// -----------------------------------------------------------------------------
// CCAConversationPC::CloseConversations
// -----------------------------------------------------------------------------
//

void CCAConversationPC::CloseConversations()
    {
    iChatInterface->CloseAllContainers();
    }

// -----------------------------------------------------------------------------
// CCAConversationPC::LeaveJoinedGroupsL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::LeaveJoinedGroupsL()
    {
    iEngine.GetGroupInterface()->LeaveJoinedGroupsL();
    }



// -----------------------------------------------------------------------------
// CCAConversationPC: HandleContactDelete
// -----------------------------------------------------------------------------
//
void CCAConversationPC::HandleContactDelete( const TDesC& aContact )
    {

    iObserver->HandleContactChange( aContact );

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: HandleAddition
// -----------------------------------------------------------------------------
//
void CCAConversationPC::HandleAddition( MCAContactList& /*aList*/, MCAStoredContact& aContact )
    {

    if ( CAUtils::NeutralCompare( aContact.UserId(), *iContactId ) == 0 )
        {
        iObserver->HandleContactChange( aContact.UserId() );

        }

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: HandleChange
// -----------------------------------------------------------------------------
//
void CCAConversationPC::HandleChange( MCAContactList* /*aList*/, MCAStoredContact* aContact,
                                      TStorageManagerGlobals::TCAObserverEventType aEventType,
                                      TBool /*aUserIdChanged*/ )
    {

    const TDesC& userId = ( aContact ? aContact->UserId() : KNullDesC );

    if ( aContact )
        {


        if ( !iContactId )
            {
            return;
            }
        if ( CAUtils::NeutralCompare( userId, *iContactId ) == 0 )
            {
            // TO be added later. If required.

            }
        }
    else if ( aEventType == TStorageManagerGlobals::EStorageEventMultipleChanges )
        {
        // TO be added later. If required.
        // multiple contacts were added, update title

        }

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: HandleChatEvent
// -----------------------------------------------------------------------------
//
void CCAConversationPC::HandleChatEvent( TChatEventType aEvent,
                                         MCAMessage* aMessage /*= NULL*/ )
    {

    const TDesC& wvid( aMessage ? aMessage->Sender() : KNullDesC );
    const TPtrC& identification( aMessage ? iContacts->Identification( wvid ) : KNullDesC() );

    if ( aEvent == EUnreadCountChanged && aMessage )
        {
        if ( aMessage->MessageType() != MCAMessage::EMessagePTOP )
            {
            // don't show flasher if this isn't p2p message
            // or if there's no container (this view isn't active)
            return;
            }

        //fix - When image is sent within our application
        //the wvid is KNullDesC(),
        //This is to avoid adding this KNullDesC to our chat	store
        if ( !CAUtils::NeutralCompare( wvid, KNullDesC ) )
            {
            return;
            }

        // Show flasher if sender is not iContactId or own id
        if ( ( CAUtils::NeutralCompare( wvid, *iContactId ) != 0 ) &&
             ( CAUtils::NeutralCompare(
                   wvid, iContacts->OwnStatus().UserId() ) != 0 ) )
            {
            iObserver->HandleChatEvent( wvid , identification );
            }
        }
    }



// -----------------------------------------------------------------------------
// CCAConversationPC: HandleMessageError
// -----------------------------------------------------------------------------
//
void CCAConversationPC::HandleMessageError( TInt aError, MCAMessage* aMessage )
    {

    TPtrC nick( KNullDesC );
    if ( aMessage )
        {
        if ( aMessage->ContentType() == MCAMessage::EContentPicture )
            {
            iObserver->HandleMessageError( aError, KNullDesC(), TEnumsPC::EContentPicture );
            return;
            }

        nick.Set( iContacts->Identification( aMessage->Recipient() ) );
        }

    CHAT_DP( D_CHAT_LIT( "CCAConversationsContainer::\
                         DoHandleMessageErrorL %d %S" ),
             aError, &nick );

    if ( aError == ECSPUnknownUserId )
        {
        // This way error can be recognized in note mapper
        aError += Imps_ERROR_BASE;
        }

    iObserver->HandleMessageError( aError, nick, TEnumsPC::EContentOther );

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: DeleteChatL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::DeleteChatL( const TDesC& aServerAddress,
                                     const TDesC& aUserId,
                                     const TDesC& aTargetId /*= KSendBufferId*/ )
    {
    iChatInterface->DeleteChatL( aServerAddress,
                                 aUserId,
                                 aTargetId );
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: InitialiseResourcesL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::InitialiseResourcesL( const TDesC& aContactId )
    {

    iContactId = aContactId.AllocL();

    // resolve message read interface
    iReadInterface =
        &iChatInterface->MessageReadInterfaceL( KNullDesC, KNullDesC, *iContactId );
    iWriteInterface =
        &iChatInterface->MessageWriteInterfaceL( KNullDesC, KNullDesC );


    iContacts->AddObserverL( this );// listen these events..
    // Register this as chat observer
    iChatInterface->RegisterChatObserver( this );

    const MCAMessageUtils& mUtils = iEngine.MessageUtils();
    mUtils.MessageErrorInformer().RegisterObserver( this );

    iMsgContainerInfo = &iReadInterface->TargetId();

    }



// -----------------------------------------------------------------------------
// CCAConversationPC: ReleaseResources
// -----------------------------------------------------------------------------
//
void CCAConversationPC::ReleaseResources()
    {

    iContacts->RemoveObserver( this );

    iChatInterface->UnregisterChatObserver( this );

    const MCAMessageUtils& mUtils = iEngine.MessageUtils();
    mUtils.MessageErrorInformer().UnregisterObserver( this );

    if ( iContactId )
        {
        delete iContactId;

        iContactId = NULL;
        }

    }



// -----------------------------------------------------------------------------
// CCAConversationPC: GetIdentification
// -----------------------------------------------------------------------------
//
const TPtrC CCAConversationPC::
GetIdentification( const TDesC& aContactId )
    {
    return iContacts->Identification( aContactId );
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: GetActiveConversationReadInterfaceId
// -----------------------------------------------------------------------------
//
const TDesC& CCAConversationPC::GetReadInterfaceId()
    {
    return iMsgContainerInfo->Target();
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: GetLogFileName
// -----------------------------------------------------------------------------
//
void CCAConversationPC::GetLogFileName( TPtr& aLogFileNamePtr )
    {
    aLogFileNamePtr.Copy( iContacts->Identification(
                              iMsgContainerInfo->Target() ).Left( KMaxLogFileName ) );


    }



// -----------------------------------------------------------------------------
// CCAConversationPC: StartLoggingL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::RecordChatL( const TDesC& aLogFilename )
    {
    iReadInterface->StartLoggingL( aLogFilename );

    if ( iReadInterface->IsLoggingOn() )
        {
        iReadInterface->EndLoggingL();
        }
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: SendMessageL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::SendMessageL( const TDesC& aMessageBuffer )
    {

    MCAMessageCreator& creator = iEngine.MessageUtils().MessageCreator();

    HBufC* ownId = iEngine.GetInternalSettingsInterface()->ValueL(
                       MCASettings::EOwnWVUserID );
    CleanupStack::PushL( ownId );

    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        0,
        KNullDesC,
        KNullDesC,
        *ownId,
        iMsgContainerInfo->Target(), // Recipient
        NULL,
        NULL,
        aMessageBuffer,
        KNullDesC8,
        KNullDesC8,
        MCAMessage::EMessageSent
        };

    iWriteInterface->AppendL( creator.CreateMessageL( data ) );

    CleanupStack::PopAndDestroy( ownId );

    }



// -----------------------------------------------------------------------------
// CCAConversationPC: PopulateGroupDetailsLC
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC* CCAConversationPC::PopulateGroupDetailsLC()
    {
    RPointerArray<MCAStoredGroup> groupList;

    CDesCArray* groupNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupNames );

    CDesCArray* groupIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupIds );

    CleanupClosePushL( groupList );
    iGroups->PopulateGroupsListL( groupList );

    // there's no need to skip groups in this case. but we do need to make sure
    // the pairs are in correct order after sorting

    // we can use CCAServerContactsArray, as we deal with descriptor tuple
    // just as with contacts. the class naming is a bit misleading

    TInt count( groupList.Count() );
    for ( TInt index( 0 ); index < count; index++ )
        {
        groupNames->AppendL( groupList[ index ]->GroupName() );
        groupIds->AppendL( groupList[ index ]->GroupId() );
        }

    CleanupStack::PopAndDestroy(); // groupList.Close()

    CleanupStack::Pop( 2, groupNames ); // groupIds,groupNames

    CCAServerContactsArrayPC* pairsArray =
        CCAServerContactsArrayPC::NewL( groupNames, groupIds, KNullDesC );

    //Not owned. Transfer ownership of pairsArray to the caller.
    //since we return as MCAServerContactsArrayPC object sliciing will happen
    //so its better to push the original data onto stack
    CleanupStack::PushL( pairsArray );

    return pairsArray;

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: AddObserverL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::AddObserver(
    MCAConversationObserverPC* aObserver )
    {

    ASSERT( aObserver );

    iObserver = aObserver;

    }


// -----------------------------------------------------------------------------
// CCAConversationPC: RemoveObserver
// -----------------------------------------------------------------------------
//
void CCAConversationPC::RemoveObserver()
    {
    iObserver = NULL;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: GetReadInterfaceName
// -----------------------------------------------------------------------------
//
TPtrC CCAConversationPC::GetReadInterfaceName()
    {
    return iReadInterface->Name() ;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: ReadInterface
// -----------------------------------------------------------------------------
//
MCAMessagesReadInterface& CCAConversationPC::ReadInterfaceL() const
    {
    if ( !iReadInterface )
        {
        User::Leave( KErrGeneral );
        }
    return *iReadInterface;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: WriteInterface
// -----------------------------------------------------------------------------
//
MCAMessagesWriteInterface& CCAConversationPC::WriteInterfaceL() const
    {
    if ( !iWriteInterface )
        {
        User::Leave( KErrGeneral );
        }
    return *iWriteInterface;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: SetActiveConversationL
// -----------------------------------------------------------------------------
//
void CCAConversationPC::SetActiveConversationL( const TDesC& aWvId )
    {

    if ( iContactId )
        {
        delete iContactId;

        iContactId = NULL;
        }

    iContactId = aWvId.AllocL();

    // resolve message read interface
    iReadInterface =
        &iChatInterface->MessageReadInterfaceL( KNullDesC, KNullDesC, *iContactId );

    iMsgContainerInfo = &iReadInterface->TargetId();

    }

// -----------------------------------------------------------------------------
// CCAConversationPC: GroupCount
// -----------------------------------------------------------------------------
//
TInt CCAConversationPC::GroupCount( TBool aOnlyVisible )
    {
    return iGroups->GroupCount( aOnlyVisible ) ;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: ContactCount
// -----------------------------------------------------------------------------
//
TInt CCAConversationPC::ContactCount()
    {
    return iContacts->ContactCount();
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: CheckReply
// -----------------------------------------------------------------------------
//
void CCAConversationPC::CheckReply( TPtrC& aWvid, TBool& aContactInlist )
    {
    if ( !iReadInterface->TargetId().IsAddToContactAsked() )
        {
        // Set true, so we do not try to check this one again.
        iReadInterface->TargetId().SetAddToContactAsked( ETrue );

        if ( iReadInterface->MessageCount() )
            {
            aWvid.Set( iMsgContainerInfo->Target() );

            if ( !iContacts->FindAnyContact( aWvid ) )
                {
                aContactInlist = EFalse;
                }

            }
        }
    }

// -----------------------------------------------------------------------------
// CCAConversationPC: FindAnyContact
// -----------------------------------------------------------------------------
//
TBool CCAConversationPC::FindAnyContact( const TDesC& aContactId )
    {

    if ( iContacts->FindAnyContact( aContactId ) )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCAConversationPC: IsActiveConversation
// -----------------------------------------------------------------------------
//
TBool CCAConversationPC::IsActiveConversation( const TDesC& aContactId )

    {
    TBool retValue = EFalse;

    if ( iContactId )
        {
        if ( !CAUtils::NeutralCompare( aContactId, *iContactId ) )
            {
            retValue = ETrue;
            }
        }

    return  retValue;
    }



//-----------------------------------------------------------
//CCAConversationPC::SetMessageReadInterfaceL
//-----------------------------------------------------------
void CCAConversationPC::SetMessageReadInterfaceL( const TDesC& aWvId )
    {

    CHAT_DP( D_CHAT_LIT( " Trying to find chat data for %S" ), aWvId );

    iReadInterface = &iChatInterface->MessageReadInterfaceL( KNullDesC, KNullDesC, aWvId );

    iMsgContainerInfo = &iReadInterface->TargetId();

    }



//-----------------------------------------------------------
//CCAConversationPC::SetMessageReadInterfaceL
//-----------------------------------------------------------
void CCAConversationPC::SetMessageWriteInterfaceL()
    {

    iWriteInterface = &iChatInterface->MessageWriteInterfaceL( KNullDesC, KNullDesC );

    }


//-----------------------------------------------------------
//CCAConversationPC::MessageCount
//-----------------------------------------------------------
TInt CCAConversationPC::MessageCount()
    {
    if ( iReadInterface )
        {
        return iReadInterface->MessageCount();
        }

    return 0;

    }
//    End of File
