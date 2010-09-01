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
* Description:  List library singleton factory
*
*/

#ifndef __CPENGLISTLIBFACTORY_H__
#define __CPENGLISTLIBFACTORY_H__

//  INCLUDES
#include "PEngWVCspVersion.h"
#include "MPEngListLibFactory.h"
#include <e32base.h>


// FORWARD DECLARATIONS
class   CPEngSessionSlotId;


// CLASS DECLARATION

/**
 * List library singleton factory
 *
 * Holds all reference accessed list library singletons.
 * Factory is destroyed when last singleton gets destroyed.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class CPEngListLibFactory : public CBase,
            public MPEngListLibFactory
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngListLibFactory* InstanceLC();


    private:  // Destructor

        /**
         * Destructor.
         */
        virtual ~CPEngListLibFactory();



    public: // From MPEngListLibFactory

        /**
         * Remove Contact List Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveAttrListManager(
            CPEngAttributeListManager* aAttrManager );

        /**
         * Remove Attribute transaction List Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveAttrListTransManager(
            CPEngAttributeListTransactionManager* aAttrTransManager );

        /**
         * Remove Contact List Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveCntListManager(
            CPEngContactListManager* aCntManager );

        /**
         * Remove Contact List transaction Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveCntListTransManager(
            CPEngContactListTransactionManager* aCntTransManager );

        /**
         * Remove Authorization Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveAuthorizationManager(
            CPEngAuthorizationManager * aAuthManager );

        /**
         * Remove Authorization transaction Manager
         * @see <MPEngListLibFactory.h>
         */
        void RemoveAuthorizationTransManager(
            CPEngAuthorizationTransFactory *aAuthTransManager );



    public: // New functions

        /**
         * Open reference to the factory class
         *
         * @since 3.0
         */
        void Open();


        /**
         * Close reference to the factory class
         * If reference count reaches zero, factory is deleted
         *
         * @since 3.0
         */
        void Close();


        /**
         * Access methods for gettings different manager objects
         * for given session slot.
         *
         * @since 3.0
         * @param aSessionSlot SessionSlotId identifying the slot
         *                     for which to get the manager.
         * @return Manager object for given session slot.
         */
        CPEngAttributeListManager* AttrListManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        CPEngAttributeListTransactionManager* AttrListTransManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        CPEngContactListManager* CntListManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        CPEngContactListTransactionManager* CntListTransManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        CPEngAuthorizationManager* AuthorizationManagerL(
            const CPEngSessionSlotId& aSessionSlot );

        CPEngAuthorizationTransFactory* AuthorizationTransManagerL(
            const CPEngSessionSlotId& aSessionSlot );


    private:    // new private functions


        /**
         *  Find manager in array
         *  @since 3.0
         *  @param aArray array to look in
         *  @param aSessionSlot session slot id to look by
         *  @return index in array or KErrNotFound
         */
        template< class T>
        static TInt FindArrayElement( const RPointerArray<T>& aArray,
                                      const CPEngSessionSlotId& aSessionSlot );

        /**
         * Remove element from array.
         *
         * @since 3.0
         * @param aArray array to look in
         * @param aElement element to remove from array
         */
        template <class T>
        void RemoveArrayElement( RPointerArray<T>& aArray,
                                 T* aElement );


        /**
         * Templated helpers to locate correct
         * manager object for given session slot.
         * Instantiates new manager if one for
         * the slot doesn't exist yet.
         *
         * @since 3.0
         * @param aArray array to look in
         * @param aSessionSlot SessionSlotId identifying
         *        the slot for which to get the manager.
         * @return The manager object for session slot.
         */
        template <class T>
        T* ManagerTwoParamL( RPointerArray<T>& aArray,
                             const CPEngSessionSlotId& aSessionSlot );

        template <class T>
        T* ManagerThreeParamL( RPointerArray<T>& aArray,
                               const CPEngSessionSlotId& aSessionSlot );


        /**
         * Check reference access and destroys self
         * if no more access open.
         *
         * @since 3.0
         */
        void CheckRefAccessD();


    private:    // private constructors

        /**
         * C++ default constructor.
         */
        CPEngListLibFactory();

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

    private:    // Data

        /// OWN: Access count to the factory
        TInt                                                iAccessCount;

        /// OWN: attribute list managers
        RPointerArray<CPEngAttributeListManager>            iAttrStore;

        /// OWN: Contact list managers
        RPointerArray<CPEngContactListManager>              iCntStore;

        /// OWN: Attribute Lists Transaction Manager
        RPointerArray<CPEngAttributeListTransactionManager> iAttrTransManStore;

        /// OWN: Contact list transaction manager
        RPointerArray<CPEngContactListTransactionManager>   iCntTransManStore;

        /// OWN:
        RPointerArray<CPEngAuthorizationManager>            iAuthManStore;

        /// OWN: Authorization Manager
        RPointerArray<CPEngAuthorizationTransFactory>       iAuthTransManStore;

        /// OWN: CSP Version
        TPEngWVCspVersion                                   iCSPVersion;
    };

#endif      // __CPENGLISTLIBFACTORY_H__

// End of File
