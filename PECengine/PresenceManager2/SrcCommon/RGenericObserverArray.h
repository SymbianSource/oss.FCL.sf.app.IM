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
* Description:  Thin templated RGenericObserverArray.
*
*/

#ifndef __RGENERICOBSERVERARRAY_H__
#define __RGENERICOBSERVERARRAY_H__

//  INCLUDES
#include <E32Base.h>
#include "RGenericObserverArrayBase.h"



// CLASS DECLARATION
/**
 * Strictly typed observer notify mediator.
 *
 * Template typed interface to manage observer notifications
 * and notify errors. Typical mediator implementation
 * should notify given observer with type specific way.
 *
 * Notify mediator encapsulates the observer array management
 * (done by RGenericObserverArray) from observer type specific
 * notify details (implemented by concrete notify mediator).
 *
 * @since 3.0
 */
template < class T >
class MGenObserverNotifyMediator
    {
    public:

        /**
         * Observer notification.
         *
         * Template method to implement observer notification.
         * If notification handling leaves, the leave error
         * is reported back for the to same observer using the
         * MediateNotifyError() method.
         *
         * @since 3.0
         * @param aObserverToNotify The observer to notify.
         * @param aNotifyData Data to use in notification.
         */
        virtual void MediateNotifyL( T& aObserverToNotify ) = 0;


        /**
         * Observer notification from error.
         *
         * Reports the leave error propagated from
         * previous MediateNotifyL() call, back to mediator.
         *
         * @since 3.0
         * @param aObserverToNotify The notified observer which leaved.
         * @param aLeaveError The propagated leave code.
         */
        virtual void MediateNotifyError( T& aObserverToNotify, TInt aLeaveError ) = 0;



    protected:

        /**
         * Protected destructor.
         */
        virtual ~MGenObserverNotifyMediator() {}
    };




// CLASS DECLARATION
/**
 * Type templated generic observer array to manage
 * and notify observers.
 *
 * Generic observer array to manage observers and notify them
 * Adds type safety with thin template to generic
 * RGenericObserverArrayBase. Observer notification is done
 * trough generic MGenObserverNotifyMediatorBase notify
 * mediator. Array functionality supports observer remove and
 * add during the notify loop. Array doesn't own registered
 * observers.
 *
 * @since 3.0
 */
template < class T >
class RGenericObserverArray :
            private RGenericObserverArrayBase       // CSI: 70 #
    {
    public: //Constructor / destructor.

        /**
         * C++ constructor.
         */
        inline RGenericObserverArray();


        /**
         * Closes the array.
         *
         * Closes the array and frees all memory allocated to it.
         * The function must be called before this array object
         * goes out of scope.
         *
         * NOTE!
         * The function does not delete the observers whose pointers
         * are contained in the array.
         */
        void Close();


    public:  //Methods to maintain observers


        /**
         * Adds observer, handles errors by returning
         * error code.
         *
         * Adds observer, observer must point to valid
         * object. If aObserver is NULL, returns KErrArgument.
         *
         * @since 3.0
         * @see AddObserverL
         * @param aObserver The observer to add.
         * @return KErrNone is successful.
         * KErrNoMemory if out of memory.
         * KErrArgument if aObserver is NULL.
         */
        inline TInt AddObserver( const T* aObserver );


        /**
         * Adds observer, handles errors by leaving.
         *
         * Adds observer, observer must point to valid
         * object. If aObserver is NULL, leaves with
         * KErrArgument.
         *
         * @since 3.0
         * @see AddObserver
         * @param aObserver The observer to add.
         */
        inline void AddObserverL( const T* aObserver );


        /**
         * Removes observer.
         *
         * @since 3.0
         * @param aObserver The observer to remove.
         * @return KErrNone if observer was found, else KErrNotFound.
         */
        inline TInt RemoveObserver( T* aObserver );


        /**
         * Observer count.
         *
         * Gets observer count.
         *
         * @since 3.0
         * @return Current observer count.
         */
        inline TInt Count();


    public:  //Methods to notify observers

        /**
         * Observer notify implementation.
         *
         * Notifies registered observers using given
         * observer notify mediator. Prior notify
         * loop protects the internal observer array
         * state against loop time observer removing.
         *
         * @since 3.0
         * @param aNotifyMediator The notify mediator to
         * use to notify the observers. Usage of mediator
         * separates the real notify details from observer
         * array managing details.
         */
        inline void NotifyObservers( MGenObserverNotifyMediator< T >& aNotifyMediator );




        /**
         * Observer notify implementation.
         *
         * Notifies registered observers using given
         * observer notify mediator from given error.
         * Prior notify loop protects the internal
         * observer array state against loop time
         * observer removing.
         *
         * @since 3.0
         * @param aNotifyMediator The notify mediator to
         * use to notify the observers. Usage of mediator
         * separates the real notify details from observer
         * array managing details.
         * @param aError The error to mediate to error observers.
         */
        inline void NotifyErrorObservers( MGenObserverNotifyMediator< T >& aNotifyMediator,
                                          TInt aError );



        /**
         * Checks is the notify loop running.
         *
         * @return ETrue if notify loop is running. Else EFalse.
         */
        inline TBool NotifyLoopRunning() const;

    };


// Inline methods
#include "RGenericObserverArray.inl"

#endif      //__RGENERICOBSERVERARRAY_H__
//  End of File


