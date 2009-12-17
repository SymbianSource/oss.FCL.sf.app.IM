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

#ifndef MPENGPARSER_H
#define MPENGPARSER_H

#include "PEngXMLTags.h"
//#include "MPEngAdvTransactionStatus2.h"



//FORWARD DECLARATION
class MPEngXMLParser;
class MPEngAdvTransactionStatus2;



// FUNCTION PROTOTYPES

/**
 * Factory method to create XML Parser.
 *
 * @since 3.0
 */
IMPORT_C MPEngXMLParser* CreateXMLParserL();
IMPORT_C MPEngXMLParser* CreateXMLParserLC();





//  CLASS DEFINITION
/**
 * This class defines the interface for Presence XML Parser.
 * XML parser is used to parse XML data.
 *
 * @since 2.1
 */
class MPEngXMLParser
    {
    public: //Parsing functions


        /**
         * Decodes given XML data according to the given string to be found.
         * @param aBuffer Initial data to be parsed
         * @param aString The tag name to be searched
         * @param aTags Tells if the tags are included in the result
         * @param aLevel Restricts the parsing to a certain tag level, 0 means
         *                that there's no restrictions
         * @return TBool The boolean value of how decoding went.
         */
        virtual TBool DecodeL( const TDesC8& aBuffer,
                               const TDesC8& aString,
                               TBool aTags,
                               TInt aLevel = 0 ) = 0;


        /**
         * Gets the number of decode results.
         * @return TInt number of results.
         */
        virtual TInt Count() = 0;



        /**
         * Gets raw result from the decoded result array.
         *
         * @param aIndex the wanted result
         * @return TPtrC8 a pointer to the result data
         */
        virtual TPtrC8 ResultL( TInt aIndex = 0 ) = 0;



        /**
         * Gets narrow text from the decoded result array.
         *
         * Performed steps:
         * 1. UnEscapes XML entities.
         *
         * @param aIndex Index of wanted result.
         * @return Narrow text. Buffer ownership is returned to caller.
         */
        virtual HBufC8* ResultAsNarrowTextL( TInt aIndex = 0 ) = 0;



        /**
         * Gets unicode text from the decoded result array.
         *
         * Performed steps:
         * 1. UnEscapes XML entities.
         * 2. Converts text from Utf8 to Unicode.
         *
         * @param aIndex Index of wanted result.
         * @return Unicode text. Buffer ownership is returned to caller.
         */
        virtual HBufC16* ResultAsUnicodeTextL( TInt aIndex = 0 ) = 0;



        /**
         * Gets WV Address from the decoded result array.
         *
         * Performed steps:
         * 1. UnEscapes XML entities.
         * 2. Converts text from Utf8 to Unicode.
         * 3. UnEscapes WV Address characters
         *
         * @param aIndex Index of wanted result.
         * @return WV Address. Buffer ownership is returned to caller.
         */
        virtual HBufC16* ResultAsWVAddressL( TInt aIndex = 0 ) = 0;



        /**
         * Gets Base64Decodeddata from the decoded result array.
         *
         * Performed steps:
         * 1. Coverts data from the BASE64 format
         *
         * @param aIndex Index of wanted result.
         * @return Base64 decoded text. Buffer ownership is returned to caller.
         */
        virtual HBufC8* ResultAsBase64DecodedL( TInt aIndex = 0 ) = 0;




    public: //Transaction result parsing functions


        /**
         * Parses the transaction result from the XML data.
         * Overloaded version to allow client to define
         * wether the valid result structure is required or not.
         *
         * @param aBuffer The XML buffer containing the response
         * @param aTransactionOperation The TransactionOperation that
         * caused the possible detailed errors.
         * @param aStatus MPEngAdvTransactionStatus object where the
         * userIDs and return values are stored.
         * @param aResultRequired If ETrue, the valid result
         * structure is required. If EFalse, or valid result
         * structure is not required. If valid result
         * structure is required but it isnt found from XML,
         * parser generates KPEngNwErrInternalServerOrNetworkError.
         * On the return aResultRequired contains ETrue, if valid
         * structure was found. Else EFalse.
         * @return The main result value from transaction status.
         */
        virtual TInt ParseResultL( const TDesC8& aBuffer,
                                   TInt aTransactionOperation,
                                   MPEngAdvTransactionStatus2& aStatus,
                                   TBool& aResultRequired ) = 0;

        /**
         * Parses the transaction result from the XML data.
         * Overloaded version requiring the valid result
         * structure.
         *
         * @param aBuffer The XML buffer containing the response
         * @param aTransactionOperation The TransactionOperation that
         * caused the possible detailed errors.
         * @param aStatus MPEngAdvTransactionStatus object where the
         * userIDs and return values are stored.
         * @return The main result value from transaction status.
         */
        virtual TInt ParseResultL( const TDesC8& aBuffer,
                                   TInt aTransactionOperation,
                                   MPEngAdvTransactionStatus2& aStatus ) = 0;

        /**
         * Parses the transaction result from the XML data.
         * Client can pass the contact list id, where the results relate.
         * Requires the valid result structure.
         *
         * @param aBuffer The XML buffer containing the response
         * @param aTransactionOperation The TransactionOperation that
         * caused the possible detailed errors.
         * @param aStatus MPEngAdvTransactionStatus object where the
         * userIDs and return values are stored.
         * @param aContactList The name of the contact list this result was
         * attached to.
         * @return The main result value from transaction status.
         */
        virtual TInt ParseContactListResultL( const TDesC8& aBuffer,
                                              TInt aTransactionOperation,
                                              const TDesC& aContactList,
                                              MPEngAdvTransactionStatus2& aStatus ) = 0;




    public: //Destructor & Cleanup support

        /**
         *  Release method.
         *  When called, deletes object and
         *  frees all its resources.
         *
         *  Concrete parser object can be thus pushed
         *  on the CleanupStack using CleanupClosePushL().
         */
        virtual void Close() = 0;


        /**
         * Inline virtual destructor.
         * Concrete instancies can be deleted
         * trough this interface.
         */
        virtual ~MPEngXMLParser() {}


    };

#endif      //  MPENGPARSER_H

//  End of File
