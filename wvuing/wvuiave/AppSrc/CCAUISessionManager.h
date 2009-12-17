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
* Description:  UI Session Manager - does the UI related things like branding
*                setting status pane icon etc during login, logout, etc.
*
*/


#ifndef CCAUISESSIONMANAGER_H
#define CCAUISESSIONMANAGER_H

//  INCLUDES
#include "PublicEngineDefinitions.h"

#include <e32base.h>
#include <badesca.h>
#include <PEngWVServices2.h>
#include "MCAUiLoginCmdCB.h"
#include <cimpspresenceconnectionuing.h> // for enum TIMPSLoginType
#include <WVSettingsSharedDataNG.h> // for enum TWVSettingsChatLoginValues

// FORWARD DECLARATIONS
class MCASkinVariant;
class MEikCommandObserver;
class CCAStatusPaneHandler;
class MCAOpBrandVariant;
class MCAViewSwitcher;
class CIMPSSAPSettings;
class MProfileEngine;
class CPEngNWSessionSlotID2;
class MCAContactFetchObserver;
class MCAServiceStateObserver;
class CCAReactiveAuthorization;



// CLASS DECLARATION

/**
* UI Session Manager - does the UI related things like branding
* setting status pane icon etc during login, logout, etc.
*/
class CCAUISessionManager : public CBase, public MCAUiLoginCmdCB



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
        * @param aConnUI		   Connection UI..
        * @param aSkin             Handle to skin variant for Series 60 Platform skin support
        * @param aBrand            Handle to operator branding variant
        *                          (UI branding)
        * @param aCommandObserver  For relaying exit-commands
        * @param aViewSwitcher     For view switching functionality (needed
        *                          to refresh currect view after branding)
        * @param aStatusPane       For handling status-pane changes caused by
        *                          session state change
        * @param aBrandMbmFullPath Full path to branding resource
        */
        static CCAUISessionManager* NewL( CIMPSPresenceConnectionUi& aConnUI,
                                          MCASkinVariant* aSkin  = NULL,
                                          MCAOpBrandVariant* aBrand  = NULL,
                                          MEikCommandObserver* aCommandObserver = NULL,
                                          MCAViewSwitcher* aViewSwitcher  = NULL,
                                          CCAStatusPaneHandler* aStatusPane  = NULL,
                                          TDes* aBrandMbmFullPath = NULL );

        /**
        * Destructor.
        */
        virtual ~CCAUISessionManager();

    private:

        /**
         * C++ default constructor.
         * @param aConnUI		   Connection UI..
         * @param aSkin             Handle to skin variant for Series 60 Platform skin support
         * @param aBrand            Handle to operator branding variant
         *                          (UI branding)
         * @param aCommandObserver  For relaying exit-commands
         * @param aViewSwitcher     For view switching functionality (needed
         *                          to refresh currect view after branding)
         * @param aStatusPane       For handling status-pane changes caused by
         *                          session state change
         * @param aBrandMbmFullPath Full path to branding resource
         */

        CCAUISessionManager( CIMPSPresenceConnectionUi& aConnUI,
                             MCASkinVariant* aSkin,
                             MCAOpBrandVariant* aBrand,
                             MEikCommandObserver* aCommandObserver,
                             MCAViewSwitcher* aViewSwitcher,
                             CCAStatusPaneHandler* aStatusPane,
                             TDes* aBrandMbmFullPath );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // New functions

        /**
        * Is IM logged to network server
        * @return Is IM online
        */
        TBool IsLoggedIn() const;

        /**
         * Command creation n starts Command execution for Login,Logout or Exit
         * @param aLoginParams specifies if it is a login/logout or exit
         */
        TBool LoginL( MCAUiLoginCmdCB::TLoginParams aLoginParams );
        /**
        * Returns is the given feature supported by the session
        * @since 3.2
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
         * @since 3.2
        * @return ETrue if off-line profile is phone current profile
        */
        TBool IsOfflineProfileOn() const;

        /**
         * Displays a list query for selecting a domain ("@domain.com" etc.)
         * implemented in IMPS Connection UI because of the
         * variation.
         * @param aSelectedDomain Here is copied the domain user selected.
         * @return If user cancelled the query return 0,
         *         otherwise the dialog dismiss key code
         * @since 3.2
         */
        TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain );

        /**
         * Checks if background login is running.
         * @return ETrue if background login is running.
         */
        TBool IsBackgroundTaskRunning();

    public: //From Base Class MCAUiLoginCmdCB



        /**
        * Used by the login command to check if it is a login,logout or exit command
        * @param aLoginParams is set with enum value for login or logout or exit
        */
        void GetParameters( MCAUiLoginCmdCB::TLoginParams& aLoginParams );

        /**
        * Updates status pane icon according to session state
        * @param aLandscapeOnly If ETrue, set the icons only in landscape mode
        */
        void SetStatusPaneIconsL( TBool aLandscapeOnly = EFalse );

        /**
        * Changes UI brand for selected logged in server
        * @since 3.2
        * @param aRefreshView Refresh current view after changing brand.
        *                     This closes dialogs so make sure that no dialogs
        *                     are open when this is called with ETrue
        * @param aServerName  If given, then branding will be done based on
                              given server, otherwise current server will be
                              retrieved from connectionUI/SAPSettingsStore
        * return Did branding actually change anything.
        */
        TBool BrandUIL( TBool aRefreshView = ETrue,
                        const TDesC& aServerName = KNullDesC );
        /**
         * @return iCancelLogin value
         */
        TBool CancelLoginValue();

        /**
         * Clears Universal Indication Pane's IM-icon
         */
        void IMUIPIConForcedClear();

        /**
         * Closes all open dialogs on forced logout.
         */
        void ShutDialogsL();

        /**
         * Capture events if needed
         */
        void CaptureEventsL();

        /**
         * Releases events capturing
         */
        void ReleaseCapturingL();

        /**
         * Refresh View After Login
         */
        void RefreshViewAfterLoginL();

        /**
         * Shows wait dialog. Remember to call CleanupStack::Pop and DismissWaitDialogL
         * after this! Call CleanupStack::Pop first. Leaves one item in cleanup stack.
         * @param aTextResource Resource ID of text to be displayed
         * @param aShowImmediately Should the dialog be shown immediately
         */
        void ShowProcessingNoteLC();

        /**
         * DismissProcessingNoteL to dismiss the note
         * @param aError ,show the error if any
         */
        void DismissProcessingNoteL( TInt aError, TBool aValue );
        /**
         * Finalize construction of chat
         * @since 3.2
         */
        void FinalizeChatConstructionL();

        /**
        * Refreshes current view
        * @since 3.2
        * @return ETrue, if refresh view launched. EFalse if not.
        */
        TBool RefreshCurrentViewL();

        /**
         * Sets the ImpsServices on UISessionManager by SessionHandler Cmd
         * @param aImpsServices - the Supported IMPS services
         * @param aBrandUIFlag - if true BrandUIL fn would be called else not
         * @param aRefreshFlag - if true refreshing of the view would be done in BrandUIL() else not
         */
        void SetIMPSServices( TPEngWVCspServicesTree2 aImpsServices,
                              TBool aBrandUIFlag = EFalse, TBool aRefreshFlag = EFalse ) ;
        /**
         * Checkes whether server is defined, if the server is not defined
         * then returns false.
         */

        TBool IsServerDefined();

        /**
         * Shows Post Login Notes like welcome note etc if not always online case
         * @param aCurrentLoginIsFirst is true if the current login is first login
         * @param aManualLogin is true if login is manual else false
         */
        void ShowPostLoginNotesL ( TBool aCurrentLoginIsFirst, TBool aManualLogin ) ;

        /**
        * Notifes observer from SAP access events.
        * (Connection UI is logging into or logging
        * out from certain network server.)
        * @see MIMPSConnProcessUi.
        *
        * @since 3.2
        * @param aServerName The accessed server name.
        * @param aServerURL The accessed server URL.
        * @param aConnProcessUi Connection process UI.
        * If connection process is executed in environment
        * which doesn't have UI, aConnProcessUi is NULL.
        * Else aConnProcessUi is a valid pointer to
        * Connection Process UI interface.
        */
        //Callback from Command Session Handler
        void HandleSapAccessEventEventL( const TDesC& aServerName,
                                         const TDesC& aServerURL,
                                         MIMPSConnProcessUi* aConnProcessUi );


    private:

        /**
         * Checks if there is default server information available, and launches
         * server-query, if not.
         * @param aShowNote If ETrue, show an error note in error situations
         * @return Tells if server information is available after call
         */
        TBool CheckCurrentServerDataL( TBool aShowNote = ETrue );

        /**
         * Check for and show queries to change login type to always
         * online after manual login, according to variation.
         */
        void ShowAlwaysOnlineAfterFirstLoginL();

        /**
         * Gets the current login type
         * @return the login type
         */
        TIMPSLoginType LoginType();

        /**
         * Sets the current login type
         * @param aLoginType tells login type e.g. Manual etc
         */
        void SetLoginTypeL( TWVSettingsChatLoginValues aLoginType );

        /**
         * Asks alias and updates presence, etc.
         */
        void AskAliasL();


        /**
         * Show welcome note, if it's enabled by resource variation
         */
        void ShowWelcomeNoteL();

        /**
         * Show the password queries, if they are enabled by resource
         * variation.
         */
        void ShowPasswordQueriesL();

        /**
         * Show the application launch query, if it's enabled
         * by resource variation.
         */
        void ShowApplicationLaunchQueryL();

        /**
         * Set the current server as default.
         */
        void SetCurrentServerAsDefaultL();

        /**
         * @return ETrue if current server is default
         * @since 3.2
         */
        TBool CurrentServerIsDefaultL();


        /**
         * Shows warning (message query) note about password saving
         * if variatred so.
         * @since S60 v3.1
         */
        void ShowPasswordSavingWarningL() const;

    private:    // Data

        // Doesnt Own. Connection UI
        //Removed the ownership of iConnUI from this file and made it a reference
        //APPUI owns this connection UI instance and shares the same with this component.
        CIMPSPresenceConnectionUi& iConnUI;

        // Negotiated services
        TPEngWVCspServicesTree2 iImpsServices;

        // Command observer
        MEikCommandObserver* iCommandObserver;

        // Used skin
        MCASkinVariant* iSkin;

        // UI branding
        MCAOpBrandVariant* iBrand;

        // Status pane
        CCAStatusPaneHandler* iStatusPane;

        // Handle to mbm-file full path
        TDes* iBrandMbmFullPath;

        // View switching (for refreshing views)
        MCAViewSwitcher* iViewSwitcher;


        // Doesn't own. Profile API  checking if offline profile is on
        MProfileEngine* iProfileApi;

        // should the view be refreshed
        TBool iRefresh;

        // if application is already logged in and we are
        // fetching lists, this is set.
        //for later ref
        //TBool iAlreadyLoggedIn;


        // Network session slot ID
        CPEngNWSessionSlotID2* iSessionSlotID;

        // alias support
        // owns, the alias given by the user (although stored in Settings)
        HBufC*  iAlias;
        TPtr    iAliasDes;      // ptr to iAlias
        TBool   iSupportAlias;  // alias support enabled or not


        // cancelling the alias query
        TBool iCancelLogin;


        // current login is the first one to this server
        // (set by UpdateFirstLoginL)
        TBool iCurrentLoginIsFirst;

        // ETrue if user saved the user id and password
        TBool iUidAndPasswordSaved;


        //Tells if it is a login/logout or exit
        MCAUiLoginCmdCB::TLoginParams iLoginParams;

        // reactive authorization handler, owned
        CCAReactiveAuthorization* iReactiveAuth;


    };

#endif      // CCAUISESSIONMANAGER_H

// End of File

