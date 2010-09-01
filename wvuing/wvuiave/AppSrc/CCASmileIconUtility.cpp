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
#include "CCASmileIconUtility.h"
#include "CCASmileIconDialog.h"
#include "ccasmilestring.h"
#include "ccasmileyinformation.h"
#include "IMUtils.h"

#include "CCAAppUi.h"
#include "MCASkinVariant.h"
#include "CCAApp.h"
#include "CCAVariantFactory.h"
#include "ChatDebugPrint.h"
#include "chatdebugassert.h"

#include <chatNG.rsg>
#include <bautils.h>
#include <PUAcodes.hrh>
#include <barsread.h> // RResourceReader
#include <coemain.h>
#include <eikenv.h>
#include <gulicon.h>
#include <akniconutils.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASmileIconUtility* CCASmileIconUtility::NewL(
    MCASkinVariant& aSkinVariant,
    MCAAppUi& aAppUi )
    {
    CCASmileIconUtility* self = new ( ELeave ) CCASmileIconUtility( aSkinVariant, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL( R_SMILE_ARRAY_RESOURCE );
    CleanupStack::Pop( self );
    return self;
    }

// Two-phased constructor.
CCASmileIconUtility* CCASmileIconUtility::NewL(
    MCASkinVariant& aSkinVariant,
    MCAAppUi& aAppUi,
    TInt aResourceId )
    {
    CCASmileIconUtility* self = new ( ELeave ) CCASmileIconUtility( aSkinVariant, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCASmileIconUtility::~CCASmileIconUtility()
    {
    iAppUi.RemoveResourceChangeObserver( this );
    iIconArray.ResetAndDestroy();
    iStringArray.ResetAndDestroy();
    iSmileArray.ResetAndDestroy();
    iIconToSmileyPointers.Close();

    iSmileDlgIconArray.ResetAndDestroy();
    }

// C++ constructor can NOT contain any code, that
// might leave.
//
CCASmileIconUtility::CCASmileIconUtility( MCASkinVariant& aSkinVariant,
                                          MCAAppUi& aAppUi )
        : iSkinVariant( aSkinVariant ), iAppUi( aAppUi )
    {
    }

// ConstructL
//
void CCASmileIconUtility::ConstructL( TInt aResourceId )
    {
    // we need to be resource aware
    iAppUi.AddResourceChangeObserver( this );

    ConstructFromResourceL( aResourceId );
    }

// ---------------------------------------------------------
// CCASmileIconUtility::ConstructSmileDialogIconArrayL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconUtility::ConstructSmileDialogIconArrayL( TInt aResourceId )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    TInt iconId;
    TInt maskId;
    TInt err( KErrNone );

    TInt iconCount( reader.ReadInt16() );
    for ( TInt i( 0 ); i < iconCount; ++i )
        {
        iconId = reader.ReadInt16();
        maskId = reader.ReadInt16();

        CGulIcon* icon = iSkinVariant.LoadBitmapL( iconId, maskId,
                                                   iAppUi.MbmFullPath() );

        err = iSmileDlgIconArray.Append( icon );
        if ( err )
            {
            delete icon;
            User::Leave( err );
            }

        TInt stringCount( reader.ReadInt16() );
        if ( !stringCount )
            {
            User::Leave( KErrGeneral );
            }

        HBufC* shortertString = NULL;
        for ( TInt j( 0 ); j < stringCount; ++j )
            {
            HBufC* smileString = reader.ReadHBufCL();
            delete smileString;
            }
        }
    CleanupStack::PopAndDestroy( ); // reader
    }

// ---------------------------------------------------------
// CCASmileIconUtility::LaunchSmileIconDialogL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileIconUtility::LaunchSmileIconDialogL( TInt& aSelectedIconId )
    {
    if ( iSmileDlgIconArray.Count() <= 0 )
        {
        ConstructSmileDialogIconArrayL( R_SMILE_ARRAY_RESOURCE );
        }

    CCASmileIconDialog* dialog = CCASmileIconDialog::NewL( iSmileDlgIconArray,
                                                           aSelectedIconId );
    TInt dialogVal = dialog->ExecuteLD( R_SELECT_SMILE_DIALOG );

    return dialogVal;
    }

// ---------------------------------------------------------
// CCASmileIconUtility::SmileIconCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileIconUtility::SmileIconCount() const
    {
    return iIconArray.Count();
    }

// ---------------------------------------------------------
// CCASmileIconUtility::GetSmileIcon
// (other items were commented in a header).
// ---------------------------------------------------------
//
const CGulIcon* CCASmileIconUtility::GetSmileIcon( TInt aIconId ) const
    {
    return iIconArray[ aIconId ];
    }

// ---------------------------------------------------------
// CCASmileIconUtility::GetSmileString
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CCASmileIconUtility::GetSmileString( TInt aIconId ) const
    {
    return iStringArray[ iIconToSmileyPointers[ aIconId ] ]->HasShortestString() ?
           iStringArray[ iIconToSmileyPointers[ aIconId ] ]->ShortestSmileString() :
           iStringArray[ iIconToSmileyPointers[ aIconId ] ]->SmileString();
    }

// ---------------------------------------------------------
// CCASmileIconUtility::SearchSmilesL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconUtility::SearchSmilesL( const TDesC& aStr, TDes& aFixedStr,
                                         RArray<TInt>* aArray )
    {

    CHAT_DP( D_CHAT_LIT( "CCASmileIconUtility::SearchSmilesL: begin %S" ),
             &aStr );
    TInt i;
    iSmileArray.ResetAndDestroy();
    SearchSmilesL( aStr, iSmileArray, &aFixedStr );

    aArray->Reset();
    TInt arrayCount( iSmileArray.Count() );

    for ( i = 0; i < arrayCount; ++i )
        {
        User::LeaveIfError( aArray->Append( iSmileArray[ i ]->Index() ) );
        }

    iSmileArray.ResetAndDestroy();
    CHAT_DP( D_CHAT_LIT( "CCASmileIconUtility::SearchSmilesL: done %S" ),
             &aFixedStr );
    }

// -----------------------------------------------------------------------------
// CCASmileIconUtility::SearchSmilesL
// Search smileys from string and sort them in linear order.
// -----------------------------------------------------------------------------
//
void CCASmileIconUtility::SearchSmilesL( const TDesC& aStr,
                                         RPointerArray< CCASmileyInformation >& aSmileyInfoArray,
                                         TDes* aFixedStr /*= NULL */,
                                         TInt aStartPos /* = 0 */ ) const
    {
    CHAT_DP( D_CHAT_LIT( "CCASmileIconUtility::SearchSmilesL2: %S %d" ),
             &aStr, aStartPos );

    HBufC* text = HBufC::NewMaxLC( aStr.Length() );
    TPtr fixedText( text->Des() );
    fixedText.Copy( aStr.Left( fixedText.MaxLength() ) );

    HBufC* replaceString = HBufC::NewMaxLC( iLongestSmileyLength );
    TPtr replace( replaceString->Des() );
    if ( aFixedStr )
        {
        replace.Fill( KPuaCodeSmileyIconPadding, 1 );
        }
    else
        {
        //we dont need fixed, but we do need correct positions
        replace.Fill( KPuaCodeSmileyIconPadding, iLongestSmileyLength );
        }

    // order to sort array by the position
    TLinearOrder< CCASmileyInformation > order( CCASmileIconUtility::Compare );

    // for i-loop goes throug every smile string
    // while-loop searches smiles from given string
    TInt arrayCount = iStringArray.Count();
    TInt index = 0;
    for ( TInt i = 0; i < arrayCount; ++i )
        {
        const TDesC& smileString( iStringArray[ i ]->SmileString() );
        index = fixedText.Find( smileString );
        while ( index != KErrNotFound )
            {
            CCASmileyInformation* smileInfo = CCASmileyInformation::NewL();
            CleanupStack::PushL( smileInfo );

            smileInfo->SetPosition( index + aStartPos );
            smileInfo->SetIndex( iStringArray[ i ]->SmileIconIndex() );
            smileInfo->SetSmileyStringL( smileString );
            smileInfo->SetIcon( EFalse );

            User::LeaveIfError( aSmileyInfoArray.Append( smileInfo ) );
            CleanupStack::Pop( smileInfo );

            TInt smilyLength = smileString.Length();
            // Only replace the same amount of chars than in smiley, so we
            // don't mess up the length or index table.
            fixedText.Replace( index,
                               smilyLength,
                               replace.Left( smilyLength ) );
            index = fixedText.Find( smileString );
            } // while
        } // for i

    aSmileyInfoArray.Sort( order );
    if ( aFixedStr )
        {
        *aFixedStr = fixedText;
        }

    CleanupStack::PopAndDestroy( 2, text ); // replaceString, text
    CHAT_DP_TXT( "CCASmileIconUtility::SearchSmilesL2: done" );
    }

// ---------------------------------------------------------
// CCASmileIconUtility::Compare
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASmileIconUtility::Compare( const CCASmileyInformation& aFirst,
                                   const CCASmileyInformation& aSecond )
    {
    return ( aFirst.Position() - aSecond.Position() );
    }

// -----------------------------------------------------------------------------
// CCASmileIconUtility::CompareSmileyLengthReversed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASmileIconUtility::CompareSmileyLengthReversed(
    const CCASmileString& aFirst,
    const CCASmileString& aSecond )
    {
    return ( aSecond.SmileString().Length() - aFirst.SmileString().Length() );
    }

