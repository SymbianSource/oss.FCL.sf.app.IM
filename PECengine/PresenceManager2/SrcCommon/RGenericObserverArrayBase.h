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

#ifndef __RGENERICOBSERVERARRAYBASE_H__
#define __RGENERICOBSERVERARRAYBASE_H__

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
 * Main idea behind the notify mediator is encapsulate
 * the observer array management from actual observer
 * type specific details. (Type specific details will
 * be implemented by the concrete mediator.) Usage
 * of mediator base interface allows generic
 * observer array implementation.
 *
 * @since 3.0
 */
class MGenObserverNotifyMediatorBase
    {
    public:

        /**
         * Observer notification.
         * This member is internal and not intended for use.
         */
        virtual void MediateNotifyL( TAny* aObserverToNotify ) = 0;


        /**
         * Observer notification from error.
         * This member is internal and not intended for use.
         */
        virtual void MediateNotifyError( TAny* aObserverToNotify, TInt aLeaveError ) = 0;


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
 * @since 3.0
 */
NONSHARABLE_CLASS( RGenericObserverArrayBase ) :
        private RPointerArrayBase   // CSI: 70 #
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
    void NotifyObservers( MGenObserverNotifyMediatorBase& aNotifyMediator );


    /**
     * Observer notify implementation.
     * This member is internal and not intended for use.
     */
    void NotifyErrorObservers( MGenObserverNotifyMediatorBase& aNotifyMediator,
                               TInt aError );

    /**
     * Observer notify implementation.
     * This member is internal and not intended for use.
     */
    TBool IsNotifyLoopRunning() const;


private: //private helpers

    /**
     * Private helper.
     * This member is internal and not intended for use.
     */
    void FinalizeObsArrayAfterNotifyLoop();


private: //Data

    //OWN: Flag describing the observer array state
    TBool iNotifyLoopRunning;
    };

#endif      //__RGenericObserverArrayBase_H__
//  End of File


