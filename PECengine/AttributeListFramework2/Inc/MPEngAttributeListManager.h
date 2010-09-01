/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence Attribute List manager abstract interface
*
*/



#ifndef __MPENGATTRIBUTELISTMANAGER_H__
#define __MPENGATTRIBUTELISTMANAGER_H__

//  INCLUDES
#include <e32base.h>

// Attribute list model
#include "MPEngAttributeList2.h"

/**
 *  Presence Attribute List manager abstract interface
 *
 *  This interface is used by the Presence Manager to retrieve attribute lists
 *  and store them after editing then.
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */
class MPEngAttributeListManager
    {

    public: // new public functions

        /**
         * Close reference to the Class
         *
         * @since 3.0
         */
        virtual void Close() = 0;

        /**
         * Create Empty attribute list
         *
         * @since 3.0
         * @return empty attribute list
         */
        virtual MPEngAttributeList2* CreateEmptyAttributeListL() =  0;

        /**
         * Get default attribute list
         *
         * @since 3.0
         * @param attribute list set
         * @return default attribute list
         */
        virtual MPEngAttributeList2* GetDefaultAttributeListL(
            TPEngAttributeListsSet2 aAttrListsSet ) = 0;

        /**
         * Set new default attribute list
         *
         * @since 3.0
         * @param aAttrList attribute list to be set as default
         */
        virtual void SetAsDefaultAttributeListL(
            MPEngAttributeList2& aAttrList ) = 0;

        /**
         * Delete default attribute list
         *
         * @since 3.0
         */
        virtual void DeleteDefaultAttributeListL() = 0;

        /**
         * Get attribute list attached to the contact Id
         *
         * @since 3.0
         * @param aContactId contact id
         * @param attribute list set
         * @return attribute list attached to contact Id
         */
        virtual MPEngAttributeList2* GetAttributeListForUserL(
            const TDesC& aContactId,
            TPEngAttributeListsSet2 aAttrListsSet ) = 0;

        /**
         * Get attribute list attached to the contact list
         *
         * @since 3.0
         * @param aContactList contact list
         * @param attribute list set
         * @return attribute list attached to contact list
         */
        virtual MPEngAttributeList2* GetAttributeListForContactListL(
            const TDesC& aContactList,
            TPEngAttributeListsSet2 aAttrListsSet ) = 0;

        /**
         * Attach attribute list to the contact list
         *
         * @since 3.0
         * @param aContactList contact list to attach to
         * @param aAttrList attribute list to attach
         */
        virtual void AttachAttributeListToContactListL(
            const TDesC& aContactList,
            MPEngAttributeList2& aAttrList ) = 0;

        /**
         * Attach attribute list to the contact id
         *
         * @since 3.0
         * @param aContactId contact id to attach to
         * @param aAttrList attribute list to attach
         */
        virtual void AttachAttributeListToUserL(
            const TDesC& aContactId,
            MPEngAttributeList2& aAttrList ) = 0;

        /**
         * Delete attribute list from Contact list
         *
         * @since 3.0
         * @param aContactList contact list
         */
        virtual void DeleteAttributeListFromContactListL(
            const TDesC& aContactList ) = 0;

        /**
         *  Delete attribute list from Contact Id
         *
         * @since 3.0
         * @param aContactId contact id
         */
        virtual void DeleteAttributeListFromUserL(
            const TDesC& aContactId ) = 0;

    protected:
        /**
         * Protected Destructor.
         */
        virtual ~MPEngAttributeListManager() {};

    };

#endif //   __MPENGATTRIBUTELISTMANAGER_H__

// End of File
