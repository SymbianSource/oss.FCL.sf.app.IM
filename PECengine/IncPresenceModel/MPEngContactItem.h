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
* Description:  Contact item interface.
*
*/

#ifndef __MPENGCONTACTITEM_H
#define __MPENGCONTACTITEM_H


//  INCLUDES
#include <E32Std.h>



// FORWARD DECLARATIONS
class MPEngContactItemExt;

// CONSTANTS
/********************************************************/
/* Contact Item property levels.                        */
/* @since 3.0                                           */
/********************************************************/


/**
 * Presence Engine contact Item native permanent properties.
 * Those properties are permanent over the sessions.
 */
const TUint KPEngCntItemPropertyNativePermanent =   0x00000010;


/**
 * Presence Engine contact Item native cached properties.
 * These property values are discarded on the session close.
 */
const TUint KPEngCntItemPropertyNativeCached =      0x00000020;


/**
 * External permanent contact Item properties.
 *
 * These properties are not used by the Presence Engine but
 * the clients can use this level to store external
 * settings that must be permanent over the sessions.
 */
const TUint KPEngCntItemPropertyExternalPermanent = 0x00000030;

/**
 * External cached contact Item properties.
 *
 * These properties are not used by the Presence Engine but
 * the clients can use this level to store external settings
 * that needs to be cleared when the session is closed.
 */
const TUint KPEngCntItemPropertyExternalCached =    0x00000040;





// CLASS DECLARATION

/**
 * Interface for one contact item in the contact list.
 *
 * @since 3.0
 */
class MPEngContactItem
    {
    public: // Contact item values

        /**
         * Contact Id of the contact Item
         *
         * @since 3.0
         * @return Contact Id of the contact item.
         */
        virtual const TDesC& Id() const = 0;


        /**
         * Nick name of the contact Item
         *
         * If the contact doesn't have nick defined,
         * returns KNullDesC.
         *
         * @since 3.0
         * @return Nick name of the contact item.
         */
        virtual const TDesC& NickName() const = 0;


        /**
         * Update nick name of the contact item
         *
         * New nick name is assigned to the contact item. Old nick name
         * will be replaced once contact list is updated to the server.
         *
         * @since 3.0
         * @param aNickName New nick name.
         */
        virtual void UpdateNickNameL( const TDesC& aNickName ) = 0;


    public: // Property getters

        /**
         *  Gets a property.
         *
         *  Gets value of property.
         *  Overloaded version for different data types.
         *
         *  @since 3.0
         *  @param  aName Name of the property.
         *  @param  aLevel Level of the property to look for.
         *  @param  aValue Where value is be stored.
         *  @return KErrNone if property was found. Else KErrNotFound.
         */
        virtual TInt GetProperty( TUint aName,
                                  TUint aLevel,
                                  TInt& aValue ) const = 0;

        virtual TInt GetProperty( TUint aName,
                                  TUint aLevel,
                                  TPtrC8& aValue ) const = 0;

        virtual TInt GetProperty( TUint aName,
                                  TUint aLevel,
                                  TPtrC16& aValue ) const = 0;

        /**
         *  Gets a property.
         *
         *  @since 3.0
         *  @param  aName Name of the property.
         *  @param  aLevel Level of the property to look for.
         *  @return if property is found, its value, if not found
         *			zero is returned as default value
         */
        virtual TInt Property( TUint aName,
                               TUint aLevel ) const = 0;

    public: // Property setters


        /**
         *  Sets a property.
         *
         *  Sets value of property.
         *  Overloaded version for different data types.
         *
         *  @since 3.0
         *  @param  aName Name of the property
         *              Predefined Property names are specified above
         *				Client can also use own property names
         *  @param  aLevel Level of the property to set.
         *				Predefined levels are specified above.
         *  @param  aValue Value to be set.
         */
        virtual void SetPropertyL( TUint aName,
                                   TUint aLevel,
                                   TInt aValue ) = 0;

        virtual void SetPropertyL( TUint aName,
                                   TUint aLevel,
                                   const TDesC8& aValue ) = 0;

        virtual void SetPropertyL( TUint aName,
                                   TUint aLevel,
                                   const TDesC16& aValue ) = 0;


        /**
         *  Delete a property
         *
         *	Deletes property out of the contact list preferences
         *  Property defined by the PEC engine cannot be deleted
         *	if defined property is not found, function will do nothing
         *
         *  @since 3.0
         *  @param aName name of the property to delete
         *	@param aLevel level of the property to delete
         */
        virtual void DeletePropertyL( TUint aName, TUint aLevel ) = 0;


    public: // Those functions won't any longer supported
        // Opaque value extensions


        /**
         * Gets opaque values.
         */
        virtual TInt GetPermanentValue( const TDesC& aKey,
                                        TPtrC8& aValue ) const = 0;
        virtual TInt GetCachedValue( const TDesC& aKey,
                                     TPtrC8& aValue ) const = 0;

        /**
         * Sets opaque values.
         */
        virtual void SetPermanentValueL( const TDesC& aKey,
                                         const TDesC8& aValue ) = 0;
        virtual void SetCachedValueL( const TDesC& aKey,
                                      const TDesC8& aValue ) = 0;

        /**
         * Delete opaque values.
         */
        virtual void DeletePermanentValueL( const TDesC& aKey ) = 0;
        virtual void DeleteCachedValueL( const TDesC& aKey ) = 0;

    private: //Extensions interfaces


        /**
         * Contact item extension interface.
         *
         * @since 3.0
         * @return Contact item extension.
         */
        virtual MPEngContactItemExt* Extension() = 0;
        virtual const MPEngContactItemExt* Extension() const = 0;



    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibits deletion
         * through the interface.
         */
        virtual ~MPEngContactItem() {};

    };
#endif      //  __MPENGCONTACTITEM_H

//  End of File

