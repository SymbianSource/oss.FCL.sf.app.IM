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



#ifndef __CIMPSALWAYSONLINEPLUGINIMP_H
#define __CIMPSALWAYSONLINEPLUGINIMP_H

//  INCLUDES
#include "MPEngAOWaitObserver.h"
#include "MPEngAOCallObserver.h"
#include "MIMPSScheduleObserver.h"
#include "MPEngAONwStatusObserver.h"
#include "MIMPSSharedDataObserver.h"
#include "MIMPSSharedData.h"
#include "MIMPSReconnectNoteObserver.h"
#include "timpsaokeys.h"
#include "MPEngAODialogStatusNotifier.h"

//#include <SharedDataClient.h>
#include <WVSettingsSharedDataNG.h>
#include <mimpsconnuipresenceeventobserverng.h>
#include <mimpsconnuiconnectionmodeobserverng.h>
#include <f32file.h>
#include <barsc.h>

// FORWARD DECLARATIONS
class CIMPSConnUiConnectionModeNotifier;
class CIMPSPresenceAAConnection;
class CPEngAOCallObserver;
class CPEngAOPluginTimer;
class CWVSAPSettings;
class CIMPSAOKeys;
class CIMPSAlwaysOnlineScheduler;
class CPEngAONwStatusObserver;
class CIMPSReconnectNoteHandler;
class CErrorUI;
class CPEngNWSessionSlotID2;
class CIMPSConnUiPresEventNotifier;
class CPEngAODialogMonitor;
class CCADialogMonitor;
class CAknGlobalMsgQuery;
/**
*  The implementation for the always online plugin
*
*  @lib PEngAOPlugin.lib
*  @since 2.6
*/
class CIMPSAlwaysOnlinePluginImp : public CBase,
            public MIMPSConnUiConnectionModeObserver,
            public MPEngAOWaitObserver,
            public MPEngAOCallObserver,
            public MIMPSScheduleObserver,
            public MPEngAONwStatusObserver,
            public MIMPSSharedDataObserver,
            public MIMPSReconnectNoteObserver,
            public MPEngAODialogStatusNotifier
    {

    public:  // Enumerations

        /**
         * Plugin implementation type
         */
        enum TIMPSAOPluginImpType
            {
            EIMAOPluginImp = 1,
            EPEngAOPluginImp = 2
            };

        /**
        * The internal state of the plugin
        */
        enum TIMPSAOPluginState
            {
            EIMPSAOPluginRunning = 0,
            EIMPSAOPluginDoingLogin = 1,
            EIMPSAOPluginDoingLogout = 2
            };

        /**
        * The waiting state of the plugin
        */
        enum TIMAOPluginWaitReason
            {
            EIMPSAOPluginNotWaiting = 0,
            EIMPSAOPluginWaitingForLogin = 1,
            EIMPSAOPluginWaitingForReconnect = 2,
            EIMPSAOPluginWaitingForDisconnect = 3
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aType Plugin implementation type
        */
        static CIMPSAlwaysOnlinePluginImp* NewL();

        /**
        * Destructor.
        */
        virtual ~CIMPSAlwaysOnlinePluginImp();


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

    public: // Functions from MPEngAOCallObserver

        /**
        * @see MPEngAOWaitObserver
        */
        void HandleCallEndedL( TBool aEnded );

    public: // Functions from MIMPSConnUiConnectionModeObserver

        /**
        * @see MIMPSConnUiConnectionModeObserver
        */
        void HandleConnectionModeEventL( CIMPSConnUiConnectionModeNotifier* aNotifier,
                                         TIMPSConnectionModeEvent aEvent );

        /**
        * @see MIMPSConnUiConnectionModeObserver
        */
        void HandleConnectionModeEventNotifyError( CIMPSConnUiConnectionModeNotifier* aNotifier,
                                                   TInt aError );

    public: // Functions from MIMPSScheduleObserver
        /**
        * @see MIMPSScheduleObserver
        */
        void HandleScheduleEvent( TBool aIsScheduledLoginOk );

    public: // Functions from MPEngAONwStatusObserver
        /**
         * @see MPEngAONwStatusObserver
         */
        void HandleNwStatusChange( TBool aNwAvailable );

    public: // Functions from MIMPSReconnectNoteObserver
        /**
        * This call is made when user has answered to global note
        * @param TInt aAnswer result for answer
        */
        void HandleNoteAnswered( TInt aAnswer );

        /**
         * This call Is made when note showing ins canceled for some reason
         */
        void HandleNoteCancelled();

    public: // Functions from MCADialogStatusNotifier
        /**
        * Interface function from CCADialogMonitor
        * Command ID's can be found in alwaysonlinemanagercommon.h
        * Handles the command coming from the Always Online Manager
        * @param aStatusCode The Status code as an integer.
        * @return void
        */
        void CommonDialogDismissedL( const TInt aStatusCode );


    private:  // New functions

        /**
        * Does the login using the Connection UI
        */
        void DoLogin();

        /**
        * Does the logout using the Connection UI
        */
        void DoLogout( TBool aIsScheduledLogout = EFalse );

        /**
        * Starts the waiting timer
        * @param aTime time when timer is launched.
        */
        void StartTimer( TTime aTime );

        /**
        * Reads TBUF resource with given id
        * @param aResourceId TBUF resource id
        * @return read resource as string
        */
        HBufC* ReadResourceLC( TInt aResourceId );

    public:

        /**
        * Sets the flag for allowing the network operations on or off
        * @param aAllowed The state of allowing operations to network
        */
        void SetNetworkOpsAllowed( TBool aAllowed );

        /**
        * Starts the plugin and all of it's notifiers.
        */
        void StartPluginL();

        /**
        * Stops the plugin and all of it's notifiers.
        */
        void StopPlugin();

        /**
         * Check if always online plugin is needed or not.
         * @return ETrue if needed. EFalse if not.
         */
        TBool CheckPluginNeed();

        /**
        * Sets the roaing status on or off
        * @aRoamingStatus ETrue if roaing is on, else EFalse
        */
        void SetRoaming( TBool aRoamingStatus );

        /**
        * Handles the roaing status
        */
        void HandleRoamingL();

        /**
        * Sets whether the roaing warning is to be shown or not
        */
        void SetShowRoamingWarning( TBool aShowRoamWarning );

        /**
        * Check if user is logged in.
        * @return ETrue if Logged in. EFalse if not.
        */
        TBool IsUserLoggedInL();

        /**
        * Check if current login type setting is 'Auto in home n/w" .
        * @return ETrue if it is Auto in home n/w. EFalse otherwise.
        */
        TBool IsLoginAutoInHomeNW();

        /**
        * Mark the testing class a friend
        */
        friend class T_CIMPSAlwaysOnlinePluginImp; // CSI: 36 # Only used for testing

    private:

        /**
        * C++ default constructor.
        */
        CIMPSAlwaysOnlinePluginImp();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aType Plugin implementation type
        */
        void ConstructL();

        /**
        * Resets the AO Plugin states
        */
        void Reset();

        /**
        * Checks all the connection variables (home network, offline profile
        * etc. etc.) and does the login if everything is set so
        */
        void CheckAllConnectionVariables();

        /**
        * Starts the waiting for next login, uses the CPEngAOPluginTimer
        */
        void StartWaiting();

        /**
         * Checks if error is KErrNoMemory or KErrDiskFull and
         * forwards errors to current active scheduler
         * @param aError given error
         * @return returns ETrue if aError is KErrNoMemory or KErrDiskFull
         */
        TBool CheckMemoryErrors( TInt aError );

        /**
         * Checks if networks operations (login, logout) are currently allowed
         * @return ETrue if network operations are allowed.
         */
        TBool IsNetworkOperationsAllowed();

        /**
         * Initiates the shared keys used in IMPSCommonUI
         */
        void InitializeSharedKeysL();

        /**
         * Initiates members for active plugin.
         */
        void InitializePluginL();

        /**
         * Free unneeded parts of plugin.
         */
        void FreeMemory();

    private:    /// Data
        /// Autologin state
        TInt iAutoLogin;

        /// Tells if the network ops are allowed
        TBool iNetworkOpsAllowed;

        /// The count of how many times we have tried to login
        TInt iLoginCount;

        /// Variable to store clock time before login try
        /// used to help count next try moment
        TTime iTimeBeforeLoginTry;

        /// The internal state of the AO Plugin
        TIMPSAOPluginState iPluginState;

        /// The roaing state
        TBool iRoaming;

        /// Shared data clients to read/notice common settings, owned
        MIMPSSharedData* iSharedDataServiceSettings;
        MIMPSSharedData* iSettingChangesNotifier;


        /// A pointer to the connection UI, owned
        CIMPSPresenceAAConnection* iConnectionUI;

        /// A pointers to the connection mode notifiers, owned
        CIMPSConnUiConnectionModeNotifier* iConnectionNotifier;

        /// A timer to wait before the relogin, owned
        CPEngAOPluginTimer* iTimer;

        /// An observer for the ongoing call, owned
        CPEngAOCallObserver* iCallObserver;

        /// Observer for network availability, owned
        CPEngAONwStatusObserver* iNwObserver;

        /// Defines the current wait reason of the Always Online plugin
        TIMAOPluginWaitReason iWaitReason;

        /// Tells if the scheduled login is in "should be logged in" state
        TBool iScheduledLoginOk;

        /// Keeps the state of user-made login / logouts
        TBool iUserControlledConnection;

        /// Store information about possible another connection event
        TBool iEventInQueue;
        TBool iQueuedLogoutIsScheduledLogout;

        /// Tells if with previous login attempt user denied
        /// disconnecting from server (other that default server)
        TBool iCurServerDisconnectDenied;

        /// Reserved pointer for future extension
        TAny* iReserved;

        /// Holds implementation type specific keys and enumerations
        TIMPSAOKeys iKeys;

        /// Manages always online scheduled logins/logouts, owned
        CIMPSAlwaysOnlineScheduler* iScheduler;

        /// Handler for showing glober confirmation query about reconnect
        /// continuation for IM, owned
        CIMPSReconnectNoteHandler* iReconnectNoteHandler;

        /// Used for showing global error notes, owned
        CErrorUI* iErrorUI;

        // Used for handling global msg query dialog
        CPEngAODialogMonitor* iDlgMonitor;

        // Global message query dialog. Used for roaing warning.
        CAknGlobalMsgQuery* iGlobalMsgQuery;

        // Holds the text message for roaing query
        HBufC* iRoamingQueryText;

        // Holds the titke of roaing query
        HBufC* iRoamingQueryTitle;

        // Flag indicating whether roaing warning is shown or not
        TBool iRoamingWarningShownAlready;

        // Flag indicating if roaing warning is to be shown
        TBool iShowRoamingWarning;

        // Flag indicating if the logout is , in case user doen't
        // want to login in Roaing
        TBool iIsToLogoutForRoaming;

        /// File server session, owned
        RFs iFs;

        /// IMPS AO plugin resource file, owned
        RResourceFile iResFile;
    };

#endif  // __CIMPSALWAYSONLINEPLUGINIMP_H

// End of File
