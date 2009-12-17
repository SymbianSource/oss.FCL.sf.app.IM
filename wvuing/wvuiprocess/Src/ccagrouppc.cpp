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
* Description:  Implementation for PC Group Interface
*
*/



// INCLUDE FILES
#include "ccagrouppc.h"
#include "MCAStoredGroups.h"
#include "CCAStorageManagerFactory.h"
#include "mcagroupobserverpc.h"
#include "CCAEngine.h"
#include "MCAChatInterface.h"
#include "mcamessageswriteinterface.h"
#include "mcamessagecontainerinfo.h"
#include "MCAStoredGroup.h"
#include "MCAMessage.h"
#include "MCAStoredContacts.h"
#include "MCAGroupEventObserverPC.h"
#include "MCAGroupOperations.h"
#include "MCAGroupManagerInterface.h"
#include "MCAExtendedStoredGroup.h"
#include "MCAMessageUtils.h"
#include "MCAMessageErrorInformer.h"
#include "MCAMessageCreator.h"
#include "CAUtils.h"
#include "CCAServerContactsArrayPC.h"
#include "CAPresenceManager.h"
#include "MCAPresence.h"




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAGroupPC: Constructor
// -----------------------------------------------------------------------------
//
CCAGroupPC::CCAGroupPC( MCAGroupManagerInterface* aGroupInterface,
                        MCAChatInterface& aChatInterface,
                        const MCAMessageUtils& aMessageUtils  )
        : iGroupInterface( aGroupInterface ),
        iChatInterface( aChatInterface ),
        iMessageUtils( aMessageUtils )

    {
    }


