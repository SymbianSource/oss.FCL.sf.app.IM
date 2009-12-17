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
* Description:  Contact data container implementation
*
*/



#ifndef CCACONTACTLIST_H
#define CCACONTACTLIST_H

//  INCLUDES
#include <e32base.h>
#include "MCAContactList.h"
#include "MCAStoredContact.h"

// FORWARD DECLARATIONS
class MCAStoredContact;
class CCAContactSorter;
class MCAStorageInfo;
class MCAStoredContactsObserver;

// CLASS DECLARATION

/**
 *  Contact list data implementation
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
NONSHARABLE_CLASS( CCAContactList ) : public CBase, public MCAContactList
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aObserver. Observer for events
     * @param aSorter. Sorter for contaclist.
     * @param aInfo. Information from storage
     * @param alistId. List id of list. Default value KNullDesC, if not set.
     * @param aDisplayname. Name of list.
     */
    static CCAContactList* NewL(    MCAStoredContactsObserver& aObserver,
    CCAContactSorter& aSorter,
    MCAStorageInfo& aInfo,
    const TDesC& aListId,
    const TDesC& aDisplayName = KNullDesC );
    /**
     * Two-phased constructor.
     * @param aObserver. Observer for events
     * @param aSorter. Sorter for contaclist.
     * @param aInfo. Information from storage
     * @param alistId. List id of list.
     * @param alistId. List id of list. Default value KNullDesC, if not set.
     */
    static CCAContactList* NewLC(   MCAStoredContactsObserver& aObserver,
                                    CCAContactSorter& aSorter,
                                    MCAStorageInfo& aInfo,
                                    const TDesC& aListId,
                                    const TDesC& aDisplayName = KNullDesC );

    /**
     * Destructor.
     */
    ~CCAContactList();


public:

    /**
     * @see MCAContactList
     */
    const TDesC& ListId() const;

    /**
     * @see MCAContactList
     */
    TPtrC DisplayName() const;

    /**
     * @see MCAContactList
     */
    void SetDisplayNameL( const TDesC& aDisplayName );

    /**
     * @see MCAContactList
     */
    void SetCollapsed( TBool aCollapsed );

    /**
     * @see MCAContactList
     */
    TBool Collapsed() const;

    /**
     * @see MCAContactList
     */
    TInt Count() const;

    /**
     * @see MCAContactList
     */
    MCAStoredContact& operator[]( TInt aIndex ) const;

    /**
     * @see MCAContactList
     */
    MCAStoredContact& FilteredContact(
        TInt aIndex, TStorageManagerGlobals::TFilterType aFilter ) const;

    /**
     * @see MCAContactList
     */
    virtual TInt FilteredCount(
        TStorageManagerGlobals::TFilterType aFilter ) const;

    /**
     * @see MCAContactList
     */
    TInt PendingMessages() const;

    /**
     * @see MCAContactList
     */
    void Sort();

    /**
     * @see MCAContactList
     */
    void ResortContact( MCAStoredContact* aContact );
    void ResortUnKnownContact( MCAStoredContact* aContact );

    /**
     * @see MCAContactList
     */
    TBool Selected() const;

    /**
     * @see MCAContactList
     */
    void SetSelected( TBool aSelected );

    /**
     * @see MCAContactList
     */
    TInt FindIndexOfContact(
        const MCAStoredContact* aContact,
        TStorageManagerGlobals::TFilterType aFilter =
            TStorageManagerGlobals::EFilterAll ) const;

    /**
     * @see MCAContactList
     */
    TBool FilterAllowsContact( const MCAStoredContact* aContact,
                               TStorageManagerGlobals::TFilterType aFilter ) const;

    /**
     * @see MCAContactList
     */
    void SetSynchronised( TSynchroniseState aSynchroniseState = ESynchroniseDone );

    /**
     * @see MCAContactList
     */
    TSynchroniseState Synchronised() const;

