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
* Description:  Generic observer array.
*
*/

#ifndef __RGENERICOBSERVERARRAYBASE_H
#define __RGENERICOBSERVERARRAYBASE_H

//  INCLUDES
#include <E32Base.h>



// CLASS DECLARATION
/**
 * Observer notify mediator base.
 *
 * Base interface to manage observer notifications
 * and notify errors. Typical implementation will
 * forward notifications to actual type specific
 * mediator.
 *
 *
 * the observer array management from actual observer
 * type specific details. (Type specific details will
 * be implemented by the concrete mediator.) Usage
 * of mediator base interface allows geneic
 * observer array implementation.
 *
 * @since 2.1
 */

NONSHARABLE_CLASS( MGenObserverNotifyMediatorBase )
    {
public:

    /**
     * Observer notification.
     * This member is internal and not intended for use.
     */
    virtual void MediateNotifyL( TAny* aObserverToNotify, TAny* aNotifyData ) = 0;

    /**
     * Observer notification from error.
     * This member is internal and not intended for use.
     */
    virtual void MediateNotifyError( TAny* aObserverToNotify, TInt aLeaveError ) = 0;


    /**
     * Observer notification from error.
     * This member is internal and not intended for use.
     */
    virtual void MediateError( TAny* aObserverToNotify, TInt aError ) = 0;


protected:

    /**
     * Protected destructor.
     */
    virtual ~MGenObserverNotifyMediatorBase() {}
    };




// CLASS DECLARATION
/**
 * Generic observer array.
 *
 * Generic observer array to manage observers
 * and notify them. Observers are managed as
 * typeless TAny pointers. Notification is done
 * trough generic MGenObserverNotifyMediatorBase
 * notify mediator.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( RGenericObserverArrayBase ) : private RPointerArrayBase
    {
protected: //protected members for derived classes

    /**
     * C++ constructor.
     * This member is internal and not intended for use.
     */
    RGenericObserverArrayBase();

    /**
     * Closes the array and frees all memory allocated to it.
     * This member is internal and not intended for use.
     */
    void Close();

    /**
     * Adds observer.
     * This member is internal and not intended for use.
     */
    TInt AddObserver( const TAny* aObserver );

    /**
     * Removes observer.
     * This member is internal and not intended for use.
     */
    TInt RemoveObserver( const TAny* aObserver );

    /**
     * Gets observer count.
     * This member is internal and not intended for use.
     */
    TInt Count();

    /**
     * Observer notify implementation.
     * This member is internal and not intended for use.
     */
    void NotifyObservers( MGenObserverNotifyMediatorBase& aNotifyMediator,
                          TAny* aNotifyData );

    /**
     * Observer notify implementation.
     * This member is internal and not intended for use.
     */
    void NotifyObserversFromError( MGenObserverNotifyMediatorBase& aNotifyMediator,
                                   TInt aError );

private: //private helpers

    /**
     * Private helper.
     * This member is internal and not intended for use.
     */
    void FinalizeObsArrayAfterNotifyLoop();


private: //Data
    ///< Flag describing the observer array state, owned
    TBool iNotifyLoopRunning;
    };

#endif      //__RGENERICOBSERVERARRAYBASE_H
//  End of File


