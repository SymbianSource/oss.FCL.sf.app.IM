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
* Description:  Contact list model implementation
*
*/

#ifndef __CPENGCONTACTLISTMODBASE_H__
#define __CPENGCONTACTLISTMODBASE_H__

//  INCLUDES
#include    "MPEngContactListAdvance.h"
#include    "CPEngStoreEntry.h"
#include    "MPEngContactListSettingsObserver.h"
#include    "MPEngContactListModStore.h"
#include    "TPEngContactListBaseSettings.h"
#include    <e32base.h>


// FORWARD DECLARATIONS
class CPEngContactListModItemContainer;
class CPEngContactListModChangeMonitor;
class CPEngContactListSettings;
class MPEngContactListSettingsManager;


// HELP CLASS DECLARATION

/**
 * Reference owning pointer array.
 */
template<class T>
class RReffArray : public RPointerArray<T>
    {
    public:

        /**
         * Overwritten Close() behaviour.
         * Closes access to items.
         */
        inline void Close()
            {
            TInt count( RPointerArray<T>::Count() );
            for ( TInt x( 0 ) ; x < count ; ++x )
                {
                RPointerArray<T>::operator[]( x )->Close();
                }
            RPointerArray<T>::Reset();
            }
    };




// CLASS DECLARATION
/**
 * Contact list model
 * Main implementation of the Contact List model.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListModBase ) :
        public CPEngStoreEntry,
        public MPEngContactListAdvance,
        public MPEngContactListSettingsObserver,
        public MPEngContactListModStore
    {
public: // Constructors, destructor
    /**
    * Two-phased constructor.
    */
    static CPEngContactListModBase* NewL(
        CPEngContactListSettings& aListSetting,
        MPEngStorageManager& aStorageManager,
        MPEngContactListSettingsManager& aCntLstSettingsManager );

    /**
    * Two-phased constructor.
    */
    static CPEngContactListModBase* NewLC(
        CPEngContactListSettings& aListSetting,
        MPEngStorageManager& aStorageManager,
        MPEngContactListSettingsManager& aCntLstSettingsManager );


    /**
     * Destructor
     */
    ~CPEngContactListModBase();



protected: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListModBase(
        CPEngContactListSettings& aListSettings,
        MPEngContactListSettingsManager& aCntLstSettingsManager );

    /**
     * Symbian constructor.
     */
    void ConstructL( MPEngStorageManager& aStorageManager );





public: // From MPEngContactListAdvance

    /**
     * Gets Reference to the once item of the contact lists
     * @see <MPEngContactListAdvance.h>
     */
    MPEngContactItemAdvance& ContactItemAdvance(
        TInt aIndex,
        TPEngContactListView aView = EPEngCntListLocalView );




public: // From MPEngContactList2

    /**
     * Inserts a Contact into the Contact list.
     * @see <MPEngContactList2>
     */
    TInt AddContactL( const TDesC& aContact,
                      const TDesC& aNickName = KNullDesC );

    /**
     * Removes Contact from the Contact list
     * @see <MPEngContactList2>
     */
    void RemoveContactL( const TDesC& aContact );



    /**
     * Find Contact in the contact list
     * @see <MPEngContactList2>
     */
    TInt FindContact( const TDesC& aContact,
                      TPEngContactListView aView = EPEngCntListLocalView ) const;



    /**
     * Count of the Contacts of the Contact list view
     * @see <MPEngContactList2>
     */
    TInt Count( TPEngContactListView aView = EPEngCntListLocalView ) const;


    /**
     * Contact Item of the Contact list
     * @see <MPEngContactList2>
     */
    MPEngContactItem& ContactItem(
        TInt aIndex,
        TPEngContactListView aView = EPEngCntListLocalView );

    const MPEngContactItem& ContactItem(
        TInt aIndex,
        TPEngContactListView aView = EPEngCntListLocalView ) const;


    /**
     * Contacts which were removed from the contact list
     * @see <MPEngContactList2>
     */
    const MDesCArray& RemovedContacts( ) const;


    /**
     * Contacts which were added to the contact list
     * @see <MPEngContactList2>
     */
    const MDesCArray& AddedContacts( ) const;


    /**
     * Contact list Settings
     * @see <MPEngContactList2>
     */
    MPEngContactListProperties& ListProperties();


    /**
     * Remove all contacts from the contact list
     * @see <MPEngContactList2>
     */
    void RemoveAllContactsL( );


    /**
     * Suspend contact list update notifications
     * @see <MPEngContactList2>
     */
    void SuspendUpdateNotification( );


    /**
     * Release sending of the contact list update notifications
     * @see <MPEngContactList2>
     */
    void ReleaseUpdateNotification( );


    /**
     * Roll back all changes
     * @see <MPEngContactList2>
     */
    TInt RollBackAllChangesL( );


    /**
     * Contact list locked by other client
     * @see <MPEngContactList2>
     */
    TBool UpdateInProgress( ) const;


    /**
     * Update of the contact list required
     * @see <MPEngContactList2>
     */
    TBool UpdateRequired() const;




