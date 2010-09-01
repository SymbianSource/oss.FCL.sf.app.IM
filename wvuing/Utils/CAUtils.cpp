/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utils for IM modules.
*
*/


#include "CAUtils.h"
#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"
#include "ChatDebugPrint.h"
#include "impsbuilddefinitions.h"

#include <EscapeUtils.h>
#include <collate.h>

// needed to skip prefixes in domain-neutral comparison (TUint version)
const TUint KColonUInt( ':' );

// needed to skip domains in domain-neutral comparison (TUint version)
const TUint KAtUInt( '@' );
// general colon needed in various places
_LIT( KColon, ":" );

// general slash needed in various places
_LIT( KSlash, "/" );

// needed to skip domains in domain-neutral comparison
_LIT( KAt, "@" );

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;

// max number to store any values
const TInt KMAX_NUMBER = 12;

// max number to fit
const TInt KMAX_LIMIT = 255;

// max number base
const TInt KMAX_NUMBERBASE = 10;

// some character value constants
//const TUint K7LSBMask       = 0x7f     // bitmask for 7 lsb bits
const TUint K8BitMask       = 0xff;     // full bitmask
//const TUint KCharCodeZero   = 0x30     // '0'
//const TUint KCharCodeSmallZ = 0x7a     // 'z'
//const TUint KCharCodeSpace  = 0x20     // ' '
const TUint KCharToArea     = 0x030;

// Unused areas (2) in ISO 8859-1 specification
//const TUint KUnused8859start1 = 0x00
const TUint KUnused8859end1 = 0x1F;
const TUint KUnused8859start2 = 0x7F;
const TUint KUnused8859end2 = 0x9F;

// forbidden characters that cannot appear in Wireless Village Ids
_LIT( KWVForbiddenCharacters, "/@+ \t:" );

/**
 * Constants to use in validation.
 *
 */
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

// length of "wv:" prefix
const TInt KCUImpsId_WVPrefixLength = 3;

// maximum length of user id
const TInt KServerWVUserIdMaxLength = 50;

// defines how far we are going to search for the protocol part in user id
const TInt KMaxLengthSearchProtPart = 4;

// maximum numerical value to append to generated WVID.
const TInt KMaxAppendNumber = 9999;

// -----------------------------------------------------------------------------
// CAUtils::NeutralCompare
// -----------------------------------------------------------------------------
//
TInt CAUtils::NeutralCompare( const TDesC& aId1,
                              const TDesC& aId2, TBool aDomainNeutral )
    {
    // points to user part of id
    TPtrC ptrId1( aId1 );
    TPtrC ptrId2( aId2 );

    // Reduce looking for protocol part only to beginning of the WVID and
    // skip protocol part ("anything:") in the beginning of the WVID
    TInt colonPos1 = aId1.Left( KMaxLengthSearchProtPart ).Locate( KColonUInt );

    // first id
    if ( ( KErrNotFound != colonPos1 ) && ( aId1.Length() - 1 != colonPos1 ) )
        {
        // contains ":", and it is not the last char
        ptrId1.Set( aId1.Mid( colonPos1 + 1 ) );
        }

    TInt colonPos2 = aId2.Left( KMaxLengthSearchProtPart ).Locate( KColonUInt );

    // second id
    if ( ( KErrNotFound != colonPos2 ) && ( aId2.Length() - 1 != colonPos2 ) )
        {
        // contains ":", and it is not the last char
        ptrId2.Set( aId2.Mid( colonPos2 + 1 ) );
        }

    // find out if we have domains in the ids
    TInt domainPos1( ptrId1.Locate( KAtUInt ) );
    TInt domainPos2( ptrId2.Locate( KAtUInt ) );

    TBool domainIn1( KErrNotFound != domainPos1 );
    TBool domainIn2( KErrNotFound != domainPos2 );

    // points to domains in the neutral id
    TPtrC ptrDom1( KNullDesC );
    TPtrC ptrDom2( KNullDesC );

    // points to user parts in the neutral id
    TPtrC ptrUid1( ptrId1 );
    TPtrC ptrUid2( ptrId2 );

    // separate user id parts and domain parts
    if ( domainIn1 )
        {
        ptrDom1.Set( ptrId1.Mid( domainPos1 + 1 ) );
        ptrUid1.Set( ptrId1.Mid( 0, domainPos1 ) );
        }

    if ( domainIn2 )
        {
        ptrDom2.Set( ptrId2.Mid( domainPos2 + 1 ) );
        ptrUid2.Set( ptrId2.Mid( 0, domainPos2 ) );
        }

    // Create custom collation method to ignore punctuations
    // index 0 gets the default method
    TCollationMethod collation =
        *Mem::CollationMethodByIndex( 0 );
    collation.iFlags |= TCollationMethod::EIgnoreNone;

    // domains are compared only when it is really needed
    // check if userid part is the same in both ids
    TInt idResult = ptrUid1.CompareC( ptrUid2, KCollationLevel, &collation );

    if ( idResult != 0 )
        {
        return idResult;
        }

    // id part is same, we have to compare domain

    // If domain comparison is neutral and one id is without domain
    // -> Domains are same. Other situation domainResult stays valid.
    if ( aDomainNeutral && ( domainIn1 ^ domainIn2 ) )
        {
        return 0;
        }
    else
        {
        return ptrDom1.CompareC( ptrDom2, KCollationLevel, &collation );
        }
    }

