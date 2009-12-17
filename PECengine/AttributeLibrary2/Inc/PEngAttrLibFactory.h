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
* Description:  PEngAttrLibFactory - object factory
*
*/

#ifndef __PENGATTRLIBFACTORY_H
#define __PENGATTRLIBFACTORY_H

//  INCLUDES
#include <E32Std.h>


// FORWARD DECLARATIONS
class CPEngSessionSlotId;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrTransactionEnv;
class MPEngTransactionFactory;



// CLASS DECLARATION

/**
 * Attribute Library Factory
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
class PEngAttrLibFactory
    {
    public: // Reference counted singletons

        /**
         * Gets a reference to Attribute Manager.
         *
         * Client must release instance by calling
         * MPEngPresenceAttrManager::Close() when it is no
         * more needed. If instance is needed just temporarily
         * (not as member), use AttributeManagerInstanceLC()
         * and the instance is freed by CleanupStack.
         *
         * @since 3.0
         * @param aSlotId
         * @return Attribute manager instance.
         */
        IMPORT_C static MPEngPresenceAttrManager* AttributeManagerInstanceL(
            const CPEngSessionSlotId& aSlotId );
        IMPORT_C static MPEngPresenceAttrManager* AttributeManagerInstanceLC(
            const CPEngSessionSlotId& aSlotId );


        /**
         * Gets a reference to Attribute Transaction environment.
         *
         * Attribute transaction environment provides services
         * to server side attribute transactions.
         *
         * Client must release instance by calling
         * MPEngPresenceAttrTransactionEnv::Close() when it is no
         * more needed. If instance is needed just temporarily
         * (not as member), use AttributeTransEnvInstanceLC()
         * and the instance is freed by CleanupStack.
         *
         * @since 3.0
         * @param aSlotId
         * @return Attribute transaction environment instance.
         */
        IMPORT_C static MPEngPresenceAttrTransactionEnv* AttributeTransEnvInstanceL(
            const CPEngSessionSlotId& aSlotId );
        IMPORT_C static MPEngPresenceAttrTransactionEnv* AttributeTransEnvInstanceLC(
            const CPEngSessionSlotId& aSlotId );


        /**
         * Gets a reference to Attribute Transaction environment.
         *
         * Attribute transaction environment provides services
         * to server side attribute transactions.
         *
         * Client must release instance by calling
         * MPEngTransactionFactory::Close() when it is no
         * more needed. If instance is needed just temporarily
         * (not as member), use AttributeTransFactoryInstanceLC()
         * and the instance is freed by CleanupStack.
         *
         * @since 3.0
         * @param aSlotId
         * @return Attribute transaction factory instance.
         */
        IMPORT_C static MPEngTransactionFactory* AttributeTransFactoryInstanceL(
            const CPEngSessionSlotId& aSlotId );
        IMPORT_C static MPEngTransactionFactory* AttributeTransFactoryInstanceLC(
            const CPEngSessionSlotId& aSlotId );



    public: // Debug support

        /**
         * Debug and test support for reference counted singletons.
         * @since 3.0
         */
        IMPORT_C static void __DbgObjectCounts( TInt& aHolder,
                                                TInt& aAttrMan,
                                                TInt& aTransEnv,
                                                TInt& aTransFact );

        IMPORT_C static void __DbgAttrManRefCount( const CPEngSessionSlotId& aSlotId,
                                                   TInt& aRefCount );

        IMPORT_C static void __DbgTransEnvRefCount( const CPEngSessionSlotId& aSlotId,
                                                    TInt& aRefCount );

        IMPORT_C static void __DbgTransFactRefCount( const CPEngSessionSlotId& aSlotId,
                                                     TInt& aRefCount );



    private: // Prohibited constructors and destructor

        PEngAttrLibFactory();
        ~PEngAttrLibFactory();

    };

#endif      // __PENGATTRLIBFACTORY_H

// End of File
