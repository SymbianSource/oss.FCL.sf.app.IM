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
* Description:  Flow controller for messages
*
*/


// INCLUDE FILES
#include    <coemain.h>

#include    "CCAMessageFlowHandlerPC.h"
#include    "ChatDebugAssert.h"
#include    "MCAMessageContainer.h"
#include    "MCAMessagesReadInterface.h"
#include    "MCASettings.h"
#include    "ChatDebugPrint.h"
#include    "imutils.h"
#include    "imnginternalcrkeys.h"
#include    "imprivatecrkeys.h"
#include    "imvariant.hrh"
#include	"CCARecordedChatsPC.h"
#include	"CCAProcessManagerFactory.h"
#include	"CCAConversationMessage.h"
#include	"CCAEngine.h"
#include	"MCAProcessManager.h"
#include	"MCARecordedChatsPC.h"


// CONSTANTS
const TInt KMilliToMicro     = 1000;    // multiplier for converting milliseconds to microseconds

const TInt KTimeIntervalSlow = 2000;    // slowest message speed (in milliseconds)
const TInt KTimeIntervalFast =  200;    // fastest message speed (in milliseconds)

const TInt KTimeIntervalOpen =    0;    // message fetching speed when opening the view
const TInt KInitialMessages  =    3;    // fetch n messages at once when opening the view

const TInt KSettingValueMin  =    1;    // minimum value for flow control setting
const TInt KSettingValueMax  =    3;    // maximum value for flow control setting


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::CCAMessageFlowHandlerPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAMessageFlowHandlerPC::CCAMessageFlowHandlerPC( MCAMessageContainer& aMessageContainer,
                                                  MCAMessagesReadInterface& aReadInterface,
                                                  MCASettings& aSettings )
        : CTimer( EPriorityStandard ),
        iMessages( aMessageContainer ),
        iReadInterface( aReadInterface ),
        iSettings( aSettings ),
        iFetchMessages( ETrue )

    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCAMessageFlowHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAMessageFlowHandlerPC* CCAMessageFlowHandlerPC::NewL(
    MCAMessageContainer& aMessageContainer,
    MCAMessagesReadInterface& aReadInterface,
    MCASettings& aSettings,
    TBool aRecordedChatHandler )
    {
    CCAMessageFlowHandlerPC* self = new( ELeave ) CCAMessageFlowHandlerPC(
        aMessageContainer,
        aReadInterface,
        aSettings );

    CleanupStack::PushL( self );
    self->ConstructL( aRecordedChatHandler );
    CleanupStack::Pop( self );

    return self;
    }
// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::ConstructL( TBool aRecordedChatHandler )
    {
    // construct base class
    CTimer::ConstructL();

    // and observe changes
    iReadInterface.SetObserver( this );

    // fetch flow control value from settings
    UpdateTimeIntervalL();

    // and observe
    iSettings.AddObserverL( this );

    // Check CR variation
    if ( aRecordedChatHandler )
        {
        iBgOpeningMode = EFalse;
        }
    else
        {
        iBgOpeningMode = IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                         & EIMFeatBackgroundGroupOpening;
        }

    // start the timer if there are messages
    iInitialMsgCount = iReadInterface.MessageCount();
    if ( iInitialMsgCount + iReadInterface.UnreadCount() > 0 )
        {
        // lock the buffer
        iReadInterface.Lock( ETrue );

        // use faster timer when constructing
        iTimeInterval = KTimeIntervalOpen * KMilliToMicro;
        After( iTimeInterval );
        }


    }


// Destructor
CCAMessageFlowHandlerPC::~CCAMessageFlowHandlerPC()
    {

    if ( !iChatDeleted )
        {
        iReadInterface.SetObserver( NULL );
        iReadInterface.Lock( EFalse );
        }
    iSettings.RemoveObserver( this );
    Cancel();

    }


// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::FetchMessages
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::FetchMessages( TBool aFetch )
    {
    TBool reallyInBg =
        CCoeEnv::Static()->RootWin().OrdinalPosition() == 0 ? EFalse : ETrue;

    if ( !reallyInBg && !aFetch )
        {
        // Application is not really in background, this can happen
        // if key lock is activated while application is in foreground
        aFetch = ETrue;
        }

    iFetchMessages = aFetch;
    if ( iFetchMessages && !iChatDeleted )
        {
        // we're allowed to fetch messages again
        if ( iReadInterface.UnreadCount() > 0 && !IsActive() )
            {
            // there are some unread messages
            // => start the timer (if not active already)
            After( iTimeInterval );
            }
        }
    else if ( iMsgIndex < iInitialMsgCount )
        {
        Cancel();
        After( iTimeInterval );
        return;
        }
    else
        {
        // we're not allowed to fetch new messages any more, so cancel the timer
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::HandleMessageEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::HandleMessageEvent( TMessageEventType aEvent,
                                                  TInt aIndex )
    {
    CHAT_DP( D_CHAT_LIT( "CCAMessageFlowHandlerPC::HandleMessageEvent event %d, \
                         index %d" ), aEvent, aIndex );
    switch ( aEvent )
        {
        case ENewMessage:
            {
            if ( !IsActive() && iFetchMessages )
                {
                // if not active, start timer
                After( iTimeInterval );
                }

            if ( IsOpening() && iBgOpeningMode )
                {
                // New message during opening phase
                iNewMsgWhileOpening = ETrue;
                }

            // otherwise do nothing as timer fetches the messages
            break;
            }
        case EMessageDeleted:
            {
            TInt corIndex = MessageIndexCorrection( aIndex );
            iMessages.RemoveMessage( corIndex );
            break;
            }
        case EMessageChanged:
            {
            TInt corIndex = MessageIndexCorrection( aIndex );
            TRAPD( err, iMessages.MessageChangedL( corIndex ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            break;
            }
        case EChatDeleted:
            {
            iChatDeleted = ETrue;
            iMessages.MarkDeleted();
            break;
            }
        default:
            {
            // not supported
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::RunL()
    {
    TBool messages( ETrue );
    TBool opening = IsOpening();

    // If we're opening the view, fetch KInitialMessages at a time, otherwise just one
    TInt count( opening ? KInitialMessages : 1 );

    while ( count-- > 0 && messages )
        {
        if ( iBgOpeningMode && opening )
            {
            // Opening the chat with background message fetching.
            // Perform steps in following order:
            // 1. Add possible new messages that are received during
            //    the opening phase to the end of chat normally
            // 2. Insert unread messages to the beginning of chat
            // 3. Insert read messages to the beginning
            if ( iNewMsgWhileOpening && iFetchMessages )
                {
                // Add the new message now
                CCAConversationMessage* msg = CCAConversationMessage::NewL(
                                                  iReadInterface.ReadNextUnread() );

                CleanupStack::PushL( msg );
                iMessages.AddMessageL( *msg );
                CleanupStack::Pop( msg );

                iNewMsgWhileOpening = EFalse;
                }
            else if ( iReadInterface.UnreadCount() > 0 && iFetchMessages )
                {
                // Insert unread messages, insert in last-to-first order
                iAddedUnreadMsgs++;

                CCAConversationMessage* msg = CCAConversationMessage::NewL(
                                                  iReadInterface.ReadUnreadFromIndex(
                                                      iReadInterface.MessageCount()
                                                      + iReadInterface.UnreadCount()
                                                      - iAddedUnreadMsgs )  );

                CleanupStack::PushL( msg );
                iMessages.InsertMessageL( *msg );
                CleanupStack::Pop( msg );

                }
            else if ( iMsgIndex < iInitialMsgCount )
                {
                // Insert unread messages, in last-to-first order
                iAddedUnreadMsgs = 0;   // Init to zero
                TInt index = iInitialMsgCount - 1 - iMsgIndex;
                CCAConversationMessage* msg = CCAConversationMessage::NewL(
                                                  iReadInterface.Message( index ) );

                CleanupStack::PushL( msg );
                iMessages.InsertMessageL( *msg );
                CleanupStack::Pop( msg );

                iMsgIndex++;
                }
            else
                {
                // Check if the initial speed was active
                if ( opening )
                    {
                    UpdateTimeIntervalL();
                    }
                messages = EFalse;
                iReadInterface.Lock( EFalse );
                }
            }
        else
            {
            // Functionality in opening in releases 3.1 and earlier
            // and normal functionality when the chat is already fully opened
            // and new messages are received.
            // 1. Add read messages in first-to-last order
            // 2. Add unread messages in first-to-last order
            if ( iMsgIndex < iInitialMsgCount )
                {
                // Add read messages
                CCAConversationMessage* msg = CCAConversationMessage::NewL(
                                                  iReadInterface.Message( iMsgIndex++ ) );
                CleanupStack::PushL( msg );
                iMessages.AddMessageL( *msg );
                CleanupStack::Pop( msg );

                }
            else if ( iReadInterface.UnreadCount() > 0 )
                {
                // Add unread messages
                CCAConversationMessage* msg = CCAConversationMessage::NewL(
                                                  iReadInterface.ReadNextUnread() );
                CleanupStack::PushL( msg );
                iMessages.AddMessageL( *msg );
                CleanupStack::Pop( msg );
                }
            else
                {
                // Check if the initial speed was active
                if ( opening )
                    {
                    UpdateTimeIntervalL();
                    }
                messages = EFalse;
                iReadInterface.Lock( EFalse );
                }
            }
        }

    // And restart timer if needed
    if ( messages )
        {
        Cancel();
        After( iTimeInterval );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::RunError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAMessageFlowHandlerPC::RunError( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCAMessageFlowHandlerPC::RunError (%d)" ), aError );

    // Something leaved in RunL
    if ( aError == KErrNoMemory )
        {
        // inform user about low memory
        CActiveScheduler::Current()->Error( aError );
        }

    if ( IsActive() )
        {
        // stop processing messages
        Cancel();
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::HandleSettingsChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    // code scanner warning can be ignored
    // non-leaving function in leavining function
    switch ( aChangedSettingEnum )
        {
        case MCASettings::EMessageFlowSettingLevel:
            {
            UpdateTimeIntervalL();
            // no need to restart the timer even if we're running,
            // because it will get updated after next message
            break;
            }
        default:
            {
            // no need to react to other changes
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::UpdateTimeIntervalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageFlowHandlerPC::UpdateTimeIntervalL()
    {
    // codescanner warning can be ignored
    TInt flowSetting( iSettings.Value(
                          MCASettings::EMessageFlowSettingLevel ) );

    // flowSetting is from KSettingValueMin to KSettingValueMax
    TInt range( KSettingValueMax - KSettingValueMin );
    TInt newRange( KTimeIntervalFast - KTimeIntervalSlow );

    // convert it to a range from KTimeIntervalSlow to KTimeIntervalFast
    TInt flowSpeed( ( flowSetting - KSettingValueMin )*newRange / range );

    // update the end point
    flowSpeed += KTimeIntervalSlow;

    // validate the result
    if ( flowSpeed > KTimeIntervalSlow )
        {
        flowSpeed = KTimeIntervalSlow;
        }

    if ( flowSpeed < KTimeIntervalFast )
        {
        flowSpeed = KTimeIntervalFast;
        }

    // and convert from milliseconds to microseconds
    iTimeInterval = flowSpeed * KMilliToMicro;
    }

// -----------------------------------------------------------------------------
// CCAMessageFlowHandlerPC::IsOpening
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAMessageFlowHandlerPC::IsOpening() const
    {
    return iTimeInterval ==
           TTimeIntervalMicroSeconds32( KTimeIntervalOpen * KMilliToMicro );
    }
// -----------------------------------------------------------------------------
// CCAMessageFlowHandler::MessageIndexCorrection
// -----------------------------------------------------------------------------
//
TInt CCAMessageFlowHandlerPC::MessageIndexCorrection( TInt aIndex ) const
    {
    if ( !IsOpening() || !iBgOpeningMode )
        {
        // if not opening chat or message insertion is not used
        // => then index is already correct
        return aIndex;
        }

    // We need to calculate correct index
    TInt index = -1;
    if ( iAddedUnreadMsgs > 0 )
        {
        // we have only added some unread messages
        TInt totalCount = iReadInterface.MessageCount() +
                          iReadInterface.UnreadCount();
        index = aIndex - ( totalCount - iAddedUnreadMsgs );
        }
    else
        {
        // added some read messages
        index = aIndex - ( iInitialMsgCount - 1 - iMsgIndex );
        }

    CHAT_DP( D_CHAT_LIT( "CCAMessageFlowHandler::MessageIndexCorrection \
        old index: %d, corrected index: %d" ), aIndex, index );
    return index;
    }

//  End of File
