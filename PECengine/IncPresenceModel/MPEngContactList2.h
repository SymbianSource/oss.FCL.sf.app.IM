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
* Description:  Contact list model interface & related definitions.
*
*/

#ifndef __MPENGCONTACTLIST2_H__
#define __MPENGCONTACTLIST2_H__


//  INCLUDES
#include <E32Std.h>
#include <BaDescA.h>
#include <MPEngContactItem.h>



// DATA TYPES


/**
 * Contact list sub views.
 *
 * @since 3.0
 */
enum TPEngContactListView
    {
    /**
     * Contact list as it presented in the storage of the PEC engine
     * This contact list contains also changes which has been
     * done on the contact list, but haven't been yet commited to the
     * Network server, therefore network version of the contact
     * list can vary till the next update call.
     * NOTE!
     * Local and Network contact list are identical once
     * Contact list is not editable
     */
    EPEngCntListLocalView = 1,

    /**
     * Contact list as it exists on network server
     * This is version of the contact list, as it was last time
     * updated to the Network Server.
     * Content is mostly same as EPEngCntListLocalVersion, unless
     * there have been dome some changes on the contact list
     * and client haven't updated them yet to the network.
     * NOTE!
     * Local and Network contact list are identical once
     * Contact list is not editable
     */
    EPEngCntListNetworkView = 2,

    /**
     * Contacts which were removed from the contact list.
     * Those contact have been removed from the contact list
     * within last update of the contact list to the network.
     * Contact can be removed on demand of the Client, or network
     * server can refuse contact item when it is tried to be added
     * Contact are also removed when contact list update transaction
     * fails for what ever reason.
     * NOTE!
     * Removed Contact are not defined for not editable contact lists
     */
    EPEngRemovedContacts = 3,

    /**
     * Contacts which were added to the contact.
     * Those contact have been added to the contact list
     * within last update of the contact list to the network.
     * Contact can be added on demand of the Client, or network
     * server can also merge two contact into one, if they are same
     * and it failed to be recognized by the client. For intance
     * lack of knowledge of the Domain name.
     * NOTE!
     * Added Contact are not defined for not editable contact lists
     */
    EPEngAddedContacts = 4
    };

// CONSTANTS

/**
 *	Special case of the contact list created by the Server
 *	Content of the contact list is set of users subscribing
 *	presence of the loged user.
 *	Watcher List does not have to be available always, depends on
 *	the server user is logged in.
 */
_LIT( KPEngWatcherList, "watcherlist" );


// FORWARD DECLARATIONS
class MPEngContactListProperties;





// CLASS DECLARATION
/**
 * General API for different contact lists.
 *
 * Interface allows users to manipulate and retrieve
 * data from the contact list.
 *
 * @since 3.0
 */
