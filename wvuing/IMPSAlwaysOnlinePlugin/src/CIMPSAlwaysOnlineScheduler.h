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
* Description:  Helper class for always online scheduling
*
*/



#ifndef __CIMPSALWAYSONLINESCHEDULER_H__
#define __CIMPSALWAYSONLINESCHEDULER_H__

//  INCLUDES
#include "MPEngAOWaitObserver.h"
#include "MIMPSSharedDataObserver.h"
#include "MIMPSSharedData.h"
#include "timpsaokeys.h"

// FORWARD DECLARATIONS
class MIMPSScheduleObserver;
class CPEngAOPluginTimer;
class CIMPSAOKeys;

/**
*  The implementation for the always online scheduling
*
*  @lib PEngAOPlugin.lib
*  @since 2.6
*/
class CIMPSAlwaysOnlineScheduler : public CBase,
            public MPEngAOWaitObserver,
            public MIMPSSharedDataObserver
    {

    public:  // Enumerations

        /**
         * State of the scheduling
         */
        enum TIMPSAOPluginScheduleState
            {
            EIMPSAOPluginAlldayConnection = 0,
            EIMPSAOPluginNormalBeforeConnection = 1,
            EIMPSAOPluginNormalDuringConnection = 2,
            EIMPSAOPluginNormalAfterConnection = 3,
            EIMPSAOPluginOvernightOldConnection = 4,
            EIMPSAOPluginOvernightNoConnection = 5,
            EIMPSAOPluginOvernightNewConnection = 6,
            EIMPSAOPluginInvalidSchedule = 10
            };

        /**
        * The waiting state of the scheduler
        */
        enum TIMPSAOSchedulerWaitReason
            {
            EIMPSAOSchedulerNotWaiting = 0,
            EIMPSAOSchedulerWaitingForReconnect = 1,
            EIMPSAOSchedulerWaitingForDisconnect = 2,
            EIMPSAOSchedulerWaitingForAllDayConnRefresh = 3
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aKeys Shared datakeys and enumeration for retrieve
        *              right information
        */
        static CIMPSAlwaysOnlineScheduler* NewL( TIMPSAOKeys& aKeys );

        /**
        * Destructor.
        */
        virtual ~CIMPSAlwaysOnlineScheduler();

    public: // Functions from MIMPSMIMPSSharedDataObserver

        /**
        * Handles notify from permanent keys' changes
        * @see MIMPSMIMPSSharedDataObserver
        */
        void HandlePermanentKeyNotifyL( const TUid aUid,
                                        const TIMPSSharedKeys aKey );

        /**
        * Handles notify from temporary keys' changes
        * @see MIMPSMIMPSSharedDataObserver
        */
        void HandleTemporaryKeyNotifyL( const TUid aUid,
                                        const TIMPSSharedKeys aKey );

    public: // Functions from MPEngAOWaitObserver

        /**
        * @see MPEngAOWaitObserver
        */
        void HandleTimeWaited();

        /**
        * @see MPEngAOWaitObserver
        */
        void HandleTimeWaitedError( TInt aError );

    public: // New functions

        /**
        * Adds observer for this scheduler
        * @param aSchObserver observer for scheduled login/logouts or schedule changes
        */
        void SetObserver( MIMPSScheduleObserver *aSchObserver );

        /**
         * Scheduler starts schedule login/logout actions as set in settings and
         * stars observing settings changes.
         */
        void StartSchedulingL();

        /**
         * Stops login/logout scheduling and stops observing settings changes.
         */
        void StopScheduling();

        /**
         * Check if scheduled login is OK.
         * @return ETrue if scheduled login is OK, else EFalse
         */
        TBool IsScheduledLoginOk();

        /**
        * Mark the testing class a friend, because this class is not "public" class
        */
        friend class T_CIMPSAlwaysOnlineScheduler; // CSI: 36 # Only used for testing

    private:

        /**
        * C++ default constructor.
        * @param aKeys Shared datakeys and enumeration for retrieve
        *              right information
        */
        CIMPSAlwaysOnlineScheduler( TIMPSAOKeys& aKeys );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /*
        * Checks whether it's ok to do the login by the scheduled
        * setting or not. If not, starts to wait for the event.
        * If the login is allowed by schedule, starts to wait for
        * the next logout.
        */
        void CheckScheduledLoginSettings();

        /**
         * Checks if there is a connection scheduled for the
         * given day.
         * @param aToday The day the connection is queried for
         * @paran aConnectionDays The set connection days in the
         *		  shared data.
         * @return TBool ETrue if there's an connection on aToday
         */
        TBool ResolveIfConnectionToday( TInt aToday, TInt aConnectionDays );

        /**
         * Gets the connection days from the shared data key
         * @return TInt The bitmask of the user selected connection days
         */
        TInt ResolveConnectionDays();

        /**
         * Finds the next connection day
         * @param aToday Today's day's number
         * @paran aConnectionDays The set connection days in the
         *		  shared data.
         * @param aFindConnection ETrue if we are looking for next connection day
         * @return TInt How many days until next connection
         */
        TInt FindNextConnectionChangeDay( TInt aToday,
                                          TInt aConnectionDays,
                                          TBool aFindConnectionDay );

        /**
        * Resolves the connection hours
        * @param aTimeNow is the current time
        * @param aStartTime is the connection start time
        * @param aEndTime is the connection end time
        * @return TBool If overnight schedule
        */
        TInt FindNextConnectionHours( TTime aTimeNow,
                                      TTime& aStartTime,
                                      TTime& aEndTime );

        /**
         * Sets scheduled login state
         * @param aIsScheduledLoginOk ETrue if client should be logged in, else EFalse
         * @param aReason   Tells what scheduler is waiting according to schedule
         */
        void SetScheduledLogin( TBool aIsScheduledLoginOk,
                                TIMPSAOSchedulerWaitReason aReason );

        /**
        * Starts the waiting timer
        * @param aTime, time for timer to lauch
        */
        void StartTimer( TTime aTime );

        /**
         * Parses time from string. Panics if time cannot be parsed.
         * @param aTime variable where to time is parsed
         * @param aTimeString to be parsed time in string format
         */
        void ParseTime( TTime& aTime, const TDesC& aTimeString );

        /**
         * Calculates time difference between two time values
         * in minutes. Panics if time difference calculation fails.
         * @param aTime time to where difference is calculated
         * @param aComparableTime time where aTime difference is calculated
         * @param aInterval time difference is stored to this variable.
         */
        void MinutesInterval( TTime aTime,
                              TTime aComparableTime,
                              TTimeIntervalMinutes& aInterval );

        /**
        * Returns the hour part of the connection time as integer.
        * Returns the given default if parsing fails.
        * @param aTime Time where from hour is parsed
        * @param aDefault Default value which is return if parsing fails
        * @return Parsed hour integer
        * @since 2.6
        */
        TInt Hour( const TDesC& aTime, TInt aDefault );

        /**
        * Returns the minute part of the connection time as integer
        * Returns the given default if parsing fails.
        * @param aTime Time where from minute is parsed
        * @param aDefault Default value which is return if parsing fails
        * @return Parsed minute integer
        * @since 2.6
        */
        TInt Minute( const TDesC& aTime, TInt aDefault );

    private:    // Data

        /// Shared data client to read common settings, owned
        MIMPSSharedData* iSharedDataClient;

        /// Listens settings changes, owned
        MIMPSSharedData* iSettingChangesNotifier;

        /// Timer for waiting connect and disconnects according to schedule.
        /// Owned.
        CPEngAOPluginTimer* iTimer;

        /// Tells if the scheduled login is in "should be logged in" state
        TBool iScheduledLoginOk;

        /// Defines the current wait reason of the Scheduler
        TIMPSAOSchedulerWaitReason iWaitReason;

        /// Observer for schedule changes, not owned
        MIMPSScheduleObserver* iScheduleObserver;

        /// shared data keys etc to get wanted information
        TIMPSAOKeys& iKeys;

        /// helper variable to track if scheduling is started or not
        TBool iSchedulingIsStarted;

        /// variable to track if current schedule check is
        /// scheduler startup check or timed check
        TBool iStartupCheckOngoing;

    };

#endif      // __CIMPSALWAYSONLINESCHEDULER_H__

// End of File
