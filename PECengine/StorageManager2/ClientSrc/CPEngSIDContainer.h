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
* Description:  Storage Id container
*
*/



#ifndef __CPENGSIDCONTAINER_H__
#define __CPENGSIDCONTAINER_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngSIDContainer.h"

// FORWARD DECLARATIONS
class MPEngSIDChangeObserver;

/**
 *  One storage Id container
 *  It hold reference to the all observers
 *	which are observing this storage Id
 *
 *  @lib PEngStorMan2.dll
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngSIdContainer ):
        public CBase,
        public MPEngSIdContainer
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSIdContainer* NewLC( const TDesC& aSId );

    /**
     * Destructor.
     */
    virtual ~CPEngSIdContainer();

public: // New functions of MPEngSIDContainer

    /**
     * Returns engaged SId
     * More info in <MPEngSIDContainer.h>
     * @since 3.0
     */
    const TDesC& SId() const;

    /**
     *	Add Observer container to the container
     *	More info in <MPEngSIDContainer.h>
     *	@since 3.0
     */
    TInt AddObserverContainer(
        const CPEngObserverContainer* aObserver );

    /**
     *	Remove Observer from the container
     *	More info in <MPEngSIDContainer.h>
     *	@since 3.0
     */
    void RemoveObserverContainer(
        const CPEngObserverContainer* aObserver );

public: // New functions

    /**
     *  Notify change of the storage Id to the observers
     *
     *  @since 3.0
     */
    void NotifySIdChangeL();

    /**
     *  SId published to the Server
     *
     *  @since 3.0
     *  @return ETrue if it was published
     */
    TBool SIdPublished() const;

    /**
     *  Set SId as published to the Server
     *
     *  @since 3.0
     *  @return ETrue if yes
     */
    void SetSIdPublished();

    /**
     *  Observers count
     *
     *  @since 3.0
     *  @return count of observers
     */
    TInt ObserverCount() const;

    /**
     *  Allocated free slots in array
     *
     *  @since 3.0
     *  @param count of new slots to be still allocated
     *  @param aDone number already allocated slots
     *  @return KErrNone if OK otherwise wise error code
     */
    TInt AllocateSlots( TInt aCount, TInt aDone = 0 );

private:

    /**
     * C++ default constructor.
     */
    CPEngSIdContainer();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC& aSId );

private:    // Data
    /// OWN: Store ID owned
    HBufC*									iSId;

    /// REF: Observer Container array
    RPointerArray<CPEngObserverContainer>	iObservers;


    /// Flag is SID was published to the server
    TBool									iPublished;
    };

#endif      // __CPENGSIDCONTAINER_H__

// End of File
