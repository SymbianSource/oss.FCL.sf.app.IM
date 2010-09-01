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
* Description:  Private chat list view's container class
*
*/


// INCLUDE FILES
#include    "CCAInvitationContainer.h"
#include    "ccainvitationview.h"
#include    "caexternalinterface.h"
#include	"IMDialogUtils.h"

#include    "MCAViewSwitcher.h"

#include    "CCAStatusPaneHandler.h"
#include    "ChatDebugPrint.h"
#include    "CCABlockingUI.h"
#include    "chatngclient.hrh"
#include	"CCAAppUi.h"
#include    "MCASkinVariant.h"
#include    "CCAVariantFactory.h"
#include    "CCAApp.h"
#include    "CCAGroupUtils.h"
#include    "IMUtils.h"
#include    "CAUtils.h"
#include    "ChatDebugAssert.h"

#include	"MCAUiLoginCmdCB.h"
#include 	"MCABlockingPC.h"
#include    "MCAProcessManager.h"
#include    "MCAMainviewArrayPC.h"
#include    "MCAInvitationPC.h"
#include    "MCASettingsPC.h"
#include    "TEnumsPC.h"
#include    "CCATextViewControl.h"
#include	"CATouchUtils.h"
#include	"MCARecipientObserver.h"
#include 	"CCAProcessManagerFactory.h"

#include    "ImpsCSPAllErrors.h"

