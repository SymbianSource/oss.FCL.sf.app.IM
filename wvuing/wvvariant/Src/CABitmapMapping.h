/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines the corresponding TAknsItemIds for bitmaps
*
*/



#ifndef CABITMAPMAPPING_H
#define CABITMAPMAPPING_H


// INCLUDES
#include <avkon.mbg>
#include <AknsConstants.h>

#include "impsbuilddefinitions.h"
#include "cacontexticonmapping.h"

// STRUCTURES
struct TCABitmapMapping
    {
    TInt iBitmapId;
    // must be pointer since msvc wont compile this if it's reference
    const TAknsItemID* iItemId;
    TInt iColorGroupId;
    };


// CONSTANTS

static const TCABitmapMapping KCABitmaps[] =
    {
    // the color groups are from Avkon Skin LAF

        { EMbmChatngQgn_prop_im_imsg_new,                             &KAknsIIDQgnPropImImsgNew,        EAknsCIQsnIconColorsCG1 },
    { EMbmChatngQgn_indi_im_watch_add,                            &KAknsIIDQgnIndiImWatchAdd,       EAknsCIQsnIconColorsCG13 },

    { EMenu_im_on,                                              &KAknsIIDQgnMenuImOn,               0 },
    { EMenu_im_on_title,                                        &KAknsIIDQgnPropImOnTitle,          0 },
    { EMenu_im_off,                                             &KAknsIIDQgnMenuImOffCxt,           0 },
    { EMenu_im_off_title,                                       &KAknsIIDQgnPropImOffTitle,         0 },
    { EMenu_im_away,                                            &KAknsIIDQgnMenuImAway,             0 },
    { EMenu_im_away_title,                                      &KAknsIIDQgnPropImAwayTitle,        0 },
    { EMenu_im_invisible,                                       &KAknsIIDQgnMenuImInvisible,        0 },
    { EMenu_im_invisible_title,                                 &KAknsIIDQgnPropImInvisibleTitle,   0 },
    { EMenu_im_busy,                                            &KAknsIIDQgnMenuImInvisible,        0 },
    { EMenu_im_busy_title,                                      &KAknsIIDQgnPropImBusyTitle,        0 },

// not in use
//  { EMbmChatngQgn_menu_im_lst,                                  &KAknsIIDQgnMenuImLst,            0 },
    { EMbmChatngQgn_prop_im_friends,                              &KAknsIIDQgnPropImFriends,        0 },
    { EMbmChatngQgn_prop_im_ibox,                                 &KAknsIIDQgnPropImIbox,           0 },
    { EMbmChatngQgn_prop_im_ibox_new,                             &KAknsIIDQgnPropImIboxNew,        0 },
    //{ EMbmChatngQgn_prop_im_imsg_new,                             &KAknsIIDQgnPropImImsgNew,            0 },
    { EMbmChatngQgn_prop_im_friend_off,                           &KAknsIIDQgnPropImFriendOff,      0 },
    { EMbmChatngQgn_prop_im_friend_on,                            &KAknsIIDQgnPropImFriendOn,       0 },
    { EMbmChatngQgn_prop_im_on_mobile_title,                      &KAknsIIDQgnPropImOnMobileTitle,  0 },
    { EMbmChatngQgn_prop_im_friend_away,                          &KAknsIIDQgnPropImFriendAway,     0 },
    { EMbmChatngQgn_prop_im_friend_busy,                          &KAknsIIDQgnPropImFriendBusy,     0 },
    { EMbmChatngQgn_prop_im_friend_invisible,                     &KAknsIIDQgnPropImFriendInvisible, 0 },
    { EMbmChatngQgn_prop_im_from,                                 &KAknsIIDQgnPropImFrom,           EAknsCIQsnIconColorsCG13 },
    { EMbmChatngQgn_prop_im_saved,                                &KAknsIIDQgnPropImSaved,          0 },
    { EMbmChatngQgn_prop_im_saved_chat_tab4,                      &KAknsIIDQgnPropImSavedChatTab4,  EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_friend_tab4,                          &KAknsIIDQgnPropImFriendTab4,     EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_ibox_tab4,                            &KAknsIIDQgnPropImIboxTab4,       EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_saved_chat_tab3,                      &KAknsIIDQgnPropImSavedChatTab3,  EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_friend_tab3,                          &KAknsIIDQgnPropImFriendTab3,     EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_ibox_tab3,                            &KAknsIIDQgnPropImIboxTab3,       EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_list_tab4,                            &KAknsIIDQgnPropImListTab4,       EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_im_saved_conv,                           &KAknsIIDQgnPropImSavedConv,      0 },
    { EMbmChatngQgn_prop_im_saved_chat,                           &KAknsIIDQgnPropImSavedChat,      0 },
    { EMbmChatngQgn_indi_marked_add,                              &KAknsIIDQgnIndiMarkedAdd,        EAknsCIQsnIconColorsCG13 },
    { EMbmChatngQgn_prop_im_user_created_active,                  &KAknsIIDQgnPropImUserCreatedActive, 0 },
    { EMbmChatngQgn_prop_im_user_created_active_unread,           &KAknsIIDQgnPropImUserCreatedActiveUnread, 0 },
    { EMbmChatngQgn_prop_im_nonuser_created_saved_active,         &KAknsIIDQgnPropImNonuserCreatedSavedActive, 0 },
    { EMbmChatngQgn_prop_im_nonuser_created_saved_active_unread,  &KAknsIIDQgnPropImNonuserCreatedSavedActiveUnread, 0 },
    { EMbmChatngQgn_prop_im_nonuser_created_saved_inactive,       &KAknsIIDQgnPropImNonuserCreatedSavedInactive, 0 },
    { EMbmChatngQgn_prop_im_joined_not_saved,                     &KAknsIIDQgnPropImJoinedNotSaved, 0 },
    { EMbmChatngQgn_prop_im_joined_not_saved_unread,              &KAknsIIDQgnPropImJoinedNotSavedUnread, 0 },
    { EMbmChatngQgn_prop_im_user_created_inactive,                &KAknsIIDQgnPropImUserCreatedInactive, 0 },
    { EMbmChatngQgn_prop_im_friends_collapsed,                    &KAknsIIDQgnPropImFriendsCollapsed,   0 },
    { EMbmChatngQgn_prop_im_friends_expanded,                     &KAknsIIDQgnPropImFriendsExpanded,    0 },
    { EMbmChatngQgn_prop_im_chats_collapsed,                      &KAknsIIDQgnPropImChatsCollapsed,   0 },
    { EMbmChatngQgn_prop_im_chats_expanded,                       &KAknsIIDQgnPropImChatsCollapsed,   0 },
    { EMbmChatngQgn_prop_im_new_invit,                            &KAknsIIDQgnPropImNewInvit,       0 },
    { EMbmChatngQgn_prop_im_user_blocked,                         &KAknsIIDQgnPropImUserBlocked,    0 },
    { EMbmChatngQgn_prop_im_imsg,                                 &KAknsIIDQgnPropImImsg,           0 },
    { EMbmChatngQgn_prop_im_list,                                 &KAknsIIDQgnPropImList,           0 },
    { EMbmChatngQgn_prop_im_list_unread,                          &KAknsIIDQgnPropImListUnread,     0 },
    { EMbmChatngQgn_prop_im_many,                                 &KAknsIIDQgnPropImMany,           0 },
    { EMbmChatngQgn_prop_im_tome,                                 &KAknsIIDQgnPropImTome,           EAknsCIQsnIconColorsCG13 },
    { EMbmChatngQgn_stat_im_uni,                                  &KAknsIIDQgnStatImUni,            EAknsCIQsnComponentColorsCG3 },
    { EMbmChatngQgn_indi_sett_protected_add,                      &KAknsIIDQgnIndiSettProtectedAdd, EAknsCIQsnComponentColorsCG6a },
    { EMbmChatngQgn_prop_image_corrupted,                         &KAknsIIDQgnPropImageCorrupted,   0 },
    { EMbmChatngQgn_prop_unknown,                                 &KAknsIIDQgnPropUnknown,          0 },
    { EMbmChatngQgn_prop_im_smileys_angry,                        &KAknsIIDQgnPropImSmileysAngry,   0 },
    { EMbmChatngQgn_prop_im_smileys_bored,                        &KAknsIIDQgnPropImSmileysBored,   0 },
    { EMbmChatngQgn_prop_im_smileys_crying,                       &KAknsIIDQgnPropImSmileysCrying,  0 },
    { EMbmChatngQgn_prop_im_smileys_glasses,                      &KAknsIIDQgnPropImSmileysGlasses, 0 },
    { EMbmChatngQgn_prop_im_smileys_happy,                        &KAknsIIDQgnPropImSmileysHappy,   0 },
    { EMbmChatngQgn_prop_im_smileys_indif,                        &KAknsIIDQgnPropImSmileysIndif,   0 },
    { EMbmChatngQgn_prop_im_smileys_kiss,                         &KAknsIIDQgnPropImSmileysKiss,    0 },
    { EMbmChatngQgn_prop_im_smileys_laugh,                        &KAknsIIDQgnPropImSmileysLaugh,   0 },
    { EMbmChatngQgn_prop_im_smileys_robot,                        &KAknsIIDQgnPropImSmileysRobot,   0 },
    { EMbmChatngQgn_prop_im_smileys_sad,                          &KAknsIIDQgnPropImSmileysSad,     0 },
    { EMbmChatngQgn_prop_im_smileys_shock,                        &KAknsIIDQgnPropImSmileysShock,   0 },
    { EMbmChatngQgn_prop_im_smileys_skeptical,                    &KAknsIIDQgnPropImSmileysSkeptical,   0 },
    { EMbmChatngQgn_prop_im_smileys_sleepy,                       &KAknsIIDQgnPropImSmileysSleepy,      0 },
    { EMbmChatngQgn_prop_im_smileys_sunglasses,                   &KAknsIIDQgnPropImSmileysSunglasses,  0 },
    { EMbmChatngQgn_prop_im_smileys_surprise,                     &KAknsIIDQgnPropImSmileysSurprise,    0 },
    { EMbmChatngQgn_prop_im_smileys_tired,                        &KAknsIIDQgnPropImSmileysTired,       0 },
    { EMbmChatngQgn_prop_im_smileys_veryhappy,                    &KAknsIIDQgnPropImSmileysVeryhappy,   0 },
    { EMbmChatngQgn_prop_im_smileys_verysad,                      &KAknsIIDQgnPropImSmileysVerysad,     0 },
    { EMbmChatngQgn_prop_im_smileys_wickedsmile,                  &KAknsIIDQgnPropImSmileysWickedsmile, 0 },
    { EMbmChatngQgn_prop_im_smileys_wink,                         &KAknsIIDQgnPropImSmileysWink,        0 },
    { EMbmChatngQgn_indi_phone_add,                               &KAknsIIDQgnIndiPhoneAdd,             EAknsCIQsnComponentColorsCG11 },

    // other than Chat icons
    { EMbmAvkonQgn_prop_empty,                                  &KAknsIIDQgnPropEmpty, 0 },

    // end of list
    { KErrNotFound, &KAknsIIDNone, KErrNotFound }

    };

#endif      // CABITMAPMAPPING_H

// End of File
