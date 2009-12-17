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
* Description:  Chat client's editor.
*
*/


// INCLUDE FILES
#include "CCAMessageEditor.h"
#include "ChatDefinitions.h"
#include "CCAMessageExtensionsHandler.h"
#include "IMUtils.h"
#include "IMDialogUtils.h"
#include "mcatapeventobserver.h"

#include <eikenv.h>
#include <aknutils.h>
#include <txtrich.h> // CRichText
#include <chatNG.rsg>
#include <eikon.rsg>    // R_EIK_EDWIN_CTRL_HOTKEYS & R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS
#include <AknsBasicBackgroundControlContext.h>
#include "CCAEditIndicator.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// CONSTANTS

// TKeyEvent.iCode limit for hotkeys, see DoOfferKeyEvenL in eikedwin.cpp
const TInt KChatKeyEventHotkeyLimit = 100;
// Buffer descriptor size for
// R_EIK_EDWIN_CTRL_HOTKEYS & R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS
// see DoOfferKeyEvenL in eikedwin.cpp
const TInt KChatResourceBufferSize = 24;

// Amount of extra characters every smiley takes in the editor
const TInt KChatAddCharsPerSmiley = 2;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAMessageEditor::CCAMessageEditor(
    CCAMessageExtensionsHandler& aMessageHandler )
        : iMessageHandler( aMessageHandler )
    {
    }

// Symbian OS default constructor can leave.
void CCAMessageEditor::ConstructL( const CCoeControl* aParent, TInt
                                   aNumberOfLines, TInt aTextLimit, TInt aEdwinFlags,
                                   TInt aFontControlFlags,
                                   TInt aFontNameFlags )
    {

    CEikRichTextEditor::ConstructL( aParent, aNumberOfLines, aTextLimit,
                                    aEdwinFlags, aFontControlFlags, aFontNameFlags );
    }


// Destructor
CCAMessageEditor::~CCAMessageEditor()
    {
    delete iBgContext;
    delete iFgContext;
    delete iClipboardBuffer;
    }

// ---------------------------------------------------------
// CCAMessageEditor::NumberOfLines()
// This method returns the number of lines if expanded
// ---------------------------------------------------------
//
TInt CCAMessageEditor::NumberOfLines() const
    {
    return iNumberOfLines;
    }


