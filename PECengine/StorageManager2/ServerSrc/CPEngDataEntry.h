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
* Description:  Implements one store entry of the storage server
*
*/



#ifndef __CPENGDATAENTRY_H__
#define __CPENGDATAENTRY_H__

//  INCLUDES
#include <e32base.h>

#include "PEngStorageGlobals.h"


//  FORWARD DECLARATIONS
class RFs;
class MPEngHandlerListenSIDs;

/**
 *  Implements one store entry of the storage server
 *  Store entries are collected by the Storage folders
 *  @since 3.0
 */

class CPEngDataEntry
            : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngDataEntry* NewL( RFs& aFs,
                                     const TDesC& aTempFolder,
                                     const TDesC& aKey,
                                     TPEngStorageType aType );

        /**
         * Two-phased constructor.
         */
        static CPEngDataEntry* NewLC( RFs& aFs,
                                      const TDesC& aTempFolder,
                                      const TDesC& aKey,
                                      TPEngStorageType aType );

        /**
         * Destructor.
         */
        virtual ~CPEngDataEntry();

    public: // New functions of the base class

        /**
          * Returns the data of this entry
          * Function leaves with KErrNotFound if data is NULL pointer
          * Otherwise not other leave is allowed
          *
          * @since 3.0
          * @return descriptor with data of store entry.
          */
        const TDesC8& DataL() const;

        /**
         * Returns the key of this entry.
         *
         * @since 3.0
         * @return descriptor with the key
         */
        const TDesC& Key() const;

        /**
         * Return pointer to key buffer, needed when ordering entries in B-tree.
         *
         * @since 3.0
         * @return pointer to key buffer.
         */
        HBufC** KeyBuffer();

        /**
         * Get Lock Status
         *
         * @since 3.0
         * @param aPriority priority of the lock status to check
         * @return lock status
         */
        TBool LockStatus( TPengStorageLockPriority aPriority ) const;

        /**
         * Get lock count
         * Get lock count, no matter the priority of the lock
         *
         * @since 3.0
         * @return count
         */
        TInt LockCount() const;

        /**
         *  Increase Lock count of if client is allowed
         *
         *  @since 3.0
         *  @param aSessionID ID of the Session command came from
         *  @param aSubSessionID ID of the Sub-session command came from
         *  @param aPriority priority of the lock command
         *
         */
        TInt IncreaseLockCountL( TInt32 aSessionID,
                                 TInt aSubSessionID,
                                 TPengStorageLockPriority aPriority );

        /**
         * Decrease Lock count of if client is allowed
         *
         * @since 3.0
         * @param aSessionID ID of the Session command came from
         * @param aSubSessionID ID of the Sub-session command came from
         */
        TInt DecreaseLockCountL( TBool aNotifyActive,
                                 TInt32 aSessionID,
                                 TInt aSubSessionID,
                                 CPEngDataEntry*& aNotifEntry );

        /**
         * Returns Current version of the store entry
         *
         * @since 3.0
         * @return current version number
         */
        TInt Version() const;

        /**
         * Returns Time of the last modification
         *
         * @since 3.0
         * @return time
         */
        const TTime& LastModification() const;

        /**
         * Asserts that store entry's storage type is required one.
         * If not, leaves with KErrArgument.
         *
         * @since 3.0
         * @param aType The supposed storage type.
         */
        void AssertStorageTypeL( TPEngStorageType aType ) const;




    public: // New functions of the base class with default implementation

        /**
         *  Set entry data, owner ship of the passed buffer is called by
         *  the function. Descriptor buffer is in the clean up stack
         *  and should be poped/destroyed when when it is not needed.
         *  If derived class defines own implementation of this function
         *  CheckVersionNumberL() should be called before any operation
         *  and UpdateVersionNumber() on the end of the new implementation
         *  before function returns to caller. See CheckVersionNumberL()
         *  and UpdateVersionNumber() description
         *
         *  Default implementation check Version, deletes old buffer,
         *  adopts new buffer, and updates version number
         *
         * @since 3.0
         * @param aData data buffer to be set to store entry
         *        function takes ownership of the buffer descriptor
         * @param aType data type for check it, also Version check
         *                  is determined from the type
         * @return if notification needs to be scheduled, return valid pointer
         */
        virtual const CPEngDataEntry*
        SetDataLX( HBufC8* aNewData,
                   TPEngStorageType aType,
                   TBool aNotifyActive,
                   TInt32 aSessionID,
                   TInt aSubSessionID );

        /**
         *  Deletes store entry from storage
         *  Derived version can do additional cleaning if needed
         *
         *  Default implementation removes permanent part of the store
         *  entry, if there is, from the file system
         *  @since 3.0
         */
        virtual void DeleteStoreEntry();


        /**
         * Set hashed path of the store entry
         * Function takes ownership of the passed des buffer
         *
         * @since 3.0
         * @param aPath new hached path
         */
        virtual void SetHashedPath( HBufC* aPath );

        /**
         * Initializes store entry. This function checks the type
         * of thes store entry and load data from disc if needed
         * This call always follows construction of the store entry
         * before store entry si added to the tree.
         * If store entry does not exists (for example for permanents entry)
         * , function returns EFalse, so caller is informed that
         * this entry has not meaning to be held if some data will not be
         * stored into it
         *
         * @return ETrue if store entry has content
         * @since 3.0
         */
        virtual TBool InitializeStoreEntryL();

    public: // new functions for observing support

        /**
         *
         * @since 3.0
         */
        void AddObserverL( MPEngHandlerListenSIDs& aObserver );

        /**
         *
         * @since 3.0
         */
        void RemoveObserver( MPEngHandlerListenSIDs& aObserver );


    protected: // help functions of base class

        /**
         *  Checks version number of passed data and internal buffer
         *  if they do not match.
         *  and function leave with KErrAccessDenied
         *  Version control flag is taken in account
         *
         *  @since 3.0
         *  @param aNewData data buffer.
         *  @param aVersionControl flag is version if checked ETrue = yes
         */
        void CheckVersionNumberL( const TDesC8& aNewData, TPEngStorageType aType );

        /**
         * Checks if passed sub-session is correct or not, in case lock
         * is active. Function leave with KErrLocked if passed information
         * does not match if lock was activated from passed sub-session,
         * function does nothing
         *
         * @since 3.0
         * @param aSessionID session
         * @param aSubSessionID sub-session
         */
        void CheckLockSubSessionL( TInt32 aSessionID, TInt aSubSessionID );

        /**
         * Updates version number of the Store Entry
         * This suppose to be called when SetDataL function is called
         *
         * @since 3.0
         * @param aData is data buffer.
         */
        void UpdateVersionNumber();

    protected:  // private constructors

        /**
        * C++ default constructor.
        */
        CPEngDataEntry( RFs& aFs,
                        const TDesC& aTempFolder,
                        TPEngStorageType aType );

        /**
        * Symbian second phase constructor
        */
        void BaseConstructL( const TDesC& aKey );

    private:    // new private functions

        void UpdatePermanentPartL( const TDesC8& aNewData );

        void TrySeekPermanentPartL( RReadStream& aStream, TInt& aPermanentDataLength ) const;


        HBufC8* PermanentFromFileOrNullL() const;

        void NotifyUpdate();

    protected:   // Protected Data to allow derivation

        /// REF: File server reference
        RFs&                            		iFs;

        /// REF: Temp folder of the storage
        const TDesC&                    		iTempFolder;

        /// OWN: Storage Type
        const TPEngStorageType          		iType;

        /// OWN: Storage ID of the Storage Entry
        HBufC*                          		iKey;

        /// OWN: Hashed Store ID with the path
        HBufC*                          		iHashedPath;

        /// OWN: Data of the stored entry
        HBufC8*                         		iData;

        /// OWN: Last time entry was modified
        TTime                           		iModified;

        /// OWN: Version count of the the entry ( used for confirmation)
        TInt32                          		iVersion;

        /// OWN: Flag if entry is locked, ETrue = Entry locked
        TInt                            		iLockCount;

        /// OWN: Lock Session
        TInt                            		iLockSession;

        /// OWN: Lock Sub-session
        TInt                            		iLockSubSession;

        /// OWN: Lock Priority
        TPengStorageLockPriority        		iLockPriority;

        /// OWN: List of observers
        ///      Entries not owned
        RPointerArray<MPEngHandlerListenSIDs>  	iObservers;
    };

#endif
