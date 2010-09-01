/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Synchronous client group notify mediator.
*
*/


// INCLUDE FILES
#include <E32std.h>
#include "CCnUiClientGroupUiNotifyMediator.h"

#include "CnUiCommon.hrh"
#include "CnUiPanics.h"
#include "IMPSCommonUiDebugPrint.h"



const TInt KClientGroupQueryTimeout = 30000000;    //30 seconds, defined in UI specification



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiClientGroupUiNotifyMediator* CCnUiClientGroupUiNotifyMediator::NewLC()
    {
    CCnUiClientGroupUiNotifyMediator* self = new ( ELeave ) CCnUiClientGroupUiNotifyMediator();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiClientGroupUiNotifyMediator::~CCnUiClientGroupUiNotifyMediator()
    {
    //cancel the self
    Cancel();
    CancelActiveChannels();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientGroupUiNotifyMediator::CCnUiClientGroupUiNotifyMediator()
        : CTimer( CActive::EPriorityUserInput )
    {
    }


// EPOC default constructor can leave.
void CCnUiClientGroupUiNotifyMediator::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::CGQActiveConnectionScheduledCloseL()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientGroupUiNotifyMediator::CGQActiveConnectionSuppressForScheduledConnectionL(
    const TDesC& aServerName,
    const TArray<TIMPSConnectionClient>& aClientsToShow )
    {
    return IssueTheQueryL( ECGQActiveConnectionSuppressForScheduledConnection, //RQ id from hrh
                           aServerName,
                           KClientGroupQueryTimeout,
                           EFalse,       //default result for timeout & deletion cases
                           aClientsToShow );
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::CGQActiveConnectionScheduledCloseL()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientGroupUiNotifyMediator::CGQActiveConnectionScheduledCloseL(
    const TArray<TIMPSConnectionClient>& aClientsToShow )
    {
    return IssueTheQueryL( ECGQActiveConnectionScheduledClose, //RQ id from hrh
                           KNullDesC,
                           KClientGroupQueryTimeout,
                           ETrue,       //default result for timeout & deletion cases
                           aClientsToShow );

    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::IssueTheQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientGroupUiNotifyMediator::IssueTheQueryL(
    TInt aQueryMessageId,
    const TDesC& aDynamicInsertText,
    TTimeIntervalMicroSeconds32 aQueryTimeout,
    TBool aDefaultResult,
    const TArray<TIMPSConnectionClient>& aClientsToShow )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrInUse ) );
    CancelActiveChannels();

    //issue client messages & time out notification
    IssueMessageToGroupsL( aQueryMessageId,
                           aDynamicInsertText,
                           aClientsToShow );
    After( aQueryTimeout );

    //wait the query to complete
    WaitCompletion();

    //get the completion result
    TBool queryResult = aDefaultResult;
    if ( iStatus == KErrNone )
        {
        queryResult = iQueryResult;
        }

    //and cancel all activity
    Cancel();               //cancel the time out timer
    CancelActiveChannels(); //cancel still active channels

    return queryResult;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::HandleChannelMsg()
// MCnUiGroupChannelListener
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::HandleChannelMsg( TInt aGroupId,
                                                         TGCChannelID aChannelId,
                                                         TInt aChannelMsg )
    {
    IMPSCUI_DP_TXT( "CCnUiClientGroupUiNotifyMediator::HandleChannelMsg()" );
    //there might come other messages from note cancellations etc.
    //those are ignored and only accept/declines are handled
    //==>query will timeout if no accept / decline is received
    if ( aChannelMsg == ERemoteQueryAccepted )
        {
        RemoveFromActiveChannels( aGroupId, aChannelId );
        iQueryResult = ETrue;
        iStatus = KErrNone;
        Completed();
        }

    else if ( aChannelMsg == ERemoteQueryDeclined )
        {
        RemoveFromActiveChannels( aGroupId, aChannelId );
        iQueryResult = EFalse;
        iStatus = KErrNone;
        Completed();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::RunL
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::RunL()
    {
    IMPSCUI_DP_TXT( "CCnUiClientGroupUiNotifyMediator::RunL() Timed out" );
    iStatus = KErrTimedOut;
    Completed();
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::DoCancel
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::DoCancel()
    {
    IMPSCUI_DP_TXT( "CCnUiClientGroupUiNotifyMediator::DoCancel()" );
    CTimer::DoCancel();
    iStatus = KErrCancel;
    Completed();
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::RunError
// -----------------------------------------------------------------------------
//
TInt CCnUiClientGroupUiNotifyMediator::RunError( TInt /*aError */ )
    {
    iStatus = KErrGeneral;
    Completed();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::WaitCompletion()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::WaitCompletion()
    {
    //wait with the scheduler loop
    IMPSCUI_DP_TXT( "CCnUiClientGroupUiNotifyMediator::WaitCompletion() - starting wait" );
    if ( !iWait.IsStarted() )
        {
        // Code scanner warning "active object called without checking
        // whether it is active or cancelling it first" ignored because
        // CActiveSchedulerWait is not an active object
        iWait.Start(); // CSI: 10 # See above
        }
    IMPSCUI_DP_TXT( "CCnUiClientGroupUiNotifyMediator::WaitCompletion() - wait done" );
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::Completed()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::Completed()
    {
    //break away from the waiting scheduler loop
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::IssueMessageToGroupsL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::IssueMessageToGroupsL(
    TInt aRemoteMessage,
    const TDesC& aExtraData,
    const TArray<TIMPSConnectionClient>& aClientGroups )
    {
    const TInt groupCount = aClientGroups.Count();
    for ( TInt ii( 0 ); ii < groupCount; ++ii  )
        {
        //issue the message to channel and listen further events from that channel
        TIMPSConnectionClient clientId = aClientGroups[ ii ];
        CCnUiGroupChannel* gc = CCnUiGroupChannel::NewLC( clientId,
                                                          ECnUiRemoteUiNotificationsChannel );

        gc->WriteL( aRemoteMessage, aExtraData );
        gc->ListenL( this );
        User::LeaveIfError( iGroupChannels.Append( gc ) );
        CleanupStack::Pop( gc ); //gc
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::CancelActiveChannels()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::CancelActiveChannels()
    {
    //issue the cancel to all channels
    const TInt channelCount = iGroupChannels.Count();
    for ( TInt ii( 0 ); ii < channelCount; ++ii  )
        {
        TInt ignore;
        TRAP( ignore, iGroupChannels[ ii ]->WriteL( ERemoteCancelAllActivity ) );
        }

    //and destroy those
    iGroupChannels.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyMediator::RemoveFromActiveChannels()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyMediator::RemoveFromActiveChannels( TInt aGroupId,
                                                                 TGCChannelID aChannelId )
    {
    //given channel is tried to remove from group of active channels
    //==> go downwards from the last index and remove all matching ones
    TInt ii = ( iGroupChannels.Count() - 1 ); //last index is 1 smaller than total count
    for ( ; ii >= 0; ii-- )
        {
        CCnUiGroupChannel* gc = iGroupChannels[ ii ];
        if ( ( gc->GroupID() == aGroupId ) && ( gc->ChannelId() == aChannelId ) )
            {
            iGroupChannels.Remove( ii );
            delete gc;
            }
        }
    }


// end of file
