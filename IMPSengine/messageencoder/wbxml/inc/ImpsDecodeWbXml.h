/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Imps engine wbxml message decoder
*
*
*/


#ifndef CImpsDecodeWbXml_H
#define CImpsDecodeWbXml_H

//  INCLUDES
#include <e32base.h>
#include <cstack.h>
#include "ImpsDataAccessorApi.h"
#include "ImpsMessageInterpreterApi.h"
#include "ImpsWbXmlData.h"
#include "nw_encoder_domencoder.h"
#include "nw_wbxml_reader.h"
#include "ImpsXmlUtils.h"

// CONSTANTS
const TInt KMaxDictionaries           = 2;

// DATA TYPES

	// decoding state
	enum TImpsDecodeState
        {
            EImpsDecodeStateIdle,
            EImpsDecodeStateTagStart,
            EImpsDecodeStateTagEnd,
            EImpsDecodeStateAttributeStart,
            EImpsDecodeStateAttributeValue,
            EImpsDecodeStateContent,
            EImpsDecodeStateExtension,
            EImpsDecodeStateExtensionValue,
            EImpsDecodeStateLiteral,
            EImpsDecodeIllegal
        };

// FORWARD DECLARATIONS
class CImpsKey;
class MImpsCSPSession;

// CLASS DECLARATION

