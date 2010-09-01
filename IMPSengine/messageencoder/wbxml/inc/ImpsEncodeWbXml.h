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
* Description:  Imps engine wbxml message encoder
*
*
*/


#ifndef CImpsEncodeWbXml_H
#define CImpsEncodeWbXml_H

//  INCLUDES
#include <e32base.h>
#include "ImpsMessageInterpreterApi.h"
#include "nw_encoder_domencoder.h"
#include "ImpsXmlUtils.h"
#include "impscommonenums.h"

// CONSTANTS
const TInt KImpsMaxDictionaries        = 2;
const TInt KImpsValueListAlternative   = 0x01;
const TInt KImpsValueListOptional      = 0x02;
const TInt KImpsValueListMultiple      = 0x04;
const TInt KImpsValueListOnceOrMore    = 0x08;
const TInt KImpsValueListEnd           = 0x10;
const TInt KImpsValueListEmpty         = 0x20;
const TInt KImpsSubList                = 0x40;
const TInt KImpsMaxTextItemCount       = 2;
const TInt KImpsExtraElemEnd           = -2;

// FORWARD DECLARATIONS
class MImpsKey;

// CLASS DECLARATION

class CImpsEncodeWbXml :public CBase, public MImpsEncoder
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CImpsEncodeWbXml* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CImpsEncodeWbXml();

    public: // New functions
        
        /**
        * method builds wbxml encoded message based on data
        * included in class aImpsData
        * @param aImpsData reference to MImpsDataAccessor class.
        * @param aEncodeBuffer buffer to hold the encoded message,
        * @return (parser) error code
        */
        void EncodeMessageL(
            MImpsDataAccessor& aImpsData,
            CBufFlat& aEncodeBuffer );

    private:

        /**
        * Destructor
        */
        virtual void Destroy();

        /**
        * wbxml message MIME type
        * @return MIME type
        */
        virtual TPtrC8 MimeType();
         

        struct SImpsValueList {
        TInt Begin;
        TInt Length;
        TInt Next;
        TInt Current;
        TInt CurrentLength;
        TInt Flags;
        };

        /**
        * C++ default constructor.
        */
        CImpsEncodeWbXml( );

        /**
        * By default constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsEncodeWbXml( const CImpsEncodeWbXml& );

        // Prohibit assigment operator
        CImpsEncodeWbXml& operator= ( const CImpsEncodeWbXml& );
        
        /**
        * method adds one element to DOM tree
        * @param aElement element name
        * @param aNode parent DOM node
        * @param aNode element index if may exist multiple times
        * @return error code
        */
        TInt EncodeElementL(TDesC &aElement, NW_DOM_ElementNode_t* aNode, TInt aIndex);

        /**
        * method creates dictionary, which maps WBXML tokens to DTD tags
        * @return error code
        */
        TInt CreateDictionary( );

        /**
        * method reads one element declaration from DTD
        * @param aElement element name
        * @return error code
        */
        TPtrC ReadElementDeclaration(TDesC &aElement);

        /**
        * method stores node value to DOM tree
        * @param aToken element WBXML token value
        * @param aNode element DOM node
        * @param aValue element value
        * @return error code
        */
        TInt EncodeValueL(TInt aToken, NW_DOM_TextNode_t* aNode,TInt aValue);

        /**
        * method reads next value in element declaration
        * @param aValueList
        * @return value
        */
        TPtrC ReadNextValue(SImpsValueList* aValueList,TPtrC aValues);

        /**
        * method adds attribute values to element DOM node
        * @param aToken element WBXML token value
        * @param aNode element DOM node
        * @return error code
        */
        TInt EncodeAttributes(TInt aToken, NW_DOM_ElementNode_t* aNode);

        /**
        * method encodes alternative values in WV element decralation
        * @param aToken element WBXML token value
        * @param aNode parent DOM node
        * @return error code
        */
        TInt EncodeSubListL(TPtrC aValue, NW_DOM_TextNode_t* aNode);

        /**
        * method encodes multibyte integer value
        * @param aValue integer value
        * @param aBuffer for encoded value
        */
        void StoreInteger(TUint32 aValue, TPtr8 aBuffer);

        /**
        * method sets DTD pointer
        * @param aOffset offset in DTD buffer
        * @param aLength string length
        * @return new pointer value
        */
        TPtrC SetPointer(TInt aOffset,TInt aLength );

        /**
        * method count string length
        * @param aPtr contains string
        * @return string length
        */
        TInt CountContentSize(TDesC*& aPtr);

        /**
        * method counts message content size
        * @param aToken content size token
        * @return success code
        */
        TInt SetContentSizeL(TInt aToken, NW_DOM_ElementNode_t* aNode);

        /**
        * method selects alternative value
        * @param aToken element token
        * @param aValue
        * @return alternative value
        */
