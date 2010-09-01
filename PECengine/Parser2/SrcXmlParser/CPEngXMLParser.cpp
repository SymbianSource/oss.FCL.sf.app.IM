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
* Description:  XML Parser implementation
*
*/

// INCLUDE FILES

#include <e32base.h>
#include <utf.h>
#include <imcvcodc.h>
#include <PEngWVPresenceErrors2.h>

#include "CPEngXMLParser.h"
#include "MPEngAdvTransactionStatus2.h"
#include "CPEngParserStack.h"




// =================== MACROS =======================
#define STORE_AND_RETURN_IF_IN_ERROR( aParser, aErr ) \
    if ( aErr < KErrNone ) \
        { \
        aParser->SetError( aErr ); \
        return NW_STAT_FAILURE; \
        } \
     


// ================= LOCAL FUNCTIONS =======================
/**
 * Maps given CSP status code to WV error code range
 * defined in PEngWVPresenceErrors.h
 *
 * @since 3.0
 * @param aCSPStatusCode The CSP status code to map.
 * @return WV Error code.
 */
TInt MapWVCspStatusToError( TInt aCSPStatusCode )
    {
    //Map the CSP successful (200) ==> KErrNone
    if ( aCSPStatusCode == 200 )
        {
        return KErrNone;
        }

    return KPEngErrorWVServerResponseBase - aCSPStatusCode;
    }


/**
 * Checks is the given CSP status code in the CSP status code range.
  *
 * @since 3.0
 * @param aCSPStatusCode The CSP status code to check.
 * @return ETrue if the given int is inside of the CSP status code range.
 *         Else EFalse.
 */
TInt IsInCspStatusCodeRange( TInt aCSPStatusCode )
    {
    return ( aCSPStatusCode >= KPEngWVStatusCodeBase ) &&
           ( aCSPStatusCode <= KPEngWVStatusCodeTop );
    }


/**
 * Decodes given CSP status code descriptor block
 * to WV error code.
 *
 * @since 3.0
 * @param aCSPStatusCode The CSP status code to map.
 * @param aWVErrorCode The mapped error code
 * @return ETrue if the decode was successful.
 *         Else EFalse.
 */
TBool DecodeWVCspStatusCodeToError( const TDesC8& aCSPStatusCode,
                                    TInt& aWVErrorCode )
    {
    //Try decode the CSP status code to integer & map it to WV Error Code
    TInt cspStatusInt;
    TLex8 lexer( aCSPStatusCode );
    lexer.SkipSpace();
    TInt lexErr = lexer.Val( cspStatusInt );
    if ( lexErr == KErrNone )
        {
        if ( IsInCspStatusCodeRange( cspStatusInt ) )
            {
            aWVErrorCode = MapWVCspStatusToError( cspStatusInt );
            return ETrue;
            }
        }

    return EFalse;
    }







// ================= MEMBER FUNCTIONS =======================


