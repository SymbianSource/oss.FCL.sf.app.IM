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
* Description:  Store API to retrieve contact list models.
*
*/

#ifndef __CPENGCONTACTLISTSTORE2_H__
#define __CPENGCONTACTLISTSTORE2_H__

// INCLUDES
#include <E32Base.h>
#include <BamDescA.h>
#include "TPEngContactListBaseSettings.h"


//FORWARD DECLARATIONS
class MPEngContactList2;
class MPEngContactListProperties2;
class CPEngContactListStore2Imp;
class CPEngNWSessionSlotID2;
class TPEngContactListBaseSettings;
class MPEngContactListProperties;


/**
 * Store API to retrieve and manage contact lists.
 *
 * With this API clients can access, initialize
 * and manage presence contact lists.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngContactListStore2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngContactListStore2 object.
         *
         * Instantiates CPEngContactListStore2 object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for Presence Engine client side active
         * objects. These active objects are used when asynchronously delivering
         * events to observers or maintaining Presence Cache state.
         *
         * @return New CPEngContactListStore2 instance.
         */
        IMPORT_C static CPEngContactListStore2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngContactListStore2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngContactListStore2();



    private:

        CPEngContactListStore2();


        //-----------------------------------------------------------------------
    public: /* List of contact lists */


        /**
         * List of all contact lists. Contains both Presence Engine
         * native contact lists and unknown server contact lists.
         *
         * @since 3.0
         * @return List of all contact lists names. Contains both unknown
         *         server contact lists and Presence Engine local contact
         *         lists.
         */
        IMPORT_C const MDesCArray& AllContactListsL() const;



        /**
         * Contact list which content has been synchronized between
         * network and local client. Contact list models of such
         * a contact lists can be retrieved by the client and used.
         * This group of the contact list consist from the contact lists
         * of the native contact lists of the PEC engine and unknown
         * contact lists of the Network server.
         *
         * @since 3.0
         * @return List of active contact lists.
         */
        IMPORT_C const MDesCArray& SynchronizedContactListsL() const;




        //-----------------------------------------------------------------------
    public: /* Contact list access */


        /**
         * Gets a reference to presence contact list.
         *
         * Ownership of contact list remains on the Presence Engine.
         * Contact list is identified by its name defined when
         * the list was created or by the name how it exists on the
         * network server if the list is unknown server contact list.
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         * @return Contact list. Client can read and edit the contact
         *         list contents. Edit results are automaticly preserved
         *         to Presence engine's storage.
         */
        IMPORT_C MPEngContactList2& ContactListL( const TDesC& aContactListName );





        //-----------------------------------------------------------------------
    public: /* Contact list management */


        /**
         * Creates a new contact list.
         *
         * Created new contact list is based on the:
         *  - contact list name
         *  - contact list base settings
         *
         * Created contact list is stored permanently and
         * it is made available for clients for next sessions.
         *
         * Method returns reference to contact list property interface.
         * Client can use this property interface to define further
         * contact list properties. If client doesn't set further
         * settings, default values are used.
         *
         * Contact list must be synchronized before the client can
         * use it freely.
         *
         * @see CPEngContactListTransaction2::SynchronizeContactList()
         *
         * @since 3.0
         * @param aContactListName Name for the new contact list.
         * @param aBaseSettings Base settings for the new contact list.
         * @return Reference to just created contact lists properties.
         * Properties ownership isn't returned.
         */
        IMPORT_C MPEngContactListProperties& CreateContactListL(
            const TDesC& aContactListName,
            TPEngContactListBaseSettings& aBaseSettings );


        /**
         * Deletes a contact list.
         *
         * Sets the contact list deleted, so it cannot be used anymore.
         * When the deleted contact list is synchronized with
         * the network server, it is also to deleted from
         * network server.
         *
         * @see CPEngContactListTransaction2::SynchronizeContactList()
         *
         * @since 3.0
         * @param aContactList Name of the contact list.
         */
        IMPORT_C void DeleteContactListL(
            const TDesC& aContactListName );

        /**
         *  Change contact list type
         *
         *  Contact list type is updated. Client does not have to retrieve
         *  again contact list model if it was retrieved before. Change
         *  is imidiate.
         *
         *  @since 3.0
         *  @param aContactList name of the contact list to change type
         *  @param aContactListType new type of the contact list.
         */
        IMPORT_C void ChangeContactListTypeL(
            const TDesC& aContactList,
            TPEngContactListType aContactListType );


        /**
         * Gets the contact list properties.
         *
         * Contact list properties interface allows to view
         * and edit contact list properties. Contact list
         * properties can be retrieved for all contact lists.
         *
         * If the contact list isn't synchronized with network
         * server, some of the details are not available.
         * Particularly  display name of the unknown server
         * contact lists is unavailable.
         *
         * @see CPEngContactListTransaction2::SynchronizeContactList()
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         * @return Contact list properties.
         *         Properties ownership isn't returned.
         */
        IMPORT_C MPEngContactListProperties& ContactListPropertiesL(
            const TDesC& aContactListName );



        /**
         * Gets the contact list properties.
         *
         * Contact list properties interface allows to view
         * and edit contact list properties. Contact list
         * properties can be retrieved for all contact lists.
         *
         * If the contact list isn't synchronized with network
         * server, some of the details are not available.
         * Particularly  display name of the unknown server
         * contact lists is unavailable.
         *
         * @see CPEngContactListTransaction2::SynchronizeContactList()
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         * @return Contact list properties.
         *         Properties ownership isn't returned.
         */
        IMPORT_C MPEngContactListProperties& DefaultContactListPropertiesL();


        /**
         * Sets a contact list as default contact list.
         *
         * To make selection effective, contact list must be
         * synchronized with network server.
         *
         * @see CPEngContactListTransaction2::SynchronizeContactList()
         *
         * @since 3.0
         * @param aContactListName Contact list to be set as default
         * contact list.
         */
        IMPORT_C void SetAsDefaultContactListL(
            const TDesC& aContactListName );




        //-----------------------------------------------------------------------
    public:  /* Contact list releasing */



        /**
         * Releases the contact list from the memory.
         *
         * Releases contact list from the client side memory.
         * Contact list can be still used by other clients.
         *
         * If client wants to use same contact list again,
         * it can be done by retrieving the list again with the
         * ContactListL() method.
         *
         * @since 3.0
         * @param aContactListName The contact list to be released.
         * @return Currently KErrNone.
         */
        IMPORT_C TInt ReleaseContactList( const TDesC& aContactListName );





        //-----------------------------------------------------------------------
    public: /* @deprecated contact lists management functions*/

        IMPORT_C const MDesCArray& ActiveContactListsL() const;
        IMPORT_C const MDesCArray& NativeContactListsL() const;
        IMPORT_C const MDesCArray& UnknownServerContactListsL() const;
        IMPORT_C TInt ReleaseUnknownContactListFromStore(
            const TDesC& aContactListName );





        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngContactListStore2Imp*     iImp;
    };



#endif      //  __CPENGCONTACTLISTSTORE2_H__

//End of file
