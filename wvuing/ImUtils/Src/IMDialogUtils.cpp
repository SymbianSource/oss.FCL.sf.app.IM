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
* Description:  Utility class for dialogs
*
*/



// INCLUDE FILES
#include    "IMDialogUtils.h"
#include    "ccatextquerydialog.h"
#include    "ChatDebugPrint.h"
#include    "ChatDebugAssert.h"
#include    "ChatDefinitions.h"
#include    <aknlists.h>
#include    <aknpopup.h>
#include	<aknnotedialog.h>
#include    <aknmessagequerydialog.h>
#include    <aknnotewrappers.h>
#include    <stringloader.h>
#include    <ChatNG.rsg>

// ---------------------------------------------------------
// IMDialogUtils::DisplayInformationNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMDialogUtils::DisplayInformationNoteL( const TDesC& aText )
    {
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( aText );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayInformationNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMDialogUtils::DisplayInformationNoteL( TInt aTextResource,
                                                      const TDesC& aParameter )
    {
    HBufC* text = NULL;
    if ( aParameter == KNullDesC )
        {
        text = CEikonEnv::Static()->AllocReadResourceLC( aTextResource );
        }
    else
        {
        text = StringLoader::LoadLC( aTextResource, aParameter );
        }

    DisplayInformationNoteL( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayPopupListL()
// Displays popup list
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMDialogUtils::DisplayPopupListL( MDesCArray* aItemList,
                                                const TInt aTitleResourceId,
                                                TListBoxModelItemArrayOwnership aOwnerShipType,
                                                const TInt aEmptyListPrompt /*=-1*/
                                              )
    {
    CAknSinglePopupMenuStyleListBox* listBox =
        new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );
    CAknPopupList* popUpList = CAknPopupList::NewL( listBox,
                                                    R_AVKON_SOFTKEYS_OK_EMPTY, AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popUpList );
    listBox->ConstructL( popUpList,
                         CEikListBox::ELeftDownInViewRect | CEikListBox::EDisableHighlight );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );


    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( aItemList );
    model->SetOwnershipType( aOwnerShipType );

    if ( aEmptyListPrompt != KErrNotFound )
        {
        HBufC* emptyText = CEikonEnv::Static()->AllocReadResourceLC(
                               aEmptyListPrompt );
        popUpList->ListBox()->View()->SetListEmptyTextL( *emptyText );
        CleanupStack::PopAndDestroy( emptyText );
        }
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC(
                        aTitleResourceId );
    popUpList->SetTitleL( *prompt );
    CleanupStack::PopAndDestroy( prompt );


    popUpList->ExecuteLD();
    CleanupStack::Pop( popUpList );
    CleanupStack::PopAndDestroy( listBox );

    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayPopupListL()
// Displays popup list
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMDialogUtils::DisplayPopupListL( TInt& aSelectedIndex, MDesCArray* aItemList,
                                                const TDesC& aTitle,
                                                TListBoxModelItemArrayOwnership aOwnerShipType,
                                                const TInt aEmptyListPrompt  /*=-1*/	)
    {
    CAknSinglePopupMenuStyleListBox* listBox =
        new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );
    CAknPopupList* popUpList = CAknPopupList::NewL( listBox,
                                                    R_AVKON_SOFTKEYS_SELECT_BACK, AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popUpList );
    listBox->ConstructL( popUpList,
                         CEikListBox::ELeftDownInViewRect/*|CEikListBox::EDisableHighlight */ );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );


    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( aItemList );
    model->SetOwnershipType( aOwnerShipType );

    if ( aEmptyListPrompt != KErrNotFound )
        {
        HBufC* emptyText = CEikonEnv::Static()->AllocReadResourceLC(
                               aEmptyListPrompt );
        popUpList->ListBox()->View()->SetListEmptyTextL( *emptyText );
        CleanupStack::PopAndDestroy( emptyText );
        }

    popUpList->SetTitleL( aTitle );



    TInt ret = popUpList->ExecuteLD();
    CleanupStack::Pop( popUpList );

    if ( ret )
        {
        aSelectedIndex = listBox->CurrentItemIndex();
        }

    CleanupStack::PopAndDestroy( listBox );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayTextQueryDialogL()
// Displays text query dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayTextQueryDialogL(
    TDes& aDataText,
    const TInt aTitleResourceId,
    const TInt aDialogResourceId,
    TBool aPredictiveInput /* = EFalse */,
    TBool aLeftSoftkeyAlwaysVisible /* = EFalse */,
    TLeftSoftkey aLeftSoftkey /* = EDefault */,
    TInitialSelectionMode aSelectionMode /* = ESelectAll */,
    TBool aDomainSelectionQuery /* = EFalse */ )
    {
    // CodeScanner warning ignored because CS does not
    // notice that PrepareLC puts the dialog to cleanupstack
    CAknTextQueryDialog* dlg = CCATextQueryDialog::NewL( aDataText, // CSI: 35 # See comment above
                                                         CAknQueryDialog::ENoTone,
                                                         aLeftSoftkeyAlwaysVisible,
                                                         aDomainSelectionQuery );

    dlg->PrepareLC( aDialogResourceId );
    HBufC* text = CEikonEnv::Static()->AllocReadResourceLC( aTitleResourceId );
    dlg->SetPromptL( *text );
    dlg->SetPredictiveTextInputPermitted( aPredictiveInput );

    CleanupStack::PopAndDestroy( text );

    // Change cba
    if ( aLeftSoftkey == ESend )
        {
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL( R_CHAT_INVITE_CBA_BUTTONS_SEND );
        }

    // Set correct selection mode
    switch ( aSelectionMode )
        {
        case ESelectNone:
            {
            CCoeControl* ctrl = dlg->ControlOrNull( EGeneralQuery );
            if ( ctrl )
                {
                CAknQueryControl* query = static_cast<CAknQueryControl*>( ctrl );
                ctrl = query->ControlByLayoutOrNull( EDataLayout );
                if ( ctrl )
                    {
                    CEikEdwin* edwin = static_cast<CEikEdwin*>( ctrl );
                    edwin->AddFlagToUserFlags( CEikEdwin::ENoAutoSelection );
                    }
                }
            break;
            }
        default:
            {
            // Use default flags in edwin
            break;
            }
        }
    return ( dlg->RunLD() );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayMessageQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayMessageQueryDialogL( const TInt aDialogResourceId,
                                                         const TDesC& aMsgText,
                                                         const TDesC& aHeaderText,
                                                         TBool aPredictiveInput )
    {
    // CodeScanner warning ignored because CS does not
    // notice that PrepareLC puts the dialog to cleanupstack
    CAknMessageQueryDialog* dlg = new( ELeave )CAknMessageQueryDialog(); // CSI: 35 # See comment above
    dlg->PrepareLC( aDialogResourceId );
    if ( aMsgText != KNullDesC )
        {
        dlg->SetMessageTextL( aMsgText );
        }
    if ( aHeaderText != KNullDesC )
        {
        dlg->SetHeaderTextL( aHeaderText );
        }
    dlg->SetPredictiveTextInputPermitted( aPredictiveInput ); // T9
    return ( dlg->RunLD() );
    }

// ---------------------------------------------------------
// IMDialogUtils::MultiselectionListQueryDialogLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CDesCArray* IMDialogUtils::MultiselectionListQueryDialogL(
    const MDesCArray& aItemList,
    const TDesC& aTitle )
    {
    TInt count( aItemList.MdcaCount() );
    if ( count == 0 )
        {
        // empty list
        return NULL;
        }

    // array for indices
    CArrayFixFlat<TInt>* selectedIndices =
        new( ELeave ) CArrayFixFlat<TInt>( count );
    CleanupStack::PushL( selectedIndices );

    // append icon formatting
    CDesCArray* lst = new( ELeave )CDesCArrayFlat( count );
    CleanupStack::PushL( lst );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC item( aItemList.MdcaPoint( i ) );
        HBufC* text = HBufC::NewMaxLC( item.Length() +
                                       KIconTabulator().Length() );
        TPtr txt( text->Des() );
        txt.Copy( KIconTabulator );
        txt.Append( item );

        CHAT_DP( D_CHAT_LIT( "appending '%S' to list" ), &txt );

        lst->AppendL( *text );
        CleanupStack::PopAndDestroy( text );
        }

    // show the query
    TInt retVal( IMDialogUtils::DisplayListQueryDialogL(
                     selectedIndices, lst, R_CHATVIEW_RECIPIENT_QUERY, NULL,
                     KNullDesC, aTitle ) );
    // CAknListQueryDialog returns 0 if user cancels the query
    if ( !retVal )
        {
        // query cancelled
        CleanupStack::PopAndDestroy( 2, selectedIndices ); // lst, selectedIndices
        return NULL;
        }

    // selected items array
    CDesCArray* selected = NULL;

    count = selectedIndices->Count();
    if ( count == 0 )
        {
        // nothing selected, return empty array
        CleanupStack::PopAndDestroy( 2, selectedIndices ); // lst, selectedIndices
        selected = new( ELeave )CDesCArrayFlat( 1 );
        }
    else
        {
        // specified items selected
        selected = new( ELeave )CDesCArrayFlat( count );
        CleanupStack::PushL( selected );
        for ( TInt i( 0 ); i < count; ++i )
            {
            // take out the icon formatting ("0\t")
            TPtrC item( lst->MdcaPoint( selectedIndices->At( i ) ).Mid(
                            KIconTabulator().Length() ) );
            selected->AppendL( item );
            CHAT_DP( D_CHAT_LIT( "selected %S from list" ), &item );
            }
        CleanupStack::Pop( selected );
        CleanupStack::PopAndDestroy( 2, selectedIndices ); // lst, selectedIndices
        }
    return selected;
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayListQueryDialogL()
// This method displays ListQueryDialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayListQueryDialogL( CArrayFix< TInt >* aIndexArray,
                                                      MDesCArray* aItemList,
                                                      const TInt aDialogResourceId,
                                                      CArrayFix< TInt >* aArrayOfSelectionIndexes /*NULL*/,
                                                      const TDesC& aListEmptyText /*= KNullDesC*/,
                                                      const TDesC& aAlternativeTitle /*= KNullDesC*/ )
    {
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( aIndexArray );
    dlg->PrepareLC( aDialogResourceId );
    dlg->SetItemTextArray( aItemList );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( aArrayOfSelectionIndexes )
        {
        dlg->ListBox()->SetSelectionIndexesL( aArrayOfSelectionIndexes );
        if ( aArrayOfSelectionIndexes->Count() == 1 )
            {
            dlg->ListBox()->SetCurrentItemIndexAndDraw(
                aArrayOfSelectionIndexes->At( 0 ) );
            }

        }
    if ( aItemList->MdcaCount() == 0 )
        {
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL( R_AVKON_SOFTKEYS_CANCEL );
        }

    if ( aListEmptyText != KNullDesC )
        {
        dlg->ListBox()->View()->SetListEmptyTextL( aListEmptyText );
        }

    if ( aAlternativeTitle != KNullDesC )
        {
        dlg->SetHeaderTextL( aAlternativeTitle );
        }

    return ( dlg->RunLD() );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayListQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayListQueryDialogL( TInt* aSelectedIndex,
                                                      MDesCArray* aItemList,
                                                      const TInt aDialogResourceId,
                                                      const TDesC& aAlternativeTitle /*KNullDesC*/,
                                                      const TDesC& aListEmptyText /*= KNullDesC*/,
                                                      const TBool aIsForwardQuery /* EFalse */,
                                                      const TBool aIsOnlySelect /* = EFalse */ )

    {
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog(
        aSelectedIndex );
    dlg->PrepareLC( aDialogResourceId );
    dlg->SetItemTextArray( aItemList );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( aAlternativeTitle != KNullDesC )
        {
        dlg->SetHeaderTextL( aAlternativeTitle );
        }

    if ( aItemList->MdcaCount() == 0 )
        {
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL( R_AVKON_SOFTKEYS_CANCEL );
        }
    else
        {
        if ( aIsForwardQuery )
            {
            CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
            cba.SetCommandSetL( R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT );
            }
        }

    if ( aIsOnlySelect )
        {
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL( R_CHAT_SOFTKEYS_SELECT_EMPTY );
        }

    dlg->ListBox()->SetCurrentItemIndex( *aSelectedIndex );

    if ( aListEmptyText != KNullDesC )
        {
        dlg->ListBox()->View()->SetListEmptyTextL( aListEmptyText );
        }

    return ( dlg->RunLD() );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayNoteDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayNoteDialogL( const TInt aResourceId )
    {
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( CAknNoteDialog::
                                                         EConfirmationTone, CAknNoteDialog::EShortTimeout );
    return dlg->ExecuteLD( aResourceId );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayNoteDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayNoteDialogL( const TDesC& aText,
                                                 const TInt aButtonsResourceId, /*= R_AVKON_SOFTKEYS_EMPTY*/
                                                 const CAknNoteDialog::TTimeout& aTimeout /*= CAknNoteDialog::EShortTimeout*/ )
    {
    // CodeScanner warning ignored because CS does not
    // notice that PrepareLC puts the dialog to cleanupstack
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( // CSI: 35 # See comment above
        CAknNoteDialog::EConfirmationTone , aTimeout );

    dlg->PrepareLC( R_CHAT_INFO_NOTE_TEMPLATE );
    dlg->SetTextL( aText );

    CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
    cba.SetCommandSetL( aButtonsResourceId );

    return dlg->RunLD();
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayErrorNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayErrorNoteL( const TInt aResourceId )
    {
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog(
        CAknNoteDialog::EErrorTone , CAknNoteDialog::ELongTimeout );

    return dlg->ExecuteLD( aResourceId );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayErrorNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayErrorNoteL( const TDesC& aText, TBool aSoftkeyOk /*= EFalse*/ )
    {
    if ( aSoftkeyOk )
        {
        return IMDialogUtils::DisplayQueryDialogL( R_CHAT_ERROR_NOTE_TEMPLATE_WITH_OK, aText );
        }
    else
        {
        // CodeScanner warning ignored because CS does not
        // notice that PrepareLC puts the dialog to cleanupstack
        CAknNoteDialog* dlg =   // CSI: 35 # See comment above
            new ( ELeave ) CAknNoteDialog( CAknNoteDialog::EErrorTone ,
                                           CAknNoteDialog::ELongTimeout );
        dlg->PrepareLC( R_CHAT_ERROR_NOTE_TEMPLATE );
        dlg->SetTextL( aText );
        return dlg->RunLD();
        }
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayQueryDialogL( TInt aDialogResourceId,
                                                  TInt aTextResourceId )
    {
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC( aTextResourceId );
    TInt ret( DisplayQueryDialogL( aDialogResourceId, *prompt ) );
    CleanupStack::PopAndDestroy( prompt );

    return ret;
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayQueryDialogL( TInt aDialogResourceId,
                                                  const TDesC& aPrompt )
    {
    CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
        CAknQueryDialog::ENoTone );
    if ( aPrompt != KNullDesC )
        {
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( aPrompt );
        CleanupStack::Pop( dlg );
        }

    return dlg->ExecuteLD( aDialogResourceId );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayYesNoConfirmationDialogL( TInt aTextResourceId )
    {
    return IMDialogUtils::DisplayQueryDialogL(
               R_GENERIC_YES_NO_CONFIRMATION_QUERY,
               aTextResourceId );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayQueryDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayYesNoConfirmationDialogL( const TDesC& aPrompt )
    {
    return IMDialogUtils::DisplayQueryDialogL(
               R_GENERIC_YES_NO_CONFIRMATION_QUERY,
               aPrompt );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayOKErrorNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayErrorConfirmationNoteL( TInt aTextResourceId )
    {
    return IMDialogUtils::DisplayQueryDialogL(
               R_GENERIC_OK_ERROR_NOTE,
               aTextResourceId );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayErrorConfirmationNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayErrorConfirmationNoteL( const TDesC& aPrompt )
    {
    return IMDialogUtils::DisplayQueryDialogL(
               R_GENERIC_OK_ERROR_NOTE,
               aPrompt );
    }

// ---------------------------------------------------------
// IMDialogUtils::DisplayConfirmationNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMDialogUtils::DisplayConfirmationNoteL( TInt aTextResourceId )
    {
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC( aTextResourceId );
    CAknConfirmationNote* dlg = new( ELeave )CAknConfirmationNote();
    TInt ret = dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    return ret;
    }

//  End of File
