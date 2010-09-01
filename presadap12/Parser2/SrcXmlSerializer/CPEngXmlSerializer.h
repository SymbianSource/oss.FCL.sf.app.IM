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

#ifndef __CPENGXMLSERIALIZER_H__
#define __CPENGXMLSERIALIZER_H__

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>

#include "MPEngXmlSerializer.h"
#include "TPEngWriter.h"



//MODULE DATA STRUCTURES
/**
 * Enumeration defining different
 * serializer states.
 */
enum TPEngSerializerState
    {
    EPEngInRoot =  0,  ///< Initial state. No start tag or element open (ready to accept start tags or content).
    EPEngInStartTag,   ///< Start tag's start written (ready to accept XML attributes).
    EPEngInElement,    ///< Start tag closed, in element (ready to accept content).
    };


/**
 * Container for serializer state specific data.
 *
 * Data holded by this structure is
 * used when rolling back the serializer
 * state.
 */
struct TPEngSerializerStateData
    {
    TInt iWriterLength;           ///< Length of produced XML in the writer.
    TPEngSerializerState iState;  ///< Serializer state enumeration.
#ifdef __SERIALIZER_TAG_NAME_ASSERT
    TInt iAssertionStackCount;    ///< Count of elements there in the element stack.
#endif
    };



//FORWARD DECLARATION
class CPEngTagAssertionStack;



// CLASS DECLARATION
/**
 * XML Serializer implementation.
 *
 * @lib PEngParser.lib
 * @since 2.1
 */
class CPEngXmlSerializer : public CBase,
            public MPEngXMLSerializer
    {
    public:  // Two-phased constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 2.1
         */
        IMPORT_C static CPEngXmlSerializer* NewL( TDes8& aBuffer );
        IMPORT_C static CPEngXmlSerializer* NewLC( TDes8& aBuffer );


        /**
         * Destructor.
         */
        ~CPEngXmlSerializer();

    private:

        /**
         * C++ constructor.
         */
        CPEngXmlSerializer( TDes8& aBuffer );

        /**
         * Symbian OS constructor.
         */
        void ConstructL();

    public:  // XML Serialization methods from MPEngXmlSerializer


        void Close();

        MPEngXMLSerializer& StartTagL( const TDesC8& aName );
        MPEngXMLSerializer& AttributeL( const TDesC8& aName, const TDesC8& aValue );
        MPEngXMLSerializer& EndTagL( const TDesC8& aName );


        MPEngXMLSerializer& RawValueL( const TDesC8& aValue );
        MPEngXMLSerializer& NarrowTextL( const TDesC8& aText );
        MPEngXMLSerializer& UnicodeTextL( const TDesC16& aText );
        MPEngXMLSerializer& WvAddressL( const TDesC16& aAddress );
        MPEngXMLSerializer& Base64DataL( const TDesC8& aData );



    public: //Serializer state handling support

        /**
         * @see MPEngXmlSerializer
         *
         * @since 2.1
         */
        void PushSerializerStateL();

        /**
         * @see MPEngXmlSerializer
         *
         * @since 2.1
         */
        void CommitState();

        /**
         * @see MPEngXmlSerializer
         *
         * @since 2.1
         */
        void RollbackState();

        /**
         * @see MPEngXmlSerializer
         *
         * @since 2.1
         */
        TInt PushedStateCount();


    protected: //Private helpers


        void CheckAndCloseOpenStartTagL();
        TInt EncodeWvAddressChars( TDes16& aEncodedAddress,
                                   const TDesC16& aUnicodeAddress );

        void WriteXmlEscapedL( const TDesC8& aString,
                               TBool aEscapeQuotes );

        void WriteXmlEscapedL( TUint8 aCharacter,
                               TBool aEscapeQuotes );





    protected: //Assertion helpers

        void __AssertNoXmlEscapedCharsL( const TDesC8& aString );

        void __AssertSerializerStateL( TBool aInCorrectState,
                                       TPEngSerializerPanics aPanicReason );

        void __AssertNotEmptyL( const TDesC8& aString );


#ifdef __SERIALIZER_TAG_NAME_ASSERT
        void __AssertEndTagName( const TDesC8& aEndTagName );
#endif // __SERIALIZER_TAG_NAME_ASSERT


    private:    // Data

        //OWN: serializer state
        TPEngSerializerState iState;

        //OWN: Content writer
        TPEngWriter          iWriter;

        //OWN: Start tag count
        TInt                 iStartTagCount;

        //OWN: Pushed states
        RArray< TPEngSerializerStateData >  iStateStack;

#ifdef __SERIALIZER_TAG_NAME_ASSERT
        //OWN: Start tag assertion stack
        CPEngTagAssertionStack*             iAssertionStack;
#endif //   __SERIALIZER_TAG_NAME_ASSERT
    };

#endif      //  __CPENGXMLSERIALIZER_H__


//  End of File