#include    <chatNG.mbg>
#include    <barsread.h>
#include	<impserrors.h>
#include    <chatNG.rsg>
#include    <aknlists.h>
#include    <eikclbd.h>
#include    <aknnotedialog.h>
#include    <aknsfld.h>
#include	<stringloader.h>
#include    <aknquerydialog.h>
#include    <AknIconUtils.h>
#include    <akniconarray.h>
#include    <txtrich.h>
#include    <aknenv.h>
#include    <aknsettingcache.h>
#include    <aknutils.h>
#include    <AknsUtils.h>
#include    <AppLayout.cdl.h>
#include    <AknsBasicBackgroundControlContext.h>
#include    <hlplch.h>
#include	<csxhelp/imng.hlp.hrh>
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// constants
const TInt KTextMargin = 100;  // normal text margin in twips
const TInt KBoldMargin = 200;  // bold text margin in twips
const TInt KSpaceBefore = 10;  // space before each paragraph (twips)
const TInt KPixelsBetweenLines = 5; // pixels between text lines

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAInvitationContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAInvitationContainer* CCAInvitationContainer::NewL(
    const TRect& aRect,
    TInt aInvitationIndex,
    TPtr aRejectReason,
    MCAViewSwitcher& aViewSwitcher,
    CCAView& aView,
    TInt& aTabId,
    TBool aContextFlag,
    MCARecipientObserver& aMenuObserver )
    {
    CCAInvitationContainer* self =
        new ( ELeave ) CCAInvitationContainer( aViewSwitcher,
                                               aInvitationIndex,
                                               aRejectReason,
                                               aView,
                                               aTabId,
                                               aContextFlag,
                                               aMenuObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::CCAInvitationContainer()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInvitationContainer::CCAInvitationContainer( MCAViewSwitcher& aViewSwitcher,
                                                TInt aInvitationIndex,
                                                TPtr aRejectReason,
                                                CCAView& aView,
                                                TInt& aTabId,
                                                TBool aContextFlag,
                                                MCARecipientObserver& aMenuObserver )
        : iViewSwitcher( aViewSwitcher ),
        iView( aView ),
        iInvitationIndex( aInvitationIndex ),
        iRejectReason( aRejectReason ),
        iTabId( aTabId ),
        iContextFlag( aContextFlag ),
        iMenuObserver( aMenuObserver ),
        iLongTapUsed( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );
    iAppUi->AddLayoutChangeObserver( this );
    iInvitationPC = iAppUi->GetProcessManager().GetInvitationsInterface();
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                     KAknsIIDQsnBgAreaMainMessage, Rect(), ETrue );

    // Granularity of 2 because we have listbox and findbox
    iControlArray = new ( ELeave ) CArrayPtrSeg<CCoeControl>( 2 );

    // Construct CAknLongTapDetector object
    LongTapDetectorL();

    // construct rich text object and basic formatting
    iParaFormatLayer = CParaFormatLayer::NewL();
    iCharFormatLayer = CCharFormatLayer::NewL();
    SetRect( aRect );

    MCABlockingPC* blockingPC = iAppUi->GetProcessManager().GetBlockingInterface();


    iBlockingUI = CCABlockingUI::NewL( blockingPC );

    iCba = CEikButtonGroupContainer::Current();
    UpdateCbaL();

    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::~CCAInvitationContainer()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

CCAInvitationContainer::~CCAInvitationContainer()
    {
    if ( iAppUi && !iAppUi->IsUnderDestruction() )
        {
        iAppUi->RemoveLayoutChangeObserver( this );
        }

    if ( iControlArray )
        {
        iControlArray->ResetAndDestroy();
        }


    delete iControlArray;
    delete iBlockingUI;
    if ( iParaFormatLayer )
        {
        delete iParaFormatLayer;
        iParaFormatLayer = NULL;
        }
    if ( iCharFormatLayer )
        {
        delete iCharFormatLayer;
        iCharFormatLayer = NULL;
        }

    if ( iRichText )
        {
        delete iRichText;
        iRichText = NULL;
        }
    delete iBgContext;

    delete iIdentification;

    delete iDateTime;

    delete iLongTapDetector;
    }

// ---------------------------------------------------------
// CCAInvitationContainer::DisplayBlockedListL()
// Displays list of blocked persons
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::DisplayBlockedListL() const
    {
    iBlockingUI->DisplayBlockedListL();
    }

// ---------------------------------------------------------
// CCAInvitationContainer::DisplayUnblockListQueryL()
// Displays unblock list query
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::DisplayUnblockListQueryL() const
    {
    iBlockingUI->DisplayUnblockListQueryL();
    }


// ---------------------------------------------------------
// CCAInvitationContainer::DisplayBlockTextQueryL()
// Displays query that takes input for wvid
// (other items were commented in a header)
// ---------------------------------------------------------
void CCAInvitationContainer::DisplayBlockTextQueryL() const
    {
    iBlockingUI->DisplayBlockTextQueryL();
    }

// ---------------------------------------------------------
// CCAInvitationContainer::JoinToGroupL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::JoinToGroupL( TInt aInviteIndex )
    {
    CHAT_DP_TXT( "invitation accepted" );

    // we must store the groupid because it's not valid after replying
    HBufC* groupId = iInvitationPC->GroupName( aInviteIndex ).AllocLC();

    HBufC* screenName =
        static_cast<CCAAppUi*>( iCoeEnv->AppUi() )->ScreenNamePtr();
    // join the group
    TInt err = iAppUi->GroupUtils()->JoinGroupL( EFalse, *groupId, screenName->Des() );
    if ( err == EOperationCancelled)
    	{
        CleanupStack::PopAndDestroy( groupId );
    	return; 
    	}
    if ( ( err == ECSPInvalidParameter ) || ( err == ECSPGroupDoesNotExist ) )
        {
        // If for some reason Group doesnt exist/Invalid, since we have called
        // ReplyInvitationL, the current invitation becomes invalid.
        // So even we cannot switch to the group view, as the group is invalid/doesnt exist
        // So better goback to the main view.
        if ( iAppUi->CAStatusPane() )
            {
            iAppUi->CAStatusPane()->SynchroniseAndRemoveTabGroup();
            }
        iAppUi->SwitchViewBackL( KUidFriendsListView );
        }

    // send invitation reply
    HBufC* ownWVIDbuf = iAppUi->GetProcessManager().GetSettingsInterface( )->
                        GetSapSettingValuePCL( TEnumsPC::EOwnWVUserID, NULL );
    CleanupStack::PushL( ownWVIDbuf );
    iInvitationPC->ReplyInvitationL( aInviteIndex , ETrue, KNullDesC,
                                     *ownWVIDbuf );
    CleanupStack::PopAndDestroy( ownWVIDbuf );

    CleanupStack::PopAndDestroy( groupId );
    }

// ---------------------------------------------------------
// CCAInvitationContainer::LayoutChangedL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::LayoutChangedL( TInt aType )
    {

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
//  CCAInvitationContainer::SizeChanged()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::SizeChanged()
    {
    // Update the font to correct size
    if ( iParaFormatLayer && iCharFormatLayer )
        {
        // Correct font
        const CFont* font = GetLayoutFont();

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
        // 215 = Color from application LAF (Smart message)
        charFormat.iFontPresentation.iTextColor = AKN_LAF_COLOR_STATIC( 215 );
        charFormatMask.SetAttrib( EAttColor );

        //can't leave here
        TRAPD( err, iParaFormatLayer->SetL( &paraFormat, paraFormatMask );
               iCharFormatLayer->SetL( charFormat, charFormatMask ) );
        if ( err != KErrNone )
            {
            CHAT_DP(
                D_CHAT_LIT( "CCAInvitationContainer::SizeChanged leave: %d!" ),
                err );
            CActiveScheduler::Current()->Error( err );
            }

        }

    if ( iTextView )
        {
        iTextView->SetRect( Rect() );
        }

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }

    SetLayout();
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAInvitationContainer::CountComponentControls() const
    {
    return iControlArray->Count();
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCoeControl* CCAInvitationContainer::ComponentControl( TInt aIndex ) const
    {
    if ( ( aIndex < 0 ) || ( aIndex >= iControlArray->Count() ) )
        {
        return NULL;
        }
    return iControlArray->At( aIndex );
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCAInvitationContainer::OfferKeyEventL( const TKeyEvent& aEvent,
                                                     TEventCode aType )
    {
    if ( aEvent.iScanCode == EStdKeyYes )
        {
        return EKeyWasConsumed;
        }

    CHAT_DP( D_CHAT_LIT(
                 "CCAInvitationContainer::OfferKeyEventL %d %d" ),
             aEvent,
             aType );
    switch ( aEvent.iCode )
        {
        case EKeyBackspace:
            {
            // ask the confirmation
            HBufC* prompt =
                iEikonEnv->AllocReadResourceLC( R_CHAT_CONV_LIST_INV_DEL );
            CAknQueryDialog* dlg = new( ELeave )CAknQueryDialog();
            CleanupStack::PushL( dlg );
            dlg->SetPromptL( prompt->Des() );
            CleanupStack::Pop( dlg );
            TInt ret( dlg->ExecuteLD( R_CLOSE_COVERSATION_QUERY ) );
            CleanupStack::PopAndDestroy( prompt );
            // should be deleted
            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                DeleteInvitationL( iInvitationIndex );
                if ( iAppUi->CAStatusPane() )
                    {
                    iAppUi->CAStatusPane()->SynchroniseAndRemoveTabGroup();
                    }
                iAppUi->SwitchViewBackL( KUidFriendsListView );
                }
            return EKeyWasConsumed;
            }

        // Flowthrough, because cases are identical.
        case EKeyRightArrow:
        case EKeyLeftArrow:
            {
            return iAppUi->OfferTabEventL( aEvent, aType );
            }

        case EKeyDownArrow:
            {
            //Only if the last line is not visible send the key
            //to the texControl so that it scrolls, hence dont consume the key
            //If the lastline is viewable no necessity to scroll any more, hence
            //consume the key

            if ( !iTextView->IsVisible( iLastSelection.HigherPos() ) )
                {
                // we're scrolling down and bottom line is not shown,
                // so scroll messages up by one line
                return EKeyWasNotConsumed;
                }

            return EKeyWasConsumed;
            }

        case EKeyEnter:
            {
            iMenuObserver.ShowPopUpMenuL();
            return EKeyWasConsumed;
            }

        default:
            {
            break;
            }
        }

    UpdateCbaL();
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_INVITE ;
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::SetLayout()
    {

    }
// ---------------------------------------------------------
// CCAInvitationContainer::HandleError
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAInvitationContainer::HandleError( TInt aError ) const
    {
    CHAT_DP( D_CHAT_LIT( "CCAInvitationContainer::HandleError,\
                          aError = %d" ), aError );

    if ( aError )
        {
        CActiveScheduler::Current()->Error( aError );
        }
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::IsInvitationOpen
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TBool CCAInvitationContainer::IsInvitationOpen()
    {
    return iInvitationOpen;
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::UpdateCbaL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::UpdateCbaL()
    {
    TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__CONTEXTOPTIONS;
    iCba->SetCommandSetL( cbaRes );
    iCba->DrawNow();
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::SwitchToSingleListViewL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::SwitchToSingleListViewL( )
    {
    iAppUi->SwitchViewL( KUidFriendsListView );
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::DeleteInvitation
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::DeleteInvitationL( TInt aInviteindex )
    {
    iInvitationPC->DeleteInvitationL( aInviteindex );
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::RejectInvitationL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::RejectInvitationL( TInt aInviteindex, TPtr aRejectReason )
    {
    CHAT_DP_TXT( "invitation rejected" );

    // and send invitation reply (reject)
    HBufC* ownWVIDbuf = iAppUi->GetProcessManager().GetSettingsInterface( )->
                        GetSapSettingValuePCL( TEnumsPC::EOwnWVUserID, NULL );
    CleanupStack::PushL( ownWVIDbuf );

    iInvitationPC->ReplyInvitationL( aInviteindex, EFalse, aRejectReason, *ownWVIDbuf );
    CleanupStack::PopAndDestroy( ownWVIDbuf );
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::InvitationSwitchTabL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::InvitationSwitchTabL( TInt iCurrenntActiveTab )
    {
    CHAT_DP_TXT( "Switching between invitations" );
    PrepareTextL( iCurrenntActiveTab );

    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::PrepareTextL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::PrepareTextL( TInt aInvitationIndex )
    {
    TInt invitationIndex = aInvitationIndex;

    delete iIdentification;
    iIdentification = NULL;

    iIdentification = iInvitationPC->InvUserIdentificationL( invitationIndex ).AllocL();

    // get and format the time/date string
    HBufC* tFormat = iEikonEnv->AllocReadResourceLC(
                         R_QTN_TIME_USUAL_WITH_ZERO );
    HBufC* dFormat = iEikonEnv->AllocReadResourceLC(
                         R_QTN_DATE_SHORT_WITH_ZERO );
    CDesCArray* array = new( ELeave )CDesCArrayFlat( 2 ); // two items
    CleanupStack::PushL( array );
    array->AppendL( *tFormat );
    array->AppendL( *dFormat );
    HBufC* datetime = StringLoader::LoadLC( R_QTN_CHAT_INVITE_DATE_TIME,
                                            *array );

    delete iDateTime;
    iDateTime = NULL;

    iDateTime = HBufC::NewL( datetime->Length() );
    TPtr ptr( iDateTime->Des() );


    TTime receviedTime = iInvitationPC->ReceivedAt( invitationIndex );
    if ( receviedTime != 0 )
        {
        receviedTime.FormatL( ptr, *datetime );
        }

    AknTextUtils::LanguageSpecificNumberConversion( ptr );


    CleanupStack::PopAndDestroy( 4, tFormat );


    //delete the controls if any - as this might be called when u
    //switch between tabs
    if ( iControlArray )
        {
        iControlArray->ResetAndDestroy();
        }

    delete iRichText;

    iRichText = NULL;
    iTextView = NULL;

    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    // Set text colour according to skin
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    TRgb defaultSkinTextColor( KRgbBlack );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               defaultSkinTextColor,
                               KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG6 );
    charFormat.iFontPresentation.iTextColor = defaultSkinTextColor;
    charFormatMask.SetAttrib( EAttColor );
    iRichText->SetInsertCharFormatL( charFormat, charFormatMask, 0 );

    AknsUtils::RegisterControlPosition( this );
    DisplayTextL( invitationIndex );

    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::DisplayTextL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::DisplayTextL( TInt aInvitationindex )	//  End of File
    {
    // initialize the textviewcontrol now when we know the size of dialog
    iTextView = CCATextViewControl::NewL( Rect(), *this, iRichText );

    iTextView->SetRect( Rect() );

    iControlArray->AppendL( iTextView );

    InsertLineL( R_QTN_CHAT_INV_MESSAGE );
    InsertLineL( iInvitationPC->Message(  aInvitationindex ), ETrue );

    InsertLineL( R_QTN_CHAT_INV_FROM );
    InsertLineL( *iIdentification, ETrue );

    InsertLineL( R_QTN_CHAT_INV_TOGROUP );

    // Display group name instead of group id, if the process errors, display the group id
    TPtrC groupNamePtr = iAppUi->GroupUtils()->GetGroupName(
                             iInvitationPC->GroupName( aInvitationindex ) );

    if ( groupNamePtr.CompareC( KNullDesC ) )
        {
        InsertLineL( groupNamePtr, ETrue );
        }
    else
        {
        InsertLineL( CCAAppUi::DisplayId( iInvitationPC->GroupName( aInvitationindex ) ), ETrue );
        }

    InsertLineL( R_QTN_CHAT_INV_DATE );
    InsertLineL( *iDateTime, ETrue );

    // update scrollbars
    MCATextView* textView = static_cast<MCATextView*>( iTextView );
    TCursorSelection sel( 0, 0 );
    textView->ScrollVisibleL( sel, ETrue );
    textView->SetPenBehaviour( MCATextView::EPenForwardToParent );

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }

    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::InsertLineL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::InsertLineL( TInt aTextResourceId,
                                          TBool aBold /*= EFalse*/ )
    {
    HBufC* text = iEikonEnv->AllocReadResourceLC( aTextResourceId );
    InsertLineL( *text, aBold );
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// CCAInvitationContainer::InsertLineL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInvitationContainer::InsertLineL( const TDesC& aText,
                                          TBool aBold /*= EFalse*/ )
    {
    TInt startPos( iRichText->DocumentLength() );

    if ( startPos != 0 )
        {
        // Start new paragraph if this is not the first line
        iRichText->InsertL( startPos,
                            CEditableText::EParagraphDelimiter );
        startPos++;
        }

    iRichText->InsertL( startPos, aText );

    //Store the cursor position of last line of the text.
    //this will help us in determining whether the lastline is
    //viewable or not.
    TInt endPos( iRichText->DocumentLength() );

    iLastSelection.SetSelection( endPos - 1, startPos );

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;

    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;

    if ( aBold )
        {
        charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
        charFormatMask.SetAttrib( EAttFontStrokeWeight );

        paraFormat.iLeftMarginInTwips = KBoldMargin;
        paraFormatMask.SetAttrib( EAttLeftMargin );
        }
    else
        {
        charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightNormal );
        charFormatMask.SetAttrib( EAttFontStrokeWeight );

        paraFormat.iLeftMarginInTwips = KTextMargin;
        paraFormatMask.SetAttrib( EAttLeftMargin );
        }

    iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                 startPos, aText.Length() );
    iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                 startPos, aText.Length() );

    if ( startPos != 0 )
        {
        startPos--;
        }
    TCursorSelection sel( iRichText->DocumentLength(), startPos );
    MCATextView* textView = static_cast<MCATextView*>( iTextView );
    textView->HandleFormatChangedL( sel );
    }

// --------------------------------------------------------------------------
// CCAInvitationContainer::LongTapDetectorL
// --------------------------------------------------------------------------
//
CAknLongTapDetector& CCAInvitationContainer::LongTapDetectorL()
    {
    if ( !iLongTapDetector )
        {
        iLongTapDetector = CAknLongTapDetector::NewL( this );
        }
    return *iLongTapDetector;
    }

// --------------------------------------------------------------------------
// CCAInvitationContainer::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CCAInvitationContainer::HandleLongTapEventL(
    const TPoint& /*aPenEventLocation*/,
    const TPoint& /*aPenEventScreenLocation*/ )
    {
    if ( CATouchUtils::PenEnabled() )
        {
        iLongTapUsed = ETrue;
        }
    }

// ---------------------------------------------------------
// CCAInvitationContainer::GetLayoutFont
//
// ---------------------------------------------------------
//
const CFont* CCAInvitationContainer::GetLayoutFont()
    {
    TAknLayoutText labelLayout;
    labelLayout.LayoutText( Rect(),
                            AppLayout::Smart_Messages_Line_2( 0 ) );
    return labelLayout.Font();
    }

// ---------------------------------------------------------
// CCAInvitationContainer::IsBlocked
// ---------------------------------------------------------
//
TBool CCAInvitationContainer::IsBlocked( const TDesC& aUserId )
    {
    TBool isBlocked = EFalse;
    isBlocked = iAppUi->GetProcessManager().GetBlockingInterface( )->
                IsContactBlocked( aUserId );
    return isBlocked;
    }


// ---------------------------------------------------------
// CCAInvitationContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CCAInvitationContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {

    if ( !CATouchUtils::PenEnabled() )
        {
        return;
        }

    // Long tap detect
    if ( iLongTapDetector )
        {
        iLongTapDetector->PointerEventL( aPointerEvent );
        }

    // Just return after long tap
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up && iLongTapUsed )
        {
        iLongTapUsed = EFalse;
        return;
        }

    // Single tap on main pane (not on scroll bar)
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        iMenuObserver.ShowPopUpMenuL();
        }

    }

//End of file
