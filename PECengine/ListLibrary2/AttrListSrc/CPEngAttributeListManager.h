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
* Description:  Attribute list manager
*
*/

#ifndef __CPENGATTRIBUTELISTMANAGER_H__
#define __CPENGATTRIBUTELISTMANAGER_H__


//  INCLUDES
#include "CPEngStoreEntry.h"
#include "MPEngAttributeListManager.h"
#include "CPEngAttributeListItem.h"
#include <e32base.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class   MPEngPresenceAttrManager;
class   MPEngStorageManager;
class   MPEngListLibFactory;
class   CPEngSessionSlotId;


// DATA TYPES
typedef TBool ( CPEngAttributeListItem::*ItemDefaultSet )( void ) const;


// CLASS DECLARATION
/**
 * Attribute lists manager
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListManager ) :
        public CPEngStoreEntry,
        public MPEngAttributeListManager
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListManager* NewL( MPEngListLibFactory& aFactory,
    const CPEngSessionSlotId& aSessionSlot );

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListManager* NewLC( MPEngListLibFactory& aFactory,
                                             const CPEngSessionSlotId& aSessionSlot );

    /**
     * Destructor.
     */
    ~CPEngAttributeListManager();


public: // Session recognize functions

    /**
     * Session ID
     * @since 3.0
     * @return session Id
     */
    const CPEngSessionSlotId& SessionId() const;

    /**
     * Increase ref count
     *
     * @since 3.0
     */
    void Open();



public: // Functions from MPEngAttributeListManager

    /**
     * Close reference to the Class
     * @see <MPEngAttributeListManager.h>
     */
    void Close();


    /**
     * Create Empty attribute list
     * @see <MPEngAttributeListManager.h>
     */
    MPEngAttributeList2* CreateEmptyAttributeListL();


    /**
     * Get default attribute list
     * @see <MPEngAttributeListManager.h>
     */
    MPEngAttributeList2* GetDefaultAttributeListL(
        TPEngAttributeListsSet2 aAttrListsSet );


    /**
     * Set new default attribute list
     * @see <MPEngAttributeListManager.h>
     */
    void SetAsDefaultAttributeListL(
        MPEngAttributeList2& aAttrList );


    /**
     * Delete default attribute list
     * @see <MPEngAttributeListManager.h>
     */
    void DeleteDefaultAttributeListL();


    /**
     * Get attribute list attached to the contact Id
     * @see <MPEngAttributeListManager.h>
     */
    MPEngAttributeList2* GetAttributeListForUserL(
        const TDesC& aContactId,
        TPEngAttributeListsSet2 aAttrListsSet );


    /**
     * Get attribute list attached to the contact list
     * @see <MPEngAttributeListManager.h>
     */
    MPEngAttributeList2* GetAttributeListForContactListL(
        const TDesC& aContactList,
        TPEngAttributeListsSet2 aAttrListsSet );

    /**
     * Attach attribute list to the contact list
     * @see <MPEngAttributeListManager.h>
     */
    void AttachAttributeListToContactListL(
        const TDesC& aContactList,
        MPEngAttributeList2& aAttrList );


    /**
     * Attach attribute list to the contact id
     * @see <MPEngAttributeListManager.h>
     */
    void AttachAttributeListToUserL(
        const TDesC& aContactId,
        MPEngAttributeList2& aAttrList );

    /**
     * Delete attribute list from Contact list
     * @see <MPEngAttributeListManager.h>
     */
    void DeleteAttributeListFromContactListL(
        const TDesC& aContactList );

    /**
     * Delete attribute list from Contact Id
     * @see <MPEngAttributeListManager.h>
     */
    void DeleteAttributeListFromUserL(
        const TDesC& aContactId );



public: // From CPEngStoreEntry

    /**
     * ExternalizeL provides way to externalize data from store entry.
     * @see <CPEngStoreEntry.h>
     */
    void ExternalizeL( RWriteStream& aStream,
                       TPEngStorageType aStorageType ) const;

    /**
     * InternalizeL provides way to internalise data to store entry.
     * @see <CPEngStoreEntry.h>
     */
    void InternalizeL( RReadStream& aStream,
                       TPEngStorageType aStorageType );


    /**
     * This method is used to retrieve ID for storing.
     * @see <CPEngStoreEntry.h>
     */
    const TDesC& StorageId() const;


    /**
     * This method is used to ask entry size in bytes.
     * @see <CPEngStoreEntry.h>
     */
    TUint32 EntrySize() const;


    /**
     * Handle change of the Storage IDs
     * @see <CPEngStoreEntry.h>
     * @since 3.0
     */
    void HandleSIDsChangeL( CPtrCArray& aChangedSIDs );


    /**
     * Called when SIDs notifier fails
     * @see <CPEngStoreEntry.h>
     * @since 3.0
     */
    void HandleSIDNotifyError( TInt aError );



