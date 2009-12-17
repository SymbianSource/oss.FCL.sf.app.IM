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
* Description:  Build definitions to compile CCB in 3.0 platform
 *
*/



#ifndef IM30COMPATIBILITY_H
#define IM30COMPATIBILITY_H

// ---------------------------------------------------------------------------
// FEATURE DEFINITIONS
// ---------------------------------------------------------------------------
#define RD_30_COMPATIBILITY_MODE
#define RD_30_DISABLE_TOUCH
#define RD_30_DISABLE_MSK

// ---------------------------------------------------------------------------
// ICONS
// ---------------------------------------------------------------------------
//  Copy following graphics to S60 Icons and Bitmaps2 with masks
//  + qgn_prop_im_friend_busy
//  + qgn_prop_im_busy_title
//  + qgn_menu_im_busy          (svg)


// Define unknown SkinID's
#define KAknsIIDQgnPropImOnMobileTitle          KAknsIIDNone // CSI: 8 # Can't use upper case
#define KAknsIIDQgnPropImFriendBusy             KAknsIIDNone // CSI: 8 # Can't use upper case
#define KAknsIIDQgnPropImBusyTitle              KAknsIIDNone // CSI: 8 # Can't use upper case
#define KAknsIIDQgnMenuImBusy                   KAknsIIDNone // CSI: 8 # Can't use upper case

// ---------------------------------------------------------------------------
// MSK VARIATION / DOWNGRADE
// ---------------------------------------------------------------------------
// AppUi construction
#define EAknEnableMSK                           0 // CSI: 8 # Can't use upper case

// General avkon softkeys
#ifndef R_AVKON_SOFTKEYS_OK_EMPTY__OK
#define R_AVKON_SOFTKEYS_OK_EMPTY__OK           R_AVKON_SOFTKEYS_OK_EMPTY
#endif

#ifndef R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT
#define R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT  R_AVKON_SOFTKEYS_SELECT_CANCEL
#endif

#ifndef R_AVKON_SOFTKEYS_OK_CANCEL__OK
#define R_AVKON_SOFTKEYS_OK_CANCEL__OK          R_AVKON_SOFTKEYS_OK_CANCEL
#endif

#ifndef R_AVKON_SOFTKEYS_YES_NO__YES
#define R_AVKON_SOFTKEYS_YES_NO__YES            R_AVKON_SOFTKEYS_YES_NO
#endif

// Msk commands
#ifndef EAknSoftkeyContextOptions
// KMinTInt
#define EAknSoftkeyContextOptions               0x80000000 // CSI: 8 # Can't use upper case
#endif

// ---------------------------------------------------------------------------
// TOUCH SUPPORT
// ---------------------------------------------------------------------------

// When you need to use AknLayoutUtils::PenEnabled() use
// CATouchUtils::PenEnabled()


#endif // IM30COMPATIBILITY_H

// End of File