class MPEngContactList2
    {

    public: // Mutators

        /**
         * Inserts a Contact into the Contact list.
         *
         * Each Contact can exist only once in the contact list.
         * If added contact already exists in the contact list, function
         * Leaves with KErrAlready exists.
         * If Contact List nick name needs to be changed for contact,
         * MPEngContactItem.h interface should be used instead.
         * This function can also fail if some other client
         * is already editing content of this contact list.
         *
         * Function leaves with KErrNoMemory, if there is insufficient
         * memory available to perform operation.
         *
         * If the function leaves, the Contact list is left in the state
         * it was before
         *
         * !NOTE!
         * This function is not available for Non-Editable contact lists.
         *
         * @since 3.0
         *
         * @param aContact Contact to be added to the contact list
         * @param aNickName nick name for the contact
         *			If nick name is not passed, no nick name is defined
         *			for the contact, same as KNullDesC value.
         * @return The position within the contact list.
         */
        virtual TInt AddContactL( const TDesC& aContact,
                                  const TDesC& aNickName = KNullDesC ) = 0;

        /**
         * Removes Contact from the Contact list
         *
         * If contact does not exists in the contact list, function
         * does nothing.
         *
         * Function leaves with KErrNoMemory, if there is insufficient
         * memory available to perform operation.
         *
         * If the function leaves, the Contact list is left in the state
         * it was before
         *
         * !NOTE!
         * This function is not available for Non-Editable contact lists.
         *
         * @since 3.0
         *
         * @param aContact The contact to be removed from the Contact list
         */
        virtual void RemoveContactL( const TDesC& aContact ) = 0;



        /**
         * Find Contact in the contact list
         *
         * Finds Contact in defined contact list.
         * Search is performed with the comparison sense to
         * Contact specifics and also smart domain comparison is employed
         * if home domain is known to the system.
         * By default search is performed on the Local contact list view
         *
         * !NOTE!
         * Not all views are available for Non-Editable contact lists
         *
         * @since 3.0
         *
         * @param aContact contact to look for
         * @param aView Contact list view to look for Contact in, by default
         *              Local contact list view is used
         * @return position
         */
        virtual TInt FindContact(
            const TDesC& aContact,
            TPEngContactListView aView
            = EPEngCntListLocalView ) const = 0;



        /**
         * Count of the Contacts of the Contact list view
         *
         * Returns count of the contact IDs on the defined Contact List
         * view.
         * By default Local List view is used
         *
         * !NOTE!
         * Not all views are available for Non-Editable contact lists
         *
         * @since 3.0
         * @param aView Contact list view to get count for, by default
         *              Local contact list view is used
         * @return Count of the Contact of the list
         */
        virtual TInt Count( TPEngContactListView aView
                            = EPEngCntListLocalView ) const = 0;


        /**
         * Contact Item of the Contact list
         *
         * Gets reference to the particular contact item model
         * of the contact list. This model can be used to view contact
         * item details and modify them.
         * Both const and non const overload available.
         *
         * There is no difference if contact item model was retrieved
         * from Network or local view as far as contact item exists in
         * both views.
         *
         * !NOTE!
         * Contact Item can be retrieved only for Local and network
         * Contact list view. Since there are not details
         * available for Removed and Added contacts, except contact part,
         * also editing of those lists is not allowed at any case.
         * More about contact item model services can be seen in
         * MPEngContactItem.h
         *
         * @since 3.0
         * @param aIndex index of desired contact Item within list
         * @param aSublist sublist of the contact list
         * @return reference to contact Item model
         */
        virtual MPEngContactItem& ContactItem(
            TInt aIndex,
            TPEngContactListView aView
            = EPEngCntListLocalView ) = 0;

        virtual const MPEngContactItem& ContactItem(
            TInt aIndex,
            TPEngContactListView aView
            = EPEngCntListLocalView ) const = 0;


        /**
         * Contacts which were removed from the contact list
         *
         * List of the contact's WV Ids which were removed form the contact
         * list, either on the clients demand or by the Network server
         * Contacts can be also removed once update operation fails
         *
         * @since 3.0
         * @return des array with removed contacts
         */
        virtual const MDesCArray& RemovedContacts( ) const = 0;


        /**
         * Contacts which were added to the contact list
         *
         * List of the contact's WV Ids which were added to the contact
         * list, either on the clients demand or by the Network server.
         *
         * @since 3.0
         * @return des array with added contacts
         */
        virtual const MDesCArray& AddedContacts( ) const = 0;



    public: // Contact List properties

        /**
         * Contact list Settings
         *
         * Returns contact list properties model.
         * This model can be used to view contact list
         * details and also to edit certain parameters of the
         * contact list which are allowed to be altered once
         * contact list exists.
         *
         * @since 3.0
         * @return Contact list properties model
         */
        virtual MPEngContactListProperties& ListProperties() = 0;




    public: // Functions for storage management

        /**
         * Remove all contacts from the contact list
         *
         * Cleans contact list from all Contacts
         *
         * @since 3.0
         */
        virtual void RemoveAllContactsL( ) = 0;


        /**
         * Suspend contact list update notifications
         *
         * This should be used when client is intending to do lot
         * of small changes of the contact list content and wants
         * them to be notified to other clients all at once once
         * editing is finished.
         * Suspending is removed by calling ReleaseUpdateNotification()
         *
         * @since 3.0
         */
        virtual void SuspendUpdateNotification( ) = 0;


        /**
         * Release sending of the contact list update notifications
         *
         * Update notifications are released and also all notifications
         * which were ignored when suspend was active are now notified
         *
         * @since 3.0
         */
        virtual void ReleaseUpdateNotification( ) = 0;


        /**
         * Roll back all changes
         *
         * All un-commited changes are rolled back and
         * contact list is put in the state it exist on the network.
         * This will roll back changes of the Contact Item edition
         * and also Contact List Network related changes
         *
         * @since 3.0
         * @return KErrNone if store of the roll back state was successful
         * system wide error if storing failed
         */
        virtual TInt RollBackAllChangesL( ) = 0;


        /**
         * Contact list locked by other client
         *
         * Returns ETrue if contact list has been locked by another
         * client for editing of its content
         *
         * @since 3.0
         * @return ETrue if Contact List is locked
         */
        virtual TBool UpdateInProgress( ) const = 0;


        /**
         *  Update of the contact list required
         *
         *	Returns ETrue if contact list needs to be upated
         *
         *  @since 3.0
         *  @return ETrue if update is needed. EFalse if not.
         */
        virtual TBool UpdateRequired() const = 0;



    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MPEngContactList2() {};
    };


#endif      //  __MPENGCONTACTLIST2_H__

//  End of File



