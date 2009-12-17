/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Setting item creator
*
*/


// INCLUDE FILES
#include "CWVSettingsUISettingItemCreator.h"
#include "CWVSettingsUIDialogImp.h"
#include "WVSettingsUIPanics.h"
#include "CWVSettingsUIDefs.h"

#include	<cwvsettingsUING.rsg>
#include	<aknsettingitemlist.h>
#include	<aknsettingpage.h>
#include    <featmgr.h>
#ifdef RD_MULTIPLE_DRIVE
#include	<CAknMemorySelectionSettingItemMultiDrive.h>
#endif

#include 	"variantkeys.h"
#include 	<centralrepository.h>
#include 	<barsread.H>
// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWVSettingsUISettingItemCreator::CreateSettingItemListL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknSettingItemArray* CWVSettingsUISettingItemCreator::CreateSettingItemListL( TBool aCASettings,
                                                                               TDes& aServerInUse,
                                                                               TInt& aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                                                               TDes& aMemorySelectionType,
#endif
                                                                               TDes& aConnectionDaysIM,
                                                                               TDes& aConnectionHoursIM )
    {
    // granularity = 6, since there can be 6 items, and we start the numbering from 1
    CAknSettingItemArray* settingItemArray = settingItemArray =
                                                 new ( ELeave ) CAknSettingItemArray( 7, EFalse, 1 );
    CleanupStack::PushL( settingItemArray );

    // count for cleanupstack pop
    TInt count( 0 );

    // add setting items

    if ( aCASettings )
        {

        settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                       CWVSettingsUIDialogImp::EWVSettingsCASettings,
                                       R_WVSETTINGSVIEW_SETTING_PREFERENCES_TITLE,
                                       0,
                                       0,
                                       aServerInUse,
                                       aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                       aMemorySelectionType,
#endif
                                       aConnectionDaysIM,
                                       aConnectionHoursIM ) );
        count++;
        }

    // edit servers
    settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                   CWVSettingsUIDialogImp::EWVSettingsServers,
                                   R_WVSETTINGSVIEW_SETTING_EDITSERVER_TITLE,
                                   0,
                                   0,
                                   aServerInUse,
                                   aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                   aMemorySelectionType,
#endif
                                   aConnectionDaysIM,
                                   aConnectionHoursIM ) );
    count++;
    // server in use
    settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                   CWVSettingsUIDialogImp::EWVSettingsServerInUse,
                                   R_WVSETTINGSVIEW_SETTING_SERVERINUSE_TITLE,
                                   0,
                                   0,
                                   aServerInUse,
                                   aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                   aMemorySelectionType,
#endif
                                   aConnectionDaysIM,
                                   aConnectionHoursIM ) );
    count++;

    settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                   CWVSettingsUIDialogImp::EWVSettingsIMLoginType,
                                   R_WVSETTINGSVIEW_SETTING_IM_LOGINTYPE_TITLE,
                                   R_WVSETTINGSVIEW_SET_LOGIN,
                                   R_WVSETTINGSVIEW_SET_IM_LOGIN_TEXTS,
                                   aServerInUse,
                                   aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                   aMemorySelectionType,
#endif
                                   aConnectionDaysIM,
                                   aConnectionHoursIM ) );
    count++;

#ifdef RD_MULTIPLE_DRIVE
    // Memory Selection
    settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                   CWVSettingsUIDialogImp::EWVSettingsSaveChatsTo,
                                   R_WVSETTINGSVIEW_SETTING_MEMORY_SELECTION_TYPE,
                                   0,
                                   0,
                                   aServerInUse,
                                   aImLoginType,
                                   aMemorySelectionType,
                                   aConnectionDaysIM,
                                   aConnectionHoursIM ) );
    count++;

