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
* Description:  Presence Store Entry adapter for presence attributes.
*
*/

#ifndef CPENGMODELSTOREENTRYADAPTER_H
#define CPENGMODELSTOREENTRYADAPTER_H

//  INCLUDES
#include "CPEngStoreEntry.h"


//FORWARD DECLARATIONS
class CPEngPresenceAttrModel;
class MPEngStorageManager;



// CLASS DECLARATION
/**
 * Presence Store Entry adapter for presence attributes.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngModelStoreEntryAdapter ) : public CPEngStoreEntry
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngModelStoreEntryAdapter* NewL(
        HBufC* aStoreId,
        MPEngStorageManager& aStorageManager,
        TPEngStorageType aStorageType,
        CPEngPresenceAttrModel& aAttributeModel );


    /**
     * Destructor.
     */
    virtual ~CPEngModelStoreEntryAdapter();


private: // Constructors.

    /**
     * C++ default constructor.
     */
    CPEngModelStoreEntryAdapter( TPEngStorageType aStorageType,
                                 HBufC* aStoreId,
                                 CPEngPresenceAttrModel& aAttributeModel );

public: // New access functions

    /**
     * Access to Storage Manager used by the
     * CPEngModelStoreEntryAdapter.
     *
     * Both const and non const overload.
     * @return Reference to MPEngStorageManager.
     */
    MPEngStorageManager& StorageManager();
    const MPEngStorageManager& StorageManager() const;



private: // Functions from the CPEngStoreEntry

    /**
     * From CPEngStoreEntry.
     *
     * Externalizes the referenced
     * CPEngPresenceAttrModel data to Presence Storage.
     *
     * @since 3.0
     * @param aStream stream to write to
     * @param aStorageType storage type to externalized
     */
    void ExternalizeL( RWriteStream& aStream,
                       TPEngStorageType aStorageType ) const;

    /**
     * From CPEngStoreEntry.
     *
     * Externalizes the referenced
     * CPEngPresenceAttrModel data to Presence Storage.
     *
     * @since 3.0
     * @param aStream stream to read from
     * @param aStorageType storage type to internalized
     */
    void InternalizeL( RReadStream& aStream,
                       TPEngStorageType aStorageType );

    /**
     * From CPEngStoreEntry.
     *
     * @since 3.0
     * @return ID.
     */
    const TDesC& StorageId() const;


    /**
     * From CPEngStoreEntry.
     *
     * Returns required storage size for
     * storing this entry to Presence Storage.
     *
     * @since 3.0
     * @return Size.
     */
    TUint32 EntrySize() const;



private:    // Data

    //OWN: Store id for storing this entry.
    HBufC*  iStoreId;

    //REF: Attribute model to store
    CPEngPresenceAttrModel&     iAttributeModel;
    };


#endif      // CPENGMODELSTOREENTRYADAPTER_H

// End of File
