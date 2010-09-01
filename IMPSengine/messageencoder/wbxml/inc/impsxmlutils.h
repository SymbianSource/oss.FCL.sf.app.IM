/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*       Provides the methods for pureData handling.
*
*
*/


#ifndef CImpsXmlUtils_H
#define CImpsXmlUtils_H

//  INCLUDES
#include "ImpsDataAccessorApi.h"

// CLASS DECLARATION
class CImpsXmlUtils :public CBase
    {
    public:  
        
        /**
        * Method encodes TransactionContent to the
        * XML format and stores to the data structure 
        * @param aImpsData data structure containing the data
        */
        void TransactionContentToXmlL( MImpsDataAccessor& aImpsData );
         /**
        * Method decodes TransactionContent from the
        * XML format and stores data to the data structure 
        * @param aImpsData data structure containing the data
        * @return primitive or KErrNotFound
        */
        TInt XmlToTransactionContentL( MImpsDataAccessor& aImpsData );

        /**
        * Method writes xml element start tag to the buffer
        * @param aTag name of the tag.
        */
        void StartElementL( const TDesC8& aTag, const TDesC8& aAttribute );

        /**
        * Method writes xml element end tag to the buffer
        * @param aTag name of the tag.
        */
        void EndElementL( const TDesC8& aTag );
        
        /**
        * Method writes data to the encode buffer
        * @param aData pointer to the data 
        */
        void WriteToBufferL( const TDesC8& aData );
        
        void StorePureDataL( MImpsDataAccessor& aImpsData, TInt aIndex, TInt aMessageType );
        
        void ResetL( );

        TPtrC8 EscapeXmlL ( const TDesC8& aDes );
        /**
        * Two-phased constructor.
        */
        static CImpsXmlUtils* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CImpsXmlUtils( );

    private:
        /**
        * C++ default constructor.
        */
        CImpsXmlUtils( );
        /**
        * By default constructor is private.
        */
        void ConstructL( );
        /**
        * Method maps strings to the enumerated values
        * @param aName name of the element
        * @return enumerated value
        */
        TImpsContent NameToKeyPresencePrimitives( TDesC8* aName );
        /**
        * Method maps strings to the enumerated values
        * @param aName name of the element
        * @return enumerated value
        */
        TImpsPresenceContent NameToKeyPresenceElements( TDesC8* aName );
        /*
        * Method converts integer to the string format
        * @param aInteger integer to be converted 
        */
//        TPtrC8 ConvertIntToPtr( TInt aInteger );
        /**
        * Method converts 8-bit data to the enumerated value
        * @param aData
        * @return converted integer
        */
        TInt ConvertToIntL( TDesC8* aData, TInt aKey );
        /**
        * Method converts 16-bit descriptor to the 8-bit format
        * @param aDes descriptor to be converted 
        * @return pointer to the converted descriptor
        */
//        TPtrC8 Convert16To8L( TDesC* aDes );
        /**
        * Method converts boolean to the character format
        * @param aBool boolean to be converted
        * @return pointer to the converted descriptor
        */
//        TPtrC8 ConvertBooleanToChar( TBool aBool );
        /**
        * Method converts 8-bit data to the boolean
        * @param aData data buffer
        * @return the converted boolean
        */
        TBool ConvertToBoolean( TDesC8* aData );
        /**
        * Method writes empty xml element to the buffer
        * @param aTag name of the tag.
        */
        void WriteEmptyElementL( const TDesC8& aTag );
        /**
        * Method restores desc type element and writes it to the buffer
        * If the descriptor's length is zero, the 
        * encoder will write empty string
        * @param aKey key to be restored
        * @param aElementName name of the element
        * @param aKeyType type of the key, default value is EImpsKeyTypeIM
        */
//        void EncodeDescL( TInt aKey, 
//                          TPtrC8 aElementName, 
//                          enum TImpsKeyType = EImpsKeyTypeIM );
        /**
        * Method restores desc type elements and writes them to the buffer
        * @param aKey key to be restored
        * @param aElementName name of the element
        * @param aKeyType type of the key
        */
//        void EncodeMultipleDescsL( TInt aKey, 
//                                   TPtrC8 aElementName, 
//                                   enum TImpsKeyType = EImpsKeyTypeIM );
        /**
        * Method restores integer type element and writes it to the buffer
        * @param aKey key to be restored
        * @param aElementName name of the element
        * @param aKeyType type of the key, default value is EImpsKeyTypeIM
        */
//        void EncodeIntegerL( TInt aKey, 
//                             TPtrC8 aElementName, 
//                             enum TImpsKeyType = EImpsKeyTypeIM );
        /**
        * Method restores boolean type element and writes it to the buffer
        * @param aKey key to be restored
        * @param aElementName name of the element
        * @param aKeyType type of the key, default value is EImpsKeyTypeIM
        */
//        void EncodeBooleanL( TInt aKey, 
//                             TPtrC8 aElementName, 
//                             enum TImpsKeyType = EImpsKeyTypeIM );
//                
        /**
        * Helping methods for parsing. 
        */
        void DoParseL( );
        void ParseElementsL( );
        void UpdatePosition( TInt aLength );
        TInt FindNextTag( );
        TInt CreateKeyL( TInt aLength );
        /**
        * Save data to the internal data structure.
        */
        void SaveDataL( TDesC8* aData, TInt aKe) ;
        /**
        * Actual encoding 
        */
        void DoEncodeL( MImpsDataAccessor& aImpsDat);
        void EncodePresencePrimitiveL( MImpsKey* aKey, 
                                       MImpsDataAccessor& aImpsData );
        void UnescapeL ( );

    private:    // Data

        enum TImpsParseState
            {
            EImpsUndefined = 0,
            EImpsStartElement,
            EImpsEndElement,
            EImpsEmptyElement
            };

        CBufFlat*               iEncodeBuffer;
        HBufC8*                 iElementValue;
        HBufC*                  iElementData;
        HBufC8*                 iIntegerValue;
        MImpsDataAccessor*      iImpsData;
        MImpsKey*               iAccessKey;
        TInt                    iPosition;
        TInt                    iBufSize;
        TInt                    iEncodeBufferSize;
        TInt                    iPrimitive;
        TPtrC8                  iData;
        TBool                   iContinue;

    };

#endif      
            
// End of File