// ---------------------------------------------------------
// CCASmileIconUtility::ResizeIcons
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconUtility::ResizeIcons( const TSize& aSize )
    {
    TInt count( SmileIconCount() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        AknIconUtils::SetSize( iIconArray[ i ]->Bitmap(), aSize );
        }
    }

// ---------------------------------------------------------
// CCASmileIconUtility::ConstructFromResourceL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASmileIconUtility::ConstructFromResourceL( TInt aResourceId )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    iLongestSmileyLength = 1;
    TInt iconId;
    TInt maskId;
    TInt err( KErrNone );
    TInt iconCount( reader.ReadInt16() );
    for ( TInt i( 0 ); i < iconCount; ++i )
        {
        iconId = reader.ReadInt16();
        maskId = reader.ReadInt16();

        CGulIcon* icon = iSkinVariant.LoadBitmapL( iconId, maskId,
                                                   iAppUi.MbmFullPath() );

        err = iIconArray.Append( icon );
        if ( err )
            {
            delete icon;
            User::Leave( err );
            }

        TInt stringCount( reader.ReadInt16() );
        if ( !stringCount )
            {
            User::Leave( KErrGeneral );
            }

        HBufC* shortertString = NULL;

        for ( TInt j( 0 ); j < stringCount; ++j )
            {
            CCASmileString* smileArray = CCASmileString::NewL();
            CleanupStack::PushL( smileArray );

            HBufC* smileString = reader.ReadHBufCL();
            // ownership is transferred
            smileArray->Set( smileString, i );

            // Update the smiley length
            if ( smileString->Length() > iLongestSmileyLength )
                {
                iLongestSmileyLength = smileString->Length();
                }

            // Check for shorter smiley strings,
            // needed when converting smileys from images to
            // strings before sending to avoid descriptor overflow
            if ( shortertString )
                {
                if ( shortertString->Length() < shortertString->Length() )
                    {
                    // Set shotestString to current smiley
                    smileArray->SetShortestSmileStringL( *shortertString );
                    }
                else
                    {
                    // Found a new shortest string
                    // Set it to previous similar smileys
                    for ( TInt ii = 0; ii < j; ii++ )
                        {
                        iStringArray[iStringArray.Count() - ii - 1]->
                        SetShortestSmileStringL( *smileString );
                        }
                    // Store new shortest string
                    shortertString = smileString;
                    }
                }
            else
                {
                // First string, store to shortestString
                shortertString = smileString;
                }

            User::LeaveIfError( iStringArray.Append( smileArray ) );
            CleanupStack::Pop( smileArray );
            }
        }

    CleanupStack::PopAndDestroy(); // reader

    // Sort smileys according to their string length, so that searching
    // for smileys is done from the longest smiley to the shortest. This
    // prevents the chat from detecting smaller smileys inside the big ones.
    TLinearOrder< CCASmileString >
    order( CCASmileIconUtility::CompareSmileyLengthReversed );
    iStringArray.Sort( order );

    // Precalculate an ref table from icon to smiley.
    iIconToSmileyPointers.Reset();
    TInt count = iIconArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        User::LeaveIfError(
            iIconToSmileyPointers.Append( GetFirstSmileyIndex( i ) ) );
        }
    }

// -----------------------------------------------------------------------------
// CCASmileIconUtility::GetFirstSmileyIndex
// -----------------------------------------------------------------------------
//
TInt CCASmileIconUtility::GetFirstSmileyIndex( TInt aIconIndex ) const
    {
    TInt count = iStringArray.Count();

    for ( TInt i = 0; i < count; ++i )
        {
        if ( iStringArray[ i ]->SmileIconIndex() == aIconIndex )
            {
            return i;
            }
        }

    // Icon was not found.
    __CHAT_ASSERT_DEBUG( EFalse );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCASmileIconUtility::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CCASmileIconUtility::ResourceChangedL()
    {
    iIconArray.ResetAndDestroy();
    iStringArray.ResetAndDestroy();
    ConstructFromResourceL( R_SMILE_ARRAY_RESOURCE );
    }

// End of File

