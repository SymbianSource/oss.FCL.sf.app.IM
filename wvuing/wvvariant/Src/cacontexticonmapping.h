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
* Description:  Defines the corresponding context icons for bitmaps or SVGs
*
*/



#ifndef CACONTEXTICONMAPPING_H
#define CACONTEXTICONMAPPING_H


// INCLUDES
#include <chatNG.mbg>
#include <bldvariant.hrh>

// CONSTANTS

enum TCAContextBitmaps
    {
#ifdef __SCALABLE_ICONS
    EMenu_im_on             = EMbmChatngQgn_menu_im_on,
    EMenu_im_on_mask        = EMbmChatngQgn_menu_im_on_mask,
    EMenu_im_off            = EMbmChatngQgn_menu_im_off,
    EMenu_im_off_mask       = EMbmChatngQgn_menu_im_off_mask,
    EMenu_im_away           = EMbmChatngQgn_menu_im_away,
    EMenu_im_away_mask      = EMbmChatngQgn_menu_im_away_mask,
    EMenu_im_invisible      = EMbmChatngQgn_menu_im_invisible,
    EMenu_im_invisible_mask = EMbmChatngQgn_menu_im_invisible_mask,
    EMenu_im_busy           = EMbmChatngQgn_menu_im_busy,
    EMenu_im_busy_mask      = EMbmChatngQgn_menu_im_busy_mask,

    // Thin titlepane
    EMenu_im_on_title             = EMbmChatngQgn_prop_im_on_title,
    EMenu_im_on_title_mask        = EMbmChatngQgn_prop_im_on_title_mask,
    EMenu_im_off_title            = EMbmChatngQgn_prop_im_off_title,
    EMenu_im_off_title_mask       = EMbmChatngQgn_prop_im_off_title_mask,
    EMenu_im_away_title           = EMbmChatngQgn_prop_im_away_title,
    EMenu_im_away_title_mask      = EMbmChatngQgn_prop_im_away_title_mask,
    EMenu_im_invisible_title      = EMbmChatngQgn_prop_im_invisible_title,
    EMenu_im_invisible_title_mask = EMbmChatngQgn_prop_im_invisible_title_mask,
    EMenu_im_busy_title           = EMbmChatngQgn_prop_im_busy_title,
    EMenu_im_busy_title_mask      = EMbmChatngQgn_prop_im_busy_title_mask
#else
    EMenu_im_on             = EMbmChatngQgn_menu_im_on_cxt,
    EMenu_im_on_mask        = EMbmChatngQgn_menu_im_on_cxt_mask,
    EMenu_im_off            = EMbmChatngQgn_menu_im_off_cxt,
    EMenu_im_off_mask       = EMbmChatngQgn_menu_im_off_cxt_mask,
    EMenu_im_away           = EMbmChatngQgn_menu_im_away_cxt,
    EMenu_im_away_mask      = EMbmChatngQgn_menu_im_away_cxt_mask,
    EMenu_im_invisible      = EMbmChatngQgn_menu_im_invisible_cxt,
    EMenu_im_invisible_mask = EMbmChatngQgn_menu_im_invisible_cxt_mask,
    EMenu_im_busy           = EMbmChatngQgn_menu_im_busy_cxt,
    EMenu_im_busy_mask      = EMbmChatngQgn_menu_im_busy_cxt_mask,

    // Thin titlepane
    EMenu_im_on_title             = EMbmChatngQgn_prop_im_on_title,
    EMenu_im_on_title_mask        = EMbmChatngQgn_prop_im_on_title_mask,
    EMenu_im_off_title            = EMbmChatngQgn_prop_im_off_title,
    EMenu_im_off_title_mask       = EMbmChatngQgn_prop_im_off_title_mask,
    EMenu_im_away_title           = EMbmChatngQgn_prop_im_away_title,
    EMenu_im_away_title_mask      = EMbmChatngQgn_prop_im_away_title_mask,
    EMenu_im_invisible_title      = EMbmChatngQgn_prop_im_invisible_title,
    EMenu_im_invisible_title_mask = EMbmChatngQgn_prop_im_invisible_title_mask
    EMenu_im_busy_title           = EMbmChatngQgn_prop_im_busy_title,
    EMenu_im_busy_title_mask      = EMbmChatngQgn_prop_im_busy_title_mask
#endif
    };


#endif      // CACONTEXTICONMAPPING_H

// End of File
