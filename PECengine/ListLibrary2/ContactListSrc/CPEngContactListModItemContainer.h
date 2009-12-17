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
* Description:  Container of the one contact list item.
*
*/

#ifndef __CPENGCONTACTLISTMODITEMCONTAINER_H__
#define __CPENGCONTACTLISTMODITEMCONTAINER_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngContactItemAdvance.h"


// FORWARD DECLARATIONS
class   RWriteStream;
class   RWriteStream;
class   MPEngContactListModStore;
class   CPEngContactListProperty;



// CLASS DECLARATION

/**
 * Container of the one contact list item.
 * Implements management of one contact list item,
 * like update of the nick name and storing of the
 * cached and permanent data properties of the contact item.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListModItemContainer ) :
        public CBase,
        public MPEngContactItemAdvance
    {
public:  // Enumerations

    /**
     * Reference access flags.
     */
    enum TAccessFlags
        {
        ELocalRef     = 0x00001,
        ENetworkRef   = 0x00010
        };


public:  // Constructors and destructor

    /**
     * Two-phased constructor, constructs class with
     * passed contact id and nickname
     */
    static CPEngContactListModItemContainer* NewLC(
        MPEngContactListModStore& aStoreEntry,
        const TDesC& aContactId,
        const TDesC* aNickName );

    /**
     * Two-phased constructor, constructs class with passed contact id,
     * nickname is set as need to be updated
     */
    static CPEngContactListModItemContainer* NewNewNickLC(
        MPEngContactListModStore& aStoreEntry,
        const TDesC& aContactId,
        const TDesC& aNewNickName );

    /**
     * Two-phased constructor, constructs class from the Stream
     */
    static CPEngContactListModItemContainer* NewLC(
        MPEngContactListModStore& aStoreEntry,
        RReadStream& aStream );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListModItemContainer();



private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListModItemContainer( MPEngContactListModStore& aStoreEntry );

    /**
     * Constructs container with WV ID and Nickname
     */
    void ConstructL( const TDesC& aContactId,
                     const TDesC* aNickName );

    /**
     * Construct container with WV ID,
     * Nickname set as need to be updated
     */
    void ConstructNewNickL( const TDesC& aContactId,
                            const TDesC& aNewNickName );




public: // Functions from  MPEngContactItem

    /**
     * Contact Id of the contact Item
     * @see <MPEngContactItem.h>
     */
    const TDesC& Id() const;


    /**
     * Nick name of the contact Item
     * @see <MPEngContactItem.h>
     */
    const TDesC& NickName() const;


    /**
     * Update nick name of the contact item
     * @see <MPEngContactItem.h>
     */
    void UpdateNickNameL( const TDesC& aNickName );


    /**
     * Gets a property.
     * @see <MPEngContactItem.h>
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
     * @see <MPEngContactItem.h>
     */
    TInt Property( TUint aName,
                   TUint aLevel ) const;


    /**
     * Sets a property.
     * @see <MPEngContactItem.h>
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
     * @see <MPEngContactItem.h>
     */
    void DeletePropertyL( TUint aName, TUint aLevel );
    TInt GetPermanentValue( const TDesC& aKey, TPtrC8& aValue ) const;
    TInt GetCachedValue( const TDesC& aKey, TPtrC8& aValue ) const;
    void SetPermanentValueL( const TDesC& aKey, const TDesC8& aValue );
    void SetCachedValueL( const TDesC& aKey, const TDesC8& aValue );
    void DeletePermanentValueL( const TDesC& aKey );
    void DeleteCachedValueL( const TDesC& aKey );

    /**
     * Extensions for the future
     * @see <MPEngContactItem.h>
     */
    MPEngContactItemExt* Extension() {
        return NULL;
        };
    const MPEngContactItemExt* Extension() const {
        return NULL;
        };