public: // New functions

    /**
     * Set list id
     * @param aListId. New list id for list.
     */
    void SetListIdL( const TDesC& aListId );

    /**
     * Find contact by contact id.
     * @param aContactId. Id to find.
     * @return Pointer to found contact. NULL if not found.
     */
    MCAStoredContact* FindContact( const TDesC& aContactId );

    /**
     * Find contact by contact id.
     * @param aContactId. Pointer of contact to find.
     * @return Pointer to found contact. NULL if not found.
     */
    MCAStoredContact* FindContact( const MCAStoredContact* aContact );

    /**
     * Find contact by contact nickname.
     * @param aNick. Id to find.
     * @return Pointer to found contact. NULL if not found.
     */
    MCAStoredContact* FindContactByNick( const TDesC& aNick );

    /**
     * Remove contact from list
     * @param aContactId. Id of contact to be removed.
     */
    TInt RemoveContact( const TDesC& aContactId );

    /**
     * Add contact to list.
     * @param aContact. Contact to be added to list.
     * @return Pointer of added contact
     */
    MCAStoredContact* AddContactL( MCAStoredContact* aContact );

    /**
     * Count of contacts in list. Skipp all ofline contacts.
     */
    TInt ContactCount( TBool aSkipOfflineContacts,
                       TBool aSkipBlocekedContacts ) const;

    /**
     * Count of online items in list
     * @return count of online contacts
     */
    TInt OnlineCount() const;

    /**
     * Count of non-blocked items in list
     * @return count of non-blocked contacts
     */
    TInt NonBlockedCount() const;

    /**
     * Get online list item by index from list
     * @param aIndex. Index of online item.
     * @return Online contact in index.
     */
    MCAStoredContact& OnlineContact( TInt aIndex ) const;

    /**
     * Get non-blocked list item by index from list
     * @param aIndex. Index of non-blocked item.
     * @return non-blocked contact in index.
     */
    MCAStoredContact& NonBlockedContact( TInt aIndex ) const;

    /**
     * Set primary list properties in use.
     * @param aPrimaryInuse. If ETrue, primary in use.
     *                       EFalse secondary in use.
     */
    void SetPrimaryInuse( TBool aPrimaryInUse );

private: // Own methods

    /**
     * Constructor
     */
    CCAContactList( MCAStoredContactsObserver& aObserver,
                    CCAContactSorter& aSorter,
                    MCAStorageInfo& aInfo );

    /**
     * Two-phased constructor
     */
    void ConstructL( const TDesC& aListId, const TDesC& aDisplayName );

    /**
     * Identity relation for finding contact by id.
     */
    static TBool ContactFindById( const TDesC& aId,
                                  const MCAStoredContact& aContact );

    /**
     * Identity relation for finding contact by nickname.
     */
    static TBool ContactFindByNick( const TDesC& aId,
                                    const MCAStoredContact& aContact );
    /**
     *
     */
    TInt FindContactIndex( const TDesC& aContactId,
                           TInt& aOrderedIndex ) const;

    /**
     * Gets the index of contact with the given nick
     */
    TInt FindContactIndexByNick( const TDesC& aNick ) const;


    TInt FindContactInArray( const TDesC& aId,
                             TBool ( *aCompare )( const TDesC&, const MCAStoredContact& ) ) const;

    TInt FindContactEntry( const MCAStoredContact* aContact ) const;

    TInt FindContactByUserId( const TDesC& aUserId,
                              TInt& aIndexOrderedArray ) const;

private:    // Data

    /// Stored contacts in contact list.
    RPointerArray< MCAStoredContact > iContacts;

    /// Alphabeticaly ordered Contacts
    RPointerArray< MCAStoredContact > iOrderedContacts;

    /// Sorter for contacts.
    CCAContactSorter& iSorter;

    /// Information from storage.
    MCAStorageInfo& iInfo;

    /// Id of list
    HBufC* iListId;

    /// Displayname of list
    HBufC* iDisplayName;

    /// Collapsed or not. This will be primary state of list.
    TBool iPrimaryCollapsed;

    /// When needed secondary collapse status, this is needed.
    TBool iSecondaryCollapsed;

    /// This will be primary/secondary collapsed.
    TBool *iCurrentCollapsed;

    /// Selected or not
    TBool iSelected;

    /// Observer for events.
    MCAStoredContactsObserver& iObserver;

    /// Synchronise state
    TSynchroniseState iSyncState;
    };


#endif      // CCACONTACTLIST_H

// End of File