public: // Functions from CPEngStoreEntry

    /**
     * ExternalizeL provides way to externalize data from store entry.
     * @see <MPEngStoreEntry.h>
     */
    void ExternalizeL( RWriteStream& aStream,
                       TPEngStorageType aStorageType ) const;

    /**
     * InternalizeL provides way to internalise data to store entry.
     * @see <MPEngStoreEntry.h>
     */
    void InternalizeL( RReadStream& aStream,
                       TPEngStorageType aStorageType );

    /**
     * Store Id of the contact list
     * @see <MPEngStoreEntry.h>
     */
    virtual const TDesC& StorageId() const;

    /**
     * Store size of the contact list
     * @see <MPEngStoreEntry.h>
     */
    virtual TUint32 EntrySize() const;




public: // CPEngStoreEntry / MPEngSIDChangeObserver

    /**
     * Handle change of the Storage IDs
     * @see <MPEngSIDChangeObserver.h>
     */
    void HandleSIDsChangeL( CPtrCArray& aChangedSIDs );

    /**
     * Called when SIDs notifier fails
     * @see <MPEngSIDChangeObserver.h>
     */
    void HandleSIDNotifyError( TInt aError );




public: //  MPEngContactListModStore

    /**
     * Gets reference to the store entry size
     * Size can be modified through this reference
     *
     * @see <MPEngContactListModStore.h>
     */
    TInt& StoreSizeCount( );

    /**
     * Stores entry to the store
     * @see <MPEngContactListModStore.h>
     */
    void StoreEntryL();



public: // function of the MPEngContactListSettingsObserver

    /**
     * Refresh contact list settings reference
     * @see <MPEngContactListSettingsObserver.h>
     */
    void RefreshSettingsReference( );




