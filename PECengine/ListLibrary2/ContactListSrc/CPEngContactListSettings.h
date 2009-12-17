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
* Description:  Container for contact list settings.
*
*/



#ifndef __CPENGCONTACTLISTSETTINGS_H__
#define __CPENGCONTACTLISTSETTINGS_H__

//  INCLUDES
#include    <e32base.h>
#include    "MPEngContactListProperties.h"
#include    "TPEngContactListBaseSettings.h"

// CONSTANTS

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Has contact list been synchronized
 *
 * Possible property values:
 *  - 0: (Default) not synchronized
 *  - 1: synchronized
 */
const TUint KPEngHasBeenSynchronized =          0x0000101; // 257


/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Does contact list exist on the server
 *
 * Possible property values:
 *  - 0: (Default) Does not exist on server
 *  - 1: list exists on the server
 */
const TUint KPEngListExistsOnServer =           0x0000103; //259

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Update of the contact list properties needed
 *
 * Possible property values:
 *  - 0: (Default) not updated required
 *  - 1: updated required
 */
const TUint    KPEngPropertiesUpdateNeeded =   0x0000104; //260

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Should contact list be deleted from server
 *
 * Possible property values:
 *  - 0: (Default) no delete needed
 *  - 1: Delete of the contact list needed
 */
const TUint KPEngContactListDeleteNeeded =      0x0000105; //261

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Flag if update of the subscription is needed
 *
 * Possible property values:
 *  - 0: (Default) no need of update
 *  - 1: update of the subscription needed
 */
const TUint KPEngSubcriptionUpdate =            0x0000106; //262

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyNativeCached
 *
 * Flag if contact list is new default contact list
 *
 * Possible property values:
 *  - 0: (Default) not det as new default contact list
 *  - 1: marked as to be new default contact list
 */
const TUint KPEngNewDefaultContactList =        0x0000107; //263




// FORWARD DECLARATIONS
class   RWriteStream;
class   MPEngContactListSettingsManager;
class   CPEngContactListProperty;

// CLASS DECLARATION

/**
 *  Container of one contact list item.
 *  It includes, contact id, nick name, set of optional features
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListSettings ) :
        public CBase,
        public MPEngContactListProperties

    {
public:  // Constructors and destructor

    /**
     *  Two-phased constructor.
     */
    static CPEngContactListSettings* NewLC(
        const TDesC& aContactList,
        const TPEngContactListBaseSettings& aBaseSettings,
        MPEngContactListSettingsManager& aManager );

    /**
     *  Two-phased constructor.
     */
    static CPEngContactListSettings* NewServerLC(
        const TDesC& aContactList,
        const TDesC& aServerName,
        MPEngContactListSettingsManager& aManager );

    /**
     * Two-phased constructor.
     */
    static CPEngContactListSettings* NewStreamLC(
        RReadStream& aStream,
        MPEngContactListSettingsManager& aManager );
    /**
     * Destructor.
     */
    virtual ~CPEngContactListSettings();



