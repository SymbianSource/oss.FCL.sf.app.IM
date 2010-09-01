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
* Description:  Rich text container
*
*/



// INCLUDE FILES
#include    "CCARichTextContainer.h"
#include    "CCAMessageWrapper.h"
#include    "MCATextView.h"
#include    "ChatDebugAssert.h"
#include    "IMUtils.h"
//#include    "IMPSCspAllErrors.h"
#include    "IMNoteMapper.h"
#include    "ChatDefinitions.h"
#include    "ChatDebugPrint.h"
#include    "IMUtils.h"
#include    "MCAStoredContacts.h"
#include    "CCAStorageManagerFactory.h"
#include    "CCAAppSettingsSAPExt.h"

#include    "CCAPicture.h"
#include    "ccaappui.h"
//#include    "ccaengine.h"
#include    "MCAConversationMessage.h"
#include    "MCAMessageUtils.h"
#include    "MCAContentProcessor.h"
#include    "CCAContentMessage.h"
#include    "CCAMessageExtensionsHandler.h"
#include    "impsbuilddefinitions.h"

#include    <chatNG.rsg>
#include    <AknIconUtils.h>
#include    <txtrich.h>     // CRichtText
#include    <AknUtils.h>
#include    <aknsettingcache.h>
#include    <aknenv.h>
#include    <aknconsts.h>
#include    <finditemengine.h>
#include    <gulicon.h>
#include    <barsread.h> // resourcereader
#include    <stringloader.h>
#include 	<CAVariationNG.rsg>

#include    <aknsdrawutils.h>
#include    <aknsutils.h>
#include    <aknutils.h>

#include	"MCASettingsPC.h"
#include	"MCAMsgAddedToRichTxtCtrlObserver.h"

#include	"MCAProcessManager.h"
#include	"MCAGroupPC.h"
#include    "IMUtils.h"

