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
* Description:  Attribute Lists Transaction manager internal interface
*
*/

#ifndef __MPENGATTRIBUTELISTTRANSACTIONMANAGER_H__
#define __MPENGATTRIBUTELISTTRANSACTIONMANAGER_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


/**
 * Attribute Lists Transaction manager abstract interface
 *
 * This interface is used by the transaction to commit changes
 * after transaction was completed
 */
class MPEngAttributeListTransactionManager
    {
    public:
        /**
         * Commit attribute list creation.
         * It marks attribute list as created in the store.
         *
         * @since 3.0
         * @param aAttrList attribute list to mark as synchronized
         */
        virtual void AttributeListCreated( const RArray<TUint32>& aAttrList ) = 0;


        /**
         * Attribute list synchronized with the server
         *
         * @since 3.0
         */
        virtual void AttributeEngineSynchronizedL() = 0;


        /**
         * Commit successful creation of the attribute list for contact list
         *
         * @since 3.0
         * @param  aContact contact list which to commit
         */
        virtual void CommitContactListL( const TDesC& aContactList ) = 0;


        /**
         * Commit successful creation of the attribute list for contact Id
         *
         * @since 3.0
         * @param aContact Contact Id which to commit
         */
        virtual void CommitContactIdL( const TDesC& aContactId ) = 0;


        /**
         * Rollback not successful creation of the attribute list
         * for contact list
         *
         * @since 3.0
         * @param aContactList contact list which to rollback
         */
        virtual void RollBackContactListL( const TDesC& aContactList ) = 0;


        /**
         * Rollback not successful creation of the attribute list for contact Id
         *
         * @since 3.0
         * @param aContactId Contact Id which to rollback
         */
        virtual void RollBackContactIdL( const TDesC& aContactId ) = 0;


        /**
         * Commit creation of the default attribute list
         *
         * @since 3.0
         */
        virtual void CommitDefaultL() = 0;

        /**
         * Roll back creattion of the default attribute list
         *
         * @since 3.0
         */
        virtual void RollBackDefaultL() = 0;


        /**
         * Commit contacts, creating of the attribute list was successful
         *
         * @since 3.0
         * @param aContactLists list of contact lists
         * @param aContactIDs list of the contact IDs
         */
        virtual void CommitDefinedContactsL(
            const CDesCArray* aContactLists,
            const CDesCArray* aContactIds ) = 0;

        /**
         * RollBack contacts. Rollbacks changes which were
         * not successfully commited on the server side
         *
         * @since 3.0
         * @param aContactLists list of contact lists
         * @param aContactIDs list of the contact IDs
         */
        virtual void RollBackDefinedContactsL(
            const CDesCArray* aContactLists,
            const CDesCArray* aContactIds ) = 0;

        /**
         * Store changes of the attribute list to the store
         *
         * @since 3.0
         */
        virtual void StoreChangesToStoreL() = 0;


    protected:
        /**
         * Protected destructor.
         */
        virtual ~MPEngAttributeListTransactionManager() {};

    };

#endif //   __MPENGATTRIBUTELISTTRANSACTIONMANAGER_H__

// End of File

