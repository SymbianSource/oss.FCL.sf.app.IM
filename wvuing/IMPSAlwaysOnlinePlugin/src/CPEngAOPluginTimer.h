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
* Description:  This class implements the waiting for next login retry feature
*				 for the Always Online Plugin
*
*/


#ifndef __CPENGAOPLUGINTIMER_H
#define __CPENGAOPLUGINTIMER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPEngAOWaitObserver;

// CLASS DECLARATION

/**
*  The CPEngAOPluginTimer class handles the waiting functionality for the
*  Always Online plugin.
*
*  @lib PEngAOPlugin.dll
*  @since 2.1
*/
class CPEngAOPluginTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEngAOPluginTimer* NewL();

        /**
        * Destructor.
        */
        virtual ~CPEngAOPluginTimer();

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
        * Sets the observer to which the notify about the timer events is
        * given.
        * @param aObserver The observer
        */
        void SetObserver( MPEngAOWaitObserver* aObserver );

        /**
         * This method is implemented to enable debugging in the call. It only
         * calls the CTimer::At method.
         * @param aTime Time when to complete the request
         */
        void At( const TTime& aTime );

    private:

        /**
        * C++ default constructor.
        */
        CPEngAOPluginTimer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        /// A pointer to the observer, not owned
        MPEngAOWaitObserver* iObserver;

        /// Reserved pointer for future extension
        TAny* iReserved;

        /// Used for resetting timer when time has set during
        /// timer active state
        TTime iTimerStarted;
        TTime iExpectedLaunchTime;
    };

#endif      // __CPENGAOPLUGINTIMER_H

// End of File
