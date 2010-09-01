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
* Description:  Domain selection query pop-up
*
*/


#include <akniconutils.h>
#include <avkon.mbg>
#include <cimpssapsettings.h>
#include <cimpssapsettingsstore.h>
#include <cimpssapsettingslist.h>

#include "ccnuidomainselectionquerydialog.h"

_LIT( KListItemFormat, "%d\t%S" );
_LIT( KIMPreviousDomainIndex, "KIMPreviousDomainIndex" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::CCnUiDomainSelectionQueryDialog
// ---------------------------------------------------------------------------
//
CCnUiDomainSelectionQueryDialog::CCnUiDomainSelectionQueryDialog(
    TInt* aIndex, CIMPSSAPSettings* aSap )
        : CAknListQueryDialog( aIndex ),
        iOwnershipType( ELbmOwnsItemArray ),
        iSap( aSap ),
        iSapOwned( EFalse )
    {
    // No implementation needed
    }

// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::ConstructL()
    {
    TPtrC avkonBmpPath( AknIconUtils::AvkonIconFileName() );

    // Create icons for radio button and empty A-column icon
    AknIconUtils::CreateIconL(
        iRadioButton,
        iRadioButtonMask,
        avkonBmpPath,
        EMbmAvkonQgn_indi_radiobutt_on,
        EMbmAvkonQgn_indi_radiobutt_on_mask );

    AknIconUtils::CreateIconL(
        iEmpty,
        iEmptyMask,
        avkonBmpPath,
        EMbmAvkonQgn_indi_radiobutt_off,
        EMbmAvkonQgn_indi_radiobutt_off_mask );

    // Try to find stored SAP, needed for accessing previously
    // selected domain index when query is displayed again after
    // user has emptied user id field in id & password query.
    // If stored SAP is not found iSap is used directly.
    if ( iSap )
        {
        CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

        // Because logged in SAP has UID of zero, we must
        // find the correct UID manually from list
        TInt index = KErrNotFound;
        TInt retVal = sapList->FindNameL( iSap->SAPName(), index );

        if ( index != KErrNotFound && retVal != KErrNotFound )
            {
            // Found it, use it
            CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();

            TUint32 sapUid = sapList->UidForIndex( index );
            sapStore->GetSAPL( sapUid, storedSap );

            iSap = storedSap;
            iSapOwned = ETrue;
            CleanupStack::Pop( storedSap );
            }

        CleanupStack::PopAndDestroy( 2, sapStore );
        }
    }


// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::NewL
// ---------------------------------------------------------------------------
//
CCnUiDomainSelectionQueryDialog* CCnUiDomainSelectionQueryDialog::NewL(
    TInt* aIndex,
    CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    CCnUiDomainSelectionQueryDialog* self =
        new ( ELeave ) CCnUiDomainSelectionQueryDialog( aIndex, aSap );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::~CCnUiDomainSelectionQueryDialog
// ---------------------------------------------------------------------------
//
CCnUiDomainSelectionQueryDialog::~CCnUiDomainSelectionQueryDialog()
    {
    delete iRadioButton;
    delete iRadioButtonMask;
    delete iEmpty;
    delete iEmptyMask;

    if ( iOwnershipType == ELbmOwnsItemArray )
        {
        delete iItemTextArray;
        }

    if ( iSapOwned )
        {
        delete iSap;
        }
    }

// ---------------------------------------------------------------------------
// From class CAknListQueryDialog.
// CCnUiDomainSelectionQueryDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCnUiDomainSelectionQueryDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// From class CAknListQueryDialog.
// CCnUiDomainSelectionQueryDialog::PreLayoutDynInitL
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::PreLayoutDynInitL()
    {
    // First call PreLayoutDynInitL from base class
    CAknListQueryDialog::PreLayoutDynInitL();

    TInt prevIndex = 0;

    // Get previous selected domain from sap
    if ( iSap )
        {
        TInt sapError;
        sapError = iSap->GetOpaqueInt( KIMPreviousDomainIndex, prevIndex );

        if ( sapError != KErrNotFound && sapError != KErrNone )
            {
            User::Leave( sapError );
            }
        }

    // Check prevIndex for array boundaries
    // If resources are changed prevIndex may go out of bounds
    if ( prevIndex > ( ListBox()->Model()->NumberOfItems() - 1 ) )
        {
        prevIndex = 0;
        }

    UpdateRadioButtonL( prevIndex );
    ListBox()->SetCurrentItemIndex( prevIndex );
    }

// ---------------------------------------------------------------------------
// From class CAknListQueryDialog.
// CCnUiDomainSelectionQueryDialog::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CCnUiDomainSelectionQueryDialog::OkToExitL( TInt aButtonId )
    {
    // Check reaction of query to button id
    TBool exit = CAknListQueryDialog::OkToExitL( aButtonId );

    if ( exit )
        {
        TInt itemIndex = ListBox()->CurrentItemIndex();

        // Update radio button before exiting
        UpdateRadioButtonL( itemIndex, ETrue );

        // Update selected domain index to SAP
        if ( iSap )
            {
            CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
            CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
            sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

            // Because logged in SAP has UID of zero, we must
            // find the correct UID manually from list
            TInt index = KErrNotFound;
            TInt retVal = sapList->FindNameL( iSap->SAPName(), index );

            if ( index != KErrNotFound && retVal != KErrNotFound )
                {
                // Found it, update the correct SAP
                CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();

                TUint32 sapUid = sapList->UidForIndex( index );
                sapStore->GetSAPL( sapUid, storedSap );
                storedSap->SetOpaqueInt( KIMPreviousDomainIndex, itemIndex );
                sapStore->UpdateOldSAPL( storedSap, sapUid );

                CleanupStack::PopAndDestroy( storedSap );
                }
            else
                {
                iSap->SetOpaqueInt( KIMPreviousDomainIndex, itemIndex );
                }

            CleanupStack::PopAndDestroy( 2, sapStore );
            }
        }

    return exit;
    }

// ---------------------------------------------------------------------------
// From class CAknListQueryDialog.
// CCnUiDomainSelectionQueryDialog::SetItemTextArray
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::SetItemTextArray( MDesCArray* aItemTextArray )
    {
    iItemTextArray = aItemTextArray;

    // Create array with icon indexes
    TRAP_IGNORE(
        TInt itemCount = iItemTextArray->MdcaCount();

        CDesCArray* indexedArray = new ( ELeave ) CDesCArrayFlat( itemCount );
        CleanupStack::PushL( indexedArray );

        for ( TInt i = 0; i < itemCount; ++i )
    {
    TPtrC item = iItemTextArray->MdcaPoint( i );
        HBufC* indexedItem = HBufC::NewLC( item.Length() + KListItemFormat().Length() );
        TPtr ptr( indexedItem->Des() );
        ptr.Format( KListItemFormat, i, &item );
        indexedArray->AppendL( *indexedItem );
        CleanupStack::PopAndDestroy( indexedItem );
        }

    // Set indexed array for CAknListQueryDialog
    CAknListQueryDialog::SetItemTextArray( indexedArray );
    CAknListQueryDialog::SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( indexedArray );
    ); // TRAP_IGNORE
    }

// ---------------------------------------------------------------------------
// From class CAknListQueryDialog.
// CCnUiDomainSelectionQueryDialog::SetOwnershipType
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::SetOwnershipType(
    TListBoxModelItemArrayOwnership aOwnershipType )
    {
    iOwnershipType = aOwnershipType;
    }

// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::UpdateRadioButtonL
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::UpdateRadioButtonL(
    TInt aSelectedIndex,
    TBool aForceUpdate /*= EFalse*/ )
    {
    TInt itemCount = iItemTextArray->MdcaCount();

    CArrayPtr<CGulIcon>* iconsArray = new ( ELeave ) CArrayPtrFlat<CGulIcon>( itemCount );
    CleanupStack::PushL( TCleanupItem( DestroyCloseArray, iconsArray ) );

    for ( TInt i = 0; i < itemCount; ++i )
        {
        CGulIcon* icon = NULL;

        if ( i == aSelectedIndex )
            {
            icon = CGulIcon::NewL( iRadioButton, iRadioButtonMask );
            }
        else
            {
            icon = CGulIcon::NewL( iEmpty, iEmptyMask );
            }

        // Keep ownership of bitmaps
        icon->SetBitmapsOwnedExternally( ETrue );
        CleanupStack::PushL( icon );
        iconsArray->AppendL( icon );    // Ownership transferred
        CleanupStack::Pop( icon );
        }

    // Set icons to query
    SetIconArrayL( iconsArray );
    CleanupStack::Pop( iconsArray );    // Ownership transferred

    if ( aForceUpdate )
        {
        // Call DrawNow from list box to avoid flickering of dialog
        ListBox()->DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CCnUiDomainSelectionQueryDialog::DestroyCloseArray
// ---------------------------------------------------------------------------
//
void CCnUiDomainSelectionQueryDialog::DestroyCloseArray( TAny* aObject )
    {
    reinterpret_cast< CArrayPtr<CGulIcon>* >( aObject )->ResetAndDestroy();
    }

