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
* Description:  Creator class for views.
*
*/


// INCLUDES
#include    "CAViewCreator.h"
#include    "CCARecordedChatsListView.h"
#include    "CCARecordedChatsView.h"
#include    "CAExternalInterface.h"
#include    "CCAConversationsView.h"
#include    "CCASettingsView.h"
#include    "CCASearchView.h"
#include    "CCARefreshView.h"
#include    "CCAChatView.h"
#include 	"CCASingleListView.h"
#include 	"ccainvitationview.h"

#include    <ChatNG.rsg>

// -----------------------------------------------------------------------------
// CAViewCreator::CreateMainViewL
// Create all needed views
// -----------------------------------------------------------------------------
//
void CAViewCreator::CreateMainViewL( CCAAppUi& aAppUi )
    {
    CCASingleListView* view4 = new ( ELeave ) CCASingleListView;
    CleanupStack::PushL( view4 );
    view4->ConstructL( R_CHATCLIENT_FRIENDSLIST_VIEW, KUidFriendsListView );
    aAppUi.AddViewL( view4 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view4 );
    TVwsViewId aTempId = view4->ViewId();
    view4->ActivateViewL( aTempId );
    aAppUi.SetDefaultViewL( *view4 );
    }

// -----------------------------------------------------------------------------
// CAViewCreator::CreateOtherViewsL
// Create all needed views
// -----------------------------------------------------------------------------
//
void CAViewCreator::CreateOtherViewsL( CCAAppUi& aAppUi )
    {
    // Create chat-view
    CCAChatView* view2 = new ( ELeave ) CCAChatView;
    CleanupStack::PushL( view2 );
    // Chat-view has a specialized ConstructL instead of BaseConstructL
    view2->ConstructL( R_CHATCLIENT_CHAT_VIEW, KUidChatView, aAppUi.MessageHandler() );
    aAppUi.AddViewL( view2 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view2 );

    // Create settings view
    CCASettingsView* view6 = new ( ELeave ) CCASettingsView;
    CleanupStack::PushL( view6 );
    view6->ConstructL( R_CHATCLIENT_SETTINGS_VIEW, KUidSettingsView );
    aAppUi.AddViewL( view6 );
    CleanupStack::Pop( view6 );


    // Create search view
    CCASearchView* view7 = new ( ELeave ) CCASearchView;
    CleanupStack::PushL( view7 );
    view7->BaseConstructL( R_CHATCLIENT_SEARCH_VIEW, KUidSearchView );
    aAppUi.AddViewL( view7 );
    CleanupStack::Pop( view7 );

    // Conversations view
    CCAConversationsView* view9 = new ( ELeave ) CCAConversationsView;
    CleanupStack::PushL( view9 );
    view9->ConstructL( R_CHATCLIENT_P2P_VIEW, KUidConversationsView, aAppUi.MessageHandler() );
    aAppUi.AddViewL( view9 );
    CleanupStack::Pop( view9 );

    // Create recorded chats list-view
    CCARecordedChatsListView* view10 = new ( ELeave ) CCARecordedChatsListView;
    CleanupStack::PushL( view10 );
    // Chatlist-view has a specialized ConstructL instead of BaseConstructL
    view10->ConstructL( R_CHATCLIENT_RECORDEDCHATSLIST_VIEW , KUidRecordedChatListView );
    aAppUi.AddViewL( view10 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view10 );

    // Create recorded chats view
    CCARecordedChatsView* view11 = new ( ELeave ) CCARecordedChatsView;
    CleanupStack::PushL( view11 );
    // Chatlist-view has a specialized ConstructL instead of BaseConstructL
    view11->ConstructL( R_CHATCLIENT_RECORDEDCHATS_VIEW , KUidRecordedChatView );
    aAppUi.AddViewL( view11 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view11 );
    view11->SetRecordedChatProvider( view10->RecordedChatArrayProvider() );

    // View for branding refresh functionality
    CCARefreshView* view12 = new ( ELeave ) CCARefreshView( aAppUi.RefreshWaitFlag() );
    CleanupStack::PushL( view12 );
    view12->BaseConstructL( R_CHATCLIENT_REFRESHVIEW, KUidRefreshView );
    aAppUi.AddViewL( view12 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view12 );

    // View for Invitation
    CCAInvitationView* view14 = new ( ELeave ) CCAInvitationView;
    CleanupStack::PushL( view14 );
    view14->ConstructL( R_CHATCLIENT_INVITATIONS_VIEW, KUidInvitationView );
    aAppUi.AddViewL( view14 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view14 );
    }

// end of file
