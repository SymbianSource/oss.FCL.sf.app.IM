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
* Description:  Timer implementation to wait multiple refresh.
*
*/


#ifndef CCAREFRESHTIMER_H
#define CCAREFRESHTIMER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MCARefreshTimerObserver;


// CLASS DECLARATION

/**
*  The CCARefreshTimer class handles the waiting functionality for the
*  storage manager to wait before calling observer in case of multiple
*  contacts adding.
*
*  @lib CAStorage.dll
*  @since 2.8
*/
class CCARefreshTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aObserver The observer to which the notify about the
        *                  timer events is
        * given.
        */
        static CCARefreshTimer* NewL( MCARefreshTimerObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CCARefreshTimer(  );

    protected:  // Functions from CTimer

        /**
        * @see CTimer
        */
        void RunL();

        /**
        * @see CTimer
        */
        TInt RunError( TInt aError );

    public: // new methods

        /**
         * Starts timer. If timer is already running, stops timer and
         * restarts it.
         */
        void Start( );

    private:

        /**
        * C++ default constructor.
        */
        CCARefreshTimer( MCARefreshTimerObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        // A pointer to the observer
        MCARefreshTimerObserver* iObserver;			/// Not owned
    };

#endif      // CCAREFRESHTIMER_H

// End of File
