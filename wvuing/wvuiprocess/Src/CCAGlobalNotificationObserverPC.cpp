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
* Description:  Global interface for the UI to get notifications when theres a
*				 change in contact, list, invite, settings, conversation, group etc
*
*/


#include "MCAContactListModel.h"
#include "MCAChatInterface.h"
#include "MCAInvite.h"
#include "MCASettings.h"
#include "TStorageManagerGlobals.h"
#include "MCAContactList.h"
#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"
#include "CCAStorageManagerFactory.h"
#include "MCAMessage.h"
#include "MCAMessageUtils.h"
#include "MCAUiGlobalNotificationObserverPC.h"
#include "CCAGlobalNotificationObserverPC.h"
#include "MCAMessageErrorInformer.h"
#include "MCAContactLists.h"
#include "capresencemanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::CCAGlobalNotificationObserverPC
// -----------------------------------------------------------------------------

CCAGlobalNotificationObserverPC::CCAGlobalNotificationObserverPC(
    MCAContactListModel& aContactListModel,
    MCAChatInterface& aChatInterface,
    MCAInvite& aInviteInterface,
    MCASettings& aSettingsInterface,
    const MCAMessageUtils& aMessageUtils )
        : iContactListModel( aContactListModel ),
        iChatInterface( aChatInterface ),
        iInviteInterface( aInviteInterface ),
        iSettingsInterface( aSettingsInterface ),
        iMessageUtils( aMessageUtils )
    {
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::NewL
// -----------------------------------------------------------------------------

CCAGlobalNotificationObserverPC* CCAGlobalNotificationObserverPC::NewL(
    MCAContactListModel& aContactListModel,
    MCAChatInterface& aChatInterface,
    MCAInvite& aInviteObserver,
    MCASettings& aSettingsInterface,
    const MCAMessageUtils& aMessageUtils )
    {
    CCAGlobalNotificationObserverPC* self
    = new ( ELeave ) CCAGlobalNotificationObserverPC(
        aContactListModel,
        aChatInterface,
        aInviteObserver,
        aSettingsInterface,
        aMessageUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::ConstructL
// -----------------------------------------------------------------------------
void CCAGlobalNotificationObserverPC::ConstructL()
    {
    iContactInterface = CCAStorageManagerFactory::ContactListInterfaceL();
    iContactInterface->AddObserverL( this );// listen these events..
    iInviteInterface.AddInviteObserver( this );
    iSettingsInterface.AddObserverL( this );  //register with engine

    // Register this as chat observer
    iChatInterface.RegisterChatObserver( this );

    iMessageUtils.MessageErrorInformer().RegisterObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::~CCAGlobalNotificationObserverPC
// -----------------------------------------------------------------------------
CCAGlobalNotificationObserverPC::~CCAGlobalNotificationObserverPC()
    {
    iGlobalNotificationObservers.ResetAndDestroy();
    iGlobalNotificationObservers.Close();
    if ( iContactInterface )
        {
        iContactInterface->RemoveObserver( this );
        }
    iInviteInterface.RemoveInviteObserver( this );
    iSettingsInterface.RemoveObserver( this );
    iChatInterface.UnregisterChatObserver( this );
    iMessageUtils.MessageErrorInformer().UnregisterObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleContactDelete
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAGlobalNotificationObserverPC::HandleContactDelete( const TDesC& aContactId )
    {
    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        iGlobalNotificationObservers[index]->HandleDelete( aContactId, TEnumsPC::EContactItem );
        }

    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleAddition
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAGlobalNotificationObserverPC::HandleAddition( MCAContactList& /*aList*/, MCAStoredContact& /*aContact*/ )
    {
    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        iGlobalNotificationObservers[index]->HandleAddition( TEnumsPC::EContactItem );
        }
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleChange
// MCAStoredContactsObserver implementation
// -----------------------------------------------------------------------------

void CCAGlobalNotificationObserverPC::HandleChange( MCAContactList* aList, MCAStoredContact* /*aContact*/,
                                                    TStorageManagerGlobals::TCAObserverEventType aEventType,
                                                    TBool /*aUserIdChanged */ )
    {
    // we need to handle the change here..from storage manager...
    TEnumsPC::TChange eventtype = TEnumsPC::EUnknownEvent;
    TInt error( KErrNone );
    switch ( aEventType )
        {
        case TStorageManagerGlobals::EStorageEventPreChange:
            {
            // ignore the pre-change events
            return;
            }
        case TStorageManagerGlobals::EStorageEventListDelete:
        case TStorageManagerGlobals::EStorageEventMultipleChanges:
            {
            TInt index = iContactListModel.IndexOfList( aList, EFalse, ETrue );

            eventtype = aEventType == TStorageManagerGlobals::EStorageEventListDelete
                        ? TEnumsPC::EDelete : TEnumsPC::EMultipleChanges;

            for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
                {
                iGlobalNotificationObservers[index]->HandleChange(
                    TEnumsPC::EContactListItem,	eventtype );
                }

            break;
            }
        case TStorageManagerGlobals::EStorageEventPostChange:
            {
            // call back the UI with HandleChange here....
            for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
                {
                iGlobalNotificationObservers[index]->HandleChange(
                    TEnumsPC::EContactItem, TEnumsPC::EChanged );
                }
            break;
            }
        case TStorageManagerGlobals::EStorageEventListAddition:
        case TStorageManagerGlobals::EStorageEventListChanged:
            {
            eventtype = aEventType == TStorageManagerGlobals::EStorageEventListAddition
                        ? TEnumsPC::EAddition : TEnumsPC::EChanged;
            for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
                {
                iGlobalNotificationObservers[index]->HandleChange(
                    TEnumsPC::EContactListItem, eventtype );
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if ( error )
        {
        CActiveScheduler::Current()->Error( error );
        }

    }

// ---------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleSettingsChangeL
// @see MCASettingsObserver
// ---------------------------------------------------------

void CCAGlobalNotificationObserverPC::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {

    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        TEnumsPC::TCASettingValues settings = ConvertToSettingsEnumsPC( aChangedSettingEnum );
        iGlobalNotificationObservers[index]->HandleSettingsChangeL( settings );
        }

    }
// --------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::ConvertToSettingsEnumsPC
// (other items were commented in a header).
// --------------------------------------------------------------------
//
TEnumsPC::TCASettingValues CCAGlobalNotificationObserverPC::ConvertToSettingsEnumsPC(
    TInt aStatus )
    {
    MCASettings::TCASettingFlags engineFlags = ( MCASettings::TCASettingFlags )aStatus;

    TEnumsPC::TCASettingValues setValue = TEnumsPC::EUnknownValue;

    // Map status to setting item
    switch ( engineFlags )
        {
        case MCASettings::EDefaultScreenNameInUse :
            {
            setValue =  TEnumsPC::EDefaultScreenNameInUse;
            break;
            }
        case MCASettings::EAutomaticPresenceUpdate :
            {
            setValue =  TEnumsPC::EAutomaticPresenceUpdate;
            break;
            }

        case MCASettings::EShowOffline :
            {
            setValue =  TEnumsPC::EShowOffline;
            break;
            }
        case MCASettings::EShowTimeStamps :
            {
            setValue =  TEnumsPC::EShowTimeStamps;
            break;
            }
        default:
            {
            break;
            }
        }
    return setValue ;
    }

// ---------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleInvitation()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::HandleInvitationEventL(
    TInviteEventType aEvent, const MCAInvitation* aInvitation )
    {
    TEnumsPC::TInviteType inviteType = TEnumsPC::EInviteUnknown;
    switch ( aEvent )
        {
        case MCAInviteObserver::ENewInvite:
            {
            inviteType = TEnumsPC::ENewInvite;
            break;
            }
        case MCAInviteObserver::EInviteCancel: // Flowthrough
        case MCAInviteObserver::EInviteExpired:
            {
            inviteType = TEnumsPC::EInviteCancelOrExpired;
            break;
            }

        case MCAInviteObserver::EInviteRead:
            {
            inviteType = TEnumsPC::EInviteRead;
            break;
            }
        default:
            {
            inviteType = TEnumsPC::EInviteUnknown;
            break;
            }
        }
    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        if ( aInvitation )
            {
            iGlobalNotificationObservers[index]->HandleInvitationEventL(
                inviteType,
                iContactInterface->Identification( aInvitation->UserID() ) ,
                aInvitation->Message() );
            }

        else
            {
            iGlobalNotificationObservers[index]->HandleInvitationEventL(
                inviteType,
                KNullDesC(), KNullDesC() );
            }
        }

    }

// ---------------------------------------------------------
// CCAGlobalNotificationObserverPC::HandleInvitationResponse()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::HandleInvitationResponse(
    TBool aAcceptance,
    const TDesC& aUserId,
    const TDesC& aGroupName,
    const TDesC& aResponse )
    {
    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        iGlobalNotificationObservers[index]->HandleInvitationResponse( aAcceptance,
                                                                       aUserId,
                                                                       aGroupName,
                                                                       aResponse );
        }
    }

// ---------------------------------------------------------
// CCAGlobalNotificationObserverPC::MCAGlobalNotificationObserverPCL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::AddGlobalObserverL(
    MCAUiGlobalNotificationObserverPC* aGlobalObserver )
    {
    ASSERT( aGlobalObserver );

    TInt status( iGlobalNotificationObservers.Append( aGlobalObserver ) );
    User::LeaveIfError( status );
    }

// ---------------------------------------------------------
// CCAGlobalNotificationObserverPC::RemoveGlobalObserver()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::RemoveGlobalObserver(
    MCAUiGlobalNotificationObserverPC* aGlobalObserver )
    {
    ASSERT( aGlobalObserver );

    const TInt status( iGlobalNotificationObservers.Find( aGlobalObserver ) );
    if ( status != KErrNotFound )
        {
        iGlobalNotificationObservers.Remove( status );
        iGlobalNotificationObservers.Compress();
        }
    }


// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC: HandleChatEvent
// -----------------------------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::HandleChatEvent( TChatEventType aEvent,
                                                       MCAMessage* aMessage /*= NULL*/ )
    {

    TEnumsPC::TChatEventType aEventType = TEnumsPC::EEventNotSpecified;
    TEnumsPC::TMessagerType aMsgrType = TEnumsPC::EMessageOther;
    TEnumsPC::TMessageType aMsgType = TEnumsPC::EMessageInvalid;

    const TDesC& wvid( GetWvId( aMessage ) );
    const TDesC& msgText( aMessage ? aMessage->Text() : KNullDesC );
    const TPtrC& identification( aMessage ?
                                 iContactInterface->Identification( wvid ) : KNullDesC() );

    switch ( aEvent )
        {
        case EGroupListChanged:
            {
            aEventType = TEnumsPC::EGroupListChanged;
            break;
            }
        case EChatListChanged:
            {
            aEventType = TEnumsPC::EChatListChanged;
            break;
            }
        case ESendListChanged:
            {
            aEventType = TEnumsPC::ESendListChanged;
            break;
            }
        case EUnreadCountChanged:
            {
            aEventType = TEnumsPC::EUnreadCountChanged;
            break;
            }
        case EMemoryLow:
            {
            aEventType = TEnumsPC::EMemoryLow;
            break;
            }
        default:
            {
            break;
            }

        }

    if ( aMessage )
        {

        switch ( aMessage->MessagerType() )
            {
            case MCAMessage::EMessageSent:
                {
                aMsgrType = TEnumsPC::EMessageSent;
                break;
                }
            case MCAMessage::EMessageReceived:
                {
                aMsgrType = TEnumsPC::EMessageReceived;
                break;
                }
            default:
                {
                break;
                }

            }


        switch ( aMessage->MessageType() )
            {
            case MCAMessage::EMessagePTOP:
                {
                aMsgType = TEnumsPC::EMessagePTOP;
                break;
                }
            case MCAMessage::EMessageWhisper:
                {
                aMsgType = TEnumsPC::EMessageWhisper;
                break;
                }

            case MCAMessage::EMessageGroup:
                {
                aMsgType = TEnumsPC::EMessageGroup;
                break;
                }
            case MCAMessage::EMessageSystem:
                {
                aMsgType = TEnumsPC::EMessageSystem;
                break;
                }
            default:
                {
                break;
                }

            }

        }

    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        iGlobalNotificationObservers[index]->HandleChatEvent(
            aEventType,
            wvid ,
            identification,
            msgText,
            aMsgrType,
            aMsgType );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::HandleMessageError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGlobalNotificationObserverPC::HandleMessageError( TInt aError, MCAMessage* aMessage )
    {
    for ( TInt index = 0; index < iGlobalNotificationObservers.Count(); index++ )
        {
        iGlobalNotificationObservers[index]->HandleMessageError(
            aError,
            aMessage->Text() );
        }
    }


// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::GetWvId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCAGlobalNotificationObserverPC::GetWvId( MCAMessage* aMessage )
    {
    if ( aMessage )
        {

        if ( aMessage->MessageType() == MCAMessage::EMessagePTOP )
            {
            return aMessage->Sender();
            }
        else
            {
            return aMessage->Target();
            }
        }

    return KNullDesC;
    }
// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::IsContactListsSyncDoneL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGlobalNotificationObserverPC::IsContactListsSyncDoneL()
    {

    MCAContactLists* contactLists =
        CAPresenceManager::InstanceL()->ContactLists();
    if ( !contactLists )
        {
        // not logged in yet, so sync state is ok
        return ETrue;
        }
    return contactLists->IsSyncDone();
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::StoredContactsIsAllSynchronised
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGlobalNotificationObserverPC::StoredContactsIsAllSynchronised( TBool& aFailed )
    {
    return iContactInterface->IsAllSynchronised( aFailed );
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::IsValidListInterface
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGlobalNotificationObserverPC::IsValidStoredContactsInterface()
    {
    if ( iContactInterface )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCAGlobalNotificationObserverPC::IsValidListInterfaceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGlobalNotificationObserverPC::IsValidListInterfaceL()
    {
    MCAContactLists* contactLists =
        CAPresenceManager::InstanceL()->ContactLists();
    if ( contactLists )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//end of file