//        TInt SelectAlternativeValueL(TInt aToken, TPtrC aValue);

        /**
        * method updates data accessor key
        * @param aToken element token
        */
        void PopAccessKeyL(TInt aToken);

        /**
        * method fetches next element 
        * @param aValueList
        * @param aElementCounter
        * @param aRcode
        * @param aElementList
        * @param aCurrentElement
        * @return next element
        */
        TPtrC FetchNextElement(SImpsValueList* aValueList,TInt& aElementCounter,TInt& aRcode,
                                TPtrC aElementList,TPtrC aCurrentElement);

        /**
        * method updates data accessor key
        * @param aToken element token
        * @param aIndex element index
        */
        TInt SetDataAccessorKeyL(TInt aToken,TInt aIndex);

        /**
        * method reads sub element list
        * @param aElement
        * @param aValueList
        */
        void ReadElementList(TPtrC aElement, SImpsValueList* aValueList);

        /**
        * method reads wbxml token value
        * @param aElement
        * @return wbxml token
        */
        TInt ReadToken(TPtrC aElement);

        /**
        * method stores 8 bit string to DOM node
        * @param aToken element token
        * @param aElemCount
        * @return
        */
        TInt EncodeString8L(TInt aToken,TInt& aElemCount);

        /**
        * method stores extension value to DOM node
        * @param aToken element token
        * @param aElemCount
        * @return
        */
        TInt EncodeExtL(TInt aToken,TInt& aElemCount);

        /**
        * method stores string to DOM node
        * @param aToken element token
        * @param aElemCount
        * @return
        */
        TInt EncodeStringL(TInt aToken,TInt& aElemCount);

        /**
        * method stores integer value to DOM node
        * @param aToken element token
        * @param aElemCount
        * @return
        */
        TInt EncodeIntegerL(TInt aToken,TInt& aElemCount);

        /**
        * method stores boolean value to DOM node
        * @param aToken element token
        * @param aElemCount
        * @return
        */
        TInt EncodeBooleanL(TInt aToken,TInt& aElemCount);

        /**
        * method searches possible prefix from string
        * @param aString is given string
        * @param aOffset
        * @return possible prefix
        */
        TInt SearchPrefix(TPtrC8 aString,TInt& aOffset);

    private:    // Data
        
    TInt                      iError;
    CBufFlat*                 iEncodeBuffer;
    MImpsDataAccessor*        iImpsData;
    MImpsKey*                 iAccessKey;
    NW_DOM_DocumentNode_t*    iDocument;
    TInt                      iPosition;
    TInt                      iCurrentDictionary;
    HBufC*                    iCspDtd;
    HBufC*                    iContentDtd;
    HBufC*                    iPresenceDtd;
    TInt                      iValuePosition;
    TInt                      iElementPosition;
    TInt                      iDictionaryCount;
    NW_WBXML_Dictionary_t*    iDictionaries[KImpsMaxDictionaries];
    CImpsWbXmlData*           iWbXmlData;
    HBufC*                    iElementValue;
    HBufC8*                   iStoreValue;
    NW_String_t               iString;
    NW_WBXML_Opaque_t         iOpaque;
    NW_DOM_TextItem_t         iTextItem[KImpsMaxTextItemCount];
    NW_DOM_AttrVal_t          iAttribute;
    TInt                      iNameSpace;
    TImpsKeyType              iKeyType;
    TInt                      iPEC;
    CImpsXmlUtils*            iXmlUtils;
    TImpsCspVersion           iCspVersion;

    };

#endif      // ?INCLUDE_H   
            
// End of File