// -----------------------------------------------------------------------------
// CAUtils::DisplayId
// -----------------------------------------------------------------------------
//
TPtrC CAUtils::DisplayId( const TDesC& aId, TBool aListHiding )
    {
    TPtrC ret( aId );

    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );
    if ( err != KErrNone || !contacts )
        {
        // some error, don't modify
        CHAT_DP_FUNC_DP( "DisplayId", "Got some error, not hiding" );
        return ret;
        }

    // if we've branded the feature out, don't modify anything
    // UI CR : 101-39728:  Check for prefix hiding or Wv hiding.
    if ( contacts->WVHiding() || contacts->WVHidingPrefixOnly() )
        {
        // locate ":" for userid, groupid.
        // locate "/" for list id.
        TInt pos = aId.FindC( aListHiding ? KSlash : KColon );

        if ( ( pos != KErrNotFound ) && ( pos != aId.Length() - 1 ) )
            {
            // contains the special character, and it is not the last char
            // remove everything before the special char (including the char)
            ret.Set( aId.Mid( pos + 1 ) );
            }
        if ( contacts->WVHiding() || aListHiding )
            {
            // remove also the domain part
            TInt domainPos = ret.FindC( KAt );
            if ( ( domainPos != KErrNotFound ) && ( domainPos != 0 ) )
                {
                ret.Set( ret.Mid( 0, domainPos ) );
                }

            }
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CAUtils::GenerateIdLC
// -----------------------------------------------------------------------------
//
HBufC* CAUtils::GenerateIdLC( const TDesC& aId, TInt aNum )
    {
    // copy ID
    HBufC* result = HBufC::NewLC( KServerWVUserIdMaxLength );
    TPtr resultPtr( result->Des() );
    resultPtr.Copy( aId );

    TInt len( resultPtr.Length() );

    // Start from end of data
    for ( TInt i( len - 1 ); i >= 0; --i )
        {
        // Do quick and dirty UNICODE to ISO-8859-1 conversion
        // and modify all non-ISO-8859-1 characters
        // to be in valid ranges
        const TInt& currentChar = resultPtr[ i ];
        TUint ch = currentChar & K8BitMask;

        // is ch in first unused area
        if ( ch <= KUnused8859end1 )
            {
            ch += KUnused8859end1 + 1;
            }

        // is ch in second unused area
        if ( ch >= KUnused8859start2 && ch <= KUnused8859end2 )
            {
            TInt difference = ch - KUnused8859start2;
            ch = KUnused8859end2 + difference + 1;
            }

        TChar isoFixedChar = TChar( ch );
        // put it into the string
        HBufC* isoFixed = HBufC::NewLC( 1 );
        isoFixed->Des().Append( isoFixedChar );

        resultPtr.Replace( i, 1, *isoFixed );
        CleanupStack::PopAndDestroy( isoFixed );

        // now all the specials get removed
        TPtrC c = resultPtr.Mid( i, 1 );  // next character
        TInt position = KWVForbiddenCharacters().FindF( c );

        if ( position != KErrNotFound )
            {
            // the current character is a FORBIDDEN CHARACTER,
            // remove it
            resultPtr.Delete( i, 1 );
            }
        }

    CHAT_DP( D_CHAT_LIT( "CAUtils::GenerateIdLC conversion '%S' -> '%S'" ), &aId, &resultPtr );

    // "Append" number
    if ( aNum > 0 && aNum <= KMaxAppendNumber )
        {
        TBuf<KMAX_NUMBER> number; // 12, enough space to store a very large number
        number.AppendNum( aNum );

        TInt numLength = number.Length();
        TInt resultLength = resultPtr.Length();
        if ( numLength + resultLength > KServerWVUserIdMaxLength )
            {
            // can't append directly because we would exceed the space.
            // use replace instead
            TInt usableSpace = KServerWVUserIdMaxLength - resultLength;
            TInt startPos = resultLength - numLength + usableSpace;
            TInt replaceAmount = numLength - usableSpace;

            resultPtr.Replace( startPos, replaceAmount, number );
            }
        else
            {
            // enough space in the end, just append
            resultPtr.AppendNum( aNum );
            }
        }

    CHAT_DP( D_CHAT_LIT( "CAUtils::GenerateIdLC returning '%S'" ), &resultPtr );
    return result;
    }

// -----------------------------------------------------------------------------
//  CAUtils::ExternalizeBufferToStreamL
//  Helper method for externalize the buffer
// -----------------------------------------------------------------------------
//
void CAUtils::ExternalizeBufferToStreamL( const TDesC& aBuffer, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aBuffer.Length() );
    aStream.WriteL( aBuffer );
    }

