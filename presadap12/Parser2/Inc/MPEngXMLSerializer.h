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
* Description:  interface for Presence XML Serializer.
*
*/

#ifndef __MPENGXMLSERIALIZER_H
#define __MPENGXMLSERIALIZER_H

//  INCLUDES
#include <E32Std.h>


//CONSTANTS

/**
* Serializer panic reasons.
* @since 2.1
*/
enum TPEngSerializerPanics
    {
    EPEngSrlzr_AttributeNotAllowed = 0,
    EPEngSrlzr_EndTagNotAllowed = 1,
    EPEngSrlzr_EmptyInputString = 2,
    EPEngSrlzr_EndTagNameMismatch = 3,
    EPEngSrlzr_StateStackUnderflow = 4,
    EPEngSrlzr_XmlMarkupCharNotAllowed = 5,
    };

/**
* Serializer panic category.
* @since 2.1
*/
_LIT( KXmlSerializer, "XMLSerializ" );




//FORWARD DECLARATION
class MPEngXMLSerializer;


// FUNCTION PROTOTYPES

/**
 * Factory method to create XML Serializer.
 *
 * @since 3.0
 */
IMPORT_C MPEngXMLSerializer* CreateXmlSerializerL( TDes8& aBuffer );
IMPORT_C MPEngXMLSerializer* CreateXmlSerializerLC( TDes8& aBuffer );





//  CLASS DEFINITION
/**
* This class defines the interface for Presence XML Serializer.
* XML serializer is used for generating XML fragments.
*
* It offers features to write XML tags, XML attributes and actual
* XML fragment content.
*
* @since 2.1
*/
class MPEngXMLSerializer
    {
    public: //Element handling methods

        /**
         * Writes a start tag.
         *
         * Writes a start tag with the given name.
         * If aName is zero length descriptor,
         * panics in debug builds with EEmptyInputString
         * and release builds leaves with KErrArgument.
         *
         * @since 2.1
         * @param aName Tags name.
         * @return Self.
         */
        virtual MPEngXMLSerializer& StartTagL( const TDesC8& aName ) = 0;


        /**
         * Writes an attribute.
         *
         * Attributes must be written directly after starting
         * the tag with StartTagL() method. It is possible write
         * several attributes by issuing multiple calls to AttributeL().
         * If attribute isn't written directly after starting the tag,
         * panics in debug builds with EPEngSrlzr_AttributeNotAllowed and
         * leaves on release builds with KErrNotSupported.
         *
         * If aName is empty descriptor, panics in debug
         * builds with EPEngSrlzr_EmptyInputString and release
         * builds leaves with KErrArgument. However, aValue can
         * be empty descriptor.
         *
         * @since 2.1
         * @param aName The attribute name.
         * @param aValue The attribute value.
         * @return Self.
         */
        virtual MPEngXMLSerializer& AttributeL( const TDesC8& aName,
                                                const TDesC8& aValue ) = 0;


        /**
         * Writes end tag.
         *
         * Writes end tag with the given name.
         * (Closes thus lastly written start tag.)
         *
         * If there isn't any start tag to close, panics in debug
         * builds with EPEngSrlzr_EndTagNotAllowed and release builds
         * leaves with KErrNotSupported.
         *
         * If aName is zero length descriptor,
         * panics in debug builds with EPEngSrlzr_EmptyInputString
         * and release builds leaves with KErrArgument.
         *
         * If passed end tag name aName doesn't match corresponding
         * start tag name, panics in debug builds with
         * EPEngSrlzr_EndTagNameMismatch.
         * For performance reasons this check isn't done on
         * release builds ==> possible end tag name mismatches
         * are ignored in release builds.
         *
         * @since 2.1
         * @param aName Tag name.
         * @return Self.
         */
        virtual MPEngXMLSerializer& EndTagL( const TDesC8& aName ) = 0;




    public: //Content writing methods


        /**
         * Writes raw protocol value without _ANY_ encoding.
         *
         * Checks the written value doesn't contain XML markup characters.
         * If it contains, handles the error with panic:
         * EPEngSrlzr_XmlMarkupCharNotAllowed.
         *
         * @since 3.0
         * @param aValue Raw protocol value to write.
         * @return Self.
         */
        virtual MPEngXMLSerializer& RawValueL( const TDesC8& aValue ) = 0;


        /**
         * Writes 8 bit text.
         *
         * Performed steps:
         * 1. Escapes XML entities.
         *
         * @since 3.0
         * @param aText Text to write.
         * @return Self.
         */
        virtual MPEngXMLSerializer& NarrowTextL( const TDesC8& aText ) = 0;


        /**
         * Writes Unicode text.
         *
         * Performed steps:
         * 1. Converts text from Unicode to Utf8
         * 2. Escapes XML entities
         *
         * @since 3.0
         * @param aText Text to write.
         * @return Self.
         */
        virtual MPEngXMLSerializer& UnicodeTextL( const TDesC16& aText ) = 0;


        /**
         * Writes WV Address.
         *
         * Performed steps:
         * 1. Escapes WV Address characters
         * 2. Converts text from Unicode to Utf8
         * 3. Escape XML entities
         *
         * @since 3.0
         * @param aAddress Address to write.
         * @return Self.
         */
        virtual MPEngXMLSerializer& WvAddressL( const TDesC16& aAddress ) = 0;


        /**
         * Writes data in BASE64 format.
         *
         * Performed steps:
         * 1. Coverts data to BASE64 format
         *
         * @since 3.0
         * @param aData The data to write.
         * @return Self.
         */
        virtual MPEngXMLSerializer& Base64DataL( const TDesC8& aData ) = 0;






    public: //Serializer state handling methods

        /**
         * Stores serializer current state.
         *
         * Stores serializer current state to its internal
         * state stack. State consists from previously generated
         * XML fragment and other serializer internal data.
         *
         * States are stored to stack like structure and later
         * serializer state can be restored back to pushed state
         * or pushed state can be committed.
         *
         * On serializer destruction all previously pushed
         * states are committed automaticly.
         *
         * @since 2.1
         */
        virtual void PushSerializerStateL() = 0;

        /**
         * Commits topmost state in the state stack.
         *
         * Commits topmost state in the serializer
         * state stack. If there isn't any state to
         * commit, panics in debug builds with
         * EPEngSrlzr_StateStackUnderflow. In
         * release builds underflow is ignored.
         *
         * @since 2.1
         */
        virtual void CommitState() = 0;

        /**
         * Rollbacks the serializer state to topmost
         * state in the state stack.
         *
         * Rollbacks the serializer state to topmost
         * state in the state stack. If there isn't
         * any state to commit, panics in debug builds
         * with EPEngSrlzr_StateStackUnderflow. In
         * release builds underflow is ignored.
         *
         * @since 2.1
         */
        virtual void RollbackState() = 0;

        /**
         * Gets the count of pushed states.
         *
         * @since 2.1
         * @return Count of pushed states.
         */
        virtual TInt PushedStateCount() = 0;


    public: //Destructor & Cleanup support

        /**
         *  Release method.
         *  When called, deletes object and
         *  frees all its resources.
         *
         *  Concrete serializer object can be thus pushed
         *  on the CleanupStack using CleanupClosePushL().
         */
        virtual void Close() = 0;


        /**
         * Inline virtual destructor.
         * Concrete instancies can be deleted
         * trough this interface.
         */
        virtual ~MPEngXMLSerializer() {}

    };

#endif      //__MPENGXMLSERIALIZER_H


//  End of File






















