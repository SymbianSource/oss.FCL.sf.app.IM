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
* Description: Timers assemble for imps engine base function
*
*               CImpsIdleTimer
*               Idle timer to lauch Poll requests in CSP protocol.
*               CImpsPDPIdleTimer
*               Idle timer to close idle PDP-context (IAP).
*               CImpsPDPOpenTimer
*               Timer to open idle PDP-context.
*               CImpsShutdownTimer
*               Idle timer to lauch keep-alive and Poll requests in CSP protocol.
*
*/


#ifndef IDLETIMER_H
#define IDLETIMER_H

//  INCLUDES
#include "impssendreceive.h"

// FORWARD DECLARATIONS
class CImpsServer;
class MImpsCSPSession;

// CLASS DECLARATION


/**
* CImpsBaseTimer
* Base timer class for different timers
*/
class CImpsBaseTimer : public CActive
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority Active objects priority
        */
        CImpsBaseTimer( TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsBaseTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        virtual void Start( TInt aWaitSeconds );

        /**
        * Stop the timer
        */
        virtual void Stop( );

        /**
        * Reset the timer after message is sent
        */
        virtual void Reset();

    protected:
        // From base class
        void DoCancel();

    protected:
        RTimer          iTimer;
        TBool           iReset;
        TInt            iSeconds;
    };

/**
* CImpsIdleTimer
* Idle timer to lauch Poll requests in CSP protocol.
*/
class CImpsIdleTimer: public CImpsBaseTimer
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsIdleTimer( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsIdleTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        void Start( TInt aWaitSeconds );

        /**
        * Stop the timer
        */
        void Stop( );

    protected:
        // From base class
        void RunL();
    protected:  // data
        MImpsCSPSession&    iServer;
    };

/**
* CImpsPDPIdleTimer
* Idle timer to close idle PDP-context (IAP)
*/
class CImpsPDPIdleTimer: public CImpsBaseTimer
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsPDPIdleTimer( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsPDPIdleTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        void Start( TInt aWaitSeconds );

        /**
        * Stop the timer
        */
        void Stop( );

        /**
        * Reset the timer after message is sent
        */
        void Reset();

    protected:
        // From base class
        void RunL();
    protected:  // data
        MImpsCSPSession&    iServer;
    };

/**
* CImpsPDPOpenTimer
* Timer to open idle PDP-context
*/
class CImpsPDPOpenTimer: public CImpsBaseTimer
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsPDPOpenTimer( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsPDPOpenTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        void Start( TInt aWaitSeconds );

        /**
        * Stop the timer
        */
        void Stop( );

    protected:
        // From base class
        void RunL();
    protected:  // data
        MImpsCSPSession&    iServer;
    };


/**
* CImpsShutdownTimer
* Idle timer to lauch keep-alive and Poll requests in CSP protocol.
*/
class CImpsShutdownTimer: public CImpsBaseTimer
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsShutdownTimer( CImpsServer& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsShutdownTimer();

        /**
        * Start waiting.
        * Use this instead of base class Start(TTnt)
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        void Start( TInt aWaitSeconds ); //lint !e1411

        /**
        * Stop the timer
        */
        void Stop( );

    protected:
        // From base class
        void RunL();
    protected:  // data
        CImpsServer&    iServer;
    private:
        MImpsCSPSession*    iSess;
    };

/**
* CImpsExpiryTimer
* Timer to discard expired CSP requests
*/
class CImpsExpiryTimer: public CImpsBaseTimer
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsExpiryTimer(
            CImpsServer& aServer,
            TImpsEventType aType,
            TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsExpiryTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        * @param aWaitSeconds wait time in seconds
        */
        void Start( TInt aWaitSeconds );

        /**
        * Stop the timer
        */
        void Stop( );

    protected:
        // From base class
        void RunL();
    protected:  // data
        CImpsServer&    iServer;
    private:
        TImpsEventType  iType;

    };


/**
* CImpsSendQueued
* This sends so many queued messages to trasnport as possible.
*/
class CImpsSendQueued: public CActive
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        */
        CImpsSendQueued( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsSendQueued();

        /**
        * Start request for sending
        */
        void Send(  );


    protected:
        // From base class
        void RunL();
        void DoCancel();
    private:
        MImpsCSPSession&    iServer;
        TBool               iCanceled;

    };


#endif      // IDLETIMER_H

// End of File
