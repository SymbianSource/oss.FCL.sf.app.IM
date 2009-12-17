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


// INCLUDE FILES
#include "CIMPSAlwaysOnlineScheduler.h"
#include "CPEngAOPluginTimer.h"
#include "MIMPSScheduleObserver.h"
#include "IMPSCommonUiDebugPrint.h"
#include "IMPSAOPluginPanics.h"
#include "CIMPSSharedDataFactory.h"
#include "MIMPSSharedData.h"
#include "IMPSUIDDefs.h"
#include "wvsettingsshareddataNG.h"

// CONSTANTS
const TInt KTimeValueLength( 2 );
const TInt KTimeStringMaxLength( 10 );			// the length of the time string
const TInt KDefaultStartHour( 8 );
const TInt KDefaultEndHour( 20 );
const TInt KDefaultMinute( 0 );
_LIT( KDefaultScheduleStartTime, "08:00" );		// default times
_LIT( KDefaultScheduleEndTime, "20:00" );

// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::CIMPSAlwaysOnlineScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSAlwaysOnlineScheduler::CIMPSAlwaysOnlineScheduler( TIMPSAOKeys& aKeys ) :
        iScheduledLoginOk( EFalse ),
        iKeys( aKeys ),
        iSchedulingIsStarted( EFalse ),
        iStartupCheckOngoing( EFalse )
    {

    }

// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::ConstructL(  )
    {
    iSharedDataClient =
        CIMPSSharedDataFactory::CreatePermanentKeyHandlerL( this,
                                                            KIMPSServSettUid );

    iSettingChangesNotifier =
        CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( this,
                                                            KIMPSServSettNotifyUid );

    iTimer = CPEngAOPluginTimer::NewL();
    }



// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSAlwaysOnlineScheduler* CIMPSAlwaysOnlineScheduler::NewL( TIMPSAOKeys& aKeys )
    {
    CIMPSAlwaysOnlineScheduler* self = new( ELeave ) CIMPSAlwaysOnlineScheduler( aKeys );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CIMPSAlwaysOnlineScheduler::~CIMPSAlwaysOnlineScheduler()
    {
    delete iSharedDataClient;
    delete iSettingChangesNotifier;
    delete iTimer;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::StartScheduling
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::StartSchedulingL()
    {
    IMPSCUI_DP_FUNC_ENTER( "StartScheduling" );

    if ( !iSchedulingIsStarted )
        {
        iSchedulingIsStarted = ETrue;
        iTimer->SetObserver( this );

        CheckScheduledLoginSettings();

        // start listening schedule change events
        TInt err ( iSettingChangesNotifier->SubscribeSet( KIMPSServSettNotifyUid,
                                                          iKeys.ScheduleChangedKey() ) );
        if ( err )
            {
            StopScheduling();
            User::Leave( err );
            }
        }
    IMPSCUI_DP_FUNC_DONE( "StartScheduling" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::StopScheduling
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::StopScheduling()
    {
    IMPSCUI_DP_FUNC_ENTER( "StopScheduling" );
    if ( iSchedulingIsStarted )
        {

        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        iTimer->SetObserver( NULL );

        iSettingChangesNotifier->UnSubscribe( KIMPSServSettNotifyUid,
                                              iKeys.ScheduleChangedKey() );

        iScheduledLoginOk = EFalse;
        iWaitReason = EIMPSAOSchedulerNotWaiting;
        iSchedulingIsStarted = EFalse;
        }
    IMPSCUI_DP_FUNC_DONE( "StopScheduling" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::IsScheduledLoginOk()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CIMPSAlwaysOnlineScheduler::IsScheduledLoginOk()
    {
    return iScheduledLoginOk;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::HandlePermanentKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CIMPSAlwaysOnlineScheduler::HandlePermanentKeyNotifyL( const TUid /* aUid */,
                                                             const TIMPSSharedKeys /* aKey */ )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandlePermanentKeyNotifyL" );
    IMPSCUI_DP_FUNC_DONE( "HandlePermanentKeyNotifyL" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::HandleTemporaryKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::HandleTemporaryKeyNotifyL( const TUid aUid,
                                                            const TIMPSSharedKeys aKey )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleTemporaryKeyNotifyL" );

    if ( ( aUid == KIMPSServSettNotifyUid ) &&
         ( aKey == iKeys.ScheduleChangedKey() ) )
        {
        CheckScheduledLoginSettings();
        }

    IMPSCUI_DP_FUNC_DONE( "HandleTemporaryKeyNotifyL" );
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::HandleTimeWaitedL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::HandleTimeWaited()
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleTimeWaited" );
    // check settings again and need for restart wait timer
    CheckScheduledLoginSettings();
    IMPSCUI_DP_FUNC_DONE( "HandleTimeWaited" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::HandleTimeWaitedError
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::HandleTimeWaitedError( TInt aError )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleTimeWaitedError" );
    if ( aError != KErrCancel )
        {
        CheckScheduledLoginSettings();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleTimeWaitedError" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::SetObserverL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::SetObserver( MIMPSScheduleObserver *aSchObserver )
    {
    IMPSCUI_DP_FUNC_ENTER( "SetObserver" );
    iScheduleObserver = aSchObserver;
    IMPSCUI_DP_FUNC_DONE( "SetObserver" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::CheckScheduledLoginSettingsL
// Checks the scheduling settings
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::CheckScheduledLoginSettings()
    {
    IMPSCUI_DP_FUNC_ENTER( "CheckScheduledLoginSettings" );
    // get the connection days
    TInt connectionDays( ResolveConnectionDays() );
    if ( connectionDays == EWVSettingsNone )
        {
        SetScheduledLogin( EFalse, EIMPSAOSchedulerNotWaiting );
        IMPSCUI_DP_FUNC_DONE( "CheckScheduledLoginSettings" );
        return;
        }

    // get current hometime
    // used also as the general waiting time, so we won't need yet another TTime
    // instance and so that the switch-case looks a bit prettier
    TTime currentTime;
    currentTime.HomeTime();

    TDateTime curTime( currentTime.DateTime() );

    IMPSCUI_DP( D_IMPSCUI_LIT( "current day [%d], month [%d]" ), curTime.Day() + 1,
                curTime.Month() + 1 );
    IMPSCUI_DP( D_IMPSCUI_LIT( "current hour [%d], minu [%d]" ), curTime.Hour(),
                curTime.Minute() );
    TInt today( currentTime.DayNoInWeek() );
    TTime connectionStartTime;
    TTime connectionEndTime;
    TInt schState( FindNextConnectionHours( currentTime,
                                            connectionStartTime,
                                            connectionEndTime ) );

    IMPSCUI_DP( D_IMPSCUI_LIT( "schedule state = %d" ), schState );

    TTimeIntervalDays untilChange( 0 );
    switch ( schState )
        {
            // start time and end time are same
        case EIMPSAOPluginAlldayConnection:
            {
            if ( ResolveIfConnectionToday( today, connectionDays ) )
                {
                // setting next checking time next day start time
                SetScheduledLogin( ETrue, EIMPSAOSchedulerWaitingForAllDayConnRefresh );
                untilChange = 1;
                currentTime = ( connectionStartTime + untilChange );
                }
            else
                {
                // setting next checking time to next connection day
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = FindNextConnectionChangeDay( today, connectionDays, ETrue );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        // current time is < start time
        case EIMPSAOPluginNormalBeforeConnection:
            {
            if ( ResolveIfConnectionToday( today, connectionDays ) )
                {
                // setting next checking to current day connection start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                currentTime = connectionStartTime;
                }
            else
                {
                // setting next checking time to next connection day start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = ( FindNextConnectionChangeDay( ( today + 1 ), connectionDays, ETrue ) );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        // current time is between start and end time
        case EIMPSAOPluginNormalDuringConnection:
            {
            if ( ResolveIfConnectionToday( today, connectionDays ) )
                {
                // setting next checking to current day connection end time
                SetScheduledLogin( ETrue, EIMPSAOSchedulerWaitingForDisconnect );
                currentTime = connectionEndTime;
                }
            else
                {
                // setting next checking to next connection day start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = FindNextConnectionChangeDay( today, connectionDays, ETrue );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        // current time is more than end time
        case EIMPSAOPluginNormalAfterConnection:
            {
            if ( ResolveIfConnectionToday( today + 1, connectionDays ) )
                {
                // setting next checking to next day connection start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = 1;
                currentTime = ( connectionStartTime + untilChange );
                }
            else
                {
                // setting next checking to next connection day start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = ( FindNextConnectionChangeDay( today + 1, connectionDays, ETrue ) + 1 );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        // current time is between start and end time,
        // start time < 00:00, current time and endtime > 00:00
        case EIMPSAOPluginOvernightOldConnection:
            {
            if ( ResolveIfConnectionToday( ( today - 1 ), connectionDays ) )
                {
                // setting next checking to current day connection end time
                SetScheduledLogin( ETrue, EIMPSAOSchedulerWaitingForDisconnect );
                currentTime = connectionEndTime;
                }
            else
                {
                // setting next checking to next connection day start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = ( FindNextConnectionChangeDay( today + 1, connectionDays, ETrue ) + 1 );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        // current time is > end time
        case EIMPSAOPluginOvernightNoConnection:
            {
            // setting next checking to next connection day start time
            SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
            untilChange = FindNextConnectionChangeDay( today, connectionDays, ETrue );
            currentTime = ( connectionStartTime + untilChange );
            break;
            }

        // current time is between start and end time,
        // start time and current time < 00:00, endtime > 00:00
        case EIMPSAOPluginOvernightNewConnection:
            {
            if ( ResolveIfConnectionToday( today, connectionDays ) )
                {
                // setting next checking to next day connection end time
                SetScheduledLogin( ETrue, EIMPSAOSchedulerWaitingForDisconnect );
                untilChange = 1;
                currentTime = ( connectionEndTime + untilChange );
                }
            else
                {
                // setting next checking to next connection day start time
                SetScheduledLogin( EFalse, EIMPSAOSchedulerWaitingForReconnect );
                untilChange = ( FindNextConnectionChangeDay( today, connectionDays, ETrue ) );
                currentTime = ( connectionStartTime + untilChange );
                }
            break;
            }

        default:
            {
            SetScheduledLogin( EFalse, EIMPSAOSchedulerNotWaiting );
            IMPSCUI_DP_FUNC_DONE( "CheckScheduledLoginSettings" );
            return;
            }

        }

    StartTimer( currentTime );
    IMPSCUI_DP_FUNC_DONE( "CheckScheduledLoginSettings" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::ResolveIfConnectionToday
//
// ---------------------------------------------------------
//
TBool CIMPSAlwaysOnlineScheduler::ResolveIfConnectionToday( TInt aToday,
                                                            TInt aConnectionDays )
    {
    TBool retVal( EFalse );
    // this is needed because the method is used for checking yesterdays also
    //
    // to monday
    if ( aToday < EMonday )
        {
        aToday = ESunday;
        }

    switch ( aToday )
        {
        case EMonday:
            {
            if ( aConnectionDays & EWVSettingsMonday )
                {
                retVal = ETrue;
                }
            break;
            }
        case ETuesday:
            {
            if ( aConnectionDays & EWVSettingsTuesday )
                {
                retVal = ETrue;
                }
            break;
            }
        case EWednesday:
            {
            if ( aConnectionDays & EWVSettingsWednesday )
                {
                retVal = ETrue;
                }
            break;
            }
        case EThursday:
            {
            if ( aConnectionDays & EWVSettingsThursday )
                {
                retVal = ETrue;
                }
            break;
            }
        case EFriday:
            {
            if ( aConnectionDays & EWVSettingsFriday )
                {
                retVal = ETrue;
                }
            break;
            }
        case ESaturday:
            {
            if ( aConnectionDays & EWVSettingsSaturday )
                {
                retVal = ETrue;
                }
            break;
            }
        case ESunday:
            {
            if ( aConnectionDays & EWVSettingsSunday )
                {
                retVal = ETrue;
                }
            break;
            }
        default:
            {
            // retVal keeped as false
            break;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::ResolveConnectionDays
//
// ---------------------------------------------------------
//
TInt CIMPSAlwaysOnlineScheduler::ResolveConnectionDays()
    {
    TInt connectionDays( KErrNotFound );

    //check if getting the connection day went ok
    if ( iSharedDataClient->GetIntKey( iKeys.ConnDaysKey(), connectionDays ) != KErrNone )
        {
        // if there was an error reading form the shared data file, set to UI spec default
        connectionDays = EWVSettingsAll;
        }
    if ( ( connectionDays < EWVSettingsNone ) ||
         ( connectionDays > EWVSettingsAll ) )
        {
        // if the found value was an invalid one, set it to default value
        connectionDays = EWVSettingsAll;
        }

    return connectionDays;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::FindNextConnectionChangeDay
//
// ---------------------------------------------------------
//
TInt CIMPSAlwaysOnlineScheduler::FindNextConnectionChangeDay( TInt aToday,
                                                              TInt aConnectionDays,
                                                              TBool aFindConnectionDay )
    {
    // first search for the connection day from the end of the week
    // but skip the first day (=today)
    TBool dayFound( EFalse );
    TInt dayIndex;
    for ( dayIndex = aToday; dayIndex <= ESunday; dayIndex++ )
        {
        if ( aFindConnectionDay )
            {
            if ( ResolveIfConnectionToday( dayIndex, aConnectionDays ) )
                {
                dayFound = ETrue;
                break;
                }
            }
        else
            {
            if ( !ResolveIfConnectionToday( dayIndex, aConnectionDays ) )
                {
                dayFound = ETrue;
                break;
                }
            }
        }

    // connection day wasn't found on the end part of the week,
    // check next the beginning of the week until today
    if ( !dayFound )
        {
        for ( dayIndex = EMonday; dayIndex <= aToday; dayIndex++ )
            {
            if ( aFindConnectionDay )
                {
                if ( ResolveIfConnectionToday( dayIndex, aConnectionDays ) )
                    {
                    dayFound = ETrue;
                    break;
                    }
                }
            else
                {
                if ( !ResolveIfConnectionToday( dayIndex, aConnectionDays ) )
                    {
                    dayFound = ETrue;
                    break;
                    }
                }
            }
        }

    // next connection change wasn't found at all, return an error
    if ( !dayFound )
        {
        return KErrNotFound;
        }
    else
        {
        // calculate the amount of days until the next connection event
        if ( dayIndex >= aToday )
            {
            dayIndex = ( dayIndex - aToday );
            }
        else
            {
            // 7 is the number of days in a week
            dayIndex = ( 7 - ( aToday - dayIndex ) );
            }
        }

    return dayIndex;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::FindNextConnectionHours
//
// ---------------------------------------------------------
//
TInt CIMPSAlwaysOnlineScheduler::FindNextConnectionHours( TTime aTimeNow,
                                                          TTime& aStartTime,
                                                          TTime& aEndTime )
    {
    TIMPSAOPluginScheduleState scheduleState( EIMPSAOPluginInvalidSchedule );
    TTime comparableTime;
    // convert the current time to the format we use here
    TBuf<KTimeStringMaxLength> tmpBufStart;
    TBuf<KTimeStringMaxLength> tmpBufEnd;

    // parse the current time
    comparableTime = aTimeNow;

    // resolve the next connection time
    // check first if the time can be found
    if ( ( iSharedDataClient->GetStringKey( iKeys.ConnStartKey(), tmpBufStart ) != KErrNone ) ||
         ( iSharedDataClient-> GetStringKey( iKeys.ConnEndKey(), tmpBufEnd ) != KErrNone ) )
        {
        // we did not get the times, set the defaults
        tmpBufStart = KDefaultScheduleStartTime();
        tmpBufEnd = KDefaultScheduleEndTime();
        }

    // there was a normal connection schedule
    aStartTime.HomeTime();
    aEndTime.HomeTime();

    TDateTime dateTime = aStartTime.DateTime();
    // get the start time
    // read the hour part of the time
    TInt connStartHour = Hour( tmpBufStart, KDefaultStartHour );
    dateTime.SetHour( connStartHour );
    // read the minute part of the time
    TInt connStartMinute = Minute( tmpBufStart, KDefaultMinute );
    dateTime.SetMinute( connStartMinute );
    // set the start time
    aStartTime = dateTime;

    // get the end time
    // read the hour part
    TInt connEndHour = Hour( tmpBufEnd, KDefaultEndHour );
    dateTime.SetHour( connEndHour );
    // read the minute part
    TInt connEndMinute = Minute( tmpBufEnd, KDefaultMinute );
    dateTime.SetMinute( connEndMinute );
    // set the end time
    aEndTime = dateTime;

    // now check the connection start and end times and the state of the
    // connection
    TTimeIntervalMinutes minuteInterval;
    // case 1: all day schedule
    if ( aStartTime == aEndTime )
        {
        scheduleState = EIMPSAOPluginAlldayConnection;
        MinutesInterval( aStartTime, comparableTime, minuteInterval );
        aStartTime = ( aTimeNow + minuteInterval );
        MinutesInterval( aEndTime, comparableTime, minuteInterval );
        aEndTime = ( aTimeNow + minuteInterval );
        }
    // case 2: normal schedule
    else if ( aStartTime < aEndTime )
        {
        // case 2a: about to make a login
        if ( comparableTime < aStartTime )
            {
            scheduleState = EIMPSAOPluginNormalBeforeConnection;
            MinutesInterval( aStartTime, comparableTime, minuteInterval );
            aStartTime = ( aTimeNow + minuteInterval );
            }

        else
            {
            // case 2b: between start and end times
            if ( comparableTime < aEndTime )
                {
                scheduleState = EIMPSAOPluginNormalDuringConnection;
                MinutesInterval( aStartTime, comparableTime, minuteInterval );
                aStartTime = ( aTimeNow + minuteInterval );
                MinutesInterval( aEndTime, comparableTime, minuteInterval );
                aEndTime = ( aTimeNow + minuteInterval );
                }
            // case 2c: connection over for today
            else // ( aEndTime <= comparableTime )
                {
                scheduleState = EIMPSAOPluginNormalAfterConnection;
                MinutesInterval( aStartTime, comparableTime, minuteInterval );
                aStartTime = ( aTimeNow + minuteInterval );
                }
            }

        }
    // case 3: overnight schedule
    else
        {
        // case 3a: connection not ended yet
        if ( comparableTime < aEndTime )
            {
            scheduleState = EIMPSAOPluginOvernightOldConnection;
            MinutesInterval( aStartTime, comparableTime, minuteInterval );
            aStartTime = ( aTimeNow + minuteInterval );
            MinutesInterval( aEndTime, comparableTime, minuteInterval );
            aEndTime = ( aTimeNow + minuteInterval );
            }

        else
            {
            // case 3b: time between connections
            if ( comparableTime < aStartTime )
                {
                scheduleState = EIMPSAOPluginOvernightNoConnection;
                MinutesInterval( aStartTime, comparableTime, minuteInterval );
                aStartTime = ( aTimeNow + minuteInterval );
                }
            // case 3c: connection started again
            else // ( aStartTime <= comparableTime )
                {
                scheduleState = EIMPSAOPluginOvernightNewConnection;
                MinutesInterval( aStartTime, comparableTime, minuteInterval );
                aStartTime = ( aTimeNow + minuteInterval );
                MinutesInterval( aEndTime, comparableTime, minuteInterval );
                aEndTime = ( aTimeNow + minuteInterval );
                }
            }
        }

    return scheduleState;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::Hour()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CIMPSAlwaysOnlineScheduler::Hour( const TDesC& aTime, TInt aDefault )
    {
    TPtrC hourPtr( aTime.Left( KTimeValueLength ) );

    TLex lex( hourPtr );
    TInt hourInt ( aDefault );
    // we can ignore the error, if it fails we set the default
    lex.Val( hourInt );
    return hourInt;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::Minute()
//
// (other items were commented in a header).
// ---------------------------------------------------------
TInt CIMPSAlwaysOnlineScheduler::Minute( const TDesC& aTime, TInt aDefault )
    {
    TPtrC minPtr( aTime.Right( KTimeValueLength ) );
    TLex lex( minPtr );
    TInt minuteInt ( aDefault );
    // we can ignore the error, if it fails we set the default
    lex.Val( minuteInt );
    return minuteInt;
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::SetScheduledLogin
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::SetScheduledLogin( TBool aIsScheduledLoginOk,
                                                    TIMPSAOSchedulerWaitReason aReason )
    {
    IMPSCUI_DP_FUNC_ENTER( "SetScheduledLogin" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aIsScheduledLoginOK = %d" ), aIsScheduledLoginOk );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aReason = %d" ), aReason );
    TBool prevScheduledLoginOK( iScheduledLoginOk );
    iScheduledLoginOk = aIsScheduledLoginOk;
    iWaitReason = aReason;

    if ( iScheduleObserver && !iStartupCheckOngoing &&
         ( ( prevScheduledLoginOK != iScheduledLoginOk ) ||
           ( aReason == EIMPSAOSchedulerWaitingForAllDayConnRefresh ) ) )
        {
        iScheduleObserver->HandleScheduleEvent( iScheduledLoginOk );
        }
    IMPSCUI_DP_FUNC_DONE( "SetScheduledLogin" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::StartTimer
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::StartTimer( TTime aTime )
    {
    IMPSCUI_DP_FUNC_ENTER( "StartTimer" );
    if ( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }

    TDateTime launchTime( aTime.DateTime() );
    IMPSCUI_DP( D_IMPSCUI_LIT( "Waiting for [%d] at:" ), iWaitReason );
    IMPSCUI_DP( D_IMPSCUI_LIT( "year [%d] " ), launchTime.Year() );
    IMPSCUI_DP( D_IMPSCUI_LIT( "day [%d], month [%d]" ), launchTime.Day() + 1,
                launchTime.Month() + 1 );
    IMPSCUI_DP( D_IMPSCUI_LIT( "hour [%d], minu [%d]" ), launchTime.Hour(),
                launchTime.Minute() );

    iTimer->At( aTime );
    IMPSCUI_DP_FUNC_DONE( "StartTimer" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::ParseTime
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::ParseTime( TTime& aTime,
                                            const TDesC& aTimeString )
    {
    if ( aTime.Parse( aTimeString ) < KErrNone )
        {
        User::Panic( KIMPSAOPluginPanicCategory, ETimeParsingFailed );
        }
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlineScheduler::ParseTime
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlineScheduler::MinutesInterval( TTime aTime,
                                                  TTime aComparableTime,
                                                  TTimeIntervalMinutes& aInterval )
    {
    if ( aTime.MinutesFrom( aComparableTime, aInterval ) != KErrNone )
        {
        User::Panic( KIMPSAOPluginPanicCategory,
                     ETimeIntervalCalculationFailed );
        }
    }
