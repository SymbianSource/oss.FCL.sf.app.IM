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
* Description: 
* AO class for alive timer. 
*
*/


#ifndef CImpsAliveManager_H
#define CImpsAliveManager_H


//  INCLUDES
#include <e32base.h>
#include "impsservercommon.h"

// CONSTANTS
const TInt KImpsMaxAliveRetryOnce = 2;   // max retries in one round
const TInt KImpsMaxAliveScheduleErr = 1; // nbr of scheduled rounds may fail
const TInt KUseAfterLimit = 120;         // internal aux contant


// DATA TYPES
enum TImpsAliveState
    {
    EImpsAliveIdle,      // Timer not started
    EImpsAliveActive,    // Timer started
    EImpsAlivePending,   // Waiting response
    };

// FORWARD DECLARATIONS
class MImpsCSPSession;
class CImpsAliveManager;
class CImpsAliveTimer;

// CLASS DECLARATION

class CImpsAliveManager : public CActive
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        * @param aServer WV engine server
        */
        static CImpsAliveManager* NewL( MImpsCSPSession& aServer );

        /**
        * Destructor.
        */
        virtual ~CImpsAliveManager();

    public:


        /**
        * Start KeepAlive timer for idle times
        * @param aInterval idle time in seconds
        * @return error code
        */
        void StartTimer( TInt aInterval );

        /**
        * Stop KeepAlive timer for idle times
        */
        void StopTimer(  );

        /**
        * Check transport response and decide what to do.
        * @param aCode error code
        */
        void CheckResp( TInt aCode );

        /**
        * Check transport error and decide what to do.
        * @param aTid failed transaction-id
        */
        void CheckError( const TDesC& aTid );

        /**
        * Send KeepAlive primitive
        * @param aSchedule true if triggered by scheduled timer
        */
        void SendKeepAlive( TBool aSchedule );

    protected:
        // From base class
        void RunL();

        void DoCancel();

    private:

        /**
        * C++ default constructor.
        * @param aServer WV engine server
        */
        CImpsAliveManager( MImpsCSPSession& aServer );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsAliveManager( const CImpsAliveManager& );
        // Prohibit assigment operator
        CImpsAliveManager& operator= ( const CImpsAliveManager& );

        /**
        * Do the error routines.
        */
        void DoHandleError( );

        /**
        * Close CSP Session
        */
        void DoSessionClose();


    private:    // Data
        TInt                iFailCount;
        TInt                iTimerFail;
        TImpsAliveState     iState;
        MImpsCSPSession&    iServer;
        CImpsAliveTimer*    iTimer;
        TBool               iScheduled;
        TBuf<KImpsMaxTID>   iTid;
        TInt                iSeconds;
    };



// CLASS DECLARATION

/**
* CImpsAliveTimer
* Idle timer to lauch Keep-Alive requests in CSP protocol.
*/
class CImpsAliveTimer: public CActive
    {
    public:

        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority active object priority
        */
        CImpsAliveTimer( CImpsAliveManager& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsAliveTimer();

        /**
        * Start waiting. If not reset again while wait time
        * then Alive msg will be sent.
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

        void DoCancel();
    private:
        TInt                iSeconds;
        CImpsAliveManager&  iMgr;
        RTimer              iTimer;
        TBool               iReset;
        TBool               iCanceled;
        TInt                iMicroSeconds;

    };


#endif      // ?INCLUDE_H   

// End of File
