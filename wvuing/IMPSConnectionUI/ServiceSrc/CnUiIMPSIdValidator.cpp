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
* Description:  Validator to check the IMPS id correctness.
*
*/


// INCLUDE FILES
#include    <EscapeUtils.h>
#include    "CnUiIMPSIdValidator.h"


//CONSTS
/**
 * Constants to use in validation.
 *
 * For future: These could be merged with IMPS Common UI
 */
const TInt KServerWVUserIdMaxLength = 50;
_LIT( KCUImpsId_WhiteSpace, " " );
_LIT( KCUImpsId_At, "@" );
_LIT( KCUImpsId_Dot, "." );
_LIT( KCUImpsId_TwoDots, ".." );
_LIT( KCUImpsId_Slash, "/" );
_LIT( KCUImpsId_Plus, "+" );
_LIT( KCUImpsId_Tabulator, "\t" );
_LIT( KCUImpsId_WVPrefix, "wv:" );

_LIT( KCUImpsId_AtEnc, "%40" );
_LIT( KCUImpsId_SlashEnc, "%2F" );
_LIT( KCUImpsId_PlusEnc, "%2B" );
_LIT( KCUImpsId_TabulatorEnc, "%09" );
_LIT( KCUImpsId_WhiteSpaceEnc, "%20" );

const TInt KCUImpsId_WVPrefixLength = 3;



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CnUiIMPSIdValidator::ValidLoginIdL()
// -----------------------------------------------------------------------------
//
TBool CnUiIMPSIdValidator::ValidLoginIdL( const TDesC& aPresenceId )
    {
    TBool idCorrect( ETrue );

    HBufC* wvId = aPresenceId.AllocLC();
    TPtr wvIdPtr = wvId->Des();

    TInt maxLength ( KServerWVUserIdMaxLength );

    // let's take out the "wv:" from beginning of user id
    TInt position( KErrNotFound );
    if ( KCUImpsId_WVPrefix().CompareF( wvId->Left( KCUImpsId_WVPrefixLength ) ) == 0 )
        {
        wvIdPtr.Delete( 0, KCUImpsId_WVPrefix().Length() );
        // we just took out 3 characters from the id, we have to adjust the max length
        maxLength = maxLength - KCUImpsId_WVPrefixLength;
        }

    // where is "@" ?
    TInt atIndex( wvId->Find( KCUImpsId_At ) );
    // we want the domain part without the '@'
    TPtrC domainPart( wvId->Mid( atIndex + 1 ) );

    if ( atIndex > 0 )
        {
        // check if the domain part is empty
        if ( domainPart.Length() == 0 )
            {
            idCorrect = EFalse;
            }
        }
    else if ( atIndex == 0 )
        {
        // the '@' is the first character
        idCorrect = EFalse;
        }


    // check the correctness of the domain part
    if ( ( domainPart.Find( KCUImpsId_At() ) ) != KErrNotFound )
        {
        // extra @-mark found in the domain part
        idCorrect = EFalse;
        }

    TInt returnValue ( domainPart.Find( KCUImpsId_Dot() ) );
    if ( returnValue != KErrNotFound )
        {
        // the part after the '.'
        TPtrC partAfterDot ( domainPart.Mid( returnValue + 1 ) );
        // the part before the '.'
        TPtrC partBeforeDot ( domainPart.Left( returnValue ) );
        // if the '.' is the last character or the first character after the '@'
        // the domain part is wrong
        if ( ( 0 == partAfterDot.Length() ) || ( 0 == partBeforeDot.Length() ) )
            {
            idCorrect = EFalse;
            }
        }

    if ( ( domainPart.Find( KCUImpsId_TwoDots() ) ) != KErrNotFound )
        {
        // there are two sequential dots in the domain part
        idCorrect = EFalse;
        }

    // first we must delete one '@' char if existing
    position = wvIdPtr.Find( KCUImpsId_At );
    if ( position != KErrNotFound )
        {
        wvIdPtr.Delete( position, KCUImpsId_At().Length() );
        // @-character was removed from the ID, since it should not be encoded
        // so the maxlength has to be decreased by it's length
        maxLength = maxLength - KCUImpsId_At().Length();
        }

    // check the id for forbidden characters

    if ( CnUiIMPSIdValidator::ForbiddenChars( wvIdPtr ) )
        {
        idCorrect = EFalse;
        }

    TInt length( wvIdPtr.Length() );
    // no 16bit Unicode chars characters allowed in the user part
    for ( TInt index = 0; index < length; index++ )
        {
        TUint value = wvIdPtr[index];
        // if the character does not fit into 8 bits, it's forbidden
        if ( value > 255 )
            {
            idCorrect = EFalse;
            }
        }

    // check if the user id is too long when encoded, but only if there are no forbidden chars
    if ( idCorrect )
        {
        HBufC* encodedUserId = EscapeUtils::EscapeEncodeL( *wvId, EscapeUtils::EEscapeUrlEncoded );

        if ( encodedUserId->Length() > maxLength )
            {
            idCorrect = EFalse;
            }
        delete encodedUserId;
        }

    CleanupStack::PopAndDestroy( wvId );

    return idCorrect;
    }


// -----------------------------------------------------------------------------
// CnUiIMPSIdValidator::ValidLoginIdL()
// -----------------------------------------------------------------------------
//
TBool CnUiIMPSIdValidator::ForbiddenChars( const TDesC& aImpsId )
    {
    TBool returnValue( EFalse );

    // no empty chars allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_WhiteSpace() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }
    // no encoded empty chars allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_WhiteSpaceEnc() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }

    // no '+' allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_Plus() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }
    // no encoded '+' allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_PlusEnc() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }

    // no '\t' allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_Tabulator() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }
    // no encoded tabulator allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_TabulatorEnc() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }

    // no '/' allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_Slash() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }
    // no encoded '/' allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_SlashEnc() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }

    // no encoded '@' chars allowed in the user part
    if ( ( aImpsId.Find( KCUImpsId_AtEnc() ) ) != KErrNotFound )
        {
        returnValue = ETrue;
        }

    return returnValue;
    }

//  End of File