class CImpsDecodeWbXml :public CBase, public MImpsDecoder
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CImpsDecodeWbXml* NewL( MImpsCSPSession* aCallback );

    public: // New functions
        
        /**
        * IMPS wbxml message decoder 
        * @param aImpsData reference to MImpsDataAccessor class.
        * @param aEncodeBuffer buffer to hold the decoded message,
        */
        void DecodeMessageL(
            MImpsDataAccessor& aImpsData,
            HBufC8** aEncodeBuffer );

        /**
        * Method decodes (wb)xml message to internal data structures
        * in asynchronous manner. The completion of the operation is signaled in
        * MImpsCSPSession::TransportResponseParsedL() method.
        * @param aEncodeBuffer buffer to hold the encoded message
        *        Please note that the buffer ownership is taken!!!
        */
        TInt DecodeMsgAsyncL(
            HBufC8** aBuffer);

        /**
        * Method to cancel the asynchronous decoding
        */
        void CancelDecoding();
        
    private:

        /**
        * Destructor
        */
        virtual void Destroy();
        
        /**
        * Destructor.
        */
        virtual ~CImpsDecodeWbXml();

        /**
        * C++ default constructor.
        */
        CImpsDecodeWbXml();

        /**
        * method updates current parser state
        * @param aState new state
        */
        void SetState(TImpsDecodeState aState);

        /**
        * method stores WBXML inline string
        * @param aParser parser pointer
        * @param aLength string length
        * @return (parser) error code
        */
        NW_Status_t StoreInlineStringL(NW_WBXML_Parser_t* aParser,TInt aLength);

        /**
        * method stores WBXML table string
        * @param aIndex string table index
        * @return (parser) error code
        */
        NW_Status_t StoreTableStringL(TUint32 aIndex);

        /**
        * method stores WBXML opaque data
        * @param aParser parser pointer
        * @param aLength data length
        * @return (parser) error code
        */
        NW_Status_t StoreOpaqueData(NW_WBXML_Parser_t* aParser,TUint32 aLength);

        /**
        * method verifies WBXML document header
        * @param aDocument WBXML document pointer
        * @return (parser) error code
        */
        NW_Status_t StoreDocument(NW_WBXML_Document_t* aDocument);

        /**
        * method for wbxml token handling
        * @param aToken WBXML token value
        * @return (parser) error code
        */
        NW_Status_t StoreTokenL(TUint32 aToken);

        /**
        * method for wbxml entity handling
        * @param aValue value to be decoded
        * @return (parser) error code
        */
        NW_Status_t DecodeEntityL(TUint32 aValue);

        /**
        * method stores parsed out data to data accessor and
        * removes token from data access key
        * @return (parser) error code
        */
        NW_Status_t TagEndHandleL( );

        /**
        * parser exception handler
        * @param aException exception value
        */
        void ExceptionHandle(NW_WBXML_Exception_t aException);
      
        /**
        * method stores parsed string
        * @param aString string to be stored
        */
        void StoreStringL(NW_String_t aString);

        /**
        * method creates the correct index in case of multiple elements
        * @param aImpsKey key of the element
        */
        void CheckIfMultipleElementsL(TInt aImpsKey);


        /**
        * method checks if the given token is a presence transaction token
        * @param aToken
        */
        TBool IsPureTransaction(TInt aToken);


        /**
        * method makes a simple 16bit to 8bit conversion for DTD elements
        * Use only if the unicode contains only ASCII characters!
        * To avoid dynamic memory allocation the maximum length of the DTD 
        * element is set to be 100 characters! (Should be enough in WV DTDs)
        * @param aToken
        */
        void ConvertToDes8( TUint16* aElement, TDes8& aConverted );


        /**
        * By default constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsDecodeWbXml( const CImpsDecodeWbXml& );

        // Prohibit assigment operator
        CImpsDecodeWbXml& operator= ( const CImpsDecodeWbXml& );

    private:    // Data
        TInt                    iError;
        TInt                    iCurrentDictionary;
        TInt                    iDictionaryCount;
        NW_WBXML_Dictionary_t*  iDictionaries[KMaxDictionaries];
        TInt                    iDecodingState;
//        HBufC8*               iDecodeBuffer;
        NW_WBXML_Document_t*    iDocument;
        TInt                    iPreviousElement;
        TInt                    iExtension;
        TInt                    iAttribute;
        MImpsDataAccessor*      iImpsData;
        MImpsKey*               iAccessKey;
        CImpsWbXmlData*         iWbXmlData;
        TInt                    iCurrentCodePage;
        TInt                    iElementToken;
        HBufC8*                 iStoreValue8;
        HBufC8*                 iElementValue;
        HBufC*                  iStoreValue;
        TInt                    iExtensionValue;
        TBuf8<4>                iVersion;
        NW_WBXML_Opaque_t       iOpaque;
        TInt                    iContentSize;
        TImpsKeyType            iKeyType;
        TInt                    iNameSpace;
        TInt                    iPreviousNameSpace;
        CImpsXmlUtils*          iXmlUtils;
        TInt                    iPEC;
        TBool                   iDecodeToXml;
        TInt                    iTransactions;
        TBool                   iTypeStatus;
        CStack<NW_String_UCS2Buff_t, EFalse>*          iStack;
        TInt			iPureMessageType;

    public:     // Friend classes

    friend NW_Status_t TagStart(NW_WBXML_Parser_t* aParser, void* aContext);
    friend NW_Status_t FQTokenL(NW_WBXML_Parser_t* aParser,TUint32 aFqtoken, void* aContext);
    friend NW_Status_t InlineStringL(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext);
    friend NW_Status_t TableStringL(NW_WBXML_Parser_t* aParser,TUint32 aIndex, void* aContext);
    friend NW_Status_t Extension(NW_WBXML_Parser_t* aParser,void* aContext);
    friend NW_Status_t Opaque(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext);
    friend NW_Status_t BinaryL(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext);
    friend NW_Status_t AttributeStart(NW_WBXML_Parser_t* aParser, void* aContext);
    friend NW_Status_t StartDocument(NW_WBXML_Parser_t* aParser,NW_WBXML_Document_t* aDocument, void* aContext);
    friend NW_Status_t AttributeValue(NW_WBXML_Parser_t* aParser, void* aContext);
    friend NW_Status_t TagEndL(NW_WBXML_Parser_t* aParser, void* aContext);
    friend NW_Status_t Content(NW_WBXML_Parser_t* aParser, void* aContext);
    friend NW_Status_t EntityL(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext);
    friend NW_Status_t Exception(NW_WBXML_Parser_t* aParser,NW_WBXML_Exception_t aException, void* aContext);
    friend NW_Status_t Binary(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext);

    };

#endif      // ?INCLUDE_H   
            
// End of File
