/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network session handling (login, logout, etc.)
*
*/


#ifndef CCASESSIONHANDLERCMD_H
#define CCASESSIONHANDLERCMD_H

//  INCLUDES
#include "PublicEngineDefinitions.h"
#include "MCASessionHandlerCmd.h"

#include <e32base.h>
#include <mimpsconnuipresenceeventobserverng.h>
#include <ImpsServices.h>
#include <mimpsconnprocessobserverng.h>
#include <badesca.h>
#include <WVSettingsSharedDataNG.h>
#include <PEngWVServices2.h>
#include "MCABackgroundTaskObserver.h"
#include "MCALoginRefreshObserverPC.h"
// FORWARD DECLARATIONS
class CIMPSPresenceConnectionUi;
class CIMPSConnUiClntFilteredPresEventNotifier;
class CIMPSSAPSettings;
class MProfileEngine;
class CPEngNWSessionSlotID2;
//class CCAReactiveAuthorization; //for ref. later
class MCAContactFetchObserver;
class MCAServiceStateObserver;
class MCAProcessManager;
class MCAUiLoginCmdCB;


// CLASS DECLARATION

/**
*  Network session handling (login, logout, etc.)
*
*/
class CCASessionHandlerCmd : public CBase, public MIMPSConnUiPresenceEventObserver,
            public MIMPSConnProcessObserver,
            public MCASessionHandlerCmd,
            public MCABackGroundTaskObserver

    {
    public: // Enums
        /**
         * Use to ask if a feature is supported or not
         * EGroup:  Group feature
         * ESearch: Search feature
         * EInvite: Invite feature
         * EBlock:  Block feature
         * EAttrList: Attribute List Function (defined in Preseence Engine)
         * ECLIMod: Support for create contact list
         * EGroupRejectList:  Support for reject user(s) from group
         */
        enum TSupportedFeature
            {
            EGroup = 1,
            ESearch,
            EInvite,
            EBlock,
            EAttrList,
            ECLIMod,
            EGroupRejectList
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aProcessManager         Reference to process Manager component
        * @param aUiLoginCmdCB			 Reference to UI Login CallBack interface
        */
        static CCASessionHandlerCmd* NewL( MCAProcessManager& aProcessManager,
                                           MCAUiLoginCmdCB& aUiLoginCmdCB );

        /**
        * Destructor.
        */
        virtual ~CCASessionHandlerCmd();

    public: // New functions



        /**
        * Is IM logged to network server
        *
        * @return Is IM online
        */
        TBool IsLoggedIn() const;

        /**
        * Executes login-sequence.
        *
        * @param aAskConfirmation if true yes/no confirmation
        *        qtn_chat_need_to_login is shown.
        * @param aIsStartupLogin if true login call is done during
        *        application startup
        * @return Was operation successful
        */
        TBool LoginL(   TBool aAskConfirmation = ETrue,
                        TBool aIsStartupLogin = EFalse );

        /**
        * Executes logout-sequence.
        *
        * @return Was operation successful
        */
        TBool LogoutL();




        /**
        * Returns is the given feature supported by the session
        *
        * @param aFeature Feature
        * @return Is supported
        */
        TBool IsSupported( TSupportedFeature aFeature );

        /**
         * @return Current SAP data
         */
        CIMPSSAPSettings* CurrentSAPLC();

        /**
        * Checks if phone offline profile is phone current profile.
         *
        * @return ETrue if off-line profile is phone current profile
        */
        TBool IsOfflineProfileOn() const;

        /**
         * Fetches the contact lists
         * @param aAlwaysOnlineActive determines if login was made with AO or
         *        manually. This parameter is used only if background handling
         *        is active.
         *        ETrue: Login was done with AlwaysOnline
         *        EFalse: Login was done with Manual login type
         */
        void FetchContactListsL( TBool aAlwaysOnlineActive );

        /**
         * @return The current network session slot id
         */
        const CPEngNWSessionSlotID2& NetworkSessionSlotIDL();

        /**
        * Does the application exit
        */
        void DoApplicationExitL();

    public:  //From Base Class MCASessionHandlerCmd
        /**
         * Add given observer to observer list
         * @param aObserver to add
         * @return none
         */
        void AddContactFetchObserverL( MCAContactFetchObserver* aObserver );

        /**
         * Remove given observer from observer list
         * @param aObserver observer to remove
         * @return none
         */
        void RemoveContactFetchObserver( MCAContactFetchObserver* aObserver );

        /**
         * Add given observer to observer list
         * @param aObserver to add
         * @return none
         */
        void AddServiceStateObserversL( MCAServiceStateObserver* aObserver );

        /**
         * Remove given observer from observer list
         * @param aObserver observer to remove
         * @return none
         */
        void RemoveServiceStateObservers( MCAServiceStateObserver* aObserver );

        /**
         * Check if contact fetching is done
         * @return ETrue if done, EFalse otherwise
         */
        TBool IsFetchDone();

        /**
         * @return ETrue if the contact list has been refreshed
         *         during this session
         */
        TBool ContactsRefreshed() const;

        /**
         * Gets the IsAlreadyLoggedOut flag value
         * @return ETrue if Already Logged Out
         */
        TBool IsAlreadyLoggedOut() const;


        /**
         * Sets the contact refreshed flag
         * @param aRefreshed ETrue if the contact list has been refreshed
         */
        void SetContactsRefreshed( TBool aRefreshed );


        /**
         * Checks if the background login is running.
         * @return ETrue if the background login is running.
         */
        TBool IsBackgroundTaskRunning() const;

    protected: // Functions from base classes

        /**
        * @see MIMPSConnUiPresenceEventObserver
        */
        void HandlePresenceEventL( CIMPSConnUiPresEventNotifier* aNotifier,
                                   const CPEngNWSessionSlotID2& aSessionSlotID,
                                   TIMPSConnectionClient aClient,
                                   TIMPSPresenceServiceEvent aEvent );

        /**
        * @see MIMPSConnUiPresenceEventObserver
        */
        void HandlePresenceEventNotifyError(
            CIMPSConnUiPresEventNotifier* aNotifier,
            TInt aError );

    private: // From MIMPSConnProcessObserver

        /**
        * @see MIMPSConnProcessObserver
        */
        void HandleSapAccessEventEventL( const TDesC& aServerName,
                                         const TDesC& aServerURL,
                                         MIMPSConnProcessUi* aConnProcessUi );


    private:

        /**
        * C++ default constructor.
        */
        CCASessionHandlerCmd( MCAProcessManager& aProcessManager,
                              MCAUiLoginCmdCB& aUiLoginCmdCB );


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Gets the current login type
         * @return the login type
         */
        TIMPSLoginType LoginType();

        /**
         * CIdle-kind of function for background task implementation
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TInt BackgroundTasks( TAny *aInstance );

        /**
         * Real implementation of background tasks
         * @return Need for recall
         */
        TInt DoBackgroundTasks();

        /**
         * CIdle-kind of function for background task implementation
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TInt BackgroundSelect( TAny *aInstance );

        /**
         * Real implementation of background tasks
         */
        void DoBackgroundSelectL();

        /**
        * Gets available services for session
        *
        * @return Does the screen need refreshing, i.e. if some dynamic UI
        * features have changed.
        */
        TBool GetServicesL();

        /**
        * Notifies engine about changes network state
        *
        * @param aState New state
        */
        void NotifyEngineL( TNetworkState aState,
                            CPEngNWSessionSlotID2* aSessionSlotID = NULL );

        /**
         * Closes all open conversations
         */
        void CloseConversations();

        /**
         * Update the first login toggle in the SAP settings.
         */
        void UpdateFirstLoginL();
        /**
         * Updates the application settings according to current
         * variation.
         *
         */
        void UpdateDefaultSettingsL();

        /**
         * Notify observers of completeted fetch process
         * @param aError result of contact fetch
         * @return none
         */
        void NotifyContactFetchObserversL( TInt aError );

        /**
         * Notify observers of service state change
         * @param aEvent Presence event
         * @return none
         */
        void NotifyServiceStateObserversL( TIMPSPresenceServiceEvent aEvent );

        /**
         * To know whether Background task has started
         * @return TBool
         */
        TBool IsBackgroundTaskPendingL();

        /**
         * DoLogoutL
         * @return TBool
         */
        TBool DoLogoutL();

        /**
         * WaitToCompleteBackgroundTask
         * Pop the wait note processing
         * @return ETrue
         */
        TBool WaitToCompleteBackgroundTaskL();


    public :

        /**
        * Notify observers of background state change
        * MCABackGroundTaskObserver
        * @param aCompleted is state
        * @return none
        */
        void HandleBackGroundTaskCompleteL( TBool aCompleted ) ;


    private:    // Data


        // Owns. Connection UI
        CIMPSPresenceConnectionUi* iConnUI;

        // Owns. Event notifier from ConnectionUI
        CIMPSConnUiClntFilteredPresEventNotifier* iConnUIEventNotifier;

        // Negotiated services
        TPEngWVCspServicesTree2 iImpsServices;

        // Last state of application
        TIMPSPresenceServiceEvent iLastState;

        // Owns. CIdle to launch login
        CIdle* iIdle;

        // Owns. CIdle to launch contact list selectiong
        CIdle* iSelectIdle;

        // Doesn't own. Profile API  checking if offline profile is on
        MProfileEngine* iProfileApi;

        // contact list refresh flag
        TBool iContactsRefreshed;

        // owns. SAP settings of previously logged in server
        CIMPSSAPSettings* iPreviousSAP;

        // For example phone call when chat is online.
        TBool iTemporaryNetworkDown;


        // if the login or logout process is ongoing
        TBool iLogging;

        // if application is already logged in and we are
        // fetching lists, this is set.

        // Network session slot ID
        CPEngNWSessionSlotID2* iSessionSlotID;



        // Was the presence initialization succesfull in construction phase.
        TBool iPresenceInitializationSuccesfull;

        // current login is the first one to this server
        // (set by UpdateFirstLoginL)
        TBool iCurrentLoginIsFirst;

        // Observers for contact fetching process.
        RPointerArray<MCAContactFetchObserver> iFetchObservers;

        // ETrue if contacts are fetched from PEC engine
        TBool iIsContactFetchDone;

        // Observers for service state changes.
        RPointerArray<MCAServiceStateObserver> iServiceStateObservers;

        //Process manager
        MCAProcessManager& iProcessManager;

        // Background task  done or not
        TBool iBackgroundTaskPending;

        //UI Login Call back Interface
        MCAUiLoginCmdCB& iUiLoginCmdCB;

        // if application is already logged out and
        // we do not have that information yet from
        // connection ui, we can use this flag.
        TBool iAlreadyLoggedOut;

        // wait vivible or not
        TBool iWaitVisible;


        TBool iBackgroundTaskRunning;
    };

#endif      // CCASESSIONHANDLERCMD_H

// End of File

