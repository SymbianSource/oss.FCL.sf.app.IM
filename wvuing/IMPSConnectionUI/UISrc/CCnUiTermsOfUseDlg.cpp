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
* Description:  UI dialog for showing the Terms of use of a certain server
*
*/

#include    "ccnuitermsofusedlg.h"

#include    "CnUiCommon.hrh"

#include    <txtrich.h>
#include    <aknenv.h>
#include    <aknsettingcache.h>
#include    <bldvariant.hrh>
#include    <aknquerydialog.h>
#include    <stringloader.h>
#include    <eikmenup.h>
#include    <avkon.rsg>
#include    <avkon.hrh>
#include    <eikappui.h>
#include    <eikenv.h>
#include    <aknutils.h>
#include    <aknsutils.h>
#include    <hlplch.h>
#include    <featmgr.h>
#include    <aknsbasicbackgroundcontrolcontext.h>
#include    <akneditstateindicator.h>
#include    <eikrted.h> // CEikRichTextEditor
#include    <barsread.h>
#include    <applayout.cdl.h> //cdl
#include    <aknlayoutscalable_apps.cdl.h>
#include    <impsconnectionuing.rsg>
// item finder (Automatic highlight)
#include    <finditemmenu.rsg>
#include    <finditemmenu.h>
#include    <finditem.hrh>
#include    <finditemdialog.h>
// title pane handling
#include    <akntitle.h>
#include    <aknlists.h>
#include    <aknnavi.h>
#include    <utf.h>
#include    <txtetext.h>

#include    "impscommonuibuilddefinitions.h"

const TInt KTextMargin = 100;  // normal text margin in twips
const TInt KSpaceBefore = 10;  // space before each paragraph (twips)
const TInt KPixelsBetweenLines = 5; // pixels between text lines
const TInt KIMAcceptIndex = 1; // which item "Accept" is in the resource file

