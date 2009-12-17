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
* Description:  IMPS Connection UI public constants.
*
*/

#ifndef __IMPSPRESENCECONNECTIONUICONSTSNG_H
#define __IMPSPRESENCECONNECTIONUICONSTSNG_H

// INCLUDES
#include <E32Std.h>
#include "ChatNGCommonUIDs.h"

/**
 * Client application enumeration.
 */
enum TIMPSConnectionClient
    {
    EIMPSConnClientPEC = 0x1,   ///<Client belongs to PEC application group.
    EIMPSConnClientIM  = 0x2    ///<Client is IM application.
    };


/**
 * Login type enumeration.
 *
 * Defines behaviour wanted from
 * Connection UI.
 */
enum TIMPSLoginType
    {
    EIMPSManualLogin = 0x1,        ///<"Manual" login with server list query.
    EIMPSApplicationLaunch = 0x2,  ///< Application start time login. Direct login to SAPSettingsStore default server.
    EIMPSAAConnectionStart = 0x3   ///< AA Connection start. Starts AA connection for selected client to SAPSettingsStore default server.
    };


/**
 * Application level presence events.
 *
 * Application level presence events.
 * These events map 1 to 1 to the
 * PEC engine events.
 *
 * NOTE!! Further events may be added here later.
 */
enum TIMPSPresenceServiceEvent
    {
    EIMPSPresenceServiceUnknownEvent = -1,  ///<For not matched PEC engine events
    EIMPSPresenceServiceOffline = 0,        ///<Presence service not available
    EIMPSPresenceServiceOnline,             ///<Presence service available (logged in or network connection came back)
    EIMPSPresenceServiceSlotChanged,        ///<PEC engine network session slot has been changed
    EIMPSPresenceServiceSlotRemoved,        ///<PEC engine network session slot has been removed
    EIMPSPresenceServiceForceLogOut,        ///<Force log out from the IMPS network server
    EIMPSPresenceServiceTransport           ///<A transport specific event
    };


/**
 * Application exit type enumeration.
 *
 * Defines behaviour wanted from
 * Connection UI.
 */
enum TIMPSExitType
    {
    EIMPSNormalExit             = 0x1,  ///<Application close related queries are shown
    EIMPSForceExit              = 0x2,  ///<Application exit is handled silently without user interaction
    EIMPSLeaveSessionOpenExit   = 0x3,  ///<Application exit, network session is left open
    };


/**
 * Connection mode event enumerations.
 * NOTE!! Further events may be added here later.
 */
enum TIMPSConnectionModeEvent
    {
    EIMPSCMEUnknown         = -1,           ///<Connection UI hasn't recognized any particular connection mode change.

    ///<Login/logout events
    EIMPSCMEUserLevelSelectionEventGroup = 1,
    EIMPSCMEUserLevelLogout = 2,            ///<User has performed a logout trough the ConnectionUI's UI. (manual login)
    EIMPSCMEUserLevelLogin  = 3,            ///<User has performed a login trough the ConnectionUI's UI. (manual logout)
    EIMPSCMEUserLevelAAConnectionStart = 4, ///<User has started AA connection in UI level

    ///<Significant scheduling client start/stop events
    EIMPSCMESSClientEventGroup = 101,
    EIMPSCMEFirstSSClientStart = 102,       ///<First significant scheduling client started
    EIMPSCMELastSSClientStop   = 103        ///<Last significant scheduling client stopped
    };




/**
 * Connection settings event enumerations.
 * NOTE!! Further events may be added here later.
 */
enum TIMPSConnectionSettingsEvent
    {
    EIMPSCSEUnknown         = -1,   ///<Connection UI hasn't recognized any particular connection setting change.

    ///<Always Automatic (a.k.a Always Online) settings events
    EIMPSCSEAASettingsEventGroup = 1,
    EIMPSCSEAlwaysAutomaticOn    = 2,        ///<Always online has been enabled from IMPS connection settings
    EIMPSCSEAlwaysAutomaticOnInHNW = 3,      ///<Always online has been enabled from IMPS connection settings
    EIMPSCSEAlwaysAutomaticOff   = 4,        ///<Always online has been disabled from IMPS connection settings

    ///<Always Automatic scheduling events
    EIMPSCSEAASchedulingTimeEventGroup      = 101,
    EIMPSCSEAAScheduledConnectionStartTime  = 102,  ///<Scheduled AA connection start time passed
    EIMPSCSEAAScheduledConnectionEndTime    = 103,  ///<Scheduled AA connection end time passed
    };



