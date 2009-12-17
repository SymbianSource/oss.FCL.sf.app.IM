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
* Description:  Offers the always online functionality for the Presence Engine
*                and the Chat application
*
*/


// INCLUDE FILES
#include "CIMPSAlwaysOnlinePluginImp.h"
#include "CPEngAOPluginTimer.h"
#include "CPEngAOCallObserver.h"
#include "IMPSCommonUiDebugPrint.h"
//#include "PresenceErrors.h"

#include "CIMPSAlwaysOnlineScheduler.h"
#include "CPEngAONwStatusObserver.h"
#include "cimpsconnuiconnectionmodenotifierng.h"
#include "cimpspresenceaaconnectionng.h"
#include "CIMPSSharedDataFactory.h"
#include "MIMPSSharedData.h"
#include "CIMPSReconnectNoteHandler.h"
#include "IMPSUIDDefs.h"
#include "impspresenceconnectionuiconstsng.h"

#include <PEngAOPluginNG.rsg>
#include <flogger.h>
#include <PEngPresenceEngineConsts2.h>
#include <avkon.hrh>
#include <errorui.h>
#include <e32property.h>    // RProperty

#include <APGCLI.H>
#include <APACMDLN.H>
#include <aknglobalmsgquery.h>
#include <bautils.h>

#include "cpengaodialogmonitor.h"
#include "impscommonuibuilddefinitions.h"

// CONSTANTS

// after first 12 connection tries is shown global note for continuation
// if user accepts continuation then 8 new tries and after that note
// is shown again user accepts again then 8 new tries and so on....
const TInt KFirstReconnectionRetryCycleLength( 12 );

const TInt KRestReconnectionRetryCyclesLength( 8 );


//wait time after changing from offline to not offline state, in microseconds
const TInt KWaitTimeFromOfflineToNotOffline = 22000000;

// the wait times are from IMPS Service Settings UI specification and they are
// in minutes. They are also relative to the previously waited, not
// absolute wait times. For first fife tries are defined values and after that
// reconnection is tried after 15 minutes intervals
const TInt KMaxNumberOfDefinedWaitTimes( 5 );

const TInt KLoginWaitTime[KMaxNumberOfDefinedWaitTimes] = { 0, 2, 3, 5, 5 };

const TInt KDefaultLoginWaitTime = 15;

_LIT_SECURITY_POLICY_PASS( KIMPSSharedReadPolicy );
_LIT_SECURITY_POLICY_PASS( KIMPSSharedWritePolicy );