#include <AknLayoutScalable_Apps.cdl.h>
#include "TCAChatListBoxLayout.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// CONSTANTS
const TInt KChatHighlightColor = 244;
const TInt KPixelsBetweenLines = 8;      // pixels between text lines
const TInt KTimeStampMaxLength = 15;     // max length of timestamp text
const TInt KUnicodeRLM         = 0x200F; // Right-to-Left Mark
const TInt KUnicodeLRM         = 0x200E; // Left-to-Right Mark
const TInt KMaxLength          = 10;    // Max Length for user identity
_LIT( KSeparator, ": " );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCARichTextContainer::CCARichTextContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCARichTextContainer::CCARichTextContainer( MCAAppUi& aAppUi,
                                            MCASettingsPC& aSettingsPC,
                                            MCATextView& aTextView,
                                            CCAMessageExtensionsHandler& aExtensionsHandler,
                                            CGulIcon*& aFromMe,
                                            CGulIcon*& aToMe,
                                            CGulIcon*& aUnsupported,
                                            CGulIcon*& aCorrupted,
                                            MGraphicsDeviceMap& aMap,
                                            TBool aScrollOver,
                                            MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver
                                          )
        : iAppUi( aAppUi ),
        iSettings( aSettingsPC ),
        iTextView( aTextView ),
        iFromMe( aFromMe ),
        iToMe( aToMe ),
        iUnsupported( aUnsupported ),
        iCorrupted( aCorrupted ),
        iItemHighlight( ETrue ),
        iHighlightState( ENoHighlight ),
        iExtensionsHandler( aExtensionsHandler ),
        iMap( aMap ),
        iScrollOver( aScrollOver ),
        iAddMsgObserver( aAddMsgObserver )
    {
    iThumbSize = TSize( 42, 36 );
    iMessagesDeleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::ConstructL()
    {
    iAppUi.AddLayoutChangeObserver( this );

    // setup layout
    TAknLayoutId layoutId;
    CAknEnv::Static()->GetCurrentLayoutId( layoutId );

    AknIconUtils::SetSize( iCorrupted->Bitmap(), iThumbSize );
    AknIconUtils::SetSize( iUnsupported->Bitmap(), iThumbSize );

    // get icon settings
    iOwnMsgIcon = IMUtils::IntResourceValueL(
                      RSC_CHAT_VARIATION_OWN_MSG_ICON );

    iMsgIcon = IMUtils::IntResourceValueL(
                   RSC_CHAT_VARIATION_MSG_ICON );

    CCAAppUi* appUI = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );

    //Removed the ownership of iConnUI from this file and made it a reference
    //APPUI owns this connection UI instance and shares the same with this component.
    // get user-defined colors from SAP settings
    CCAAppSettingsSAPExt* SAPExtension = CCAAppSettingsSAPExt::NewLC( appUI->ConnectionUI() );
    TInt ownCol( SAPExtension->IntValueL( KIMOwnMessageColourKey ) );
    TInt otherCol( SAPExtension->IntValueL( KIMOthersMessageColourKey ) );
    CleanupStack::PopAndDestroy( SAPExtension );

    // Check timestamp setting
    iShowTimeStamps = iSettings.GetBoolValuePC( TEnumsPC::EShowTimeStamps, NULL );

    if ( ownCol != KErrNotFound )
        {
        iOwnMsgColorInUse = ETrue;
        iOwnMsgColor = ownCol;
        }
    if ( otherCol != KErrNotFound )
        {
        iMsgColorInUse = ETrue;
        iMsgColor = otherCol;
        }

    // check resource colours
    // The default colours settings is present only in the variation resource file. so we use IntResourceValueFromRssL instead of IntResourceValueL
    TBool defaultColors = IMUtils::IntResourceValueFromRssL(
                              RSC_CHAT_DEFAULT_MESSAGE_COLORS );
    iColorWholeMessage = IMUtils::IntResourceValueFromRssL(
                             RSC_CHAT_COLOR_WHOLE_MESSAGE );

    if ( !defaultColors && !iMsgColorInUse )
        {
        // user-defined color not defined and resource colour in use
        TResourceReader reader;
        // Since the color settings is not part of Cenrep, the ID is changed to denote the same.
        CCoeEnv::Static()->CreateResourceReaderLC( reader,
                                                   RSC_CHAT_MESSAGE_COLOR );
        TUint8 red( reader.ReadUint8() );
        TUint8 green( reader.ReadUint8() );
        TUint8 blue( reader.ReadUint8() );
        iMsgColor = TRgb( red, green, blue );
        iMsgColorInUse = ETrue;
        CleanupStack::PopAndDestroy(); // reader
        }

    if ( !defaultColors && !iOwnMsgColorInUse )
        {
        // user-defined color not defined and resource colour in use
        TResourceReader reader;
        // Since the color settings is not part of Cenrep, the ID is changed to denote the same.
        CCoeEnv::Static()->CreateResourceReaderLC( reader,
                                                   RSC_CHAT_OWN_MESSAGE_COLOR );
        TUint8 red( reader.ReadUint8() );
        TUint8 green( reader.ReadUint8() );
        TUint8 blue( reader.ReadUint8() );
        iOwnMsgColor = TRgb( red, green, blue );
        iOwnMsgColorInUse = ETrue;
        CleanupStack::PopAndDestroy(); // reader
        }

    CHAT_DP( D_CHAT_LIT( " msgColor red=%d, green=%d, blue=%d" ),
             iMsgColor.Red(), iMsgColor.Green(), iMsgColor.Blue() );
    CHAT_DP( D_CHAT_LIT( " ownMsgColor red=%d, green=%d, blue=%d" ),
             iOwnMsgColor.Red(), iOwnMsgColor.Green(), iOwnMsgColor.Blue() );

    iPrimaryFont = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont,
                                               NULL );
    iSecondaryFont = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont,
                                                 NULL );

    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
    paraFormat.iLanguage = CAknEnv::Static()->SettingCache().InputLanguage();
    paraFormatMask.SetAttrib( EAttParaLanguage );

    paraFormat.iFillColor = KRgbWhite;
    paraFormatMask.SetAttrib( EAttFillColor );

    paraFormat.iLeftMarginInTwips = 100;
    paraFormat.iIndentInTwips = -100;
    paraFormatMask.SetAttrib( EAttLeftMargin );
    paraFormatMask.SetAttrib( EAttIndent );

    // Create zoom factor object
    TZoomFactor devicemap( CCoeEnv::Static()->ScreenDevice() );
    devicemap.SetZoomFactor( TZoomFactor::EZoomOneToOne );

    // Get the line hight and font height
    TRect mainPaneRect;
    TAknLayoutRect lineRect;
    TAknLayoutRect layoutRect;

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    TAknWindowLineLayout readingPane = AknLayoutScalable_Apps::im_reading_pane( TChatListBoxLayout::EWithoutEditor );
    layoutRect.LayoutRect( mainPaneRect, readingPane );

    TAknWindowLineLayout lineLayout = AknLayoutScalable_Apps::list_im_single_pane( 0 );
    lineRect.LayoutRect( layoutRect.Rect(), lineLayout );
    TInt lineHeight = lineRect.Rect().Height();

    TAknLayoutText text;
    text.LayoutText( layoutRect.Rect(),
                     AknLayoutScalable_Apps::list_single_im_pane_t1( 0 ).LayoutLine() );

    // Set line space
    paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips;
    paraFormatMask.SetAttrib( EAttLineSpacingControl );

    paraFormat.iLineSpacingInTwips = devicemap.VerticalPixelsToTwips( lineHeight );
    paraFormatMask.SetAttrib( EAttLineSpacing );

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontSpec = iSecondaryFont->FontSpecInTwips();
    // Set font height
    charFormat.iFontSpec.iHeight = devicemap.VerticalPixelsToTwips( text.TextRect().Height() );
    charFormatMask.SetAttrib( EAttFontTypeface );
    charFormatMask.SetAttrib( EAttFontHeight );

    // Text color
    TRgb defaultSkinTextColor( KRgbBlack );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               defaultSkinTextColor,
                               KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG6 );

    //Store the default masg color
    iDefaultMsgColor =  defaultSkinTextColor;

    charFormat.iFontPresentation.iTextColor = defaultSkinTextColor;
    charFormatMask.SetAttrib( EAttColor );

    charFormat.iFontPresentation.iPictureAlignment =
        TFontPresentation::EAlignTop;
    charFormatMask.SetAttrib( EAttFontPictureAlignment );

    charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightNormal );
    charFormatMask.SetAttrib( EAttFontStrokeWeight );


    iParaFormatLayer = CParaFormatLayer::NewL( &paraFormat, paraFormatMask );
    iCharFormatLayer = CCharFormatLayer::NewL( charFormat, charFormatMask );

    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARichTextContainer* CCARichTextContainer::NewL(
    MCAAppUi& aAppUi,
    MCASettingsPC& aSettingsPC,
    MCATextView& aTextView,
    CCAMessageExtensionsHandler& aExtensionsHandler,
    CGulIcon*& aFromMe,
    CGulIcon*& aToMe,
    CGulIcon*& aUnsupported,
    CGulIcon*& aCorrupted,
    MGraphicsDeviceMap& aMap,
    TBool aScrollOver /*= ETrue*/,
    MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver /*= NULL*/
)
    {
    CCARichTextContainer* self = new( ELeave ) CCARichTextContainer(
        aAppUi,
        aSettingsPC,
        aTextView,
        aExtensionsHandler, aFromMe, aToMe,
        aUnsupported, aCorrupted,
        aMap,
        aScrollOver, aAddMsgObserver
    );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCARichTextContainer::~CCARichTextContainer()
    {
    delete iParaFormatLayer;
    delete iCharFormatLayer;
    delete iRichText;
    iMessages.ResetAndDestroy();
    iAppUi.RemoveLayoutChangeObserver( this );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::SelectedMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAConversationMessage* CCARichTextContainer::SelectedMessage()
    {
    if ( iHighlightState != ENoHighlight )
        {
        return &( iMessages[iSelected]->Message() );
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::SelectedItem
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCARichTextContainer::SelectedItemL()
    {
    if ( iHighlightState == EItemSelected )
        {
        TCursorSelection curSel( iMessages[iSelected]->Selection() );

        // calculate item highlight position
        TCursorSelection itemSel(
            iMessages[iSelected]->Highlights()[iSelectedItem] );

        TInt messageStart( curSel.LowerPos() );
        curSel.iAnchorPos = itemSel.iAnchorPos + messageStart;
        curSel.iCursorPos = itemSel.iCursorPos + messageStart;

        HBufC* text = HBufC::NewMaxL( curSel.Length() );
        TPtr ptr( text->Des() );
        iRichText->Extract(	ptr, curSel.LowerPos(), curSel.Length() );
        return text;
        }
    else
        {
        return KNullDesC().AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::SelectedItemType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARichTextContainer::SelectedItemType()
    {
    if ( iHighlightState == EItemSelected )
        {
        return iMessages[iSelected]->HighlightTypes()[iSelectedItem];
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::RichText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MLayDoc* CCARichTextContainer::TextLayout()
    {
    return iRichText;
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::MoveHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARichTextContainer::MoveHighlightL( THighlightMovement aDirection )
    {
    __CHAT_ASSERT_DEBUG( aDirection == EPrevious || aDirection == ENext );
    TBool up( aDirection == EPrevious );
    TInt messageCount( iMessages.Count() );
    if ( messageCount == 0 )
        {
        // no messages, nothing to highlight
        DisableHighlightL();
        return KErrNotFound;
        }

    __CHAT_ASSERT_DEBUG( iSelected >= 0 && iSelected < messageCount );
    TInt highlightCount( iMessages[iSelected]->Highlights().Count() );

    if ( !iItemHighlight )
        {
        // if item highlighting is not on, clear the item count
        highlightCount = 0;
        }

    // if message is highlighted, but it doesn't fit to screen
    // scroll it line by line
    if ( iHighlightState == EMessageTop || iHighlightState == EMessageBottom )
        {
        TCursorSelection& sel = iMessages[iSelected]->Selection();
        if ( up )
            {
            // up
            if ( !iTextView.IsVisible( sel.LowerPos() ) )
                {
                // we're scrolling up and top line is not shown,
                // so scroll messages down by one line
                iTextView.ScrollLinesL( 1 );
                return KErrNone;
                }
            }
        else if ( !iTextView.IsVisible( sel.HigherPos() ) )
            {
            // we're scrolling down and bottom line is not shown,
            // so scroll messages up by one line
            iTextView.ScrollLinesL( -1 );
            return KErrNone;
            }
        }

    // update highlight position
    switch ( iHighlightState )
        {
        case ENoHighlight:  // no highlight, select last  or first message
            {
            if ( up )
                {
                iSelected = messageCount - 1;

                // Get highlight count of prev message
                // if item highlighting is on
                if ( iItemHighlight )
                    {
                    highlightCount = iMessages[iSelected]->Highlights().Count();
                    }

                if ( highlightCount > 0 )
                    {
                    // highlight items inside last message (if any)
                    iHighlightState = EItemSelected;
                    iSelectedItem = highlightCount - 1;
                    }
                else
                    {
                    // bottom of last message
                    iHighlightState = EMessageBottom;
                    }
                }
            else
                {
                // top of first message
                iHighlightState = EMessageTop;
                iSelected = 0;
                }
            break;
            }
        case EItemSelected: // highlighting items
            {
            if ( up )
                {
                // up, previous item
                --iSelectedItem;
                if ( iSelectedItem < 0 )
                    {
                    // end of highlights, focus message
                    iHighlightState = EMessageTop;
                    }
                }
            else
                {
                // down, next item
                ++iSelectedItem;
                if ( iSelectedItem >= highlightCount )
                    {
                    // end of highlights
                    if ( iSelected < messageCount - 1 )
                        {
                        // focus next message
                        ++iSelected;
                        iHighlightState = EMessageTop;
                        }
                    else if ( iScrollOver )
                        {
                        // going down from last message,
                        // disable highlight so editor can take the focus
                        DisableHighlightL();
                        return KErrNone;
                        }
                    else
                        {
                        // Loop back to beginning
                        iSelected = 0;
                        iSelectedItem = 0;
                        iHighlightState = EMessageTop;
                        }

                    }
                }
            break;
            }
        case EMessageTop:   // highlighting message ("top" of it)
            {
            if ( up )
                {
                // up, highlighting previous message
                if ( iSelected > 0 )
                    {
                    // prev
                    --iSelected;

                    // get highlight count of prev message
                    // if item highlighting is on
                    if ( iItemHighlight )
                        {
                        highlightCount = iMessages[iSelected]->Highlights().Count();
                        }

                    if ( highlightCount > 0 )
                        {
                        // highlight items inside prev message (if any)
                        iHighlightState = EItemSelected;
                        iSelectedItem = highlightCount - 1;
                        }
                    else
                        {
                        // highlight whole prev message
                        iHighlightState = EMessageBottom;
                        }
                    }
                else if ( iScrollOver )
                    {
                    // going up from first message,
                    // disable highlight, so editor can take the focus
                    DisableHighlightL();
                    return KErrNone;
                    }
                else
                    {
                    // Loop to last message
                    iSelected = messageCount - 1;
                    highlightCount = 0;

                    // Get highlight count of last message
                    // if item highlighting is on
                    if ( iItemHighlight )
                        {
                        highlightCount =
                            iMessages[iSelected]->Highlights().Count();
                        }

                    if ( highlightCount > 0 )
                        {
                        // Highlight items inside last message (if any)
                        iHighlightState = EItemSelected;
                        iSelectedItem = highlightCount - 1;
                        }
                    else
                        {
                        // Highlight whole last message
                        iHighlightState = EMessageBottom;
                        }
                    }
                }
            else
                {
                // down, highlight items inside this message (if any)
                if ( highlightCount > 0 )
                    {
                    iSelectedItem = 0;
                    iHighlightState = EItemSelected;
                    }
                else if ( iSelected < messageCount - 1 )
                    {
                    // next
                    ++iSelected;
                    iHighlightState = EMessageTop;
                    }
                else if ( iScrollOver )
                    {
                    // going down from last message,
                    // disable highlight, so editor can take the focus
                    DisableHighlightL();
                    return KErrNone;
                    }
                else
                    {
                    // Loop back to beginning
                    iSelected = 0;
                    iSelectedItem = 0;
                    iHighlightState = EMessageTop;
                    }
                }
            break;
            }
        case EMessageBottom:   // highlighting message ("bottom" of it)
            {
            if ( up )
                {
                // up, highlight items inside this message (if any)
                if ( highlightCount > 0 )
                    {
                    iSelectedItem = highlightCount - 1;
                    iHighlightState = EItemSelected;
                    }
                else if ( iSelected > 0 )
                    {
                    // prev
                    --iSelected;

                    // get highlight count of prev message
                    // if item highlighting is on
                    if ( iItemHighlight )
                        {
                        highlightCount = iMessages[iSelected]->Highlights().Count();
                        }

                    if ( highlightCount > 0 )
                        {
                        // highlight items inside prev message (if any)
                        iHighlightState = EItemSelected;
                        iSelectedItem = highlightCount - 1;
                        }
                    else
                        {
                        iHighlightState = EMessageBottom;
                        }
                    }
                else if ( iScrollOver )
                    {
                    // going up from first message,
                    // disable highlight, so editor can take the focus
                    DisableHighlightL();
                    return KErrNone;
                    }
                else
                    {
                    // Loop to last message
                    iSelected = messageCount - 1;
                    highlightCount = 0;

                    // Get highlight count of last message
                    // if item highlighting is on
                    if ( iItemHighlight )
                        {
                        highlightCount =
                            iMessages[iSelected]->Highlights().Count();
                        }

                    if ( highlightCount > 0 )
                        {
                        // Highlight items inside last message (if any)
                        iHighlightState = EItemSelected;
                        iSelectedItem = highlightCount - 1;
                        }
                    else
                        {
                        // Highlight whole last message
                        iHighlightState = EMessageBottom;
                        }
                    }
                }
            else
                {
                // down, highlighting next message
                if ( iSelected < messageCount - 1 )
                    {
                    // next
                    ++iSelected;
                    iHighlightState = EMessageTop;
                    }
                else if ( iScrollOver )
                    {
                    // going down from last message,
                    // disable highlight, so editor can take the focus
                    DisableHighlightL();
                    return KErrNone;
                    }
                else
                    {
                    // Loop back to beginning
                    iSelected = 0;
                    iSelectedItem = 0;
                    iHighlightState = EMessageTop;
                    }
                }
            break;
            }
        default:
            {
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }

    // get the selection
    TCursorSelection curSel( CurrentSelection() );

    // remove colors from old highlight
    TInt textLen( iRichText->DocumentLength() );
    if ( iPrevious.iAnchorPos < textLen &&
         iPrevious.iCursorPos <= textLen )
        {
        BackColorL( iPrevious, KRgbWhite );
        TextBackColorL( iPrevious, KRgbWhite );
        }

    // and set new one
    if ( iHighlightState == EItemSelected )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            TextBackColorL( curSel, color );
            }
        else
            {
            TextBackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }
    else
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            BackColorL( curSel, color );
            }
        else
            {
            BackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }

    // handle changed format
    TCursorSelection changed( Union( iPrevious, curSel ) );

    iTextView.HandleFormatChangedL( changed );

    iTextView.ScrollVisibleL( curSel, ETrue );

    iPrevious = curSel;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::DisableHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::DisableHighlightL()
    {
    iHighlightState = ENoHighlight;

    TCursorSelection sel( 0, 0 );
    TInt count( iMessages.Count() );
    if ( count > 0 )
        {
        sel = iMessages[count-1]->Selection();
        }

    // remove colors from old highlight (if any)
    TInt textLen( iRichText->DocumentLength() );
    if ( iPrevious.iAnchorPos < textLen &&
         iPrevious.iCursorPos <= textLen )
        {
        BackColorL( iPrevious, KRgbWhite );
        TextBackColorL( iPrevious, KRgbWhite );
        iTextView.HandleFormatChangedL( iPrevious );
        }

    // set focus to last message
    iTextView.ScrollVisibleL( sel, EFalse );
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::SetItemHighlight
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::SetItemHighlight( TBool aItemHighlight )
    {
    iItemHighlight = aItemHighlight;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::Highlighted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCARichTextContainer::Highlighted()
    {
    return iHighlightState != ENoHighlight;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::Highlighted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARichTextContainer::HighlightItemAtPosL( TInt aPos )
    {
    TInt messageCount = iMessages.Count();
    if ( messageCount == 0 )
        {
        // no messages
        return KErrNotFound;
        }

    // Store old values
    TInt oldSelectedMessage = iSelected;
    TInt oldSelectedItem = iSelectedItem;
    THighlightState oldHighlight = iHighlightState;

    // Find message in position aPos
    TBool found = EFalse;
    for ( TInt i = 0; i < messageCount; ++i )
        {
        TCursorSelection sel = iMessages[ i ]->WholeSelection();
        if ( aPos >= sel.LowerPos() &&
             aPos <= sel.HigherPos() )
            {
            // Found the message
            iSelected = i;
            iHighlightState = EMessageTop;
            found = ETrue;
            // stop searching
            break;
            }
        }

    if ( !found )
        {
        // not found
        return KErrNotFound;
        }

    // Check if there are items inside the message that can be highlighted.
    if ( iItemHighlight )
        {
        TInt hlCount = iMessages[ iSelected ]->Highlights().Count();
        TInt relativePos =
            aPos - iMessages[ iSelected ]->WholeSelection().LowerPos();

        for ( TInt i = 0; i < hlCount; ++i )
            {
            TCursorSelection sel = iMessages[ iSelected ]->Highlights()[ i ];

            // Highlight's selection is relative to message
            if ( relativePos >= sel.LowerPos() &&
                 relativePos <= sel.HigherPos() )
                {
                // Found an item
                iHighlightState = EItemSelected;
                iSelectedItem = i;
                // stop searching
                break;
                }
            }
        }

    // Update UI only if selection has changed
    if ( iSelected != oldSelectedMessage ||
         iSelectedItem != oldSelectedItem ||
         oldHighlight != iHighlightState )
        {
        UpdateSelectionL();
        return KErrNone;
        }
    // Already highlighted
    return KErrAlreadyExists;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::UpdateSelection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::UpdateSelectionL()
    {
    // get the selection
    TCursorSelection curSel( CurrentSelection() );

    // remove colors from old highlight
    TInt textLen( iRichText->DocumentLength() );
    if ( iPrevious.iAnchorPos < textLen &&
         iPrevious.iCursorPos <= textLen )
        {
        BackColorL( iPrevious, KRgbWhite );
        TextBackColorL( iPrevious, KRgbWhite );
        }

    // and set new one
    if ( iHighlightState == EItemSelected )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            TextBackColorL( curSel, color );
            }
        else
            {
            TextBackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }
    else
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            BackColorL( curSel, color );
            }
        else
            {
            BackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }

    // handle changed format
    TCursorSelection changed( Union( iPrevious, curSel ) );
    iTextView.HandleFormatChangedL( changed );
    iTextView.ScrollVisibleL( curSel, ETrue );

    iPrevious = curSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::InsertUserIdentityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::InsertUserIdentityL(
    CCAMessageWrapper& aMessageWrapper )
    {
    TInt stampStart = 0;
    TCursorSelection timeSel( stampStart, stampStart );
    TInt stampEnd = stampStart;
    MCAConversationMessage& message = aMessageWrapper.Message();
    TPtrC sender;
    TPtrC recipient;
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    if ( message.MessagerType() == TEnumsPC::EMessageSent
         && message.MessageType() != TEnumsPC::EMessageSystem )
        {
        sender.Set( contacts->OwnStatus().Identification() );
        }
    else
        {
        sender.Set( contacts->Identification( message.Sender() ) );
        }
    recipient.Set( contacts->Identification( message.Recipient() ) );

    if ( message.MessagerType() == TEnumsPC::EMessageSent &&
         message.MessageType() == TEnumsPC::EMessagePTOP &&
         iOwnMsgIcon )
        {
        if ( sender.Length() >= KMaxLength )
            {
            TPtrC aTempPtr;
            aTempPtr.Set( sender.Left( 8 ) );
            TBuf<11> aTruncateSender( aTempPtr );
            aTruncateSender.Append( _L( ".. " ) );
            iRichText->InsertL( stampStart, aTruncateSender );
            stampEnd = stampStart + aTruncateSender.Length();
            }
        else
            {
            TBuf<10> aTruncateSender( sender );
            aTruncateSender.Append( KSpace );
            iRichText->InsertL( stampStart, aTruncateSender );
            stampEnd = stampStart + aTruncateSender.Length();
            }
        }
    else if ( message.MessageType() == TEnumsPC::EMessagePTOP &&
              message.MessagerType() == TEnumsPC::EMessageReceived &&
              iMsgIcon )
        {
        if ( recipient.Length() >= KMaxLength )
            {
            TPtrC aTempPtr;
            aTempPtr.Set( recipient.Left( 8 ) );
            TBuf<11> aTruncateRecipient( aTempPtr );
            aTruncateRecipient.Append( _L( ".. " ) );
            iRichText->InsertL( stampStart, aTruncateRecipient );
            stampEnd = stampStart + aTruncateRecipient.Length();
            }
        else
            {
            TBuf<10> aTruncateRecipient( recipient );
            aTruncateRecipient.Append( KSpace );
            //CEikonEnv::Static()->InfoMsg(aTruncateRecipient);
            iRichText->InsertL( stampStart, aTruncateRecipient );
            stampEnd = stampStart + aTruncateRecipient.Length();
            }
        }
    timeSel.SetSelection( stampStart, stampEnd - 1 );

    // bold the User Identity
    if ( timeSel.Length() > 0 )
        {
        BoldL( timeSel );
        }

    return timeSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::InsertTimeStampL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::InsertTimeStampL(
    CCAMessageWrapper& aMessageWrapper )
    {
    TInt start = 0;
    TCursorSelection timeSel( start, start );

    if ( !iSettings.GetBoolValuePC( TEnumsPC::EShowTimeStamps, NULL ) ||
         aMessageWrapper.Message().SystemMessageType() ==
         TEnumsPC::ESystemMessageDateChange )
        {
        // 1. Setting for time stamp is OFF
        // 2. Special case: date change don't have time stamp.
        return timeSel;
        }

    // format time
    HBufC* timeStamp = HBufC::NewLC( KTimeStampMaxLength + KSpace().Length() );
    TPtr timePtr( timeStamp->Des() );

    HBufC* timeStampFormat = NULL;
    timeStampFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );

    TTime time = aMessageWrapper.Message().TimeStamp();
    TRAPD( err, time.FormatL( timePtr, *timeStampFormat ) );
    if ( err == KErrOverflow )
        {
        // Reserved text space was not enough.
        // Adjust the KTimeStampMaxLength
        __CHAT_ASSERT_DEBUG( EFalse );

        // in release builds return without timestamp
        CleanupStack::PopAndDestroy( 2, timeStamp ); // timeStamp, timeStampFormat
        return timeSel;
        }

    // Convert numbers to local language
    AknTextUtils::LanguageSpecificNumberConversion( timePtr );

    // Set Alignment of Pm/Am from local setting
    // This keeps the am/pm text close to the time.
    timePtr.Append( AknLayoutUtils::LayoutMirrored() ?
                    KUnicodeRLM : KUnicodeLRM );

    //	IM client UI customization, phase 2
    //  Based on Variation flag, changes Time Stamp format in conversation view.
    //  where Time Stamp should appear in brackets.

    // read variation flag values
    TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
    // and check dynamic features
    if ( requireUserIdentity )
        {
        timePtr.Insert( 0, _L( "(" ) );
        timePtr.Append( _L( ")" ) );
        timePtr.Append( KSpace );
        }
    else
        {
        timePtr.Append( KSpace );
        }

    iRichText->InsertL( start, timePtr );

    CleanupStack::PopAndDestroy( 2, timeStamp ); // timeStamp, timeStampFormat
    timeSel.SetSelection( start, timePtr.Length() - 1 );

    // bold the time
    if ( timeSel.Length() > 0 )
        {
        BoldL( timeSel );
        }

    return timeSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::InsertContentL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARichTextContainer::InsertContentL( CCAMessageWrapper& aMessageWrapper )
    {
    MCAConversationMessage& message = aMessageWrapper.Message();

    TEnumsPC::TContentType type( message.ContentType() );
    switch ( type )
        {
        case TEnumsPC::EContentText:
            {
            TInt oldLen = iRichText->DocumentLength();

            // Insert text
            TPtrC msg( message.Text() );

            HBufC* tempMsg = NULL;

            TInt paraBreak = msg.Locate( CEditableText::EParagraphDelimiter );
            // Replace all paragraph delimiters with line breaks
            // to keep alingment consistent
            if ( paraBreak != KErrNotFound )
                {
                tempMsg = msg.AllocLC();
                TPtr ptr( tempMsg->Des() );
                ptr.Zero();

                ReplaceParaDelimsWithLineBreaks( msg, ptr );
                msg.Set( ptr );
                }

            HBufC* numberConv = NULL;

            // Convert numerals to local language
            // 1) date item
            if ( aMessageWrapper.Message().SystemMessageType() ==
                 TEnumsPC::ESystemMessageDateChange )
                {
                // Don't touch the original message
                numberConv = msg.AllocLC();
                TPtr ptr( numberConv->Des() );
                AknTextUtils::LanguageSpecificNumberConversion( ptr );
                msg.Set( ptr );
                }

            iRichText->InsertL( 0, msg );

            TInt textEnd = msg.Length();

            // Check for extensions (smileys)
            TCursorSelection selection( 0, textEnd );
            TInt bfrConv = iRichText->DocumentLength();
            iExtensionsHandler.ConvertSelectionToExtensionL( *iRichText,
                                                             selection );

            // Move textEnd index if extensions were found
            textEnd -= ( bfrConv - iRichText->DocumentLength() );

            // Add nbs to preserve formatting
            iRichText->InsertL( textEnd,
                                CEditableText::EZeroWidthNoBreakSpace );

            // If this is system message, it should be bold
            if ( message.MessageType() == TEnumsPC::EMessageSystem )
                {
                TCursorSelection sel( 0, textEnd );
                BoldL( sel );
                // System messages need different kind of alignment
                CParaFormat paraFormat;
                TParaFormatMask paraFormatMask;
                paraFormat.iLeftMarginInTwips = 0;
                paraFormatMask.SetAttrib( EAttLeftMargin );
                iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                             0, textEnd + 1 );
                }
            else
                {
                // Apply alignment for "normal" messages
                CParaFormat paraFormat;
                TParaFormatMask paraFormatMask;
                paraFormat.iLeftMarginInTwips = 100;
                paraFormatMask.SetAttrib( EAttLeftMargin );
                iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                             0, textEnd + 1 );

                // Remove bolding from message content
                TCharFormat charFormat;
                TCharFormatMask charFormatMask;
                charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightNormal );
                charFormatMask.SetAttrib( EAttFontStrokeWeight );
                iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                             0, textEnd + 1 );
                }

            // Cleanup
            if ( numberConv )
                {
                CleanupStack::PopAndDestroy( numberConv );
                }
            if ( tempMsg )
                {
                CleanupStack::PopAndDestroy( tempMsg );
                }

            return iRichText->DocumentLength() - oldLen;
            }
        case TEnumsPC::EContentPicture: // Flowthrough
        case TEnumsPC::EContentOther:
            {
            TInt oldLen = iRichText->DocumentLength();
            // insert thumbnail (if it's ready)
            AddThumbL( 0, aMessageWrapper );
            return iRichText->DocumentLength() - oldLen;
            }
        case TEnumsPC::EContentInvalid:
            {
            break;
            }
        default:
            {
            // Unsupported type
            __CHAT_ASSERT_DEBUG( false );
            break;
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::InsertNickL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::InsertNickL(
    CCAMessageWrapper& aMessageWrapper )
    {
    // insert nick
    MCAConversationMessage& message = aMessageWrapper.Message();

    if ( message.FailedMessage() )
        {
        return 	TCursorSelection( 0, 0 );
        }

    TPtrC sender;
    TPtrC recipient;
    if ( message.MessageType() == TEnumsPC::EMessageGroup ||
         message.MessageType() == TEnumsPC::EMessageWhisper )
        {
        sender.Set( message.Sender() );

        if ( message.MessagerType() == TEnumsPC::EMessageSent )
            {
            recipient.Set( message.Recipient() );
            }
        else
            {
            CCAAppUi* appUI = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );
            recipient.Set( appUI->GetProcessManager().GetGroupInterface()->ScreenName() );
            }

        }
    else
        {
        MCAStoredContacts* contacts =
            CCAStorageManagerFactory::ContactListInterfaceL();
        if ( message.MessagerType() == TEnumsPC::EMessageSent
             && message.MessageType() != TEnumsPC::EMessageSystem )
            {
            sender.Set( contacts->OwnStatus().Identification() );
            }
        else
            {
            sender.Set( contacts->Identification( message.Sender() ) );
            }

        recipient.Set( contacts->Identification( message.Recipient() ) );
        }
    TCursorSelection nickSel( 0, 0 );

    if ( message.MessagerType() == TEnumsPC::EMessageSent &&
         message.MessageType() == TEnumsPC::EMessagePTOP &&
         iOwnMsgIcon )
        {
        // this is sent (or failed) p2p message,
        // insert "from me" icon and separator
        // (if icons are in use)
        //	IM client UI customization, phase 2
        //  Based on Variation flag, changes Nick format in conversation view
        //  removes arrow sign and append only a seperator.

        // read variation flag values
        TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
        // and check dynamic features
        if ( !requireUserIdentity )
            {
            CCAPicture* pic = new( ELeave ) CCAPicture( iMap, iFromMe );
            TPictureHeader header;
            header.iPicture = TSwizzle<CPicture>( pic );
            iRichText->CancelInsertCharFormat();    // to prevent ETEXT 31 panic
            iRichText->InsertL( 0, header ); // takes ownership
            iRichText->InsertL( 1, KSeparator );
            nickSel.iAnchorPos = 1;
            }
        else
            {
            iRichText->InsertL( 0, KSeparator );
            nickSel.iAnchorPos = 0;
            }
        }
    else
        {
        TInt nickStart = 0;
        TInt nickEnd = nickStart;

        if ( message.MessageType() == TEnumsPC::EMessageWhisper )
            {
            // this is send/received whisper message (to us),
            // insert sender, "to me" icon and separator
            iRichText->InsertL( nickStart, sender );
            nickEnd = sender.Length();

            CCAPicture* pic = NULL;
            if ( message.MessagerType() == TEnumsPC::EMessageSent )
                {
                pic = new( ELeave )CCAPicture( iMap,
                                               iFromMe );
                }
            else
                {
                pic = new( ELeave )CCAPicture( iMap,
                                               iToMe );
                }
            TPictureHeader header;
            header.iPicture = TSwizzle<CPicture>( pic );
            iRichText->CancelInsertCharFormat(); // to prevent ETEXT 31 panic
            iRichText->InsertL( nickEnd, header ); // takes ownership

            iRichText->InsertL( nickEnd + 1, recipient );
            nickEnd = nickEnd + 1 + recipient.Length();

            iRichText->InsertL( nickEnd, KSeparator );
            }
        else if ( message.MessageType() == TEnumsPC::EMessagePTOP &&
                  message.MessagerType() == TEnumsPC::EMessageReceived &&
                  iMsgIcon )
            {
            // received p2p message and we want to display icon
            // insert "to me" icon and separator
            //	IM client UI customization, phase 2
            //  Based on Variation flag, changes Nick format in conversation view
            //  removes arrow sign and append only a seperator.

            // read variation flag values
            TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
            // and check dynamic features
            if ( !requireUserIdentity )
                {
                CCAPicture* pic = new( ELeave ) CCAPicture( iMap,
                                                            iToMe );
                TPictureHeader header;
                header.iPicture = TSwizzle<CPicture>( pic );
                iRichText->CancelInsertCharFormat(); // to prevent ETEXT 31 panic
                iRichText->InsertL( nickStart, header ); // takes ownership
                iRichText->InsertL( nickStart + 1, KSeparator );
                nickEnd = 1;
                }
            else
                {
                iRichText->InsertL( nickStart, KSeparator );
                nickEnd = 0;
                }
            }
        else if ( sender.Length() > 0 )
            {
            // this is normal group message (or p2p message if icons are not used),
            // insert sender and separator
            iRichText->InsertL( nickStart, sender );
            nickEnd = nickStart + sender.Length();
            iRichText->InsertL( nickEnd, KSeparator );
            }

        // formatting (bold nickname)
        nickSel.SetSelection( nickStart, nickEnd );
        if ( nickSel.Length() > 0 )
            {
            BoldL( nickSel );
            }
        }
    return nickSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::LayoutChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::LayoutChangedL( TInt /*aType*/ )
    {
    UpdateSkinnedTextColourL();
    if ( Highlighted() )
        {
        ChangeHighlightColorL();
        }
    }

TInt CCARichTextContainer::IntResourceValueL( TInt aResourceId )
    {
    // Show user Identity, if variated so
    TResourceReader reader;
    TInt value( KErrNone );
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );
    value = ResourceUtils::ReadTInt32L( reader );
    CleanupStack::PopAndDestroy(); // reader

    return value;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::AddMessageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::AddMessageL( MCAConversationMessage& aMessage )
    {
    if ( !iShowTimeStamps &&
         aMessage.SystemMessageType() == TEnumsPC::ESystemMessageDateChange )
        {
        // if timestamp setting is OFF we don't show day change items
        return;
        }

    TInt oldCount = iMessages.Count();

    // create wrapper
    CCAMessageWrapper* wrapper = CCAMessageWrapper::NewL( aMessage, *this );

    CleanupStack::PushL( wrapper );
    iMessages.AppendL( wrapper );   // takes the ownership
    CleanupStack::Pop( wrapper );

    if ( aMessage.ContentType() == TEnumsPC::EContentPicture )
        {
        if ( IMUtils::ContentProtectedL( aMessage.ContentData() ) )
            {
            aMessage.SetProcessState( TEnumsPC::EContentNotSupported );
            }
        else
            {
            // add thumbnail processor for message
            aMessage.AddContentProcessorL( iThumbSize );

            // If this is recorded chat container (iScrollOver == EFalse)
            // and added message was first move highlight
            if ( oldCount == 0 && !iScrollOver )
                {
                iMsgAddedToContentProcessor = 1;
                }
            }
        }

    // start new paragraph
    TInt lineStart = iRichText->DocumentLength();
    if ( lineStart != 0 )
        {
        iRichText->InsertL( lineStart, CEditableText::EParagraphDelimiter );
        }

    // Set Alignment from local layout
    iRichText->InsertL( iRichText->DocumentLength(),
                        AknLayoutUtils::LayoutMirrored() ? KUnicodeRLM : KUnicodeLRM );

    // add message to rich text
    TInt startPos( iRichText->DocumentLength() );

    //	IM client UI customization, phase 2
    //  Based on Variation flag, enables User Identity with Time Stamp
    //  in chat conversation view.

    // read variation flag values
    TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
    // and check dynamic features
    if ( requireUserIdentity )
        {
        TCursorSelection userSel = AddUserIdentityL( *wrapper );
        }

    TCursorSelection timeSel = AddTimeStampL( *wrapper );
    TCursorSelection nickSel = AddNickL( *wrapper );
    AddContentL( startPos, *wrapper );
    TInt endPos( iRichText->DocumentLength() );

    // set selection for highlight and the whole selection
    // (so that this can be removed)
    // append deletioncount to new messages
    wrapper->Selection().SetSelection(    endPos - 1, startPos );
    wrapper->WholeSelection().SetSelection( endPos, lineStart );
    wrapper->MessageSelection().SetSelection( endPos - 1, nickSel.HigherPos() );

    CHAT_DP( D_CHAT_LIT( "owncol: %d msgcol: %d" ), iOwnMsgColorInUse, iMsgColorInUse );

    // append coloring
    TCursorSelection& sel = iColorWholeMessage ? wrapper->Selection() : nickSel;
    TInt len( sel.Length() );
    CHAT_DP( D_CHAT_LIT( "selectiong length %d" ), len );

    // Make sure the background (highlight) is not copied from
    // previous message.
    BackColorL( sel, KRgbWhite );
    TextBackColorL( sel, KRgbWhite );

    if ( len > 0 &&
         aMessage.MessageType() != TEnumsPC::EMessageSystem )
        {
        // custom colors
        if ( aMessage.MessagerType() == TEnumsPC::EMessageSent )
            {
            // own msg
            if ( iOwnMsgColorInUse )
                {
                TextColorL( sel, iOwnMsgColor );
                }
            else
                {
                //default color
                TextColorL( sel, iDefaultMsgColor );
                }
            }
        else if ( aMessage.MessagerType() == TEnumsPC::EMessageReceived )
            {
            // other msg
            if ( iMsgColorInUse )
                {
                TextColorL( sel, iMsgColor );
                }
            else
                {
                //default color
                TextColorL( sel, iDefaultMsgColor );
                }
            }

        }


    // inform of inserted rich text
    TCursorSelection curSel( CurrentSelection() );
    TRAPD( error, iTextView.HandleAdditionL( lineStart == 0, curSel, EFalse ) );
    if ( error != KErrNone )
        {
        CHAT_DP_TXT( "CCARichTextContainer::AddMessageL: Recovering from error" );

        // Remove failed text
        TCursorSelection del = wrapper->WholeSelection();
        iRichText->DeleteL( del.LowerPos(), del.Length() );

        if ( error == KErrNoMemory )
            {
            // If OOM => stop processing all our pending messages
            iRecoveringFromOOM = ETrue;
            TInt count = iMessages.Count();
            for ( TInt i = count - 1; i >= 0; --i )
                {
                TRAP_IGNORE( iMessages[i]->Message().RemoveProcessingL() );
                }
            iRecoveringFromOOM = EFalse;
            }
        else
            {
            TRAP_IGNORE( wrapper->Message().RemoveProcessingL() );
            }

        // Remove the failed wrapper and leave
        TInt index = iMessages.Find( wrapper );
        if ( index != KErrNotFound )
            {
            delete wrapper;
            iMessages.Remove( index );
            }

        // Try to handle changes once more
        TRAPD( updateErr, iTextView.HandleGlobalChangeNoRedrawL() );
        if ( !updateErr )
            {
            TCursorSelection curSel( CurrentSelection() );
            TRAP_IGNORE( iTextView.ScrollVisibleL( curSel, EFalse ) );
            }
        User::Leave( error );
        }

    // and scroll (unless in highlight-mode)
    if ( iHighlightState == ENoHighlight )
        {
        TCursorSelection curSel( CurrentSelection() );
        iTextView.ScrollVisibleL( curSel, EFalse );

        // If this is recorded chat container (iScrollOver == EFalse)
        // and added message was first move highlight
        // so we have focus on the topmost item.
        if ( oldCount == 0 && !iScrollOver )
            {
            if ( !iMsgAddedToContentProcessor )
                {
                MoveHighlightL( ENext );
                }
            if ( iAddMsgObserver )
                {
                iAddMsgObserver->HandleMessageAddedL( iMessages.Count() );
                }
            }

        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::MessageChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::MessageChangedL( TInt aIndex )
    {
    CHAT_DP_FUNC_ENTER( "CCARichTextContainer::MessageChangedL" );
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < iMessages.Count() );

    if ( iRecoveringFromOOM )
        {
        // Don't handle changes if we had OOM situation.
        return;
        }

    CCAMessageWrapper* wrapper = iMessages[ aIndex ];

    // we don't know what's changed, so update whole message

    TCursorSelection& thumbPos = wrapper->ThumbPos();
    if ( thumbPos.iAnchorPos != 0 )
        {
        TInt oldLength( thumbPos.Length() );

        // update thumbnail
        if ( thumbPos.Length() > 0 )
            {
            // delete old thumb
            iRichText->DeleteL( thumbPos.iAnchorPos, oldLength );

            TCursorSelection& sel = wrapper->Selection();
            TCursorSelection& wholeSel = wrapper->WholeSelection();

            sel.iCursorPos -= thumbPos.Length();
            wholeSel.iCursorPos -= thumbPos.Length();
            }
        AddThumbL( thumbPos.iAnchorPos, *wrapper );
        TInt newLength = wrapper->ThumbPos().Length();

        // if there are messages after this one,
        // change the selections of those so that they
        // are correct after we've added some characters
        UpdateWrappers( aIndex + 1, newLength - oldLength );

        // inform of deleted and inserted text
        if ( !wrapper->IsInserted() )
            {
            iTextView.HandleInsertDeleteL( wrapper->ThumbPos(), oldLength );
            }
        }

    if ( !wrapper->IsInserted() )
        {
        iTextView.HandleFormatChangedL( wrapper->Selection(),
                                        iHighlightState != ENoHighlight );
        }
    else
        {
        // HandleGlobalChangeNoRedrawL worked before, but not anymore.. dunno why.
        // We have to use this instead.
        TCursorSelection sel = CurrentSelection();
        iTextView.HandleAdditionL( ETrue, sel, EFalse );
        }

    // Scroll view correctly
    TCursorSelection sel = CurrentSelection();
    iTextView.ScrollVisibleL( sel, EFalse );

    //Messages whose content are images
    //have completed the processing
    if ( iMsgAddedToContentProcessor )
        {
        //decrement the messages which has completed processing
        iMsgAddedToContentProcessor--;

        if ( !iScrollOver )
            {
            //only for recorded chats iScrollOver=EFalse
            //put the focus on the first item
            MoveHighlightL( ENext );
            }

        else
            {
            //when user navigate among tab.
            //for conversations/groupview
            if ( ( iHighlightState == ENoHighlight ) &&
                 !iMsgAddedToContentProcessor &&
                 iAddMsgObserver )
                {
                iAddMsgObserver->HandleMessageAddedL( iMessages.Count() );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::RemoveMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::RemoveMessage( TInt aIndex )
    {
    __CHAT_ASSERT_DEBUG( aIndex >= 0 && aIndex < iMessages.Count() );
    CCAMessageWrapper* wrapper = iMessages[ aIndex ];

    TCursorSelection& sel = wrapper->WholeSelection();
    TInt selectionLength( sel.Length() );

    TRAPD( err, iRichText->DeleteL( sel.LowerPos(), selectionLength ) )
    if ( err != KErrNone )
        {
        // nothing else we could do if the rich text deletion,
        // for some reason, didn't succeed.
        CActiveScheduler::Current()->Error( err );
        }
    else
        {
        // inform viewer of deletion
        TCursorSelection startPos( sel.LowerPos(), sel.LowerPos() );
        TRAPD( err, iTextView.HandleInsertDeleteL( startPos, selectionLength ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        iTextView.Redraw();

        // message deleted from rich text, we must update the
        // positions of all messages after this one
        UpdateWrappers( aIndex + 1, -selectionLength );

        // delete wrapper
        delete wrapper;
        iMessages.Remove( aIndex );

        // check selection
        TInt count( iMessages.Count() );
        if ( iSelected >= count )
            {
            iSelected = count - 1;
            if ( iSelected < 0 )
                {
                // no messages, nothing to highlight
                iSelected = 0;
                TRAPD( err, DisableHighlightL() );
                if ( err != KErrNone )
                    {
                    CActiveScheduler::Current()->Error( err );
                    }
                }
            }

        // check scroll position
        TCursorSelection scroll( iTextView.ScrollSelection() );
        TInt docLen( iRichText->DocumentLength() );
        if ( scroll.HigherPos() >= docLen && docLen ) // no need for scroll with zero doclen
            {
            // update if it was wrong
            scroll.iAnchorPos = docLen - 1;
            scroll.iCursorPos = docLen - 1;
            TRAPD( err, iTextView.ScrollVisibleL( scroll, EFalse ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::IsDeleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCARichTextContainer::IsDeleted() const
    {
    return iMessagesDeleted;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::MarkDeleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::MarkDeleted()
    {
    iMessagesDeleted = ETrue;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::InsertMessageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::InsertMessageL( MCAConversationMessage& aMessage )
    {
    if ( !iShowTimeStamps &&
         aMessage.SystemMessageType() == TEnumsPC::ESystemMessageDateChange )
        {
        // if timestamp setting is OFF we don't show day change items
        return;
        }
    // create wrapper
    CCAMessageWrapper* wrapper = CCAMessageWrapper::NewL( aMessage, *this );

    CleanupStack::PushL( wrapper );
    iMessages.InsertL( wrapper, 0 ); // takes the ownership
    CleanupStack::Pop( wrapper );

    if ( aMessage.ContentType() == TEnumsPC::EContentPicture )
        {
        if ( IMUtils::ContentProtectedL( aMessage.ContentData() ) )
            {
            aMessage.SetProcessState( TEnumsPC::EContentNotSupported );
            }
        else
            {
            // add thumbnail processor for message
            aMessage.AddContentProcessorL( iThumbSize );

            //once any message added to the content processor
            //increment this iMsgAddedToContentProcessor
            iMsgAddedToContentProcessor++;

            }
        }

    TInt oldLen = iRichText->DocumentLength();

    // Insert paragraph break if this is not the last message
    TInt paraBreak = 0;
    if ( iMessages.Count() > 1 )
        {
        iRichText->InsertL( 0, CEditableText::EParagraphDelimiter );
        paraBreak = 1;
        }

    // Insert message to rich text in reverse order compared to adding
    TInt contentLen = InsertContentL( *wrapper );
    TCursorSelection nickSel = InsertNickL( *wrapper );
    TCursorSelection timeSel = InsertTimeStampL( *wrapper );
    //	IM client UI customization, phase 2
    //  Based on Variation flag, enables User Identity with Time Stamp
    //  in chat conversation view.

    // read variation flag values
    TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
    TCursorSelection userSel;
    if ( requireUserIdentity )
        {
        userSel = InsertUserIdentityL( *wrapper );
        }

    TInt dirChrLen = iRichText->DocumentLength();
    iRichText->InsertL( 0, AknLayoutUtils::LayoutMirrored() ?
                        KUnicodeRLM : KUnicodeLRM );
    dirChrLen = iRichText->DocumentLength() - dirChrLen;

    wrapper->SetInserted( ETrue );

    // read variation flag values
    //TInt needUserIdentity = IntResourceValueL(RSC_CHAT_VARIATION_SHOW_USER_IDENTITY);
    TInt needUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
    if ( needUserIdentity )
        {
        // Update user selection
        userSel.iAnchorPos += dirChrLen;
        userSel.iCursorPos += dirChrLen;
        // Update timestamp selection
        timeSel.iAnchorPos += userSel.HigherPos();
        timeSel.iCursorPos += userSel.HigherPos();
        // Update nick selection
        nickSel.iAnchorPos += timeSel.HigherPos();
        nickSel.iCursorPos += timeSel.HigherPos();
        }
    else
        {
        // Update timestamp selection
        timeSel.iAnchorPos += dirChrLen;
        timeSel.iCursorPos += dirChrLen;
        // Update nick selection
        nickSel.iAnchorPos += timeSel.HigherPos();
        nickSel.iCursorPos += timeSel.HigherPos();
        }
    // Update thumb pos
    TCursorSelection& thumbPos = wrapper->ThumbPos();
    thumbPos.iAnchorPos += nickSel.HigherPos() + KSeparator().Length();
    thumbPos.iCursorPos += nickSel.HigherPos() + KSeparator().Length();

    TInt addedLen = iRichText->DocumentLength() - oldLen;

    // parse text for highlights and store positions to messagewrapper
    HBufC* text = HBufC::NewMaxLC( contentLen );
    TPtr txt( text->Des() );
    iRichText->Extract( txt,
                        nickSel.HigherPos() + KSeparator().Length() + 1, // Don't extract separator and space
                        contentLen );
    ParseTextL( txt, nickSel.HigherPos() + KSeparator().Length() + 1,
                nickSel.HigherPos() + KSeparator().Length() + 1, *wrapper );
    CleanupStack::PopAndDestroy( text );

    // Set selection for highlight and the whole selection
    // so that this message can be removed
    wrapper->Selection().SetSelection( addedLen - 1 - paraBreak, 0 );
    wrapper->WholeSelection().SetSelection( addedLen - 1, 0 );
    wrapper->MessageSelection().SetSelection( addedLen - 1,
                                              nickSel.HigherPos() + 1 );

    if ( iHighlightState != ENoHighlight )
        {
        BackColorL( wrapper->WholeSelection(), KRgbWhite );
        TextBackColorL( wrapper->WholeSelection(), KRgbWhite );
        }

    // Update wrappers after this if there is any
    if ( iMessages.Count() > 1 )
        {
        UpdateWrappers( 1, addedLen );
        }

    // Append coloring
    TCursorSelection& sel = iColorWholeMessage ? wrapper->Selection() : nickSel;
    TInt len( sel.Length() );
    CHAT_DP( D_CHAT_LIT( "selectiong length %d" ), len );

    if ( len > 0 &&
         aMessage.MessageType() != TEnumsPC::EMessageSystem )
        {
        // Custom colors
        if ( aMessage.MessagerType() == TEnumsPC::EMessageSent )
            {
            // Own msg
            if ( iOwnMsgColorInUse )
                {
                TextColorL( sel, iOwnMsgColor );
                }
            else
                {
                //default color
                TextColorL( sel, iDefaultMsgColor );
                }

            }
        else if ( aMessage.MessagerType() == TEnumsPC::EMessageReceived )
            {
            // Other msg
            if ( iMsgColorInUse )
                {
                TextColorL( sel, iMsgColor );
                }
            else
                {
                //default color
                TextColorL( sel, iDefaultMsgColor );
                }
            }
        }

    if ( len > 0 &&
         aMessage.MessageType() == TEnumsPC::EMessageSystem )
        {
        TextColorL( sel, iDefaultMsgColor );
        }
    // Inform of inserted rich text
    TInt error = KErrNone;
    if ( oldLen == 0 )
        {
        TCursorSelection curSel( CurrentSelection() );
        // First message inserted, format the whole text
        TRAP( error, iTextView.HandleAdditionL( ETrue, curSel, EFalse ) );
        }
    else
        {
        TCursorSelection curSel( CurrentSelection() );
        // There might be a faster method to handle text addition, but
        // this works for now.
        TRAP( error, iTextView.HandleAdditionL( ETrue, curSel, EFalse ) );
        }

    if ( error != KErrNone )
        {
        // Remove the failed wrapper and leave
        wrapper->Message().RemoveProcessingL();
        TInt index = iMessages.Find( wrapper );
        if ( index != KErrNotFound )
            {
            delete wrapper;
            iMessages.Remove( index );
            }
        User::Leave( error );
        }

    // And scroll (unless in highlight-mode)
    if ( iHighlightState == ENoHighlight )
        {
        TCursorSelection curSel( CurrentSelection() );
        iTextView.ScrollVisibleL( curSel, EFalse );

        //if no messages are being processed for the content
        //i.e., all messages are text messages
        if ( !iMsgAddedToContentProcessor && iAddMsgObserver )
            {
            iAddMsgObserver->HandleMessageAddedL( iMessages.Count() );
            }
        }
    else
        {
        // In highlight mode, message inserted -> increase selected
        // message index and previous selection indexes
        iSelected++;
        iPrevious.iAnchorPos += addedLen;
        iPrevious.iCursorPos += addedLen;

        TCursorSelection curSel( CurrentSelection() );
        iTextView.ScrollVisibleL( curSel, EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::UpdateWrappers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::UpdateWrappers( TInt aFirstIndex, TInt aChange )
    {
    TInt count( iMessages.Count() );
    for ( TInt i( aFirstIndex ); i < count; ++i )
        {
        CCAMessageWrapper* wrapper = iMessages[i];
        TCursorSelection& selection = wrapper->Selection();
        TCursorSelection& wholeSelection = wrapper->WholeSelection();
        TCursorSelection& thumbPos = wrapper->ThumbPos();
        selection.iAnchorPos += aChange;
        selection.iCursorPos += aChange;
        wholeSelection.iAnchorPos += aChange;
        wholeSelection.iCursorPos += aChange;
        thumbPos.iAnchorPos += aChange;
        thumbPos.iCursorPos += aChange;
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::Union
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::Union( const TCursorSelection& aSel1,
                                              const TCursorSelection& aSel2 )
    {
    // make union of selections
    TCursorSelection changed;
    changed.iAnchorPos = aSel1.LowerPos();
    changed.iCursorPos = aSel1.HigherPos();

    TInt low(  aSel2.LowerPos() );
    TInt high( aSel2.HigherPos() );
    if ( low < changed.iAnchorPos )
        {
        changed.iAnchorPos = low;
        }
    if ( high > changed.iCursorPos )
        {
        changed.iCursorPos = high;
        }

    return changed;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::CurrentSelection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::CurrentSelection() const
    {
    TCursorSelection sel( 0, 0 );
    if ( iHighlightState == ENoHighlight )
        {
        // if nothing highlighted, return last message
        TInt count( iMessages.Count() );
        if ( count > 0 )
            {
            sel = iMessages[count-1]->Selection();
            }
        }
    else
        {
        // something highlighted, start with current message
        sel = iMessages[iSelected]->Selection();

        // check if the highlighted selection is item inside message
        if ( iHighlightState == EItemSelected )
            {
            // calculate item highlight position
            TCursorSelection itemSel(
                iMessages[iSelected]->Highlights()[iSelectedItem] );

            TInt messageStart( sel.LowerPos() );
            sel.iAnchorPos = itemSel.iAnchorPos + messageStart;
            sel.iCursorPos = itemSel.iCursorPos + messageStart;
            }
        }

    return sel;
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::TextBackColorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::TextBackColorL( const TCursorSelection& aSel,
                                           const TRgb& aColor )
    {
#ifdef _DEBUG
    TInt len( iRichText->DocumentLength() );
    CHAT_DP( D_CHAT_LIT(
                 "CCARichTextContainer::TextBackColorL selection: %d,%d, textlen %d" ),
             aSel.iAnchorPos, aSel.iCursorPos, len );
#endif //_DEBUG

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttFontHighlightColor );
    charFormatMask.SetAttrib( EAttFontHighlightStyle );

    charFormat.iFontPresentation.iHighlightColor = aColor;
    charFormat.iFontPresentation.iHighlightStyle = aColor == KRgbWhite ?
                                                   TFontPresentation::EFontHighlightNone :
                                                   TFontPresentation::EFontHighlightNormal;

    iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                 aSel.LowerPos(),
                                 aSel.Length() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::TextColorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::TextColorL( const TCursorSelection& aSel,
                                       const TRgb& aColor )
    {
#ifdef _DEBUG
    TInt len( iRichText->DocumentLength() );
    CHAT_DP( D_CHAT_LIT(
                 "CCARichTextContainer::TextColorL selection: %d,%d, textlen %d" ),
             aSel.iAnchorPos, aSel.iCursorPos, len );
#endif //_DEBUG
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttColor );
    charFormat.iFontPresentation.iTextColor = aColor;

    iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                 aSel.LowerPos(),
                                 aSel.Length() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::BackColorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::BackColorL( const TCursorSelection& aSel,
                                       const TRgb& aColor )
    {
#ifdef _DEBUG
    TInt len( iRichText->DocumentLength() );
    CHAT_DP( D_CHAT_LIT(
                 "CCARichTextContainer::BackColorL selection: %d,%d, textlen %d" ),
             aSel.iAnchorPos, aSel.iCursorPos, len );
#endif //_DEBUG

    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
    paraFormatMask.SetAttrib( EAttFillColor );

    paraFormat.iFillColor = aColor;
    iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                 aSel.LowerPos(),
                                 aSel.Length() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::HighLightItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::HighLightItemL( const TCursorSelection& aSel )
    {
#ifdef _DEBUG
    TInt len( iRichText->DocumentLength() );
    CHAT_DP( D_CHAT_LIT(
                 "CCARichTextContainer::HighLightItemL selection: %d,%d, textlen %d" ),
             aSel.iAnchorPos, aSel.iCursorPos, len );
#endif //_DEBUG

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttFontUnderline );

    charFormat.iFontPresentation.iUnderline = EUnderlineOn;

    iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                 aSel.LowerPos(),
                                 aSel.Length() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::BoldL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::BoldL( const TCursorSelection& aSel )
    {
#ifdef _DEBUG
    TInt len( iRichText->DocumentLength() );
    CHAT_DP( D_CHAT_LIT(
                 "CCARichTextContainer::BoldL selection: %d,%d, textlen %d" ),
             aSel.iAnchorPos, aSel.iCursorPos, len );
#endif //_DEBUG

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;

    charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
    charFormatMask.SetAttrib( EAttFontStrokeWeight );

    iRichText->ApplyCharFormatL( charFormat, charFormatMask,
                                 aSel.LowerPos(),
                                 aSel.Length() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::ParseTextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::ParseTextL( const TDesC& aText, TInt aStartPos,
                                       TInt aRelativePos, CCAMessageWrapper& aMessageWrapper )
    {
    CFindItemEngine* findItem = CFindItemEngine::NewL( aText,
                                                       CFindItemEngine::TFindItemSearchCase(
                                                           CFindItemEngine::EFindItemSearchPhoneNumberBin |
                                                           CFindItemEngine::EFindItemSearchMailAddressBin |
                                                           CFindItemEngine::EFindItemSearchURLBin/* |
                                CFindItemEngine::EFindItemSearchScheme*/ )
                                                     );

    CleanupStack::PushL( findItem );

    // reset array
    RArray<TCursorSelection>& highlights = aMessageWrapper.Highlights();
    RArray<TInt>& types = aMessageWrapper.HighlightTypes();
    highlights.Reset();
    types.Reset();

    // get found items
    const CArrayFixFlat<CFindItemEngine::SFoundItem>* items =
        findItem->ItemArray();
    TInt count( items->Count() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        const CFindItemEngine::SFoundItem item = items->At( i );

        // selection in rich text
        TCursorSelection realSel( aStartPos + item.iStartPos + item.iLength,
                                  aStartPos + item.iStartPos );

        // relative selection inside one message
        TCursorSelection relativeSel( aRelativePos + item.iStartPos +
                                      item.iLength,
                                      aRelativePos + item.iStartPos );

        highlights.AppendL( relativeSel );
        types.AppendL( item.iItemType );

        // apply highlight item formatting
        HighLightItemL( realSel );
        }

    CleanupStack::PopAndDestroy( findItem );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::AddNickL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::AddNickL(
    CCAMessageWrapper& aMessageWrapper )
    {


    // insert nick
    MCAConversationMessage& message = aMessageWrapper.Message();

    if ( message.FailedMessage() )
        {
        return 	TCursorSelection( 0, 0 );
        }

    TPtrC sender;
    TPtrC recipient;

    if ( message.MessageType() == TEnumsPC::EMessageGroup ||
         message.MessageType() == TEnumsPC::EMessageWhisper )
        {
        sender.Set( message.Sender() );
        recipient.Set( message.Recipient() );
        }
    else
        {
        MCAStoredContacts* contacts =
            CCAStorageManagerFactory::ContactListInterfaceL();
        if ( message.MessagerType() == TEnumsPC::EMessageSent
             && message.MessageType() != TEnumsPC::EMessageSystem )
            {
            sender.Set( contacts->OwnStatus().Identification() );
            }
        else
            {
            sender.Set( contacts->Identification( message.Sender() ) );
            }

        recipient.Set( contacts->Identification( message.Recipient() ) );
        }
    TCursorSelection nickSel( 0, 0 );

    if ( message.MessagerType() == TEnumsPC::EMessageSent &&
         message.MessageType() == TEnumsPC::EMessagePTOP &&
         iOwnMsgIcon )
        {
        // this is sent (or failed) p2p message,
        // insert "from me" icon and separator
        // (if icons are in use)

        //	IM client UI customization, phase 2
        //  Based on Variation flag, changes Nick format in conversation view
        //  removes arrow sign and append only a seperator.

        // read variation flag values
        TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
        // and check dynamic features
        if ( !requireUserIdentity )
            {
            CCAPicture* pic = new( ELeave )CCAPicture( iMap, iFromMe );
            TPictureHeader header;
            header.iPicture = TSwizzle<CPicture>( pic );
            iRichText->CancelInsertCharFormat();    // to prevent ETEXT 31 panic
            iRichText->InsertL( iRichText->DocumentLength(), header ); // takes ownership
            }
        iRichText->InsertL( iRichText->DocumentLength(), KSeparator );
        }
    else
        {
        TInt nickStart( iRichText->DocumentLength() );
        TInt nickEnd( nickStart );

        if ( message.MessageType() == TEnumsPC::EMessageWhisper )
            {
            // This is received whisper message (to us),
            // insert sender, "to me" icon and separator
            iRichText->InsertL( nickStart, sender );
            nickEnd = iRichText->DocumentLength();

            CCAPicture* pic = NULL;
            if ( message.MessagerType() == TEnumsPC::EMessageSent )
                {
                pic = new( ELeave )CCAPicture( iMap,
                                               iFromMe );
                }
            else
                {
                pic = new( ELeave )CCAPicture( iMap,
                                               iToMe );
                }
            TPictureHeader header;
            header.iPicture = TSwizzle<CPicture>( pic );
            iRichText->CancelInsertCharFormat(); // to prevent ETEXT 31 panic
            iRichText->InsertL( nickEnd, header ); // takes ownership
            iRichText->InsertL( iRichText->DocumentLength(), recipient );
            nickEnd = iRichText->DocumentLength();
            iRichText->InsertL( iRichText->DocumentLength(), KSeparator );
            }
        else if ( message.MessageType() == TEnumsPC::EMessagePTOP &&
                  message.MessagerType() == TEnumsPC::EMessageReceived &&
                  iMsgIcon )
            {
            // received p2p message and we want to display icon
            // insert "to me" icon and separator
            //	IM client UI customization, phase 2
            //  Based on Variation flag, changes Nick format in conversation view
            //  removes arrow sign and append only a seperator.

            // read variation flag values
            TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
            // and check dynamic features
            if ( !requireUserIdentity )
                {
                CCAPicture* pic = new( ELeave )CCAPicture( iMap,
                                                           iToMe );
                TPictureHeader header;
                header.iPicture = TSwizzle<CPicture>( pic );
                iRichText->CancelInsertCharFormat(); // to prevent ETEXT 31 panic
                iRichText->InsertL( iRichText->DocumentLength(), header ); // takes ownership
                }
            nickEnd = iRichText->DocumentLength();
            iRichText->InsertL( iRichText->DocumentLength(), KSeparator );
            }
        else if ( sender.Length() > 0 )
            {
            // this is normal group message (or p2p message if icons are not used),
            // insert sender and separator
            iRichText->InsertL( nickStart, sender );
            nickEnd = iRichText->DocumentLength();
            iRichText->InsertL( iRichText->DocumentLength(), KSeparator );
            }

        // formatting (bold nickname)
        nickSel.SetSelection( nickStart, nickEnd );
        if ( nickSel.Length() > 0 )
            {
            BoldL( nickSel );
            }
        }
    return nickSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::AddTimeStampL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::AddTimeStampL(
    CCAMessageWrapper& aMessageWrapper )
    {
    TInt start = iRichText->DocumentLength();
    TCursorSelection timeSel( start, start );

    if ( !iSettings.GetBoolValuePC( TEnumsPC::EShowTimeStamps, NULL ) ||
         aMessageWrapper.Message().SystemMessageType() ==
         TEnumsPC::ESystemMessageDateChange )
        {
        // 1. Setting for time stamp is OFF
        // 2. Special case: date change don't have time stamp.
        return timeSel;
        }

    // format time
    HBufC* timeStamp = HBufC::NewLC( KTimeStampMaxLength + KSpace().Length() );
    TPtr timePtr( timeStamp->Des() );

    HBufC* timeStampFormat = NULL;
    timeStampFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );

    TTime time = aMessageWrapper.Message().TimeStamp();
    TRAPD( err, time.FormatL( timePtr, *timeStampFormat ) );
    if ( err == KErrOverflow )
        {
        // Reserved text space was not enough.
        // Adjust the KTimeStampMaxLength
        __CHAT_ASSERT_DEBUG( EFalse );

        // in release builds return without timestamp
        CleanupStack::PopAndDestroy( 2, timeStamp ); // timeStamp, timeStampFormat
        return timeSel;
        }

    // Convert numbers to local language
    AknTextUtils::LanguageSpecificNumberConversion( timePtr );

    // Set Alignment of Pm/Am from local setting
    // This keeps the am/pm text close to the time.
    timePtr.Append( AknLayoutUtils::LayoutMirrored() ?
                    KUnicodeRLM : KUnicodeLRM );

    //	IM client UI customization, phase 2
    //  Based on Variation flag, changes Time Stamp format in conversation view.
    //  where Time Stamp should appear in brackets.

    // read variation flag values
    TInt requireUserIdentity = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SHOW_USER_IDENTITY );
    // and check dynamic features
    if ( requireUserIdentity )
        {
        // append time to rich text
        timePtr.Insert( 0, _L( "(" ) );
        timePtr.Append( _L( ")" ) );
        timePtr.Append( KSpace );
        }
    else
        {
        timePtr.Append( KSpace );
        }
    iRichText->InsertL( start, timePtr );

    CleanupStack::PopAndDestroy( 2, timeStamp ); // timeStamp, timeStampFormat
    timeSel.SetSelection( start, iRichText->DocumentLength() - 1 );

    // bold the time
    if ( timeSel.Length() > 0 )
        {
        BoldL( timeSel );
        }

    return timeSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::AddUserIdentityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::AddUserIdentityL(
    CCAMessageWrapper& aMessageWrapper )
    {
    TInt start = iRichText->DocumentLength();
    TCursorSelection userSel( start, start );

    MCAConversationMessage& message = aMessageWrapper.Message();
    TPtrC sender;
    TPtrC recipient;
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();
    if ( message.MessagerType() == TEnumsPC::EMessageSent
         && message.MessageType() != TEnumsPC::EMessageSystem )
        {
        sender.Set( contacts->OwnStatus().Identification() );
        }
    else
        {
        sender.Set( contacts->Identification( message.Sender() ) );
        }
    recipient.Set( contacts->Identification( message.Recipient() ) );

    if ( message.MessagerType() == TEnumsPC::EMessageSent &&
         message.MessageType() == TEnumsPC::EMessagePTOP &&
         iOwnMsgIcon )
        {
        if ( sender.Length() >= KMaxLength )
            {
            TPtrC aTempPtr;
            aTempPtr.Set( sender.Left( 8 ) );
            TBuf<10> aTruncateSender( aTempPtr );
            aTruncateSender.Append( _L( ".." ) );
            iRichText->InsertL( start, aTruncateSender );
            }
        else
            {
            iRichText->InsertL( iRichText->DocumentLength(), sender );
            }
        }
    else if ( message.MessageType() == TEnumsPC::EMessagePTOP &&
              message.MessagerType() == TEnumsPC::EMessageReceived &&
              iMsgIcon )
        {
        if ( recipient.Length() >= KMaxLength )
            {
            TPtrC aTempPtr;
            aTempPtr.Set( recipient.Left( 8 ) );
            TBuf<10> aTruncateRecipient( aTempPtr );
            aTruncateRecipient.Append( _L( ".." ) );
            iRichText->InsertL( iRichText->DocumentLength(), aTruncateRecipient );
            }
        else
            {
            iRichText->InsertL( iRichText->DocumentLength(), recipient );
            }
        }
    iRichText->InsertL( iRichText->DocumentLength(), KSpace );
    userSel.SetSelection( start, iRichText->DocumentLength() - 1 );

    // bold the time
    if ( userSel.Length() > 0 )
        {
        BoldL( userSel );
        }

    return userSel;
    }
// -----------------------------------------------------------------------------
// CCARichTextContainer::AddContentL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::AddContentL( TInt aMsgStart,
                                        CCAMessageWrapper& aMessageWrapper )
    {
    MCAConversationMessage& message = aMessageWrapper.Message();

    TEnumsPC::TContentType type( message.ContentType() );
    switch ( type )
        {
        case TEnumsPC::EContentText:
            {
            // insert text
            TPtrC msg( message.Text() );

            HBufC* tempMsg = NULL;

            TInt paraBreak = msg.Locate( CEditableText::EParagraphDelimiter );
            // Replace all paragraph delimiters with line breaks
            // to keep alingment consistent
            if ( paraBreak != KErrNotFound )
                {
                tempMsg = msg.AllocLC();
                TPtr ptr( tempMsg->Des() );
                ptr.Zero();

                ReplaceParaDelimsWithLineBreaks( msg, ptr );
                msg.Set( ptr );
                }

            HBufC* numberConv = NULL;

            // Convert numerals to local language
            // 1) date item
            if ( aMessageWrapper.Message().SystemMessageType() ==
                 TEnumsPC::ESystemMessageDateChange )
                {
                // don't touch the original message
                numberConv = msg.AllocLC();
                TPtr ptr( numberConv->Des() );
                AknTextUtils::LanguageSpecificNumberConversion( ptr );
                msg.Set( ptr );
                }

            TInt textStart( iRichText->DocumentLength() );
            iRichText->InsertL( textStart, msg );

            // check for extensions (smileys)
            TCursorSelection selection( iRichText->DocumentLength(),
                                        textStart );
            iExtensionsHandler.ConvertSelectionToExtensionL( *iRichText,
                                                             selection );

            TInt textEnd( iRichText->DocumentLength() );

            // add nbs to preserve formatting
            iRichText->InsertL( iRichText->DocumentLength(),
                                CEditableText::EZeroWidthNoBreakSpace );

            // if this is system message, it should be bold
            if ( message.MessageType() == TEnumsPC::EMessageSystem )
                {
                TCursorSelection sel( textStart, textEnd );
                BoldL( sel );
                // System messages need different kind of alignment
                CParaFormat paraFormat;
                TParaFormatMask paraFormatMask;
                paraFormat.iLeftMarginInTwips = 0;
                paraFormatMask.SetAttrib( EAttLeftMargin );
                iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                             textStart, sel.Length() );
                }
            else
                {
                // Apply alignment for "normal" messages
                CParaFormat paraFormat;
                TParaFormatMask paraFormatMask;
                paraFormat.iLeftMarginInTwips = 100;
                paraFormatMask.SetAttrib( EAttLeftMargin );
                iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask,
                                             textStart, textEnd - textStart );
                }

            // parse text for highlights and store positions to messagewrapper
            HBufC* text = HBufC::NewMaxLC( textEnd - textStart );
            TPtr txt( text->Des() );
            iRichText->Extract( txt, textStart, textEnd - textStart );
            ParseTextL( txt, textStart, textStart - aMsgStart, aMessageWrapper );
            CleanupStack::PopAndDestroy( text );

            // Cleanup
            if ( numberConv )
                {
                CleanupStack::PopAndDestroy( numberConv );
                }
            if ( tempMsg )
                {
                CleanupStack::PopAndDestroy( tempMsg );
                }
            break;
            }
        case TEnumsPC::EContentPicture: // Flowthrough
        case TEnumsPC::EContentOther:
            {
            // insert thumbnail (if it's ready)
            AddThumbL( iRichText->DocumentLength(), aMessageWrapper );
            break;
            }
        case TEnumsPC::EContentInvalid:
            {
            break;
            }
        default:
            {
            // unsupported type
            __CHAT_ASSERT_DEBUG( false );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::AddThumbL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::AddThumbL( TInt aPos,
                                      CCAMessageWrapper& aMessageWrapper )
    {
    TCursorSelection& thumb = aMessageWrapper.ThumbPos();
    thumb.iAnchorPos = aPos;
    thumb.iCursorPos = aPos;

    MCAConversationMessage& message = aMessageWrapper.Message();
    TEnumsPC::TContentType type = message.ContentType();
    if ( type == TEnumsPC::EContentPicture ||
         type == TEnumsPC::EContentOther )
        {
        // this is a content message
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        switch ( message.ContentProcessState() )
            {
            case TEnumsPC::EThumbnailReady:
                {
                // get the thumbnail from message
                bitmap = message.Thumbnail();
                break;
                }
            case TEnumsPC::EContentNotSupported:
                {
                bitmap = iUnsupported->Bitmap();
                mask = iUnsupported->Mask();
                break;
                }
            case TEnumsPC::EContentCorrupted:
            case TEnumsPC::EContentNotSupportedDrm:
                {
                bitmap = iCorrupted->Bitmap();
                mask = iCorrupted->Mask();
                break;
                }
            default:
                {
                // other types do not have thumbnails.
                return;
                }
            }

        // create gulicon
        CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
        //we don`t own the bitmap, just the CGulIcon
        icon->SetBitmapsOwnedExternally( ETrue );
        CleanupStack::PushL( icon );

        // insert in to rich text
        CCAPicture* pic = new( ELeave )CCAPicture( iMap, icon, KErrNotFound, EFalse );
        CleanupStack::Pop( icon ); //ownership is taken by pic

        TSize thumbnailSize;
        pic->GetOriginalSizeInTwips( thumbnailSize );

        CleanupStack::PushL( pic );
        TPictureHeader header;
        header.iPicture = TSwizzle<CPicture>( pic );
        iRichText->CancelInsertCharFormat();    // to prevent ETEXT 31 panic

        // new paragraph, image and new paragraph
        // 1 char for EParagraphDelimiter
        // 1 char for picture
        // 1 char for EZeroWidthNoBreakSpace
        // 1 char for EParagraphDelimiter
        TInt pos = aPos;
        iRichText->InsertL( pos++, CEditableText::EParagraphDelimiter );
        TInt picStart = pos;
        CleanupStack::Pop( pic );
        iRichText->InsertL( pos++, header ); // takes ownership
        iRichText->InsertL( pos++, CEditableText::EZeroWidthNoBreakSpace );
        TInt picEnd = pos;
        iRichText->InsertL( pos++, CEditableText::EParagraphDelimiter );

        // store length
        thumb.iCursorPos = pos;

        // update wrapper
        TCursorSelection& sel = aMessageWrapper.Selection();
        TCursorSelection& wholeSel = aMessageWrapper.WholeSelection();
        sel.iCursorPos += thumb.Length();
        wholeSel.iCursorPos += thumb.Length();

        // append formatting
        CParaFormat paraFormat;
        TParaFormatMask paraFormatMask;
        paraFormat.iIndentInTwips = 0;
        paraFormat.iLeftMarginInTwips = 100;
        paraFormatMask.SetAttrib( EAttIndent );
        paraFormatMask.SetAttrib( EAttLeftMargin );

        paraFormat.iLineSpacingInTwips = thumbnailSize.iHeight;
        paraFormatMask.SetAttrib( EAttLineSpacing );

        // apply formatting to picture character only
        iRichText->ApplyParaFormatL( &paraFormat, paraFormatMask, picStart,
                                     picEnd - picStart );

        CHAT_DP( D_CHAT_LIT( "*Added thumbnail to %d-%d" ),
                 thumb.iAnchorPos, thumb.iCursorPos );
        }
    }
// -----------------------------------------------------------------------------
// CCARichTextContainer::UpdateSkinnedTextColourL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::UpdateSkinnedTextColourL()
    {
    // Get current text's color
    TRgb defaultSkinTextColor( KRgbBlack );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               defaultSkinTextColor,
                               KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG6 );

    iDefaultMsgColor =  defaultSkinTextColor;
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontPresentation.iTextColor = defaultSkinTextColor;
    charFormatMask.SetAttrib( EAttColor );

    // The color settings is read from the resource file only, so IntResourceValueFromRssL is used instead of IntResourceValueL
    TBool defaultColors = IMUtils::IntResourceValueFromRssL(
                              RSC_CHAT_DEFAULT_MESSAGE_COLORS );

    // Update text colour in all messages
    TInt messagecount = iMessages.Count();
    for ( TInt i = 0; i < messagecount; ++i )
        {
        MCAConversationMessage &msg = iMessages[ i ]->Message();
        TCursorSelection sel( 0, 0 );

        // system message use always default color
        if ( msg.MessageType() == TEnumsPC::EMessageSystem )
            {
            sel = iMessages[ i ]->WholeSelection();
            }
        // Sent messages
        else if ( msg.MessagerType() == TEnumsPC::EMessageSent )
            {
            sel = SkinColoredSelection( *iMessages[ i ],
                                        defaultColors,
                                        iOwnMsgColorInUse );
            }
        // Received messages
        else if ( msg.MessagerType() == TEnumsPC::EMessageReceived )
            {
            sel = SkinColoredSelection( *iMessages[ i ],
                                        defaultColors,
                                        iMsgColorInUse );
            }

        // Only update texts which use default color.
        if ( sel.Length() > 0 )
            {
            iRichText->ApplyCharFormatL( charFormat,
                                         charFormatMask,
                                         sel.LowerPos(),
                                         sel.Length() );
            }
        }
    // also appended messages should have new colour
    iRichText->SetInsertCharFormatL( charFormat, charFormatMask, iRichText->DocumentLength() );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::SkinColoredSelection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection CCARichTextContainer::SkinColoredSelection(
    CCAMessageWrapper& aMessageWrapper,
    TBool aUseDefaultColors,
    TBool aCustomDefinedColor )
    {
    // This logic determines which part of the message is colored by the theme
    if ( !aCustomDefinedColor && aUseDefaultColors )
        {
        return aMessageWrapper.WholeSelection();
        }

    if ( !iColorWholeMessage )
        {
        return aMessageWrapper.MessageSelection();
        }

    return TCursorSelection( 0, 0 );
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::ReplaceParaDelimsWithLineBreaks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::ReplaceParaDelimsWithLineBreaks(
    const TPtrC& aSource,
    TPtr& aTarget ) const
    {
#ifdef _DEBUG
    TPtrC target;
    target.Set( aTarget );
    CHAT_DP( D_PLAIN_LIT( "CCARichTextContainer::ReplaceParaDelimsWithLineBreaks: target =  %S" ), &target );
#endif

    TInt paraBreak = aSource.Mid( aTarget.Length() ).Locate(
                         CEditableText::EParagraphDelimiter );

    if ( paraBreak == KErrNotFound )
        {
        // No more paragraph delimiters, append what's left
        aTarget.Append( aSource.Mid( aTarget.Length() ) );
        // And break away from recursion
        return;
        }

    aTarget.Append( aSource.Mid( aTarget.Length(), paraBreak ) );
    aTarget.Append( CEditableText::ELineBreak );

#ifdef _DEBUG
    target.Set( aTarget );
    CHAT_DP( D_PLAIN_LIT( "CCARichTextContainer::ReplaceParaDelimsWithLineBreaks: target =  %S" ), &target );
#endif

    ReplaceParaDelimsWithLineBreaks( aSource, aTarget );
    }


// -----------------------------------------------------------------------------
// CCARichTextContainer::GetMessageCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARichTextContainer::GetMessageCount()
    {
    return iMessages.Count();
    }
// -----------------------------------------------------------------------------
// CCARichTextContainer::GetMessageCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::ChangeHighlightColorL()
    {
    // get the selection
    TCursorSelection curSel( CurrentSelection() );

    // remove colors from old highlight
    TInt textLen( iRichText->DocumentLength() );
    if ( iPrevious.iAnchorPos < textLen &&
         iPrevious.iCursorPos <= textLen )
        {
        BackColorL( iPrevious, KRgbWhite );
        TextBackColorL( iPrevious, KRgbWhite );
        }

    // and set new one
    if ( iHighlightState == EItemSelected )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            TextBackColorL( curSel, color );
            }
        else
            {
            TextBackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }
    else
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color;

        TInt error = AknsUtils::GetCachedColor( skin,
                                                color,
                                                KAknsIIDQsnHighlightColors,
                                                EAknsCIQsnHighlightColorsCG2 );

        if ( !error )
            {
            // No error, use skinned background color
            BackColorL( curSel, color );
            }
        else
            {
            BackColorL( curSel, AKN_LAF_COLOR_STATIC( KChatHighlightColor ) );
            }
        }

    // handle changed format
    TCursorSelection changed( Union( iPrevious, curSel ) );

    iTextView.HandleFormatChangedL( changed );

    iTextView.ScrollVisibleL( curSel, ETrue );

    iPrevious = curSel;
    }

// -----------------------------------------------------------------------------
// CCARichTextContainer::UnRegisterRichTxtCtrlObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARichTextContainer::UnRegisterRichTxtCtrlObserver()
    {
    iAddMsgObserver = NULL;
    }

//  End of File
