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
* Description:  Setting item for message colour
*
*/


#include <akncolourselectiongrid.h>
#include <akniconutils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <chatng.rsg>

#include "ccacoloursettingitem.h"
#include "MCASettingSapExt.h"
#include "ccaappsettingsdialog.h"
#include "chatdefinitions.h"
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCAColourSettingItem::CCAColourSettingItem(
    MCASettingSapExt& aSAPExtension,
    TBool& aBuffer,
    TInt aIdentifier )
        : CAknSettingItem( aIdentifier ),
        iExternalValue( aBuffer ),
        iSAPExtension( aSAPExtension )
    {
    // No implementation needed
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CCAColourSettingItem::ConstructL(
    const TDesC& aKey,
    const TInt aOrdinal,
    const TDesC& aSettingTitle,
    CArrayPtr<CGulIcon>* aIconArray )
    {
    iSAPKey = aKey.AllocL();
    CAknSettingItem::ConstructL(
        EFalse,
        aOrdinal,
        aSettingTitle,
        aIconArray,
        0,
        EAknCtPopupSettingList,
        0,
        0 );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCAColourSettingItem::~CCAColourSettingItem()
    {
    delete iDefaultText;
    delete iSAPKey;
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// @see CAknSettingItem
// ---------------------------------------------------------------------------
//
void CCAColourSettingItem::StoreL()
    {
    iExternalValue = iInternalValue;
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// @see CAknSettingItem
// ---------------------------------------------------------------------------
//
void CCAColourSettingItem::LoadL()
    {
    iInternalValue = iExternalValue;
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// EditItemL overridden to launch colour selection grid.
// ---------------------------------------------------------------------------
//
void CCAColourSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    TBool retVal = EFalse;
    CEikonEnv* env = CEikonEnv::Static();

    // Array for TRgb colour values
    CArrayFixFlat<TRgb>* colors = new( ELeave ) CArrayFixFlat<TRgb>( KArrayGranularity );
    CleanupStack::PushL( colors );

    // Read colours from resources
    CDesCArrayFlat* indices = env->ReadDesCArrayResourceL( R_MESSAGE_COLORS );
    CleanupStack::PushL( indices );

    // Convert to TRgb's
    TInt cnt = indices->Count();
    for ( TInt i = 0; i < cnt; ++i )
        {
        TInt col;
        TLex lex( indices->MdcaPoint( i ) );
        User::LeaveIfError( lex.Val( col ) );
        colors->AppendL( AKN_LAF_COLOR( col ) );
        }
    CleanupStack::PopAndDestroy( indices );

    // Get current colour
    TRgb color( iSAPExtension.IntValueL( *iSAPKey ) );
    TInt startValue = color.Value();
    TBool noneChosen = ( startValue == KErrNotFound );

    // Construct colour selection grid
    CAknDialog *dlg = CAknColourSelectionGrid::NewL(
                          colors,
                          ETrue,
                          noneChosen,
                          color );

    // Use own resource to customize the "none" text.
    retVal = dlg->ExecuteLD( R_CHATCLIENT_COLOR_GRID_DLG );

    TInt colVal = KErrNotFound;
    if ( !noneChosen )
        {
        colVal = color.Value();
        }

    if ( !retVal )
        {
        colVal = startValue;
        }
    else
        {
        // Set new value to SAP
        iSAPExtension.SetIntValueL( *iSAPKey, colVal );
        }

    CleanupStack::PopAndDestroy( colors );

    // ETrue: User specific color selected
    // EFalse: Default color used
    iInternalValue = ( colVal != KErrNotFound );

    UpdateListBoxTextL();
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// @see CAknSettingItem
// ---------------------------------------------------------------------------
//
const TDesC& CCAColourSettingItem::SettingTextL()
    {
    // When using user defined colour return empty
    // descriptor, otherwise return default text
    if ( iInternalValue )
        {
        return KEmptyDesC;
        }
    else
        {
        return *iDefaultText;
        }
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// @see CAknSettingItem
// ---------------------------------------------------------------------------
//
void CCAColourSettingItem::CompleteConstructionL()
    {
    // Check that correct ConstructL was used
    if ( !iSAPKey )
        {
        User::Panic( KPanicText, EChatColourSettingItemWronglyConstructed );
        }

    // Read default colour text from resources
    CEikonEnv* env = CEikonEnv::Static();
    iDefaultText = env->AllocReadResourceAsDes16L(
                       R_QTN_CHAT_SET_COLOUR_DEFAULT );
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// @see CAknSettingItem
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCAColourSettingItem::CreateBitmapL()
    {
    // If custom colour is in use create bitmap with that colour to be
    // shown inside the setting item

    // Create rect of correct size to be used for resizing the icon
    TRect origin;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, origin );

    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( origin,
                           AknLayoutScalable_Avkon::listscroll_gen_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(),
                           AknLayoutScalable_Avkon::list_gen_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(),
                           AknLayoutScalable_Avkon::list_setting_number_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(),
                           AknLayoutScalable_Avkon::set_value_pane( 0 ) );
    TRect rect( layoutRect.Rect() );

    // Move rect to 0,0
    rect.Move( -rect.iTl.iX, -rect.iTl.iY );

    // Get current color from SAP
    TRgb color( iSAPExtension.IntValueL( *iSAPKey ) );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    // Create icon
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KAvkonBitmapFile,
        EMbmAvkonQgn_prop_empty,
        EMbmAvkonQgn_prop_empty_mask
    );

    if ( iInternalValue )
        {
        // Set colour
        AknIconUtils::SetIconColor( bitmap, color );

        // Create solid mask to make
        // colour box visible
        delete mask;
        mask = NULL;
        CleanupStack::PushL( bitmap );
        mask = AknIconUtils::CreateIconL( KAvkonBitmapFile,
                                          EMbmAvkonQgn_prop_empty );
        CleanupStack::Pop( bitmap );    // No leaving code after this, CGulIcon
        // takes ownership in NewL
        }

    // Resize bitmaps
    AknIconUtils::SetSize( bitmap, rect.Size() );
    AknIconUtils::SetSize( mask, rect.Size() );

    // icon takes ownership of bitmaps
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    // Ownership of icon gets transferred
    CFbsBitmap* returnedBitmap = SetIconMaskAndReturnBitmap( icon );
    return returnedBitmap;  // Caller takes ownership
    }

// ---------------------------------------------------------------------------
// CCAColourSettingItem::SetIconMaskAndReturnBitmap
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCAColourSettingItem::SetIconMaskAndReturnBitmap( CGulIcon* aIcon )
    {
    CFbsBitmap* returnedBitmap = NULL;

    if ( aIcon )
        {
        // Drop ownership of bitmaps from icon
        aIcon->SetBitmapsOwnedExternally( ETrue );
        // Get bitmaps
        CFbsBitmap* bitmap = aIcon->Bitmap();
        CFbsBitmap* mask = aIcon->Mask();
        delete aIcon; // Does not own bitmaps anymore, so let it go

        // Always return the bitmap and transfer ownership if there is one
        if ( bitmap )
            {
            returnedBitmap = bitmap;
            }

        // Set the mask if there is a bitmap, an icon and a mask
        TBool maskOwnershipTransferred = EFalse;
        if ( bitmap && mask && HasIcon() )
            {
            maskOwnershipTransferred = SetIconMask( mask );
            }
        if ( !maskOwnershipTransferred )
            {
            // Ownership didn't transfer
            delete mask;
            }
        }

    return returnedBitmap;  // Caller takes ownership
    }
