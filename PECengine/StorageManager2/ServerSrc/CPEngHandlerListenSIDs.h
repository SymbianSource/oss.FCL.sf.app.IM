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
* Description:  Listen Storage Id changes request handler
*
*/



#ifndef __CPENGHANDLERLISTENSIDS_H__
#define __CPENGHANDLERLISTENSIDS_H__

//  INCLUDES
#include <e32base.h>
#include "CPEngRequestHandler.h"
#include "MPEngHandlerListenSIDs.h"


// FORWARD DECLARATION
class CPEngDataEntry;
class MPEngStorageFolder;

/**
 *  Listen Storage Id changes request handler
 *  Implements Storage Id changes listen even handling
 *
 *  @since 3.0
 */
class CPEngHandlerListenSIDs :
            public CPEngRequestHandler,
            public MPEngHandlerListenSIDs
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngHandlerListenSIDs* NewLC(
            MPEngStorageFolder& aFolderManager,
            const RPEngMessage& aMessage,
            TUint32 aSessionID,
            TInt aSubSessionID );

        /**
         * Destructor.
         */
        virtual ~CPEngHandlerListenSIDs();

    public: // Function from CPEngHandlerListenSIDs

        /**
         * Notify client about changed Storage IDs
         *
         * @since 3.0
         * @param aChangedSIDs array of storage IDs which have changed
         */
        void NotifyChangedSIDs( const RPointerArray<CPEngDataEntry>& aEntries );


        /**
         * Notify client about all Sids changed
         *
         * @since 3.0
         */
        void NotifyAllSIdsChanged();


        /**
         * Entry was created
         *
         * @since 3.0
         */
        virtual void EntryCreatedL( CPEngDataEntry& aEntry );


    public: // new from CPEngRequestHandler

        /**
         * Reloads request handler with new RPEngMessage
          * @see <CPEngRequestHandler.h>
         */
        void ReloadScoutWithNewMessageL( const RPEngMessage& aNewScout );

    public: // New functions from  MPEngHandlerListenSIDs

        /**
         * Notify client about changed one Storage ID
          * @see <MPEngHandlerListenSIDs.h>
         */
        void EntryUpdated( const CPEngDataEntry& aEntry );

        /**
         * Entry is going to be deleted
         * @see <MPEngHandlerListenSIDs.h>
         */
        void EntryDeleted( const CPEngDataEntry& aEntry );

    private:  // New functions

        /**
         *	Notify client about about changes
         *	Function writes to the client side indexes of the changed SIDs and
         *	and completes RPEngMessage message
         *  @since 3.0
         *  @param aErrCode completed code
         *  @return
         */
        void NotifyClientAboutChanges( TInt aErrCode );

        /**
         *	Updates set of SIDs which client is interested in
         *	old set to SIDs is deleted
         *	If there is any fail, original Set it SIDs will stay intact
         *
         *  @since 3.0
         *  @param aMessage	message to be used for update
         */
        void UpdateClientsEngagedSIDsL( const RPEngMessage& aMessage );


        /**
         *  Fetch indexes of changes Storage IDs
         *
         *  @since 3.0
         *  @param aMessage	message to be used for update
         */
        void FetchChangedSidsL( const RPEngMessage& aMessage );



        /**
         * Clean Entries array
         *
         * @since 3.0
         */
        void CleanEntriesArrays();


        /**
         * Rebuild Entry array
         *
         * @since 3.0
         */
        void RebuildEntryArrayL( const MDesCArray& aSIds );


        /**
         * Get data entry and SId from given index
         *
         */
        CPEngDataEntry* DataEntry( TInt aIndex,
                                   TInt& aNotLoadedPtr,
                                   TPtrC& aSId );

        void AppendDataEntryL( const TDesC& aSid );
        void IndertDataEntryL( const TDesC& aSid,
                               TInt aPos,
                               TInt& aNotLdPos );

        void CleanEntriesArray( TInt aCount );
        void CleanSIdArray( TInt aCount );
        void CleanReturnArray( TInt aCount );

    private:

        /**
         * C++ default constructor.
         */
        CPEngHandlerListenSIDs( MPEngStorageFolder& aFolderManager,
                                TUint32 aSessionID,
                                TInt aSubSessionID );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( const RPEngMessage& aMessage );

    private:    // Data

        /// REF: Folder manager
        MPEngStorageFolder& 			iFolderManager;

        /// OWN: List of observed Store entries
        ///      Elements not owned.
        RPointerArray<CPEngDataEntry>   iEntries;

        /// OWN: List of store Ids of not loaded store entries
        ///      Elements owned
        RPointerArray<HBufC>			iNotLoaded;

        ///	OWN: Array where indexes of changed SIDs will be stored
        RArray<TInt>					iReturnArray;

    };

#endif      // __CPENGHANDLERLISTENSIDS_H__

// End of File
