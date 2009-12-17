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
* Description:  Contact list properties.
*
*/

#ifndef __MPENGCONTACTLISTPROPERTIES_H
#define __MPENGCONTACTLISTPROPERTIES_H


//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngContactListPropertiesExt;
class MPEngAttributeList2;
class TPEngContactListBaseSettings;



// CONSTANTS
/********************************************************/
/* Contact list property levels.                        */
/* @since 3.0                                           */
/********************************************************/


/**
 * Presence Engine contact list native permanent properties.
 * Those properties are permanent over the sessions.
 */
const TUint KPEngCntLstPropertyNativePermanent =   0x00000010;


/**
 * Presence Engine contact list native cached properties.
 * These property values are discarded on the session close.
 */
const TUint KPEngCntLstPropertyNativeCached =      0x00000020;


/**
 * External permanent contact list properties.
 *
 * These properties are not used by the Presence Engine but
 * the clients can use this level to store external
 * settings that must be permanent over the sessions.
 */
const TUint KPEngCntLstPropertyExternalPermanent = 0x00000030;

/**
 * External cached contact list properties.
 *
 * These properties are not used by the Presence Engine but
 * the clients can use this level to store external settings
 * that needs to be cleared when the session is closed.
 */
const TUint KPEngCntLstPropertyExternalCached =    0x00000040;







/********************************************************/
/* Contact list properties.                             */
/* These properties should be defined by the client     */
/* once contact list is created. If clients do not      */
/* define these, default values are used instead.       */
/*                                                      */
/* @since 3.0                                           */
/********************************************************/


/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativePermanent
 *
 * Flag if the contact lists must exist on the network server.
 *
 * Possible property values:
 *  - 0: (Default) Contact list is lazy initializated
 *       on the Network server when needed.
 *  - 1: Contact list is created on the Network server, even
 *       there is no content for such contact list.
 */
const TUint KPEngCntLstMustExistOnServer = 0x0001001;



/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativePermanent
 *
 * Flag if contact list is master on the terminal or
 * on the network server. This rule is used when local copy
 * of the contact list is synchronized with the network server.
 *
 * Possible property values:
 *  - 0: (Default) Network version of the contact list
 *       is the master.
 *  - 1: Terminal version of the lists is master.
 */
const TUint KPEngCntLstSyncMaster =			0x0001002;



// Following Contact list properties will will be hiden and not
// used as public

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativePermanent
 *
 * Flag if the contact list needs to be subscribed or not.
 *
 * Possible property values:
 *  - 0: (Default) Contact list isn't subscribed.
 *  - 1: Contact list will is subscribed.
 */
const TUint KPEngCntLstSubscribe =			0x0001003;


/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativePermanent
 *
 * Flag if the contact list is default contact list
 *
 * Possible property values:
 *  - 0: (Default) Contact list isn't default contact list.
 *  - 1: Contact is default.
 */
const TUint KPEngCntLstDefault =			0x0001004;




// CLASS DECLARATION

/**
 * Contact list settings.
 *
 * Interface to access all properties of the contact list
 *
 * @since 3.0
 */