// -----------------------------------------------------------------------------
//  CAUtils::ExternalizeBufferToStreamL
//  Helper method for externalize the buffer
// -----------------------------------------------------------------------------
//
void CAUtils::ExternalizeBufferToStreamL( const TDesC8& aBuffer, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aBuffer.Length() );
    aStream.WriteL( aBuffer );
    }

// -----------------------------------------------------------------------------
// CAUtils::InternalizeBufferFromStreamL
// Helper method for internalize the buffer
// -----------------------------------------------------------------------------
//
HBufC* CAUtils::InternalizeBufferFromStreamLC( RReadStream& aStream )
    {
    HBufC* buffer = NULL;

    TInt length = aStream.ReadInt32L();

    if ( length == 0 )
        {
        buffer = KNullDesC().AllocLC();
        }
    else
        {
        buffer = HBufC::NewLC( length );
        TPtr ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        }

    return buffer;
    }

// -----------------------------------------------------------------------------
// CAUtils::InternalizeBufferFromStreamL
// Helper method for internalize the buffer
// -----------------------------------------------------------------------------
//
HBufC8* CAUtils::InternalizeBuffer8FromStreamLC( RReadStream& aStream )
    {
    HBufC8* buffer = NULL;

    TInt length = aStream.ReadInt32L();

    if ( length == 0 )
        {
        buffer = KNullDesC8().AllocLC();
        }
    else
        {
        buffer = HBufC8::NewLC( length );
        TPtr8 ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        }

    return buffer;
    }

// -----------------------------------------------------------------------------
// CAUtils::InternalizeBufferFromStreamL
// Helper method for internalize the buffer
// -----------------------------------------------------------------------------
//
HBufC* CAUtils::InternalizeBufferFromStreamL( RReadStream& aStream )
    {
    HBufC* buffer = InternalizeBufferFromStreamLC( aStream );
    CleanupStack::Pop( buffer );
    return buffer;
    }

// -----------------------------------------------------------------------------
// CAUtils::InternalizeBufferFromStreamL
// Helper method for internalize the buffer
// -----------------------------------------------------------------------------
//
HBufC8* CAUtils::InternalizeBuffer8FromStreamL( RReadStream& aStream )
    {
    HBufC8* buffer = InternalizeBuffer8FromStreamLC( aStream );
    CleanupStack::Pop( buffer );
    return buffer;
    }