_LIT( KAOPluginResourceFile, "PENGAOPluginNG.rsc" );
_LIT( KAOPluginResourcePath, "\\Resource\\" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::CIMPSAlwaysOnlinePluginImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSAlwaysOnlinePluginImp::CIMPSAlwaysOnlinePluginImp() :
        iNetworkOpsAllowed( ETrue ),
//iPreviousEvent( EPEngPresenceServiceSAPChanged ),
        iLoginCount( 1 ), // first attempt
        iPluginState( EIMPSAOPluginRunning ),
        iRoaming( EFalse ),
        iScheduledLoginOk( EFalse ),
        iUserControlledConnection( EFalse ),
        iEventInQueue( EFalse ),
        iQueuedLogoutIsScheduledLogout( EFalse ),
        iCurServerDisconnectDenied( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::ConstructL()
    {
    iAutoLogin = iKeys.LoginManualEnum();

    // client for notice setting changes
    iSettingChangesNotifier =
        CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( this,
                                                            KIMPSServSettNotifyUid );

    // create shared data client
    iSharedDataServiceSettings =
        CIMPSSharedDataFactory::CreatePermanentKeyHandlerL( this,
                                                            KIMPSServSettUid );

    iNwObserver = CPEngAONwStatusObserver::NewL( *this );

    // Create monitor for roam ing warning dialog
    iDlgMonitor = CPEngAODialogMonitor::NewL( *this );

    User::LeaveIfError( iFs.Connect() );

    TFileName resourceFileName;
    resourceFileName.Zero();
    TFileName drivePath;
    Dll::FileName( drivePath );
    resourceFileName.Append( TParsePtrC( drivePath ).Drive() );
    resourceFileName.Append( KAOPluginResourcePath() );
    resourceFileName.Append( KAOPluginResourceFile );

    if ( resourceFileName.Length() > 0 )
        {
        // when baflutils gets an empty string, it returns "C:",
        // which breaks things
        BaflUtils::NearestLanguageFile( iFs, resourceFileName );
        }

    iResFile.OpenL( iFs, resourceFileName );
    iResFile.ConfirmSignatureL();
    }

// -----------------------------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSAlwaysOnlinePluginImp* CIMPSAlwaysOnlinePluginImp::NewL()
    {
    CIMPSAlwaysOnlinePluginImp* self = new( ELeave ) CIMPSAlwaysOnlinePluginImp();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CIMPSAlwaysOnlinePluginImp::~CIMPSAlwaysOnlinePluginImp()
    {
    FreeMemory();
    delete iSettingChangesNotifier;
    delete iSharedDataServiceSettings;
    delete iNwObserver;
    delete iRoamingQueryText;
    delete iRoamingQueryTitle;
    if (	iGlobalMsgQuery )
        {
        iGlobalMsgQuery->CancelMsgQuery();
        }
    delete iGlobalMsgQuery;
    delete iDlgMonitor;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::DoLogin
// Logs out from the wv server
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::DoLogin()
    {
    IMPSCUI_DP_FUNC_ENTER( "DoLogin" );
    // if already in doing login state or
    // if the call is active, just return when the call ends, we get a notify->
    // just return (a possible leave can be ignored)
    if ( iCallObserver->CallActive( this ) )
        {
        // if call is active, this is set as the call observer
        IMPSCUI_DP_TXT( "Call is active, cannot do login currently" );
        return;
        }
    else if ( iPluginState == EIMPSAOPluginDoingLogin )
        {
        // emptying event queue,
        // because is currently doing login, and we
        // want to be in logged state after login is done
        iEventInQueue = EFalse;
        return;
        }
    else if ( iPluginState == EIMPSAOPluginDoingLogout )
        {
        // currently doing logout, adding login action to
        // event queue
        iEventInQueue = ETrue;
        return;
        }

    // start login procedure and set correct state
    iPluginState = EIMPSAOPluginDoingLogin;
    iReconnectNoteHandler->CancelNoteShowing( this );

    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::DoLogin try[%d]" ), iLoginCount );
    TInt retVal( KErrNone );
    TInt err( KErrNone );

    iTimeBeforeLoginTry.HomeTime();

#ifdef DEBUG
    TDateTime loginTime;
    loginTime = iTimeBeforeLoginTry.DateTime();
    IMPSCUI_DP_TXT( "Time before login try: " );
    IMPSCUI_DP( D_IMPSCUI_LIT( "year [%d] " ), loginTime.Year() );
    // +1 added to get human readable form of day and month on debug print
    IMPSCUI_DP( D_IMPSCUI_LIT( "day [%d], month [%d]" ), loginTime.Day() + 1,
                loginTime.Month() + 1 );
    IMPSCUI_DP( D_IMPSCUI_LIT( "hour [%d], minu [%d]" ), loginTime.Hour(),
                loginTime.Minute() );
#endif


    TRAP( err, retVal =  iConnectionUI->LoginL( EIMPSConnClientIM ) );

    iPluginState = EIMPSAOPluginRunning;

    // regardless of the result of login, if there came a logout event during
    // the login, call DoLogout
    if ( iEventInQueue )
        {
        iEventInQueue = EFalse;
        DoLogout( iQueuedLogoutIsScheduledLogout );
        iQueuedLogoutIsScheduledLogout = EFalse;
        return;
        }

    // handle leaves with memory related errors
    CheckMemoryErrors( err );

    if ( err < KErrNone )
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "LoginL leaved with code [%d]" ), err );
        IMPSCUI_DP_TXT( "All leaves are handled as retval KErrCouldNotConnect" );
        // if the LoginL-call leaves set the return value to normal
        // return value when connection could not be made, to get reconnect
        // later working.
        retVal = KErrCouldNotConnect;
        }

    switch ( retVal )
        {
        case KErrAlreadyExists: // fallthrough
        case KErrNone:
            {
            // Login succeeded, reset the plugin states and counts
            IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::DoLogin successful try[%d]" ), iLoginCount + 1 );
            Reset();
            break;
            }
        case KErrNotFound: // fallthrough
        case KErrArgument:
        case KErrNotSupported:
            {
            // wrong password, don't try to relogin, ConnectionUI shows the note
            Reset();
            // reset because we can't retry the login if the username and / or
            // password are incorrect
            break;
            }

        case KErrAccessDenied: // fallthrough
        case KErrGeneral:
        case KErrCouldNotConnect:
            {
            // login failed with some network failure (time out or other network error) ->
            // wait and try again, show confirmation query if needed
            if ( iLoginCount == KFirstReconnectionRetryCycleLength )
                {
                iReconnectNoteHandler->ShowNote( this );
                }
            else if ( ( iLoginCount > KFirstReconnectionRetryCycleLength ) &&
                      ( ( ( iLoginCount - KFirstReconnectionRetryCycleLength )
                          % KRestReconnectionRetryCyclesLength ) == 0
                      )
                    )
                {
                iReconnectNoteHandler->ShowNote( this );
                }
            else
                {
                StartWaiting();
                iLoginCount++;
                }
            break;
            }
        case KErrCouldNotDisconnect:
            {
            // there was connection to different server than default
            // and user denied disconnecting
            iCurServerDisconnectDenied = ETrue;
            }
        break;
        default:
            {
            // something else, undefined return value
            // these errors don't affect reconnection functionality
            // For just in case checking memory related errors
            CheckMemoryErrors( retVal );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::DoLogout
// Logs out from the wv server
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::DoLogout( TBool aIsScheduledLogout /* = EFalse */ )
    {
    IMPSCUI_DP_FUNC_ENTER( "DoLogout" );

    // do logout only if we are logged in
    TBool loggedIn( EFalse );
    TInt err( KErrNone );
    TRAP( err, loggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) );

    // Checking memory errors
    CheckMemoryErrors( err );

    if ( ( !loggedIn ) || err )
        {
        // not logged in just return
        // errors are handled as not logged in.
        return;
        }
    else if ( iPluginState == EIMPSAOPluginDoingLogout )
        {
        // emptying event queue,
        // because is currently doing logout, and we
        // want to be in logged out state after logout is done
        iEventInQueue = EFalse;
        iQueuedLogoutIsScheduledLogout = EFalse;
        return;
        }
    else if ( iPluginState == EIMPSAOPluginDoingLogin )
        {
        // currently doing login, adding logout action to
        // event queue
        iEventInQueue = ETrue;
        iQueuedLogoutIsScheduledLogout = aIsScheduledLogout;
        return;
        }

    iPluginState = EIMPSAOPluginDoingLogout;

    TInt retVal( KErrNone );
    TRAP( err, retVal = iConnectionUI->LogoutL( EIMPSConnClientIM,
                                                aIsScheduledLogout ) );

    // checking memory related errors
    // other errors are  ignored, because want only forward most
    // importand errors to framework
    CheckMemoryErrors( err );

    if ( retVal == KErrAccessDenied )
        {
        iCurServerDisconnectDenied = ETrue;
        }

    iPluginState = EIMPSAOPluginRunning;

    // regardless of the result of logout, if there came a login event during
    // the login, call DoLogout
    if ( iEventInQueue )
        {
        iEventInQueue = EFalse;
        iQueuedLogoutIsScheduledLogout = EFalse;
        DoLogin();
        }
    }

// ---------------------------------------------------------
// CIMAlwaysOnlinePlugin::CheckPluginNeed()
// Checks if the autologin option is set.
// ---------------------------------------------------------
//
TBool CIMPSAlwaysOnlinePluginImp::CheckPluginNeed()
    {
    IMPSCUI_DP_FUNC_ENTER( "CheckPluginNeed" );

    TInt readValue( KErrCouldNotConnect ); // initialize to an error value

    TInt err( iSharedDataServiceSettings->GetIntKey( iKeys.LoginTypeKey(), readValue ) );
    if ( ( err != KErrNone ) ||
         ( readValue < iKeys.MinLoginTypeEnum()  ) ||
         ( readValue > iKeys.MaxLoginTypeEnum() ) )
        {
        return EFalse;
        }

    iAutoLogin = readValue;

    IMPSCUI_DP_FUNC_DONE( "CheckPluginNeed" );
    if ( iShowRoamingWarning )
        {
        return ETrue;
        }
    else
        {
        return ( ( readValue == iKeys.LoginAutoAlwaysEnum() ) ||
                 ( readValue == iKeys.LoginAutoInHomeNWEnum() ) );
        }
    }


void CIMPSAlwaysOnlinePluginImp::SetShowRoamingWarning( TBool aShowRoamWarning )
    {
    iShowRoamingWarning = aShowRoamWarning;
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::CheckAllConnectionVariables
// Checks the plugin variables and makes a login or logout
// if needed.
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::CheckAllConnectionVariables()
    {
    IMPSCUI_DP_FUNC_ENTER( "CheckAllConnectionVariablesL" );

    IMPSCUI_DP( D_IMPSCUI_LIT( "iNetworkOpsAllowed = %d " ), iNetworkOpsAllowed );
    IMPSCUI_DP( D_IMPSCUI_LIT( "iUserControlledConnection = %d " ), iUserControlledConnection );
    IMPSCUI_DP( D_IMPSCUI_LIT( "iAutoLogin = %d " ), iAutoLogin );
    IMPSCUI_DP( D_IMPSCUI_LIT( "iScheduledLoginOk = %d " ), iScheduledLoginOk );
    IMPSCUI_DP( D_IMPSCUI_LIT( "iCurServerDisconnectDenied = %d " ), iCurServerDisconnectDenied );
    IMPSCUI_DP( D_IMPSCUI_LIT( "iRoaming = %d " ), iRoaming );

    // network operations not allowed
    if ( !IsNetworkOperationsAllowed() )
        {
        IMPSCUI_DP_TXT( "NetworkOps NOT allowed" );
        IMPSCUI_DP_FUNC_DONE( "CheckAllConnectionVariablesL" );
        return;
        }

    // autogin not set
    // -> don't do anything
    else if ( ( iAutoLogin != iKeys.LoginAutoAlwaysEnum() ) &&
              ( iAutoLogin != iKeys.LoginAutoInHomeNWEnum() ) )
        {
        IMPSCUI_DP_TXT( "autologin NOT set" );
        IMPSCUI_DP_FUNC_DONE( "CheckAllConnectionVariablesL" );
        return;
        }
    // we already know that autologin is set -> no need to check that
    // check if scheduled login not ok
    else if ( !iScheduledLoginOk )
        {
        IMPSCUI_DP_TXT( "Scheduled Login NOT on" );
        DoLogout( ETrue );
        }

    else if ( ( iAutoLogin == iKeys.LoginAutoAlwaysEnum() ) ||
              ( !iRoaming && ( iAutoLogin == iKeys.LoginAutoInHomeNWEnum() ) ) )
        {
        // If its Alwayslogin, roaing and roaing warning flag is enabled
        // then ask user if wants to login
        if ( ( iAutoLogin == iKeys.LoginAutoAlwaysEnum() ) &&
             iRoaming &&
             iShowRoamingWarning )
            {

            TBool isUserLoggedIn( EFalse );
            TRAP_IGNORE( isUserLoggedIn = IsUserLoggedInL() );
            // If the dialog is not displaying now then show it
            if ( !( iDlgMonitor &&
                    iDlgMonitor->IsActive() &&
                    !isUserLoggedIn ) )
                {
                TBool isRoamingDlgShown( EFalse );
                RProperty roaming;
                TInt error = roaming.Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, isRoamingDlgShown );
                if ( !isRoamingDlgShown && !error )
                    {
                    TRAP_IGNORE( HandleRoamingL() );
                    }
                }
            }
        else
            {
            DoLogin();
            TBool isLoggedIn( EFalse );
            TRAP_IGNORE( isLoggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) );
            if ( iIsToLogoutForRoaming && isLoggedIn )
                {
                iIsToLogoutForRoaming = EFalse;
                DoLogout();
                }
            }
        }
    // check the roaing is on and we are in autologin state
    else if ( ( iAutoLogin == iKeys.LoginAutoInHomeNWEnum() ) && iRoaming )
        {
        DoLogout();
        }
    IMPSCUI_DP_FUNC_DONE( "CheckAllConnectionVariablesL" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandlePresenceEventL
// Handles the presence event change
// ---------------------------------------------------------
//
#if 0
void HandlePresenceEventL( CIMPSConnUiPresEventNotifier* /*aNotifier*/,
                           const CPEngNWSessionSlotID2& /*aSessionSlotID*/,
                           TIMPSConnectionClient /*aClient*/,
                           TIMPSPresenceServiceEvent aEvent )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandlePresenceEventL" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aEvent [%d]" ), aEvent );

    // check the previous state before deciding what to do?
    if ( ( aEvent == EPEngEventAppNWPresenceSessionClosed ) &&
         ( iPreviousEvent == EPEngPresenceServiceNetworkDown ) )
        {

        CheckAllConnectionVariables();
        }
    iPreviousEvent = aEvent;

    IMPSCUI_DP_FUNC_DONE( "HandlePresenceEventL" );
    }
#endif

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandlePermanentKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CIMPSAlwaysOnlinePluginImp::HandlePermanentKeyNotifyL( const TUid /* aUid */,
                                                             const TIMPSSharedKeys /* aKey */ )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandlePermanentKeyNotifyL" );
    IMPSCUI_DP_FUNC_DONE( "HandlePermanentKeyNotifyL" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleTemporaryKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleTemporaryKeyNotifyL( const TUid aUid,
                                                            const TIMPSSharedKeys aKey )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleTemporaryKeyNotifyL" );
    if ( ( aUid == KIMPSServSettNotifyUid ) &&
         ( aKey == iKeys.LoginTypeChangedKey() ) )
        {
        CheckPluginNeed();
        CheckAllConnectionVariables();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleTemporaryKeyNotifyL" );
    }

#if 0
// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleEventNotifyError
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandlePresenceEventNotifyError(
    CIMPSConnUiPresEventNotifier* /*aNotifier*/,
    TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::HandleEventNotifyError [%d]" ), aError );
    // check memory errors, other errors ignored
    CheckMemoryErrors( aError );
    }
#endif

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleScheduleEvent
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleScheduleEvent( TBool aIsScheduledLoginOk )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleScheduleEvent" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aIsScheduledLoginOk [%d]" ), aIsScheduledLoginOk );
    iUserControlledConnection = EFalse;
    iScheduledLoginOk = aIsScheduledLoginOk;
    if ( !aIsScheduledLoginOk )
        {
        iReconnectNoteHandler->CancelNoteShowing( this );
        }
    CheckPluginNeed(); // just in case that login type has changed same time
    CheckAllConnectionVariables();
    IMPSCUI_DP_FUNC_DONE( "HandleScheduleEvent" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleNwStatusChangeL
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleNwStatusChange( TBool aNwAvailable )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleNwStatusChange" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aNwAvailable [%d]" ), aNwAvailable );

    if ( aNwAvailable )
        {
        CheckAllConnectionVariables();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleNwStatusChange" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleNoteAnswered
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleNoteAnswered( TInt aAnswer )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleNoteAnswered" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aAnswer [%d]" ), aAnswer );

    if ( aAnswer == EAknSoftkeyYes )
        {
        iLoginCount++; // competing login "round" to
        // avoid note showing too early next time
        CheckAllConnectionVariables();
        }
    else
        {
        Reset();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleNoteAnswered" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleNoteCancelled
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleNoteCancelled()
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleNoteCancelled" );
    IMPSCUI_DP_FUNC_DONE( "HandleNoteCancelled" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleRoamingChangeL
// Handles network change event
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::SetRoaming( TBool aRoamingStatus )
    {
    IMPSCUI_DP_FUNC_ENTER( "SetRoaming" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aRoamingStatus [%d]" ), aRoamingStatus );
    iRoaming = aRoamingStatus;

    CheckAllConnectionVariables();
    IMPSCUI_DP_FUNC_DONE( "SetRoaming" );
    }


//----------------------------------------------------------
// Checks if setting is Auto login in home n/w
//----------------------------------------------------------
//
TBool CIMPSAlwaysOnlinePluginImp::IsLoginAutoInHomeNW()
    {
    if ( iAutoLogin == iKeys.LoginAutoInHomeNWEnum() )
        {
        return ETrue;
        }

    return EFalse;
    }


//----------------------------------------------------------
// Checks if user is logged in or not
//----------------------------------------------------------
//
TBool CIMPSAlwaysOnlinePluginImp::IsUserLoggedInL()
    {
    if ( iConnectionUI )
        {
        return iConnectionUI->LoggedInL( EIMPSConnClientIM );
        }
    return EFalse;
    }


//----------------------------------------------------------
// Handles the roaing status
// Handles roaing status
//----------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleRoamingL()
    {
    if ( !iGlobalMsgQuery )
        {
        iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
        }
    iGlobalMsgQuery->CancelMsgQuery();
    iDlgMonitor->Start();

    if ( !iRoamingQueryText )
        {
        iRoamingQueryText = ReadResourceLC( R_QTN_NOTE_ROAMING_ORANGE );
        CleanupStack::Pop();
        }

    if ( !iRoamingQueryTitle )
        {
        iRoamingQueryTitle = ReadResourceLC( R_QTN_NOTE_TITLE_ROAMING_ORANGE );
        CleanupStack::Pop();
        }

    iGlobalMsgQuery->ShowMsgQueryL( iDlgMonitor->iStatus, *iRoamingQueryText,
                                    R_AVKON_SOFTKEYS_YES_NO,
                                    *iRoamingQueryTitle ,
                                    KNullDesC );
    }


//----------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::CommonDialogDismissedL()
// Handle user action for roaing warning dialog.
//----------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::CommonDialogDismissedL( const TInt aStatusCode )
    {
    // Handle user action for roaing warning.
    switch ( aStatusCode )
        {
        case EAknSoftkeyNo:
            {
            if ( iConnectionUI->LoggedInL( EIMPSConnClientIM ) )
                {
                DoLogout();
                }
            else
                {
                iIsToLogoutForRoaming = ETrue;
                }
            break;
            }
        case EAknSoftkeyYes:
            {
            // Set the key that warning is shown once for session
            User::LeaveIfError( RProperty::Set( KIMPSConnUiTemporaryUid,
                                                EIMPSSharedKeysRoamingDlgShown,
                                                ETrue ) );

            // Do nothing if user is already logged in. Else login
            if ( !iConnectionUI->LoggedInL( EIMPSConnClientIM ) )
                {
                DoLogin();
                }
            }
        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAlwaysOnlinePlugin::ReadResourceLC()
// -----------------------------------------------------------------------------
//
HBufC* CIMPSAlwaysOnlinePluginImp::ReadResourceLC( TInt aResourceId )
    {
    //Own resource reader for AA plug-in environment (no CONE facilities available)
    TInt plainResourceId = 0x00000fff & aResourceId; // Remove offset from id

    HBufC8* rawDataBuf = iResFile.AllocReadLC( plainResourceId );

    //raw data buffer is actually unicode text ==> treat it so
    TPtrC16 rawData( ( TUint16* ) rawDataBuf->Ptr(),
                     rawDataBuf->Length() / 2 );

    HBufC16* resourceData = rawData.AllocL();
    CleanupStack::PopAndDestroy( rawDataBuf );

    CleanupStack::PushL( resourceData );

    return resourceData;
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleTimeWaited
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleTimeWaited()
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleTimeWaited" );
    CheckAllConnectionVariables();
    IMPSCUI_DP_FUNC_DONE( "HandleTimeWaited" );
    }

void CIMPSAlwaysOnlinePluginImp::HandleTimeWaitedError( TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::HandleTimeWaitedError [%d]" ), aError );
    if ( aError != KErrCancel )
        {
        CheckAllConnectionVariables();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleTimeWaitedError" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleConnectionModeEventL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleConnectionModeEventL(
    CIMPSConnUiConnectionModeNotifier* aNotifier,
    TIMPSConnectionModeEvent aEvent )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleConnectionModeEventL" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aEvent [%d]" ), aEvent );
    if ( aNotifier == iConnectionNotifier )
        {
        if ( aEvent == EIMPSCMEUserLevelLogin )
            {
            iUserControlledConnection = ETrue;
            // reset connection count and stop timer
            Reset();
            // no need for checking connection variables in this case --> return
            IMPSCUI_DP_FUNC_DONE( "HandleConnectionModeEventL" );

            if ( iIsToLogoutForRoaming )
                {
                iIsToLogoutForRoaming = EFalse;
                DoLogout();
                }
            return;
            }

        if ( aEvent == EIMPSCMEUserLevelLogout )
            {
            if ( iCurServerDisconnectDenied )
                {
                // logout from different server than default
                // because current server disconnect was denied
                // free to open connection to default server
                iUserControlledConnection = EFalse;
                }
            else
                {
                // logout from default server, user controlled
                // logout, because cannot make new login default
                // server until next connection start time.
                iUserControlledConnection = ETrue;
                }
            iCurServerDisconnectDenied = EFalse;
            }

        else if ( aEvent == EIMPSCMELastSSClientStop )
            {
            iUserControlledConnection = EFalse;
            iCurServerDisconnectDenied = EFalse;
            }
        CheckAllConnectionVariables();
        }
    IMPSCUI_DP_FUNC_DONE( "HandleConnectionModeEventL" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleConnectionModeEventNotifyError
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleConnectionModeEventNotifyError(
    CIMPSConnUiConnectionModeNotifier* aNotifier,
    TInt aError )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::HandleConnectionModeEventNotifyError [%d]" ), aError );

    CheckMemoryErrors( aError );

    if ( ( aError != KErrCancel ) && ( aError != KErrServerTerminated ) )
        {
        // must be trapped as the inherited method isn't leavable
        // leave can be ignored as there's nothing we can do about it
        if ( aNotifier == iConnectionNotifier )
            {
            TRAPD( err, iConnectionNotifier->StartL() );
            CheckMemoryErrors( err );
            }
        }
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::AllowNetworkOperations
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::SetNetworkOpsAllowed( TBool aAllowed )
    {
    IMPSCUI_DP_FUNC_ENTER( "SetNetworkOpsAllowed" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aAllowed [%d]" ), aAllowed );

    TBool previousNetworkOpsAllowed( iNetworkOpsAllowed );
    iNetworkOpsAllowed = aAllowed;
    // wait for a while before checking the variables after setting this on
    // because the network isn't available immediately
    // and only do it if we changed from offline to not-offline
    if ( !previousNetworkOpsAllowed && iNetworkOpsAllowed )
        {
        iWaitReason = EIMPSAOPluginWaitingForLogin;
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }

        TTimeIntervalMicroSeconds32 wTime( KWaitTimeFromOfflineToNotOffline );
        iTimer->After( wTime );
        }
    else
        {
        CheckAllConnectionVariables();
        }

    IMPSCUI_DP_FUNC_DONE( "SetNetworkOpsAllowed" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::StartPluginL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::InitializePluginL()
    {
    if ( !iReconnectNoteHandler )
        {
        iReconnectNoteHandler = CIMPSReconnectNoteHandler::NewL( EIMPSConnClientIM );
        }
    if ( !iCallObserver )
        {
        iCallObserver = CPEngAOCallObserver::NewL();
        }

    if ( !iScheduler )
        {
        iScheduler = CIMPSAlwaysOnlineScheduler::NewL( iKeys );
        iScheduler->SetObserver( this );
        }

    if ( !iTimer )
        {
        iTimer = CPEngAOPluginTimer::NewL();
        }

    if ( !iConnectionNotifier )
        {
        // get notifys when the connection mode changes
        iConnectionNotifier =
            CIMPSConnUiConnectionModeNotifier::NewL( EIMPSConnClientIM );
        iConnectionNotifier->AddObserverL( this );
        }

    iConnectionUI = CIMPSPresenceAAConnection::NewL( EIMPSConnClientIM );

    if ( !iErrorUI )
        {
        // error ui for error notes showing
        iErrorUI = CErrorUI::NewL();
        }
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::InitializePluginL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::FreeMemory()
    {
    delete iTimer;
    iTimer = NULL;

    delete iCallObserver;
    iCallObserver = NULL;

    delete iConnectionNotifier;
    iConnectionNotifier = NULL;

    delete iConnectionUI;
    iConnectionUI = NULL;

    delete iScheduler;
    iScheduler = NULL;

    delete iErrorUI;
    iErrorUI = NULL;

    delete iReconnectNoteHandler;
    iReconnectNoteHandler = NULL;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::InitializePluginL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::StartPluginL()
    {
    IMPSCUI_DP_FUNC_ENTER( "StartPluginL" );

    // start listening the shared data key events
    User::LeaveIfError(
        iSettingChangesNotifier->SubscribeSet( KIMPSServSettNotifyUid,
                                               iKeys.LoginTypeChangedKey() ) );

    // check need of plugin
    if ( CheckPluginNeed() )
        {
        InitializeSharedKeysL();
        InitializePluginL();

        // set this as the timer's observer
        iTimer->SetObserver( this );

        iConnectionNotifier->StartL();

        TRAPD( err, iScheduler->StartSchedulingL() );
        if ( err )
            {
            IMPSCUI_DP_TXT( "Scheduling start failed --> autologin NOT on" );
            iAutoLogin = iKeys.LoginManualEnum();
            Reset(); // reset reconnect functionality
            }
        iScheduledLoginOk = iScheduler->IsScheduledLoginOk();
        }
    else
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "AutoLogin NOT on --> stopping scheduling" ) );
        if ( iScheduler )
            {
            iScheduler->StopScheduling();
            iScheduledLoginOk = iScheduler->IsScheduledLoginOk();
            }
        Reset(); // reset reconnect functionality
        }

    IMPSCUI_DP_FUNC_DONE( "StartPluginL" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::StopPlugin
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::StopPlugin()
    {
    IMPSCUI_DP_FUNC_ENTER( "StopPlugin" );

    if ( iScheduler )
        {
        iScheduler->StopScheduling();
        }

    // stop listening to presence events
    if ( iConnectionNotifier )
        {
        iConnectionNotifier->Stop();
        }

    iSettingChangesNotifier->UnSubscribe( KIMPSServSettNotifyUid,
                                          iKeys.LoginTypeChangedKey() );
    if ( iCallObserver )
        {
        iCallObserver->StopObserving();
        }

    if ( iTimer )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        }

    iNwObserver->StopObserving();

    if ( iConnectionUI )
        {
        DoLogout();
        }

    FreeMemory();
    IMPSCUI_DP_FUNC_DONE( "StopPlugin" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::Reset()
// Resets the plugin
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::Reset()
    {
    IMPSCUI_DP_FUNC_ENTER( "Reset" );

    iLoginCount = 1; // first login
    iPluginState = EIMPSAOPluginRunning;
    if ( iTimer )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        }
    IMPSCUI_DP_FUNC_DONE( "Reset" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::StartWaiting
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::StartWaiting()
    {
    IMPSCUI_DP_FUNC_ENTER( "StartWaiting" );

    TTimeIntervalMinutes waitInMinutes( KDefaultLoginWaitTime );

    // then add the wait time according to what login try it is
    if ( iLoginCount < KMaxNumberOfDefinedWaitTimes )
        {
        waitInMinutes = KLoginWaitTime[iLoginCount];
        }


    TTime timeToKickIn;

    // get the what the time is right now
    timeToKickIn.HomeTime();

    if ( timeToKickIn >= ( iTimeBeforeLoginTry + waitInMinutes ) )
        {
        // login attempt has
        // taken more time than defined wait time
        // --> starting new login immediately
        IMPSCUI_DP_TXT( "Login attempt has taken more time than wait time" );
        IMPSCUI_DP_TXT( "Check all connection variables immediately" );
        CheckAllConnectionVariables();
        IMPSCUI_DP_FUNC_DONE( "StartWaiting" );
        return;
        }

    if ( timeToKickIn <= iTimeBeforeLoginTry )
        {
        // clock turned backwards during login attempt
        // so current time used as base for calculations of new login attempt
        IMPSCUI_DP_TXT( "Clock turned backwards during login attempt" );
        IMPSCUI_DP_TXT( "Using current time as base for new login attempt calculations" );
        timeToKickIn += waitInMinutes;
        }
    else
        {
        IMPSCUI_DP_TXT( "Normal situation" );
        IMPSCUI_DP_TXT( "Using time before login as base for new login attempt calculations" );
        timeToKickIn = ( iTimeBeforeLoginTry + waitInMinutes );
        }

    // and start waiting
    iWaitReason = EIMPSAOPluginWaitingForLogin;
    StartTimer( timeToKickIn );
    IMPSCUI_DP_FUNC_DONE( "StartWaiting" );
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::StartTimer
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::StartTimer( TTime aTime )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CIMPSAlwaysOnlinePluginImp::StartTimer" ) );

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
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::HandleCallEnded
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::HandleCallEndedL( TBool aEnded )
    {
    IMPSCUI_DP_FUNC_ENTER( "HandleCallEndedL" );
    IMPSCUI_DP( D_IMPSCUI_LIT( "aEnded [%d]" ), aEnded );

    if ( aEnded )
        {
        CheckAllConnectionVariables();
        }

    IMPSCUI_DP_FUNC_DONE( "HandleCallEndedL" );
    }


// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::CheckMemoryErrors
//
// ---------------------------------------------------------
//
TBool CIMPSAlwaysOnlinePluginImp::CheckMemoryErrors( TInt aError )
    {
    TInt retval( EFalse );
    if ( ( aError == KErrNoMemory ) || ( aError == KErrDiskFull ) )
        {
        // Leaves on errornote showing are ignored, because
        // cannot do anymore if errorhandling itself fails.
        TInt ignore;
        TRAP( ignore, iErrorUI->ShowGlobalErrorNoteL( aError ) );
        retval = ETrue;
        }
    return retval;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::IsNetworkOperationsAllowed
//
// ---------------------------------------------------------
//
TBool CIMPSAlwaysOnlinePluginImp::IsNetworkOperationsAllowed()
    {
    TInt retval( ETrue );

    // Network operations are not allowed
    // the user controlled login / logout event is active or
    // network is not available
    // or with previous login attempt disconnect from previous
    // server ( other than default, which ao plugin is using )
    // was denied
    if ( !iNetworkOpsAllowed ||
         iUserControlledConnection ||
         !iNwObserver->IsNetworkAvailable() ||
         iCurServerDisconnectDenied )
        {
        retval = EFalse;
        }
    return retval;
    }

// ---------------------------------------------------------
// CIMPSAlwaysOnlinePluginImp::InitializeSharedKeysL
//
// ---------------------------------------------------------
//
void CIMPSAlwaysOnlinePluginImp::InitializeSharedKeysL()
    {
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysServSettLoginTypeChangedPEC,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysServSettLoginTypeChangedIM,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysServSettScheduleChangedPEC,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysServSettScheduleChangedIM,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMClientLoginLogoutStateChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMLoginLogoutEventChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMSSClientReqistrationChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMGlobalOperationSignalChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMRemoteUiNotificationsChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECClientLoginLogoutStateChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECLoginLogoutEventChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECSSClientReqistrationChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECGlobalOperationSignalChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECRemoteUiNotificationsChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonClientLoginLogoutStateChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonLoginLogoutEventChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonSSClientReqistrationChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonGlobalOperationSignalChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonRemoteUiNotificationsChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );

    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMClientLoginLogoutStateData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMLoginLogoutEventData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMSSClientReqistrationData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMGlobalOperationSignalData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMRemoteUiNotificationsData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECClientLoginLogoutStateData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECLoginLogoutEventData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECSSClientReqistrationData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECGlobalOperationSignalData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECRemoteUiNotificationsData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonClientLoginLogoutStateData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonLoginLogoutEventData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonSSClientReqistrationData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonGlobalOperationSignalData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysCommonRemoteUiNotificationsData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );

    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMGlobalChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECGlobalChannel,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );


    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysIMGlobalData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysPECGlobalData,
                       RProperty::EByteArray,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );

    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysRoaming,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );

    RProperty::Define( KIMPSConnUiTemporaryUid,
                       EIMPSSharedKeysRoamingDlgShown,
                       RProperty::EInt,
                       KIMPSSharedReadPolicy, KIMPSSharedWritePolicy );
    }

//  End of File
