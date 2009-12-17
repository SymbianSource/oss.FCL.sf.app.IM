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



#ifndef MCACONTACTLIST_H
#define MCACONTACTLIST_H

//  INCLUDES
#include "TStorageManagerGlobals.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class MCAStoredContact;

// CLASS DECLARATION

/**
 *  Contact list data interface
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
class MCAContactList
    {
    public: // Definitions

        enum TSynchroniseState
            {
            ESynchroniseNotDone,
            ESynchroniseDone,
            ESynchroniseFailed
            };

    public: // New functions

        /**
         * List id of contact list
         * @return List id of contact list.
         */
        virtual const TDesC& ListId() const = 0;

        /**
         * Displayname of contact list
         * @return  Display name of list.
         *          If displayname is not set, Id part of list id is returned.
         */
        virtual TPtrC DisplayName() const = 0;

        /**
         * Set display name of list.
         * @param aDisplayName, new name of list.
         */
        virtual void SetDisplayNameL( const TDesC& aDisplayName ) = 0;

        /**
         * Is list collapsed or not.
         * @return ETrue, if collapsed, EFalse if expanded
         */
        virtual TBool Collapsed() const = 0;

        /**
         * Set list collapse status
         * @param aCollapsed. ETrue when collapsed, EFalse if expanded
         */
        virtual void SetCollapsed( TBool aCollapsed ) = 0;

        /**
         * Count of items in list
         * @return count of contacts.
         */
        virtual TInt Count() const = 0;

        /**
         * Get list item by index from list.
         * @param  aIndex. Index of item.
         * @return Contact in index.
         */
        virtual MCAStoredContact& operator[]( TInt aIndex ) const = 0;

        /**
         * Get Filtered list item by index from list
         * @param aIndex. Index of Filtered item.
         * @param aFilter. Filters the contacts according to this filter.
         * @return contact in index upon the filter.
         */
        virtual MCAStoredContact& FilteredContact(
            TInt aIndex, TStorageManagerGlobals::TFilterType aFilter ) const = 0;

        /**
         * Count of non-blocked items in list
         * @return count of non-blocked contacts
         */
        virtual TInt FilteredCount( TStorageManagerGlobals::TFilterType aFilter ) const = 0;

        /**
         * Get amount of pending messages in list. Sum of all contacts pendingmessages in list
         */
        virtual TInt PendingMessages() const = 0;

        /**
         * Sort items by currently active sort algorithm.
         */
        virtual void Sort() = 0;

        /**
         * Resort one contact in the contact list
         */
        virtual void ResortContact( MCAStoredContact* aContact ) = 0;
        virtual void ResortUnKnownContact( MCAStoredContact* aContact ) = 0;

        /**
         * Is list selected or not
         * @return ETrue if selected, EFalse if not.
         */
        virtual TBool Selected() const = 0;

        /**
         * Set selected status of list.
         * @param aSelected ETrue if selected, EFalse if not.
         */
        virtual void SetSelected( TBool aSelected ) = 0;

        /**
         * Find index of contact in list.
         * @param aContact. Contact item to be found.
         * @param aFilter. Filters the contacts according to this filter.
         * @return index of contact or KErrNotFound, if contact is not found.
         */
        virtual TInt FindIndexOfContact(
            const MCAStoredContact* aContact,
            TStorageManagerGlobals::TFilterType aFilter =
                TStorageManagerGlobals::EFilterAll ) const = 0;

        /*
         * Checks if the contact should be shown according to the filter
         * @param aContact Contact to check
         * @param aFilter Tells which contacts should be shown
         * @return ETrue if contact should be shown according
         *         to the given filter. EFalse otherwise.
         */
        virtual TBool FilterAllowsContact( const MCAStoredContact* aContact,
                                           TStorageManagerGlobals::TFilterType aFilter ) const = 0;


        /**
         * Set contact list as synchronised state for list. Can be failed, done or notdone.
         * @param aSynchroniseState. Default done.
         */
        virtual void SetSynchronised( TSynchroniseState aSynchroniseState = ESynchroniseDone ) = 0;

        /**
         * Get state of synchronisation.
         * @return State of synchronisation. Done, not done or failed.
         */
        virtual TSynchroniseState Synchronised() const = 0;

    protected:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~MCAContactList() {};
    };


#endif      // MCACONTACTLIST_H

// End of File