public: // New functions

    /**
     * Compare function.
     *
     * Two instances are compared based on the WV Id.
     *
     * @since 3.0
     * @param aFirst fist class for comparison
     * @param aSecond second class for comparison
     * @return -1 if fist is smaller,
     *         +1 is second is smaller,
     *         0 if they are same
     */
    static TInt Compare( const CPEngContactListModItemContainer& aFirst,
                         const CPEngContactListModItemContainer& aSecond );


    /**
     * Opens reference count.
     *
     * @since 3.0
     * @param aRefType 0 zero is indefined for default open
     *        otherwise one of the TPEngAccessFlags enumeration
     */
    void Open( TInt aRefType =  0 );


    /**
     * Closes reference count access.
     *
     * @since 3.0
     * @param aRefType 0 zero is indefined for default open
     *        otherwise one of the TPEngAccessFlags enumeration
     * @return pointer to called intance.
     *         If it is about to be deleted NULL is returned
     */
    CPEngContactListModItemContainer* CloseRef( TInt aRefType =  0 );


    /**
     * Checks reference counted access.
     *
     * @since 3.0
     * @param aRefType 0 zero is indefined for default open
     *        otherwise one of the TPEngAccessFlags enumeration
     * @return ETrue if reference is active
     */
    TBool RefActive( TInt aRefType =  0 );


    /**
     * Closes access.
     *
     * @since 3.0
     */
    void Close();


    /**
     * Externalizes the entry data to stream.
     *
     * @since 3.0
     * @param aStream write stream
     * @param aType Data to be externalized as defined in
     *        TPEngStorageType enumeration.
     */
    void ExternalizeL( RWriteStream& aStream, TInt aType ) const ;


    /**
     * Internalize from the file cached information
     * @since 3.0
     * @param aStream read stream
     * @param aType Data to be internalized as defined in
     *        TPEngStorageType enumeration.
     */
    void InternalizeL( RReadStream& aStream, TInt aType );


    /**
     * Update contact Id
     *
     * @since 3.0
     * @param aId id as it is represented by the network server
     */
    void UpdateContactId( HBufC* aId );


    /**
     * Get new Nickname
     *
     * @since 3.0
     * @return new nick name buffer pointer(can be NULL)
     */
    const HBufC* NewNick() const;


    /**
     * Get Current nick name
     *
     * @since 3.0
     * @return current nick name buffer pointer(can be NULL)
     */
    const HBufC* CurrentNick() const;


    /**
     * Get Current nick name
     *
     * @since 3.0
     * @param aNickName current nick name
     */
    void SetCurrentNickname( HBufC* aNickName );


    /**
     * Adopts the current nick name as new.
     *
     * @since 3.0
     * @return current nick name buffer pointer(can be NULL)
     */
    void AdoptCurrentNickAsNew();


    /**
     * Roll back nick name.
     * New nick name is deleted and set as NULL
     *
     * @since 3.0
     */
    void RollBackNickname();


    /**
     * Commit nickname
     * New Nick name is set as current
     *
     * @since 3.0
     */
    void CommitNickName();



    /**
     * Setter and getter for contact server index.
     * @since 3.1
     */
    TInt ServerIndex() const;
    void SetServerIndex(  TInt aIndex );



    /**
     * Is this contact a fresh one.
     *
     * @since 3.0
     * @return ETrue if it is a fresh one.
     *         Else EFalse
     */
    TBool IsFreshContact();


    /**
     * Sets the contact as fresh one or not.
     *
     * @since 3.0
     * @param ETrue if the contact is a fresh one.
     *        Else EFalse
     */
    void SetFreshContact( TBool aFresh );



private:    // Data

    /// REF: Store entry of the whole model
    MPEngContactListModStore&                iStoreEntry;

    /// OWN: Access count
    TInt                                     iAccessCount;

    /// OWN: Reference definitions
    TInt                                     iRefDefinitions;

    /// OWN: Server index for server order
    TInt									 iServerIndex;

    /// REF: Item size
    TInt&                                    iSize;

    /// OWN: Contact Id
    HBufC*                                   iContactId;

    /// OWN: Nickname of the contact
    HBufC*                                   iNickName;

    /// OWN: New Nick name
    HBufC*                                   iNewNickName;

    /// OWN: The Contact fresh status
    TBool iFresh;

    /// OWN: Properties of the contact Item
    RPointerArray<CPEngContactListProperty>  iProperties;
    };

#endif      // __CPENGCONTACTLISTMODITEMCONTAINER_H__

// End of File

