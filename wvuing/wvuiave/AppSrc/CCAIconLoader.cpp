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
* Description:  Icon loader
*
*/


// INCLUDES
#include    "impsbuilddefinitions.h"
#include    "CCAIconLoader.h"
#include    "CAExternalInterface.h"
#include    "ChatDefinitions.h"
#include    "CCAApp.h"
#include    "CCAAppUi.h"
#include    "CCAVariantFactory.h"
#include    "CCAUISessionManager.h"
#include    "MCASkinVariant.h"

#include    <e32base.h>
#include    <gulicon.h>
#include    <chatNG.mbg>
#include    <akntabgrp.h>
#include    <akniconarray.h>


// -----------------------------------------------------------------------------
// CCAIconLoader::CCAIconLoader
// Constructor
// -----------------------------------------------------------------------------
//
CCAIconLoader::CCAIconLoader( const TDesC& aMbmFullPath,
                              MCASkinVariant& aSkinVariant )
        : iMbmFullPath( aMbmFullPath ), iSkinVariant( aSkinVariant )
    {
    iAppUi = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );
    }

// -----------------------------------------------------------------------------
// CCAIconLoader::NewL
// Symbian style constructor
// -----------------------------------------------------------------------------
//
CCAIconLoader* CCAIconLoader::NewL( const TDesC& aMbmFullPath,
                                    MCASkinVariant& aSkinVariant )
    {
    CCAIconLoader* self = new ( ELeave ) CCAIconLoader( aMbmFullPath,
                                                        aSkinVariant );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAIconLoader::LoadIconsLC
// Load all needed icons identified by view id.
// -----------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CCAIconLoader::LoadIconsLC( TUid aViewIdentifier )
    {
    // Create array for listbox graphics
    CArrayPtr<CGulIcon>* icons =
        new ( ELeave ) CAknIconArray( KArrayGranularity );
    CleanupStack::PushL( icons );

    if ( aViewIdentifier == KUidMainView )
        {
        CGulIcon* ibox = iSkinVariant.LoadBitmapL( EMbmChatngQgn_prop_im_ibox,
                                                   EMbmChatngQgn_prop_im_ibox_mask,
                                                   iMbmFullPath );

        CleanupStack::PushL( ibox );
        icons->AppendL( ibox );
        CleanupStack::Pop( ibox );

        CGulIcon* friends = iSkinVariant.LoadBitmapL( EMbmChatngQgn_prop_im_friends,
                                                      EMbmChatngQgn_prop_im_friends_mask,
                                                      iMbmFullPath );

        CleanupStack::PushL( friends );
        icons->AppendL( friends );
        CleanupStack::Pop( friends );

        CGulIcon* imList = iSkinVariant.LoadBitmapL( EMbmChatngQgn_prop_im_list,
                                                     EMbmChatngQgn_prop_im_list_mask,
                                                     iMbmFullPath );
        CleanupStack::PushL( imList );
        icons->AppendL( imList );
        CleanupStack::Pop( imList );

        CGulIcon* recList = iSkinVariant.LoadBitmapL(  EMbmChatngQgn_prop_im_saved,
                                                       EMbmChatngQgn_prop_im_saved_mask,
                                                       iMbmFullPath );

        CleanupStack::PushL( recList );
        icons->AppendL( recList );
        CleanupStack::Pop( recList );

        CGulIcon* iboxNew = iSkinVariant.LoadBitmapL( EMbmChatngQgn_prop_im_ibox_new,
                                                      EMbmChatngQgn_prop_im_ibox_new_mask,
                                                      iMbmFullPath );

        CleanupStack::PushL( iboxNew );
        icons->AppendL( iboxNew );
        CleanupStack::Pop( iboxNew );

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
        CGulIcon* imListNew = iSkinVariant.LoadBitmapL(
                                  EMbmChatngQgn_prop_im_list_unread,
                                  EMbmChatngQgn_prop_im_list_unread_mask,
                                  iMbmFullPath );
        CleanupStack::PushL( imListNew );
        icons->AppendL( imListNew );
        CleanupStack::Pop( imListNew );
#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return icons;
    }

// -----------------------------------------------------------------------------
// CCAIconLoader::LoadStatusPaneIconsL
// Load status pane icons.
// -----------------------------------------------------------------------------
//
void CCAIconLoader::LoadStatusPaneIconsL( CAknTabGroup& aTabGroup )
    {
    CCAUISessionManager* session = &( iAppUi->UISessionManager() );
    TBool groups( session ? session->IsSupported(
                      CCAUISessionManager::EGroup ) : EFalse );

    // load icons
    CGulIcon* friendsList = NULL;
    CGulIcon* privateChatList = NULL;
    CGulIcon* chatList = NULL;
    CGulIcon* recordedList = NULL;

    if ( groups )
        {
        friendsList = iSkinVariant.LoadBitmapL(
                          EMbmChatngQgn_prop_im_friend_tab4,
                          EMbmChatngQgn_prop_im_friend_tab4_mask, iMbmFullPath );
        CleanupStack::PushL( friendsList );

        privateChatList = iSkinVariant.LoadBitmapL(
                              EMbmChatngQgn_prop_im_ibox_tab4,
                              EMbmChatngQgn_prop_im_ibox_tab4_mask, iMbmFullPath );
        CleanupStack::PushL( privateChatList );

        chatList = iSkinVariant.LoadBitmapL(
                       EMbmChatngQgn_prop_im_list_tab4,
                       EMbmChatngQgn_prop_im_list_tab4_mask, iMbmFullPath );
        CleanupStack::PushL( chatList );

        recordedList = iSkinVariant.LoadBitmapL(
                           EMbmChatngQgn_prop_im_saved_chat_tab4,
                           EMbmChatngQgn_prop_im_saved_chat_tab4_mask, iMbmFullPath );
        CleanupStack::PushL( recordedList );
        }
    else
        {
        friendsList = iSkinVariant.LoadBitmapL(
                          EMbmChatngQgn_prop_im_friend_tab3,
                          EMbmChatngQgn_prop_im_friend_tab3_mask, iMbmFullPath );
        CleanupStack::PushL( friendsList );

        privateChatList = iSkinVariant.LoadBitmapL(
                              EMbmChatngQgn_prop_im_ibox_tab3,
                              EMbmChatngQgn_prop_im_ibox_tab3_mask, iMbmFullPath );
        CleanupStack::PushL( privateChatList );

        CleanupStack::PushL( chatList );

        recordedList = iSkinVariant.LoadBitmapL(
                           EMbmChatngQgn_prop_im_saved_chat_tab3,
                           EMbmChatngQgn_prop_im_saved_chat_tab3_mask, iMbmFullPath );
        CleanupStack::PushL( recordedList );
        }

    aTabGroup.SetTabFixedWidthL(
        groups ? KTabWidthWithFourTabs : KTabWidthWithThreeTabs );

    aTabGroup.AddTabL( KUidChatPrivateChatListView.iUid,
                       privateChatList->Bitmap(), privateChatList->Mask() );
    privateChatList->SetBitmapsOwnedExternally( ETrue );

    aTabGroup.AddTabL( KUidFriendsListView.iUid,
                       friendsList->Bitmap(), friendsList->Mask() );
    friendsList->SetBitmapsOwnedExternally( ETrue );

    if ( groups )
        {
        aTabGroup.AddTabL( KUidChatListView.iUid,
                           chatList->Bitmap(), chatList->Mask() );
        chatList->SetBitmapsOwnedExternally( ETrue );
        }

    aTabGroup.AddTabL( KUidRecordedChatListView.iUid, recordedList->Bitmap(),
                       recordedList->Mask() );
    recordedList->SetBitmapsOwnedExternally( ETrue );

    // recorded, chatList, privateChatlist, friendsList
    CleanupStack::PopAndDestroy( 4, friendsList );
    }

// end of file