public: // From MPEngContactListProperties

    /**
     * Is contact list synchronized?
     * @see <MPEngContactListProperties.h>
     */
    TBool Synchronized() const;


    /**
     * Contact list local name.
     * @see <MPEngContactListProperties.h>
     */
    const TDesC& Name() const;


    /**
     * Contact list server name.
     * @see <MPEngContactListProperties.h>
     */
    const TDesC& ServerName() const;


    /**
     * Contact list display name.
     * @see <MPEngContactListProperties.h>
     */
    TInt DisplayName( TPtrC& aDispName ) const;


    /**
     * Sets a new display name for the contact list.
     * @see <MPEngContactListProperties.h>
     */
    void SetDisplayNameL( const TDesC& aDisplayName );


    /**
     * Is the contact list the default list?
     * @see <MPEngContactListProperties.h>
     */
    TBool IsDefault() const;


    /**
     * Gets presence attributes used for subscription.
     * @see <MPEngContactListProperties.h>
     */
    const RArray<TUint32>& SubscriptionAttributes() const;


    /**
     * Sets used presence attributes for subscription.
     * @see <MPEngContactListProperties.h>
     */
    TBool SetSubscriptionAttributesL(
        const MPEngAttributeList2& aAttributelist );
    TBool SetSubscriptionAttributesL(
        const TArray<TUint32>& aAttributelist );

    /**
     * Activates subscription for the contact list
     * @see <MPEngContactListProperties.h>
     */
    TBool ActivateSubscriptionL();


    /**
     * De Activates subscription for the contact list
     * @see <MPEngContactListProperties.h>
     */
    TBool DeActivateSubscriptionL();


    /**
     * Gets contact list base settings.
     * @see <MPEngContactListProperties.h>
     */
    const TPEngContactListBaseSettings& BaseSettings() const;


    /**
     *  Gets a property.
     *  @see <MPEngContactListProperties.h>
     */
    TInt GetProperty( TUint aName,
                      TUint aLevel,
                      TInt& aValue ) const;

    TInt GetProperty( TUint aName,
                      TUint aLevel,
                      TPtrC8& aValue ) const;

    TInt GetProperty( TUint aName,
                      TUint aLevel,
                      TPtrC16& aValue ) const;

    /**
     * Gets a property.
     * @see <MPEngContactListProperties.h>
     */
    TInt Property( TUint aName, TUint aLevel ) const;

    /**
     * Sets a property.
     * @see <MPEngContactListProperties.h>
     */
    void SetPropertyL( TUint aName,
                       TUint aLevel,
                       TInt aValue );

    void SetPropertyL( TUint aName,
                       TUint aLevel,
                       const TDesC8& aValue );

    void SetPropertyL( TUint aName,
                       TUint aLevel,
                       const TDesC16& aValue );

    /**
     * Delete a property
     * @see <MPEngContactListProperties.h>
     */
    void DeletePropertyL( TUint aName, TUint aLevel );


    /**
     *  Contact list properties extension interface.
     *  @see <MPEngContactListProperties.h>
     */
    inline MPEngContactListPropertiesExt* Extension()
        {
        return NULL;
        }

    inline const MPEngContactListPropertiesExt* Extension() const
        {
        return NULL;
        }



public: // New functions

    /**
     * Update contact list server name.
     * Server name is composed from the user name and domain address.
     *
     * @since 3.0
     * @param aUserName user name
     * @param aDomain domain address
     */
    void UpdateListServerNameL( const TDesC& aUserName,
                                const TDesC& aDomain );

    /**
     * Display name
     * This can be called even when contact list hasn't been
     * synchronized
     *
     * @since 3.0
     * @return display name
     */
    const TDesC& DispName() const;


    /**
     * Push Display name update.
     * Update display name, flag for property update it skipped
     * Ownership of given buffer is taken in all cases.
     *
     * @since 3.0
     * @param aDispName New display name.
     */
    void PushUpdateDisplayNameL( HBufC* aDispName );


    /**
     * Updates contact list type.
     *
     * @since 3.0
     * @param aContactListType New type for the contact list.
     */
    void UpdateListTypeL( TPEngContactListType aContactListType );


    /**
     * Externalizes contact list settings to the stream
     *
     * @since 3.0
     * @param aStream Stream to write to.
     */
    void ExternalizeL( RWriteStream& aStream,
                       TInt aStorageType ) const;

    /**
     * Internalize contact list settings from the stream
     *
     * @since 3.0
     * @param aStream stream to read from
     */
    void InternalizeL( RReadStream& aStream,
                       TInt aStorageType );

private:

    /**
     * C++ constructor.
     */
    CPEngContactListSettings(
        const TPEngContactListBaseSettings& aBaseSettings,
        MPEngContactListSettingsManager& aManager );

    /**
     * C++ constructor.
     */
    CPEngContactListSettings(
        MPEngContactListSettingsManager& aManager );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL( const TDesC& aContactList );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructServerL( const TDesC& aContactList,
                           const TDesC& aServerName );


private:    // Data

    /// REF: Store entry of contact list settings
    MPEngContactListSettingsManager&         iSettingsManager;

    /// OWN: Store Id of the contact list, same as local name of contact list
    HBufC*                                   iName;

    /// OWN: Contact list server name
    HBufC*                                   iServerName;

    /// OWN: Display name of the contact list
    HBufC*                                   iDisplayName;

    /// OWN: Contact list base behavior
    TPEngContactListBaseSettings             iBehavior;

    /// OWN: properties of the contact list
    RPointerArray<CPEngContactListProperty>  iProperties;

    /// OWN: Presence attributes for subscription
    RArray<TUint32>                          iSubscriptionAttrs;
    };

#endif      // __CPENGCONTACTLISTSETTINGS_H__

// End of File