public: // New functions used for ListLib private use


    /**
     * Maps contact list type to storage type.
     *
     * @since 3.0
     */
    static TPEngStorageType StoreTypeForListType( TPEngContactListType aListType );


    /**
     * Reference count support
     *
     * @since 3.0
     */
    void Open();


    /**
     * Reference count support
     *
     * @since 3.0
     */
    void Close();


    /**
     * Stores contact list content to the store
     * @since 3.0
     */
    void StoreContactListL( );


    /**
     * Fill give array with contacts need to be updated.
     *
     * Depends on the function, either contacts which need to be updated
     * are added. Or contacts which needs to be deleted are added.
     *
     * Ownership of appended contact items is not returned.
     *
     * @since 3.0
     * @param array to be filled with contacts
     */
    void FillAddNickListL(
        RPointerArray<CPEngContactListModItemContainer>& aAddNickList );
    void FillRemoveNickListL(
        RPointerArray<CPEngContactListModItemContainer>& aRemoveNickList );


    /**
     * Commit changes after successful update request
     * Request can either update new or edited contacts
     * "CommitAddContactsL" or removed contacts "CommitRemoveContactsL"
     *
     *  @since 3.0
     */
    void CommitAddContactsL();
    void CommitRemoveContactsL();


    /**
     * Roll back all changes
     * Content of the local view is set same as network one
     *
     * @since 3.0
     */
    void RollbackL();


    /**
     * Look for contact among all contact views.
     *
     * @since 3.0
     * @param aContact to loook for
     * @return contact item if found, or NULL pointer
     */
    CPEngContactListModItemContainer* FindContactInAll( const TDesC& aContact );


    /**
     * Check if Contact is valid on the Network server
     *
     * @since 3.0
     * @param aContact contact to check
     * @return ETrue if contact exist on network server
     */
    TBool ContactExistsOnServer( CPEngContactListModItemContainer& aContact );


    /**
     * Adopt new set of contacts.
     * Contacts are either adopted for both views ( Local, network)
     * "AdoptNewContactsL"
     * or only for network view "AddoptNetworkViewL" which leaves
     * contact list in state needed for update
     * Passed array owns reference to the member items.
     * Once item is adopted it is removed from passed array
     *
     * @since 3.0
     * @param aContacts list of contacts to be adopted
     */
    void AdoptNewContactsL(
        RPointerArray<CPEngContactListModItemContainer>& aContacts );
    void AdoptNetworkViewL(
        RPointerArray<CPEngContactListModItemContainer>& aContacts );


    /**
     * Remove bad contact from the contact list
     *
     * @since 3.0
     * @param aContact contact Id
     */
    void RemoveBadContactL( const TDesC& aContact );



    /**
     * Reset content of the contact list
     *
     * @since 3.0
     */
    virtual void Reset();


    /**
     * Reset Local/Network view List
     *
     * @since 3.0
     */
    void ResetLocalView();
    void ResetNetworkView();


    /**
     * Leaves with KErrInUse if the contact list
     * update is progress.
     *
     * @since 3.0
     */
    void LeaveIfListUpdateInProgressL() const;


    /**
     * Get ChangeMonitor reference.
     *
     * @since 3.0
     * @return change monitor
     */
    CPEngContactListModChangeMonitor& ChangeMonitor();


    /**
     * Get Contact List Settings
     *
     * @since 3.0
     * @return contact list settings
     */
    CPEngContactListSettings& Settings( );


    /**
     * Server Ordered Contacts
     *
     * @since 3.0
     * @return List of server ordered contacts
     */
    const RPointerArray<CPEngContactListModItemContainer>& ServerOrderedContacts() const;


    /**
     * Find contact in the array
     *
     * @since 3.0
     * @param aContact contact name
     * @return index in the array, or KErrNotFound
     */
    TInt DoFindContact(
        const RPointerArray<CPEngContactListModItemContainer>& aArray,
        const TDesC& aContact,
        TInt& aIndex,
        TBool aFirstPrefered = EFalse ) const;

    TInt DoFindContact(
        const RPointerArray<CPEngContactListModItemContainer>& aArray,
        const TDesC& aContact ) const;



private: // Helpers


    /**
     * Externalize all contacts
     *
     * @since 3.0
     * @param aStream stream to externalize to
     * @param aType storage type of externalize
     */
    void ExternalizeContactsL( RWriteStream& aStream,
                               TPEngStorageType aStorageType ) const;

    /**
     * Internalize all contacts
     *
     * @since 3.0
     * @param aStream stream to store internalize from
     * @param aType storage type of internalize
     */
    void InternalizePermanentContactsL( RReadStream& aStream );


    /**
     * Internalize all contacts
     *
     * @since 3.0
     * @param aStream stream to store internalize from
     * @param aType storage type of internalize
     */
    void InternalizeCachedContactsL( RReadStream& aStream );


    /**
     * Restore server ordered array
     */
    void RestoreServerOrderedArray();


    /**
     * Reorganizes network view's server order indexes
     */
    void ReorganizeServerIndexOrder( TInt aIndex );



private:  // Data


    /// OWN: Local view of the contacts
    ///      Elements OWNed
    RReffArray<CPEngContactListModItemContainer>   iLocalView;

    /// OWN: Network view of the contacts
    ///      Elements OWNed
    RReffArray<CPEngContactListModItemContainer>   iNetworkView;


    /// OWN: Contacts ordered in the order as network server sent it
    ///      Elements references, same content as iNetworkView
    RPointerArray<CPEngContactListModItemContainer> iServerOrder;

    /// OWN: Monitor of the changes
    CPEngContactListModChangeMonitor*   iChangesMonitor;


    /// OWN: Access count of the model
    TInt                                iAccessCount;

    /// REF: Setting of the contact list
    CPEngContactListSettings*           iSettings;

    /// REF: Contact List settings manager
    MPEngContactListSettingsManager&    iCntLstSettingsManager;

    /// OWN: Storage Id of the contact list
    HBufC*                              iStorageId;
    };

#endif      //  __CPENGCONTACTLISTMODBASE_H__


//  End of File



