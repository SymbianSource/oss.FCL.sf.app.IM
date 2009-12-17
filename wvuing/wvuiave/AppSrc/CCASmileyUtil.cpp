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
* Description:  Utility for selecting smile icon and converting
*                between smile id and smile string.
*
*/


// INCLUDE FILES
#include 	"CCASmileyUtil.h"
#include 	"CCAPicture.h"
#include    "ChatDefinitions.h"
#include	"ChatDebugPrint.h"
#include    "IMUtils.h"
#include    "ccasmileyinformation.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include	<PUAcodes.hrh>

#include	<gulicon.h>
#include 	<txtrich.h>
#include 	<gdi.h>
#include    <eikrted.h>
#include	<frmtlay.h>
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCASmileyUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCASmileyUtil* CCASmileyUtil::NewL(  MCASkinVariant& aSkinVariant,
                                     MCAAppUi& aAppUi, TMessageExtensionTypes aType )
    {
    CCASmileyUtil* self = new ( ELeave ) CCASmileyUtil( aType );
    CleanupStack::PushL( self );
    self->ConstructL( aSkinVariant, aAppUi );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::~CCASmileyUtil
// Destructor
// -----------------------------------------------------------------------------
//
CCASmileyUtil::~CCASmileyUtil()
    {
    iConvertArray.ResetAndDestroy();
    delete iSmileUtil;
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::CCASmileyUtil
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASmileyUtil::CCASmileyUtil( TMessageExtensionTypes aType )
        : MCAMessageExtension( aType )
    {
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::ConstructL
// ConstructL
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::ConstructL( MCASkinVariant& aSkinVariant,
                                MCAAppUi& aAppUi )
    {
    iSmileUtil = CCASmileIconUtility::NewL( aSkinVariant, aAppUi );
    }

// ---------------------------------------------------------
// CCASmileyUtil::InsertExtensionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileyUtil::InsertExtensionL( CEikRichTextEditor& aEditor,
                                      TBool& aCancelled )
    {
    TPictureHeader picHeader;
    TInt bitmapId( KErrNone );
    TInt retVal = iSmileUtil->LaunchSmileIconDialogL( bitmapId );
    if ( retVal )
        {
        // check if there is room for icon, if not return
        if ( ( aEditor.MaxLength() - aEditor.TextLength() ) < KChatSmileyLength )
            {
            return;
            }

        //we don't own this
        CGulIcon* icon = const_cast<CGulIcon*>(
                             iSmileUtil->GetSmileIcon( bitmapId ) );

        //create CCAPicture from selected icon
        CCAPicture* smileIcon = new ( ELeave ) CCAPicture(
            *CCoeEnv::Static()->SystemGc().Device(),
            icon,
            bitmapId );

        picHeader.iPicture = TSwizzle<CPicture>( smileIcon );
        TInt cursorPos = aEditor.CursorPos();

        // insert smiley to editor
        aEditor.RichText()->InsertL( cursorPos, picHeader );

        // icon is inserted, and it should be converted to text
        // before sending, so it needs extra chars according to smiley string length
        TInt textLimit = aEditor.MaxLength() -
                         iSmileUtil->GetSmileString( bitmapId ).Length() + 1; // 1 to take icon
        // length into count
        aEditor.SetTextLimit( textLimit );

        // apply formatting
        TCharFormat charFormat;
        TCharFormatMask charFormatMask;
        charFormat.iFontPresentation.iPictureAlignment =
            TFontPresentation::EAlignCentered;
        charFormatMask.SetAttrib( EAttFontPictureAlignment );
        aEditor.RichText()->ApplyCharFormatL( charFormat, charFormatMask, 0,
                                              aEditor.TextLength() );

        if ( &aEditor != iEditor )
            {
            iEditor = &aEditor;
            iCurrentCount = 1;
            }
        else
            {
            ++iCurrentCount;
            }

        aCancelled = EFalse;
        }
    else
        {
        aCancelled = ETrue;
        }
    }

// ---------------------------------------------------------
// CCASmileyUtil::ExtensionToStringL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileyUtil::ExtensionToStringL( CEikRichTextEditor& anEditor, TDes& aDes )
    {
    RArray<TInt> iconIndArray;
    CleanupClosePushL( iconIndArray );

    CPicture* editorPic;
    TInt i;
    TInt limit( 0 );

    // for loop gets icon positions to array
    TInt length = aDes.Length();
    for ( i = 0; i < length; ++i )
        {
        if ( aDes[ i ] ==  CEditableText::EPictureCharacter )
            {
            User::LeaveIfError( iconIndArray.Append( i ) );
            }
        }

    // real count of pictures
    TInt count = anEditor.RichText()->PictureCount();
    TInt indCount = iconIndArray.Count();

    if ( &anEditor != iEditor )
        {
        Reset();
        }

    // whisper message
    if ( indCount > iCurrentCount )
        {
        limit = indCount - iCurrentCount;
        }

    if ( indCount > 0 )
        {
        for ( i = count; i > limit; --i )
            {
            editorPic = anEditor.RichText()->PictureHandleL(
                            iconIndArray[ i - 1 ], MLayDoc::EForceLoadTrue );
            if ( !editorPic )
                {
                User::Leave( KErrBadHandle );
                }

            CCAPicture* icon = static_cast<CCAPicture*> ( editorPic );

            TInt oldLen = aDes.Length();

            TInt iconIndex = icon->Index();

            if ( iconIndex >= 0 && iconIndex < iSmileUtil->SmileIconCount() )
                {
                //delete padding char
                aDes.Delete( iconIndArray[ i - 1 ], 1 ); // 1 = one char
                //insert string smiley
                aDes.Insert( iconIndArray[ i - 1 ],
                             iSmileUtil->GetSmileString( icon->Index() ) );
                }

            }
        }
    CleanupStack::PopAndDestroy(); // iconIndArray.Close()
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::ConvertSelectionToExtensionL
// Convert selection smileys to icons.
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::ConvertSelectionToExtensionL( CEikRichTextEditor& aEditor )
    {
    TInt selectAdd( 0 );

    // for loop gets icon positions to array
    TInt start = aEditor.Selection().LowerPos();
    TInt end = aEditor.Selection().HigherPos();

    // Clear convert array, it has something, but its not correct
    iConvertArray.ResetAndDestroy();

    HBufC* des = HBufC::NewLC( IMUtils::MaxMsgLength() );
    TPtr desText( des->Des() );

    aEditor.GetText( desText );
    TInt txtLen = desText.Length();
    if ( txtLen < end )
        {
        end = txtLen;
        }
    desText.Delete( end, txtLen - end );
    desText.Delete( 0, start );

    iSmileUtil->SearchSmilesL( desText, iConvertArray, NULL, start );

#ifdef _DEBUG
    TPtrC myDebug( aEditor.Text()->Read( 0 ) );
    CHAT_DP( D_CHAT_LIT( "CCASmileyUtil::ConvertSelectionToExtensionL 3- %S" ), &myDebug );
#endif

    for ( TInt a( iConvertArray.Count() - 1 ); a >= 0; --a )
        {
        if ( iConvertArray[ a ]->Position() >= start && iConvertArray[ a ]->Position() < end ) // do we convert or not
            {
            TPictureHeader picHeader;
            // Insert icon in place

            //we don't own this
            CGulIcon* icon = const_cast<CGulIcon*>(
                                 iSmileUtil->GetSmileIcon( iConvertArray[ a ]->Index() ) );

            //create CCAPicture from selected icon
            CCAPicture* smileIcon = new ( ELeave ) CCAPicture(
                *CCoeEnv::Static()->SystemGc().Device(),
                icon,
                iConvertArray[ a ]->Index() );

            picHeader.iPicture = TSwizzle<CPicture>( smileIcon );
            aEditor.Text()->DeleteL( iConvertArray[ a ]->Position(), iConvertArray[ a ]->SmileyString().Length() );

#ifdef _DEBUG
            myDebug.Set( aEditor.Text()->Read( 0 ) );
            CHAT_DP( D_CHAT_LIT( "CCASmileyUtil::ConvertSelectionToExtensionL 3b %S %d" )
                     , &myDebug, iConvertArray[ a ]->Position() );
#endif

            aEditor.RichText()->InsertL( iConvertArray[ a ]->Position(), picHeader );

#ifdef _DEBUG
            myDebug.Set( aEditor.Text()->Read( 0 ) );
            CHAT_DP( D_CHAT_LIT( "CCASmileyUtil::ConvertSelectionToExtensionL 3c %S" ), &myDebug );
#endif

            selectAdd -= iConvertArray[ a ]->SmileyString().Length() - 1;

            // icon is inserted, and it should be converted to text
            // before sending, so it needs extra chars according to smiley string length
            TInt textLimit =
                aEditor.MaxLength() - iConvertArray[ a ]->SmileyString().Length();
            TInt currentlength = aEditor.Text()->DocumentLength();
            aEditor.SetTextLimit( Max( textLimit, currentlength ) );

            if ( &aEditor != iEditor )
                {
                iEditor = &aEditor;
                iCurrentCount = 1;
                }
            else
                {
                ++iCurrentCount;
                }
            }
        }

    // Apply formatting
    TInt selectionLen = end + selectAdd - start;
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontPresentation.iPictureAlignment =
        TFontPresentation::EAlignCentered;
    charFormatMask.SetAttrib( EAttFontPictureAlignment );
    aEditor.RichText()->ApplyCharFormatL( charFormat, charFormatMask,
                                          start, selectionLen );

    // Handle changes in editor
    aEditor.TextView()->HandleGlobalChangeL();

    // Set selection to correct after adding images
    aEditor.SetSelectionL( start, end + selectAdd );

    // Clear convert array
    iConvertArray.ResetAndDestroy();

    CleanupStack::PopAndDestroy( des );
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::ConvertSelectionToExtensionL
// Convert selection smileys to icons.
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::ConvertSelectionToExtensionL( CRichText& aRichText, TCursorSelection& aSelection )
    {
    TInt start( aSelection.LowerPos() );
    TInt length( aSelection.Length() );

    // Clear convert array and fetch the text
    iConvertArray.ResetAndDestroy();
    HBufC* text = HBufC::NewMaxLC( aRichText.DocumentLength() );
    TPtr desText( text->Des() );
    aRichText.Extract( desText, start, length );

    iSmileUtil->SearchSmilesL( desText, iConvertArray );

    for ( TInt a( iConvertArray.Count() - 1 ); a >= 0; --a )
        {
        TPictureHeader picHeader;
        // Insert icon in place
        //we don't own this
        CGulIcon* icon = const_cast<CGulIcon*>(
                             iSmileUtil->GetSmileIcon( iConvertArray[ a ]->Index() ) );

        //create CCAPicture from selected icon
        CCAPicture* smileIcon = new ( ELeave ) CCAPicture(
            *CCoeEnv::Static()->SystemGc().Device(),
            icon,
            iConvertArray[ a ]->Index() );

        picHeader.iPicture = TSwizzle<CPicture>( smileIcon );
        aRichText.DeleteL( iConvertArray[ a ]->Position() + start,
                           iConvertArray[ a ]->SmileyString().Length() );

        aRichText.InsertL( iConvertArray[ a ]->Position() + start, picHeader );
        }

    // clean up
    iConvertArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::ConvertSelectionToStringL
// Convert selection smileys to strings
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::ConvertSelectionToStringL( CEikRichTextEditor& aEditor, TBool aPreserve )
    {
    // Clear convert array
    iConvertArray.ResetAndDestroy();

    HBufC* text = HBufC::NewMaxLC( IMUtils::MaxMsgLength() );
    TPtr textCopy( text->Des() );
    aEditor.GetText( textCopy );

    // for loop gets icon positions to array
    TInt start = aEditor.Selection().LowerPos();
    TInt end = aEditor.Selection().HigherPos();

    TInt i;
    for ( i = start; i < end; ++i )
        {
        if ( textCopy[ i ] ==  CEditableText::EPictureCharacter )
            {
            CCASmileyInformation* smileInfo = CCASmileyInformation::NewL();
            CleanupStack::PushL( smileInfo );
            smileInfo->SetPosition( i );
            smileInfo->SetIcon( ETrue );
            User::LeaveIfError( iConvertArray.Append( smileInfo ) );
            CleanupStack::Pop( smileInfo );
            }
        }

    TInt count = iConvertArray.Count();
    for ( i = count - 1; i >= 0; --i )
        {
        CPicture* editorPic = aEditor.RichText()->PictureHandleL(
                                  iConvertArray[ i ]->Position(), MLayDoc::EForceLoadTrue );

        if ( !editorPic )
            {
            User::Leave( KErrBadHandle );
            }

        CCAPicture* icon = static_cast<CCAPicture*> ( editorPic );

        const TDesC& iconString = iSmileUtil->GetSmileString( icon->Index() );
        iConvertArray[ i ]->SetSmileyStringL( iconString );
        iConvertArray[ i ]->SetIndex( icon->Index() );
        }

    textCopy.Delete( end, textCopy.Length() - end );
    textCopy.Delete( 0, start );
    iSmileUtil->SearchSmilesL( textCopy, iConvertArray, NULL, start );

    // Change icons to strings
    TPtrC myDebug( aEditor.Text()->Read( 0 ) );
    CHAT_DP( D_CHAT_LIT( "CCASmileyUtil::ConvertSelectionToStringL %S" ), &myDebug );
    TInt selectAdd( 0 );
    count = iConvertArray.Count();
    for ( i = count - 1; i >= 0; --i )
        {
        if ( iConvertArray[ i ]->IsIcon() ) // Icon
            {
            TCursorSelection deleteSel( iConvertArray[ i ]->Position(),
                                        iConvertArray[ i ]->Position() + 1 );

            aEditor.InsertDeleteCharsL( iConvertArray[ i ]->Position(),
                                        iConvertArray[ i ]->SmileyString(),
                                        deleteSel );
            selectAdd += iConvertArray[ i ]->SmileyString().Length() - 1;
            aEditor.SetMaxLength( aEditor.MaxLength() + selectAdd );
            }
        }
    myDebug.Set( aEditor.Text()->Read( 0 ) );
    CHAT_DP( D_CHAT_LIT( "CCASmileyUtil::ConvertSelectionToStringL %S" ), &myDebug );

    aEditor.SetSelectionL( start, end + selectAdd );
    aEditor.TextView()->HandleGlobalChangeL();

    if ( !aPreserve )
        {
        iConvertArray.ResetAndDestroy();
        }

    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// CCASmileyUtil::DeleteExtensionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileyUtil::DeleteExtensionL( CEikRichTextEditor& aEditor, TInt aPos )
    {
    // Set cursor position before deletion.
    // Deletion may cause editor size to change and if
    // cursor pos is not updated before that ETEXT 12 panic
    // is raised.
    aEditor.SetCursorPosL( aPos, EFalse );
    aEditor.RichText()->DeleteL( aPos, 1 );
    aEditor.HandleTextChangedL();
    // Needed to keep alignment of
    // cursor correct.
    aEditor.SetCursorPosL( aPos, EFalse );

    // update editors length, it grows by 2 when icon is deleted
    TInt textLimit = aEditor.MaxLength() + 2;
    aEditor.SetTextLimit( textLimit );

    --iCurrentCount;
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::Reset
// Reset extension
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::Reset()
    {
    iCurrentCount = 0;
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::SizeChanged
// Reset extension
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::SizeChanged( TSize& aSize )
    {
    iSmileUtil->ResizeIcons( aSize );
    }

// -----------------------------------------------------------------------------
// CCASmileyUtil::ConvertSelectionToStringL
// Convert selection smileys to strings
// -----------------------------------------------------------------------------
//
void CCASmileyUtil::ConvertSelectionToStringL(
    CEikRichTextEditor& aEditor,
    TDes& aResultString,
    TCursorSelection& aSelectionAfterConversion,
    TBool aPreserve )
    {
    // Clear convert array
    iConvertArray.ResetAndDestroy();

    // Get text from editor
    HBufC* text = HBufC::NewMaxLC( IMUtils::MaxMsgLength() );
    TPtr textCopy( text->Des() );
    aEditor.GetText( textCopy );

    // For loop gets icon positions to array
    TInt start = aEditor.Selection().LowerPos();
    TInt end = aEditor.Selection().HigherPos();

    // Copy text to result string
    aResultString.Append( textCopy );

    TInt i = 0;
    for ( i = start; i < end; ++i )
        {
        if ( textCopy[i] ==  CEditableText::EPictureCharacter )
            {
            CCASmileyInformation* smileInfo = CCASmileyInformation::NewL();
            CleanupStack::PushL( smileInfo );
            smileInfo->SetPosition( i );
            smileInfo->SetIcon( ETrue );
            User::LeaveIfError( iConvertArray.Append( smileInfo ) );
            CleanupStack::Pop( smileInfo );
            }
        }

    TInt count = iConvertArray.Count();
    for ( i = count - 1; i >= 0; --i )
        {
        CPicture* editorPic = NULL;

        editorPic = aEditor.RichText()->PictureHandleL(
                        iConvertArray[i]->Position(), MLayDoc::EForceLoadTrue );

        if ( !editorPic )
            {
            User::Leave( KErrBadHandle );
            }

        CCAPicture* icon = static_cast<CCAPicture*> ( editorPic );

        const TDesC& iconString = iSmileUtil->GetSmileString( icon->Index() );
        iConvertArray[i]->SetSmileyStringL( iconString );
        iConvertArray[i]->SetIndex( icon->Index() );
        }

    textCopy.Delete( end, textCopy.Length() - end );
    textCopy.Delete( 0, start );
    iSmileUtil->SearchSmilesL( textCopy, iConvertArray, NULL, start );

    // Change icons to strings
    TInt selectAdd = 0;
    count = iConvertArray.Count();
    for ( i = count - 1; i >= 0; --i )
        {
        if ( iConvertArray[ i ]->IsIcon() ) // Icon
            {
            // Append smiley string to result
            aResultString.Delete( iConvertArray[i]->Position(), 1 );
            aResultString.Insert( iConvertArray[i]->Position(), iConvertArray[i]->SmileyString() );
            selectAdd += iConvertArray[ i ]->SmileyString().Length() - 1;
            }
        }

    if ( !aPreserve )
        {
        iConvertArray.ResetAndDestroy();
        }

    // Set correct selection
    aSelectionAfterConversion.SetSelection( end + selectAdd, start );

    CleanupStack::PopAndDestroy( text );
    }
// End of File