// -----------------------------------------------------------------------------
// CAUtils::ValidLoginIdL()
// -----------------------------------------------------------------------------
//
TBool CAUtils::ValidLoginIdL( const TDesC& aPresenceId )
    {
    TBool incorrectDomain ( EFalse );
    TBool forbiddenChars ( EFalse );
    TBool incorrectUserPart ( EFalse );

    HBufC* wvId = aPresenceId.AllocLC();
    TPtr wvIdPtr = wvId->Des();

    TInt maxLength ( KServerWVUserIdMaxLength );

    // let's take out the "wv:" from beginning of user id
    TInt position( KErrNotFound );
    if ( KCUImpsId_WVPrefix().CompareF( wvId->Left( KCUImpsId_WVPrefix().Length() ) ) == 0 )
        {
        wvIdPtr.Delete( 0, KCUImpsId_WVPrefix().Length() );
        position = KErrNotFound;
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
            incorrectDomain = ETrue;
            }
        }
    else if ( atIndex == 0 )
        {
        // the '@' is the first character
        incorrectUserPart = ETrue;
        }

    TInt returnValue = domainPart.Find( KCUImpsId_At() );
    // check the correctness of the domain part
    if ( returnValue != KErrNotFound )
        {
        // extra @-mark found in the domain part
        incorrectDomain = ETrue;
        }

    returnValue = domainPart.Find( KCUImpsId_Dot() );
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
            // the '.' is the last character
            incorrectDomain = ETrue;
            }
        }

    returnValue = domainPart.Find( KCUImpsId_TwoDots() );
    if ( returnValue != KErrNotFound )
        {
        // there are two sequential dots in the domain part
        incorrectDomain = ETrue;
        }


    // check the wvid for forbidden chars

    // first we must delete one '@' char if existing
    position = wvIdPtr.Find( KCUImpsId_At );
    if ( position != KErrNotFound )
        {
        wvIdPtr.Delete( position, KCUImpsId_At().Length() );
        }

    returnValue = wvIdPtr.Find( KCUImpsId_WhiteSpace() );
    // no empty chars allowed in the user part
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }
    // no encoded empty chars allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_WhiteSpaceEnc() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }

    // no '+' allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_Plus() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }
    // no encoded '+' allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_PlusEnc() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }


    // no '\t' allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_Tabulator() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }
    // no encoded tabulator allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_TabulatorEnc() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }

    // no '/' allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_Slash() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }
    // no encoded '/' allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_SlashEnc() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }

    // no encoded '@' chars allowed in the user part
    returnValue = wvIdPtr.Find( KCUImpsId_AtEnc() );
    if ( returnValue != KErrNotFound )
        {
        forbiddenChars = ETrue;
        }

    TPtr userId = wvId->Des();

    // no 16bit Unicode chars characters allowed in the user part
    for ( TInt index = 0; index < wvIdPtr.Length(); index++ )
        {
        TUint value = userId[index];
        // if the character does not fit into 8 bits, it's forbidden 255
        if ( value > KMAX_LIMIT )
            {
            forbiddenChars = ETrue;
            }
        }

    TBool tooLongId ( EFalse );

    // check if the user id is too long when encoded, but only if there are no forbidden chars
    if ( !forbiddenChars )
        {
        HBufC* encodedUserId = EscapeUtils::EscapeEncodeL( *wvId, EscapeUtils::EEscapeUrlEncoded );

        if ( atIndex != KErrNotFound )
            {
            // @-character was removed from the ID, since it should not be encoded
            // so the maxlength has to be decreased by it's length
            maxLength = maxLength - KCUImpsId_At().Length();
            }

        if ( encodedUserId->Length() > maxLength )
            {
            tooLongId = ETrue;
            }
        delete encodedUserId;
        }

    CleanupStack::PopAndDestroy( wvId );

    if ( forbiddenChars )
        {
        return EFalse;
        }
    else if ( incorrectDomain )
        {
        return EFalse;
        }
    else if ( tooLongId )
        {
        return EFalse;
        }
    else if ( incorrectUserPart )
        {
        return EFalse;
        }

    return ETrue;
    }