// ---------------------------------------------------------
// CCAMessageEditor::ResetL()
// This method clears the editor
// ---------------------------------------------------------
//
void CCAMessageEditor::ResetL()
    {
    iTextView->EnablePictureFrameL( EFalse );
    TBuf< KTwo > str;
    str.Zero();
    SetCursorPosL( 0, EFalse );
    SetTextL( &str );
    HandleTextChangedL();
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::SetViewRect
// Set view rect.
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::SetViewRect( const TRect& aRect )
    {
    iViewRect = aRect;
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CCAMessageEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aKeyEvent.iScanCode == EStdKeyYes && aType != EEventKey )
        {
        return EKeyWasConsumed;
        }

    TInt oldLen = TextLength();

    if ( TextLength() == MaxLength() )
        {
        if (  ( aType == EEventKey &&
                ( aKeyEvent.iCode != EKeyBackspace &&
                  aKeyEvent.iCode < EKeyDelete  )
              ) ||
              ( aType == EEventKeyUp &&
                aKeyEvent.iScanCode >= '0' &&
                aKeyEvent.iScanCode <= '9'
              )
           )
            {
            HBufC* prompt = iEikonEnv->AllocReadResourceLC(
                                R_QTN_CHAT_MAXIMUM_MESSAGE_LENGTH );
            IMDialogUtils::DisplayInformationNoteL(
                *prompt );
            CleanupStack::PopAndDestroy( prompt );

            return EKeyWasConsumed;
            }
        }

    TKeyResponse res = EKeyWasNotConsumed;

    // --------------------
    // This logic comes from CEikEdwin
    // We need to handle hot keys cut and copy
    TBool select = aKeyEvent.iModifiers & EModifierShift;
    TBool magnify = aKeyEvent.iModifiers & EModifierCtrl;
    if ( magnify && aKeyEvent.iCode < KChatKeyEventHotkeyLimit
         && aKeyEvent.iCode != ' ' )
        {
        TBuf<KChatResourceBufferSize> buf;
        if ( select )
            {
            iCoeEnv->ReadResourceL( buf, R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS );
            }
        else
            {
            iCoeEnv->ReadResourceL( buf, R_EIK_EDWIN_CTRL_HOTKEYS );
            }

        const TInt ret = buf.Locate( TChar( aKeyEvent.iCode + 'a' - 1 ) );
        switch ( ret )
            {
            case EHotKeyCut:
                {
                CcpuCutL();
                res = EKeyWasConsumed;
                break;
                }
            case EHotKeyCopy:
                {
                CcpuCopyL();
                res = EKeyWasConsumed;
                break;
                }
            default:
                {
                // No need to handle other hot keys here
                break;
                }
            }
        }
    // --------------------

    if ( res != EKeyWasConsumed )
        {
        res = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
        }

    NotifyObserverIfNeededL( oldLen, TextLength() );

    return res;
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    TBool eventConsumed = EFalse;
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        // Select smiley if user taps on it
        TRect pictRect;
        TPoint pointerPos( aPointerEvent.iPosition );
        if ( iTextView->GetPictureRectangleL( pointerPos, pictRect ) )
            {
            eventConsumed = ETrue;
            TInt pos = iTextView->XyPosToDocPosL( pointerPos );
            SetSelectionL( pos, pos + 1 );
            IgnoreEventsUntilNextPointerUp();
            }
        }

    // If user didn't start stylus down event on smiley,
    // forward all events to base class
    if ( !eventConsumed )
        {
        CEikRichTextEditor::HandlePointerEventL( aPointerEvent );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::CcpuCutL
// Handle cut
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::CcpuCutL()
    {
    if ( Selection().Length() == 0 )
        {
        return;
        }

    TInt oldLen = TextLength();

    // Set up buffer
    ConstructAndSetUpClipboardBufferL();

    // Perform cut in buffer
    iClipboardBuffer->CcpuCutL();

    // Delete selection from UI
    TCursorSelection curSel = Selection();
    TInt delLen = curSel.Length();
    RichText()->DeleteL( curSel.LowerPos(), delLen );
    // Create selection for HandleInsertDeleteL
    TInt delStart = Min( curSel.iAnchorPos, curSel.iCursorPos );
    TCursorSelection delSel( delStart, delStart );
    TextView()->HandleInsertDeleteL( delSel, delLen );
    ClearSelectionL();

    // Set limit to new. Because smileys take 2-3 characters in string format
    // so we have to multiply picture count with extra character count to get
    // possible limit.
    SetTextLimit( IMUtils::MaxMsgLength() - ( RichText()->PictureCount() *
                                              KChatAddCharsPerSmiley ) );

    NotifyObserverIfNeededL( oldLen, TextLength() );
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::CcpuCopyL
// Handle copy
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::CcpuCopyL()
    {
    // Set up buffer
    ConstructAndSetUpClipboardBufferL();

    // Perform copy in buffer
    iClipboardBuffer->CcpuCopyL();
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::CcpuPasteL
// Handle paste
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::CcpuPasteL()
    {
    TInt oldLen = TextLength();

    // Take original cursorpos in memory
    TInt pos = CursorPos();

    // Paste
    CEikRichTextEditor::CcpuPasteL();

    // Select changed part so we can check it for extensions
    SetSelectionL( pos, CursorPos() );

    // Convert strings to extensions
    iMessageHandler.ConvertSelectionToExtensionL( *this );

    // Update cursor position.
    SetCursorPosL( Selection().HigherPos(), EFalse );

    NotifyObserverIfNeededL( oldLen, TextLength() );
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::SetupSkinContextL
// Enable skinning
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::SetupSkinContextL( TBool aForceUpdate /*= EFalse*/ )
    {
    TRect screen( 0, 0, 0, 0 );

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    // If update forced, delete previous skin contexts
    if ( aForceUpdate )
        {
        delete iBgContext;
        iBgContext = NULL;
        delete iFgContext;
        iFgContext = NULL;
        }

    if ( !iFgContext && !iBgContext )
        {
        iBgContext = CAknsBasicBackgroundControlContext::NewL(
                         KAknsIIDQsnBgAreaMain, screen, EFalse );
        iFgContext = CAknsBasicBackgroundControlContext::NewL(
                         KAknsIIDQsnFrInputCenter, screen, EFalse );
        iFgContext->SetParentContext( iBgContext );
        }
    else
        {
        iFgContext->SetRect( screen );
        }

    SetSkinBackgroundControlContextL( iFgContext );
    }

TTypeUid::Ptr CCAMessageEditor::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iFgContext );
        }
    return CEikRichTextEditor::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::RefreshEditorL
// Called to force refresh of editor contents after inserting a smiley
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::RefreshEditorL()
    {
    iTextView->HandleCharEditL( CTextLayout::EFCharacterInsert );
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::SetTapObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::SetTapObserver( MCATapEventObserver* aObserver, TUint aId )
    {
    iTapObserver = aObserver;
    iTapControlId = aId;
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::SetMessageEditorObserver
// Called to force refresh of editor contents after inserting a smiley
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::SetMessageEditorObserver( MCAMessageEditorObserver* aObserver )
    {
    iEditorObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::NotifyObserverIfNeededL
// Called to force refresh of editor contents after inserting a smiley
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::NotifyObserverIfNeededL( TInt aOldLen, TInt aCurLen )
    {
    if ( iEditorObserver )
        {
        if ( !iEditIndicatorActiveObj )
            {
            TInt editorFlags = 0;

            if ( !aCurLen )
                {
                //dont show the edit indicator if editor is empty
                editorFlags = ( AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
                }
            else
                {
                //show the edit indicator if editor is non-empty
                TInt editorFlags = ( AknEdwinFlags() & ~EAknEditorFlagNoEditIndicators );
                }

            SetAknEditorFlags( editorFlags );
            }
        if ( aCurLen == 0 && aOldLen > 0 )
            {
            iEditorObserver->HandleMessageEditorEventL(
                MCAMessageEditorObserver::EEditorContentCleared );
            return;
            }
        if ( aOldLen == 0 && aCurLen > 0 )
            {
            iEditorObserver->HandleMessageEditorEventL(
                MCAMessageEditorObserver::EEditorFirstContentAdded );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::EditObserver
// Ignored to prevent odd scrolling
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::EditObserver( TInt /* aStartEdit */, TInt /* aEditLength */ )
    {
    // ignored to prevent odd scrolling
    }

// -----------------------------------------------------------------------------
// CCAMessageEditor::ConstructAndSetUpClipboardBufferL
// Construct if needed and set up clipboard buffer
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::ConstructAndSetUpClipboardBufferL()
    {
    if ( !iClipboardBuffer )
        {
        // Not yet created, instantiate now
        iClipboardBuffer = new( ELeave ) CEikEdwin();
        iClipboardBuffer->ConstructL( AknEdwinFlags() | CEikEdwin::EOwnsWindow,
                                      1,
                                      IMUtils::MaxMsgLength(),
                                      1 );
        iClipboardBuffer->SetContainerWindowL( *Parent() );
        //iClipboardBuffer->ActivateL();
        iClipboardBuffer->CreateTextViewL();
        iClipboardBuffer->MakeVisible( EFalse );
        }
    else
        {
        // Clear buffer
        TInt textLen = iClipboardBuffer->TextLength();
        iClipboardBuffer->Text()->DeleteL( 0, textLen );
        TCursorSelection delSel( 0, 0 );
        iClipboardBuffer->TextView()->HandleInsertDeleteL( delSel, textLen );
        }

    // Get converted text
    TCursorSelection copySel;
    HBufC* plainText = HBufC::NewLC( IMUtils::MaxMsgLength() );
    TPtr ptr( plainText->Des() );
    iMessageHandler.ConvertSelectionToStringL( *this, ptr, copySel, EFalse );

    // Insert only selected text to buffer
    HBufC* copiedText =
        plainText->Mid( copySel.LowerPos(), copySel.Length() ).AllocLC();
    iClipboardBuffer->SetTextL( copiedText );
    CleanupStack::PopAndDestroy( 2, plainText );
    // Select whole buffer
    iClipboardBuffer->SelectAllL();
    }


// -----------------------------------------------------------------------------
// CCAMessageEditor::ShowIndicator
// Construct if needed and set up clipboard buffer
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::ShowIndicator()
    {
    TInt editorFlags = ( AknEdwinFlags() & ~EAknEditorFlagNoEditIndicators );
    SetAknEditorFlags( editorFlags );
    }



// -----------------------------------------------------------------------------
// CCAMessageEditor::HideIndicator
// Construct if needed and set up clipboard buffer
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::HideIndicator()
    {
    if ( !TextLength() )
        {
        TInt editorFlags = ( AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
        SetAknEditorFlags( editorFlags );
        }
    }


// -----------------------------------------------------------------------------
// CCAMessageEditor::SetActiveObject
// Construct if needed and set up clipboard buffer
// -----------------------------------------------------------------------------
//

void CCAMessageEditor::SetActiveObject( CCAEditIndicator* aActiveObject )
    {
    iEditIndicatorActiveObj = 	aActiveObject;
    }


// -----------------------------------------------------------------------------
// CCAMessageEditor::ReSetActiveObject
// Construct if needed and set up clipboard buffer
// -----------------------------------------------------------------------------
//
void CCAMessageEditor::ReSetActiveObject()
    {
    iEditIndicatorActiveObj = NULL;
    }

//  End of File
