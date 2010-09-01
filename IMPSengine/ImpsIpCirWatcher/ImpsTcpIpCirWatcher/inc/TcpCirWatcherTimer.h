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
* Description: Timer for Tcp CirWatcher.
*
*
*/


#ifndef __TCPCIRWATCHERTTIMER_H__
#define __TCPCIRWATCHERTTIMER_H__

const TUint KWaitForNextPing                        = 2000;
const TUint KWaitForPingReply                       = 2001;
const TUint KWaitForNextConnectAttempt              = 2002;
const TUint KWaitForFirstConnectAttempt             = 2003;
const TUint KWaitForHELOSendAttempt                 = 2004;
const TUint KWaitForHELOResponse                    = 2005;

// INCLUDES
#include <e32base.h>
#include "TcpCirWatcherTimerCallback.h"

// CLASS DECLARATION
class CTcpCirWatcherTimer : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aCallback A pointer to MImpsTransportTimerCallback class
        * @param aRequester A pointer to the object that starts this timer
        * @param aLogging Has logging been enabled
        * @return CImpsCirWatcherTimer
        */
        static CTcpCirWatcherTimer* NewL( MTcpCirWatcherTimerCallback* aCallback );

        /**
        * Destructor.
        */
        virtual ~CTcpCirWatcherTimer();

    public: // New functions

        /**
        * Activates the timer
        * @param aDelay Expiry time in microseconds
        */
        void ActivateTimer( const TTimeIntervalMicroSeconds32 aDelay,
                            const TInt aCurrentTask );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ constructor.
        * @param aCallback A pointer to MImpsCirWatcherTimerCallback class
        * @param aRequester A pointer to the object that starts this timer
        * @param aLogging Has logging been enabled
        */
        CTcpCirWatcherTimer( MTcpCirWatcherTimerCallback* aCallback );

        /**
        * From CActive.
        */
        void RunL();

        /**
        * From CActive.
        */
        void DoCancel();

    private:    // Data

        TUint                                   iCurrentTask;
        RTimer							        iTimer;
        MTcpCirWatcherTimerCallback*	        iCallback;
    };

#endif      //__IMPSCIRWATCHERTTIMER_H__

// End of File
