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
* Description:  One Storage folder management
*
*/



#ifndef __CPENGSTORAGEFOLDER_H__
#define __CPENGSTORAGEFOLDER_H__

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include "MPEngStorageFolder.h"
#include "TPEngStorageServerBTreeKeys.h"
#include "TPEngStorageServerMessages.h"

// FORWARD DECLARATIONS
class CPEngRequestHandler;
class RPEngMessage;
class RFs;
class CPEngSessionSlotState;
class CPEngSessionSlotEvent;

/**
 *  One Storage folder management
 *  Manages data in one storage folder
 *
 *  @since 3.0
 */
class CPEngStorageFolder : public CObject,
            public MPEngStorageFolder
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngStorageFolder* NewL( RFs& aFs,
                                         const TDesC& aTempFolder,
                                         RBuf16& aCommBuff );

        /**
         * Two-phased constructor.
         */
        static CPEngStorageFolder* NewLC( RFs& aFs,
                                          const TDesC& aTempFolder,
                                          RBuf16& aCommBuff );

        /**
         * Destructor.
         */
        virtual ~CPEngStorageFolder();

    public:	// Overwritten virtual functions of the CObject

        /**
         *	Close reference to this reference counted object
         *	if state of folder is active, do not delete it
         *
         *  @since 3.0
         */
        void Close ();

    public: // new from MPEngStorageFolder

        /**
         * Get data entry
         * @see MPEngStorageFolder
         */
        CPEngDataEntry* DataEntryL( const TDesC& aSId );


    public:	// New functions used by the Server

        /**
         *	Session slot state
         *
         *  @since 3.0
         *  @return returns session slot state
         */
        CPEngSessionSlotState& SessionSlotState();

        /**
         *	Set Session slot state
         *	Ownership of the parameter is transfered
         *
         *  @since 3.0
         *  @param	aState state of session, ownership is transfered
         */
        void SetSessionSlotState( CPEngSessionSlotState& aState );

        /**
         *	Set Session Slot base folder
         *	Ownership of the parameter is transfered
         *
         *  @since 3.0
         *  @param	aFolder des buffer with the base folder
         *			ownership is transfered
         */
        void SetSessionFolder( HBufC& aFolder );

        /**
         *	Commited folder session state update
         *	It does necessary cleaning of the folder
         *
         *  @since 3.0
         */
        void CommiteStateUpdate();


    public: // New Functions of Storage Engine

        /**
          *	Store store entry into the store
          *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type and buffer of store entry
         *	@param aNotifyActive flag if notification is active
         *		if ETrue, change if notified to listeners
         *	@return data entry which was stored
         */
        const CPEngDataEntry* WriteStoreEntryL(
            const RPEngMessage& aMessage,
            TBool aNotifyActive,
            TUint32 aSessionId,
            TInt aSubSessionId );

        /**
          *	Read store entry from the store
          *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type and buffer of store entry
         */
        void ReadStoreEntryL( const RPEngMessage& aMessage );

        /**
         *	Get store entry length
         *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type
         */
        void GetStoreEntryLengthL( const RPEngMessage& aMessage );

        /**
         *	Remove store entry from the store
         *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type
         */
        void RemoveStoreEntryL( const RPEngMessage& aMessage );

        /**
         *	Lock store entry in the store
         *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type
         */
        void LockStoreEntryL( const RPEngMessage& aMessage,
                              TUint32 aSessionId,
                              TInt aSubSessionId );

        /**
         *	Un Lock store entry in the store
         *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type
         */
        const CPEngDataEntry* UnlockStoreEntryL(
            const RPEngMessage& aMessage,
            TBool aNotifyActive,
            TUint32 aSessionId,
            TInt aSubSessionId );

        /**
         *	Un Lock store entry in the store
         *
         *	@since 3.0
         *	@param aMessage message from client, which carries
         *		Store ID, store type
         */
        void IsStoreEntryLockedL( const RPEngMessage& aMessage );

    public: // New Functions of Notification Engine

        /**
         *	Start listening of defined Storage IDs
         *
         *	@since 3.0
         *	@param aMessage message with request details
         *	@param aSessionId Session ID
         *	@param aSubSessionId Sub Session ID
         */
        void ListenSIDsChangesL( const RPEngMessage& aMessage,
                                 TUint32 aSessionId,
                                 TInt aSubSessionId );

        /**
          *	Update Listen Storage IDs Request handler with new SIDs
         *
         *	@since 3.0
         *	@param aMessage message with request details
         *	@param aSessionId Session ID
         *	@param aSubSessionId Sub Session ID
         */
        void UpdateListenSIDsScoutL( const RPEngMessage& aMessage,
                                     TUint32 aSessionId,
                                     TInt aSubSessionId );

        /**
         *	Fetch Changed Storage IDs Indexes
         *
         *	@since 3.0
         *	@param aMessage message with request details
         *	@param aSessionId Session ID
         *	@param aSubSessionId Sub Session ID
         */
        void FetchChangedIdsIndexesL( const RPEngMessage& aMessage,
                                      TUint32 aSessionId,
                                      TInt aSubSessionId );

        /**
          *	Reload Asynchronou request with new Message( TRequestStatus)
          *
         *	@since 3.0
         *	@param aMessage message with request details
         *	@param aSessionId Session ID
         *	@param aSubSessionId Sub Session ID
         */
        void ReloadAsyncScoutL( const RPEngMessage& aMessage,
                                TUint32 aSessionId,
                                TInt aSubSessionId );


        /**
         *
         *
         *	@since 3.0
         *	@param aSessionId Session ID
         *	@param aSubSessionId Sub Session ID
         */
        void CancelSubSessionRequests( TUint32  aSessionId,
                                       TInt aSubSessionId );

        /**
         *	Cancel defined pending asynchronous request
         *
         *	@since 3.0
         *	@param aSessionId session ID
         *	@param aSubSessionId sub-session ID
         *	@param aRequest request to be canceled
         */
        void CancelRequest( const RPEngMessage& aMessage,
                            TUint32  aSessionId,
                            TInt aSubSessionId );

        /**
         *	Notify about change of the Storage ID, except the
         *	session from which request came
         *
         *	@since 3.0
         *	@param aMessage message with request details
         *	@param aNotifyActive flag if notification is active
         *		   if ETrue, change if notified to listeners
         *	@return data entry which was stored
         */
        const CPEngDataEntry* NotifyChangedStorageIdL( const RPEngMessage& aMessage,
                                                       TBool aNotifyActive );


        /**
         *	Notify changed SIDs to the listening clients
         *
         *	@since 3.0
         *	@param aEntries
         *	@param aSessionId
         */
        void NotifySIDChanges( const RPointerArray<CPEngDataEntry>& aEntries );

    private: // New Private Functions of the class

        /**
          *	Find Request Handler
          *
          *	@since 3.0
          * 	@param aSessionId Session ID
          *	@param aSubSessionId Sub Session ID
          *	@param aFunction request function
          *	@return pointer to Request Handler it it exists or NULL
          */
        CPEngRequestHandler* FindRequestHandler(
            TUint32 aSessionId,
            TInt aSubSessionId,
            TPEngStorageServerMessages aFunction );

        /**
         *	Get Store Entry instance
         *	If request store Entry does not exists, it will be created
         *
         *	@since 3.0
         *	@param aMessage clients message with Store ID and store type
         *	@param aCreateIfNotPressent flag if store entry shall be
         *			created if it does not exists ( ETrue = Yes )
         *	@return pointer to the Data entry
         */
        CPEngDataEntry* GetStoreEntryL( const RPEngMessage& aMessage,
                                        TBool aCreateIfNotPressent,
                                        TBtreePos& aPos,
                                        TBool aTrueSearch = EFalse );

        /**
         *	Load Store entry and add it to the tree
         *	If store entry does not exists in the store, created it
         *	if flag is switched on
         *
         *	@since 3.0
         *	@param aKey key of the Storage Entry
         *	@param aCreateIfNotPressent flag if store entry shall be
         *			created if it does not exists ( ETrue = Yes )
         * 	@return pointer to the Data entry
         */
        CPEngDataEntry* LoadStoreEntryL( TDesC& aKey,
                                         TBool aCreateIfNotPressent,
                                         TBtreePos& aPos,
                                         const RPEngMessage& aMessage );

        /**
         * Notify observer that all observer SIds changed
         *
         * @since 3.0
         */
        void NofifyAllAboutAll();

        /**
         * Look in buffered data entries
         *
         */
        CPEngDataEntry* CheckBufferedDataEntries( const TDesC& aKey );

        /**
         * Add buffered data entry pointer
         */
        void AddBufferedDataEntry( CPEngDataEntry* aDataEntry );

    private:  // New private Functions of the class
        /**
         *	Clean B tree from all entries
         *	It will remove all entries from the B tree
         *
         *	@since 3.0
         */
        void CleanTreeFromAllEntriesL();

    private:

        /**
         * C++ default constructor.
         */
        CPEngStorageFolder( RFs& aFs,
                            const TDesC& aTempFolder,
                            RBuf16& aCommBuff );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

    private:    // Data

        /// REF: File server, owned by the server class
        RFs&											iFs;

        /// REF: Temp folder of the Storage
        const TDesC&									iTempFolder;

        /// REF: buffer used for communication, to minimize
        RBuf16&											iBuffer;


        /// OWN: Folder name, Hashed
        HBufC*											iSessionFolder;

        /// OWN: Session slot state, also has session Identification
        CPEngSessionSlotState*							iSessionState;

        /// OWN: B-tree with cached entries
        TBtreeFix<CPEngDataEntry*, TDesC*>				iDataEntryTree;

        /// OWN: Key for ordering of B-tree of cached entries
        TPEngDataEntryKey								iDataEntryTreeKey;

        /// OWN: Handles memory management for B-Tree
        CMemPagePool*									iPool;

        /// OWN: Flag if B-tree is connected
        TBool											iTreeConnected;

        /// OWN: Container with the Observers, owned and also members are owned
        CObjectCon*										iObservers;

        /// REF: Last used data entries
        CPEngDataEntry*									iBuffDataEntry0;
        CPEngDataEntry*									iBuffDataEntry1;
        CPEngDataEntry*									iBuffDataEntry2;
    };

#endif      // __CPENGSTORAGEFOLDER_H__

// End of File
