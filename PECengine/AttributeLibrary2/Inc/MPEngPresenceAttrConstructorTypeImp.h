/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute type specific constructor interface.
*
*/

#ifndef __MPENGPRESENCEATTRCONSTRUCTORTYPEIMP_H
#define __MPENGPRESENCEATTRCONSTRUCTORTYPEIMP_H


//  INCLUDES
#include <E32Base.h>



// FORWARD DECLARATIONS
class MPEngPresenceAttrModelTypeImp;



/**
 * Attribute type specific constructor interface.
 *
 * @since 3.0
 */
class MPEngPresenceAttrConstructorTypeImp
    {
    public:  //Destructor

        /**
         * Destructor.
         *
         * @since 3.0
         */
        virtual ~MPEngPresenceAttrConstructorTypeImp() {};


    public: // Pure virtual functions


        /**
         * Instantiates new attribute model instance.
         *
         * @since 3.0
         * @param aUserOwnAttribute ETrue if the new instance is user own
         * attribute. Else EFalse and attribute instance presents cached
         * attribute.
         */
        virtual MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC(
            TBool aUserOwnAttribute ) const = 0;


        /**
         * Gets a attribute type specific property.
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
        virtual TInt GetProperty( TUint aPropertyName,
                                  TUint aPropertyLevel,
                                  TInt& aProperty ) const = 0;

        /**
         * Gets a attribute type specific property.
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
        virtual TInt GetProperty( TUint aPropertyName,
                                  TUint aPropertyLevel,
                                  TDes8& aProperty ) const = 0;

        /**
         * Gets a attribute type specific property.
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
        virtual TInt GetPropertyPtr( TUint aPropertyName,
                                     TUint aPropertyLevel,
                                     TPtrC16& aProperty ) const = 0;


        /**
         * Gets extensions.
         *
         * @since 3.0
         * @param aType Extension type.
         * @return Pointer to extension.
         */
        virtual TAny* TypeExtension( TUint32 aType ) = 0;



        /**
         * Gets attribute WV CSP name and namespace.
         *
         * @since 3.0
         * @param aAttributeName Gets attribute name.
         * @param aAttributeNameSpace Gets attribute name space.
         */
        virtual TInt GetCurrentCspName( TPtrC8& aAttributeName,
                                        TPtrC8& aAttributeNameSpace ) const = 0;


    };

#endif      // __MPENGPRESENCEATTRCONSTRUCTORTYPEIMP_H

//  End of File

