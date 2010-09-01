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
* Description:  XML Serializer implementation.
*
*/

// INCLUDE FILES
#include "CPEngXmlSerializer.h"
#include "PEngXmlDefs.h"
#include "PresenceDebugPrint.h"


#ifdef __SERIALIZER_TAG_NAME_ASSERT
#include "CPEngTagAssertionStack.h"
#endif

#include <E32Std.h>
#include <utf.h>
#include <imcvcodc.h>



//CONSTANTS
/**
 * State stack granularity.
 * State stack has great size
 * variety depending from use case.
 * 5 should be quite optimal.
 */
const TInt KStackGranurality = 5;

/**
 * WV schema prefix & length.
 */
_LIT( KWVSchemaPrefix, "wv:" );
const TInt KWVSchemaPrefixLength = 3;

// ================= LOCAL FUNCTIONS =======================
/**
 * Serializer panic implementation.
 */
#if defined(_DEBUG)
GLREF_C void PanicSerializer( TPEngSerializerPanics aReason )
    {
    User::Panic( KXmlSerializer, aReason );
    }
#else
GLREF_C void PanicSerializer( TPEngSerializerPanics /*aReason*/ )
    {
    }
#endif




// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CPEngXmlSerializer* CPEngXmlSerializer::NewL( TDes8& aBuffer )
    {
    CPEngXmlSerializer* self = CPEngXmlSerializer::NewLC( aBuffer );
    CleanupStack::Pop( self );
    return self;
    }