_LIT( KLineBreak, "\n" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// RunDialogLD
// ---------------------------------------------------------------------------
//
TInt CCnUiTermsOfUseDlg::RunDialogLD( TInt aResourceId )
    {
    TInt retVal;
    iRetVal = &retVal;

    ExecuteLD( aResourceId );

    return retVal;
    }

// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CCnUiTermsOfUseDlg* CCnUiTermsOfUseDlg::NewL( const TDesC& aToUFileName )
    {
    CCnUiTermsOfUseDlg* self = CCnUiTermsOfUseDlg::NewLC( aToUFileName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC()
// ---------------------------------------------------------------------------
//
CCnUiTermsOfUseDlg* CCnUiTermsOfUseDlg::NewLC( const TDesC& aToUFileName )
    {
    CCnUiTermsOfUseDlg* self = new( ELeave ) CCnUiTermsOfUseDlg;
    CleanupStack::PushL( self );
    self->ConstructL( aToUFileName );
    return self;
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::ConstructL()
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::ConstructL( const TDesC& aToUFileName )
    {
    CAknDialog::ConstructL( R_CNUI_TERMS_OF_USE_MENUBAR );

    // This dlg uses only context menu
    iMenuBar->SetMenuType( CEikMenuBar::EMenuContext );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                     KAknsIIDQsnBgAreaMainMessage, Rect(), ETrue );

    iFindItemMenu = CFindItemMenu::NewL( EIMPSCommonUIFindMenuPlaceHolder );
    iFindItemMenu->AttachItemFinderMenuL( R_CNUI_TERMS_OF_USE_MENUBAR );
    iFindItemMenu->HandleItemFinderCommandL( EFindItemCmdEnableAutoFind );

    iItemFinder = CItemFinder::NewL();
    iItemFinder->AddObserver( *this );

    iToUFileName = aToUFileName.AllocL();

    SetNewTitleTextL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCnUiTermsOfUseDlg::~CCnUiTermsOfUseDlg()
    {
    TRAP_IGNORE( RestoreTitlePaneTextL() );

    AknsUtils::DeregisterControlPosition( this );

    delete iBgContext;
    delete iItemFinder;
    delete iFindItemMenu;
    delete iOldTitleText;
    delete iToUFileName;
    }

// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CCnUiTermsOfUseDlg::CCnUiTermsOfUseDlg()
    {
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::ProcessCommandL( TInt aCommand )
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::ProcessCommandL( TInt aCommand )
    {
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) )
        {
        iFindItemMenu->HandleItemFinderCommandL( aCommand );
        return;
        }
    }

// -----------------------------------------------------------------------------
// CCnUiTermsOfUseDlg::DynInitMenuPaneL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiTermsOfUseDlg::DynInitMenuPaneL( TInt aResourceId,
                                           CEikMenuPane* aMenuPane )
    {
    switch ( aResourceId )
        {
        case R_CNUI_TERMS_OF_USE_MENU:
            {
            iFindItemMenu->AddItemFindMenuL( iItemFinder, aMenuPane,
                                             EIMPSCommonUIFindMenuPlaceHolder,
                                             KNullDesC ); // we can ignore the sender here

            // remove placeholder
            aMenuPane->SetItemDimmed( EIMPSCommonUIFindMenuPlaceHolder, ETrue );
            aMenuPane->SetItemDimmed( EFindItemCmdDisableAutoFind, ETrue );
            break;
            }
        default:
            {
            // update find menu
            iFindItemMenu->UpdateItemFinderMenuL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::HandleParsingComplete()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::HandleParsingComplete()
    {
    TRAPD( err, UpdateCbaL() );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::PreLayoutDynInitL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();
    AknsUtils::RegisterControlPosition( this );

    iEditor = static_cast<CEikRichTextEditor*>( Control( ETOURichEditorId ) );

    iParaFormatLayer = CParaFormatLayer::NewL();
    iEditor->SetParaFormatLayer( iParaFormatLayer );    // takes the ownership

    iCharFormatLayer = CCharFormatLayer::NewL();
    iEditor->SetCharFormatLayer( iCharFormatLayer );    // takes the ownership

    // Update text color from skin
    UpdateTextColorL();

    // Get ToU message
    HBufC* touText = ReadToUMessageLC();
    iEditor->SetTextL( touText );
    CleanupStack::PopAndDestroy( touText );

    iEditor->CreateScrollBarFrameL();
    // Only vertical scrollbar is needed
    iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    }

// -----------------------------------------------------------------------------
// CCnUiTermsOfUseDlg::PostLayoutDynInitL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiTermsOfUseDlg::PostLayoutDynInitL()
    {
    CAknDialog::PostLayoutDynInitL();

    // Insert paragraph delimiter to make
    // iItemFinder scroll the view all the way down
    TChar lb( CEditableText::EParagraphDelimiter );
    iEditor->Text()->InsertL( iEditor->TextLength(), lb );

    iItemFinder->SetEditor( &iEditor );

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiTermsOfUseDlg::SetSizeAndPosition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiTermsOfUseDlg::SetSizeAndPosition( const TSize& /*aSize*/ )
    {
    // dialog fills the mainpane
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    SetRect( mainPane );
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::OkToExitL( TInt aButtonId )
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCnUiTermsOfUseDlg::OkToExitL( TInt aButtonId )
    {
    switch ( aButtonId )
        {
        case EAknSoftkeyCancel:
            {
            if ( AskCancelConfirmationL() )
                {
                // user cancelled
                *iRetVal = EAknSoftkeyCancel;
                return ETrue;
                }
            return EFalse;
            }
        case EIMPSCommonUITOUContinue:
            {
            // Try to scroll down
            if ( iItemFinder->NextItemOrScrollL( CItemFinder::ENextDown ) )
                {
                // Not yet at bottom => return
                UpdateCbaL();
                return EFalse;
                }

            TInt retVal = AcceptTermsL();
            switch ( retVal )
                {
                case KIMAcceptIndex:
                    {
                    // user accepted the terms
                    *iRetVal = EAknSoftkeyOk;
                    return ETrue;
                    }

                case EAknSoftkeyCancel:
                    {
                    return EFalse;
                    }
                default:
                    {
                    // user did not accept the query
                    *iRetVal = EAknSoftkeyCancel;
                    return ETrue;
                    }
                }
            }
        case EAknSoftkeyContextOptions: // MSK command
            {
            DisplayMenuL();
            return EFalse;
            }
        case EAknSoftkeyEmpty:
            {
            return EFalse;
            }
        default:
            {
            return ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCnUiTermsOfUseDlg::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCnUiTermsOfUseDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    switch ( aKeyEvent.iCode )
        {
        case EKeyEnter: // flowthrough
        case EKeyOK:
            {
            CItemFinder::CFindItemExt& curItem = iItemFinder->CurrentItemExt();
            if ( curItem.iItemType != CItemFinder::ENoneSelected )
                {
                DisplayMenuL();
                }
            return EKeyWasConsumed;
            }
        case EKeyBackspace:
            {
            return EKeyWasConsumed;
            }

        case EKeyUpArrow:
            {
            TKeyResponse res =
                iItemFinder->NextItemOrScrollL( CItemFinder::ENextUp ) ?
                EKeyWasConsumed : EKeyWasNotConsumed;
            if ( res == EKeyWasConsumed )
                {
                UpdateCbaL();
                }
            return res;
            }

        case EKeyDownArrow:
            {
            TKeyResponse res =
                iItemFinder->NextItemOrScrollL( CItemFinder::ENextDown ) ?
                EKeyWasConsumed : EKeyWasNotConsumed;
            if ( res == EKeyWasConsumed )
                {
                UpdateCbaL();
                }
            return res;
            }

        case EKeyRightArrow:
            {
            return EKeyWasConsumed;
            }

        case EKeyEscape: // handle dialog shutdown.
            {
            TryExitL( EAknSoftkeyExit );
            return EKeyWasConsumed;
            }
        default:
            {
            return EKeyWasNotConsumed;
            }
        }
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::MopSupplyObject
//
// ---------------------------------------------------------
//

TTypeUid::Ptr CCnUiTermsOfUseDlg::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }


// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::SizeChanged
//
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::SizeChanged()
    {
    // base class method
    CAknDialog::SizeChanged();

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }

    if ( iEditor )
        {
        iEditor->SetRect( Rect() );
        }

    // Update the font to correct size
    if ( iParaFormatLayer && iCharFormatLayer )
        {
        // Correct font
        const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont );

        // format richtext
        TInt lineSpacing( font->HeightInPixels() + KPixelsBetweenLines );
        CParaFormat paraFormat;
        TParaFormatMask paraFormatMask;
        paraFormat.iLineSpacingControl =
            CParaFormat::ELineSpacingExactlyInPixels;
        paraFormatMask.SetAttrib( EAttLineSpacingControl );
        paraFormat.iLineSpacingInTwips = lineSpacing;
        paraFormatMask.SetAttrib( EAttLineSpacing );
        paraFormat.iLanguage =
            CAknEnv::Static()->SettingCache().InputLanguage();
        paraFormatMask.SetAttrib( EAttParaLanguage );
        paraFormat.iLeftMarginInTwips = KTextMargin;
        paraFormatMask.SetAttrib( EAttLeftMargin );
        paraFormat.iSpaceBeforeInTwips = KSpaceBefore;
        paraFormatMask.SetAttrib( EAttSpaceBefore );

        TCharFormat charFormat;
        TCharFormatMask charFormatMask;
        charFormat.iFontSpec = font->FontSpecInTwips();
        charFormatMask.SetAttrib( EAttFontTypeface );
        charFormatMask.SetAttrib( EAttFontHeight );

        //can't leave here
        TRAPD( err, iParaFormatLayer->SetL( &paraFormat, paraFormatMask );
               iCharFormatLayer->SetL( charFormat, charFormatMask );
               iEditor->HandleTextChangedL(); );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::AskCancelConfirmationL
//
// ---------------------------------------------------------
//
TInt CCnUiTermsOfUseDlg::AskCancelConfirmationL()
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( R_QTN_CHAT_CANCEL_TERMSOFUSE );
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    CleanupStack::PushL( query );
    query->SetPromptL( *prompt );
    CleanupStack::Pop( query ); //query
    TInt ret( query->ExecuteLD( R_GENERAL_QUERY ) );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return ret;
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::AcceptTermsL
//
// ---------------------------------------------------------
//
TInt CCnUiTermsOfUseDlg::AcceptTermsL()
    {
    TInt selection = -1;
    // CodeScanner warning "neglected to put variable on cleanup stack (id: 35)"
    // Ignored, because PrepareLC is called after this one
    CAknListQueryDialog* listQuery =
        new ( ELeave ) CAknListQueryDialog( &selection ); // CSI: 35 # see above
    listQuery->PrepareLC( R_CNUI_ACCEPT_TERMS_OF_USE_LISTQUERY );

    // Set item texts
    CDesCArray* listTexts = NULL;
    listTexts = CCoeEnv::Static()->ReadDesC16ArrayResourceL(
                    R_CNUI_ACCEPT_TERMS_OF_USE_LISTQUERY_ARRAY );
    listQuery->SetItemTextArray( listTexts );
    listQuery->SetOwnershipType( ELbmOwnsItemArray );

    if ( listQuery->RunLD() == EAknSoftkeyCancel )
        {
        return EAknSoftkeyCancel;
        }
    else
        {
        return selection;
        }
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::SetNewTitleText
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::SetNewTitleTextL()
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();

    // Store old one
    HBufC* tmp = title->Text()->AllocL();
    delete iOldTitleText;
    iOldTitleText = tmp;

    // Clear the navi pane
    // Get pointer to status-pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();
    // Get pointer to navi-pane
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    naviPane->PushDefaultL( ETrue );

    // Set the new text
    HBufC* tempTitle = iEikonEnv->AllocReadResourceLC( R_QTN_CHAT_TOU_TITLE );
    title->SetTextL( *tempTitle );
    CleanupStack::PopAndDestroy( tempTitle );
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::GetTitlePaneInstanceL
// Gets titlepane instance from CEikonEnv
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknTitlePane* CCnUiTermsOfUseDlg::GetTitlePaneInstanceL() const
    {
    CAknTitlePane* title = static_cast< CAknTitlePane* >
                           ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL(
                                 TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if ( !title )
        {
        User::Leave( KErrNotSupported );
        }
    return title;
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::DoRestoreTitlePaneTextL() const
// Does the actual restoring.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::RestoreTitlePaneTextL() const
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();

    if ( iOldTitleText )
        {
        title->SetTextL( *iOldTitleText );
        }
    else
        {
        title->SetTextToDefaultL();
        }

    // restore the navi pane
    // Get pointer to status-pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();
    // Get pointer to navi-pane
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    naviPane->Pop();
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::UpdateCbaL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::UpdateCbaL()
    {
    TInt cbaRes = R_CNUI_TERMS_OF_USE_DIALOG_CBA;
    CItemFinder::CFindItemExt& curItem = iItemFinder->CurrentItemExt();

    if ( curItem.iItemType != CItemFinder::ENoneSelected )
        {
        cbaRes = R_CNUI_SOFTKEYS_CONTINUE_CANCEL__CONTEXTOPTIONS;
        }

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( cbaRes );
    cba->DrawNow();
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::UpdateTextColorL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::UpdateTextColorL()
    {
    if ( !iEditor )
        {
        return;
        }

    // Get text color from skin
    TRgb textColor( KRgbBlack );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               textColor,
                               KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG6 );
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontPresentation.iTextColor = textColor;
    charFormatMask.SetAttrib( EAttColor );

    // Apply format
    int length = iEditor->RichText()->DocumentLength();
    iEditor->RichText()->SetInsertCharFormatL( charFormat, charFormatMask, length );
    iEditor->RichText()->ApplyCharFormatL( charFormat, charFormatMask, 0, length );
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::HandleResourceChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiTermsOfUseDlg::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        // When skin changes, we need to update text color
        TRAP_IGNORE( UpdateTextColorL();
                     iEditor->HandleTextChangedL(); );
        }
    CAknDialog::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CCnUiTermsOfUseDlg::ReadToUMessageLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCnUiTermsOfUseDlg::ReadToUMessageLC()
    {
    // Open file
    RFile handle;
    RFs& rfsSession = CCoeEnv::Static()->FsSession();
    User::LeaveIfError( handle.Open( rfsSession , *iToUFileName, EFileRead ) );
    CleanupClosePushL( handle );

    // Read tou message
    TInt size = -1;
    User::LeaveIfError( handle.Size( size ) );
    HBufC8* touText = HBufC8::NewLC( size );
    TPtr8 touMsg( touText->Des() );
    User::LeaveIfError( handle.Read( touMsg ) );

    // Convert result to HBufC*
    HBufC* temp = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *touText );
    CleanupStack::PopAndDestroy( 2 ); // toutext, handle
    CleanupStack::PushL( temp );

    // Convert '\n' linebreaks to CEditableText::ELineBreak type line breaks
    TPtr ptr( temp->Des() );
    AknTextUtils::ReplaceCharacters( ptr, KLineBreak, CEditableText::ELineBreak );

    return temp;
    }

// End of file
