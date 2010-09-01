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
* Description:  Store API to retrieve attribute lists.
*
*/

#ifndef __CPENGATTRIBUTELISTSTORE2_H__
#define __CPENGATTRIBUTELISTSTORE2_H__

// INCLUDES
#include <E32Base.h>
#include <MPEngAttributeList2.h>



//FORWARD DECLARATIONS
class CPEngAttributeListStore2Imp;
class CPEngNWSessionSlotID2;



/**
 * Store API to retrieve presence attribute lists.
 *
 * With this API clients can access and initialize
 * presence attribute lists.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngAttributeListStore2 : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */


        /**
         * Instantiates CPEngAttributeListStore2 object.
         *
         * Instantiates CPEngAttributeListStore2 object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * @Error KErrNotFound Requested NWSessionSlot not found
         * @error KErrArgument Given NWSessionSlotID malformed
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for Presence Engine client side active
         * objects. These active objects are used when asynchronously delivering
         * events to observers or maintaining Presence Cache state.
         *
         * @return New CPEngAttributeStore2 instance.
         */
        IMPORT_C static CPEngAttributeListStore2* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngAttributeListStore2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngAttributeListStore2();



    private:
        CPEngAttributeListStore2();




        //-----------------------------------------------------------------------
    public: /* Presence attribute lists */


        /**
         * Creates a new empty presence attribute list.
         * Ownership of created attribute list is transferred to caller.
         *
         * @since 3.0
         * @return New attribute list instance.
         */
        IMPORT_C MPEngAttributeList2* CreateEmptyAttributeListL();




        //-----------------------------------------------------------------------
    public: /* Default attribute list */


        /**
         * Gets default attribute list.
         *
         * @since 3.0
         * @param aAttributeListsSet Wether to get local or network
         * version of attribute list.
         * @return Attribute list instance. Ownership is returned to caller.
         */
        IMPORT_C MPEngAttributeList2* GetDefaultAttributeListL(
            TPEngAttributeListsSet2 aAttrListsSet );



        /**
         * Sets attribute list as default attribute list.
         *
         * @since 3.0
         * @param aAttributeList The attribute list to set as default
         * attribute list.
         */
        IMPORT_C void SetAsDefaultAttributeListL(
            MPEngAttributeList2& aAttributeList );


        /**
         * Deletes the default attribute list.
         *
         * @since 3.0
         */
        IMPORT_C void DeleteDefaultAttributeListL();






        //-----------------------------------------------------------------------
    public: /* Contact list attribute lists */


        /**
         * Get contact list's attribute list
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         * @param aAttributeListsSet Wether to get local or network
         * version of attribute list.
         * @return Attribute list instance. Ownership is returned to caller.
         */
        IMPORT_C MPEngAttributeList2* GetAttributeListForContactListL(
            const TDesC& aContactListName,
            TPEngAttributeListsSet2 aAttrListsSet );


        /**
         * Attaches attribute list to contact list.
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         * @param aAttributeList The attribute list to be attached.
         */
        IMPORT_C void AttachAttributeListToContactListL(
            const TDesC& aContactListName,
            MPEngAttributeList2& aAttributeList );


        /**
         * Delete attribute list from contact list
         *
         * @since 3.0
         * @param aContactListName Name of the contact list.
         */
        IMPORT_C void DeleteAttributeListFromContactListL(
            const TDesC& aContactListName );






        //-----------------------------------------------------------------------
    public: /* Presence user attribute lists */


        /**
         * Get user's attribute list
         *
         * @since 3.0
         * @param aPresenceID User's presence id.
         * @param aAttributeListsSet Wether to get local or network
         * version of attribute list model.
         * @return Attribute list instance. Ownership is returned to caller.
         */
        IMPORT_C MPEngAttributeList2* GetAttributeListForUserL(
            const TDesC& aPresenceID,
            TPEngAttributeListsSet2 aAttrListsSet );



        /**
         * Attaches attribute list to user.
         *
         * @since 3.0
         * @param aPresenceID User's presence id.
         * @param aAttributeList The attribute list to be attached.
         */
        IMPORT_C void AttachAttributeListToUserL(
            const TDesC& aPresenceID,
            MPEngAttributeList2& aList );




        /**
         * Deletes attribute list from user.
         *
         * @since 3.0
         * @param aPresenceID User's presence id.
         */
        IMPORT_C void DeleteAttributeListFromUserL(
            const TDesC& aPresenceID );




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngAttributeListStore2Imp*     iImp;

    };



#endif      //  __CPENGATTRIBUTELISTSTORE2_H__


// End of file


