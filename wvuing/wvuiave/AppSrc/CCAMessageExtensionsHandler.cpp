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
* Description:  Message extension handler
*
*/


#include "CCAMessageExtensionsHandler.h"
#include "CCASmileyUtil.h"
#include "ChatDefinitions.h"
#include "IMUtils.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include <eikrted.h>
#include <txtrich.h>

_LIT( KRIGHT_TO_LEFT_MAKR, "\x200F" );
const TInt KLENGTHOFACHAR = 1;

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::CCAMessageExtensionsHandler
// Constructor
// -----------------------------------------------------------------------------
//
CCAMessageExtensionsHandler::CCAMessageExtensionsHandler()
    {
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::~CCAMessageExtensionsHandler
// Destructor
// -----------------------------------------------------------------------------
//
CCAMessageExtensionsHandler::~CCAMessageExtensionsHandler()
    {
    iExtensionArray.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ConstructL
// Symbian OS Constructor
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::ConstructL( MCASkinVariant& aSkinVariant,
                                              MCAAppUi& aAppUi, TBool aAddSmileyHandler )
    {
    if ( aAddSmileyHandler )
        {
        //Register smileyUtils with identification type.
        CCASmileyUtil* smiley = CCASmileyUtil::NewL( aSkinVariant, aAppUi, EMessageExtensionSmiley );
        CleanupStack::PushL( smiley );
        User::LeaveIfError( iExtensionArray.Append( smiley ) );
        CleanupStack::Pop( smiley );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::NewL
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
CCAMessageExtensionsHandler* CCAMessageExtensionsHandler::NewL(
    MCASkinVariant& aSkinVariant,
    MCAAppUi& aAppUi,
    TBool aAddSmileyHandler /*= ETrue */ )
    {
    CCAMessageExtensionsHandler* self =
        new ( ELeave ) CCAMessageExtensionsHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aSkinVariant, aAppUi, aAddSmileyHandler );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ProcessMessageLC
// Process messages trough extensions.
// -----------------------------------------------------------------------------
//
HBufC* CCAMessageExtensionsHandler::ProcessMessageLC(
    CEikRichTextEditor& aEditor )
    {
    HBufC* msg = ExtensionToStringLC( aEditor );
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->Reset();
        }
    return msg;
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionHandler::ExtensionToStringL
// Convert extensions to strings.
// -----------------------------------------------------------------------------
//
HBufC* CCAMessageExtensionsHandler::ExtensionToStringLC(
    CEikRichTextEditor& aEditor )
    {
    HBufC* text = HBufC::NewMaxLC( IMUtils::MaxMsgLength() );
    TPtr tBuf( text->Des() );

    HBufC* msgBuf = aEditor.GetTextInHBufL();

    // Because the comments in TSW, we delete the mark first and add it anywhere to
    // let the akn components know.

    // Get the pointer of the buffer.
    TPtr bufPtr = msgBuf->Des();

    // An indicator for whether the buffer is processed.
    TBool isProcessed( EFalse );

    // Delete the mark any way
    for ( TInt i = 0; i < bufPtr.Length(); i++ )
        {
        TInt pos = bufPtr.Find( KRIGHT_TO_LEFT_MAKR );

        // If can not find the right-to-left mark pos = -1
        if ( pos >= 0 )
            {
            bufPtr.Delete( pos, KLENGTHOFACHAR );
            isProcessed = ETrue;
            }
        }

    // Re-add the mark back
    if ( isProcessed )
        {
        bufPtr.Insert( 0, KRIGHT_TO_LEFT_MAKR );
        bufPtr.Insert( bufPtr.Length(), KRIGHT_TO_LEFT_MAKR );
        }

    CleanupStack::PushL( msgBuf );
    TPtr msgPtr( msgBuf->Des() );

    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        tBuf = msgPtr;
        iExtensionArray[ a ]->ExtensionToStringL( aEditor, tBuf );
        CleanupStack::PopAndDestroy( msgBuf );
        msgBuf = NULL;
        msgBuf = tBuf.AllocLC();
        TPtr newPtr( msgBuf->Des() );
        msgPtr.Set( newPtr );
        }

    tBuf = msgPtr;

    TInt length = tBuf.Length();
    for ( TInt i = length - 1; i >= 0 ; --i )
        {
        if ( tBuf[ i ] ==  CEditableText::EZeroWidthNoBreakSpace )
            {
            tBuf.Delete( i, 1 ); // removing 1 character
            }
        }
    CleanupStack::PopAndDestroy( msgBuf );
    return text;
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::InsertExtensionIcon
// Find right extension handler and insert extension
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::InsertExtensionL( CEikRichTextEditor& aEditor,
                                                    TMessageExtensionTypes aType,
                                                    TBool& aCancelled )
    {
    TBool flag( EFalse );
    for ( TInt a( 0 ); a < iExtensionArray.Count() && !flag; ++a )
        {
        if ( aType == iExtensionArray[ a ]->Type() )
            {
            iExtensionArray[ a ]->InsertExtensionL( aEditor, aCancelled );
            flag = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::DeleteExtensionIcon
// Find right extension handler and delete extension
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::DeleteExtensionL( CEikRichTextEditor& aEditor,
                                                    TMessageExtensionTypes aType,
                                                    TInt aPos )
    {
    TBool flag( EFalse );
    for ( TInt a( 0 ); a < iExtensionArray.Count() && !flag; ++a )
        {
        if ( aType == iExtensionArray[ a ]->Type() )
            {
            iExtensionArray[ a ]->DeleteExtensionL( aEditor, aPos );
            flag = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ConvertSelectionToExtensionL
// Convert all extensions from strings to extensions in area.
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::ConvertSelectionToExtensionL(
    CEikRichTextEditor& aEditor )
    {
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->ConvertSelectionToExtensionL( aEditor );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ConvertSelectionToExtensionL
// Convert all extensions from strings to extensions in area.
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::ConvertSelectionToExtensionL(
    CRichText& aRichText, TCursorSelection& aSelection )
    {
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->ConvertSelectionToExtensionL( aRichText, aSelection );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ConvertSelectionToStringL
// Convert all extensions from extensions to strings in area.
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::ConvertSelectionToStringL(
    CEikRichTextEditor& aEditor, TBool aPreserve /*= EFalse*/ )
    {
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->ConvertSelectionToStringL( aEditor, aPreserve );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::SizeChanged
// Convert all extensions from extensions to strings in area.
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::SizeChanged( TSize& aSize )
    {
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->SizeChanged( aSize );
        }
    }

// -----------------------------------------------------------------------------
// CCAMessageExtensionsHandler::ConvertSelectionToStringL
// Convert all extensions from extensions to strings in area.
// -----------------------------------------------------------------------------
//
void CCAMessageExtensionsHandler::ConvertSelectionToStringL(
    CEikRichTextEditor& aEditor,
    TDes& aResultString,
    TCursorSelection& aSelectionAfterConversion,
    TBool aPreserve )
    {
    for ( TInt a( 0 ); a < iExtensionArray.Count(); ++a )
        {
        iExtensionArray[ a ]->ConvertSelectionToStringL(
            aEditor, aResultString, aSelectionAfterConversion, aPreserve );
        }
    }

// end of file
