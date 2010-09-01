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

#ifndef __CPENGXMLPARSER_H
#define __CPENGXMLPARSER_H

//  INCLUDES
#include <badesca.h>
#include <nw_cXML_Parser.h>
#include "MPEngXMLParser.h"



// FORWARD DECLARATIONS
class CPEngParserStack;

/**
 *  The parser class for decoding XML
 */
class CPEngXMLParser : public CBase,
            public MPEngXMLParser
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPEngXMLParser* NewLC();

        /**
         * Destructor.
         */
        ~CPEngXMLParser();


    private:  // Constructors

        /**
         * C++ default constructor.
         */
        CPEngXMLParser();


        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();



    public: // Functions from MPEngXMLParser

        /**
         * @See MPEngXMLParser
         */
        TBool DecodeL( const TDesC8& aBuffer,
                       const TDesC8& aString,
                       TBool aTags,
                       TInt aLevel = 0 );

        TInt Count();
        TPtrC8 ResultL( TInt aIndex = 0 );


        HBufC8* ResultAsNarrowTextL( TInt aIndex = 0 );
        HBufC16* ResultAsUnicodeTextL( TInt aIndex = 0 );
        HBufC16* ResultAsWVAddressL( TInt aIndex = 0 );
        HBufC8* ResultAsBase64DecodedL( TInt aIndex = 0 );

        TInt ParseResultL( const TDesC8& aBuffer,
                           TInt aOperationID,
                           MPEngAdvTransactionStatus2& aStatus,
                           TBool& aResultRequired );

        TInt ParseResultL( const TDesC8& aBuffer,
                           TInt aOperationID,
                           MPEngAdvTransactionStatus2& aStatus );

        TInt ParseContactListResultL( const TDesC8& aBuffer,
                                      TInt aOperationID,
                                      const TDesC& aContactList,
                                      MPEngAdvTransactionStatus2& aStatus );
        void Close();





    private: //Private helpers

        void ResetResults();
        HBufC16* DoWVAddressUnescapeL( const TDesC16& aSrc );
        HBufC8* DoXMLUnEscapingL( const TDesC8& aSrc );

        TInt DoParseResultL( const TDesC8& aBuffer,
                             MPEngAdvTransactionStatus2& aStatus,
                             TInt aOperationID,
                             const TUint32* aAttributeTypeID,
                             const TDesC* aContactListID,
                             TBool& aResultRequired );

        void DoParseDetailedResultsL( const TDesC8& aBuffer,
                                      MPEngAdvTransactionStatus2& aStatus,
                                      TInt aOperationID,
                                      const TUint32* aAttributeTypeID,
                                      const TDesC* aContactListID );

        void DoParseOneDetailedResultL( const TDesC8& aDetailedResult,
                                        MPEngAdvTransactionStatus2& aStatus,
                                        TInt aOperationID,
                                        const TUint32* aAttributeTypeID,
                                        const TDesC* aContactListID );
        void SetError( TInt aError );


    private: // The callback functions for the XML parser

        static NW_Status_t StartDocument_CB( NW_XML_Reader_t*, void* );
        static NW_Status_t EndDocument_CB( NW_XML_Reader_t*, void* );
        static NW_Status_t Tag_Start_CB( NW_XML_Reader_t* aReader,
                                         const NW_XML_Reader_Interval_t* aName,
                                         void* aParser );

        static NW_Status_t Tag_End_CB( NW_XML_Reader_t* aReader,
                                       const NW_XML_Reader_Interval_t* aName,
                                       NW_Uint32 emptyTagFlag,
                                       void* aParser );

        static NW_Status_t Attr_Start_CB( NW_XML_Reader_t*,
                                          const NW_XML_Reader_Interval_t* pI_name,
                                          void* );

        static NW_Status_t Attr_VarVal_CB( NW_XML_Reader_t*,
                                           const NW_XML_Reader_Interval_t* pI_name,
                                           const NW_XML_Reader_Interval_t* pI_value,
                                           void* );

        static NW_Status_t Attributes_End_CB( NW_XML_Reader_t*,
                                              NW_Uint32 attributeCount,
                                              void* );

        static NW_Status_t Attr_Entity_VarVal_CB ( NW_XML_Reader_t* pT,
                                                   const NW_XML_Reader_Interval_t* pI_name,
                                                   NW_Uint8* pValue,
                                                   NW_Uint32 valueByteLength,
                                                   void* pV );

        static NW_Status_t Exception_CB( NW_XML_Reader_t*, void* );




    private:    // Data

        //OWN: an array to hold the parsing results
        RArray<TPtrC8> iResults;

        //REF: the incoming xml data to be parsed
        TPtrC8 iData;

        //REF: the string (token) to be searched
        TPtrC8 iSearchable;

        //OWN:  holds the result intervals
        CPEngParserStack* iIntervals;

        //OWN: a boolean to tell if the tags will be taken into the result
        TBool iIncludeTags;

        //OWN: current parsing level
        TInt iParsingLevel;

        //OWN: restriction to the level we are parsing
        TInt iMaxParseLevel;

        //OWN: callback object for the xml parser
        NW_XML_Parser_EventCallbacks_s iCallBacks;

        //OWN:  xml toolkits xml reader object
        NW_XML_Reader_t iReader;

        //OWN: Error
        TInt    iError;
    };

#endif      // __CPENGXMLPARSER_H

//  End of File