/**
 * Connection operation enumerations.
 * NOTE!! Further events may be added here later.
 */
enum TIMPSConnectionOperation
    {
    EIMPSConnOppUnknown = -1,       ///<No particular operation ongoing
    EIMPSConnOppClientLogin = 2     ///<Client login ongoing
    };




/**
 * UI notification services available to clients.
 *
 * Available services (identified by their enum values)
 *
 * 1.When this service is started, allows UI notifications
 *   from active connection termination and suspends
 *   to be shown in host application context.
 *   Currently shows notifications only from force logout
 *   events. Notifications are shown only when the host
 *   application connection has been active and it is ended
 *   or suspended by some external reason.
 *   (For IM & PEC.)
 *
 * 2.When this service is started, allows queries about
 *   currently active network connection scheduled close
 *   to be shown in host application context when needed.
 *   Connection Agent will asks from user should this
 *   application group currently active network connection
 *   be closed right now as scheduled. User has 30 seconds timeout to
 *   react to query. If user selects "yes" or doesn't react to query,
 *   or the service isn't used by any client group application,
 *   network connection is closed as scheduled.
 *   (For IM when AA scheduling is supported.)
 *
 * 3.When this service is started, allows queries about
 *   currently active network connection termination for
 *   another scheduled connection to be shown in host
 *   application context when needed.
 *   Connection Agent will ask from user should the currently
 *   active network connection to be closed because a
 *   connection to another IMPS service is scheduled. If user selects
 *   "yes" or doesn't react to query, or the service isn't used by any
 *   client group applications, current network connection is closed
 *   and a connection to different IMPS service is opened as scheduled.
 *   (For IM & PEC when AA scheduling is supported.)
 *
 * 4.When this service is started, the connection client is
 *   registered and counted as "significant" connection client
 *   in scheduling.
 *   (For IM & PEC when AA scheduling is supported.)
 *
 * @since 2.1
 */
enum TIMPSConnectionUiAgentService
    {
    EIMPSNotifyActiveConnectionLostReasons = 1,
    EIMPSQueryActiveConnectionScheduledClose = 2,
    EIMPSQueryActiveConnectionSuppressForScheduledConnection = 3,
    EIMPSReqisterAsSignificantConnectionClientInScheduling = 4
    };



/**
 * Connection UI panic panic reasons and category
 * Not for use. Only for debug purposes.
 */
enum TIMPSConnectionUiPanicReason
    {
    EIMPSConnUi_NoCoeEnv = 0,
    EIMPSConnUi_UnknownNoteID = 1,
    EIMPSConn_NULLPtr = 2,
    EIMPSConnUi_UnknownClientType = 3,
    EIMPSConnOpContextUninitialized = 4,
    EIMPSConnStepDriverUninitialized = 5,
    EIMPSConnOpenerUnknownState = 6,
    EIMPSConnCloserUnknownState = 7
    };
_LIT( KIMPSConnUi_PanicCat, "PresConnUi" );

_LIT( KIMPSPECContactListIdentifier,  "~PEP1." );

_LIT( KIMPSSubscriptionList, "/~PEP1.0_subscriptions" );
_LIT( KIMPSPrivateList, "/~PEP1.0_privatelist" );
_LIT( KIMPSBlockList, "/~PEP1.0_blocklist" );
_LIT( KIMPSWatcherList, "watcherlist" );

// constant for AA client packing key
_LIT( KIMPSPackAAClientsKey, "IMPSAAClients" );

// constants for storing branding resource information
const TUid KBrandingUid = { KWVUIAVEEXEUIDS2 };
const TInt KBrandingResourceKey = 666;
const TInt KForcedLoginKey = 667;
const TInt KBrandingConnUIResourceKey = 668;
_LIT( KConnUiVariationResourceName, "IMConnectionUIVariationNG.rsc" );
_LIT( KConnUiResourceName, "IMPSConnectionUiNG.rsc" );
_LIT_SECURITY_POLICY_PASS( KIMPSPubSubReadPolicy );
_LIT_SECURITY_POLICY_PASS( KIMPSPubSubWritePolicy );

#endif      //__IMPSPRESENCECONNECTIONUICONSTSNG_H

//  End of File

