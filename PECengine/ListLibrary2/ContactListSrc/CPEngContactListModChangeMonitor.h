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
* Description:  Monitors contact list changes.
*
*/


#ifndef __CPENGCONTACTLISTMODCHANGEMONITOR_H__
#define __CPENGCONTACTLISTMODCHANGEMONITOR_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class MPEngContactListModStore;


// CLASS DECLARATION

/**
 * Monitor the contact list changes.
 * Stores changes which were commited on the network server
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListModChangeMonitor ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngContactListModChangeMonitor* NewL(
        MPEngContactListModStore& aStoreEntry );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListModChangeMonitor();



public: // New functions

    /**
     * Returns list of Contact Ids wich were added
     * to the contact lists with the last transaction
     *
     * @since 3.0
     * @return Array of the added Contact Ids
     */
    const MDesCArray& AddedContactIds() const;


    /**
     * Returns list of Contact Ids wich were removed from
     * the contact lists with the last transaction
     *
     * @since 3.0
     * @return Array of the removed Contact Ids
     */
    const MDesCArray& RemovedContactIds() const;


    /**
     * Returns count of added Contact Ids
     *
     * @since 3.0
     * @return count of Contact Ids
     */
    TInt CountAddedContactIds() const;


    /**
     * Returns count of removed Contact Ids
     *
     * @since 3.0
     * @return count of Contact Ids
     */
    TInt CountRemovedContactIds() const;


    /**
     * Find Contact Id in the Added Contact Ids
     *
     * @since 3.0
     * @param aContactId contact id to find
     * @param aUserDomain user domain
     * @return the position of found Contact Id, else KErrNotFound.
     */
    TInt FindContactIdInAdded( const TDesC& aContactId,
                               const TDesC& aUserDomain ) const;


    /**
     * Find Contact Id in the Removed Contact Ids
     *
     * @since 3.0
     * @param aContactId The Contact Id to find
     * @param aUserDomain user domain
     * @return the position of found Contact Id, else KErrNotFound.
     */
    TInt FindContactIdInRemoved( const TDesC& aContactId,
                                 const TDesC& aUserDomain ) const;

    /**
     * Externalizes whole presence list to stream
     *
     * @since 3.0
     * @param aStream Stream to write to.
     */
    void ExternalizeArrayL( RWriteStream& aStream ) const;


    /**
     * Internalizes whole presence list from stream
     *
     * @since 3.0
     * @param aStream stream to read from
     * @return
     */
    void InternalizeArrayL( RReadStream& aStream );


    /**
     * Add Contact Id to the list of added Contact Ids,
     * Does not leave if Contact Id already exists
     *
     * @since 3.0
     * @param aContactId to add to the list
     * @return KErrNone if it was OK, else system wide error code
     */
    TInt InsertAddedContactIdL( const TDesC& aContactId );

    /**
     * Remove Contact Id to the list of added Contact Ids
     *
     * @since 3.0
     * @param aContactId Contact Id to be removed
     * @return KErrNone if it was OK, else system wide error code
     */
    void RemoveAddedContactId( const TDesC& aContactId );


    /**
     * Reset content of the list of Added Contact Ids,
     *
     * @since 3.0
     */
    void ResetAddedContactId();


    /**
     * Add Contact Id to the list of removed Contact Ids,
     * Does not leave if Contact Id already exists
     *
     * @since 3.0
     * @param aContactId to add to the list
     * @return KErrNone if it was OK, else system wide error code
     */
    TInt InsertRemovedContactIdL( const TDesC& aContactId );


    /**
     * Remove Contact Id to the list of removed Contact Ids,
     *
     * @since 3.0
     * @param aContactId Contact Id to be removed
     * @return KErrNone if it was OK, else system wide error code
     */
    void RemoveRemovedContactId( const TDesC& aContactId );


    /**
     * Reset content of the list of removed Contact Id,
     *
     * @since 3.0
     */
    void ResetRemovedContactIds();


    /**
     * Reset the content of the list
     *
     * @since 3.0
     */
    void Reset();


protected:  // New functions

    /**
     * C++ constructor.
     */
    CPEngContactListModChangeMonitor(
        MPEngContactListModStore& aStoreEntry );


private:    // Data

    // OWN: List of added ContactIds
    CDesC16ArraySeg                 iAddedContactIds;

    // OWN: List of removed ContactIds
    CDesC16ArraySeg                 iRemovedContactIds;

    // REF: Store entry of the whole model
    MPEngContactListModStore&       iStoreEntry;

    };

#endif      // __CPENGCONTACTLISTMODCHANGEMONITOR_H__


//  End of File