class MPEngContactListProperties
    {
    public: // New functions

        /**
         *  Is contact list synchronized?
         *
         *  ETrue if local contact list has been synchronized
         *  with network server. Else EFalse.
         *
         *  @since 3.0
         *  @return ETrue if contact list is synchronized.
         *  Else EFalse.
         */
        virtual TBool Synchronized() const = 0 ;


        /**
         *  Contact list local name.
         *
         *  Unique name of the contact list how it it is addressed locally.
         *
         *  This name can vary from the how contact list is represented
         *  on the server side, since in some cases contact list name
         *  on the server side also includes user name of logged client.
         *
         *  @since 3.0
         *  @return Contact list local name.
         */
        virtual const TDesC& Name() const = 0;


        /**
         *  Contact list server name.
         *
         *  Name how contact list is addressed on the network server
         *  This name is not intended to be used for local addressing
         *  of the contact list. For that client should used Name() instead.
         *
         *  @since 3.0
         *  @return Contact list server name.
         */
        virtual const TDesC& ServerName() const = 0;


        /**
         *  Contact list display name.
         *
         *  Gets the contact list display name.
         *
         *  For contact lists that are unknown to Presence Engine,
         *  there doesn't exist diplay name for the list before
         *  the list is synchronized with network server.
         *
         *  @since 3.0
         *  @param aDispName On the return points to display name.
         *  @return KErrNone If there is a display name for the list.
         *  Else KErrNotFound.
         */
        virtual TInt DisplayName( TPtrC& aDispName ) const = 0;


        /**
         *  Sets a new display name for the contact list.
         *
         *  To make the name change active, contact list has to
         *  be updated to network.
         *
         *  @since 3.0
         *  @param aDisplayName New display name for the contact list.
         */
        virtual void SetDisplayNameL( const TDesC& aDisplayName ) = 0;


        /**
         *  Is the contact list the default list?
         *
         *  @since 3.0
         *  @return ETrue if contact list is the default
         *  contact list. Else EFalse.
         */
        virtual TBool IsDefault() const = 0;


    public: //Subscription attributes


        /**
         *  Gets presence attributes used for subscription.
         *
         *  When Contact list is not subscribed or presence
         *	attributes are not defined for subscription,
         *  returned array is empty
         *
         *  @since 3.0
         *  @return Assigned attributes for subscription.
         *  NULL if no attributes assigned.
         */
        virtual const RArray<TUint32>& SubscriptionAttributes() const = 0;


        /**
         *  Sets used presence attributes for subscription.
         *
         *  Sets a new set of presence attributes to be used
         *  for subscription of the contact list. Sets also the
         *  contact list as subscribed contact list. -ActivateSubscriptionL()
         *
         *  In order to subscrive all presence attributes known to the
         *	Presence Server, array should be left empty.
         *
         *  To make new subscription active, contact list has to
         *  be updated to network with the contact list transaction
         *	manager	<CPEngContactListTransaction2.h>
         *
         *  @since 3.0
         *  @param aAttributelist The attribute list to be used
         *  in subscription.
         *  @return ETrue if subscription settings were updated in any
         *  way and contact list needs to be Published/Synchronized
         */
        virtual TBool SetSubscriptionAttributesL(
            const MPEngAttributeList2& aAttributelist ) = 0;

        virtual TBool SetSubscriptionAttributesL(
            const TArray<TUint32>& aAttributelist ) = 0;

        /**
         *  Activates subscription for the contact list
         *
         *  Contact list is set as to be subscribed, set off attributes to
         *  be used for subscription is empty if not defined previously.
         *
         *  Contact list is marked to update its subscription at next
         *  contact list update transaction
         *
         *  @since 3.0
         *  @return ETrue if subscription settings were updated in any
         *  way and contact list needs to be Published/Synchronized
         */
        virtual TBool ActivateSubscriptionL() = 0;

        /**
         *  De Activates subscription for the contact list
         *
         *  Contact list is set as NOT to be subscribed.
         *
         *  Contact list is marked to update its subscription at next
         *  contact list update transaction
         *
         *  @since 3.0
         *  @return ETrue if subscription settings were updated in any
         *  way and contact list needs to be Published/Synchronized
         */
        virtual TBool DeActivateSubscriptionL() = 0;

    public: // Base settings getters


        /**
         *  Gets contact list base settings.
         *
         *  Base settings are given when the contact list is
         *  first time created and those cannot be modified later.
         *
         *  @since 3.0
         *  @param Gets contact list base settings.
         */
        virtual const TPEngContactListBaseSettings& BaseSettings() const = 0;


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

    private: //Extensions interfaces


        /**
         *  Contact list properties extension interface.
         *
         *  @since 3.0
         *  @return Contact list properties extension.
         */
        virtual MPEngContactListPropertiesExt* Extension() = 0;
        virtual const MPEngContactListPropertiesExt* Extension() const = 0;




    protected: //Destructor

        /**
         *  Inline virtual destructor.
         *
         *  Concrete contact list property objects
         *  can't be destroyed using this interface.
         */
        virtual ~MPEngContactListProperties() {};
    };


#endif      //  __MPENGCONTACTLISTPROPERTIES_H

//  End of File

