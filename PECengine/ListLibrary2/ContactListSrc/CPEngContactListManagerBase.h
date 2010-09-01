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
* Description:  Contact lists manager base
*
*/


#ifndef __CPENGCONTACTLISTMANAGERBASE_H__
#define __CPENGCONTACTLISTMANAGERBASE_H__


//  INCLUDES
#include "CPEngStoreEntry.h"
#include "MPEngContactListSettingsManager.h"
#include <e32base.h>
#include <badesca.h>


// CONSTANTS
_LIT( KPEngContactListManagerSId, "contactlistmanager" );


// FORWARD DECLARATIONS
class   MPEngStorageManager;
class   CPEngContactListSettings;
class   CPEngContactListModBase;
class   MPEngListLibFactory;
class   CPEngSessionSlotId;


// CLASS DECLARATION

/**
 * List manager base class
 * Implements base storing features common to list manager
 * related classes
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListManagerBase ):
        public CPEngStoreEntry,
        public MPEngContactListSettingsManager
    {
public:  // Two-phased constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CPEngContactListManagerBase();


protected: // Protected constructors

    /**
     * C++ constructor.
     */
    CPEngContactListManagerBase( MPEngListLibFactory& aFactory );

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot );



public: // New public functions

    /**
     * Session Id
     * @since 3.0
     * @return session Id
     */
    const CPEngSessionSlotId& SessionId( ) const;

    /**
     * Increase ref count
     *
     * @since 3.0
     */
    void Open( );



public: // From MPEngContactListSettingsManager


    /**
     * Get Contact list settings
     * @see <MPEngContactListSettingsManager.h>
     */
    CPEngContactListSettings* ContactListSettingsOrNull(
        const TDesC& aContactList );


    /**
     * Add contact list settings observer
     * @see <MPEngContactListSettingsManager.h>
     */
    void AddContactListSettingsObserverL(
        const MPEngContactListSettingsObserver* aObserver );


    /**
     * Remove contact list settings observer
     * @see <MPEngContactListSettingsManager.h>
     */
    void RemoveContactListSettingsObserver(
        const MPEngContactListSettingsObserver* aObserver );


    /**
     * Remove model from the array of loaded models
     * @see <MPEngContactListSettingsManager.h>
     */
    void RemoveModel( CPEngContactListModBase* aModel );


    /**
     * Get domain of the logged user,
     * @see <MPEngContactListSettingsManager.h>
     */
    const TDesC& UserDomain() const;


    /**
     * Store contact list settings
     * @see <MPEngContactListSettingsManager.h>
     */
    void StoreSettingsL();


    /**
     * Size of the storage for Contact list settings
     * @see <MPEngContactListSettingsManager.h>
     */
    TInt& StoreSize( );



public: // Functions from CPEngStoreEntry

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



public: //Functions from CPEngStoreEntry / MPEngSIDChangeObserver

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



protected:  // Contact lists management for derived classes

    /**
     * Find contact list settings
     *
     * @since 3.0
     * @param aContactList contact list name
     * @return index in the settings array or KErrNotFound
     */
    TInt FindContactListSettings( const TDesC& aContactList ) const;


public:   // Unit test cases need to use this method, therefore public
    /**
     * Inserts Contact list settings
     * If seetings exist, functions will replace existing with new one
     *
     * @since 3.1
     * @param aSettings contact list settings to be inserted
     */
    void InsertContactListSettingsL( CPEngContactListSettings& aSettings );

protected:

    /**
     * Delete Contact list settings
     * Contact list is deleted from the storage as well
     * Contact list settings is deleted as well and
     * and contact list models are notified
     *
     * @since 3.1
     * @param aContactList contact list settings to be deleted
     */
    void DeleteContactListSettingsL( const TDesC& aContactList );


    /**
     * Find default contact list
     *
     * @since 3.0
     * @return Default contact list Settings or NULL.
     */
    CPEngContactListSettings* DefaultCntListSettingsOrNull();


    /**
     * Load contact list model. If contact list model is
     * not yet loaded from store it will be loaded.
     *
     * If contact list is not synchronized and flag aAnyTime is not
     * defined, function leaves with KErrNotReady.
     *
     * @since 3.0
     * @param aContactList contact list name
     * @param aAnyTime flag if contact list synchronization is checked
     * @param aReferenceUpdate
     * @return contact list model
     */
    CPEngContactListModBase& LoadContactListModelL( const TDesC& aContactList,
                                                    TBool aReferenceUpdate,
                                                    TBool aAnyTime );

    /**
     * Finds contact list model if it is loaded and returns it.
     *
     * @since 3.0
     * @param aContactList contact list name
     * @return contact list model if loaded, or NULL
     */
    CPEngContactListModBase* ContactListModelOrNull( const TDesC& aContactList );


    /**
     * Inserts new contact list model among existing ones.
     * @since 3.0
     * @param aModel Contact list model
     */
    void InsertContactListModelL( CPEngContactListModBase& aModel );


    /**
     * Notify contact list observers about change of the settings
     *
     * @since 3.0
     */
    void NotifyCntLstSettingsObservers();


    /**
     * Can be overwritten in derived classed, it is called when
     * Contact List manager settings is refreshed from store.
     * @since 3.0
     */
    virtual void HandleSettingsUpdateL();




private: // Private helpers

    /**
     * Initializes cached domain name
     *
     * @since 3.0
     * @param aSessionSlotId session slot id.
     */
    inline void InitDomainNameL( const CPEngSessionSlotId& aSessionSlot );


    /**
     * Creates watcher list related objects.
     *
     * @since 3.0
     */
    inline void InitWatcherListL();



protected:    // Data used by derived classes

    /// OWN: Access count
    TInt                                            iAccessCount;

    /// REF: List library factory
    MPEngListLibFactory&                            iFactory;

    /// OWN: Array of all contact lists settings
    RPointerArray<CPEngContactListSettings>         iContactListSettings;

    /// OWN: pointer array of the list models
    RPointerArray<CPEngContactListModBase>          iContactLists;

    /// REF: Contact lists settings observers
    RPointerArray<MPEngContactListSettingsObserver> iCntLstSettingsObservers;

    /// OWN: Is contact lists environment synchronized
    TBool                                           iEnviromentSynchronized;

    /// OWN: Domain to which user is logged in
    TPtrC                                           iUserDomain;

    /// OWN: Session Id
    CPEngSessionSlotId*                             iSessionId;
    };

#endif      //  __CPENGCONTACTLISTMANAGERBASE_H__

//  End of File






