#endif

    if ( ( aImLoginType == EWVSettingsChatLoginAutoAlways ) ||
         ( aImLoginType == EWVSettingsChatLoginAutoInHomeNW ) )
        {
        // PEC login type automatic, we have to show the scheduling setting items also
        settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                       CWVSettingsUIDialogImp::EWVSettingsIMConnDays,
                                       R_WVSETTINGSVIEW_SETTING_PEC_CONN_ALLDAYS_TITLE,
                                       0,
                                       0,
                                       aServerInUse,
                                       aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                       aMemorySelectionType,
#endif
                                       aConnectionDaysIM,
                                       aConnectionHoursIM ) );
        count++;

        settingItemArray->AppendL( CWVSettingsUISettingItemCreator::CreateSettingItemLC(
                                       CWVSettingsUIDialogImp::EWVSettingsIMConnHours,
                                       R_WVSETTINGSVIEW_SETTING_PEC_CONN_ALLHOURS_TITLE,
                                       0,
                                       0,
                                       aServerInUse,
                                       aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                       aMemorySelectionType,
#endif
                                       aConnectionDaysIM,
                                       aConnectionHoursIM ) );
        count++;
        }

    CleanupStack::Pop( count ); // setting items
    CleanupStack::Pop( settingItemArray );
    return settingItemArray;
    }

// ---------------------------------------------------------
// CWVSettingsUISettingItemCreator::CreateSettingItemLC()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknSettingItem* CWVSettingsUISettingItemCreator::CreateSettingItemLC( TInt aIndex,
                                                                       TInt aResourceId,
                                                                       TInt aSettingPageResourceId,
                                                                       TInt aAssosiatedResource,
                                                                       TDes& aServerInUse,
                                                                       TInt& aImLoginType,
#ifdef RD_MULTIPLE_DRIVE
                                                                       TDes& aMemorySelectionType,
#endif
                                                                       TDes& aConnectionDaysIM,
                                                                       TDes& aConnectionHoursIM )
    {
    CAknSettingItem* settingItem = NULL;
    HBufC* itemTitle = NULL;

    CEikonEnv* eikonEnv = CEikonEnv::Static();
    switch ( aIndex )
        {
        case CWVSettingsUIDialogImp::EWVSettingsCASettings:
            {
            settingItem = new ( ELeave ) CAknBigSettingItemBase( 0 );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, NULL );
            break;
            }

        case CWVSettingsUIDialogImp::EWVSettingsServers:
            {
            settingItem = new ( ELeave ) CAknBigSettingItemBase( 0 );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, NULL );
            break;
            }
        case CWVSettingsUIDialogImp::EWVSettingsServerInUse:
            {
            settingItem = new ( ELeave ) CAknTextSettingItem( 0,  aServerInUse );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->SetEmptyItemTextL( KEmptyDesC );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, EEikCtEdwin );
            break;
            }
        case CWVSettingsUIDialogImp::EWVSettingsIMLoginType:
            {
            settingItem = new ( ELeave ) CAknEnumeratedTextPopupSettingItem( 0,  aImLoginType );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle,
                                     NULL, aSettingPageResourceId,
                                     EAknCtPopupField, NULL, aAssosiatedResource );
            break;
            }

#ifdef RD_MULTIPLE_DRIVE
        case CWVSettingsUIDialogImp::EWVSettingsSaveChatsTo:
            {
            settingItem = new ( ELeave ) CAknTextSettingItem( 0,  aMemorySelectionType );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, EEikCtEdwin );
            break;
            }

#endif

        case CWVSettingsUIDialogImp::EWVSettingsIMConnDays:
            {
            settingItem = new ( ELeave ) CAknTextSettingItem( 0, aConnectionDaysIM );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->SetEmptyItemTextL( KEmptyDesC );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, EEikCtEdwin );
            break;
            }


        case CWVSettingsUIDialogImp::EWVSettingsIMConnHours:
            {
            settingItem = new ( ELeave ) CAknTextSettingItem( 0,  aConnectionHoursIM );
            CleanupStack::PushL( settingItem );
            itemTitle = eikonEnv->AllocReadResourceLC( aResourceId );
            settingItem->SetEmptyItemTextL( KEmptyDesC );
            settingItem->ConstructL( EFalse, aIndex, *itemTitle, NULL, NULL, EEikCtEdwin );
            break;
            }

        default:
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy( itemTitle );

    return settingItem;
    }

// ---------------------------------------------------------
// CWVSettingsUISettingItemCreator::SchdeulingSupported()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUISettingItemCreator::SchedulingSupported()
    {
    return ETrue;
    }

//  End of File