// Two-phased constructor.
CPEngXMLParser* CPEngXMLParser::NewLC()
    {
    CPEngXMLParser* self = new ( ELeave ) CPEngXMLParser;

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Destructor
CPEngXMLParser::~CPEngXMLParser()
    {
    iResults.Close();
    delete iIntervals;


#if _BullseyeCoverage
    cov_write();
#endif
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngXMLParser::CPEngXMLParser()
    {
    }

// Symbian OS default constructor can leave.
void CPEngXMLParser::ConstructL()
    {
    iIntervals = CPEngParserStack::NewL();

    iCallBacks.StartDocument_CB = StartDocument_CB; //StartDocument_CB;
    iCallBacks.EndDocument_CB = EndDocument_CB; //EndDocument_CB;
    iCallBacks.Tag_Start_CB = Tag_Start_CB;
    iCallBacks.Attr_Start_CB = NULL; //Attr_Start_CB;
    iCallBacks.Attr_VarVal_CB = NULL; //Attr_VarVal_CB;
    iCallBacks.Attributes_End_CB = NULL; //Attributes_End_CB;
    iCallBacks.Tag_End_CB = Tag_End_CB;
    iCallBacks.Content_CB = NULL; //Content_CB;
    iCallBacks.Cdata_CB = NULL; //Cdata_CB;
    iCallBacks.Comment_CB = NULL; //Comment_CB;
    iCallBacks.PiForm_CB = NULL; //PiForm_CB;
    iCallBacks.Exception_CB = Exception_CB;  // Mostly for debugging purposes
    iCallBacks.Extension_CB = NULL; //Extension_CB;
    iCallBacks.Attr_Entity_VarVal_CB = NULL; //Attr_Entity_VarVal_CB;


    iCallBacks.pClientPointer = ( void* ) this;
    }




// ---------------------------------------------------------
// CPEngAttributeLibraryModel::DecodeL()
//
// ---------------------------------------------------------
//
TBool CPEngXMLParser::DecodeL( const TDesC8& aBuffer,
                               const TDesC8& aString,
                               TBool aTags,
                               TInt aLevel )
    {
    iMaxParseLevel = aLevel;
    iData.Set( aBuffer );
    iSearchable.Set( aString );
    iIncludeTags = aTags;
    ResetResults();


    NW_XML_Reader_InitFromBuffer( &iReader, aBuffer.Size(), const_cast<TUint8*>( iData.Ptr() ) );
    NW_XML_Reader_SetEncoding( &iReader, HTTP_iso_8859_1 );


    //Parse & Handle Errors
    NW_Status_t status = NW_XML_Parse( &iReader, &iCallBacks );
    if ( iError != KErrNone )
        {
        User::Leave( iError );
        }

    if ( status == NW_STAT_OUT_OF_MEMORY )
        {
        User::Leave( KErrNoMemory );
        }

    if ( status != NW_STAT_SUCCESS )
        {
        User::Leave( KPEngNwErrInternalServerOrNetworkError );
        }

    //And return did we find something
    if ( iResults.Count() > 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }



// ---------------------------------------------------------
// CPEngXMLParser::Count()
// Returns the result count
// ---------------------------------------------------------
//
TInt CPEngXMLParser::Count()
    {
    return iResults.Count();
    }



// ---------------------------------------------------------
// CPEngXMLParser::ResultL()
// ---------------------------------------------------------
//
TPtrC8 CPEngXMLParser::ResultL( TInt aIndex )
    {
    //Get plain result
    if ( aIndex >= iResults.Count() || aIndex < 0 )
        {
        User::Leave( KErrArgument );
        }

    return iResults[aIndex];
    }


// ---------------------------------------------------------
// CPEngXMLParser::ResultAsNarrowTextL()
// ---------------------------------------------------------
//
HBufC8* CPEngXMLParser::ResultAsNarrowTextL( TInt aIndex )
    {
    //Performed steps:
    //1. UnEscapes XML entities.

    return DoXMLUnEscapingL( ResultL( aIndex ) );
    }



// ---------------------------------------------------------
// CPEngXMLParser::ResultAsUnicodeTextL()
// ---------------------------------------------------------
//
HBufC16* CPEngXMLParser::ResultAsUnicodeTextL( TInt aIndex )
    {
    // Performed steps:
    // 1. UnEscapes XML entities.
    // 2. Converts text from Utf8 to Unicode.

    HBufC8* xmlUnEscaped = DoXMLUnEscapingL( ResultL( aIndex ) );

    CleanupStack::PushL( xmlUnEscaped );
    HBufC16* unicode = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *xmlUnEscaped );
    CleanupStack::PopAndDestroy( xmlUnEscaped );

    return unicode;
    }


// ---------------------------------------------------------
// CPEngXMLParser::ResultAsWVAddressL()
// ---------------------------------------------------------
//
HBufC16* CPEngXMLParser::ResultAsWVAddressL( TInt aIndex )
    {
    // Performed steps:
    // 1. UnEscapes XML entities.
    // 2. Converts text from Utf8 to Unicode.
    // 3. UnEscapes WV Address characters

    HBufC8* xmlUnEscaped = DoXMLUnEscapingL( ResultL( aIndex ) );

    CleanupStack::PushL( xmlUnEscaped );
    HBufC16* unicode = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *xmlUnEscaped );
    CleanupStack::PopAndDestroy( xmlUnEscaped );


    CleanupStack::PushL( unicode );
    HBufC16* wvAddress = DoWVAddressUnescapeL( *unicode );
    CleanupStack::PopAndDestroy( unicode );


    return wvAddress;
    }



// ---------------------------------------------------------
// CPEngXMLParser::ResultAsBase64DecodedL()
// ---------------------------------------------------------
//
HBufC8* CPEngXMLParser::ResultAsBase64DecodedL(  TInt aIndex )
    {
    // Performed steps:
    // 1. Coverts data from the BASE64 format


    HBufC8* decodedBuffer8 = HBufC8::NewLC( ResultL( aIndex ).Length() );
    TPtr8 decodedPtr8( decodedBuffer8->Des() );

    TImCodecB64 base64Decoder;
    base64Decoder.Initialise();

    // Decode returns ETrue, more data is needed to complete the decoding
    // ==> in this case the received buffer is considered as corrupted
    TBool moreDataNeeded = base64Decoder.Decode( ResultL( aIndex ), decodedPtr8 );
    if ( moreDataNeeded )
        {
        User::Leave( KErrCorrupt );
        }

    decodedBuffer8 = decodedBuffer8->ReAllocL( decodedBuffer8->Length() );
    CleanupStack::Pop(); //decodedBuffer8
    return decodedBuffer8;
    }


// ---------------------------------------------------------
// CPEngXMLParser::ParseResultL()
// Parses the generic WV result from the XML packet
// ---------------------------------------------------------
//
TInt CPEngXMLParser::ParseResultL( const TDesC8& aBuffer,
                                   TInt aOperationID,
                                   MPEngAdvTransactionStatus2& aStatus,
                                   TBool& aResultRequired )
    {
    return DoParseResultL( aBuffer,
                           aStatus,
                           aOperationID,
                           NULL,
                           NULL,
                           aResultRequired );
    }


// ---------------------------------------------------------
// CPEngXMLParser::ParseResultL()
// Parses the generic WV result from the XML packet
// ---------------------------------------------------------
//
TInt CPEngXMLParser::ParseResultL( const TDesC8& aBuffer,
                                   TInt aOperationID,
                                   MPEngAdvTransactionStatus2& aStatus )
    {
    //By default - result is required
    TBool resultRequired = ETrue;
    return DoParseResultL( aBuffer,
                           aStatus,
                           aOperationID,
                           NULL,
                           NULL,
                           resultRequired );
    }



// ---------------------------------------------------------
// CPEngXMLParser::ParseContactListResultL()
// Parses the contact list related WV result from the
// XML packet.
// ---------------------------------------------------------
//
TInt CPEngXMLParser::ParseContactListResultL( const TDesC8& aBuffer,
                                              TInt aOperationID,
                                              const TDesC& aContactList,
                                              MPEngAdvTransactionStatus2& aStatus )
    {
    //By default - result is required
    TBool resultRequired = ETrue;
    return DoParseResultL( aBuffer,
                           aStatus,
                           aOperationID,
                           NULL,
                           &aContactList,
                           resultRequired );
    }



// ---------------------------------------------------------
// CPEngXMLParser::Close()
// ---------------------------------------------------------
//
void CPEngXMLParser::Close()
    {
    delete this;
    }



// ---------------------------------------------------------
// CPEngXMLParser::ResetResults()
// ---------------------------------------------------------
//
void CPEngXMLParser::ResetResults()
    {
    iResults.Reset();
    iIntervals->Reset();
    iParsingLevel = 0;
    iError = KErrNone;
    }




// ---------------------------------------------------------
// CPEngXMLParser::DoWVAddressUnescapeL()
// ---------------------------------------------------------
//
HBufC16* CPEngXMLParser::DoWVAddressUnescapeL( const TDesC16& aSrc )
    {
    HBufC16* destBuffer = HBufC16::NewLC( aSrc.Length() );
    TPtr16 dest( destBuffer->Des() );


    // the length of the escape sequence, without the percent sign
    TBuf8<2> value;

    TInt srcLen( aSrc.Length() );
    for ( TInt i( 0 ); i < srcLen; i++ )
        {
        if ( aSrc[i] == '%' )
            {
            // check if there is enough characters to resolve the next real,
            // unescaped character
            if ( ( i + 2 ) >= srcLen )
                {
                // not enough chars to resolve the last escaped sequence
                // bail out
                break;
                }
            // let's take the next two characters, the encoded char is
            // of format %20
            value.Zero();
            value.Append( aSrc[++i] );
            value.Append( aSrc[++i] );

            // find the hexadecimal value from the string
            // and convert it to a character
            TLex8 lexer( value );
            TUint8 charVal;
            TInt retVal( lexer.Val( charVal, EHex ) );
            if ( retVal == KErrGeneral )
                {
                // could not convert escape chars to character value
                // just append the percent sign and characters
                dest.Append( '%' );
                dest.Append( value[0] );
                dest.Append( value[1] );
                }
            else if ( retVal != KErrNone )
                {
                User::Leave( retVal );
                }
            else
                {
                // append the found character to the destination buffer
                TChar escapedChar( charVal );
                dest.Append( escapedChar );
                }
            }
        else
            {
            // the character wasn't any escaped sequence starter -> just copy it
            dest.Append( aSrc[i] );
            }
        }


    destBuffer = destBuffer->ReAllocL( destBuffer->Length() );
    CleanupStack::Pop(); //old destBuffer
    return destBuffer;
    }



// ---------------------------------------------------------
// CPEngXMLParser::DoXMLUnEscaping()
// Decodes the XML escaped character sequences
// ---------------------------------------------------------
//
HBufC8* CPEngXMLParser::DoXMLUnEscapingL( const TDesC8& aSrc )
    {
    HBufC8* destBuffer = HBufC8::NewLC( aSrc.Length() );
    TPtr8 dest( destBuffer->Des() );

    TInt srcLen( aSrc.Length() );
    for ( TInt i( 0 ); i < srcLen; i++ )
        {
        if ( aSrc[i] == '&' )
            {
            // check if there is enough characters to resolve the next real,
            // unescaped character
            if ( ( i + 2 ) >= srcLen )
                {
                // not enough chars to resolve the last escaped sequence
                // bail out
                break;
                }
            if ( aSrc[i+1] == 'a' )
                {
                // only two possibilities if the escape string starts
                // with &a
                if ( aSrc[i+2] == 'm' )
                    {
                    dest.Append( '&' );
                    i += 4; // jump over rest of the &amp; string
                    }
                else
                    {
                    dest.Append( '\'' );
                    i += 5; // jump over rest of the &apos; string
                    }
                }
            else if ( aSrc[i+1] == 'g' )
                {
                dest.Append( '>' );
                i += 3; // jump over rest of the &gt; string
                }
            else if ( aSrc[i+1] == 'l' )
                {
                dest.Append( '<' );
                i += 3; // jump over rest of the &lt; string
                }
            else if ( aSrc[i+1] == 'q' )
                {
                dest.Append( '\"' );
                i += 5; // jump over rest of the &quot; string
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            }
        else
            {
            dest.Append( aSrc[i] );
            }
        }

    destBuffer = destBuffer->ReAllocL( destBuffer->Length() );
    CleanupStack::Pop(); //old destBuffer
    return destBuffer;
    }




// ---------------------------------------------------------
// CPEngXMLParser::DoParseResultL()
// Parses the WV result from the XML packet.
// ---------------------------------------------------------
//
TInt CPEngXMLParser::DoParseResultL( const TDesC8& aBuffer,
                                     MPEngAdvTransactionStatus2& aStatus,
                                     TInt aOperationID,
                                     const TUint32* aAttributeTypeID,
                                     const TDesC* aContactListID,
                                     TBool& aResultRequired )
    {
    //DTD: Result (Code, Description?, DetailedResult*)

    TPtrC8 resultBlock( NULL, 0 );
    TInt wvErrorCode = KErrNone;
    TBool errorCodeDecoded = EFalse;


    //Look for mandatory parts
    if ( DecodeL( aBuffer, KResultXMLTag, ETrue ) )
        {
        resultBlock.Set( ResultL() );
        if ( DecodeL( resultBlock, KCodeXMLTag, EFalse ) )
            {
            TPtrC8 cspStatusCodeBlock( ResultL() );
            errorCodeDecoded = DecodeWVCspStatusCodeToError( cspStatusCodeBlock,
                                                             wvErrorCode );
            }
        }


    //And act wether the mandatory part was found or not..
    if ( !errorCodeDecoded )
        {
        if ( aResultRequired )
            {
            aStatus.SetStatus( KPEngNwErrInternalServerOrNetworkError );
            }
        else
            {
            aStatus.SetStatus( KErrNone );
            }

        //Return to client that mandatory part wasn't found
        aResultRequired = EFalse;
        }


    else
        {
        //Mandatory part fine. Return it also to client.
        aStatus.SetStatus( wvErrorCode );
        aResultRequired = ETrue;


        //Process possible detailed entries
        TInt detailEntriesStartCount = aStatus.DetailedResultCount();
        if ( wvErrorCode == KPEngNwErrPartiallySuccessful )
            {
            DoParseDetailedResultsL( resultBlock,
                                     aStatus,
                                     aOperationID,
                                     aAttributeTypeID,
                                     aContactListID );
            }
        TInt detailEntriesEndCount = aStatus.DetailedResultCount();



        if ( detailEntriesStartCount == detailEntriesEndCount )
            {
            //No detail entries added ==>
            //Look for description and if having a valid one,
            //add it as a detail

            //Parse only 2 tag levels.
            //(Check the the result packet DTD.)
            if ( DecodeL( resultBlock, KDescriptionXMLTag, EFalse, 2 ) )
                {
                HBufC* descriptionBuf = ResultAsUnicodeTextL();
                CleanupStack::PushL( descriptionBuf );
                if ( descriptionBuf->Length() > 0 )
                    {
                    aStatus.AddDetailedResultL( aOperationID, wvErrorCode,
                                                aAttributeTypeID, NULL,
                                                aContactListID, descriptionBuf );
                    }

                CleanupStack::PopAndDestroy( descriptionBuf );
                }
            }
        }


    return aStatus.Status();
    }



// ---------------------------------------------------------
// CPEngXMLParser::DoParseDetailedResultsL()
// ---------------------------------------------------------
//
void CPEngXMLParser::DoParseDetailedResultsL( const TDesC8& aBuffer,
                                              MPEngAdvTransactionStatus2& aStatus,
                                              TInt aOperationID,
                                              const TUint32* aAttributeTypeID,
                                              const TDesC* aContactListID )
    {
    CPtrC8Array* detailedResults = new ( ELeave ) CPtrC8Array( 5 );
    CleanupStack::PushL( detailedResults );

    if ( DecodeL( aBuffer, KDetailedResultXMLTag, ETrue ) )
        {
        //Gather detailed result blocks..
        const TInt detailedResultCount = Count();
        TInt ii;
        for ( ii = 0; ii < detailedResultCount; ii++ )
            {
            detailedResults->AppendL( ResultL( ii ) );
            }


        //And handle each detailed result block..
        for ( ii = 0; ii < detailedResultCount; ii++ )
            {
            DoParseOneDetailedResultL( ( *detailedResults )[ ii ],
                                       aStatus,
                                       aOperationID,
                                       aAttributeTypeID,
                                       aContactListID );
            }
        }

    CleanupStack::PopAndDestroy( detailedResults );
    }



// ---------------------------------------------------------
// CPEngXMLParser::DoParseOneDetailedResultL()
// ---------------------------------------------------------
//
void CPEngXMLParser::DoParseOneDetailedResultL( const TDesC8& aDetailedResult,
                                                MPEngAdvTransactionStatus2& aStatus,
                                                TInt aOperationID,
                                                const TUint32* aAttributeTypeID,
                                                const TDesC* aContactListID )
    {
    //DTD: DetailedResult (Code, Description?, UserID*, GroupID*,
    //                     ScreenName*, MessageID*, ContactList*, Domain*)

    TInt wvErrorCode = KErrNone;
    TBool errorCodeDecoded = EFalse;


    //Look for mandatory parts
    if ( DecodeL( aDetailedResult, KCodeXMLTag, EFalse ) )
        {
        TPtrC8 cspStatusCodeBlock( ResultL() );
        errorCodeDecoded = DecodeWVCspStatusCodeToError( cspStatusCodeBlock,
                                                         wvErrorCode );
        }



    if ( !errorCodeDecoded )
        {
        //Detailed result is somehow malformed...
        aStatus.AddDetailedResultL( aOperationID,
                                    KPEngNwErrInternalServerOrNetworkError,
                                    aAttributeTypeID, NULL,
                                    aContactListID, NULL );
        }


    else
        {
        //Detailed result mandatory parts ok
        //Get the detailed description if any
        HBufC* descriptionBuf = NULL;
        if ( DecodeL( aDetailedResult, KDescriptionXMLTag, EFalse ) )
            {
            descriptionBuf = ResultAsUnicodeTextL();
            if ( descriptionBuf->Length() == 0 )
                {
                delete descriptionBuf;
                descriptionBuf = NULL;
                }
            }
        if ( descriptionBuf )
            {
            CleanupStack::PushL( descriptionBuf );
            }



        // And process the detailed "main" entries
        TInt detailMainEntriesStartCount = aStatus.DetailedResultCount();
            {
            //First user id's
            if ( DecodeL( aDetailedResult, KUserIDXMLTag, EFalse ) )
                {
                const TInt detailedUserIDsCount = Count();

                for ( TInt ii( 0 ); ii < detailedUserIDsCount; ii++ )
                    {
                    HBufC* userID = ResultAsWVAddressL( ii );
                    CleanupStack::PushL( userID );
                    if ( userID->Length() > 0 )
                        {
                        aStatus.AddDetailedResultL( aOperationID, wvErrorCode,
                                                    aAttributeTypeID, userID,
                                                    aContactListID, descriptionBuf );

                        }

                    CleanupStack::PopAndDestroy( userID );
                    }
                }

            if ( DecodeL( aDetailedResult, KContactList, EFalse ) )
                {
                const TInt detailedContactListsCount = Count();

                for ( TInt ii( 0 ); ii < detailedContactListsCount; ii++ )
                    {
                    HBufC* contactList = ResultAsWVAddressL( ii );
                    CleanupStack::PushL( contactList );
                    if ( contactList->Length() > 0 )
                        {
                        aStatus.AddDetailedResultL( aOperationID, wvErrorCode,
                                                    aAttributeTypeID, NULL,
                                                    contactList, descriptionBuf );

                        }

                    CleanupStack::PopAndDestroy( contactList );
                    }
                }
            }

        TInt detailMainEntriesEndCount = aStatus.DetailedResultCount();




        if ( detailMainEntriesStartCount == detailMainEntriesEndCount )
            {
            //No main type entries added ==> add code and possible
            //description as one
            aStatus.AddDetailedResultL( aOperationID,
                                        wvErrorCode,
                                        NULL,
                                        NULL,
                                        NULL,
                                        descriptionBuf );
            }


        if ( descriptionBuf )
            {
            CleanupStack::PopAndDestroy( descriptionBuf );
            }
        }
    }



// ---------------------------------------------------------
// CPEngXMLParser::SetError()
// ---------------------------------------------------------
//
void CPEngXMLParser::SetError( TInt aError )
    {
    iError = aError;
    }




// ==================== cXML CALLBACK FUNCTIONS ====================
// ---------------------------------------------------------
// CPEngXMLParser::StartDocument_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::StartDocument_CB( NW_XML_Reader_t*, void* )
    {
    //No leaving code allowed
    return NW_STAT_SUCCESS;
    }


// ---------------------------------------------------------
// CPEngXMLParser::EndDocument_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::EndDocument_CB( NW_XML_Reader_t*, void* )
    {
    //No leaving code allowed
    return NW_STAT_SUCCESS;
    }


// ---------------------------------------------------------
// CPEngXMLParser::Tag_Start_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Tag_Start_CB( NW_XML_Reader_t* /*aReader*/,
                                          const NW_XML_Reader_Interval_t* aName,
                                          void* aParser )
    {
    //No leaving code allowed

    CPEngXMLParser* parser = reinterpret_cast<CPEngXMLParser*> ( aParser );

    parser->iParsingLevel++;

    TInt len( aName->stop - aName->start );
    TPtrC8 buffer( parser->iData.Mid( aName->start, len ) );

    // now check if the found tag was the one we were looking for
    if ( buffer.Compare( parser->iSearchable ) == 0 )
        {
        TUint itemStart;
        if ( parser->iIncludeTags )
            {
            // the interval includes the tag and the first '<' also
            itemStart = aName->start - 1;
            }
        else
            {
            //Locate next tag end char
            TPtrC8 fullRemainder( parser->iData.Mid( aName->start ) );
            TInt endMarkOffset = fullRemainder.Locate( '>' );
            STORE_AND_RETURN_IF_IN_ERROR( parser, endMarkOffset );

            // here the interval doesn't include the tag nor the ending '>'
            itemStart = aName->start + endMarkOffset + 1;
            }

        TInt err = parser->iIntervals->PushStart( itemStart );
        STORE_AND_RETURN_IF_IN_ERROR( parser, err );
        }

    return NW_STAT_SUCCESS;
    }





// ---------------------------------------------------------
// CPEngXMLParser::Tag_End_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Tag_End_CB( NW_XML_Reader_t* /*aReader*/,
                                        const NW_XML_Reader_Interval_t* aName,
                                        NW_Uint32 /*emptyTagFlag*/,
                                        void* aParser )
    {

    //No leaving code allowed
    CPEngXMLParser* parser = reinterpret_cast<CPEngXMLParser*> ( aParser );

    TInt len( aName->stop - aName->start );
    TPtrC8 buffer( parser->iData.Mid( aName->start, len ) );

    // check the current XML level
    // now check if the found tag was the one we were looking for
    if ( buffer.Compare( parser->iSearchable ) == 0 )
        {
        TUint itemEnd;
        if ( parser->iIncludeTags )
            {
            // the interval includes the tag and the last '>' also

            //Locate next end char
            TPtrC8 fullRemainder( parser->iData.Mid( aName->stop ) );
            TInt endMarkOffset = fullRemainder.Locate( '>' );
            STORE_AND_RETURN_IF_IN_ERROR( parser, endMarkOffset );

            itemEnd = aName->stop + endMarkOffset + 1;
            }
        else
            {
            // here the interval doesn't include the tag nor the starting '</'
            itemEnd = aName->start - 2;
            }


        TInt err = parser->iIntervals->PushEnd( itemEnd );
        STORE_AND_RETURN_IF_IN_ERROR( parser, err );


        // next grab the data between the tags from the original XML data,
        //if we are at allowed level
        if ( ( parser->iMaxParseLevel == 0 ) ||
             ( parser->iParsingLevel <= parser->iMaxParseLevel ) )
            {
            TPoint dataInterval( parser->iIntervals->Pop() );
            TInt intervalLength( dataInterval.iY - dataInterval.iX );


            TPtrC8 resultSection( NULL, 0 );
            if ( intervalLength > 0 ) // if the stack was broken, the length will be 0
                {
                resultSection.Set( parser->iData.Mid( dataInterval.iX, intervalLength ) );
                }


            TInt err = parser->iResults.Append( resultSection );
            STORE_AND_RETURN_IF_IN_ERROR( parser, err );
            }
        }


    parser->iParsingLevel--;
    return NW_STAT_SUCCESS;
    }





