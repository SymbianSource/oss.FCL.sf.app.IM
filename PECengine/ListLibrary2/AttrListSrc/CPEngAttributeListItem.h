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
* Description:  Container of one attribute list item.
*
*/

#ifndef __CPENGATTRIBUTELISTITEM_H__
#define __CPENGATTRIBUTELISTITEM_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class	RWriteStream;
class	RReadStream;


// CLASS DECLARATION

/**
 * Container of the one attribute list item
 *
 * It includes:
 * - lists of presence attributes.
 * - list of contacts (contact lists, contact ids),
 * - list of new and to delete contacts
 * - flag of default lists, for current state and future
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */
NONSHARABLE_CLASS ( CPEngAttributeListItem ) : public CBase
    {
public:     // Enumerations

    /**
     * Enumeration to access contact of the attribute list.
     * Need to be in order, first new, then currect of each kind
     *
     * @since 3.0
     */
    enum TPEngAttrListItemArrays
        {
        ENewContactLists			= 0,
        ECurrentContactLists		= 1,
        ENewContactIDs				= 2,
        ECurrentContactIDs			= 3
        };

public:

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListItem* NewL( RReadStream& aStream );
    static CPEngAttributeListItem* NewLC( RReadStream& aStream );

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListItem* NewL(
        const RArray<TUint32>& aAttributes );
    static CPEngAttributeListItem* NewLC(
        const RArray<TUint32>& aAttributes );

    /**
     * Two-phased copy constructor.
     */
    static CPEngAttributeListItem* NewL(
        const CPEngAttributeListItem& aAttributeList );

    /**
     * Destructor.
     */
    ~CPEngAttributeListItem();



public: // Functions of base class

    /**
     * Add contact to the defined Contacts array
     *
     * @since 3.0
     * @param aContact contact to be added to the new contact lists
     * @param aArray array to be used for the operation
     * @return KErrNone of OK, otherwise system wide error code
     */
    TInt AddContactToListL( const TDesC& aContact,
                            TPEngAttrListItemArrays aArray );


    /**
     * Delete contact from contact array
     *
     * @since 3.0
     * @param aContact contact to be deleted from the new contact lists
     * @param aArray array to be used for the operation
     * @return KErrNone of OK, otherwise system wide error code
     */
    TInt DeleteContactFromList( const TDesC& aContact,
                                TPEngAttrListItemArrays aArray );


    /**
     * Return array of contact based on the passed enumeration
     *
     * @since 3.0
     * @param aArray array to get
     * @return des array of contacts
     */
    const CDesCArray& ArrayOfContacts(
        TPEngAttrListItemArrays aArray  ) const;


    /**
     * Return array of Presence attributes of the attribute list
     *
     * @since 3.0
     * @return presence attributes of the attribute list
     */
    const RArray<TUint32>& PresenceAttributes() const;


    /**
     * Update set of presence attribute of the attribute list
     *
     * @since 3.0
     * @param aNewPresenceAttributes new presence attributes
     */
    void UpdatePresenceAttributesL(
        const RArray<TUint32>& aNewPresenceAttributes );


    /**
     * Commit contact list in the attribute list
     *
     * @since 3.0
     * @param aContact contact list to be commited
     */
    void CommitContactListL( const TDesC& aContact );


    /**
     * Commit contact id in the attribute list
     *
     * @since 3.0
     * @param aContact contact ID to be commited
     */
    void CommitContactIdL( const TDesC& aContact );


    /**
     * Return if attribute list is currently default
     *
     * @since 3.0
     * @return ETrue if attribute list is set as current default
     */
    TBool CurrentlyDefault() const;


    /**
     * Return if attribute list is new default
     *
     * @since 3.0
     * @return ETrue if attribute list is set as new default
     */
    TBool NewDefault() const;


    /**
     * Set new default value to the attribute list
     *
     * @since 3.0
     * @param aNewDefault new default value
     * @return contacts count
     */
    TInt SetNewDefault( TBool aNewDefault );


    /**
     * Commit default attribute list settings
     *
     * @since 3.0
     */
    void CommitDefault();


    /**
     * Returns sync state of the attribute list
     *
     * @since 3.0
     * @return Sync state
     */
    TBool Synchronized() const;


    /**
     * Returns sync state of the attribute list
     *
     * @since 3.0
     * @param aSync new sync state
     */
    void SetSynchronization( TBool aSync );


    /**
     * Externalize attribute list to the stream
     *
     * @since 3.0
     * @param aStream stream to externalize to
     */
    void ExternalizeL( RWriteStream& aStream );


    /**
     * Internalize attribute list from the stream
     *
     * @since 3.0
     * @param stream to internalize from
     */
    void InternalizeL( RReadStream& aStream );


    /**
     * Get size of the entry of the attribute list
     *
     * @since 3.0
     * @return size of the attribute list entry for storing
     */
    TInt SizeOfEntry() const;


    /**
     * Get count of all contacts of the attributelist
     *
     * @since 3.0
     * @return contacts count
     */
    TInt ContactsCount() const;



private:	// Function of the base class

    /**
     * Get desired array
     *
     * @since 3.0
     * @param aArray enumeration of array
     * @return reference to the array
     */
    CDesCArray& Array( TPEngAttrListItemArrays aArray );


    /**
     * Commit contact id in the attribute list
     *
     * @since 3.0
     * @param aCurrentArray array of current IDs
     * @param aNewArray array of new IDs
     * @param aContact contact ID to be commited
     */
    void CommitContactL( CDesCArray& aCurrentArray,
                         CDesCArray& aNewArray,
                         const TDesC& aContact );


    /**
     * Delete item from the Des Array
     *
     * @since 3.0
     * @param aArray array to be deleted from
     * @param aItem item to be deleted from array
     * @return KErrNone if it went fine
     */
    TInt DeleteItemFromArray( CDesCArray& aArray,
                              const TDesC& aItem );


    /**
     * Copy des array
     *
     * @since 3.0
     * @param aArrayTarget targer array
     * @param aArraySource source array
     */
    static void CopyDesArrayL( CDesCArray& aArrayTarget,
                               const CDesCArray& aArraySource );

    /**
     * Copy TInt array
     *
     * @since 3.0
     * @param aArrayTarget targer array
     * @param aArraySource source array
     */
    static void CopyTIntArrayL( RArray<TUint32>& aArrayTarget,
                                const RArray<TUint32>& aArraySource );


    /**
     * Externalize des array to the stream
     *
     * @since 3.0
     * @param aArray array to be externalized
     * @param aStream stream to externalize to
     */
    static void ExternalizeDesArrayToStreamL( CDesCArray& aArray,
                                              RWriteStream& aStream );

    /**
     * Internalize des array from stream
     *
     * @since 3.0
     * @param aArray array to be internlized
     * @param aStream stream to internalized from
     */
    static TInt InternalizeDesArrayFromStreamL( CDesCArray& aArray,
                                                RReadStream& aStream );


private:	// constructors

    /**
     * Symbian constructor.
     */
    void ConstructL( const RArray<TUint32>& aAttributes );

    /**
     * Symbian constructor.
     */
    void ConstructL(
        const CPEngAttributeListItem& aAttributeList );

    /**
     * C++ constructor.
     */
    CPEngAttributeListItem();


private:    // Data

    /// OWN: Attribute list is currently default
    TBool			iCurrentlyDefault;

    /// OWN: Attribute list is to be set as default
    TBool           iNewDefault;

    /// OWN: Attribute list has been already synchronized
    TBool          iSynchronized;

    /// OWN: List of attributes of the Attribute list
    RArray<TUint32>    iListOfAttributes;

    /// OWN: List of contacts lists attached to attribute list
    CDesCArraySeg  iListOfContactLists;

    /// OWN: List of new contacts lists to be attached to attribute list
    CDesCArrayFlat iListOfNewContactLists;

    /// OWN: List of contacts attached to attribute list
    CDesCArraySeg  iListOfContactIDs;

    /// OWN: List of new contacts to be attached to attribute list
    CDesCArrayFlat iListOfNewContactIDs;

    /// OWN: Store size
    TInt           iSizeOfEntry;

    };

#endif //   __CPENGATTRIBUTELISTITEM_H__

// End of File