EXPORT_C CPEngXmlSerializer* CPEngXmlSerializer::NewLC( TDes8& aBuffer )
    {
    CPEngXmlSerializer* self = new ( ELeave ) CPEngXmlSerializer( aBuffer );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Destructor
CPEngXmlSerializer::~CPEngXmlSerializer()
    {
    iStateStack.Reset();

#ifdef __SERIALIZER_TAG_NAME_ASSERT
    delete iAssertionStack;
#endif //   __SERIALIZER_TAG_NAME_ASSERT


#if _BullseyeCoverage
    cov_write();
#endif
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngXmlSerializer::CPEngXmlSerializer( TDes8& aBuffer )
        : iState( EPEngInRoot ),
        iWriter( aBuffer ),
        iStartTagCount( 0 ),
        iStateStack( KStackGranurality )
    {
    }


// Symbian OS default constructor can leave.
void CPEngXmlSerializer::ConstructL()
    {
#ifdef __SERIALIZER_TAG_NAME_ASSERT
    iAssertionStack = CPEngTagAssertionStack::NewL( KStackGranurality );
#endif //   __SERIALIZER_TAG_NAME_ASSERT
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::Close()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::StartTagL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::StartTagL( const TDesC8& aName )
    {
    //tag name may not be empty
    __AssertNotEmptyL( aName );


#ifdef __SERIALIZER_TAG_NAME_ASSERT
    //make sure that if we can write the start tag,
    //we get it surely to element stack also
    iAssertionStack->ReserveOneL();
#endif //   __SERIALIZER_TAG_NAME_ASSERT


    CheckAndCloseOpenStartTagL(); //goes to EElementOpen state
    iWriter.WriteL( KXmlTagStart );


#ifdef __SERIALIZER_TAG_NAME_ASSERT
    TPtrC8 writtenTag( iWriter.WriteL( aName ) );
    iAssertionStack->PushL( writtenTag ); //won't fail, see previous ReserveOneL()
#else
    iWriter.WriteL( aName );
#endif //   __SERIALIZER_TAG_NAME_ASSERT

    iStartTagCount++;
    iState = EPEngInStartTag;

    return *this;
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::AttributeL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::AttributeL( const TDesC8& aName,
                                                    const TDesC8& aValue )
    {
    //attribute can be written only just after the start tag.
    __AssertSerializerStateL( ( iState == EPEngInStartTag ),
                              EPEngSrlzr_AttributeNotAllowed );

    //attribute name may not be empty
    __AssertNotEmptyL( aName );


    iWriter.WriteL( KXmlWhiteSpace );
    iWriter.WriteL( aName );
    iWriter.WriteL( KXmlEqualSign );

    TBuf8< 1 > quote; //quote is one character, either ' or "
    quote = KXmlApostrophe;
    if ( aValue.Find( KXmlDoubleQuote ) == KErrNotFound )
        {
        quote = KXmlDoubleQuote;
        }

    iWriter.WriteL( quote );
    WriteXmlEscapedL( aValue, ETrue );
    iWriter.WriteL( quote );


    return *this;
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::EndTagL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::EndTagL( const TDesC8& aName )
    {
    //Check there is start tag to close
    __AssertSerializerStateL( ( ( iState == EPEngInStartTag ) || ( iState == EPEngInElement ) ),
                              EPEngSrlzr_EndTagNotAllowed );

    //tag name may not be empty
    __AssertNotEmptyL( aName );


#ifdef __SERIALIZER_TAG_NAME_ASSERT
    //Check end tag name match corresponding start tag name
    __AssertEndTagName( aName );
#endif //   __SERIALIZER_TAG_NAME_ASSERT



    if ( iState == EPEngInStartTag )
        {
        //still in start tag ==> empty element ==> close the tag directly
        iWriter.WriteL( KXmlEmptyTagEnd );
        }


    else
        {
        //non empty element ==> write complete end tag
        iWriter.WriteL( KXmlEndTagStart );
        iWriter.WriteL( aName );
        iWriter.WriteL( KXmlTagEnd );
        }

#ifdef __SERIALIZER_TAG_NAME_ASSERT
    iAssertionStack->Pop();
#endif //   __SERIALIZER_TAG_NAME_ASSERT


    iStartTagCount--;
    if ( iStartTagCount == 0 )
        {
        //closed the last open element ==> now in root
        iState = EPEngInRoot;
        }
    else
        {
        //there is still not closed start tags ==> open elements
        iState = EPEngInElement;
        }


    return *this;
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::RawValueL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::RawValueL( const TDesC8& aValue )
    {
    //Write raw data

    CheckAndCloseOpenStartTagL();   //goes to EElementOpen state

    __AssertNoXmlEscapedCharsL( aValue );

    iWriter.WriteL( aValue );

    return *this;
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::NarrowTextL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::NarrowTextL( const TDesC8& aText )
    {
    // Performed steps:
    // 1. Escapes XML entities.


    CheckAndCloseOpenStartTagL();   //goes to EElementOpen state

    WriteXmlEscapedL( aText, EFalse );
    return *this;
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::UnicodeTextL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::UnicodeTextL( const TDesC16& aText )
    {
    // Performed steps:
    // 1. Converts text from Unicode to Utf8
    // 2. Escapes XML entities

    TBuf8<20> outputBuffer;
    TPtrC16 remainderOfUnicodeText( aText );
    TInt returnValue;

    CheckAndCloseOpenStartTagL();   //goes to EElementOpen state


    FOREVER // conversion loop
        {
        returnValue = CnvUtfConverter::ConvertFromUnicodeToUtf8( outputBuffer,
        remainderOfUnicodeText );

        // check to see that the descriptor isn’t corrupt
        if ( returnValue == CnvUtfConverter::EErrorIllFormedInput )
            {
            User::Leave( KErrCorrupt );
            }

        else if ( returnValue < KErrNone ) // future-proof against "TError" expanding
            // See SDK help for more information.
            {
            User::Leave( KErrGeneral );
            }

        //write the Utf8 fragment produced by this round
        WriteXmlEscapedL( outputBuffer, EFalse );

        if ( returnValue == 0 )
            {
            //no more text left -> break the loop
            break;
            }

        remainderOfUnicodeText.Set( remainderOfUnicodeText.Right( returnValue ) );
        }

    return *this;
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::WvAddressL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::WvAddressL( const TDesC16& aAddress )
    {
    // Performed steps:
    // 1. Escapes WV Address characters
    // 2. Converts text from Unicode to Utf8
    // 3. Escape XML entities


    TBuf16<20> outputBuffer;
    TPtrC16 remainderOfAddress( aAddress );
    TInt returnValue;

    CheckAndCloseOpenStartTagL();   //goes to EElementOpen state


    // Skip the possible "wv:" prefix from address
    // And write it as it is
    if ( aAddress.Left( KWVSchemaPrefixLength ).CompareF( KWVSchemaPrefix ) == 0 )
        {
        remainderOfAddress.Set( aAddress.Mid( KWVSchemaPrefixLength ) );
        UnicodeTextL( aAddress.Left( KWVSchemaPrefixLength ) );
        }

    FOREVER // conversion loop
        {
        returnValue = EncodeWvAddressChars( outputBuffer,
        remainderOfAddress );

        // Handle possible errors
        if ( returnValue < KErrNone )
            {
            User::Leave( KErrGeneral );
            }


        //write the WVAddress part produced by this round
        UnicodeTextL( outputBuffer );


        if ( returnValue == 0 )
            {
            //no more text left -> break the loop
            break;
            }

        remainderOfAddress.Set( remainderOfAddress.Right( returnValue ) );
        }

    return *this;
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::Base64DataL()
// -----------------------------------------------------------------------------
//
MPEngXMLSerializer& CPEngXmlSerializer::Base64DataL( const TDesC8& aRawData )
    {
    // Performed steps:
    // 1. Coverts data to BASE64 format


    CheckAndCloseOpenStartTagL();   //goes to EElementOpen state

    if ( aRawData.Length() != 0 )
        {
        TImCodecB64 base64Encoder;
        base64Encoder.Initialise();

        //Encoder doesn't itself cope with buffer overflows,
        //so allocate twice big buffer to be sure that there is enough
        //space for B64 data
        HBufC8* base64Buffer = HBufC8::NewLC( aRawData.Size() * 2 );
        TPtr8 base64Data( base64Buffer->Des() );
        base64Encoder.Encode( aRawData, base64Data );


        //for performance reasons, write
        //whole base64 data directly to the
        //xml buffer (no need to do any XML escaping)
        iWriter.WriteL( base64Data );
        CleanupStack::PopAndDestroy(); //base64Buffer
        }

    return *this;
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::PushSerializerStateL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::PushSerializerStateL()
    {
    TPEngSerializerStateData state;
    state.iWriterLength = iWriter.CurrentLength();
    state.iState =  iState;


#ifdef __SERIALIZER_TAG_NAME_ASSERT
    state.iAssertionStackCount = iAssertionStack->Count();
#endif //   __SERIALIZER_TAG_NAME_ASSERT


    User::LeaveIfError( iStateStack.Append( state ) ); //push new state to state stack
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::CommitState()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::CommitState()
    {
    //check that there is a states to commit
    __ASSERT_DEBUG( ( iStateStack.Count() > 0 ),
                    PanicSerializer( EPEngSrlzr_StateStackUnderflow ) );

    TInt stateCount( iStateStack.Count() );
    if ( stateCount > 0 )
        {
        iStateStack.Remove( stateCount - 1 ); //pop the last state away from state stack (thus -1)
        }
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::RollbackState()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::RollbackState()
    {
    //Check that there is a states to rollback
    __ASSERT_DEBUG( ( iStateStack.Count() > 0 ),
                    PanicSerializer( EPEngSrlzr_StateStackUnderflow ) );

    TInt stateCount( iStateStack.Count() );
    if ( stateCount > 0 )
        {
        TPEngSerializerStateData oldState;
        oldState = iStateStack[ stateCount - 1 ];
        iStateStack.Remove( stateCount - 1 ); //pop the last state away from state stack (thus -1)

        //rollback writer
        iWriter.ReverseTo( oldState.iWriterLength );


#ifdef __SERIALIZER_TAG_NAME_ASSERT
        //rollback assertion stack
        iAssertionStack->PopTo( oldState.iAssertionStackCount );
#endif //   __SERIALIZER_TAG_NAME_ASSERT


        //return to old state
        iState = oldState.iState;
        }
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::PushedStateCount()
// -----------------------------------------------------------------------------
//
TInt CPEngXmlSerializer::PushedStateCount()
    {
    return iStateStack.Count();
    }



// -----------------------------------------------------------------------------
// CPEngXmlSerializer::CheckAndCloseOpenStartTagL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::CheckAndCloseOpenStartTagL()
    {
    if ( iState == EPEngInStartTag )
        {
        iWriter.WriteL( KXmlTagEnd );
        iState = EPEngInElement;
        }
    }




// -----------------------------------------------------------------------------
// CPEngXmlSerializer::EncodeWvAddressChars()
// Return the number of unconverted characters left at the end of the input
// descriptor, or the error value
// -----------------------------------------------------------------------------
//
TInt CPEngXmlSerializer::EncodeWvAddressChars( TDes16& aEncodedAddress,
                                               const TDesC16& aUnicodeAddress )
    {
    if ( aUnicodeAddress.Length() == 0 )
        {
        aEncodedAddress.SetLength( 0 );
        return 0;
        }


    if ( aEncodedAddress.MaxLength() == 0 )
        {
        return aUnicodeAddress.Length();
        }

    aEncodedAddress.Zero();

    TInt addLen( aUnicodeAddress.Length() );
    for ( TInt ii( 0 ); ii < addLen; ii++ )
        {
        TUint16 characterValue = aUnicodeAddress[ ii ];

        switch ( characterValue )
            {
            case ':':
            case ';':
            case '?':
            case '&':
            case '=':
            case '+':
            case '$':
            case ',':
                {
                TInt spaceLeft = aEncodedAddress.MaxLength() - aEncodedAddress.Length();

                //Encoding special characters produces 3 characters e.g. "%yy"
                //(only three produced because encoded characters list is
                //limited above)
                if ( spaceLeft < 3 )
                    {
                    return addLen - ii;
                    }

                aEncodedAddress.Append( KPercentSign16 );
                aEncodedAddress.AppendNumUC( characterValue, EHex );
                break;
                }

            default:
                {
                TInt spaceLeft = aEncodedAddress.MaxLength() - aEncodedAddress.Length();

                //Adding one non-encoded character requires space one character
                if ( spaceLeft < 1 )
                    {
                    return addLen - ii;
                    }

                aEncodedAddress.Append( characterValue );
                break;
                }
            }
        }


    return KErrNone;
    }





// -----------------------------------------------------------------------------
// CPEngXmlSerializer::WriteXmlEscapedL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::WriteXmlEscapedL( const TDesC8& aString,
                                           TBool aEscapeQuotes )
    {
    const TInt length( aString.Length() );
    TUint8 character;

    //loop trough the all characters and escape them to writer one by one
    for ( TInt ii( 0 ); ii < length; ii++ )
        {
        character = aString[ ii ];
        WriteXmlEscapedL( character, aEscapeQuotes );
        }
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::WriteXmlEscapedL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::WriteXmlEscapedL( TUint8 aCharacter,
                                           TBool aEscapeQuotes )
    {
    switch ( aCharacter )
        {
        case '&':
            {
            iWriter.WriteL( KEntityAmpersand );
            break;
            }

        case '>':
            {
            iWriter.WriteL( KEntityGreaterThan );
            break;
            }

        case '<':
            {
            iWriter.WriteL( KEntityLowerThan );
            break;
            }

        case '"':       ///<If character is a double
        case '\'':      ///<or single quote, escape it only if required
            {
            if ( aEscapeQuotes )
                {
                if ( aCharacter == '"' )
                    {
                    iWriter.WriteL( KEntityDoubleQuote );
                    }
                else
                    {
                    iWriter.WriteL( KEntityApostrophe );
                    }

                break;
                }

            //else no need to escape quotes ==>
            //write them as normal characters
            //FLOW TROUGH
            }

        default:
            {
            iWriter.WriteL( aCharacter );
            }
        }
    }





// -----------------------------------------------------------------------------
// CPEngXmlSerializer::__AssertNoXmlEscapedCharsL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::__AssertNoXmlEscapedCharsL( const TDesC8& aString )
    {
    const TInt length( aString.Length() );
    TUint8 character;

    //loop trough the all characters and check them
    for ( TInt ii( 0 ); ii < length; ii++ )
        {
        character = aString[ ii ];
        switch ( character )
            {
            case '&':
            case '>':
            case '<':
            case '"':
            case '\'':
                {
#if defined(_DEBUG)
                PanicSerializer( EPEngSrlzr_XmlMarkupCharNotAllowed );
#else
                User::Leave( KErrArgument );
#endif

                break;
                }

            default:
                {
                break;
                }
            }
        }
    }





// -----------------------------------------------------------------------------
// CPEngXmlSerializer::__AssertSerializerStateL()
// -----------------------------------------------------------------------------
//
#if defined(_DEBUG)
void CPEngXmlSerializer::__AssertSerializerStateL( TBool aInCorrectState,
                                                   TPEngSerializerPanics aPanicReason )
#else
void CPEngXmlSerializer::__AssertSerializerStateL( TBool aInCorrectState,
                                                   TPEngSerializerPanics /*aPanicReason*/ )
#endif
    {
    if ( !aInCorrectState )
        {
#ifdef __SERIALIZER_TAG_NAME_ASSERT
        TBuf<150> buffer16; //restrict tag names shown in debug output to 150 characters.
        //150 characters should be enough for debug purposes.

        TPtrC8 startTagName = iAssertionStack->Top();
        buffer16.Copy( startTagName.Left( buffer16.MaxLength() ) );

        PENG_DP( D_PENG_LIT( "CPEngXmlSerializer: Wrong state. In tag <%S> at nesting level %i" ),
                 &buffer16, iAssertionStack->Count() );
#endif // __SERIALIZER_TAG_NAME_ASSERT


#if defined(_DEBUG)
        PanicSerializer( aPanicReason );
#else
        User::Leave( KErrNotSupported );
#endif
        }
    }


// -----------------------------------------------------------------------------
// CPEngXmlSerializer::__AssertNotEmptyL()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::__AssertNotEmptyL( const TDesC8& aString )
    {
    if ( aString.Length() == 0 )
        {
#ifdef __SERIALIZER_TAG_NAME_ASSERT
        TBuf<150> buffer16; //restrict tag names shown in debug output to 150 characters.
        //150 characters should be enough for debug purposes.

        TPtrC8 startTagName = iAssertionStack->Top();
        buffer16.Copy( startTagName.Left( buffer16.MaxLength() ) );

        PENG_DP( D_PENG_LIT( "CPEngXmlSerializer: Writing empty tag or attribute. In tag <%S> at nesting level %i" ),
                 &buffer16, iAssertionStack->Count() );
#endif // __SERIALIZER_TAG_NAME_ASSERT


#if defined(_DEBUG)
        PanicSerializer( EPEngSrlzr_EmptyInputString );
#else
        User::Leave( KErrArgument );
#endif
        }
    }


#ifdef __SERIALIZER_TAG_NAME_ASSERT
// -----------------------------------------------------------------------------
// CPEngXmlSerializer::__AssertEndTagName()
// -----------------------------------------------------------------------------
//
void CPEngXmlSerializer::__AssertEndTagName( const TDesC8& aEndTagName )
    {
    if ( aEndTagName != iAssertionStack->Top() )
        {
        TBuf<150> buffer16; //restrict tag names shown in debug output to 150 characters.
        //150 characters should be enough for debug purposes.

        buffer16.Copy( aEndTagName.Left( buffer16.MaxLength() ) );
        PENG_DP( D_PENG_LIT( "CPEngXmlSerializer: End tag mismatch. Ending with <%S> at level %i" ),
                 &buffer16, iAssertionStack->Count() );

        buffer16.Zero();
        TPtrC8 startTagName = iAssertionStack->Top();
        buffer16.Copy( startTagName.Left( buffer16.MaxLength() ) );
        PENG_DP( D_PENG_LIT( "CPEngXmlSerializer: Required start tag <%S>" ),
                 &buffer16 );

        PanicSerializer( EPEngSrlzr_EndTagNameMismatch );
        }
    }
#endif // __SERIALIZER_TAG_NAME_ASSERT


//  End of File