// ---------------------------------------------------------
// CAUtils::ReconstructIdL
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CAUtils::ReconstructIdL( const TDesC& aOrigId, const TDesC& aUserId )
    {
    CHAT_DP( D_CHAT_LIT( "** ReconstructIdL, the original  id: %S" ), &aOrigId );
    CHAT_DP( D_CHAT_LIT( "** ReconstructIdL, user entered  id: %S" ), &aUserId );

    // we got user id, add the removed parts ("wv:" and "@something" )
    TInt colonPos( aOrigId.Locate( KColonUInt ) );
    TInt domainPos( aOrigId.Locate( KAtUInt ) );
    TInt userColonPos( aUserId.Locate( KColonUInt ) );
    TInt userDomainPos( aUserId.Locate( KAtUInt ) );

    // calculate the new length and determine what additions are needed
    TInt newLength( aUserId.Length() );
    TBool addPrefix( EFalse );
    TBool addDomain( EFalse );

    if ( colonPos != KErrNotFound &&
         ( colonPos + 1 ) != aOrigId.Length() &&
         userColonPos == KErrNotFound )
        {
        // the original id had ":" (not last character) and the current one doesn't
        // => we'll be adding "wv:"
        newLength += colonPos + 1;
        addPrefix = ETrue;
        }

    if ( domainPos != KErrNotFound && userDomainPos == KErrNotFound )
        {
        // the original id had domain and the current one doesn't
        // => we'll be adding "@something"
        newLength += aOrigId.Length() - domainPos;
        addDomain = ETrue;
        }

    HBufC* newId = HBufC::NewL( newLength );
    // NOTE: NO LEAVING METHODS ALLOWED AFTER THIS!

    TPtr newIdPtr( newId->Des() );
    newIdPtr.Zero();

    if ( addPrefix )
        {
        // add removed "wv:" prefix
        newIdPtr.Append( aOrigId.Left( colonPos + 1 ) );
        }
    newIdPtr.Append( aUserId );
    if ( addDomain )
        {
        // add removed domain part
        newIdPtr.Append( aOrigId.Right( aOrigId.Length() - domainPos ) );
        }

    CHAT_DP( D_CHAT_LIT( "** ReconstructIdL, reconstructed id: %S" ), &newIdPtr );
    return newId;
    }

