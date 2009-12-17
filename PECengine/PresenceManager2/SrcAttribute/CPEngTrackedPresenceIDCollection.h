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
* Description:  Tracked Presence ID collection.
*
*/

#ifndef __CPENGTRACKEDPRESENCEIDCOLLECTION_H
#define __CPENGTRACKEDPRESENCEIDCOLLECTION_H

//  INCLUDES
#include <E32Base.h>
#include <BaDesCa.h>

//FORWARD DECLARATIONS
class CPEngTrackedPresenceID2;
class CPEngTrackedPresenceIDs2;
class CPEngTrackedPresenceIDEntry;
class CPEngTrackedPresenceIDEntryLookUp;
class TPEngTrackedPresenceEntryFilter;



/**
 * Tracked Presence ID collection.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngTrackedPresenceIDCollection ) : public CBase
    {
private: /* Enumerations */

    /**
     * Flags for list filtering
     *
     * @since 3.0
     */
    enum TFilterFlags
        {
        ENotIncludedToList = 0,
        EIncludedToList = 1
        };


    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngTrackedPresenceIDCollection object.
     *
     * @return New CPEngTrackedPresenceIDCollection instance.
     */
    static CPEngTrackedPresenceIDCollection* NewL();


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngTrackedPresenceIDCollection();



private:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngTrackedPresenceIDCollection();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



    //-----------------------------------------------------------------------
public: /* Interface */


    /**
     * Interface getter.
     *
     * @since 3.0
     * @return Reference to interface.
     */
    CPEngTrackedPresenceIDs2& Interface();




    //-----------------------------------------------------------------------
public: /* Tracked presence IDs */

    /**
     * Count of tracked presence IDs.
     *
     * @since 3.0
     * @return Count of tracked presence IDs.
     */
    TInt TrackedPresenceIDsCount() const;



    /**
     * Locates a tracked Presence ID entry by
     * Presence ID.
     *
     * @since 3.0
     * @param aPresenceID Presence ID for which to
     * locate tracked entry.
     * @return Presence ID entry.
     * Returns NULL if matching entry isn't found.
     */
    const CPEngTrackedPresenceIDEntry* FindTrackedPresenceID( const TDesC& aPresenceID ) const;


    /**
     * Gets tracked Presence ID entry by index.
     *
     * @since 3.0
     * @param aIndex The index.
     * @return Presence ID entry.
     */
    const CPEngTrackedPresenceIDEntry& TrackedPresenceID( TInt aIndex ) const;


    /**
     * Iterates to next tracked Presence ID and returns it
     * (returns the interface).
     *
     * If no more Presence IDs, returns NULL..
     *
     * @since 3.0
     * @Param Iteration start index.
     * Updates the index during iteration.
     * @return Iterated tracked Presence ID.
     */
    CPEngTrackedPresenceID2* NextTrackedPresenceID( TInt& aIterIx );


    /**
     * Adds (or updates) the given presence IDs to
     * have given tracked attributes.
     *
     * @since 3.0
     * @Param aPresenceIDs The Presence IDs to add or update.
     * @param aTypes Attributes to add or update.
     */
    void AddTrackedIdsL( const MDesCArray& aPresenceIDs,
                         const TArray<TUint32>& aTypes );

    /**
     * Removes the given presence IDs from the
     * list of tracked presence IDs.
     *
     * @since 3.0
     * @Param aPresenceIDs The Presence IDs to remove.
     */
    void RemoveTrackedIds( const MDesCArray& aPresenceIDs );


    /**
     * Removes the given attributes from the tracked presence
     * IDs. If Presence ID doesn't have after this anymore
     * tracked attributes, Presence ID is automaticly
     * removed from the list of tracked presence IDs.
     *
     * @since 3.0
     * @Param aTypes The attributes to remove.
     */
    void RemoveTrackedIds( const TArray<TUint32>& aTypes );


    /**
     * Removes the given attributes from the given presence
     * IDs. If Presence ID doesn't have after this anymore
     * tracked attributes, Presence ID is automaticly
     * removed from the list of tracked presence IDs.
     *
     * @since 3.0
     * @Param aPresenceIDs The presence IDs from which to remove.
     * @Param aTypes The attributes to remove.
     */
    void RemoveTrackedIds( const MDesCArray& aPresenceIDs,
                           const TArray<TUint32>& aTypes );

    /**
     * Removes all tracked Presence IDs.
     *
     * @since 3.0
     */
    void RemoveAllTrackedIds();



    //-----------------------------------------------------------------------
public: /* List of changed presences */


    /**
     * Removes change marks from
     * tracked Presence IDs.
     *
     * @since 3.0
     */
    void ResetPresenceChangeMarks();


    /**
     * Marks given Presence ID's given
     * attribute as changed.
     *
     * @since 3.0
     * @param aPresenceID The presence ID which to mark.
     */
    void MarkPresenceChange( const TDesC& aPresenceID );


    /**
     * Iterates to next changed Presence ID and returns it
     * (returns the interface).
     *
     * If no more Presence IDs, returns NULL..
     *
     * @since 3.0
     * @Param Iteration start index.
     * Updates the index during iteration.
     * @return Iterated tracked Presence ID.
     */
    CPEngTrackedPresenceID2* NextChangeMarkedPresenceID( TInt& aIterIx );


    //-----------------------------------------------------------------------
public: /* Filtered list */

    /**
     * Resets the filtered list.
     *
     * @since 3.0
     */
    void ResetFilteredList();


    /**
     * Adds CPEngTrackedPresenceIDEntry from
     * given index to filtered list.
     *
     * @since 3.0
     * @param aIndex The index to add.
     */
    void IncludeToFilteredList( TInt aIndex );


    /**
     * Gets a MDesCArray presenting the filtered
     * lists Presence IDs.
     *
     * @since 3.0
     * @return Filtered MDesCArray.
     */
    const MDesCArray& FilteredList() const;



    //-----------------------------------------------------------------------
public: /* Filtered list */


    /**
     * Resets all holded tracked Presence IDs
     * attribute track and change iterators.
     *
     * @since 3.0
     */
    void ResetTrackedEntriesIterators();



    //-----------------------------------------------------------------------
private: /* Data */

    //OWN: Exported class interface
    CPEngTrackedPresenceIDs2*    iInterface;

    //OWN: Attributes tracked for Presence ID
    RPointerArray< CPEngTrackedPresenceIDEntry >  iTrackedIDs;

    //OWN: Lookup key for Presence Id entry search
    CPEngTrackedPresenceIDEntryLookUp* iPresenceIdLookup;

    //OWN: Entry list filter
    TPEngTrackedPresenceEntryFilter* iEntryListFilter;

    };

#endif      //__CPENGTRACKEDPRESENCEIDCOLLECTION_H


// End of file

