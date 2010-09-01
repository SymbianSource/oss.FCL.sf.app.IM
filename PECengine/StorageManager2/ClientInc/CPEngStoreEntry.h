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
* Description:  Store entry abstract class
*
*/



#ifndef __CPENGSTOREENTRY_H__
#define __CPENGSTOREENTRY_H__

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

#include "PEngStorageGlobals.h"
#include "MPEngSIDChangeObserver.h"

// FORWARD DECLARATIONS
class MPEngStorageManagerWatcher;
class MPEngStorageManager;
class CPEngStorageManager;

// CLASS DECLARATION
/**
 *  One store entry template
 *  Each store entry of the storage manager has to be derived from
 *	this abstract class.
 *	Pure virtual function must be implemented by derived class.
 *	However virtual functions have default implementation.
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */
class CPEngStoreEntry :
            public CBase,
            public MPEngSIDChangeObserver
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CPEngStoreEntry();

    protected:	// Protected constructor to allow derivation

        /**
         * C++ default constructor.
         *
         * @param aStoragetype	storage type of the Store Entry
         */
        IMPORT_C CPEngStoreEntry( TPEngStorageType aStorageType );

        /**
         * Base constructor of the Base class
         * It registeres store entry to listen its changes
         *
         * @since 3.0
         * @param aStorageManager Storage manager
         * @param aRegister register store entry to observer its updates
         */
        IMPORT_C void BaseConstructL(
            MPEngStorageManager& aStorageManager,
            TBool aRegister = ETrue  );

        /**
         * Base constructor of the Base class
         * Overloaded version which does not register to listen changes
         *
         * @since 3.0
         */
        IMPORT_C void BaseConstructL();

    public:	// Clean up support for the Lock of the Entry

        /**
         * Release lock of the store entry
         * It is also used for clean up stack of the entry's lock support
         *
         * @since 3.0
         */
        IMPORT_C virtual void Release();

        /**
         * Lock store entry and leave lock on the clean up stack
         *
         * @since 3.0
         * @return count of the lock on the server side or KErrDisconected
         */
        IMPORT_C TInt LockLC();

        /**
         * Locks store entry
         *
         * @since 3.0
         * @param aPriority priority of the Lock to be done
         * @param aCreateEntry if store entry does not exists, it is created
         *         default value is ETrue = store entry is created if needed
         * @return count of the lock on the server side, KErrDisconected or
         *       KErrNotFound if aCreateEntry = EFalse and entry does not exist.
         */
        IMPORT_C TInt Lock( TPengStorageLockPriority
                            aPriority = EStorageLockLevelBasic,
                            TBool aCreateEntry = ETrue ) const;

        /**
         * Unlocks store entry
         *
         * @since 3.0
         * @return count of the lock on the server side or KErrDisconected
         */
        IMPORT_C TInt Unlock() const;

        /*
         * Stops sending of SID changes done through Storage Manager
         * More in <MPEngStorageManager.h>
         *
         * @since 3.0
         * @return positive number represents number on the server side
         *		counter, negative some system wide error
         */
        IMPORT_C TInt BufferServerSideNotifications();


        /*
         * Decreases counter of the Server Side Notification buffering,
          * More in <MPEngStorageManager.h>
         *
         * @since 3.0
         * @return positive number represents number on the server side
         *		counter, negative some system wide error
         */
        IMPORT_C TInt ReleaseServerSideBuffering();


        /**
         * Store this store entry
         *
         * @since 3.0
         */
        IMPORT_C void StoreL();

    public: // New functions of the base class

        /**
         * ExternalizeL provides way to externalize data from store entry.
         * For behavior of this function based on the different
         * storage type check <PEngStorageGlobals.h>
         *
         * @since 3.0
         * @param aStream stream to write to
         * @param aStorageType storage type to externalized
         */
        virtual void ExternalizeL( RWriteStream& aStream,
                                   TPEngStorageType aStorageType ) const = 0;

        /**
         * InternalizeL provides way to internalise data to store entry.
         * For behavior of this function based on the different
         * storage type check <PEngStorageGlobals.h>
         *
         * @since 3.0
         * @param aStream stream to read from
         * @param aStorageType storage type to internalized
         */
        virtual void InternalizeL( RReadStream& aStream,
                                   TPEngStorageType aStorageType ) = 0;

        /**
         * This method is used to retrieve ID for storing.
         *
         * @since 3.0
         * @return Store Id
         */
        virtual const TDesC& StorageId() const = 0;

        /*
         * This method is used to ask entry size in bytes.
         * It is called before actual call of the InternalizeL and
         * ExternalizeL to get estimation of the buffer needed for the
         * operation. If Size is incorrect before the ExternalizeL,
         * externalize operation will fail to fit data in the stream.
         * If size is incorrect before InternalizeL, operation will
         * be more time consuming (extra client/server)call involved
         *
         * For more behavior of this function based on the different
         * storage type check <PEngStorageGlobals.h>
         *
         * @since 3.0
         * @return size as TInt
         */
        virtual TUint32 EntrySize() const = 0;

    public: // Storage type support

        /**
         * Returns storage type
         *
         * @since 3.0
         * @return one of the enum values of type TPEngStorageType
         */
        IMPORT_C TPEngStorageType StorageType() const;

        /**
         * Change storage type
         * When storage type is changes, this store entry should be
         * valid. Fisrt data are clean from the storage server and
         * then storage entry is stored with new type.
         * This function is available when
         * BaseConstructL( MPEngStorageManager& aStorageManager  )
         * was called.
         *
         * @since 3.0
         * @param aNewType ne storage type
         */
        IMPORT_C void ChangeStorageTypeL( TPEngStorageType aNewType );


    public: // New functions of MPEngSIDChangeObserver with default implementation

        /**
         * Handle change of the Storage IDs
         * More information can be found in <MPEngSIDChangeObserver.h>
         *
         * @since 3.0
         */
        IMPORT_C virtual void HandleSIDsChangeL( CPtrCArray& aChangedSIDs );

        /**
         * Called when SIDs notifier fails
         * More information can be found in <MPEngSIDChangeObserver.h>
         *
         * @since 3.0
         */
        IMPORT_C virtual void HandleSIDNotifyError( TInt aError );

    protected:  // Data protected to allow derivation

        /// OWN: Reference accessed Storage Manager
        MPEngStorageManager*					iStorageManager;

        /// OWN: Size of the store entry, updated by the storage manager
        TInt									iSize;

    private:    // Data, not visible to derived classes

        /// OWN: Storage Type, cannot be modified by derived class
        TPEngStorageType						iStorageType;

        /// Last Version of the Store retrieved from Storage Manager
        TInt32									iEntryVersion;

        friend class CPEngStorageManager;       // CSI: 36 #
    };

#endif      // __CPENGSTOREENTRY_H__

// End of File