// ---------------------------------------------------------
// CAUtils::NeutralFind
// ---------------------------------------------------------
//
TInt CAUtils::NeutralFind( const MDesCArray& aUserList, const TDesC& aUserId )
    {
    TInt count( aUserList.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( CAUtils::NeutralCompare( aUserList.MdcaPoint( i ),
                                      aUserId ) == 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// Static helper method used in
// LanguageSpecificNumberConversion.
// ---------------------------------------------------------
//
static TChar NumberToBase( TChar ch )
    {
    TDigitType d[] = { EDigitTypeWestern,
                       EDigitTypeArabicIndic,
                       EDigitTypeEasternArabicIndic,
                       EDigitTypeDevanagari,
                       EDigitTypeThai
                     };
    TInt i = 0;
    TInt num = sizeof( d ) / sizeof( d[0] );
    // code scanner warning can be ignored
    while ( i < num )
        {
        if ( ch > TChar( d[i] ) && ch < TChar( d[i] + KMAX_NUMBERBASE ) )
            {
            return d[i];
            }
        i++;
        }

    return ch;
    }

// ---------------------------------------------------------
// CAUtils::LanguageSpecificNumberConversion
// ---------------------------------------------------------
//
void CAUtils::LanguageSpecificNumberConversion( TDes& aDes )
    {
    // Get current locale and digit type
    TLocale locale;
    locale.Refresh();
    TDigitType digitType = locale.DigitType();

    // Check if conversion is needed
    TBool conversion = ( digitType == EDigitTypeArabicIndic )
                       || ( digitType == EDigitTypeEasternArabicIndic )
                       || ( digitType == EDigitTypeDevanagari );

    if ( !conversion )
        {
        // Don't do anything
        return;
        }

    // Do conversion, logic for this conversion
    // comes from AknTextUtils::LanguageSpecificNumberConversion.
    // Logic is copied here to avoid using Avkon in engine side.
    TChar toArea = KCharToArea;

    switch ( digitType )
        {
        case EDigitTypeWestern:
        case EDigitTypeArabicIndic:
        case EDigitTypeEasternArabicIndic:
        case EDigitTypeDevanagari:
        case EDigitTypeThai:    // Flowthrough
            {
            toArea = digitType;
            break;
            }
        case EDigitTypeUnknown:
        case EDigitTypeAllTypes:
        default:    // Flowthrough
            {
            return;
            }
        }

    TInt length = aDes.Length();

    for ( TInt i = 0; i < length; i++ )
        {
        TChar ch = aDes[i];
        TChar fromArea = NumberToBase( ch );
        TChar::TBdCategory cat = ch.GetBdCategory();
        switch ( cat )
            {
            case TChar::EArabicNumber:
            case TChar::EEuropeanNumber:    // Flowthrough
                {
                ch += toArea;
                ch -= fromArea;
                aDes[i] = TUint16( ch );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }
// ---------------------------------------------------------
// CAUtils::CapitalizeListNameL : UI CR :  101-39727
// ---------------------------------------------------------
//
HBufC* CAUtils::CapitalizeListNameL( const TDesC& aListname )
    {
    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );

    if ( err == KErrNone && contacts
         && contacts->CapitalizingEnabled()
         && ( aListname.Length() ) )
        {
        HBufC* capitalPartBuf = HBufC::NewL( KServerWVUserIdMaxLength );
        TPtr capitalPartptr( capitalPartBuf->Des() );

        if ( aListname.Length() == 1 )
            {
            capitalPartptr.CopyUC( aListname );
            return capitalPartBuf;
            }
        capitalPartptr.CopyUC( aListname.Left( 1 ) );// copy and capitalize first letter
        capitalPartptr.Append( aListname.Right( ( aListname.Length() - 1 ) ) );// append the remaining
        return capitalPartBuf;
        }

    // some error, don't modify
    CHAT_DP_FUNC_DP( "CapitalizeListNameL", "Got some error, not Capitalizing" );
    HBufC* capitalPartBuf = aListname.AllocL();
    return capitalPartBuf;
    }


// ---------------------------------------------------------
// CAUtils::CapitalizingEnabled   UI CR : 101-39727
// ---------------------------------------------------------
//
TBool CAUtils::CapitalizingEnabled()
    {
    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );
    if ( err != KErrNone || !contacts )
        {
        // some error, return Efalse
        return EFalse;
        }
    return ( contacts->CapitalizingEnabled() );
    }


// ---------------------------------------------------------
// CAUtils::CapitalizeListNameL   UI CR : 101-39727
// ---------------------------------------------------------
//

HBufC* CAUtils::CapitalizeListIdL( const TDesC& aId )
    {
    TPtrC ret( aId );
    TBool slashfound( EFalse );
    TBool domainfound( EFalse );
    // locate "/" for list id.
    TInt pos = aId.FindC( KSlash  );

    if ( ( pos != KErrNotFound ) && ( pos != aId.Length() - 1 ) )
        {
        // make it true if slash found and it should not be at the last position
        slashfound = ETrue;
        }

    // locate for the domain part

    TInt domainPos = ret.FindC( KAt );
    if ( ( domainPos != KErrNotFound ) && ( domainPos != 0 ) )
        {
        // make it true if domain part is found and itshould not be at the first position
        domainfound = ETrue;
        }

    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );

    if ( err == KErrNone && contacts                      // extrachecking here bcoz
         && contacts->CapitalizingEnabled()// it has to return NULL
         && aId.Length() )                 // (not the list id)
        {                                                   // if error occurs
        if ( slashfound && !domainfound )
            {
            //remove upto  '/' and capitalize
            return  CAUtils::CapitalizeListNameL( aId.Right( ( aId.Length() - ( pos + 1 ) ) ) );
            }
        else  if ( slashfound && domainfound )
            {
            // remove upto  '/' and after domain part and capitalize
            return CAUtils::CapitalizeListNameL( aId.Mid( pos + 1, ( domainPos - pos - 1 ) ) );
            }
        else
            {
            // if only domain part found or both part not found just capitalize listId
            return CAUtils::CapitalizeListNameL( aId );
            }

        }

    // some error,return null
    return NULL;
    }

// End of file