public: // New functions


    /**
     * Find attribute list
     *
     * @since 3.0
     * @param aAttributeList attribute list to look for
     * @return index in array or KErrNotFound
     */
    TInt FindAttributeList( const RArray<TUint32>& aAttributeList ) const;


    /**
     * Find current default attribure list
     *
     * @since 3.0
     * @return index in array or KErrNotFound
     */
    TInt FindCurrentDefaultAttributeList() const;


    /**
     * Find new default attribute list
     *
     * @since 3.0
     * @return index in array if found or KErrNotFound
     */
    TInt FindNewDefaultAttributeList() const;


protected:  // Functions of the base class

    /**
     * Store Manager content to the storage
     * @since 3.0
     */
    TInt StoreChangesL();


private:    // new functions

    /**
     * Compare two attribute lists
     *
     * @since 3.0
     * @param aListA attribute list A
     * @param aListB attribute list B
     * @return ETrue is same, EFalse if not
     */
    TBool CompareAttributeLists( const RArray<TUint32>& aListA,
                                 const RArray<TUint32>& aListB ) const;

    /**
     * Find ID in the Des Array
     *
     * @since 3.0
     * @param aArray array to search in
     * @param aItem item to look for
     * @return inder or KErrNotFound
     */
    TInt FindItemInArray( const CDesCArray& aArray,
                          const TDesC& aItem ) const;


    /**
     * Find Contact in the defined array of attribute list
     *
     * @since 3.0
     * @param aArray array to search in
     * @param aItem item to look for
     * @return inder or KErrNotFound
     */
    TInt FindContactInAttrLists(
        const TDesC& aContact,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArray ) const;


    /**
     * Find Default attribute list
     *
     * @since 3.0
     * @param aArray array to search in
     * @return inder or KErrNotFound
     */
    TInt FindDefaultAttrList( ItemDefaultSet aDefaultSetCall ) const;


    /**
     * Get attribute list attached to the contact
     *
     * @since 3.0
     * @param aContact contact
     * @param aAttrListsSet attribute list set
     * Rest of parameters define what internal array used for operation
     * @param aArrayCurrent enumeration of current array of contacts
     * @param aArrayNew enumeration of the new array of contacts
     * @param aToDeleteContacts array of contacts for deleting
     */
    MPEngAttributeList2* GetAttributeListForContactL(
        const TDesC& aContact,
        TPEngAttributeListsSet2 aAttrListsSet,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArrayCurrent,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew );


    /**
     * Attach attribute list to the contact
     *
     * @since 3.0
     * @param aContact contact
     * @param aList attribute list models
     * Rest of parameters define what internal array used for operation
     * @param aArrayCurrent enumeration of current array of contacts
     * @param aArrayNew enumeration of the new array of contacts
     * @param aToDeleteContacts array of contacts for deleting
     */
    void AttachAttributeListToContactL(
        const TDesC& aContact,
        MPEngAttributeList2& aList,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArrayCurrent,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew,
        CDesCArray& aToDeleteContacts );


    /**
     * Delete attribute list from the contact
     *
     * @since 3.0
     * @param aContact contact
     * Rest of parameters define what internal array used for operation
     * @param aArrayNew enumeration of the new array of contacts
     * @param aToDeleteContacts array of contacts for deleting
     */
    void DeleteAttributeListFromContactL(
        const TDesC& aContact,
        CPEngAttributeListItem::TPEngAttrListItemArrays aArrayNew,
        CDesCArray& aToDeleteContacts );

protected:  // constructors

    /**
     * Symbian constructor.
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot );

    /**
     * C++ constructor.
     */
    CPEngAttributeListManager( MPEngListLibFactory& aFactory );


protected:  // Data

    /// OWN: Access count
    TInt                                    iAccessCount;

    /// REF: Factory
    MPEngListLibFactory&                    iFactory;

    /// OWN: Session Slot Id
    CPEngSessionSlotId*                     iSessionId;

    /// OWN: Attribute list engine synchronized with server
    TBool                                   iSynchronized;

    /// OWN: Presence attribute manager
    MPEngPresenceAttrManager*               iAttributeManager;

    /// OWN: Array of the attributes exists, elements OWNED
    RPointerArray<CPEngAttributeListItem>   iAttributeLists;

    /// OWN: Array of contact IDs for which attr lists should be deleted
    CDesC16ArrayFlat                        iDeleteAttrListContactIDs;

    /// OWN: Array of contact lists for which attr lists should be deleted
    CDesC16ArrayFlat                        iDeleteAttrListContactLists;

    /// OWN: Flag if default attribute list should be deleted
    TBool                                   iDeleteDefaultAttrList;
    };

#endif // __CPENGATTRIBUTELISTMANAGER_H__

// End of File

