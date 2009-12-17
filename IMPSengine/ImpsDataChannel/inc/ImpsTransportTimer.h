/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Timer for Imps Transport.
*
*
*/


#ifndef IMPSTRANSPORTTIMER_H
#define IMPSTRANSPORTTIMER_H

// INCLUDES
#include <e32base.h>
#include "ImpsTransportTimerCallback.h"

class TImpsTimingRequester;
// CLASS DECLARATION

class CImpsTransportTimer : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aCallback A pointer to MMmsViewerTimerCallback class
        */
        static CImpsTransportTimer* NewL( MImpsTransportTimerCallback* aCallback,
                                          TImpsTimingRequester* aRequester );

        /**
        * Destructor.
        */
        virtual ~CImpsTransportTimer();

    public: // New functions

        /**
        * Request to activate timer
        * @param aDelay Time in microseconds after which
        *               the callback function is called.
        */
        void ActivateTimer( TTimeIntervalMicroSeconds32 aDelay );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ constructor.
        * @param aCallback A pointer to MImpsTransportTimerCallback class
        * @param aRequester Who constructed and/or activated the timer
        * @param aLogging Trace or not
        */
        CImpsTransportTimer( MImpsTransportTimerCallback* aCallback,
                             TImpsTimingRequester* aRequester );

        /**
        * From CActive.
        * @return void
        */
        void RunL();

        /**
        * From CActive.
        * @return void
        */
        void DoCancel();

    private:    // Data

        RTimer                                  iTimer;
        TImpsTimingRequester*                   iRequester;
        MImpsTransportTimerCallback*            iCallback;
    };

#endif      //IMPSTRANSPORTTIMER_H

// End of File