// -----------------------------------------------------------------------------
// CCAGroupPC: NewL
// -----------------------------------------------------------------------------
//
CCAGroupPC* CCAGroupPC::NewL( MCAGroupManagerInterface* aGroupInterface,
                              MCAChatInterface& aChatInterface,
                              const MCAMessageUtils& aMessageUtils )
    {
    CCAGroupPC* self = new ( ELeave ) CCAGroupPC( aGroupInterface,
                                                  aChatInterface, aMessageUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: ConstructL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::ConstructL(  )
    {
    iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
    iStoredContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: Destructor
// -----------------------------------------------------------------------------
//
CCAGroupPC::~CCAGroupPC()
    {
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: RegisterGroupObserver
// -----------------------------------------------------------------------------
//
void CCAGroupPC::RegisterGroupObserver( MCAGroupObserverPC*  aGroupObserverPC )
    {
    ASSERT( aGroupObserverPC );
    iGroupObserverPC = aGroupObserverPC;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: UnRegisterGroupObserver
// -----------------------------------------------------------------------------
//
void CCAGroupPC::UnRegisterGroupObserver()
    {
    iGroupObserverPC = NULL;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: SetGroupEventObserverL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::SetGroupEventObserverL( MCAGroupEventObserverPC* aGroupEventObserverPC,
                                         const TDesC& aGroupId )
    {
    iGroupEventObserverPC = aGroupEventObserverPC;

    if ( !iGroupInterface )
        {
        return;
        }

    // Register this as chat observer
    if ( iGroupEventObserverPC )
        {
        iGroupInterface->SetGroupEventObserverL( aGroupId, this );
        }
    else
        {
        iGroupInterface->SetGroupEventObserverL( aGroupId, NULL );
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: ActiveGroupId
// -----------------------------------------------------------------------------
//
const TDesC& CCAGroupPC::ActiveGroupId()
    {
    return iReadInterface->TargetId().Target();
    }


// -----------------------------------------------------------------------------
// CCAGroupPC: ScreenName
// -----------------------------------------------------------------------------
//
const TDesC& CCAGroupPC::ScreenName() const
    {

    if ( iReadInterface )
        {
        return iReadInterface->TargetId().ScreenName();
        }


    // Fix warning: #940-D: missing return statement at end of non-void function
    else
        {
        return KNullDesC();
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: IsLoggingOn
// -----------------------------------------------------------------------------
//
TBool CCAGroupPC::IsLoggingOn() const
    {
    return iReadInterface->IsLoggingOn();
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: EndLoggingL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::EndLoggingL()
    {
    iReadInterface->EndLoggingL();
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: SaveChatL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::SaveChatL( const TDesC& aLogFilename )
    {
    iReadInterface->StartLoggingL( aLogFilename );

    if ( iReadInterface->IsLoggingOn() )
        {
        iReadInterface->EndLoggingL();
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: GetPropertiesL
// -----------------------------------------------------------------------------
//
TInt CCAGroupPC::GetPropertiesL( TBool& aIsGroupProps )
    {
    if ( !iGrOps )
        {
        return KErrGeneral;
        }

    TInt err( KErrNone );
    err = iGrOps->GetPropertiesL( iGroupProps, iPrivateProps );
    if ( iGroupProps )
        {
        aIsGroupProps = ETrue;
        }
    else
        {
        aIsGroupProps = EFalse;
        }
    return err;

    }

// -----------------------------------------------------------------------------
// CCAGroupPC: GroupName
// -----------------------------------------------------------------------------
//
TPtrC CCAGroupPC::GroupName( ) const
    {
    if ( !iGroupProps )
        {
        return KNullDesC();
        }
    return iGroupProps->GroupName();
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: IsPrivateAllowed
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPC::IsPrivateAllowed()
    {
    return ConvertTImpsPropertyBoolToTEnumsPC( iGroupProps->IsPrivateAllowed() );
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: IsOpen
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPC::IsOpen()
    {
    return ConvertTImpsPropertyBoolToTEnumsPC( iGroupProps->IsOpen() );
    }

// -----------------------------------------------------------------------------
// CCAGroupPC::ConvertTImpsPropertyBoolToTEnumsPC
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPC::ConvertTImpsPropertyBoolToTEnumsPC
( TImpsPropertyBool aEventSource )
    {

    switch ( aEventSource )
        {
        case EImpsPropUndef:
            {
            return TEnumsPC::EImpsPropUndef;
            }
        case EImpsPropNo:
            {
            return TEnumsPC::EImpsPropNo;
            }
        case EImpsPropYes:
            {
            return TEnumsPC::EImpsPropYes;
            }

        default:
            {
            return TEnumsPC::EImpsPropUndef;
            }
        }
    }


// ---------------------------------------------------------
// CCAGroupPC::GroupCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPC::GroupCount( TBool aOnlyVisible ) const
    {
    return iStoredGroups->GroupCount( aOnlyVisible );
    }


// ---------------------------------------------------------
// CCAGroupPC::ContactCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupPC::ContactCount( TBool aSkipOfflineContacts ) const
    {
    return iStoredContacts->ContactCount( aSkipOfflineContacts );
    }


// -----------------------------------------------------------------------------
// CCAGroupPC: InitialiseResourcesL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::InitialiseResourcesL( const TDesC& aGroupId )
    {
    //can be used as data member if used many times
    if ( iGroupId )
        {
        delete iGroupId ;
        iGroupId = NULL;
        }
    iGroupId = aGroupId.AllocL();

    // resolve message read interface
    //read/write interface can be done in ctor after complete refactoring
    iReadInterface =
        &( iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC, *iGroupId ) );
    iWriteInterface =
        &( iChatInterface.MessageWriteInterfaceL( KNullDesC, KNullDesC ) );

    iStoredGroup = iStoredGroups->FindGroup( *iGroupId );
    if ( iGroupInterface )
        {
        iGrOps = iGroupInterface->GroupOperationsL( *iGroupId );
        }
    // Register this as chat observer
    iChatInterface.RegisterChatObserver( this );
    //register this as message error observer
    iMessageUtils.MessageErrorInformer().RegisterObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: ReleaseResources
// -----------------------------------------------------------------------------
//
void CCAGroupPC::ReleaseResources()
    {
    iChatInterface.UnregisterChatObserver( this );
    iMessageUtils.MessageErrorInformer().UnregisterObserver( this );

    delete iGroupId;
    iGroupId = NULL;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: ReadInterface
// -----------------------------------------------------------------------------
//
MCAMessagesReadInterface& CCAGroupPC::ReadInterfaceL() const
    {
    if ( !iReadInterface )
        {
        User::Leave( KErrGeneral );
        }
    return *iReadInterface;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: WriteInterface
// -----------------------------------------------------------------------------
//
MCAMessagesWriteInterface& CCAGroupPC::WriteInterfaceL() const
    {
    if ( !iWriteInterface )
        {
        User::Leave( KErrGeneral );
        }
    return *iWriteInterface;
    }

// -----------------------------------------------------------------------------
// CCAGroupPC: HandleChatEvent
// -----------------------------------------------------------------------------
//
void CCAGroupPC::HandleChatEvent( TChatEventType aEvent, MCAMessage* aMessage )
    {

    if ( aEvent == EUnreadCountChanged && aMessage )
        {
        if ( aMessage->MessageType() != MCAMessage::EMessagePTOP )
            {
            // don't show flasher if this isn't p2p message
            // or if there's no container (this view isn't active)
            return;
            }

        const TDesC& wvid( aMessage->Sender() );

        // nickname or wv-less contact id

        if ( iGroupObserverPC && iStoredContacts )
            {
            iGroupObserverPC->HandleChatEvent( wvid ,
                                               iStoredContacts->Identification( wvid ) ) ;
            }
        }
    }

// ---------------------------------------------------------
// CCAGroupPC::HandleGroupLeft
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPC::HandleGroupLeft( TInt aReason )
    {
    if ( iGroupEventObserverPC )
        {
        iGroupEventObserverPC->HandleGroupLeft( aReason );
        }
    }

// ---------------------------------------------------------
// CCAGroupPC::HandleGroupPropertiesChanged()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPC::HandleGroupPropertiesChanged()
    {
    if ( iGroupEventObserverPC )
        {
        iGroupEventObserverPC->HandleGroupPropertiesChanged( );
        }

    }

// ---------------------------------------------------------
// CCAGroupPC::HandleMessageError()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupPC::HandleMessageError( TInt aError, MCAMessage* aMessage )
    {
    TPtrC nick( KNullDesC );

    if ( !iGroupObserverPC )
        {
        return;
        }

    if ( aMessage )
        {
        if ( aMessage->ContentType() == MCAMessage::EContentPicture )
            {
            iGroupObserverPC->HandleMessageError( aError );

            return;
            }

        nick.Set( iStoredContacts->Identification( aMessage->Recipient() ) );
        }

    iGroupObserverPC->HandleMessageError( aError, nick );
    }


// -----------------------------------------------------------------------------
// CCAGroupPC: SendMessageL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::SendMessageL( const TDesC& aMessageBuffer,
                               CDesCArrayFlat& aRecipients )
    {

    MCAMessageCreator& creator = iMessageUtils.MessageCreator();

    const MCAMessageContainerInfo& info = iReadInterface->TargetId();

    MCAMessageCreator::SMessageData data =
        {
        KMessageDataVersion,
        0,
        KNullDesC,
        KNullDesC,
        info.ScreenName(), // Sender
        info.Target(), // Recipient
        NULL,
        &aRecipients,
        aMessageBuffer,
        KNullDesC8,
        KNullDesC8,
        MCAMessage::EMessageSent
        };

    iWriteInterface->AppendL( creator.CreateMessageL( data ) );
    }


// -----------------------------------------------------------------------------
// CCAGroupPC: SetActiveGroupChatL
// -----------------------------------------------------------------------------
//
void CCAGroupPC::SetActiveGroupChatL( const TDesC& aGroupId )
    {
    if ( iGroupId )
        {
        delete iGroupId;

        iGroupId = NULL;
        }


    iGroupId = aGroupId.AllocL();

    iReadInterface =
        &( iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC, *iGroupId ) );

    iStoredGroup = iStoredGroups->FindGroup( *iGroupId );

    iGrOps = iGroupInterface->GroupOperationsL( *iGroupId );


    if ( !iGroupId )
        {
        iGroupId = KNullDesC().AllocL();
        }

    }

// -----------------------------------------------------------------------------
// CCAGroupPC::GetGroupPairArrayLC
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC* CCAGroupPC::GetGroupPairArrayLC()
    {
    RPointerArray<MCAStoredGroup> groupList;

    CDesCArray* groupNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupNames );
    CDesCArray* groupIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupIds );

    CleanupClosePushL( groupList );
    iStoredGroups->PopulateGroupsListL( groupList );

    TInt count( groupList.Count() );
    for ( TInt index( 0 ); index < count; index++ )
        {
        if ( 0 != groupList[ index ]->GroupId().CompareC( *iGroupId ) )
            {
            // other than current group
            groupNames->AppendL( groupList[ index ]->GroupName() );
            groupIds->AppendL( groupList[ index ]->GroupId() );
            }
        }
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); // groupList.Close()
    //Codescanner warning ignored
    CleanupStack::Pop( 2, groupNames );  //groupIds,groupNames

    CCAServerContactsArrayPC* pairsArray =
        CCAServerContactsArrayPC::NewL( groupNames, groupIds, KNullDesC );

    //Not owned. Transfer ownership of pairsArray to the caller.
    //since we return as MCAServerContactsArrayPC object sliciing will happen
    //so its better to push the original data onto stack
    CleanupStack::PushL( pairsArray );

    return pairsArray;
    }


// -----------------------------------------------------------------------------
// CCAGroupPC::IsActiveGroupChat
// -----------------------------------------------------------------------------
//
TBool CCAGroupPC::IsActiveGroupChat( const TDesC& aGroupId )

    {
    TBool retValue = EFalse;

    if ( iGroupId )
        {
        if ( !CAUtils::NeutralCompare( aGroupId, *iGroupId ) )
            {
            retValue = ETrue;
            }
        }

    return  retValue;
    }


//-----------------------------------------------------------
//CCAGroupPC::MessageCount
//-----------------------------------------------------------
TInt CCAGroupPC::MessageCount()
    {
    if ( iReadInterface )
        {
        return iReadInterface->MessageCount();
        }

    return 0;

    }

// End of File

