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
* Description:  List Library dll interface
*
*/

#ifndef __PENGLISTLIBRARYFACTORY_H
#define __PENGLISTLIBRARYFACTORY_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class   CPEngSessionSlotId;
class   MPEngContactListManager;
class   MPEngTransactionFactory;
class   MPEngAttributeListManager;
class   MPEngTransactionFactory;
class   MPEngAuthorizationManager;


/**
 * List Library dll interface.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class PEngListLibraryFactory
    {
    public:

        /**
         * Attribute list manager
         * Returned object is reference counted singleton object
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return attribute list manager
         */
        IMPORT_C static MPEngAttributeListManager* AttributeListsManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        /**
         * Contact list manager
         * Returned object is reference counted singleton object
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return Contact list manager
         */
        IMPORT_C static MPEngContactListManager* ContactListManagerL(
            const CPEngSessionSlotId& aSessionSlot );


        /**
         * Authorization manager
         * Returned object is reference counted singleton object
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return Authorization manager
         */
        IMPORT_C static MPEngAuthorizationManager* AuthorizationManagerL(
            const CPEngSessionSlotId& aSessionSlot );


        /**
         * Attribute list transaction manager.
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return attribute list transaction factory
         */
        IMPORT_C static MPEngTransactionFactory* AttributeListsTransactionManagerLC(
            const CPEngSessionSlotId& aSessionSlot );

        /**
         * Contact list transaction manager.
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return Contact list transaction factory
         */
        IMPORT_C static MPEngTransactionFactory* ContactListTransactionManagerLC(
            const CPEngSessionSlotId& aSessionSlot );


        /**
         * Authorization transaction manager.
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return Authorization transaction factory
         */
        IMPORT_C static MPEngTransactionFactory* AuthorizationTransactionManagerLC(
            const CPEngSessionSlotId& aSessionSlot );


    private: // Prohibited constructors and destructor

        PEngListLibraryFactory();
        ~PEngListLibraryFactory();

    };

#endif      // __PENGLISTLIBRARYFACTORY_H

// End of File
