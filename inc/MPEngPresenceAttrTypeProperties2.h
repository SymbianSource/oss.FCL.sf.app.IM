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
* Description:  Presence attribute type properties.
*
*/

#ifndef __MPENGPRESENCEATTRTYPEPROPERTIES2_H
#define __MPENGPRESENCEATTRTYPEPROPERTIES2_H

//  INCLUDES
#include <E32Std.h>


//FORWARD DECLARATIONS
class MPEngPresenceAttrTypePropertiesExt2;



// DATA DECLARATION


/**
 * Attribute data originator.
 *
 * Attribute data originator specifies which
 * presence service element is responsible of
 * filling the attribute data.
 *
 * Purely server originated attributes and
 * also local attributes are not allowed to
 * be published from client to network server.
 *
 * @since 3.0
 */
enum TPEngAttributeOriginator
    {
    /**
     * Attribute originator is unknown.
     */
    EPEngOriginatorUnknown = -1,


    /**
     * Attribute is local one and can't be used in network transactions.
     */
    EPEngLocalAttribute = 1,


    /**
     * Attribute values are filled by the network server.
     */
    EPEngServerOriginated = 2,


    /**
     * Attribute values are filled by the publishing client.
     */
    EPEngClientOriginated = 3,


    /**
     * Attribute values are filled by the client and
     * the network server.
     */
    EPEngClientServerOriginated = 4
    };





// CLASS DECLARATION
/**
 * Attribute type properties.
 *
 * Interface to access presence attribute properties.
 * Concrete presence attribute types share these properties
 * but they have different property values.
 *
 * @since 3.0
 */
class MPEngPresenceAttrTypeProperties2
    {
    public: //Attribute type specific data

        /**
         * Attribute type ID.
         *
         * Returns ID identifying attribute type.
         *
         * Attribute type ID identifies the presence data
         * contained in the attribute. Concrete attribute types
         * have their own data semantics that has to be known
         * before using the attribute model.
         *
         * @since 3.0
         * @return Attribute type identifier.
         */
        virtual TUint32 Type() const = 0;


        /**
         * Presence attribute data originator.
         *
         * Possible originator values are listed in
         * TPEngAttributeOriginator enumeration.
         *
         * @since 3.0
         * @return Originator.
         */
        virtual TPEngAttributeOriginator Originator() const = 0;



    public: //Generic properties


        /**
         * Gets attribute type property as TInt.
         *
         * @since 3.0
         * @parm aPropertyName The property name.
         * @parm aPropertyLevel The property level.
         * @param aProperty On the return contains the property value.
         * @return  Result from property get:
         *  - KErrNone if property succesfully get.
         *  - KErrNotSupported if requested property not supported.
         *  - Else one of system wide error codes.
         */
        virtual TInt GetProperty(
            TUint aPropertyName,
            TUint aPropertyLevel,
            TInt& aProperty ) const = 0;


        /**
         * Gets attribute type property as 8 bit descriptor
         *
         * @since 3.0
         * @parm aPropertyName The property name.
         * @parm aPropertyLevel The property level.
         * @param aProperty On the return contains the property value.
         * @return  Result from property get:
         *  - KErrNone if property succesfully get.
         *  - KErrNotSupported if requested property not supported.
         *  - Else one of system wide error codes.
         */
        virtual TInt GetProperty(
            TUint aPropertyName,
            TUint aPropertyLevel,
            TDes8& aProperty ) const = 0;




        /**
         * Gets attribute type property as 16 bit pointer descriptor
         *
         * @since 3.0
         * @parm aPropertyName The property name.
         * @parm aPropertyLevel The property level.
         * @param aProperty On the return contains the property value.
         * @return  Result from property get:
         *  - KErrNone if property succesfully get.
         *  - KErrNotSupported if requested property not supported.
         *  - Else one of system wide error codes.
         */
        virtual TInt GetPropertyPtr(
            TUint aPropertyName,
            TUint aPropertyLevel,
            TPtrC16& aProperty ) const = 0;




    private: //Extensions interfaces


        /**
         * Generic attribute property extension interface.
         *
         * @since 3.0
         * @return Attribute property extension.
         */
        virtual MPEngPresenceAttrTypePropertiesExt2* Extension() = 0;


        /**
         * Attribute model type specific property extension.
         *
         * @since 3.0
         * @return Attribute type specific property extension.
         */
        virtual TAny* TypeExtension( TUint32 aType ) = 0;




    protected: //Destructor

        /**
         * Inline virtual destructor.
         *
         * Concrete attribute property objects
         * can't be destroyed using this interface.
         */
        virtual ~MPEngPresenceAttrTypeProperties2() {};

    };



#endif      // __MPENGPRESENCEATTRTYPEPROPERTIES2_H


//  End of File



