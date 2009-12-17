/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish & Subscribe property change observer
*
*/


#ifndef CCAPROPERTYOBSERVER_H
#define CCAPROPERTYOBSERVER_H

//  INCLUDES
#include <e32base.h>        // CActive
#include <e32property.h>    // RProperty

#include "MCAPropertyNotificationObserver.h"


// CLASS DECLARATION

/**
*  Implementation of the Publish & Subscribe property change observer
*
*  @lib MGXMediaFileAPI.lib
*  @since 2.6
*/
NONSHARABLE_CLASS( CCAPropertyObserver ) : public CActive
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aObserver Observer to be informed when observed
    *                  property changes
    */
    static CCAPropertyObserver* NewL(
        MCAPropertyNotificationObserver& aObserver );

    /**
    * Destructor.
    */
    virtual ~CCAPropertyObserver();

public:     // New functions

    /**
    * Starts observing the given key for any changes
    * Notifies the observer when the property changes
    * @since 2.6
    * @param aCategory, Category of the observed property
    * @param aKey, Observed property
    */
    void ObservePropertyChangeL( TUid aCategory, TUint aKey );

    /**
    * Stops observing the key given with call ObserverPropertyChangeL
    */
    void CancelObserve( );

    /**
    * Gets the uid this observer is observing
    * @return the category
    */
    TUid Category();

    /**
    * Gets the uid this observer is observing
    * @return the key
    */
    TUint Key();


protected:  // Functions from base classes

    /**
    * From CActive
    */
    void RunL();

    /**
    * From CActive
    */
    void DoCancel();

private:

    /**
    * C++ default constructor.
    */
    CCAPropertyObserver( MCAPropertyNotificationObserver& aObserver );


private:    // Data

    // Owns: Observed property
    RProperty iProperty;

    // Observer to be informed when property changes
    MCAPropertyNotificationObserver& iObserver;

    // the category this observer is attached to
    TUid iCategory;
    // the key this observer is attached to
    TUint iKey;

    };

#endif      // CCAPROPERTYOBSERVER_H

// End of File