// ---------------------------------------------------------
// CPEngXMLParser::Attr_Start_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Attr_Start_CB( NW_XML_Reader_t*,
                                           const NW_XML_Reader_Interval_t* /*pI_name*/,
                                           void* )
    {
    //No leaving code allowed

    return NW_STAT_SUCCESS;
    }


// ---------------------------------------------------------
// CPEngXMLParser::Attr_VarVal_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Attr_VarVal_CB( NW_XML_Reader_t*,
                                            const NW_XML_Reader_Interval_t* /*pI_name*/,
                                            const NW_XML_Reader_Interval_t* /*pI_value*/,
                                            void* )

    {
    //No leaving code allowed

    return NW_STAT_SUCCESS;
    }



// ---------------------------------------------------------
// CPEngXMLParser::Attributes_End_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Attributes_End_CB( NW_XML_Reader_t*,
                                               NW_Uint32 /*attributeCount*/,
                                               void* )
    {
    //No leaving code allowed

    return NW_STAT_SUCCESS;
    }



// ---------------------------------------------------------
// CPEngXMLParser::Attr_Entity_VarVal_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Attr_Entity_VarVal_CB ( NW_XML_Reader_t* /*pT*/,
                                                    const NW_XML_Reader_Interval_t* /*pI_name*/,
                                                    NW_Uint8* /*pValue*/,
                                                    NW_Uint32 /*valueByteLength*/,
                                                    void* /*pV*/ )
    {
    //No leaving code allowed

    return NW_STAT_SUCCESS;
    }


// ---------------------------------------------------------
// CPEngXMLParser::Exception_CB()
// ---------------------------------------------------------
//
NW_Status_t CPEngXMLParser::Exception_CB( NW_XML_Reader_t*, void* )
    {
    //No leaving code allowed

    return NW_STAT_FAILURE;
    }


//  End of File


