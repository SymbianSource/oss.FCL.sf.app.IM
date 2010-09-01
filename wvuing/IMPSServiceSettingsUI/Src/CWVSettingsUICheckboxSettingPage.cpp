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
* Description:  Checkbox Setting page
*
*/

// INCLUDE FILES
#include    "CWVSettingsUICheckboxSettingPage.h"
#include    <cwvsettingsuing.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWVSettingsUICheckboxSettingPage* CWVSettingsUICheckboxSettingPage::NewL(
    TInt aResourceId,
    CSelectionItemList* aSelectionItemList )
    {
    CWVSettingsUICheckboxSettingPage* self = new( ELeave )
    CWVSettingsUICheckboxSettingPage( aResourceId, aSelectionItemList );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->SetSettingPageObserver( self );
    self->SetCbaL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::HandleSettingPageEventL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUICheckboxSettingPage::HandleSettingPageEventL( CAknSettingPage* /*aSettingPage*/,
                                                                TAknSettingPageEvent aEventType )
    {
    if ( aEventType == EEventSettingChanged )
        {
        SetCbaL();
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::OfferKeyEventL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CWVSettingsUICheckboxSettingPage::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // Store current selection
    TInt oldInd = ListBoxControl()->CurrentItemIndex();
    // Offer event to base class
    TKeyResponse retVal = CAknCheckBoxSettingPage::OfferKeyEventL( aKeyEvent,
                                                                   aType );
    // Get new index
    TInt curInd = ListBoxControl()->CurrentItemIndex();
    if ( curInd != oldInd )
        {
        // CBA update needed
        SetCbaL();
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::HandlePointerEventL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUICheckboxSettingPage::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    // Deliver event to base class
    CAknCheckBoxSettingPage::HandlePointerEventL( aPointerEvent );
    // CBA update may be needed
    SetCbaL();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUICheckboxSettingPage::CWVSettingsUICheckboxSettingPage( TInt aResourceId,
                                                                    CSelectionItemList* aSelectionItemList )
        : CAknCheckBoxSettingPage( aResourceId, aSelectionItemList ),
        iSelectionItemList( aSelectionItemList )
    {
    }

// -----------------------------------------------------------------------------
// CWVSettingsUICheckboxSettingPage::SetCbaL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUICheckboxSettingPage::SetCbaL()
    {
    TInt count = iSelectionItemList->Count();
    TBool anySelected( EFalse );
    // check if any item is selected
    for ( TInt index( 0 ); index < count; index++ )
        {
        if ( iSelectionItemList->At( index )->SelectionStatus() )
            {
            // atleast one item selected
            anySelected = ETrue;
            break;
            }
        }

    // change the CBA
    TInt resId = R_AVKON_SOFTKEYS_OK_CANCEL;
    if ( !anySelected )
        {
        // if no items are selected we must hide the "ok"-key
        resId = R_WVSETTINGSVIEW_SOFTKEYS_EMPTY_CANCEL__MARK;
        }
    else
        {
        TInt curSel = ListBoxControl()->CurrentItemIndex();
        TBool curSelected = iSelectionItemList->At( curSel )->SelectionStatus();
        if ( curSelected )
            {
            // OK/Cancel/Unmark
            resId = R_WVSETTINGSVIEW_SOFTKEYS_OK_CANCEL__UNMARK;
            }
        else
            {
            // OK/Cancel/Mark
            resId = R_WVSETTINGSVIEW_SOFTKEYS_OK_CANCEL__MARK;
            }
        }

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( resId );
    cba->DrawNow();
    DrawNow();
    }

// Destructor
CWVSettingsUICheckboxSettingPage::~CWVSettingsUICheckboxSettingPage()
    {
    }

//  End of File
